#pragma once
#include "Project Greenshift.h"
/*
 *  Copyright (C) 2001-2026 Jared Ivey
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

#include <cstdarg>
#include <cstddef>
#include <memory>
#include <utility>

 /****************************************************************************
  *
  * ContiguousAlignedMemory - allocates multiple chunks of memory contiguously and aligns to specified number of bytes
  *
  ****************************************************************************/

  /****************************************************************************
   *
   * ContinuousAlignedMemory - not intended as a general purpose mem allocator
   *
   * for most things, use malloc or new, etc...
   *
   * However, when HUGE blocks of memory are needed for processing data where
   * maximum speed is desired, having those blocks in contiguous memory may
   * help reduce fragmentation of those chunks, and decrease cache misses.
   *
   * One thing is required (or at least highly suggested) that the allocated
   * memory will not need to be resized, and will be around for a long time.
   *
   *
   * Also, some things like SSE instructions (a.k.a. MMX2) require memory
   * access to be 16 byte aligned.
   *
   ****************************************************************************/

//

/****************************************************************************
 * @brief Specifies alignment requirements for contiguous memory allocations.
 *
 * Alignment is primarily used to ensure that allocated memory can be
 * accessed efficiently by SIMD instructions and cache-friendly algorithms.
 *
 * The enum values correspond to commonly used SIMD register widths:
 *
 *     Bytes8   - Typically sufficient for MMX-era code.
 *     Bytes16  - Typically sufficient for SSE/SSE2/SSE3/SSE4.x.
 *     Bytes32  - Typically sufficient for AVX/AVX2.
 *     Bytes64  - Typically sufficient for AVX-512 and aligns to a
 *                typical modern CPU cache line size.
 *
 * Notes:
 *
 * - Larger alignments generally increase allocation overhead slightly
 *   but simplify future SIMD experimentation.
 *
 * - Alignment guarantees that the start of each allocated page is aligned
 *   to the requested boundary.
 *
 * - Choosing a larger alignment than currently required is typically
 *   safe and may avoid future allocator changes when experimenting with
 *   wider SIMD instruction sets.
 *
 * - Alignment does not guarantee improved performance by itself; the
 *   benefit depends on the access patterns and instructions used.
 ****************************************************************************/
enum class MemoryAlignment : std::size_t {
	Standard = alignof(std::max_align_t), // Alignment for ordinary C++ types with fundamental alignment requirements
	None = 1,     // No alignment beyond what new[] naturally provides.
	Bytes8 = 8,   // commonly sufficient for MMX
	Bytes16 = 16, // commonly sufficient for SSE
	Bytes32 = 32, // commonly sufficient for AVX
	Bytes64 = 64, // commonly sufficient for AVX-512
};


/**
 * @class ContiguousAlignedMemory
 *
 * @brief Owns multiple aligned memory pages stored inside a
 *        single contiguous allocation.
 *
 * This class is intended for large, long-lived, fixed-size data
 * structures where predictable memory layout, cache locality,
 * reduced heap fragmentation, and SIMD-friendly alignment are
 * desired.
 *
 * Every page is individually aligned to the requested byte
 * boundary, allowing the storage to be used safely with SIMD
 * instructions that require aligned access.
 *
 * The page table provides random access to each page while the
 * underlying storage remains contained within a single
 * allocation.
 *
 * This is not intended as a general-purpose allocator.
 */
class ContiguousAlignedMemory
{
public:
	ContiguousAlignedMemory() = default;
	~ContiguousAlignedMemory() = default;

	ContiguousAlignedMemory(ContiguousAlignedMemory&&) noexcept = default;
	ContiguousAlignedMemory& operator=(ContiguousAlignedMemory&&) noexcept = default;

	ContiguousAlignedMemory(const ContiguousAlignedMemory&) = delete;
	ContiguousAlignedMemory& operator=(const ContiguousAlignedMemory&) = delete;

	/// <summary>
	/// Allocates a single contiguous block that contains a page table and aligned page buffers, initializes the page table, and returns pointers to the block and page table.
	/// </summary>
	/// <param name="nPages">Number of pages to allocate. Must be > 0.</param>
	/// <param name="nPageSize">Requested page size in bytes. Must be > 0. Each page size is rounded up to the specified alignment.</param>
	/// <param name="alignment">Alignment in bytes for pages (default "Ordinary C++ type alignment"). Must be > 0. Pages are rounded up to a multiple of this value and the returned page buffers are aligned accordingly.</param>
	/// <returns>An error_t status code: SUCCESS on success; ERR_NULL if an input pointer or size is invalid; ERR_MALLOC if memory allocation fails.</returns>
	[[nodiscard]]
	error_t Allocate(
		dword_t nPages,
		dword_t nPageSize,
		MemoryAlignment alignment = MemoryAlignment::Standard)
	{
		void* outMemory = nullptr;

		Reset();
		error_t err = Allocate(
			&outMemory,
			&m_pageTable,
			nPages,
			nPageSize,
			alignment);

		if (err != SUCCESS)
			return err;

		m_upAllocation.reset(static_cast<byte_t*>(outMemory));
		m_pAligned = reinterpret_cast<byte_t*>(m_pageTable);
		m_pageCount = nPages;

		return err;
	}

