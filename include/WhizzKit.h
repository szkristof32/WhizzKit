#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <functional>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <string_view>
#include <thread>
#include <utility>

namespace WhizzKit {

#pragma region ArenaAllocator

	struct ArenaAllocatorError
	{
		enum class Type
		{
			None = 0,
			NoArena, OutOfMemory
		};
		Type ErrorType;
		std::string_view AdditionalInfo;
	};

	class ArenaAllocator
	{
	public:
		template<typename _Ty>
		using ArenaAllocatorResult = std::expected<_Ty, ArenaAllocatorError>;
	public:
		ArenaAllocator() = default;
		ArenaAllocator(size_t arenaSize)
			: m_ArenaSize(arenaSize), m_Arena(new std::byte[arenaSize]), m_Pointer(m_Arena)
		{
			if (m_Arena)
				memset(m_Arena, 0, m_ArenaSize);
		}
		~ArenaAllocator()
		{
			delete[] m_Arena;

			m_Arena = m_Pointer = nullptr;
			m_ArenaSize = 0;
		}

		ArenaAllocator(const ArenaAllocator&) = delete;
		ArenaAllocator& operator=(const ArenaAllocator&) = delete;

		ArenaAllocator(ArenaAllocator&& other) noexcept
			: m_ArenaSize(std::exchange(other.m_ArenaSize, 0)), m_Arena(std::exchange(other.m_Arena, nullptr)), m_Pointer(std::exchange(other.m_Pointer, nullptr))
		{
		}
		ArenaAllocator& operator=(ArenaAllocator&& other) noexcept
		{
			std::swap(m_ArenaSize, other.m_ArenaSize);
			std::swap(m_Arena, other.m_Arena);
			std::swap(m_Pointer, other.m_Pointer);

			return *this;
		}

		ArenaAllocatorResult<void*> Allocate(size_t size, size_t alignment = 0)
		{
			if (!m_Arena)
				return std::unexpected(ArenaAllocatorError{ ArenaAllocatorError::Type::NoArena, "" });

			size_t remainingBytes = m_ArenaSize - (size_t)(m_Pointer - m_Arena);
			auto pointer = (void*)m_Pointer;
			const auto alignedAddress = std::align(alignment, size, pointer, remainingBytes);
			if (remainingBytes < size)
				return std::unexpected(ArenaAllocatorError{ ArenaAllocatorError::Type::OutOfMemory, "" });

			m_Pointer = (std::byte*)pointer + size;
			return pointer;
		}
		template<typename _Ty>
		ArenaAllocatorResult<_Ty*> Allocate()
		{
			auto allocation = Allocate(sizeof(_Ty), alignof(_Ty));
			if (!allocation)
				return std::unexpected(allocation.error());

			return reinterpret_cast<_Ty*>(*allocation);
		}
		template<typename _Ty, typename... _Args>
		ArenaAllocatorResult<_Ty*> Emplace(_Args&&... args)
		{
			if (!m_Arena)
				return std::unexpected(ArenaAllocatorError{ ArenaAllocatorError::Type::NoArena, "" });

			const auto allocation = Allocate<_Ty>();
			if (!allocation)
				return std::unexpected(allocation.error());

			return new (*allocation) _Ty{ std::forward<_Args>(args)... };
		}
	private:
		std::byte* m_Arena = nullptr;
		std::byte* m_Pointer = nullptr;
		size_t m_ArenaSize = 0;
	};

#pragma endregion

#pragma region Promise

	template<typename _Ty, typename _Err = bool>
	class Promise
	{
	public:
		enum class Status
		{
			None = 0,
			Pending, Ready, Failed
		};

		using ResolveFn = std::function<void(_Ty)>;
		using RejectFn = std::function<void(_Err)>;
		using PromiseFn = std::function<void(ResolveFn, RejectFn)>;
		using PromiseFnWoutReject = std::function<void(ResolveFn)>;
	public:
		Promise(std::variant<PromiseFn, PromiseFnWoutReject> promiseFn)
			: m_Status(Status::Pending)
		{
			m_WorkerThread = std::thread(std::bind(&Promise::Worker, this, std::placeholders::_1), promiseFn);
		}
		~Promise()
		{
			m_WorkerThread.join();
		}

		std::expected<_Ty, _Err> Await()
		{
			m_Status = Status::Pending;
			WaitForStatusChange(m_Status);

			return m_Result;
		}
		Promise& Then(std::function<void(_Ty)> fulfilled, std::function<void(_Err)> rejected = nullptr)
		{
			std::scoped_lock lock(m_ThenableMutex);
			m_Thenables.push({ fulfilled, rejected });

			return *this;
		}
		Promise& Catch(std::function<void(_Err)> rejected)
		{
			std::scoped_lock lock(m_ThenableMutex);
			m_Thenables.push({ nullptr, rejected });

			return *this;
		}
		void Finally(std::function<void()> finally)
		{
			std::scoped_lock lock(m_ThenableMutex);
			auto thenFn = [&](_Ty value) { finally(); };
			m_Thenables.push({ thenFn, nullptr, true });
		}

		inline Status GetStatus() const { return m_Status; }
		inline operator bool() const { return m_Status == Status::Ready; }

		inline _Ty GetResult() const { return m_Result.value(); }
		inline _Ty GetError() const { return m_Result.error(); }

		inline _Ty operator*() const { return GetResult(); }
		inline _Ty* operator->() const { return m_Result.operator->(); }
	private:
		void WaitForStatusChange(Status status)
		{
			using namespace std::chrono_literals;

			std::this_thread::sleep_for(10ns);

			while (m_Status == status)
			{
				std::this_thread::sleep_for(1ns);
			}
		}
		void Worker(std::variant<PromiseFn, PromiseFnWoutReject> promise)
		{
			WaitForStatusChange(Status::None);

			if (std::holds_alternative<PromiseFn>(promise))
			{
				std::get<PromiseFn>(promise)([&](_Ty result) { m_Result = result; m_Status = Status::Ready; },
					[&](_Err error) { m_Result = std::unexpected(error); m_Status = Status::Failed; });
			}
			else if (std::holds_alternative<PromiseFnWoutReject>(promise))
			{
				std::get<PromiseFnWoutReject>(promise)([&](_Ty result) { m_Result = result; m_Status = Status::Ready; });
			}

			while (!m_Thenables.empty())
			{
				std::scoped_lock lock(m_ThenableMutex);

				auto thenable = m_Thenables.front();
				if (m_Result && thenable.Fulfilled)
					thenable.Fulfilled(m_Result.value());
				else if (thenable.IsFinal)
					thenable.Fulfilled(_Ty{});
				else if (!m_Result && thenable.Rejected)
					thenable.Rejected(m_Result.error());
				m_Thenables.pop();
			}
		}
	private:
		struct Thenable
		{
			using FulfilledFn = std::function<void(_Ty)>;
			using RejectedFn = std::function<void(_Err)>;

			FulfilledFn Fulfilled;
			RejectFn Rejected;

			bool IsFinal = false;
		};
	private:
		std::thread m_WorkerThread;
		std::atomic<Status> m_Status = Status::None;

		std::queue<Thenable> m_Thenables;
		std::mutex m_ThenableMutex;

		std::expected<_Ty, _Err> m_Result;
	};

#pragma endregion

}
