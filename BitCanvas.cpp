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
  * BitCanvas
  *
  ****************************************************************************/

//ESP reference instructions are larger than EBP references.

#include "BitCanvas.h"

#include "adetect.h"
#include "ColorUtils.hpp"
#include "ContiguousAlignedMemoryAllocator.h"
#include "DeltaField.h"
#include "DeltaTween.h"
#include "MyDictionary.h"
#include "Expression.h"
#include "MathUtils.hpp"
#include "MemoryUtils.hpp"
#include "Palette.h"
#include "TweenThread.h"

  //I don't think I should include math just for fabs()
#include <cstdlib>
#include <emmintrin.h> // SSE2 intrinsics
//#include <mm3dnow.h>
//#include <emmintrin.h>
//#include <xmmintrin.h>
#ifdef USE_MMX_INTRINSICS
#include <mmintrin.h>
#endif
//#include <dvec.h>
//#include <fvec.h>
//#include <ivec.h>

//#include <Regstr.h>
//#include <Sqlole.h>
//#include <Wininet.h>

//#include <complex>
//_INFCODE
//_NANCODE

/*
 * extra size should be multiple of DWORD so generic copy last line
 * can use 32 bit memory access to move the data for all bit depths.
 */

 /*
  * error checking is lacking.
  *
  * PREFETCH1() prefetches memory to the cache

 16-byte alignment is required for most SSE instructions. If VectorC incorrectly assumes 16-byte alignment, you will get alignment exceptions. Also, unaligned memory accesses are significantly slower than aligned accesses
  */



  /*
   * Use CreateDibSection to create a bitmap whose pixels are directly addressable
   * from a program but which also has an HBITMAP handle for GDI use.
   */


   /*
	* HACK to get OpenGL to work
	*/
void* BitCanvas::GetPixelMemory(dword_t* nWidth, dword_t* nHeight, dword_t* nPixelFormat)
{

	*nWidth = BufferWidth();
	*nHeight = BufferHeight();
	*nPixelFormat = BitDepth() / BITS_PER_BYTE;

	return m_pWriteOnlyBuffer;
}


/****************************************************************************
 *
 * New - creates a new BitCanvas with the specified arguments
 *
 * pre: *outBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    BitCanvas::New( //const dword_t dwWidth,
	//const dword_t dwHeight,
	//const dword_t dwBitDepth,
	BitCanvas** outBitCanvas,
	MyDictionary<mychar_t*>* inConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals,
	MyDictionary<value_t*>* inValues)
{
	error_t  err = SUCCESS;
	dword_t  dwWidth;
	dword_t  dwHeight;
	dword_t  dwBitDepth;
	dword_t  dwByteDepth;
	dword_t  dwLCM;
	dword_t  dwDWORDPitch;
	PIXELMAPDESCRIPTION pmd{};
	TWEENDESCRIPTION    tween_description{};
	value_t nWidth, nHeight, nBitDepth;
	value_t* pValue;

	if (outBitCanvas == NULL || inConfig == NULL)
		return ERR_NULL;

	if (inValues != NULL)
	{
		inValues->SetValue("canvas_width", &nWidth);
		inValues->SetValue("canvas_height", &nHeight);
		inValues->SetValue("canvas_depth", &nBitDepth);
	}

	nWidth = 640;
	nHeight = 380;
	nBitDepth = 8;

	//    dwWidth    = LCLIP( atol( inConfig->GetValue( "visible_width",  "640" )) );
	//    dwHeight   = LCLIP( atol( inConfig->GetValue( "visible_height", "360" )) );
	//    dwBitDepth = LCLIP( atol( inConfig->GetValue( "calculated_depth", "8" )) );

	dwBitDepth = (dword_t)Expression::Evaluate(inConfig->GetValue("canvas_depth"), nBitDepth, inValues, inGlobals);
	nBitDepth = (value_t)dwBitDepth;

	dwWidth = (dword_t)Expression::Evaluate(inConfig->GetValue("canvas_width"), nWidth, inValues, inGlobals);
	nWidth = (value_t)dwWidth;
	if (inValues != NULL)
	{
		pValue = inValues->GetValue("buffer_width", &nWidth);
		if (*pValue < nWidth)
		{
			dwWidth = (dword_t)*pValue;
			nWidth = (value_t)dwWidth;
		}
	}

	dwHeight = (dword_t)Expression::Evaluate(inConfig->GetValue("canvas_height"), nHeight, inValues, inGlobals);
	dwHeight -= dwHeight % 2; /* make the height even */
	nHeight = (value_t)dwHeight;
	if (inValues != NULL)
	{
		pValue = inValues->GetValue("buffer_height", &nHeight);
		if (*pValue < nHeight)
		{
			dwHeight = (dword_t)*pValue;
			dwHeight -= dwHeight % 2; /* make the height even */
			nHeight = (value_t)dwHeight;
		}
	}


	if (inValues != NULL)
	{
		inValues->RemoveValue("canvas_depth");
		inValues->RemoveValue("canvas_height");
		inValues->RemoveValue("canvas_width");
	}

	if (dwBitDepth == 24)
		dwBitDepth = 32;

	if (dwBitDepth <= 8)
	{
		dwBitDepth = 8;
		dwByteDepth = 1;
		dwLCM = 4;
	}
	else
		if (dwBitDepth <= 16)
		{
			dwBitDepth = 16;
			dwByteDepth = 2;
			dwLCM = 2;
		}
		else
		{
			dwBitDepth = 32;
			dwByteDepth = 4;
			dwLCM = 1;
		}


	dwWidth = dwWidth - (dwWidth % dwLCM);

	dwDWORDPitch = dwWidth * dwByteDepth / sizeof(dword_t);


	/*
	 * visible_width * byte_depth MUST be a multiple of 4
	 */

	tween_description.dwFrameSize = dwWidth * dwHeight * sizeof(PIXELMAP);
	pmd.dwWidth = dwWidth;
	pmd.dwHeight = dwHeight;

	tween_description.dwMaximumFrames
		= LCLIP(atol(inConfig->GetValue("maximum_frames", "16")));

	tween_description.dwDefaultFrames
		= LCLIP(atol(inConfig->GetValue("default_frames", "8")));

	tween_description.dwDefaultFrames
		= min(tween_description.dwDefaultFrames,
			tween_description.dwMaximumFrames);

	tween_description.dwTweenFrameRepeatValue
		= atol(inConfig->GetValue("tween_repeat", "4"));

	tween_description.dwPlainFrameRepeatValue
		= atol(inConfig->GetValue("plain_repeat", "1"));

	tween_description.pData = &pmd;

	/*
	 * create the BitCanvas based on the requested bit depth
	 */
	switch (dwBitDepth)
	{
	case 8:
		*outBitCanvas = new BitCanvas8(dwWidth, dwHeight, tween_description);
		break;

	case 16:
		*outBitCanvas = new BitCanvas16(dwWidth, dwHeight, tween_description);
		break;

	case 24:
	case 32:
		*outBitCanvas = new BitCanvas32(dwWidth, dwHeight, tween_description);
		break;

	default: /* error: invalid bit depth */
		*outBitCanvas = NULL;
	}

	if (*outBitCanvas == NULL)
		return ERR_BITCANVAS;


	/*
	 * initialize the BitCanvas
	 */
	if ((err = (*outBitCanvas)->Initialize()) == SUCCESS)
		return SUCCESS;
	else
	{
		delete* outBitCanvas;
		return err;
	}

}

bool cpu_has_sse2()
{
#if defined(_M_X64) || defined(_M_AMD64)
	return true;  // SSE2 is guaranteed on x64
#else
	int regs[4];
	__cpuid(regs, 1);  // CPUID leaf 1
	return (regs[3] & (1 << 26)) != 0;  // EDX bit 26 = SSE2
#endif
}

/****************************************************************************
 *
 * BitCanvas - constructor
	 *
	 * ok, I admit that m_nCacheLength equation needs some explaining
	 *
	 * since the variable is a constant, I have to put everything on one line
	 * but I'll break it down in this comment so it's easier to understand
	 *
	 * I want to set the number of dwords to cache
	 *
	 * first the number of pixels I want to cache is
	 *    pixels_to_cache   =  width + 1
	 *
	 * simple right?  That's just the first line plus one more pixel
	 *
	 * next it might be nice to know how many bytes are required for a pixel
	 *  bytes_per_pixel   = bits_per_pixel / bits_per_byte
	 *
	 * next I need to know how many bytes that will be
	 *  bytes_to_cache    = pixels_to_cache * bytes_per_pixel
	 *
	 * Now that I know how many bytes I want to cache, all that's left
	 * is to determine the number of DWORDs necessary to store those bytes
	 *  dwords_to_cache   = (bytes_to_cache - 1) / bytes_per_dword + 1
	 *
 ****************************************************************************/
BitCanvas::BitCanvas(const dword_t dwWidth,
	const dword_t dwHeight,
	const dword_t dwBitDepth,
	const TWEENDESCRIPTION& tween_description) :
	m_dwBitDepth(dwBitDepth),
	m_dwBufferWidth(dwWidth),
	m_dwBufferHeight(dwHeight),
	m_dwHalfWidth(dwWidth >> 1),
	m_dwHalfHeight(dwHeight >> 1),
	m_hTransitionTable(tween_description),
	m_dwBufferByteLength(dwWidth* dwHeight* dwBitDepth / BITS_PER_BYTE),
	m_dwBufferLength(dwWidth* dwHeight),
	m_dwCacheLength(((dwWidth + 1)* dwBitDepth / BITS_PER_BYTE - 1) / sizeof(dword_t) + 1),
	m_nLineWidthFactor(dwWidth / 640.0f)
{
	/*
	 * initialize all members
	 */
	m_pDelta = NULL;
	m_pUnalignedDoubleBuffer = NULL;

	m_pReadOnlyBuffer = NULL; /* aligned buffer */
	m_pWriteOnlyBuffer = NULL; /* aligned buffer */

	//    m_pPaletteEntry           = PGetPalette()->palPalEntry; /* so I don't have to keep dereferencing */

		/*
		 * MMX is only *actually* used for 32 bit color.
		 * So, the fastest C++ only routines for 8 and 16 are "x86"
		 */
	if (m_dwBitDepth == 32)
		m_nDeltaOptimizationLevel = cpu_has_sse2() ? OL_SSE2 : (GetCPUCaps(HAS_MMX) ? OL_MMX : OL_CPP); // OL_x86 has MMX intrinsics
	else
		m_nDeltaOptimizationLevel = OL_x86;


	SetDrawingAspect(0.0f, false);

	m_visibleX.SetBounds(0.0f, (value_t)(BufferWidth() - 1));
	m_visibleY.SetBounds(0.0f, (value_t)(BufferHeight() - 1));
	//    m_visibleX.SetInterval( 0, min(m_pBitCanvas->BufferWidth(), m_pBitCanvas->BufferHeight()) );
	//    m_visibleY.SetInterval( 0, min(m_pBitCanvas->BufferWidth(), m_pBitCanvas->BufferHeight()) );

		/*
		 * why is this right again?  I need to verify its correctness.
		 *
		 * It's wrong, it should be based on SCREEN dimensions and aspect
		 */
	m_logicalX.SetBounds(-(value_t)BufferWidth() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()),
		(value_t)BufferWidth() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()));
	m_logicalY.SetBounds(-(value_t)BufferHeight() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()),
		(value_t)BufferHeight() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()));


#ifdef USE_BITMAP
	m_hDC = NULL;

	/*
	biHeight
	Specifies the height of the bitmap, in pixels. If biHeight is positive,
	the bitmap is a bottom-up DIB and its origin is the lower-left corner.
	If biHeight is negative, the bitmap is a top-down DIB and its origin is
	the upper-left corner.

	If biHeight is negative, indicating a top-down DIB, biCompression must
	be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be compressed.

	Windows 98, Windows 2000: If biCompression is BI_JPEG or BI_PNG, the
	biHeight member specifies the height of the decompressed JPEG or PNG
	image file, respectively.


	 */


#ifdef USE_DOUBLE_BITMAP
	m_hDoubleBitmap = NULL;

	ZeroMemory(&m_hDoubleBitmapInfo, sizeof(m_hDoubleBitmapInfo));
	m_hDoubleBitmapInfo.bmiHeader.biSize = sizeof(m_hDoubleBitmapInfo.bmiHeader);
	m_hDoubleBitmapInfo.bmiHeader.biWidth = BufferWidth();
#ifdef INVERTED_BITMAP
	m_hDoubleBitmapInfo.bmiHeader.biHeight = ((long)BufferHeight() + 2) * 2;
#else
	m_hDoubleBitmapInfo.bmiHeader.biHeight = -((long)BufferHeight() + 2) * 2;
#endif
	m_hDoubleBitmapInfo.bmiHeader.biPlanes = 1;
	m_hDoubleBitmapInfo.bmiHeader.biBitCount = (WORD)BitDepth();
	m_hDoubleBitmapInfo.bmiHeader.biCompression = BI_RGB;
	//    m_hDoubleBitmapInfo.bmiHeader.biCompression   = BI_BITFIELDS;
	m_hDoubleBitmapInfo.bmiHeader.biSizeImage = 0;
	m_hDoubleBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	m_hDoubleBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	m_hDoubleBitmapInfo.bmiHeader.biClrUsed = 0;
	m_hDoubleBitmapInfo.bmiHeader.biClrImportant = 0;


	/*
		if( BitDepth() == 16 )
		{
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[0] = 0x0000F800;
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[1] = 0x000007E0;
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[2] = 0x0000001F;
		}
		else
		if( BitDepth() == 32 )
		{
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[0] = 0x00FF0000;
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[1] = 0x0000FF00;
			((DWORD*)m_hDoubleBitmapInfo.bmiColors)[2] = 0x000000FF;
		}/**/

#else // not Double Bitmap

	m_hReadOnlyBitmap = NULL;
	m_hWriteOnlyBitmap = NULL;


	ZeroMemory(&m_hReadOnlyBitmapInfo, sizeof(m_hReadOnlyBitmapInfo));
	m_hReadOnlyBitmapInfo.bmiHeader.biSize = sizeof(m_hReadOnlyBitmapInfo.bmiHeader);
	m_hReadOnlyBitmapInfo.bmiHeader.biWidth = BufferWidth();
#ifdef TOP_DOWN_DIB
	m_hReadOnlyBitmapInfo.bmiHeader.biHeight = ((long)BufferHeight() + 2);
#else
	m_hReadOnlyBitmapInfo.bmiHeader.biHeight = -((long)BufferHeight() + 2);
#endif
	m_hReadOnlyBitmapInfo.bmiHeader.biPlanes = 1;
	m_hReadOnlyBitmapInfo.bmiHeader.biBitCount = (WORD)BitDepth();
	m_hReadOnlyBitmapInfo.bmiHeader.biCompression = BI_RGB;
	//    m_hReadOnlyBitmapInfo.bmiHeader.biCompression   = BI_BITFIELDS;
	m_hReadOnlyBitmapInfo.bmiHeader.biSizeImage = 0;
	m_hReadOnlyBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	m_hReadOnlyBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	m_hReadOnlyBitmapInfo.bmiHeader.biClrUsed = 0;
	m_hReadOnlyBitmapInfo.bmiHeader.biClrImportant = 0;

	ZeroMemory(&m_hWriteOnlyBitmapInfo, sizeof(m_hWriteOnlyBitmapInfo));
	m_hWriteOnlyBitmapInfo.bmiHeader.biSize = sizeof(m_hWriteOnlyBitmapInfo.bmiHeader);
	m_hWriteOnlyBitmapInfo.bmiHeader.biWidth = BufferWidth();
#ifdef TOP_DOWN_DIB
	m_hWriteOnlyBitmapInfo.bmiHeader.biHeight = ((long)BufferHeight() + 2);
#else
	m_hWriteOnlyBitmapInfo.bmiHeader.biHeight = -((long)BufferHeight() + 2);
#endif
	m_hWriteOnlyBitmapInfo.bmiHeader.biPlanes = 1;
	m_hWriteOnlyBitmapInfo.bmiHeader.biBitCount = (WORD)BitDepth();
	m_hWriteOnlyBitmapInfo.bmiHeader.biCompression = BI_RGB;
	//    m_hWriteOnlyBitmapInfo.bmiHeader.biCompression   = BI_BITFIELDS;
	m_hWriteOnlyBitmapInfo.bmiHeader.biSizeImage = 0;
	m_hWriteOnlyBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	m_hWriteOnlyBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	m_hWriteOnlyBitmapInfo.bmiHeader.biClrUsed = 0;
	m_hWriteOnlyBitmapInfo.bmiHeader.biClrImportant = 0;

	/*    if( BitDepth() == 16 )
		{
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[0] = 0x0000F800;
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[1] = 0x000007E0;
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[2] = 0x0000001F;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[0] = 0x0000F800;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[1] = 0x000007E0;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[2] = 0x0000001F;
		}
		else
		if( BitDepth() == 32 )
		{
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[0] = 0x00FF0000;
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[1] = 0x0000FF00;
			((DWORD*)m_hReadOnlyBitmapInfo.bmiColors)[2] = 0x000000FF;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[0] = 0x00FF0000;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[1] = 0x0000FF00;
			((DWORD*)m_hWriteOnlyBitmapInfo.bmiColors)[2] = 0x000000FF;
		}/**/
