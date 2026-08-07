#pragma once
#include "Project Greenshift.h"
/*
 *  Copyright (C) 2026 Jared Ivey
 *
 *  This file is part of Project Greenshift
 *
 *  OSI Certified Open Source Software
 *
 *  Project Greenshift is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; version 2 only.
 *
 *  Project Greenshift is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


/**
 * @brief A fixed-capacity, ordered set with FIFO-by-insertion semantics.
 *
 * This container stores unique elements up to a compile-time constant capacity.
 * The logical FIFO order is the sequence of stored elements starting at the oldest
 * index (`m_dwHeadIndex`) and continuing for `m_dwCount` elements.
 *
 * Adding a value that already exists is a no-op; it does not update its position/age.
 *
 * When inserting into a full container, the oldest element is evicted (FIFO) to make room.
 *
 * @tparam DataType     Element type.
 * @tparam MaximumSize  Maximum number of elements (compile-time constant).
 */
template<class DataType, size_t MaximumSize>
class StaticFifoSet
{
private:
	DataType	m_pData[MaximumSize] = {};
	size_t		m_dwHeadIndex = 0; // The index of the oldest element in the ring
	size_t		m_dwCount = 0;     // Current count (0 to MaxSize)

public:
	StaticFifoSet() = default;

	/**
	 * @brief Adds a new value to the set.
	 *
	 * If the set is not full, it is placed in the next available slot in the ring.
	 *
	 * If the set is at capacity (count == MaximumSize), it replaces the current oldest element
	 * and shifts the head index forward to maintain the rolling window.
	 */
	void Add(const DataType& val)
	{
		if (Contains(val))
			return; // nothing to do, do not update the position of the existing item.

		if (m_dwCount < MaximumSize)
		{
			// Case: NOT FULL. Target is the next empty slot in the ring.
			m_pData[(m_dwHeadIndex + m_dwCount) % MaximumSize] = val;
			m_dwCount++;
		}
		else
		{
			// Case: FULL capacity (reached the limit).
			// We replace the current 'oldest' element with the new value.
			m_pData[m_dwHeadIndex] = val;
			// Move head forward so the fresh replacement becomes the newest-relative to its shift.
			m_dwHeadIndex = (m_dwHeadIndex + 1) % MaximumSize;
		}
	};

	/**
	 * @brief Checks if a value exists within the current rolling window of elements.
	 */
	bool Contains(const DataType& val) const
	{
		for (unsigned int i = 0; i < m_dwCount; ++i)
		{
			if (m_pData[(m_dwHeadIndex + i) % MaximumSize] == val)
				return true;
		}
		return false;
	};
	bool Includes(const DataType& val) const
	{
		return Contains(val);
	};


	// --- Getters ---
	size_t Size() const { return m_dwCount; }
	size_t Capacity() const { return MaximumSize; }
};

