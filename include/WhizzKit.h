#pragma once

#include <cstdint>
#include <cstddef>
#include <expected>
#include <string_view>

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
		ArenaAllocator(size_t arenaSize);
		~ArenaAllocator();

		ArenaAllocator(const ArenaAllocator&) = delete;
		ArenaAllocator& operator=(const ArenaAllocator&) = delete;

		ArenaAllocator(ArenaAllocator&& other) noexcept;
		ArenaAllocator& operator=(ArenaAllocator&& other) noexcept;

		ArenaAllocatorResult<void*> Allocate(size_t size, size_t alignment = 0);
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


	
}

#ifdef WZ_KIT_HEADER_ONLY
#include "WhizzKit_Impl.h"
#endif