#endif // Double Bitmap

#ifdef UNDEFINED
	Windows 95 / 98: When the biCompression member is BI_BITFIELDS, the system
		supports only the following 16bpp color masks : A 5 - 5 - 5 16 - bit image, where the
		blue mask is 0x001F, the green mask is 0x03E0, and the red mask is 0x7C00;
	and a 5 - 6 - 5 16 - bit image, where the
		blue mask is 0x001F, the green mask is 0x07E0, and the red mask is 0xF800
#endif

#endif

} /* BitCanvas() */


/****************************************************************************
 *
 * ~BitCanvas
 *
 ****************************************************************************/
BitCanvas::~BitCanvas()
{
	m_hTransitionTable.TerminateThread();
	ReleaseBuffers();
} /* ~BitCanvas() */


/****************************************************************************
 *
 * ReleaseBuffers
 *
 ****************************************************************************/
void BitCanvas::ReleaseBuffers(void)
{
	m_pReadOnlyBuffer = NULL; /* aligned buffer */
	m_pWriteOnlyBuffer = NULL; /* aligned buffer */

#ifdef USE_BITMAP
	if (m_hDC != NULL)
	{
		DeleteDC(m_hDC);
		m_hDC = NULL;
	}
#ifdef USE_DOUBLE_BITMAP
	if (m_hDoubleBitmap != NULL)
	{
		DeleteObject(m_hDoubleBitmap);
		m_hDoubleBitmap = NULL;
	}
#else // Double Bitmap
	if (m_hReadOnlyBitmap != NULL)
	{
		DeleteObject(m_hReadOnlyBitmap);
		m_hReadOnlyBitmap = NULL;
	}
	if (m_hWriteOnlyBitmap != NULL)
	{
		DeleteObject(m_hWriteOnlyBitmap);
		m_hWriteOnlyBitmap = NULL;
	}
#endif // Double Bitmap
#else
	SAFE_DELETE_ARRAY(m_pUnalignedDoubleBuffer);
#endif
}


/****************************************************************************
 *
 * SetOptimizationLevel
 *
 ****************************************************************************/
void BitCanvas::SetOptimizationLevel(const opt_level_t enumOptLevel)
{
	m_nDeltaOptimizationLevel = enumOptLevel;
}


/****************************************************************************
 *
 * Initialize
 *
 ****************************************************************************/
error_t BitCanvas::Initialize(void)
{
	const    dword_t        nBytes_to_Align = 16;
	const    dword_t        nBytes_per_Pixel = BitDepth() / BITS_PER_BYTE;
	const    dword_t        nBuffer_Length = BufferWidth() * BufferHeight();
	const    dword_t        nCache_Size = m_dwCacheLength * sizeof(dword_t);
	const    dword_t        nBuffer_Size = nBuffer_Length * nBytes_per_Pixel
		+ nCache_Size;

#ifdef USE_BITMAP
	UINT    uiFlags = (BitDepth() == 8) ? DIB_PAL_COLORS : DIB_RGB_COLORS;
	//    uiFlags = DIB_RGB_COLORS;
	m_hDC = CreateCompatibleDC(NULL);
	//    DIB_PAL_COLORS
#ifdef USE_DOUBLE_BITMAP
	m_hDoubleBitmap = CreateDIBSection(m_hDC, &m_hDoubleBitmapInfo, uiFlags, (void**)&m_pWriteOnlyBuffer, NULL, 0);
	if (m_hDoubleBitmap == NULL)
		return ERR_MALLOC;

	m_pReadOnlyBuffer = &m_pReadBuffer8[BufferWidth() * (BufferHeight() + 2) * BitDepth() / BITS_PER_BYTE];

#else // Double Bitmap

	m_hWriteOnlyBitmap = CreateDIBSection(m_hDC, &m_hWriteOnlyBitmapInfo, uiFlags, (void**)&m_pWriteOnlyBuffer, NULL, 0);
	if (m_hWriteOnlyBitmap == NULL || m_pWriteOnlyBuffer == NULL)
		return ERR_NULL;

	m_hReadOnlyBitmap = CreateDIBSection(m_hDC, &m_hReadOnlyBitmapInfo, uiFlags, (void**)&m_pReadOnlyBuffer, NULL, 0);
	if (m_hReadOnlyBitmap == NULL || m_pReadOnlyBuffer == NULL)
		return ERR_MALLOC;
#endif // Double Bitmap

#else  // Use Bitmap

	error_t err;
	void** pPageTable;

	err = ContiguousAlignedMemory::Allocate(&m_pUnalignedDoubleBuffer,
		&pPageTable,
		2,
		nBuffer_Size,
		nBytes_to_Align);
	if (err != SUCCESS)
		return err;

	m_pReadOnlyBuffer = pPageTable[0];
	m_pWriteOnlyBuffer = pPageTable[1];

	// Be sure to initialize the extra line, the cache past the end that contains a COPY of the first line of pixels!
	memset(m_pReadOnlyBuffer, 0, nBuffer_Size);
	memset(m_pWriteOnlyBuffer, 0, nBuffer_Size);

#endif

	return SUCCESS;
}



/****************************************************************************
 *
 * FlipBuffers
 *
 ****************************************************************************/
void BitCanvas::FlipBuffers(void)
{
	void* tmpBuf;

	tmpBuf = m_pReadOnlyBuffer;
	m_pReadOnlyBuffer = m_pWriteOnlyBuffer;
	m_pWriteOnlyBuffer = tmpBuf;

#ifdef USE_BITMAP
#ifndef USE_DOUBLE_BITMAP
	tmpBuf = m_hReadOnlyBitmap;
	m_hReadOnlyBitmap = m_hWriteOnlyBitmap;
	m_hWriteOnlyBitmap = (HBITMAP)tmpBuf;
#endif
#endif

	//    this->Changed( 'Flip' );
}



/**
 * @brief Interpolates between two drawing aspect ratios.
 *
 * This version of SetDrawingAspect calculates an intermediate aspect ratio
 * based on the percentage provided, and chooses between two zoom modes.
 *
 * @param nAspect  - The first aspect ratio (if 0.0f, defaults to buffer aspect).
 * @param bZoom    - The zoom mode to use if nPercent < 0.5f.
 * @param nAspect2 - The second aspect ratio (if 0.0f, defaults to buffer aspect).
 * @param bZoom2   - The zoom mode to use if nPercent >= 0.5f.
 * @param nPercent - The weight (0.0 to 1.0) used to interpolate between nAspect and nAspect2.
 */
void BitCanvas::SetDrawingAspect(value_t nAspect, const bool bZoom, const value_t nAspect2, const bool bZoom2, const value_t nPercent)
{
	const value_t        nWidth = (value_t)BufferWidth();
	const value_t        nHeight = (value_t)BufferHeight();
	const value_t        nBufferAspect = nWidth / nHeight;
	const value_t nAs = nAspect == 0.0f ? nBufferAspect : nAspect;
	const value_t nAs2 = nAspect2 == 0.0f ? nBufferAspect : nAspect2;

	SetDrawingAspect(nAs + (nAs2 - nAs) * nPercent, nPercent < 0.5f ? bZoom : bZoom2);
}

/**
 * @brief Sets the drawing aspect ratio and scaling basis.
 *
 * This function sets the aspect ratio for drawing. The `bZoom` flag determines
 * whether the scaling dimension (used to normalize aspect-ratio-adjusted width)
 * is based on the maximum or minimum dimension of the buffer.
 *
 * @param nAspect - The aspect ratio to use (if 0.0f, defaults to the buffer's native aspect ratio).
 * @param bZoom   - If true, uses the maximum buffer dimension as the scaling basis;
 *                  if false, uses the minimum buffer dimension.
 */
void BitCanvas::SetDrawingAspect(value_t nAspect, const bool bZoom)
{
	const value_t        nWidth = (value_t)BufferWidth();
	const value_t        nHeight = (value_t)BufferHeight();
	// It is the dimension used to normalize the aspect-ratio-adjusted width.
	const value_t        nScaleDimention = (value_t)(bZoom ? (max(BufferWidth(), BufferHeight())) : (min(BufferWidth(), BufferHeight())));

	// nAspect is a term with units   width/height
	// so to compare screenwidth and screen height, you have to convert the units to match
	// screenwidth/aspect == screenheight


	if (nAspect == 0.0f)
		nAspect = nWidth / nHeight;


	/* Jared's way            */   /*     Andy's way            */
	if (bZoom ? (nAspect > nWidth / nHeight) : (nAspect <= nWidth / nHeight))
	{
		m_nWidthFactor = 0.5f * nHeight * nAspect;
		m_nHeightFactor = 0.5f * nHeight;
	}
	else
	{
		m_nWidthFactor = 0.5f * nWidth;
		m_nHeightFactor = 0.5f * nWidth / nAspect;
	}



	/*
		m_visibleX.SetBounds( 0.0f, (value_t)(BufferWidth()  - 1) );
		m_visibleY.SetBounds( 0.0f, (value_t)(BufferHeight() - 1) );
	//    m_visibleX.SetInterval( 0, min(m_pBitCanvas->BufferWidth(), m_pBitCanvas->BufferHeight()) );
	//    m_visibleY.SetInterval( 0, min(m_pBitCanvas->BufferWidth(), m_pBitCanvas->BufferHeight()) );

	//    /*
	//     * why is this right again?  I need to verify its correctness.
	//     *
	//     * It's wrong, it should be based on SCREEN dimensions and aspect
	//     * /
		m_logicalX.SetBounds( -(value_t)BufferWidth()  * 1.0f / (value_t)min(BufferWidth(), BufferHeight()),
							   (value_t)BufferWidth()  * 1.0f / (value_t)min(BufferWidth(), BufferHeight()) );
		m_logicalY.SetBounds( -(value_t)BufferHeight() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()),
							   (value_t)BufferHeight() * 1.0f / (value_t)min(BufferWidth(), BufferHeight()) );
	/**/
	m_logicalX.SetBounds(-(value_t)BufferWidth() / nAspect / nScaleDimention,
		(value_t)BufferWidth() / nAspect / nScaleDimention);
	m_logicalY.SetBounds(-(value_t)BufferHeight() / nAspect / nScaleDimention,
		(value_t)BufferHeight() / nAspect / nScaleDimention);
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * CopyLastLine - generic copy last line
 *
 ****************************************************************************/
void BitCanvas::CopyLastLine(void)
{
	BYTE* srcBuffer;
	BYTE* dstBuffer;
	DWORD i;
	const DWORD lnStop = (m_nBufferWidth + 1) * m_nBitDepth / BITS_PER_BYTE;

	srcBuffer = m_pWriteBuffer8;
	dstBuffer = m_pWriteBuffer8 +
		m_nBufferWidth * m_nBufferHeight * BitDepth() / BITS_PER_BYTE;
	for (i = 0; i < lnStop; i++)
		*dstBuffer++ = *srcBuffer++;
}
#endif


#ifdef UNDEFINED
/****************************************************************************
 *
 * CopyLastLine - generic copy last line
 *
 ****************************************************************************/
void BitCanvas::CopyLastLine(void)
{
	/*    static const DWORD nBufferLength = m_nBufferWidth * m_nBufferHeight * BitDepth() / BITS_PER_BYTE;
		DWORD *srcBuffer;
		DWORD *dstBuffer;
		DWORD i;

		srcBuffer = (DWORD*)m_pWriteBuffer8;
		dstBuffer = (DWORD*)(m_pWriteBuffer8 + nBufferLength);
		for( i = m_nCacheLength; i != 0; i-- )
			*dstBuffer++ = *srcBuffer++;

		/**/

	DWORD* srcBuffer = (DWORD*)m_pWriteBuffer8;
	DWORD* dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_nBufferByteLength);
	DWORD i;

	for (i = m_nCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;
}
#endif


#ifdef USE_BITMAP
/****************************************************************************
 *
 * CopyTo - BitBlit'er... why won't it work correctly?
 *
 ****************************************************************************/
void BitCanvas::CopyTo(const DWORD nWidth,
	const DWORD nHeight,
	HDC hDC)
{
	HPALETTE    hPalette = CreatePalette(PGetPalette());
	SelectObject(m_hDC, m_hReadOnlyBitmap);  /* SHOULD CHECK FOR ERROR */
	//    SelectObject( m_hDC, m_hWriteOnlyBitmap );  /* SHOULD CHECK FOR ERROR */
	SelectPalette(m_hDC, hPalette, FALSE);
	RealizePalette(m_hDC);
	BitBlt(hDC, (nWidth - BufferWidth()) / 2, (nHeight - BufferHeight()) / 2, BufferWidth(), BufferHeight(), m_hDC, 0, 0, SRCCOPY);
	DeleteObject(hPalette);
}
#endif

/****************************************************************************
 *
 * CopyTo - copy format router
 *
 ****************************************************************************/
void BitCanvas::CopyTo(void* lpSurface,
	const dword_t nWidth,
	const dword_t nHeight,
	const dword_t nPitch,
	const dword_t nPixelFormat)
{
	const    dword_t    nPixelWidth = nPitch / (nPixelFormat + 1);
	const    dword_t    nDeadSpace = nPixelWidth - m_dwBufferWidth;
	const    dword_t    nStartOffset = (
		nPixelWidth * (nHeight - m_dwBufferHeight) +
		(nWidth - m_dwBufferWidth)
		) >> 1;
	void* lpSurfaceStart = ((byte_t*)lpSurface) +
		nStartOffset * (nPixelFormat + 1);


#ifndef UNDEFINED
	if (BitDepth() == (nPixelFormat + 1) * BITS_PER_BYTE)
	{
		this->CopyToSameBitdepth(lpSurfaceStart, nPitch);
	}
	else
#endif

		switch (nPixelFormat)
		{
		case 0:
			this->CopyTo8(lpSurfaceStart, nDeadSpace);
			break;
		case 1:
			this->CopyTo16(lpSurfaceStart, nDeadSpace);
			break;
		case 2:
			this->CopyTo24(lpSurfaceStart, nDeadSpace);
			break;
		case 3:
			this->CopyTo32(lpSurfaceStart, nDeadSpace);
			break;
		default: /* what should default do? */
			break;
		}
}


/****************************************************************************
 *
 * SetDelta -
 *
 ****************************************************************************/
error_t    BitCanvas::SetDelta(DeltaField* pDelta)
{
	error_t err;

	/*
	 * thread should not be busy if m_pDelta is NULL, but I'll check first just in case
	 */
	if (m_hTransitionTable.IsBusy())
		return ERR_THREADBUSY;

	if (pDelta == NULL)
		return ERR_NULL;
	//        DumpToFile( "error.txt", ErrorString(ERR_NULL), "\n" );

	if (m_pDelta == NULL)
		err = m_hTransitionTable.Initialize(pDelta);
	else
		err = m_hTransitionTable.ScheduleTween(0, m_pDelta, pDelta); /* MUST CHANGE THE HARDCODED CONSTANT!
																		* 0 uses default
																		*/


	if (err == SUCCESS)
		m_pDelta = pDelta;

	return err;
}


/****************************************************************************
 *
 * DoDelta - delta field optimization router
 *
 ****************************************************************************/
void BitCanvas::DoDelta(const PIXELMAP* lpTransitionTable)
{
	SynchronizeReadBufferCache();

	switch (m_nDeltaOptimizationLevel)
	{
	case OL_CPP:
		this->DoDelta_cpp(lpTransitionTable);
		break;
	case OL_x86:
		this->DoDelta_x86(lpTransitionTable);
		break;
	case OL_MMX:
		this->DoDelta_MMX(lpTransitionTable);
		break;
	case OL_SSE2:
		this->DoDelta_SSE(lpTransitionTable);
		break;
	default:  /* what should default do? */
		break;
	}

	SynchronizeWriteBufferCache();
}

/****************************************************************************
 *
 * DoDelta_x86 - default, do CPP
 *
 ****************************************************************************/
void BitCanvas::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	this->DoDelta_cpp(lpTransitionTable);
}


/****************************************************************************
 *
 * DoDelta_MMX - default, do x86
 *
 ****************************************************************************/
void BitCanvas::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	this->DoDelta_x86(lpTransitionTable);
}


/****************************************************************************
 *
 * DoDelta_SSE - default, do MMX
 *
 ****************************************************************************/
void BitCanvas::DoDelta_SSE(const PIXELMAP* lpTransitionTable)
{
	this->DoDelta_MMX(lpTransitionTable);
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * PlotLineBresenham -
 *
 ****************************************************************************/
void    BitCanvas::PlotLineBresenham(
	long x0,
	long y0,
	const long x1,
	long y1,
	const BYTE color)
{
	long dy = y1 - y0;
	long dx = x1 - x0;
	long stepx, stepy;

	if (dy < 0L) { dy = -dy;  stepy = -1L; }
	else { stepy = 1L; }
	if (dx < 0L) { dx = -dx;  stepx = -1L; }
	else { stepx = 1L; }
	dy <<= 1;                                                  // dy is now 2*dy
	dx <<= 1;                                                  // dx is now 2*dx

	PutPixel(x0, y0, color);
	if (dx > dy) {
		long fraction = dy - (dx >> 1);                         // same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0L) {
				y0 += stepy;
				fraction -= dx;                                // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;                                    // same as fraction -= 2*dy
			PutPixel(x0, y0, color);
		}
	}
	else {
		long fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0L) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			PutPixel(x0, y0, color);
		}
	}
}
#endif

