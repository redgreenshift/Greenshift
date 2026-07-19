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

#ifndef _ContiguousAlignedMemory_H_
#define _ContiguousAlignedMemory_H_


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
class ContiguousAlignedMemory
{
public:
	static error_t  Allocate(void** outMemory,
		void*** outPageTable,
		const DWORD nPages,
		const DWORD nPageSize,
		const DWORD nBytesToAlign = 8)
	{
		DWORD   nAlignment_Overhead = nBytesToAlign - 1;
		DWORD   nAligned_Page_Size;
		DWORD   nAligned_Memory_Size;
		DWORD   nPage_Table_Size;
		DWORD   nTotal_Allocation_Size;
		DWORD   index;
		void* pUnalignedBase;
		void* pAlignedBase;
		DWORD   nAlignmentOffset;

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


		nTotal_Allocation_Size = sizeof(DWORD)
			+ nPage_Table_Size
			+ nAlignment_Overhead
			+ nAligned_Memory_Size;

		/*
		 * create the block of memory
		 */
		(*outMemory) = new BYTE[nTotal_Allocation_Size];
		if (*outMemory == NULL)
			return ERR_MALLOC;

		/*
		 * setup the page table
		 */
		*((DWORD*)(*outMemory)) = nPages;

		/*
		 * tell the caller where the page table is
		 */
		*outPageTable = (void**)&((DWORD*)(*outMemory))[1];


		pUnalignedBase = (void*)(&((BYTE*)(*outMemory))[sizeof(DWORD) + nPage_Table_Size]);

		nAlignmentOffset = ((DWORD)pUnalignedBase) % nBytesToAlign;

		if (nAlignmentOffset != 0)
			nAlignmentOffset = nBytesToAlign - nAlignmentOffset;

		//        DumpToFile( "alignlog.txt", nAlignmentOffset, "\n" );

		pAlignedBase = ((BYTE*)pUnalignedBase) + nAlignmentOffset;

		for (index = 0; index < nPages; index++)
			((void**)(&((DWORD*)(*outMemory))[1]))[index] =
			((BYTE*)pAlignedBase) + nAligned_Page_Size * index;

		return SUCCESS;

	};

};


#endif /* _ContiguousAlignedMemory_H_ */

