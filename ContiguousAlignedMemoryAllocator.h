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

#include <memory>

/**
 * @class ContiguousAlignedMemory
 * @brief A static utility class for allocating a single, contiguous block of memory for multiple pages.
 *
 * This class provides a mechanism to allocate one large, contiguous block that contains:
 * 1. A header (storing the number of pages).
 * 2. A page table (an array of pointers to the start of each page).
 * 3. The requested pages of memory, with each page start being aligned to the specified byte boundary.
 *
 * This is a specialized allocator designed for scenarios where a single large allocation is preferred
 * to reduce memory fragmentation and cache misses, such as when processing large, fixed-size data buffers.
 *
 * @note This is not intended as a general-purpose allocator. Because it uses a single block, the
 *       lifetime of the memory must be managed by the caller (e.g., by deleting the pointer
 *       returned in 'outMemory').
 */
class ContiguousAlignedMemory
{
public:
	/// <summary>
	/// Allocates a single contiguous block that contains a page table and aligned page buffers, initializes the page table, and returns pointers to the block and page table.
	/// </summary>
	/// <param name="outMemory">Pointer to a void* that receives the address of the allocated block (allocated with new byte_t[]). The caller is responsible for freeing the block (delete[]).</param>
	/// <param name="outPageTable">Pointer to a void** that receives the address of the page table (an array of void* entries) located inside the allocated block. Do not free the page table separately.</param>
	/// <param name="nPages">Number of pages to allocate. Must be > 0.</param>
	/// <param name="nPageSize">Requested page size in bytes. Must be > 0. Each page size is rounded up to the specified alignment.</param>
	/// <param name="nBytesToAlign">Alignment in bytes for pages (default 8). Must be > 0. Pages are rounded up to a multiple of this value and the returned page buffers are aligned accordingly.</param>
	/// <returns>An error_t status code: SUCCESS on success; ERR_NULL if an input pointer or size is invalid; ERR_MALLOC if memory allocation fails.</returns>
	static error_t  Allocate(void** outMemory,
		void*** outPageTable,
		const dword_t nPages,
		const dword_t nPageSize,
		const dword_t nBytesToAlign = 8)
	{
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
		if (outMemory == NULL
			|| outPageTable == NULL
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
		if (*outMemory == NULL)
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

		nAlignmentOffset = ((dword_t)pUnalignedBase) % nBytesToAlign;

		if (nAlignmentOffset != 0)
			nAlignmentOffset = nBytesToAlign - nAlignmentOffset;

		//        DumpToFile( "alignlog.txt", nAlignmentOffset, "\n" );

		pAlignedBase = ((byte_t*)pUnalignedBase) + nAlignmentOffset;

		for (index = 0; index < nPages; index++)
			((void**)(&((dword_t*)(*outMemory))[1]))[index] =
			((byte_t*)pAlignedBase) + nAligned_Page_Size * index;

		return SUCCESS;

	};

};