/**
 * @brief Draws a line segment using Bresenham's line-drawing algorithm.
 *
 * Plots a line from (x0, y0) to (x1, y1) using integer-only arithmetic to
 * determine the closest pixel coordinates, ensuring high performance by
 * avoiding floating-point operations.
 *
 * This implementation supports all slopes (horizontal, vertical, and diagonal).
 * For optimization, it converts 2D coordinates into a 1D linear buffer offset
 * to minimize multiplication operations within the drawing loop. Coordinates
 * falling outside the canvas boundaries are clipped.
 *
 * @param[in] x0    - The starting X-coordinate.
 * @param[in] y0    - The starting Y-coordinate.
 * @param[in] x1    - The ending X-coordinate.
 * @param[in] y1    - The ending Y-coordinate.
 * @param[in] color - The color value used to draw the line.
 *
 * @note Vertical positions are handled as absolute buffer offsets to
 *       minimize multiplication; the drawable area is constrained by
 *       BufferWidth() and BufferHeight().
 */
void BitCanvas::PlotLineBresenham(
	long x0,
	long y0,
	const long x1,
	long y1,
	const byte_t color)
{
	static const long buffer_width = BufferWidth();
	static const long buffer_length = buffer_width * (BufferHeight() - 1);
	long dy = y1 - y0;
	long dx = x1 - x0;
	long stepx, stepy;

	if (dy < 0L) { dy = -dy;  stepy = -buffer_width; }
	else { stepy = buffer_width; }
	if (dx < 0L) { dx = -dx;  stepx = -1L; }
	else { stepx = 1L; }
	dy <<= 1;
	dx <<= 1;

	y0 *= buffer_width;
	y1 *= buffer_width;
	if (y0 >= 0L && x0 >= 0L && y0 < buffer_length && x0 < buffer_width)
		PutPixel(x0 + y0, color);
	if (dx > dy) {
		long fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0L) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			if (y0 >= 0L && x0 >= 0L && y0 < buffer_length && x0 < buffer_width)
				PutPixel(x0 + y0, color);
		}
	}
	else {
		long fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0L) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			if (y0 >= 0L && x0 >= 0L && y0 < buffer_length && x0 < buffer_width)
				PutPixel(x0 + y0, color);
		}
	}
}



/**
 * @brief Plots a thick line using the Bresenham algorithm.
 *
 * This function implements a thick-line version of the Bresenham line-drawing
 * algorithm, allowing for a specified line width. It calculates the line
 * path and applies a brush-like thickness or offset to create a thicker stroke.
 *
 * @param x0 Starting - x-coordinate of the line.
 * @param y0 Starting - y-coordinate of the line.
 * @param x1 Ending   - x-coordinate of the line.
 * @param y1 Ending   - y-coordinate of the line.
 * @param line_width  - The thickness (width) of the line in pixels.
 * @param color       - The palette index of the color to use for drawing the line.
 *
 * @note Vertical positions are handled as absolute buffer offsets to
 *       minimize multiplication; the drawable area is constrained by
 *       BufferWidth() and BufferHeight().
 */
void BitCanvas::PlotLineBresenhamThick(
	long x0,
	long y0,
	long x1,
	long y1,
	const long line_width,
	const byte_t color)
{
	static const long buffer_width = BufferWidth();
	static const long buffer_length = buffer_width * (BufferHeight() - 1);
	const long half_width = line_width >> 1;
	long dy = y1 - y0;
	long dx = x1 - x0;
	long stepx, stepy;
	long lw;

	if (dy < 0L) { dy = -dy;  stepy = -buffer_width; }
	else { stepy = buffer_width; }
	if (dx < 0L) { dx = -dx;  stepx = -1L; }
	else { stepx = 1L; }
	dy <<= 1;
	dx <<= 1;

	y0 *= buffer_width;
	y1 *= buffer_width;
	if (dx > dy) { /* slope < 1... loop across x */
		long xi = x0;
		long yi = y0;
		long xf = x1;
		long yf = y1;

		for (lw = -half_width; lw < half_width; lw++)
		{
			long fraction = dy - (dx >> 1);
			x0 = xi;
			y0 = yi + lw * buffer_width;
			x1 = xf;
			y1 = yf + lw * buffer_width;
			while (x0 != x1) {
				if (fraction >= 0L) {
					y0 += stepy;
					fraction -= dx;
				}
				x0 += stepx;
				fraction += dy;
				if (y0 >= 0L && x0 >= 0L && y0 < buffer_length && x0 < buffer_width)
					PutPixel(x0 + y0, color);
			}
		}

	}
	else { /* slope >= 1... loop across y */
		long base;
		long offset;
		long fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0L) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
#ifdef UNDEFINED
			if (y0 >= 0L && x0 >= 0L + half_width && y0 < buffer_length && x0 < buffer_width - half_width)
				//            PutPixel( x0+y0, color );
				PutPixel((DWORD)(x0 + y0 - half_width), (WORD)line_width, (BYTE)color);
#else
			if (y0 >= 0L && y0 < buffer_length && x0 > -half_width && x0 < buffer_width + half_width)
				for (base = x0 + y0, offset = -half_width; offset <= half_width; offset++)
				{
					if (x0 + offset > 0 && x0 + offset < buffer_width)
						PutPixel(base + offset, color);
				}

#endif
		}
	}
}


//#define USE_INTERVAL
#ifdef USE_INTERVAL
/* version from before using aspect */



/****************************************************************************
 *
 * DrawLine -
 *
 ****************************************************************************/
void    BitCanvas::DrawLine(const value_t x1,
	const value_t y1,
	const value_t x2,
	const value_t y2,
	const value_t line_width,
	const value_t color)
{
	const value_t   fX1 = m_visibleX[m_logicalX.Normalize(x1)];
	const value_t   fY1 = m_visibleY[m_logicalY.Normalize(y1)];
	const value_t   fX2 = m_visibleX[m_logicalX.Normalize(x2)];
	const value_t   fY2 = m_visibleY[m_logicalY.Normalize(y2)];

	const long   lX1 = (long)(fX1 + 0.5f);
	const long   lY1 = (long)(fY1 + 0.5f);
	const long   lX2 = (long)(fX2 + 0.5f);
	const long   lY2 = (long)(fY2 + 0.5f);


	const long   change_in_x = lX2 - lX1;
	const long   change_in_y = lY2 - lY1;

	const long half_width = ((long)line_width) >> 1;
	const long start_width = -half_width - (((long)line_width) & 1);

	long lWidth;

	BYTE q;

	/*
	 * Don't try to draw lines that don't even enter the screen
	 */
	if ((lY1 < 0L && lY2 < 0L)
		|| (lX1 < 0L && lX2 < 0L)
		|| (lY1 > (long)BufferHeight() && lY2 > (long)BufferHeight())
		|| (lX1 > (long)BufferWidth() && lX2 > (long)BufferWidth())
		)
		return;


	if (color >= 1.0f)
		q = 255;
	else if (color <= 0.0f)
		q = 0;
	else
		q = (BYTE)(color * 255.0f + 0.5f);

#ifndef FORCE_LINE_WIDTH_TO_1
	if (line_width >= 1.5f)
	{
		PlotDot(fX1, fY1, line_width, color);
		//        PlotDot( fX2, fY2, line_width, color );
#ifdef UNDEFINED
		if (labs(change_in_y) > labs(change_in_x))
			for (lWidth = start_width; lWidth < half_width; lWidth++)
				PlotLineBresenham(lX1 + lWidth, lY1, lX2 + lWidth, lY2, q);
		else
			for (lWidth = start_width; lWidth < half_width; lWidth++)
				PlotLineBresenham(lX1, lY1 + lWidth, lX2, lY2 + lWidth, q);
#else
		PlotLineBresenhamThick(lX1, lY1, lX2, lY2, (long)(line_width + 0.5f), q);
#endif
	}
	else
#endif  /* FORCE_LINE_WIDTH_TO_1 */
		PlotLineBresenham(lX1, lY1, lX2, lY2, q);

}



/****************************************************************************
 *
 * DrawDot - need to optimize this to go a lot faster... or draw lines with something else
 *
 ****************************************************************************/
void    BitCanvas::DrawDot(const value_t x,
	const value_t y,
	const value_t dot_size,
	const value_t color)
{
	this->PlotDot(m_visibleX[m_logicalX.Normalize(x)],
		m_visibleY[m_logicalY.Normalize(y)],
		dot_size * m_nLineWidthFactor,
		color);
}

#else // USE_INTERVAL
/****************************************************************************
 *
 * DrawLine -
 *
 ****************************************************************************/
void    BitCanvas::DrawLine(const value_t x1,
	const value_t y1,
	const value_t x2,
	const value_t y2,
	const value_t line_width_parameter,
	const value_t color)
{
	const value_t   fX1 = m_dwHalfWidth + x1 * m_nWidthFactor;
	const value_t   fY1 = m_dwHalfHeight + y1 * m_nHeightFactor;
	const value_t   fX2 = m_dwHalfWidth + x2 * m_nWidthFactor;
	const value_t   fY2 = m_dwHalfHeight + y2 * m_nHeightFactor;

	const long   lX1 = (long)(fX1 + 0.5f);
	const long   lY1 = (long)(fY1 + 0.5f);
	const long   lX2 = (long)(fX2 + 0.5f);
	const long   lY2 = (long)(fY2 + 0.5f);


	//const long   change_in_x = lX2 - lX1;
	//const long   change_in_y = lY2 - lY1;

	const value_t   line_width = line_width_parameter * m_nLineWidthFactor;
	//const long half_width = ((long)line_width) >> 1;
	//const long start_width = -half_width - (((long)line_width) & 1);

	//    long lWidth;

	byte_t q;

	/*
	 * Don't try to draw lines that don't even enter the screen
	 */
	if ((lY1 < 0L && lY2 < 0L)
		|| (lX1 < 0L && lX2 < 0L)
		|| (lY1 > (long)BufferHeight() && lY2 > (long)BufferHeight())
		|| (lX1 > (long)BufferWidth() && lX2 > (long)BufferWidth())
		)
		return;


	if (color >= 1.0f)
		q = 255;
	else if (color <= 0.0f)
		q = 0;
	else
		q = (byte_t)(color * 255.0f + 0.5f);

#ifndef FORCE_LINE_WIDTH_TO_1
	if (line_width >= 1.5f)
	{
		PlotDot(fX1, fY1, line_width, color);
		//        PlotDot( fX2, fY2, line_width, color );
#ifdef UNDEFINED
		if (labs(change_in_y) > labs(change_in_x))
			for (lWidth = start_width; lWidth < half_width; lWidth++)
				PlotLineBresenham(lX1 + lWidth, lY1, lX2 + lWidth, lY2, q);
		else
			for (lWidth = start_width; lWidth < half_width; lWidth++)
				PlotLineBresenham(lX1, lY1 + lWidth, lX2, lY2 + lWidth, q);
#else
		PlotLineBresenhamThick(lX1, lY1, lX2, lY2, (long)(line_width + 0.5f), q);
#endif
	}
	else
#endif  /* FORCE_LINE_WIDTH_TO_1 */
		PlotLineBresenham(lX1, lY1, lX2, lY2, q);

}



/****************************************************************************
 *
 * DrawDot - need to optimize this to go a lot faster... or draw lines with something else
 *
 ****************************************************************************/
void    BitCanvas::DrawDot(const value_t x,
	const value_t y,
	const value_t dot_size,
	const value_t color)
{
	this->PlotDot(m_dwHalfWidth + x * m_nWidthFactor,
		m_dwHalfHeight + y * m_nHeightFactor,
		dot_size * m_nLineWidthFactor,
		color);
}

#endif // USE_INTERVAL

#ifndef UNDEFINED
/****************************************************************************
 *
 * PlotDot - need to reduce the confusing junk code
 *
 ****************************************************************************/
void    BitCanvas::PlotDot(const value_t x,
	const value_t y,
	const value_t dot_size,
	const value_t color)
{
	const long  lX = (long)(x + 0.5f);
	const long  lY = (long)(y + 0.5f);
	const long  dot_diameter = (long)(dot_size);
	const long  dot_odd = dot_diameter & 1L;
	const long  dot_radius = (dot_diameter >> 1) + dot_odd;
	const long  dot_radius_squared = dot_radius * (dot_radius + 1L - dot_odd);

	long    lpy;
	long    lpy_squared;
	long    ipx;
	long    slice_width;

	byte_t    q;

	if (color >= 1.0f)
		q = 255;
	else if (color <= 0.0f)
		q = 0;
	else
		q = (byte_t)(color * 255.0f + 0.5f);


#ifndef FORCE_LINE_WIDTH_TO_1
	/*
	 * if the rounded dot size would be at least 2, draw it as a circle,
	 * else just draw the single pixel
	 */
	if (dot_diameter > 1L)
	{
		for (lpy = -dot_radius + dot_odd; lpy < dot_radius; lpy++)
		{
			lpy_squared = (lpy + ((dot_odd == 0L && lpy > 0L) ? 1L : 0L));
			lpy_squared *= lpy_squared;

			LONG_SQRT_X2(dot_radius_squared - lpy_squared, slice_width);

			if (dot_diameter == 6L && slice_width == 3L)
			{
				ipx = 2L;
				slice_width++;
			}
			else
				if (dot_diameter == 3L && lpy != 0L)
				{
					ipx = 0L;
					slice_width = 1L;
				}
				else
				{
					ipx = (slice_width - dot_odd) >> 1;
					slice_width = (ipx << 1) + dot_odd; /* this looks idiotic, but that's what makes it work */
				}

			this->PutPixel((lX - ipx), (lY + lpy), (word_t)slice_width, q);
		}
	}
	else
#endif  /* FORCE_LINE_WIDTH_TO_1 */
		/*
		 * should it not draw if the size is negative?
		 */
		this->PutPixel(lX, lY, q);

}
#endif

#ifdef UNDEFINED
/****************************************************************************
 *
 * PlotDot - need to optimize this to go a lot faster... or draw lines with something else
 *
 ****************************************************************************/
void	BitCanvas::PlotDot(const value_t x,
	const value_t y,
	const value_t dot_size,
	const value_t color)
{
	//	const WORD	dot_diameter = (WORD)floor(dot_size + 0.5f);
	const value_t	radius = dot_size / 2.0f;
	const value_t	radiusSquared = radius * radius;

	const	long yStart = (long)ceil(-radius);
	const	long yEnd = (long)ceil(radius);
	value_t	dx;
	long	lpy;
	long	ipx;
	//	long	ipy;

	BYTE	q;

	if (color >= 1.0f)
		q = 255;
	else if (color <= 0.0f)
		q = 0;
	else
		q = (BYTE)(color * 255.0f + 0.5f);


	/*
	 * if the rounded dot size would be at least 2, draw it as a circle,
	 * else just draw the single pixel
	 */
	if (dot_size >= 1.5f)
	{
		for (lpy = -(long)radius; lpy < radius; lpy++)
		{
			dx = (value_t)sqrt(radiusSquared - lpy * lpy);

			ipx = (long)floor(dx);

			this->PutPixel(
				(long)(x - ipx),
				(long)(y - lpy),
				(WORD)(ipx << 1),
				q);
		}
	}
	else
		/*
		 * should it not draw if the size is negative?
		 */
		this->PutPixel((long)floor(x), (long)floor(y), q);
}
#endif

/****************************************************************************
 *
 * SetPalette -
 *
 ****************************************************************************/
error_t    BitCanvas::SetPalette(Palette* pPalette)
{
	return m_pPalette.Initialize(pPalette);
}


/****************************************************************************
 *
 * SetPalette -
 *
 ****************************************************************************/
error_t    BitCanvas::SetPalette(Palette* pPalette1, Palette* pPalette2, const value_t nPercent)
{
	return m_pPalette.Initialize(pPalette1, pPalette2, nPercent);
}



















/****************************************************************************
 ****************************************************************************
 *
 * BitCanvas8
 *
 ****************************************************************************
 ****************************************************************************/

 /****************************************************************************
  *
  * BitCanvas8
  *
  ****************************************************************************/
BitCanvas8::BitCanvas8(const dword_t dwWidth, const dword_t dwHeight,
	const TWEENDESCRIPTION& td)
	: BitCanvas(dwWidth, dwHeight, 8, td)
{
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas8::PutPixel(const dword_t pixelOffset, const byte_t color)
{
	m_pWriteBuffer8[pixelOffset] = color;
}

/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas8::PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color)
{
	word_t    i;

	for (i = 0; i < pixel_width; i++)
		m_pWriteBuffer8[pixelOffset + i] = color;
}



/****************************************************************************
 *
 * GetPixel
 *
 ****************************************************************************/
dword_t BitCanvas8::GetPixel(const dword_t pixelOffset)
{
	return m_pReadBuffer8[pixelOffset];
}



// Don't need OKLab interpolation because it's COLOR INDEXES not color values in a color space.... maybe consider using OKLab for 16/32bit color???

/*
Function	Read Method				Math Approach				Loop Structure
DoDelta_cpp	4 separate BYTE reads	(p1+p2+p3+p4) DECAY8		Nested for (H × W)
DoDelta_x86	2 WORD reads			Bitmasking -> DECAY8		Single do-while
DoDelta_MMX	2 WORD reads			Bitmasking -> decay256[]	Single do-while
*/

