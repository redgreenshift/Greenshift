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


#include "CollectionsAbstract.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max


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
class StaticFifoSet : public ICollectionContract<DataType>
{
private:
	DataType	m_pData[MaximumSize] = {};
	size_t		m_dwHeadIndex = 0; // The index of the oldest element in the ring
	size_t		m_dwCount = 0;     // Current count (0 to MaxSize)

	size_t		m_dwMaxSize = MaximumSize;

public:
	StaticFifoSet() = default;
	virtual ~StaticFifoSet() override { /* EMPTY */ }

	/**
	 * @brief Adds a new value to the set.
	 *
	 * If the set is not full, it is placed in the next available slot in the ring.
	 *
	 * If the set is at capacity (count == MaximumSize), it replaces the current oldest element
	 * and shifts the head index forward to maintain the rolling window.
	 *
	 * @param val - The value to add; ignored if already present.
	 * @returns SUCCESS always, since an existing value is a no-op success.
	 */
	virtual error_t Add(const DataType& val) override
	{
		if (Contains(val))
			return SUCCESS; // nothing to do, do not update the position of the existing item.

		if (m_dwCount < m_dwMaxSize)
		{
			// Case: NOT FULL. Target is the next empty slot in the ring.
			m_pData[(m_dwHeadIndex + m_dwCount) % m_dwMaxSize] = val;
			m_dwCount++;
		}
		else
		{
			// Case: FULL capacity (reached the limit).
			// We replace the current 'oldest' element with the new value.
			m_pData[m_dwHeadIndex] = val;
			// Move head forward so the fresh replacement becomes the newest-relative to its shift.
			m_dwHeadIndex = (m_dwHeadIndex + 1) % m_dwMaxSize;
		}

		return SUCCESS;
	}

	/**
	 * @brief Removes `val` from the set, preserving FIFO order of remaining elements.
	 *
	 * @param val - element value to remove.
	 * @returns SUCCESS if the element was found and removed; ERR_NOTFOUND otherwise.
	 */
	virtual error_t Remove(const DataType& val) override
	{
		// 1. Find the logical index of the element (0 = oldest, m_dwCount-1 = newest)
		for (size_t i = 0; i < m_dwCount; ++i)
		{
			size_t const actualIndex = (m_dwHeadIndex + i) % m_dwMaxSize;
			if (m_pData[actualIndex] == val)
			{
				// 2. Shift all elements after this one one position to the left
				// This preserves the relative FIFO order.
				for (size_t j = i + 1; j < m_dwCount; ++j)
				{
					size_t const current = (m_dwHeadIndex + j) % m_dwMaxSize;
					size_t const previous = (m_dwHeadIndex + j - 1) % m_dwMaxSize;
					m_pData[previous] = m_pData[current];
				}

				// 3. Decrement the count.
				// Note: m_dwHeadIndex does not need to change because the element
				// at i+1 has now shifted into the position of the removed element.
				m_dwCount--;
				return SUCCESS;
			}
		}

		return ERR_NOTFOUND;
	}

	/**
	 * @brief Checks if a value exists within the current rolling window of elements.
	 */
	virtual bool Contains(const DataType& val) const override
	{
		for (size_t i = 0; i < m_dwCount; ++i)
		{
			if (m_pData[(m_dwHeadIndex + i) % m_dwMaxSize] == val)
				return true;
		}
		return false;
	}

	virtual void Clear() override
	{
		m_dwHeadIndex = 0;
		m_dwCount = 0;
	}
	virtual bool Empty() const override { return m_dwCount == 0; }

	/**
	 * Clears all elements and adjusts the maximum capacity to be at most dwSize.
	 */
	void RestrictSize(const size_t dwSize)
	{
		Clear();
		m_dwMaxSize = std::min(dwSize, MaximumSize);
	}

	// --- Getters ---
	virtual size_t Size() const override { return m_dwCount; }
	virtual size_t Capacity() const override { return m_dwMaxSize; }
};

#pragma pop_macro("max")
#pragma pop_macro("min")