	// Returns true if storage is currently allocated.
	bool IsAllocated() const
	{
		return m_upAllocation != nullptr;
	}

	explicit operator bool() const
	{
		return IsAllocated();
	}

	// Returns the aligned start of the contiguous storage.
	byte_t* data()
	{
		return m_pAligned;
	}

	// Returns the page table contained within the allocation.
	void** PageTable()
	{
		return m_pageTable;
	}

	// Returns the specified page from the page table.
	const void* Page(size_t index) const
	{
		return m_pageTable[index];
	}

	// Returns the specified page from the page table.
	void* Page(size_t index)
	{
		return m_pageTable[index];
	}

	size_t PageCount() const
	{
		return m_pageCount;
	}

	// Releases the allocation and returns the object
	// to its default-constructed state.
	void Reset()
	{
		// Invalidate all externally visible state.
		m_pAligned = nullptr;
		m_pageTable = nullptr;
		m_pageCount = 0;

		// Release the backing allocation.
		m_upAllocation.reset();
	}

private:
	std::unique_ptr<byte_t[]> m_upAllocation;
	byte_t* m_pAligned = nullptr;
	void** m_pageTable = nullptr;
	dword_t m_pageCount = 0;

	/// <summary>
	/// Allocates a single contiguous block that contains a page table and aligned page buffers, initializes the page table, and returns pointers to the block and page table.
	/// </summary>
	/// <param name="outMemory">Pointer to a void* that receives the address of the allocated block (allocated with new byte_t[]). The caller is responsible for freeing the block (delete[]).</param>
	/// <param name="outPageTable">Pointer to a void** that receives the address of the page table (an array of void* entries) located inside the allocated block. Do not free the page table separately.</param>
	/// <param name="nPages">Number of pages to allocate. Must be > 0.</param>
	/// <param name="nPageSize">Requested page size in bytes. Must be > 0. Each page size is rounded up to the specified alignment.</param>
	/// <param name="alignment">Alignment in bytes for pages. Must be > 0. Pages are rounded up to a multiple of this value and the returned page buffers are aligned accordingly.</param>
	/// <returns>An error_t status code: SUCCESS on success; ERR_NULL if an input pointer or size is invalid; ERR_MALLOC if memory allocation fails.</returns>
	static error_t  Allocate(void** outMemory,
		void*** outPageTable,
		const dword_t nPages,
		const dword_t nPageSize,
		const MemoryAlignment alignment)
	{
		dword_t   nBytesToAlign = std::to_underlying(alignment);
		dword_t   nAlignment_Overhead = nBytesToAlign - 1;
		dword_t   nAligned_Page_Size;
		dword_t   nAligned_Memory_Size;
		dword_t   nPage_Table_Size;
		dword_t   nTotal_Allocation_Size;
		dword_t   index;
		void* pUnalignedBase;
		void* pAlignedBase;
		dword_t   nAlignmentOffset;

		/*
		 *
		 */
		if (outMemory == nullptr
			|| outPageTable == nullptr
			|| nBytesToAlign == 0
			|| nPages == 0
			|| nPageSize == 0)
			return ERR_NULL;

		nAligned_Page_Size = nPageSize - (nPageSize % nBytesToAlign);

		if (nAligned_Page_Size != nPageSize)
			nAligned_Page_Size += nBytesToAlign;

		nAligned_Memory_Size = nPages * nAligned_Page_Size;

		nPage_Table_Size = nPages * sizeof(void*);


		nTotal_Allocation_Size = sizeof(dword_t)
			+ nPage_Table_Size
			+ nAlignment_Overhead
			+ nAligned_Memory_Size;

		/*
		 * create the block of memory
		 */
		(*outMemory) = new byte_t[nTotal_Allocation_Size];
		if (*outMemory == nullptr)
			return ERR_MALLOC;

		/*
		 * setup the page table
		 */
		*((dword_t*)(*outMemory)) = nPages;

		/*
		 * tell the caller where the page table is
		 */
		*outPageTable = (void**)&((dword_t*)(*outMemory))[1];


		pUnalignedBase = (void*)(&((byte_t*)(*outMemory))[sizeof(dword_t) + nPage_Table_Size]);

		nAlignmentOffset = ((uintptr_t)pUnalignedBase) % nBytesToAlign;

		if (nAlignmentOffset != 0)
			nAlignmentOffset = nBytesToAlign - nAlignmentOffset;

		//DumpToFile( "alignlog.txt", nAlignmentOffset, "\n" );

		pAlignedBase = ((byte_t*)pUnalignedBase) + nAlignmentOffset;

		// Each page start is also aligned.
		for (index = 0; index < nPages; index++)
		{
			((void**)(&((dword_t*)(*outMemory))[1]))[index] =
				((byte_t*)pAlignedBase) + nAligned_Page_Size * index;
		}

		return SUCCESS;

	};

};

// If I ever want a new name:
using ContiguousStorage = ContiguousAlignedMemory;