// DISPATCH:

void BitCanvas8::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	DoDelta_cpp_BASELINE(lpTransitionTable);
}

void BitCanvas8::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	DoDelta_cpp_OPTIMIZED(lpTransitionTable);
}

void BitCanvas8::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	DoDelta_cpp_experiment1(lpTransitionTable);
}

void BitCanvas8::DoDelta_SSE(const PIXELMAP* lpTransitionTable)
{
	DoDelta_SSE2_experiment1(lpTransitionTable);
}

// IMPLEMENTATIONS:

/****************************************************************************
 * @brief This is a baseline/REFERENCE implementation of the straightforward
 * algorithm for 8bit color.
 *
 * I expect to use one of the more optimized implementations.
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_cpp_BASELINE(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  width;
	dword_t  height;
	dword_t  pixel, pixel3;
	byte_t  p1, p2, p3, p4;

	for (height = 0; height < m_dwBufferHeight; height++)
	{
		for (width = 0; width < m_dwBufferWidth; width++)
		{
#ifndef NO_ANTIALIASING
			pixel = lpTransitionTable[pixelOffset];
			pixel3 = pixel + m_dwBufferWidth;
			p1 = m_pReadBuffer8[pixel];
			p2 = m_pReadBuffer8[pixel + 1];
			p3 = m_pReadBuffer8[pixel3];
			p4 = m_pReadBuffer8[pixel3 + 1];

			m_pWriteBuffer8[pixelOffset] = (p1 + p2 + p3 + p4) DECAY8;
#else
			m_pWriteBuffer8[pixelOffset] = m_pReadBuffer8[lpTransitionTable[pixelOffset]];
#endif

			pixelOffset++;
		}
	}
}

/****************************************************************************
 *
 * @brief DEFAULT for 8bit color
 *
 * I tried coding x86 assembly by hand, but I found that the compiler is much
 * better then me at assembly optimization, but I did optimize this code by
 * looking at the disassembly, identified points that could be faster, and
 * then tweaked code in the corresponding area to influence the generated code.
 *
 * Of course, measuring performance and only keeping tweaks that actually had a benefit.
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_cpp_OPTIMIZED(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  p1, p2;
	dword_t  pixel, pixel3;
	byte_t*  pReadBuffer = m_pReadBuffer8;
	byte_t*  pWriteBuffer = m_pWriteBuffer8;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth; // faster than using local, becaus it's already CONST
		pixel = *(word_t*)(&(pReadBuffer[pixel]));
		pixel3 = *(word_t*)(&(pReadBuffer[pixel3]));

		p1 = (pixel & 0x00FF) + (pixel3 & 0x00FF);

		p2 = (pixel & 0xFF00) + (pixel3 & 0xFF00);

		/* decay the buffer (ie: make it slightly darker than the average) */
		p1 = (p1 + (p2 >> 8)) DECAY8;

		//m_pWriteBuffer8[pixelOffset] = (BYTE)p1;
		pWriteBuffer[pixelOffset] = (byte_t)p1;

#if 0
#if EXTREME_DEBUGGING
		/* DEBUG CODE, REMOVE IMMEDIATELY */
		if (p1 > 255)
			DumpToFile("error.txt", p1, "\n");
		//pWriteBuffer[pixelOffset] = (pixelOffset & 0x1) ? 255 : 0;
#endif
#endif

		pixelOffset++;
	} while (pixelOffset < m_dwBufferByteLength);
}

/****************************************************************************
 *
 * @brief EXPERIMENTAL, place to explore potential optimizations;
 * NOT actually tied to MMX intrinsics/assembly
 *
 * cpp_OPTIMIZED and experiment1 are nearly identical — both use packed WORD loads.
 * The divergence is in the final decay step: cpp_OPTIMIZED keeps the same DECAY8 formula
 * while "experiment1" swaps it for a simpler >> 2 shift plus a saturation table lookup
 * that avoids overflow issues without branches.
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_cpp_experiment1(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  p1, p2;
	dword_t  pixel, pixel3;
	byte_t*  pReadBuffer = m_pReadBuffer8;
	byte_t*  pWriteBuffer = m_pWriteBuffer8;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth; // faster than using local, because it's already CONST
		pixel = *(word_t*)(&(pReadBuffer[pixel]));
		pixel3 = *(word_t*)(&(pReadBuffer[pixel3]));

		p1 = (pixel & 0x00FF) + (pixel3 & 0x00FF);

		p2 = (pixel & 0xFF00) + (pixel3 & 0xFF00);

		//p1 = (p1 + (p2 >> 8)) DECAY8;
		p1 = (p1 + (p2 >> 8)) >> 2;

#if 0
#if EXTREME_DEBUGGING
		/* DEBUG CODE, REMOVE IMMEDIATELY */
		if (p1 > 255)
			DumpToFile("error.txt", p1, "\n");
		//pWriteBuffer[pixelOffset] = (pixelOffset & 0x1) ? 255 : 0;
#endif
#endif

		/* decay the buffer (ie: make it slightly darker than the average) */
		//m_pWriteBuffer8[pixelOffset] = (BYTE)p1;
		pWriteBuffer[pixelOffset] = decay256[p1];

		pixelOffset++;
	} while (pixelOffset < m_dwBufferByteLength);
}

void BitCanvas8::DoDelta_SSE2_experiment1(const PIXELMAP* lpTransitionTable)
{
	DoDelta_cpp_OPTIMIZED(lpTransitionTable);
}

/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas8::CopyTo8(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	byte_t* lpBuffer = m_pReadBuffer8;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = *lpBuffer++;
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo16
 *
 ****************************************************************************/
void    BitCanvas8::CopyTo16(void* lpSurface, const dword_t nDeadSpace)
{
	word_t* lpSurface2 = (word_t*)lpSurface;
	byte_t* lpBuffer = m_pReadBuffer8;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = RGB16(
								r32to16(GetRed(*lpBuffer)),
								g32to16(GetGreen(*lpBuffer)),
								b32to16(GetBlue(*lpBuffer++))
								);
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo24
 *
 ****************************************************************************/
void    BitCanvas8::CopyTo24(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	byte_t* lpBuffer = m_pReadBuffer8;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = GetRed(*lpBuffer);
			*lpSurface2++ = GetGreen(*lpBuffer);
			*lpSurface2++ = GetBlue(*lpBuffer++);
		}

		//skip over the cache area
		lpSurface2 += 3 * nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo32
 *
 ****************************************************************************/
void    BitCanvas8::CopyTo32(void* lpSurface, const dword_t nDeadSpace)
{
	dword_t* lpSurface2 = (dword_t*)lpSurface;
	byte_t*  lpBuffer = m_pReadBuffer8;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = RGB32(GetRed(*lpBuffer), GetGreen(*lpBuffer), GetBlue(*lpBuffer++));
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}













/****************************************************************************
 ****************************************************************************
 *
 * BitCanvas16
 *
 ****************************************************************************
 ****************************************************************************/

 /****************************************************************************
  *
  * BitCanvas16
  *
  ****************************************************************************/
BitCanvas16::BitCanvas16(const dword_t dwWidth, const dword_t dwHeight,
	const TWEENDESCRIPTION& td)
	: BitCanvas(dwWidth, dwHeight, 16, td)
{
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas16::PutPixel(const dword_t pixelOffset, const byte_t color)
{
	m_pWriteBuffer16[pixelOffset] = (word_t)RGB16(r32to16(GetRed(color)),
		g32to16(GetGreen(color)),
		b32to16(GetBlue(color)));
}

/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas16::PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color)
{
	word_t    i;

	for (i = 0; i < pixel_width; i++)
		m_pWriteBuffer16[pixelOffset + i] = (word_t)RGB16(r32to16(GetRed(color)),
			g32to16(GetGreen(color)),
			b32to16(GetBlue(color)));
}



/****************************************************************************
 *
 * GetPixel
 *
 ****************************************************************************/
dword_t BitCanvas16::GetPixel(const dword_t pixelOffset)
{
	return m_pReadBuffer16[pixelOffset];
}


// DISPATCH:

void BitCanvas16::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	//DoDelta_cpp_BASELINE_WITH_DECAY_63(lpTransitionTable);
	DoDelta_cpp_OPTIMIZED_WITH_DECAY_63(lpTransitionTable);
}

void BitCanvas16::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	//DoDelta_cpp_OPTIMIZED_WITH_DECAY_63(lpTransitionTable);
	DoDelta_SSE2_experiment2(lpTransitionTable);
}

void BitCanvas16::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	//DoDelta_cpp_OPTIMIZED_WITH_DECAY_63_AND_GREENSHIFT_CORRECTION(lpTransitionTable);
	DoDelta_SSE2_experiment5(lpTransitionTable);
}

void BitCanvas16::DoDelta_SSE(const PIXELMAP* lpTransitionTable)
{
	DoDelta_SSE2_Intrinsics(lpTransitionTable);
}


// IMPLEMENTATIONS:

/****************************************************************************
 * @brief This is a baseline/REFERENCE implementation of the straightforward
 * algorithm for 16bit color.
 *
 * I expect to use one of the more optimized implementations.
 *
 * pre: delta fields are precalculated
 *
 * No decay. Just average. This is faster than decay.
 *
 * VERIFIED ACCURATE.
 *
 * VERIFIED BAD IDEA: it often works fine, but periodically gets LARGE sections
 * of a single color which don't decay, so I think "No Decay" is probably a bad idea.
 * NoDecay does not guarantee convergence. Large areas of a single color can
 * get stuck for a while and not continually fade/dissapate/decay every frame.
 * Implementation is DEPRECATED outside of testing/verification and research.
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  pixel, pixel3;
	word_t   p1, p2, p3, p4;
	dword_t  bufferLength = m_dwBufferWidth * m_dwBufferHeight;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;
		p1 = m_pReadBuffer16[pixel];
		p2 = m_pReadBuffer16[pixel + 1];
		p3 = m_pReadBuffer16[pixel3];
		p4 = m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				((RFromRGB16(p1) + RFromRGB16(p2) + RFromRGB16(p3) + RFromRGB16(p4)) DECAY16),
				((GFromRGB16(p1) + GFromRGB16(p2) + GFromRGB16(p3) + GFromRGB16(p4)) DECAY16),// & ~0x1, //16bit greenshift correction (probably bad/invalid idea)
				((BFromRGB16(p1) + BFromRGB16(p2) + BFromRGB16(p3) + BFromRGB16(p4)) DECAY16)
			);

		pixelOffset++;
	} while (pixelOffset < bufferLength);
}

/****************************************************************************
 * @brief This is a baseline/REFERENCE implementation of the straightforward
 * algorithm for 16bit color.
 *
 * I expect to use one of the more optimized implementations.
 *
 * pre: delta fields are precalculated
 *
 * Faster decay to black. Unsure if I like. NEED to numerically compare to
 * 8bit grayscale, and decide the ideal 16 (and 32bit) decay values
 *
 * NOTE: 8bit takes around 190 frames to decay to black (at 127/128 decay).
 * 16bit takes a MAXIMUM of 62 frames to decay to black.
 * There is no benefit to use a smaller decay like 1023/1024, as it decays
 * at the same rate as 63/64. Furthermore, the decay is identical to subtracting 1
 * before dividing by 4 for the average, which presents an optimization opportunity.
 * If substraction produces the same result, then why multiply at all?
 *
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_31(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  pixel, pixel3;
	word_t   p1, p2, p3, p4;
	dword_t  bufferLength = m_dwBufferWidth * m_dwBufferHeight;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;
		p1 = m_pReadBuffer16[pixel];
		p2 = m_pReadBuffer16[pixel + 1];
		p3 = m_pReadBuffer16[pixel3];
		p4 = m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				((RFromRGB16(p1) + RFromRGB16(p2) + RFromRGB16(p3) + RFromRGB16(p4)) * 31 >> 7),
				((GFromRGB16(p1) + GFromRGB16(p2) + GFromRGB16(p3) + GFromRGB16(p4)) * 31 >> 7),
				((BFromRGB16(p1) + BFromRGB16(p2) + BFromRGB16(p3) + BFromRGB16(p4)) * 31 >> 7)
			);

		pixelOffset++;
	} while (pixelOffset < bufferLength);
}

/****************************************************************************
 * @brief This is a baseline/REFERENCE implementation of the straightforward
 * algorithm for 16bit color.
 *
 * I expect to use one of the more optimized implementations.
 *
 * pre: delta fields are precalculated
 *
 * Decay to black, same value as 32bit (which may take longer and therefore not look the same).
 *
 * VERIFIED ACCURATE.
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63(const PIXELMAP* lpTransitionTable)
{
	dword_t  pixelOffset = 0;
	dword_t  pixel, pixel3;
	word_t   p1, p2, p3, p4;
	dword_t  bufferLength = m_dwBufferWidth * m_dwBufferHeight;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;
		p1 = m_pReadBuffer16[pixel];
		p2 = m_pReadBuffer16[pixel + 1];
		p3 = m_pReadBuffer16[pixel3];
		p4 = m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				(((RFromRGB16(p1) + RFromRGB16(p2) + RFromRGB16(p3) + RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) + GFromRGB16(p2) + GFromRGB16(p3) + GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) + BFromRGB16(p2) + BFromRGB16(p3) + BFromRGB16(p4)) * 63) >> 8)
			);

		pixelOffset++;
	} while (pixelOffset < bufferLength);
}

/****************************************************************************
 * @brief This is a baseline/REFERENCE implementation of the straightforward
 * algorithm for 16bit color.
 *
 * I expect to use one of the more optimized implementations.
 *
 * pre: delta fields are precalculated
 *
 * Decay to black, same as 32bit.
 *
 * NOTE: Just for testing and experimentation, NOT for production use!
 *
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY(const PIXELMAP* lpTransitionTable, dword_t decayNumerator, dword_t decayDenominator)
{
	dword_t  pixelOffset = 0;
	dword_t  pixel, pixel3;
	word_t   p1, p2, p3, p4;
	dword_t  bufferLength = m_dwBufferWidth * m_dwBufferHeight;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;
		p1 = m_pReadBuffer16[pixel];
		p2 = m_pReadBuffer16[pixel + 1];
		p3 = m_pReadBuffer16[pixel3];
		p4 = m_pReadBuffer16[pixel3 + 1];

		if (decayNumerator == 0)
		{
			DoDelta_cpp_OPTIMIZED_WITH_DECAY_63_AND_GREENSHIFT_CORRECTION(lpTransitionTable);
		}
		else if (decayDenominator > 0)
		{
			m_pWriteBuffer16[pixelOffset] =
				RGB16(
					byte_t(((RFromRGB16(p1) + RFromRGB16(p2) + RFromRGB16(p3) + RFromRGB16(p4)) * decayNumerator) / (4 * decayDenominator)),
					byte_t(((GFromRGB16(p1) + GFromRGB16(p2) + GFromRGB16(p3) + GFromRGB16(p4)) * decayNumerator) / (4 * decayDenominator)),
					byte_t(((BFromRGB16(p1) + BFromRGB16(p2) + BFromRGB16(p3) + BFromRGB16(p4)) * decayNumerator) / (4 * decayDenominator))
				);
		}
		else
		{
			// EXPERIMENTAL, just subtract 1 before average
			// NOTE: This behaves IDENTICALLY to decay 63/64, but may be faster
			m_pWriteBuffer16[pixelOffset] =
				RGB16(
					byte_t(((RFromRGB16(p1) + RFromRGB16(p2) + RFromRGB16(p3) + RFromRGB16(p4)) - 1) / (4)),
					byte_t(((GFromRGB16(p1) + GFromRGB16(p2) + GFromRGB16(p3) + GFromRGB16(p4)) - 1) / (4)),
					byte_t(((BFromRGB16(p1) + BFromRGB16(p2) + BFromRGB16(p3) + BFromRGB16(p4)) - 1) / (4))
				);

		}

		pixelOffset++;
	} while (pixelOffset < bufferLength);
}

/****************************************************************************
 * @brief This is a REFERENCE implementation used for 16bit color;
 * optimized
 *
 * pre: delta fields are precalculated
 *
 * TODO: JRDV: Need to determine why it doesn't match the BASELINE!
 *
 * VERIFIED ACCURATE w.r.t. BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY
 *
 * VERIFIED BAD IDEA: it often works fine, but periodically gets LARGE sections
 * of a single color which don't decay, so I think "No Decay" is probably a bad idea.
 * NoDecay does not guarantee convergence. Large areas of a single color can
 * get stuck for a while and not continually fade/dissapate/decay every frame.
 * Implementation is DEPRECATED outside of testing/verification and research.
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY(const PIXELMAP* lpTransitionTable)
{
	dword_t  destPixelOffset = 0;  /* initialize destination pixel offset */
	dword_t  srcPixelOffset, srcPixelOffset3;
	dword_t  srcPixel12, srcPixel34;
	dword_t  redHi, redLow;
	dword_t  greenHi, greenLow;
	dword_t  blueHi, blueLow;
	dword_t  destPixel;

	/*
	 * the source pixel actually consists of 4 pixels, the RGB components of
	 * which must be averaged to find the value for the destination pixel.
	 *
	 * the 4 pixels are numbered as follows:
	 *     p1 | p2
	 *     ---+---
	 *     p3 | p4
	 *
	 *  p1 and p3 are considered the "high pixels"
	 *  p2 and p4 are the low pixels, since they are stored in the high order
	 *  bits and low order bits of a DWORD respectively.
	 *
	 * the below code may be a little confusing, because of the way I acquire
	 * the pixel values, and then average them.
	 *
	 * Exploiting the fact memory reads access 32 bits of
	 * information, and each pixel is 16 bits, the code is written to read in
	 * two pixels using a single memory read.  Memory access is one of the
	 * slowest operations, so any reduction in those operations can
	 * potentially speed things up significantly.
	 *
	 * Another optimization was using "clever data structure" to use
	 * 2 additions to perform the work of 3 additions
	 */
	do
	{
		srcPixelOffset = lpTransitionTable[destPixelOffset]; /* p1 offset */
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;  /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset3]));

		/* RED:
		 * this next line does a lot.
		 *
		 * 1) mask the red bits.
		 *    there are two pixels in each DWORD so 0xF800F800 is necessary to
		 *    clear the green and blue bits in both pixels.
		 *
		 * 2) shift each packed red pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 *
		 * 3) sum the two packed pixels.
		 *    this may be the most confusing part.
		 *    Since the pixel data is packed, a single addition can sum the
		 *    two high pixels and the two low pixels in parallel
		 */
		 // NOTE: Divide RED first, so we don't overflow when adding.
		redLow = ((srcPixel12 & 0xF800F800) >> 2) +
			((srcPixel34 & 0xF800F800) >> 2);

		/* the next step is to unpack the pixel data,
		 * so get the high pixel data into the low bits
		 */
		redHi = redLow >> 16;

		/*
		 * sum the high order sum and low order sum, and mask to red bits only
		 */
		destPixel = (redLow + redHi) & 0xF800;


		/* GREEN:
		 * this next line does a lot.  And is slightly different from red
		 *
		 * 1) mask the green bits.
		 *    there are two pixels in each DWORD so 0x07E007E0 is necessary to
		 *    clear the red and blue bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 *
		 * 3) shift the packed green sum pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 */
		 // NOTE: Green should add and divide to combine
		greenLow = (((srcPixel12 & 0x07E007E0)) +
			((srcPixel34 & 0x07E007E0))) >> 2;

		greenHi = greenLow >> 16;  /* unpack the high order green pixel data */

		/*
		 * sum the hi and low green data,
		 * mask it to the green bits only,
		 * and then or the green bits with the red in the destination pixel
		 */
		destPixel |= ((greenLow + greenHi)) & 0x07E0;


		/* BLUE:
		 * this next line does a lot.  And different from either red or green
		 *
		 * 1) mask the blue bits.
		 *    there are two pixels in each DWORD so 0x001F001F is necessary to
		 *    clear the red and green bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 */
		 // NOTE: Add BLUE first, so we don't drop bits by dividing early.
		blueLow = ((srcPixel12 & 0x001F001F)) +
			((srcPixel34 & 0x001F001F));

		blueHi = blueLow >> 16;  /* unpack the high order blue pixel data */

		/*
		 * this next line does more than either red or green
		 *
		 * 1) sum the hi and low blue data
		 *
		 * 2) average the blue pixel data
		 *
		 * 3) mask it to the blue bits only
		 *
		 * 4) or the blue bits into the destination pixel
		 */
		destPixel |= ((blueLow + blueHi) >> 2) & 0x001F;



		/*
		 * write the destination pixel data to the destination pixel
		 */
		m_pWriteBuffer16[destPixelOffset] = (word_t)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);
}

