#pragma once

#include "WhizzKit.h"

#include <cstring>
#include <memory>
#include <utility>

namespace WhizzKit {

	ArenaAllocator::ArenaAllocator(size_t arenaSize)
		: m_ArenaSize(arenaSize), m_Arena(new std::byte[arenaSize]), m_Pointer(m_Arena)
	{
		if (m_Arena)
			memset(m_Arena, 0, m_ArenaSize);
	}

	ArenaAllocator::~ArenaAllocator()
	{
		delete[] m_Arena;

		m_Arena = m_Pointer = nullptr;
		m_ArenaSize = 0;
	}

	ArenaAllocator::ArenaAllocator(ArenaAllocator&& other) noexcept
		: m_ArenaSize(std::exchange(other.m_ArenaSize, 0)), m_Arena(std::exchange(other.m_Arena, nullptr)), m_Pointer(std::exchange(other.m_Pointer, nullptr))
	{
	}

	ArenaAllocator& ArenaAllocator::operator=(ArenaAllocator&& other) noexcept
	{
		std::swap(m_ArenaSize, other.m_ArenaSize);
		std::swap(m_Arena, other.m_Arena);
		std::swap(m_Pointer, other.m_Pointer);

		return *this;
	}

	ArenaAllocator::ArenaAllocatorResult<void*> ArenaAllocator::Allocate(size_t size, size_t alignment)
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

}