/****************************************************************************
 * @brief This is the DEFAULT implementation used for 16bit color;
 * optimized
 *
 * pre: delta fields are precalculated
 *
 * Intention was to try to combat the "greenshift" effect by decaying the green channel slightly faster.
 *
 * VERIFIED ACCURATE w.r.t. BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63
 ****************************************************************************/
void    BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63(const PIXELMAP* lpTransitionTable)
{
	dword_t  destPixelOffset = 0;  /* initialize destination pixel offset */
	dword_t  srcPixelOffset, srcPixelOffset3;
	dword_t  srcPixel12, srcPixel34;
	dword_t  redHi, redLow;
	dword_t  greenHi, greenLow;
	dword_t  blueHi, blueLow;
	dword_t  destPixel;


	/*
	 * the source pixel actually consists of 4 pixels, the RGB components of
	 * which must be averaged to find the value for the destination pixel.
	 *
	 * the 4 pixels are numbered as follows:
	 *     p1 | p2
	 *     ---+---
	 *     p3 | p4
	 *
	 *  p1 and p3 are considered the "high pixels"
	 *  p2 and p4 are the low pixels, since they are stored in the high order
	 *  bits and low order bits of a DWORD respectively.
	 *
	 * the below code may be a little confusing, because of the way I acquire
	 * the pixel values, and then average them.
	 *
	 * Exploiting the fact memory reads access 32 bits of
	 * information, and each pixel is 16 bits, the code is written to read in
	 * two pixels using a single memory read.  Memory access is one of the
	 * slowest operations, so any reduction in those operations can
	 * potentially speed things up significantly.
	 *
	 * Another optimization was using "clever data structure" to use
	 * 2 additions to perform the work of 3 additions
	 */
	do
	{
		srcPixelOffset = lpTransitionTable[destPixelOffset]; /* p1 offset */
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;  /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset3]));

		/* RED:
		 * this next line does a lot.
		 *
		 * 1) mask the red bits.
		 *    there are two pixels in each DWORD so 0xF800F800 is necessary to
		 *    clear the green and blue bits in both pixels.
		 *
		 * 2) shift each packed red pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 *    NOTE: THIS IS IMPORTANT! Dividing first means we don't overflow when adding
		 *
		 * 3) sum the two packed pixels.
		 *    this may be the most confusing part.
		 *    Since the pixel data is packed, a single addition can sum the
		 *    two high pixels and the two low pixels in parallel
		 */
		 // NOTE: Divide RED first, so we don't overflow when adding.
		redLow = ((srcPixel12 & 0xF800F800) >> 2) +
			((srcPixel34 & 0xF800F800) >> 2);

		/* the next step is to unpack the pixel data,
		 * so get the high pixel data into the low bits
		 */
		redHi = redLow >> 16;

		/*
		 * sum the high order sum and low order sum, and mask to red bits only
		 */
		destPixel = (((redLow + redHi) * 63) >> 6) & 0xF800;


		/* GREEN:
		 * this next line does a lot.  And is slightly different from red
		 *
		 * 1) mask the green bits.
		 *    there are two pixels in each DWORD so 0x07E007E0 is necessary to
		 *    clear the red and blue bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 *
		 * 3) shift the packed green sum pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 */
		 // NOTE: Green should add and divide to combine
		greenLow = (((srcPixel12 & 0x07E007E0)) +
			((srcPixel34 & 0x07E007E0)));
		greenHi = greenLow >> 16;  /* unpack the high order green pixel data */

		/*
		 * sum the hi and low green data,
		 * mask it to the green bits only,
		 * and then or the green bits with the red in the destination pixel
		 */
		destPixel |= (((greenLow + greenHi) * 63) >> 8) & 0x07E0;


		/* BLUE:
		 * this next line does a lot.  And different from red
		 *
		 * 1) mask the blue bits.
		 *    there are two pixels in each DWORD so 0x001F001F is necessary to
		 *    clear the red and green bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 *
		 * The math is asymmetric between RED and GREEN/BLUE channels because
		 * dividing first would potentially TRUNCATE some blue bits.
		 */
		 // NOTE: Add BLUE first, so we don't drop bits by dividing early.
		blueLow = ((srcPixel12 & 0x001F001F)) +
			((srcPixel34 & 0x001F001F));

		blueHi = blueLow >> 16;  /* unpack the high order blue pixel data */

		/*
		 * this next line does more than red
		 *
		 * 1) sum the hi and low blue data
		 *
		 * 2) average the blue pixel data
		 *
		 * 3) mask it to the blue bits only
		 *
		 * 4) `OR` the blue bits into the destination pixel
		 */
		destPixel |= (((blueLow + blueHi) * 63) >> 8) & 0x001F;



				/*
				 * write the destination pixel data to the destination pixel
				 */
		m_pWriteBuffer16[destPixelOffset] = (word_t)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);
}

/****************************************************************************
 * @brief This is the CANDIDATE implementation used for 16bit color;
 * optimized
 *
 * pre: delta fields are precalculated
 *
 * Intention was to try to combat the "greenshift" effect by decaying the green channel slightly faster.
 *
 * At 63/64 decay rate:
 * RED and BLUE converge to zero/black afetr 30 frames.
 * GREEN converges to zero/black after 62 frames.
 * At 14/15 decay rate:
 * GREEN converges to black after 30 frames (but has terrible artifacting)
 * At 15/16 decay rate:
 * GREEN converges to black after 32 frames (and has no noticable artifacting)
 * Therefore, 15/16 decay rate for GREEN is the current candidate for counteracting the greenshift effect.
 *
 ****************************************************************************/
void    BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63_AND_GREENSHIFT_CORRECTION(const PIXELMAP* lpTransitionTable)
{
	dword_t  destPixelOffset = 0;  /* initialize destination pixel offset */
	dword_t  srcPixelOffset, srcPixelOffset3;
	dword_t  srcPixel12, srcPixel34;
	dword_t  redHi, redLow;
	dword_t  greenHi, greenLow;
	dword_t  blueHi, blueLow;
	dword_t  destPixel;


	/*
	 * the source pixel actually consists of 4 pixels, the RGB components of
	 * which must be averaged to find the value for the destination pixel.
	 *
	 * the 4 pixels are numbered as follows:
	 *     p1 | p2
	 *     ---+---
	 *     p3 | p4
	 *
	 *  p1 and p3 are considered the "high pixels"
	 *  p2 and p4 are the low pixels, since they are stored in the high order
	 *  bits and low order bits of a DWORD respectively.
	 *
	 * the below code may be a little confusing, because of the way I acquire
	 * the pixel values, and then average them.
	 *
	 * Exploiting the fact memory reads access 32 bits of
	 * information, and each pixel is 16 bits, the code is written to read in
	 * two pixels using a single memory read.  Memory access is one of the
	 * slowest operations, so any reduction in those operations can
	 * potentially speed things up significantly.
	 *
	 * Another optimization was using "clever data structure" to use
	 * 2 additions to perform the work of 3 additions
	 */
	do
	{
		srcPixelOffset = lpTransitionTable[destPixelOffset]; /* p1 offset */
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;  /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset3]));

		/* RED:
		 * this next line does a lot.
		 *
		 * 1) mask the red bits.
		 *    there are two pixels in each DWORD so 0xF800F800 is necessary to
		 *    clear the green and blue bits in both pixels.
		 *
		 * 2) shift each packed red pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 *
		 * 3) sum the two packed pixels.
		 *    this may be the most confusing part.
		 *    Since the pixel data is packed, a single addition can sum the
		 *    two high pixels and the two low pixels in parallel
		 */
		 // NOTE: Divide RED first, so we don't overflow when adding.
		redLow = ((srcPixel12 & 0xF800F800) >> 2) +
			((srcPixel34 & 0xF800F800) >> 2);

		/* the next step is to unpack the pixel data,
		 * so get the high pixel data into the low bits
		 */
		redHi = redLow >> 16;

		/*
		 * sum the high order sum and low order sum, and mask to red bits only
		 */
		destPixel = (((redLow + redHi) * 63) >> 6) & 0xF800;


		/* GREEN:
		 * this next line does a lot.  And is slightly different from red
		 *
		 * 1) mask the green bits.
		 *    there are two pixels in each DWORD so 0x07E007E0 is necessary to
		 *    clear the red and blue bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 *
		 * 3) shift the packed green sum pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 */
		 // NOTE: Green should add and divide to combine (DO WE NEED TO DIVIDE FIRST TO ENSURE NO OVERFLOW DURING MULTIPLY? It gets more complex with decay and green correction)
		greenLow = (((srcPixel12 & 0x07E007E0)) +
			((srcPixel34 & 0x07E007E0))) >> (2);
		greenHi = greenLow >> 16;  /* unpack the high order green pixel data */

		/*
		 * sum the hi and low green data,
		 * mask it to the green bits only,
		 * and then or the green bits with the red in the destination pixel
		 *
		 * NOTE: I tried using 14/15 for greenshift correction, and it ended up with bizarre artifacting.
		 * I bet the numbers don't fit exactly in the mack space for green: 0x07E0 (but that doesn't make sense... it _should_ work)
		 * So, I tried using 15/16 for greenshift correction, has no noticable problems. I'll save this off as a likely candidate.
		 */
		//destPixel |= ((((greenLow + greenHi) * 14) / 15) << 3) & 0x07E0;
		//destPixel |= ((((greenLow + greenHi) * 15) / 16) << 3) & 0x07E0;
		//destPixel |= ((((greenLow + greenHi) * 63) / 64) << 3) & 0x07E0;
		//destPixel |= ((((greenLow + greenHi) * 14) / 15)) & 0x07E0;
		destPixel |= ((((greenLow + greenHi) * 15) / 16)) & 0x07E0;


		/* BLUE:
		 * this next line does a lot.  And different from either red or green
		 *
		 * 1) mask the blue bits.
		 *    there are two pixels in each DWORD so 0x001F001F is necessary to
		 *    clear the red and green bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 */
		 // NOTE: Add BLUE first, so we don't drop bits by dividing early.
		blueLow = ((srcPixel12 & 0x001F001F)) +
			((srcPixel34 & 0x001F001F));

		blueHi = blueLow >> 16;  /* unpack the high order blue pixel data */

		/*
		 * this next line does more than either red or green
		 *
		 * 1) sum the hi and low blue data
		 *
		 * 2) average the blue pixel data
		 *
		 * 3) mask it to the blue bits only
		 *
		 * 4) or the blue bits into the destination pixel
		 */
		destPixel |= (((blueLow + blueHi)*63) >> 8) & 0x001F;



				/*
				 * write the destination pixel data to the destination pixel
				 */
		m_pWriteBuffer16[destPixelOffset] = (word_t)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);
}

/****************************************************************************
 *
 * @brief EXPERIMENTAL, place to explore potential 16bit optimizations;
 * NOT actually tied to MMX intrinsics/assembly;
 * do not use as default without testing/measuring.
 *
 * pre: delta fields are precalculated
 *
 * I think the intention was to remove the decay, but try to combat the "greenshift" effect
 * a different way, which could mean 16bit could run as fast as the BASELINE
 *
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY_GreenCorrected_experiment1(const PIXELMAP* lpTransitionTable)
{
	dword_t  destPixelOffset = 0;  /* initialize destination pixel offset */
	dword_t  srcPixelOffset, srcPixelOffset3;
	dword_t  srcPixel12, srcPixel34;
	dword_t  redHi, redLow;
	dword_t  greenHi, greenLow;
	dword_t  blueHi, blueLow;
	dword_t  destPixel;

	/*
	 * the source pixel actually consists of 4 pixels, the RGB components of
	 * which must be averaged to find the value for the destination pixel.
	 *
	 * the 4 pixels are numbered as follows:
	 *     p1 | p2
	 *     ---+---
	 *     p3 | p4
	 *
	 *  p1 and p3 are considered the high pixels
	 *  p2 and p4 are the low pixels, since they are stored in the high order
	 *  bits and low order bits of a DWORD respectively.
	 *
	 * the below code may be a little confusing, because of the way I acquire
	 * the pixel values, and then average them.
	 *
	 * Exploiting the fact memory reads access 32 bits of
	 * information, and each pixel is 16 bits, the code is written to read in
	 * two pixels using a single memory read.  Memory access is one of the
	 * slowest operations, so any reduction in those operations can
	 * potentially speed things up significantly.
	 *
	 * Another optimization was using "clever data structure" to use
	 * 2 additions to perform the work of 3 additions
	 */
	do
	{
		srcPixelOffset = lpTransitionTable[destPixelOffset]; /* p1 offset */
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;  /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(dword_t*)(&(m_pReadBuffer16[srcPixelOffset3]));

		/* RED:
		 * this next line does a lot.
		 *
		 * 1) mask the red bits.
		 *    there are two pixels in each DWORD so 0xF800F800 is necessary to
		 *    clear the green and blue bits in both pixels.
		 *
		 * 2) shift each packed red pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 *
		 * 3) sum the two packed pixels.
		 *    this may be the most confusing part.
		 *    Since the pixel data is packed, a single addition can sum the
		 *    two high pixels and the two low pixels in parallel
		 */
		redLow = ((srcPixel12 & 0xF800F800) >> 2) +
			((srcPixel34 & 0xF800F800) >> 2);

		/* the next step is to unpack the pixel data,
		 * so get the high pixel data into the low bits
		 */
		redHi = redLow >> 16;

		/*
		 * sum the high order sum and low order sum, and mask to red bits only
		 */
		destPixel = (redLow + redHi) & 0xF800;


		/* GREEN:
		 * this next line does a lot.  And is slightly different from red
		 *
		 * 1) mask the green bits.
		 *    there are two pixels in each DWORD so 0x07E007E0 is necessary to
		 *    clear the red and blue bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 *
		 * 3) shift the packed green sum pixel 2 bits to the right
		 *    effectively dividing by 4 for averaging the pixels
		 */
		greenLow = (((srcPixel12 & 0x07F007E0)) +
			((srcPixel34 & 0x07E007E0))) >> 2;

		greenHi = greenLow >> 16;  /* unpack the high order green pixel data */

		/*
		 * sum the hi and low green data,
		 * mask it to the green bits only,
		 * and then or the green bits with the red in the destination pixel
		 */
		destPixel |= ((greenLow + greenHi)) & 0x07E0;


		/* BLUE:
		 * this next line does a lot.  And different from either red or green
		 *
		 * 1) mask the blue bits.
		 *    there are two pixels in each DWORD so 0x001F001F is necessary to
		 *    clear the red and green bits in both pixels.
		 *
		 * 2) sum the two packed pixels.
		 */
		blueLow = ((srcPixel12 & 0x001F001F)) +
			((srcPixel34 & 0x001F001F));

		blueHi = blueLow >> 16;  /* unpack the high order blue pixel data */

		/*
		 * this next line does more than either red or green
		 *
		 * 1) sum the hi and low blue data
		 *
		 * 2) average the blue pixel data
		 *
		 * 3) mask it to the blue bits only
		 *
		 * 4) or the blue bits into the destination pixel
		 */
		destPixel |= ((blueLow + blueHi) >> 2) & 0x001F;



		/*
		 * write the destination pixel data to the destination pixel
		 */
		m_pWriteBuffer16[destPixelOffset] = (word_t)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);
}


// AI Assisted, baseline SSE2 implementation just to determine what we're working with.
void BitCanvas16::DoDelta_SSE2_experiment1(const PIXELMAP* lpTransitionTable)
{
	const dword_t bufferLength =
		m_dwBufferWidth * m_dwBufferHeight;

	const __m128i redMask =
		_mm_set1_epi16(_REDMASK16);

	const __m128i greenMask =
		_mm_set1_epi16(_GREENMASK16);

	const __m128i blueMask =
		_mm_set1_epi16(_BLUEMASK16);

	const __m128i decay63 =
		_mm_set1_epi16(63);

	const __m128i zero =
		_mm_setzero_si128();

	dword_t pixelOffset = 0;

	while (pixelOffset + 7 < bufferLength)
	{
		word_t src1[8];
		word_t src2[8];
		word_t src3[8];
		word_t src4[8];

		//
		// Scalar gather.
		//
		for (int i = 0; i < 8; ++i)
		{
			const dword_t pixel =
				lpTransitionTable[pixelOffset + i];

			const dword_t pixel3 =
				pixel + m_dwBufferWidth;

			src1[i] = m_pReadBuffer16[pixel];
			src2[i] = m_pReadBuffer16[pixel + 1];
			src3[i] = m_pReadBuffer16[pixel3];
			src4[i] = m_pReadBuffer16[pixel3 + 1];
		}

		const __m128i p1 =
			_mm_loadu_si128(
				reinterpret_cast<const __m128i*>(src1));

		const __m128i p2 =
			_mm_loadu_si128(
				reinterpret_cast<const __m128i*>(src2));

		const __m128i p3 =
			_mm_loadu_si128(
				reinterpret_cast<const __m128i*>(src3));

		const __m128i p4 =
			_mm_loadu_si128(
				reinterpret_cast<const __m128i*>(src4));

		//
		// Unpack R.
		//
		__m128i r =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(p1, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(p2, redMask),
						REDSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(p3, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(p4, redMask),
						REDSHIFT16)));

		//
		// Unpack G.
		//
		__m128i g =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(p1, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(p2, greenMask),
						GREENSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(p3, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(p4, greenMask),
						GREENSHIFT16)));

		//
		// Unpack B.
		//
		__m128i b =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_and_si128(p1, blueMask),
					_mm_and_si128(p2, blueMask)),
				_mm_add_epi16(
					_mm_and_si128(p3, blueMask),
					_mm_and_si128(p4, blueMask)));

		//
		// (sum * 63) >> 8
		//
		r = _mm_mullo_epi16(r, decay63);
		g = _mm_mullo_epi16(g, decay63);
		b = _mm_mullo_epi16(b, decay63);

		r = _mm_srli_epi16(r, 8);
		g = _mm_srli_epi16(g, 8);
		b = _mm_srli_epi16(b, 8);

		//
		// Repack RGB565.
		//
		r = _mm_slli_epi16(r, REDSHIFT16);
		g = _mm_slli_epi16(g, GREENSHIFT16);

		__m128i result =
			_mm_or_si128(
				_mm_or_si128(r, g),
				b);

		_mm_storeu_si128(
			reinterpret_cast<__m128i*>(
				&m_pWriteBuffer16[pixelOffset]),
			result);

		pixelOffset += 8;
	}

	//
	// Scalar tail.
	//
	while (pixelOffset < bufferLength)
	{
		const dword_t pixel =
			lpTransitionTable[pixelOffset];

		const dword_t pixel3 =
			pixel + m_dwBufferWidth;

		const word_t p1 =
			m_pReadBuffer16[pixel];

		const word_t p2 =
			m_pReadBuffer16[pixel + 1];

		const word_t p3 =
			m_pReadBuffer16[pixel3];

		const word_t p4 =
			m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				(((RFromRGB16(p1) +
					RFromRGB16(p2) +
					RFromRGB16(p3) +
					RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) +
					GFromRGB16(p2) +
					GFromRGB16(p3) +
					GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) +
					BFromRGB16(p2) +
					BFromRGB16(p3) +
					BFromRGB16(p4)) * 63) >> 8));

		++pixelOffset;
	}
}

// AI Assisted. Work in Progress. Attempting to improve upon Round1
// Remove temporary arrays
// Doubled the speed by avoiding memory access. Same lesson I learned long ago in
// DoDelta_cpp_OPTIMIZED_WITH_DECAY_63
void BitCanvas16::DoDelta_SSE2_experiment2(const PIXELMAP* lpTransitionTable)
{
	const dword_t bufferLength =
		m_dwBufferWidth * m_dwBufferHeight;

	const __m128i redMask =
		_mm_set1_epi16(_REDMASK16);

	const __m128i greenMask =
		_mm_set1_epi16(_GREENMASK16);

	const __m128i blueMask =
		_mm_set1_epi16(_BLUEMASK16);

	const __m128i decay63 =
		_mm_set1_epi16(63);

	dword_t pixelOffset = 0;

	while (pixelOffset + 7 < bufferLength)
	{
		dword_t p[8];
		dword_t p3[8];

		for (int i = 0; i < 8; ++i)
		{
			p[i] =
				lpTransitionTable[pixelOffset + i];

			p3[i] =
				p[i] + m_dwBufferWidth;
		}

		const __m128i a =
			_mm_set_epi16(
				m_pReadBuffer16[p[7]],
				m_pReadBuffer16[p[6]],
				m_pReadBuffer16[p[5]],
				m_pReadBuffer16[p[4]],
				m_pReadBuffer16[p[3]],
				m_pReadBuffer16[p[2]],
				m_pReadBuffer16[p[1]],
				m_pReadBuffer16[p[0]]);

		const __m128i b =
			_mm_set_epi16(
				m_pReadBuffer16[p[7] + 1],
				m_pReadBuffer16[p[6] + 1],
				m_pReadBuffer16[p[5] + 1],
				m_pReadBuffer16[p[4] + 1],
				m_pReadBuffer16[p[3] + 1],
				m_pReadBuffer16[p[2] + 1],
				m_pReadBuffer16[p[1] + 1],
				m_pReadBuffer16[p[0] + 1]);

		const __m128i c =
			_mm_set_epi16(
				m_pReadBuffer16[p3[7]],
				m_pReadBuffer16[p3[6]],
				m_pReadBuffer16[p3[5]],
				m_pReadBuffer16[p3[4]],
				m_pReadBuffer16[p3[3]],
				m_pReadBuffer16[p3[2]],
				m_pReadBuffer16[p3[1]],
				m_pReadBuffer16[p3[0]]);

		const __m128i d =
			_mm_set_epi16(
				m_pReadBuffer16[p3[7] + 1],
				m_pReadBuffer16[p3[6] + 1],
				m_pReadBuffer16[p3[5] + 1],
				m_pReadBuffer16[p3[4] + 1],
				m_pReadBuffer16[p3[3] + 1],
				m_pReadBuffer16[p3[2] + 1],
				m_pReadBuffer16[p3[1] + 1],
				m_pReadBuffer16[p3[0] + 1]);

		__m128i r =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, redMask),
						REDSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(d, redMask),
						REDSHIFT16)));

		__m128i g =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, greenMask),
						GREENSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(d, greenMask),
						GREENSHIFT16)));

		__m128i bl =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_and_si128(a, blueMask),
					_mm_and_si128(b, blueMask)),
				_mm_add_epi16(
					_mm_and_si128(c, blueMask),
					_mm_and_si128(d, blueMask)));

		r = _mm_srli_epi16(
			_mm_mullo_epi16(r, decay63),
			8);

		g = _mm_srli_epi16(
			_mm_mullo_epi16(g, decay63),
			8);

		bl = _mm_srli_epi16(
			_mm_mullo_epi16(bl, decay63),
			8);

		r = _mm_slli_epi16(r, REDSHIFT16);
		g = _mm_slli_epi16(g, GREENSHIFT16);

		const __m128i result =
			_mm_or_si128(
				_mm_or_si128(r, g),
				bl);

		_mm_storeu_si128(
			reinterpret_cast<__m128i*>(
				&m_pWriteBuffer16[pixelOffset]),
			result);

		pixelOffset += 8;
	}

	//
	// Scalar tail.
	//
	while (pixelOffset < bufferLength)
	{
		const dword_t pixel =
			lpTransitionTable[pixelOffset];

		const dword_t pixel3 =
			pixel + m_dwBufferWidth;

		const word_t p1 =
			m_pReadBuffer16[pixel];

		const word_t p2 =
			m_pReadBuffer16[pixel + 1];

		const word_t p3 =
			m_pReadBuffer16[pixel3];

		const word_t p4 =
			m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				(((RFromRGB16(p1) +
					RFromRGB16(p2) +
					RFromRGB16(p3) +
					RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) +
					GFromRGB16(p2) +
					GFromRGB16(p3) +
					GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) +
					BFromRGB16(p2) +
					BFromRGB16(p3) +
					BFromRGB16(p4)) * 63) >> 8));

		++pixelOffset;
	}
}


//Exp1 = temp arrays
//Exp2 = direct vector construction(_mm_set_epi16)
//Exp3 = skipped / superseded after disassembly review
//Exp4 = 4 - pixel SSE2 variant
//Exp5 = old DWORD pseudo - SIMD concept inside SSE2

// AI Assisted. Work in Progress. Attempting to improve upon Round2
// Reduce the number of gathers. -- Parallelize reads by using DWORDs instead of multiple WORDs
// SKIPPED/ABANDONED this idea after further review. The optimizer already largely does what
// we were going to try to optimize, so it doesn't really make sense to explicitly test this
void BitCanvas16::DoDelta_SSE2_experiment3(const PIXELMAP* lpTransitionTable)
{
	DoDelta_SSE2_experiment2(lpTransitionTable);
}



// Experiment4 Idea(more interesting)
// Reduce SIMD width. -- Process only 4 output pixels per iteration.
// This is mostly an experiment to see whether the gather overhead dominates enough that fewer lanes helps.
// Honestly, I don't expect a huge win.
//
// NOTE: This turned out slightly slower than Experiment2, therefore,
// the hypothesis: "Maybe register pressure is hurting us." is false.
//
// Experiment4 answered the question cleanly: processing 8 outputs at a time is the better choice.
void BitCanvas16::DoDelta_SSE2_experiment4(const PIXELMAP* lpTransitionTable)
{
	const dword_t bufferLength =
		m_dwBufferWidth * m_dwBufferHeight;

	const __m128i redMask =
		_mm_set1_epi16(_REDMASK16);

	const __m128i greenMask =
		_mm_set1_epi16(_GREENMASK16);

	const __m128i blueMask =
		_mm_set1_epi16(_BLUEMASK16);

	const __m128i decay63 =
		_mm_set1_epi16(63);

	dword_t pixelOffset = 0;

	while (pixelOffset + 3 < bufferLength)
	{
		const dword_t p0 =
			lpTransitionTable[pixelOffset + 0];

		const dword_t p1 =
			lpTransitionTable[pixelOffset + 1];

		const dword_t p2 =
			lpTransitionTable[pixelOffset + 2];

		const dword_t p3 =
			lpTransitionTable[pixelOffset + 3];

		const dword_t q0 = p0 + m_dwBufferWidth;
		const dword_t q1 = p1 + m_dwBufferWidth;
		const dword_t q2 = p2 + m_dwBufferWidth;
		const dword_t q3 = p3 + m_dwBufferWidth;

		const __m128i a =
			_mm_set_epi16(
				0, 0, 0, 0,
				m_pReadBuffer16[p3],
				m_pReadBuffer16[p2],
				m_pReadBuffer16[p1],
				m_pReadBuffer16[p0]);

		const __m128i b =
			_mm_set_epi16(
				0, 0, 0, 0,
				m_pReadBuffer16[p3 + 1],
				m_pReadBuffer16[p2 + 1],
				m_pReadBuffer16[p1 + 1],
				m_pReadBuffer16[p0 + 1]);

		const __m128i c =
			_mm_set_epi16(
				0, 0, 0, 0,
				m_pReadBuffer16[q3],
				m_pReadBuffer16[q2],
				m_pReadBuffer16[q1],
				m_pReadBuffer16[q0]);

		const __m128i d =
			_mm_set_epi16(
				0, 0, 0, 0,
				m_pReadBuffer16[q3 + 1],
				m_pReadBuffer16[q2 + 1],
				m_pReadBuffer16[q1 + 1],
				m_pReadBuffer16[q0 + 1]);

		__m128i r =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, redMask),
						REDSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(d, redMask),
						REDSHIFT16)));

		__m128i g =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, greenMask),
						GREENSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, greenMask),
						GREENSHIFT16),
				_mm_srli_epi16(
					_mm_and_si128(d, greenMask),
					GREENSHIFT16)));

					__m128i bl =
						_mm_add_epi16(
							_mm_add_epi16(
								_mm_and_si128(a, blueMask),
								_mm_and_si128(b, blueMask)),
							_mm_add_epi16(
								_mm_and_si128(c, blueMask),
								_mm_and_si128(d, blueMask)));

					r = _mm_srli_epi16(
						_mm_mullo_epi16(r, decay63),
						8);

					g = _mm_srli_epi16(
						_mm_mullo_epi16(g, decay63),
						8);

					bl = _mm_srli_epi16(
						_mm_mullo_epi16(bl, decay63),
						8);

					r = _mm_slli_epi16(r, REDSHIFT16);
					g = _mm_slli_epi16(g, GREENSHIFT16);

					const __m128i result =
						_mm_or_si128(
							_mm_or_si128(r, g),
							bl);

					_mm_storel_epi64(
						reinterpret_cast<__m128i*>(
							&m_pWriteBuffer16[pixelOffset]),
						result);

					pixelOffset += 4;
	}

	//
	// Existing scalar tail from Exp2.
	// NOTE: Probably need to add scalar tail handling for odd widths, simply because arbitrary widths are allowed
	// **OR** do I want to add a requirement/restriction enforcing even widths?
	// That is probably better, to enforce some constraints in the name of performance.
	//
	while (pixelOffset < bufferLength)
	{
		const dword_t pixel =
			lpTransitionTable[pixelOffset];

		const dword_t pixel3 =
			pixel + m_dwBufferWidth;

		const word_t p1 =
			m_pReadBuffer16[pixel];

		const word_t p2 =
			m_pReadBuffer16[pixel + 1];

		const word_t p3 =
			m_pReadBuffer16[pixel3];

		const word_t p4 =
			m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				(((RFromRGB16(p1) +
					RFromRGB16(p2) +
					RFromRGB16(p3) +
					RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) +
					GFromRGB16(p2) +
					GFromRGB16(p3) +
					GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) +
					BFromRGB16(p2) +
					BFromRGB16(p3) +
					BFromRGB16(p4)) * 63) >> 8));

		++pixelOffset;
	}
}


// Experiment5(the one I'm most curious about)
// Use my old DWORD trick inside SSE2.
// Take my existing winner: DoDelta_cpp_OPTIMIZED_WITH_DECAY_63
// the old DWORD - packed pseudo - SIMD implementation.
//
// Exp5 may end up looking very different from Exp1/Exp2/Exp4, because it may avoid a lot of the unpacking that those versions still perform.
//
// Packed-Domain Exp5 code
//
//Exp2:
//    Gather WORD pixels
//    Expand to RGB channels
//    SIMD channel math
//    Repack
//
//Exp5:
//    Gather DWORD p1|p2
//    Gather DWORD p3|p4
//    Apply the existing packed-domain algorithm
//    across multiple DWORD lanes at once
//
//
//16-bit Benchmarks
//-----------------
//
//CPP Baseline      12.2 sec
//CPP Optimized      8.0 sec
//
//SSE2 Exp1          9.4 sec
//SSE2 Exp2          4.5 sec
//SSE2 Exp4          5.1 sec
//SSE2 Exp5          4.5 sec
//
//Winner:
//    Exp2 ~~ Exp5
//
//Observation:
//    Arithmetic design matters less than gather cost.
//
// ---------------------------------------------------------------------
// Exp5 keeps the original DWORD-packed arithmetic model and
// performs SIMD operations on packed DWORD lanes.
//
// IMPORTANT:
// This implementation requires 32-bit multiplication
// (_mm_mullo_epi32). Attempting to use _mm_mullo_epi16
// corrupts RED and GREEN channel calculations because the
// packed-domain arithmetic depends on 32-bit lane semantics.
//
void BitCanvas16::DoDelta_SSE2_experiment5(const PIXELMAP* lpTransitionTable)
{
	const dword_t bufferLength =
		m_dwBufferWidth * m_dwBufferHeight;

	const __m128i redMask =
		_mm_set1_epi32(0xF800F800);

	const __m128i greenMask =
		_mm_set1_epi32(0x07E007E0);

	const __m128i blueMask =
		_mm_set1_epi32(0x001F001F);

	const __m128i decay63 =
		_mm_set1_epi32(63);

	dword_t destPixelOffset = 0;

	while (destPixelOffset + 3 < m_dwBufferLength)
	{
		dword_t s0 =
			lpTransitionTable[destPixelOffset + 0];

		dword_t s1 =
			lpTransitionTable[destPixelOffset + 1];

		dword_t s2 =
			lpTransitionTable[destPixelOffset + 2];

		dword_t s3 =
			lpTransitionTable[destPixelOffset + 3];

		dword_t s30 = s0 + m_dwBufferWidth;
		dword_t s31 = s1 + m_dwBufferWidth;
		dword_t s32 = s2 + m_dwBufferWidth;
		dword_t s33 = s3 + m_dwBufferWidth;

		//
		// Gather packed p1|p2 DWORDs.
		//
		__m128i src12 =
			_mm_set_epi32(
				*(dword_t*)&m_pReadBuffer16[s3],
				*(dword_t*)&m_pReadBuffer16[s2],
				*(dword_t*)&m_pReadBuffer16[s1],
				*(dword_t*)&m_pReadBuffer16[s0]);

		//
		// Gather packed p3|p4 DWORDs.
		//
		__m128i src34 =
			_mm_set_epi32(
				*(dword_t*)&m_pReadBuffer16[s33],
				*(dword_t*)&m_pReadBuffer16[s32],
				*(dword_t*)&m_pReadBuffer16[s31],
				*(dword_t*)&m_pReadBuffer16[s30]);

		//
		// RED
		//
		__m128i redLow =
			_mm_add_epi32(
				_mm_srli_epi32(
					_mm_and_si128(src12, redMask),
					2),
				_mm_srli_epi32(
					_mm_and_si128(src34, redMask),
					2)); // 		redLow = ((srcPixel12 & 0xF800F800) >> 2) + ((srcPixel34 & 0xF800F800) >> 2);


		__m128i redHi =
			_mm_srli_epi32(redLow, 16); // redHi = redLow >> 16;

		__m128i red =
			_mm_add_epi32(redLow, redHi); // destPixel = (((redLow + redHi) * 63) >> 6) & 0xF800;

		red =
			//_mm_mullo_epi16(red, decay63);     // likely needs work
			//_mm_mul_epu32(red, decay63x32);
			_mm_mullo_epi32(red, decay63);

		red =
			_mm_srli_epi32(red, 6);

		red =
			_mm_and_si128(
				red,
				_mm_set1_epi32(0x0000F800));

		//
		// GREEN
		//
		__m128i greenLow =
			_mm_add_epi32(
				_mm_and_si128(src12, greenMask),
				_mm_and_si128(src34, greenMask));

		__m128i greenHi =
			_mm_srli_epi32(greenLow, 16);

		__m128i green =
			_mm_add_epi32(greenLow, greenHi);

		green =
			//_mm_mullo_epi16(green, decay63);   // likely needs work
			//_mm_mul_epu32(green, decay63x32);
			_mm_mullo_epi32(green, decay63);

		green =
			_mm_srli_epi32(green, 8);

		green =
			_mm_and_si128(
				green,
				_mm_set1_epi32(0x000007E0));

		//
		// BLUE
		//
		__m128i blueLow =
			_mm_add_epi32(
				_mm_and_si128(src12, blueMask),
				_mm_and_si128(src34, blueMask));

		__m128i blueHi =
			_mm_srli_epi32(blueLow, 16);

		__m128i blue =
			_mm_add_epi32(blueLow, blueHi);

		blue =
			//_mm_mullo_epi16(blue, decay63);    // likely needs work
			//_mm_mul_epu32(blue, decay63x32);
			_mm_mullo_epi32(blue, decay63);

		blue =
			_mm_srli_epi32(blue, 8);

		blue =
			_mm_and_si128(
				blue,
				_mm_set1_epi32(0x0000001F));

		//
		// Pack final RGB565 values.
		//
		__m128i result =
			_mm_or_si128(
				_mm_or_si128(red, green),
				blue);

		alignas(16) dword_t temp[4];

		_mm_store_si128(
			reinterpret_cast<__m128i*>(temp),
			result);

		m_pWriteBuffer16[destPixelOffset + 0] =
			static_cast<word_t>(temp[0]);

		m_pWriteBuffer16[destPixelOffset + 1] =
			static_cast<word_t>(temp[1]);

		m_pWriteBuffer16[destPixelOffset + 2] =
			static_cast<word_t>(temp[2]);

		m_pWriteBuffer16[destPixelOffset + 3] =
			static_cast<word_t>(temp[3]);

		destPixelOffset += 4;
	}

	//
	// Existing scalar tail from Exp2.
	// NOTE: Probably need to add scalar tail handling for odd widths, simply because arbitrary widths are allowed
	// **OR** do I want to add a requirement/restriction enforcing even widths?
	// That is probably better, to enforce some constraints in the name of performance.
	//
	while (destPixelOffset < bufferLength)
	{
		const dword_t pixel =
			lpTransitionTable[destPixelOffset];

		const dword_t pixel3 =
			pixel + m_dwBufferWidth;

		const word_t p1 =
			m_pReadBuffer16[pixel];

		const word_t p2 =
			m_pReadBuffer16[pixel + 1];

		const word_t p3 =
			m_pReadBuffer16[pixel3];

		const word_t p4 =
			m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[destPixelOffset] =
			RGB16(
				(((RFromRGB16(p1) +
					RFromRGB16(p2) +
					RFromRGB16(p3) +
					RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) +
					GFromRGB16(p2) +
					GFromRGB16(p3) +
					GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) +
					BFromRGB16(p2) +
					BFromRGB16(p3) +
					BFromRGB16(p4)) * 63) >> 8));

		++destPixelOffset;
	}
}


//Exp1
//-----
//9.0 sec
//
//Exp2
//-----
//4.4 sec
//Current Champion
//
//Exp3
//-----
//Skipped after disassembly review.
//Compiler already optimized much of the transition-table handling.
//
//Exp4
//-----
//5.0 sec
//Valid
//But slower than Exp2
//
//Conclusion:
//Keep Exp2
//
//Exp5 -- Packed Domain
//-----
//4.5 sec
//Valid
//Roughly the same as Exp2...
// ...
// ...even though the algorithm is completely different:
//Exp2: -- direct vector construction(_mm_set_epi16)
//    Gather WORD pixels
//    Expand to RGB channels
//    SIMD channel math
//    Repack
//
//Exp5: -- Packed-Domain, performing pseudo SIMD operations on two WORDs packed in a DWORD (the idea I used in ~2000/2001)
//    Gather DWORD p1|p2
//    Gather DWORD p3|p4
//    Apply the existing packed-domain algorithm
//    across multiple DWORD lanes at once
//
// RE RUNNING the benchmark, and measuring in milliseconds:
//Exp2: 4522.120117
//Exp5: 4539.915039
//
// That's a difference of 17ms, which is noise. That's fascinating that two completely different approaches are so similar in time.
// They're basically eqivalent, right now. Even though Exp2 tends to be ever so slightly faster in benchmarks, I'm not willing to call it yet.
//
// The question is really, "which one do we think we can optimize more?" Or rather: "continue optimizing/experimenting with BOTH!!"


//Exp2 and Exp5 arrive at almost identical performance using completely different mental models:
//
//Exp2
//Unpack channels
//SIMD channel math
//Repack
//Exp5
//Preserve packed DWORD structure
//SIMD packed-domain math
//
//Yet they converge to almost the same runtime.
//
// Which probably means the bottleneck is probably still gather construction, not arithmetic.
// This packed-domain approach was used in DoDelta_cpp_OPTIMIZED_WITH_DECAY_63 to reduce the memory reads and writes.
// It's interesting that it still works so well in SSE2 code.


// Human created TBD. See if I can do better once I learn from what the AI tries.
void BitCanvas16::DoDelta_SSE2_Intrinsics(const PIXELMAP* lpTransitionTable)
{
	const dword_t bufferLength =
		m_dwBufferWidth * m_dwBufferHeight;

	const __m128i redMask =
		_mm_set1_epi16(_REDMASK16);

	const __m128i greenMask =
		_mm_set1_epi16(_GREENMASK16);

	const __m128i blueMask =
		_mm_set1_epi16(_BLUEMASK16);

	const __m128i decay63 =
		_mm_set1_epi16(63);

	dword_t pixelOffset = 0;

	while (pixelOffset + 7 < bufferLength)
	{
		dword_t p[8];
		dword_t p3[8];

		for (int i = 0; i < 8; ++i)
		{
			p[i] =
				lpTransitionTable[pixelOffset + i];

			p3[i] =
				p[i] + m_dwBufferWidth;
		}

		const __m128i a =
			_mm_set_epi16(
				m_pReadBuffer16[p[7]],
				m_pReadBuffer16[p[6]],
				m_pReadBuffer16[p[5]],
				m_pReadBuffer16[p[4]],
				m_pReadBuffer16[p[3]],
				m_pReadBuffer16[p[2]],
				m_pReadBuffer16[p[1]],
				m_pReadBuffer16[p[0]]);

		const __m128i b =
			_mm_set_epi16(
				m_pReadBuffer16[p[7] + 1],
				m_pReadBuffer16[p[6] + 1],
				m_pReadBuffer16[p[5] + 1],
				m_pReadBuffer16[p[4] + 1],
				m_pReadBuffer16[p[3] + 1],
				m_pReadBuffer16[p[2] + 1],
				m_pReadBuffer16[p[1] + 1],
				m_pReadBuffer16[p[0] + 1]);

		const __m128i c =
			_mm_set_epi16(
				m_pReadBuffer16[p3[7]],
				m_pReadBuffer16[p3[6]],
				m_pReadBuffer16[p3[5]],
				m_pReadBuffer16[p3[4]],
				m_pReadBuffer16[p3[3]],
				m_pReadBuffer16[p3[2]],
				m_pReadBuffer16[p3[1]],
				m_pReadBuffer16[p3[0]]);

		const __m128i d =
			_mm_set_epi16(
				m_pReadBuffer16[p3[7] + 1],
				m_pReadBuffer16[p3[6] + 1],
				m_pReadBuffer16[p3[5] + 1],
				m_pReadBuffer16[p3[4] + 1],
				m_pReadBuffer16[p3[3] + 1],
				m_pReadBuffer16[p3[2] + 1],
				m_pReadBuffer16[p3[1] + 1],
				m_pReadBuffer16[p3[0] + 1]);

		__m128i r =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, redMask),
						REDSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, redMask),
						REDSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(d, redMask),
						REDSHIFT16)));

		__m128i g =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(a, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(b, greenMask),
						GREENSHIFT16)),
				_mm_add_epi16(
					_mm_srli_epi16(
						_mm_and_si128(c, greenMask),
						GREENSHIFT16),
					_mm_srli_epi16(
						_mm_and_si128(d, greenMask),
						GREENSHIFT16)));

		__m128i bl =
			_mm_add_epi16(
				_mm_add_epi16(
					_mm_and_si128(a, blueMask),
					_mm_and_si128(b, blueMask)),
				_mm_add_epi16(
					_mm_and_si128(c, blueMask),
					_mm_and_si128(d, blueMask)));

		r = _mm_srli_epi16(
			_mm_mullo_epi16(r, decay63),
			8);

		g = _mm_srli_epi16(
			_mm_mullo_epi16(g, decay63),
			8);

		bl = _mm_srli_epi16(
			_mm_mullo_epi16(bl, decay63),
			8);

		r = _mm_slli_epi16(r, REDSHIFT16);
		g = _mm_slli_epi16(g, GREENSHIFT16);

		const __m128i result =
			_mm_or_si128(
				_mm_or_si128(r, g),
				bl);

		_mm_storeu_si128(
			reinterpret_cast<__m128i*>(
				&m_pWriteBuffer16[pixelOffset]),
			result);

		pixelOffset += 8;
	}

	//
	// Existing scalar tail from Exp2.
	// NOTE: Probably need to add scalar tail handling for odd widths, simply because arbitrary widths are allowed
	// **OR** do I want to add a requirement/restriction enforcing even widths?
	// That is probably better, to enforce some constraints in the name of performance.
	//
	while (pixelOffset < bufferLength)
	{
		const dword_t pixel =
			lpTransitionTable[pixelOffset];

		const dword_t pixel3 =
			pixel + m_dwBufferWidth;

		const word_t p1 =
			m_pReadBuffer16[pixel];

		const word_t p2 =
			m_pReadBuffer16[pixel + 1];

		const word_t p3 =
			m_pReadBuffer16[pixel3];

		const word_t p4 =
			m_pReadBuffer16[pixel3 + 1];

		m_pWriteBuffer16[pixelOffset] =
			RGB16(
				(((RFromRGB16(p1) +
					RFromRGB16(p2) +
					RFromRGB16(p3) +
					RFromRGB16(p4)) * 63) >> 8),
				(((GFromRGB16(p1) +
					GFromRGB16(p2) +
					GFromRGB16(p3) +
					GFromRGB16(p4)) * 63) >> 8),
				(((BFromRGB16(p1) +
					BFromRGB16(p2) +
					BFromRGB16(p3) +
					BFromRGB16(p4)) * 63) >> 8));

		++pixelOffset;
	}
}

/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas16::CopyTo8(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	word_t* lpBuffer = m_pReadBuffer16;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (
							r16to32(RFromRGB16(*lpBuffer)) +
							g16to32(GFromRGB16(*lpBuffer)) +
							b16to32(BFromRGB16(*lpBuffer++))
							) / 3;
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo16
 *
 ****************************************************************************/
void    BitCanvas16::CopyTo16(void* lpSurface, const dword_t nDeadSpace)
{
	word_t* lpSurface2 = (word_t*)lpSurface;
	word_t* lpBuffer = m_pReadBuffer16;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = *lpBuffer++;
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo24
 *
 ****************************************************************************/
void    BitCanvas16::CopyTo24(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	word_t* lpBuffer = m_pReadBuffer16;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = r16to32(RFromRGB16(*lpBuffer));
			*lpSurface2++ = g16to32(GFromRGB16(*lpBuffer));
			*lpSurface2++ = b16to32(BFromRGB16(*lpBuffer++));
		}

		//skip over the cache area
		lpSurface2 += 3 * nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo32
 *
 ****************************************************************************/
void    BitCanvas16::CopyTo32(void* lpSurface, const dword_t nDeadSpace)
{
	dword_t* lpSurface2 = (dword_t*)lpSurface;
	word_t*  lpBuffer = m_pReadBuffer16;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = RGB32(
								r16to32(RFromRGB16(*lpBuffer)),
								g16to32(GFromRGB16(*lpBuffer)),
								b16to32(BFromRGB16(*lpBuffer++))
								);
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}














/****************************************************************************
 ****************************************************************************
 *
 * BitCanvas32
 *
 ****************************************************************************
 ****************************************************************************/

 /****************************************************************************
  *
  * BitCanvas32
  *
  ****************************************************************************/
BitCanvas32::BitCanvas32(const dword_t dwWidth, const dword_t dwHeight, const TWEENDESCRIPTION& td)
	: BitCanvas(dwWidth, dwHeight, 32, td)
{
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
 //void BitCanvas32::PutPixel(const DWORD x, const DWORD y, const COLORREF color)
 //{
 //    m_pWriteBuffer32[BufferWidth() * y + x] = RGB32( GetRValue(color),
 //                                                     GetGValue(color),
 //                                                     GetBValue(color));
 //}


 /****************************************************************************
  *
  * PutPixel
  *
  ****************************************************************************/
void BitCanvas32::PutPixel(const dword_t pixelOffset, const byte_t color)
{
	m_pWriteBuffer32[pixelOffset] = RGB32(GetRed(color),
		GetGreen(color),
		GetBlue(color));
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas32::PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color)
{
	word_t    i;

	for (i = 0; i < pixel_width; i++)
		m_pWriteBuffer32[pixelOffset + i] = RGB32(GetRed(color),
			GetGreen(color),
			GetBlue(color));
}


/****************************************************************************
 *
 * GetPixel
 *
 ****************************************************************************/
dword_t BitCanvas32::GetPixel(const dword_t pixelOffset)
{
	return m_pReadBuffer32[pixelOffset];
}

// DISPATCH:
void BitCanvas32::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	DoDelta_cpp_BASELINE(lpTransitionTable);
}

#ifdef USE_MMX_INTRINSICS
void BitCanvas32::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	DoDelta_MMX_Intrinsics(lpTransitionTable);
}
#endif

#ifdef USE_MMX_ASSEMBLY
void BitCanvas32::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	DoDelta_MMX_Assembly(lpTransitionTable);
}
#endif

void BitCanvas32::DoDelta_SSE(const PIXELMAP* lpTransitionTable)
{
	DoDelta_SSE2_Intrinsics_FIXED(lpTransitionTable);
}

// IMPLEMENTATIONS:

/****************************************************************************
 * @brief This is a baseline/reference implementation of the straightforward
 * algorithm for 32bit color, unoptimized, other than compiler optimizations.
 *
 * IMPORTANT: This is also the DEFAULT for 32bit color! (NOT DoDelta_x86)
 * This is the implementation used if the processor doesn't support MMX.
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_cpp_BASELINE(const PIXELMAP* lpTransitionTable)
{
	dword_t    pixelOffset = 0;
	dword_t    width;
	dword_t    height;
	dword_t    p1, p2, p3, p4;
	dword_t    pixel, pixel3;

	for (height = 0; height < m_dwBufferHeight; height++)
	{
		for (width = 0; width < m_dwBufferWidth; width++)
		{
			pixel = lpTransitionTable[pixelOffset];
			pixel3 = pixel + m_dwBufferWidth;
			p1 = m_pReadBuffer32[pixel];
			p2 = m_pReadBuffer32[pixel + 1];
			p3 = m_pReadBuffer32[pixel3];
			p4 = m_pReadBuffer32[pixel3 + 1];

			m_pWriteBuffer32[pixelOffset] =
				RGB32(
					((RFromRGB32(p1) + RFromRGB32(p2) + RFromRGB32(p3) + RFromRGB32(p4)) DECAY32),
					((GFromRGB32(p1) + GFromRGB32(p2) + GFromRGB32(p3) + GFromRGB32(p4)) DECAY32),
					((BFromRGB32(p1) + BFromRGB32(p2) + BFromRGB32(p3) + BFromRGB32(p4)) DECAY32)
				);
			pixelOffset++;
		}
	}
}



#ifndef ASM_DECAY
#define ASM_DECAY(d) 0x##d##d##d##d
#endif
#ifndef DECAY_SHIFT
#define DECAY_SHIFT 6  //2^6 - 1 == 003F
// WARNING: Leave this a macro!!! Never blindly accept "replace macro with constexpr" suggestions for anything referenced inside __asm.
// BAD: inline constexpr dword_t  DECAY_SHIFT = 6; //2^6 - 1 == 003F
// BAD! Made the MMX assembly routine look like TV Static!
#endif


#ifdef USE_MMX_INTRINSICS
/****************************************************************************
 *
 * @brief Optimized using MMX intrinsics
 *
 * EXPERIMENTAL, NOT USED by default. Only the MMX or CPP implementations
 * are used in 32bit color, depending on the CPU capabilities.
 *
 * I *think* this was slower than the MMX assembly version, but that measurement
 * was decades ago; what I know for sure is this implementation was to explore
 * and learn to use intrinsics, and compare performance between.
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_MMX_Intrinsics(const PIXELMAP* lpTransitionTable)
{
	const __m64    m_zero = _mm_setzero_si64();
	const __m64    decay_factor = _mm_set1_pi16(0x3F);
	__m64    pixels_12;
	__m64    pixels_34;
	__m64    upper_pixel_sum;
	__m64    lower_pixel_sum;
	__m64    px1;
	__m64    px2;
	__m64    px3;
	__m64    px4;
	__m64    final_pixel;
	const dword_t* readBase = m_pReadBuffer32;
	dword_t* writeBase = m_pWriteBuffer32;
	//    const dword_t    bufferWidth = m_nBufferWidth;
	const dword_t    bufferLength = m_dwBufferWidth * m_dwBufferHeight;
	dword_t  pixelOffset = 0;
	dword_t  pixel;
	dword_t  pixel3;
#pragma message( "hehe" )

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;

		pixels_12 = *((__m64*) & (readBase[pixel]));    /* read in pixels 1 & 2 using a single memory read operation */
		pixels_34 = *((__m64*) & (readBase[pixel3]));   /* read in pixels 3 & 4 using a single memory read operation */

		px1 = _m_punpckhbw(pixels_12, m_zero);  /* unpack pixel 1 */
		px2 = _m_punpcklbw(pixels_12, m_zero);  /* unpack pixel 2 */
		upper_pixel_sum = _m_paddw(px1, px2);   /* sum the top 2 pixels */


		px3 = _m_punpckhbw(pixels_34, m_zero);  /* unpack pixel 3 */
		px4 = _m_punpcklbw(pixels_34, m_zero);  /* unpack pixel 4 */
		lower_pixel_sum = _m_paddw(px3, px4);   /* sum the bottom two pixels */


		final_pixel = _m_paddw(upper_pixel_sum, lower_pixel_sum);    /* sum the 2 sums */

		/* decay the buffer (ie: make it slightly darker than the average) */
		final_pixel = _m_pmullw(final_pixel, decay_factor);
		final_pixel = _m_psrlwi(final_pixel, DECAY_SHIFT + 2);

		final_pixel = _m_packuswb(final_pixel, m_zero);    /* pack the average back into pixel format */


		writeBase[pixelOffset] = _m_to_int(final_pixel);

		pixelOffset++;
	} while (pixelOffset < bufferLength);

	_m_empty();  /* clear the MMX state */
}
#endif


#define buffer_length     eax
#define source_offset     ebx
#define pixel_offset      ecx
#define buffer_width      edx
#define write_base        edi
#define read_base         esi
#define trans_base        esp

#define mmx_zero          mm0
#define mmx_p1            mm1
#define mmx_p2            mm2
#define mmx_p3            mm3
#define mmx_p4            mm4
#define mmx_decay         mm7

#ifdef USE_MMX_ASSEMBLY
/****************************************************************************
 *
 * @brief Optimized using MMX assembly.
 *
 * This is the version used if the processor supports MMX instructions
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_MMX_Assembly(const PIXELMAP* lpTransitionTable)
{
	const   dword_t    widthPixels = m_dwBufferWidth;
	const   dword_t    bufferLength = widthPixels * m_dwBufferHeight;
	dword_t* readBase = m_pReadBuffer32;
	dword_t* writeBase = m_pWriteBuffer32;
	dword_t    espBackup;

	__asm
	{
		mov       espBackup, esp;  /* backup esp */

		mov       eax, 0x003f003f;            /* load the decay factor */
		//        movd      mm1, eax;
		//        movq      mmx_decay, mm1;
		//        punpcklwd mmx_decay, mm1;

		movd      mmx_decay, eax;
		movq      mm1, mmx_decay;
		psllq     mm1, 16;
		por       mmx_decay, mm1;

		pxor      mmx_zero, mmx_zero;

		mov       buffer_length, bufferLength;
		xor pixel_offset, pixel_offset;
		mov       read_base, readBase;
		mov       write_base, writeBase;
		mov       buffer_width, widthPixels;
		mov       trans_base, lpTransitionTable;


	next_pixel:
		mov       source_offset, dword ptr[trans_base + pixel_offset * 4];  /* get the transition pixel offset */

		lea       source_offset, [read_base + source_offset * 4];  /* get the address of pixel 1 */

		movq      mmx_p1, qword ptr[source_offset];    /* get the top two pixels */
		inc       pixel_offset;                                     /* increment pixel offset */
		movq      mmx_p3, qword ptr[source_offset + buffer_width * 4];    /* get lover two pixels */



		movq      mmx_p2, mmx_p1;        /* copy double pixel12 */
		movq      mmx_p4, mmx_p3;        /* copy double pixel34 */

		punpcklbw mmx_p1, mmx_zero;    /* unpack pixel 1 */
		punpckhbw mmx_p2, mmx_zero;    /* unpack pixel 2 */
		paddw     mmx_p1, mmx_p2;      /* sum top two pixels */

		punpcklbw mmx_p3, mmx_zero;    /* unpack pixel 3 */
		punpckhbw mmx_p4, mmx_zero;    /* unpack pixel 4 */
		paddw     mmx_p3, mmx_p4;      /* sum bottom two pixels */

		paddw     mmx_p1, mmx_p3;    //sum top and bottom rows */



		/* decay the buffer (ie: make it slightly darker than the average) */
		pmullw    mmx_p1, mmx_decay;
		psrlw     mmx_p1, DECAY_SHIFT + 2;

		packuswb  mmx_p1, mmx_zero; /* pack the average back into pixel format */



		movd      dword ptr[write_base + pixel_offset * 4 - 4], mmx_p1; /* write average pixel */

		cmp       buffer_length, pixel_offset;
		jne       next_pixel;         /* while bufferLength != pixelOffset */

		mov       esp, espBackup;  /* restore esp */
		emms;                      /* clear the mmx state */

		/* CopyLastLine */
		//mov       esi, write_base;
		//lea       edi, [write_base + buffer_length * 4];
		//lea       ecx, [buffer_width + 1];
		//rep       movsd; /* copy ecx dwords from [esi] to [edi]*/
	}
}
#endif


// Intrinsics Experiment (AI)
// AI implementation, Human found and FIXED the problem!
void BitCanvas32::DoDelta_SSE2_experiment1_FIXED(const PIXELMAP* lpTransitionTable)
{
	const dword_t widthPixels = m_dwBufferWidth;
	const dword_t bufferLength = widthPixels * m_dwBufferHeight;
	const __m128i zero = _mm_setzero_si128(); // 128-bit zero value
	const __m128i decay = _mm_set1_epi16(0x003f);

	for (dword_t pixelOffset = 0;
		pixelOffset < bufferLength;
		++pixelOffset)
	{
		dword_t pixel = lpTransitionTable[pixelOffset];

		dword_t* src = &m_pReadBuffer32[pixel];

		//
		// p1 p2
		//
		const __m128i top = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(src)); // loads 8 bytes from memory into the low 64 bits of an __m128i value and sets the upper 64 bits to zero.

		//
		// p3 p4
		//
		const __m128i bottom = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(src + widthPixels)); // loads 8 bytes from memory into the low 64 bits of an __m128i value and sets the upper 64 bits to zero.

		//
		// unpack to 16-bit channels
		//
		__m128i topLo = _mm_unpacklo_epi8(top, zero); // interleaves the lower 8 bytes of two 128 - bit vectors, alternating bytes from a and b. (a0, b0, a1, b1, a2, b2, a3, b3)
		__m128i topHi = _mm_unpackhi_epi8(top, zero); // interleaves the upper 8 bytes of two 128-bit vectors, alternating bytes from a and b. [a8, b8, a9, b9, …, a15, b15]
		__m128i bottomLo = _mm_unpacklo_epi8(bottom, zero);
		__m128i bottomHi = _mm_unpackhi_epi8(bottom, zero);

		//
		// sum p1+p2
		//
		__m128i sum = _mm_add_epi16(topLo, topHi);

		//
		// sum p3+p4
		//
		sum = _mm_add_epi16(sum, _mm_add_epi16(bottomLo, bottomHi));

		//
		// apply decay
		//
		sum = _mm_mullo_epi16(sum, decay); // multiplies the eight corresponding 16-bit lanes in two 128-bit integer vectors and keeps the low 16 bits of each product.
		sum = _mm_srli_epi16(sum, DECAY_SHIFT + 2); // shifts each of the eight 16-bit lanes in a right by count bits, filling the vacated high bits with zeros. Bits shifted off the right are discarded.

		// Now we need to sum the high/low 64bits of the 128bit pixel_sum_13_24
		// NOTE: this works, but I think would be more accurate to sum BEFORE the decay
		// That said, if this is significantly faster, it may be worth considering loosing a bit of precision for speed.
		// But the main point is that we HAD the data in the original implementatioon, but we discarded the upper 32bits of the lower 64bits.
		__m128i    high_bits_sum = _mm_srli_si128(sum, 8); // shift right 8 bytes
		__m128i    low_bits_sum = _mm_move_epi64(sum); // take the low 64 bits

		sum = _mm_add_epi16(high_bits_sum, low_bits_sum);    /* sum the 2 sums */

		//
		// repack
		//
		__m128i packed = _mm_packus_epi16(sum, zero); // converts the 16 signed 16-bit lanes in a and b into 16 unsigned 8-bit values, saturating each value to the range 0–255.
													// The result’s low 8 bytes come from a; its high 8 bytes come from b.For example, a negative input becomes 0, and a value above 255 becomes 255.


		//
		// store one result pixel
		//
		m_pWriteBuffer32[pixelOffset] = _mm_cvtsi128_si32(packed); // extracts the lowest 32 bits of an __m128i value and returns them as a signed 32-bit integer (int). The other 96 bits are ignored.
	}
}


// Human implementation, Human found and FIXED the problems!
void BitCanvas32::DoDelta_SSE2_Intrinsics_FIXED(const PIXELMAP* lpTransitionTable)
{
	const __m128i    m_zero = _mm_setzero_si128();
	const __m128i    decay_factor = _mm_set1_epi16(0x3F);
	__m128i    pixels_12;
	__m128i    pixels_34;
	__m128i    pixel_sum_13_24;
	__m128i    px12;
	__m128i    px34;
	__m128i    high_bits_sum_13;
	__m128i    low_bits_sum_24;
	__m128i    final_pixel;
	const dword_t* readBase = m_pReadBuffer32;
	dword_t* writeBase = m_pWriteBuffer32;
	//const dword_t    bufferWidth = m_nBufferWidth;
	const dword_t    bufferLength = m_dwBufferWidth * m_dwBufferHeight;
	dword_t  pixelOffset = 0;
	dword_t  pixel;
	dword_t  pixel3;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;

		pixels_12 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&(readBase[pixel])));  /* read in pixels 1 & 2 using a single memory read operation */
		pixels_34 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&(readBase[pixel3]))); /* read in pixels 3 & 4 using a single memory read operation */

		px12 = _mm_unpacklo_epi8(pixels_12, m_zero); /* unpack pixels 12 */
		px34 = _mm_unpacklo_epi8(pixels_34, m_zero); /* unpack pixels 34 */
		pixel_sum_13_24 = _mm_add_epi16(px12, px34); /* sum pixel rows (1+3, and 2+4) */

		// Now we need to sum the high/low 64bits of the 128bit pixel_sum_13_24
		// More accurate to sum BEFORE decay step
		high_bits_sum_13 = _mm_srli_si128(pixel_sum_13_24, 8); // take the high 64bits (shift right 8 bytes)
		low_bits_sum_24 = _mm_move_epi64(pixel_sum_13_24); // take the low 64bits

		final_pixel = _mm_add_epi16(high_bits_sum_13, low_bits_sum_24); /* sum the 2 sums */

		/* decay the buffer (ie: make it slightly darker than the average) */
		// The basic idea is that "decay" is to take 63/64 of the final value of the pixel.
		// Since we already added 4 pixels, we *also* need to divide by 4 as well.
		// We can do this by multiplying by 63 and dividing by 256 to do both divisions at the same time.
		final_pixel = _mm_mullo_epi16(final_pixel, decay_factor);   // multiply by decay_factor
		final_pixel = _mm_srli_epi16(final_pixel, DECAY_SHIFT + 2); // shift right (DECAY_SHIFT + 2) bits

		final_pixel = _mm_packus_epi16(final_pixel, m_zero); /* pack the average back into 32bit pixel format */


		writeBase[pixelOffset] = _mm_cvtsi128_si32(final_pixel); // take the low 32bits

		pixelOffset++;
	} while (pixelOffset < bufferLength);
}


/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas32::CopyTo8(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	dword_t* lpBuffer = m_pReadBuffer32;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (byte_t)((
								RFromRGB32(*lpBuffer) +
								GFromRGB32(*lpBuffer) +
								BFromRGB32(*lpBuffer++)
								) / 3);
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}


/****************************************************************************
 *
 * CopyTo16 - the compiler already optimizes the loop to go backwards
 *
 ****************************************************************************/
void    BitCanvas32::CopyTo16(void* lpSurface, const dword_t nDeadSpace)
{
	word_t* lpSurface2 = (word_t*)lpSurface;
	dword_t* lpBuffer = m_pReadBuffer32;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (word_t)RGB16(
										r32to16(RFromRGB32(*lpBuffer)),
										g32to16(GFromRGB32(*lpBuffer)),
										b32to16(BFromRGB32(*lpBuffer++))
										);
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}


/****************************************************************************
 *
 * CopyTo24
 *
 ****************************************************************************/
void    BitCanvas32::CopyTo24(void* lpSurface, const dword_t nDeadSpace)
{
	byte_t* lpSurface2 = (byte_t*)lpSurface;
	byte_t* lpBuffer = m_pReadBuffer8;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = *lpBuffer++;
			*lpSurface2++ = *lpBuffer++;
			*lpSurface2++ = *lpBuffer++;
			lpBuffer++;
		}

		//skip over the cache area
		lpSurface2 += 3 * nDeadSpace;
	}
}

/****************************************************************************
 *
 * CopyTo32
 *
 ****************************************************************************/
void    BitCanvas32::CopyTo32(void* lpSurface, const dword_t nDeadSpace)
{
	dword_t* lpSurface2 = (dword_t*)lpSurface;
	dword_t* lpBuffer = m_pReadBuffer32;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = *lpBuffer++;
		}

		//skip over the cache area
		lpSurface2 += nDeadSpace;
	}
}

