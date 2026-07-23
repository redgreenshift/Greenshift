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



#include "adetect.h"
#include "BitCanvas.h"
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
void* BitCanvas::GetPixelMemory(DWORD* nWidth, DWORD* nHeight, DWORD* nPixelFormat)
{

	*nWidth = BufferWidth();
	*nHeight = BufferHeight();
	*nPixelFormat = BitDepth() / BITS_PER_BYTE;

	return m_pWriteOnlyBuffer;
}


//#include <complex>
//_INFCODE
//_NANCODE

/****************************************************************************
 *
 * New - creates a new BitCanvas with the specified arguments
 *
 * pre: *outBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    BitCanvas::New( //const DWORD dwWidth,
	//const DWORD dwHeight,
	//const DWORD dwBitDepth,
	BitCanvas** outBitCanvas,
	MyDictionary<char*>* inConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals,
	MyDictionary<value_t*>* inValues)
{
	error_t  err = SUCCESS;
	DWORD    dwWidth;
	DWORD    dwHeight;
	DWORD    dwBitDepth;
	DWORD    dwByteDepth;
	DWORD    dwLCM;
	DWORD    dwDWORDPitch;
	PIXELMAPDESCRIPTION pmd;
	TWEENDESCRIPTION    tween_description;
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

	dwBitDepth = (DWORD)Expression::Evaluate(inConfig->GetValue("canvas_depth"), nBitDepth, inValues, inGlobals);
	nBitDepth = (value_t)dwBitDepth;

	dwWidth = (DWORD)Expression::Evaluate(inConfig->GetValue("canvas_width"), nWidth, inValues, inGlobals);
	nWidth = (value_t)dwWidth;
	if (inValues != NULL)
	{
		pValue = inValues->GetValue("buffer_width", &nWidth);
		if (*pValue < nWidth)
		{
			dwWidth = (DWORD)*pValue;
			nWidth = (value_t)dwWidth;
		}
	}

	dwHeight = (DWORD)Expression::Evaluate(inConfig->GetValue("canvas_height"), nHeight, inValues, inGlobals);
	dwHeight -= dwHeight % 2; /* make the height even */
	nHeight = (value_t)dwHeight;
	if (inValues != NULL)
	{
		pValue = inValues->GetValue("buffer_height", &nHeight);
		if (*pValue < nHeight)
		{
			dwHeight = (DWORD)*pValue;
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

	dwDWORDPitch = dwWidth * dwByteDepth / sizeof(DWORD);


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
BitCanvas::BitCanvas(const DWORD dwWidth,
	const DWORD dwHeight,
	const DWORD dwBitDepth,
	const TWEENDESCRIPTION& tween_description) :
	m_dwBitDepth(dwBitDepth),
	m_dwBufferWidth(dwWidth),
	m_dwBufferHeight(dwHeight),
	m_dwHalfWidth(dwWidth >> 1),
	m_dwHalfHeight(dwHeight >> 1),
	m_hTransitionTable(tween_description),
	m_dwBufferByteLength(dwWidth* dwHeight* dwBitDepth / BITS_PER_BYTE),
	m_dwBufferLength(dwWidth* dwHeight),
	m_dwCacheLength(((dwWidth + 1)* dwBitDepth / BITS_PER_BYTE - 1) / sizeof(DWORD) + 1),
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
		m_nDeltaOptimizationLevel = GetCPUCaps(HAS_MMX) ? OL_MMX : OL_CPP; // OL_x86 has MMX intrinsics
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
	const    DWORD        nBytes_to_Align = 16;
	const    DWORD        nBytes_per_Pixel = BitDepth() / BITS_PER_BYTE;
	const    DWORD        nBuffer_Length = BufferWidth() * BufferHeight();
	const    DWORD        nCache_Size = m_dwCacheLength * sizeof(DWORD);
	const    DWORD        nBuffer_Size = nBuffer_Length * nBytes_per_Pixel
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
	const DWORD nWidth,
	const DWORD nHeight,
	const DWORD nPitch,
	const DWORD nPixelFormat)
{
	const    DWORD    nPixelWidth = nPitch / (nPixelFormat + 1);
	const    DWORD    nDeadSpace = nPixelWidth - m_dwBufferWidth;
	const    DWORD    nStartOffset = (
		nPixelWidth * (nHeight - m_dwBufferHeight) +
		(nWidth - m_dwBufferWidth)
		) >> 1;
	void* lpSurfaceStart = ((BYTE*)lpSurface) +
		nStartOffset * (nPixelFormat + 1);


#ifndef UNDEFINED
	if (BitDepth() == (nPixelFormat + 1) * BITS_PER_BYTE)
	{
		this->CopyToSameBitdepth(lpSurfaceStart, nWidth, nHeight, nPitch);
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
	case OL_SSE:
		this->DoDelta_SSE(lpTransitionTable);
		break;
	default:  /* what should default do? */
		return;
	}
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

/****************************************************************************
 *
 * PlotLineBresenham - plots without the internal multiply
 *
 ****************************************************************************/
void    BitCanvas::PlotLineBresenham(
	long x0,
	long y0,
	const long x1,
	long y1,
	const BYTE color)
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



/****************************************************************************
 *
 * PlotLineBresenham2 - plots without the internal multiply
 *
 ****************************************************************************/
void    BitCanvas::PlotLineBresenhamThick(
	long x0,
	long y0,
	long x1,
	long y1,
	const long line_width,
	const BYTE color)
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


	const long   change_in_x = lX2 - lX1;
	const long   change_in_y = lY2 - lY1;

	const value_t   line_width = line_width_parameter * m_nLineWidthFactor;
	const long half_width = ((long)line_width) >> 1;
	const long start_width = -half_width - (((long)line_width) & 1);

	//    long lWidth;

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

	BYTE    q;

	if (color >= 1.0f)
		q = 255;
	else if (color <= 0.0f)
		q = 0;
	else
		q = (BYTE)(color * 255.0f + 0.5f);


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

			this->PutPixel((lX - ipx), (lY + lpy), (WORD)slice_width, q);
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
BitCanvas8::BitCanvas8(const DWORD dwWidth, const DWORD dwHeight,
	const TWEENDESCRIPTION& td)
	: BitCanvas(dwWidth, dwHeight, 8, td)
{
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas8::PutPixel(const DWORD pixelOffset, const BYTE color)
{
	m_pWriteBuffer8[pixelOffset] = color;
}

/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas8::PutPixel(const DWORD pixelOffset, const WORD pixel_width, const BYTE color)
{
	WORD    i;

	for (i = 0; i < pixel_width; i++)
		m_pWriteBuffer8[pixelOffset + i] = color;
}



/****************************************************************************
 *
 * GetPixel
 *
 ****************************************************************************/
DWORD BitCanvas8::GetPixel(const DWORD pixelOffset)
{
	return m_pReadBuffer8[pixelOffset];
}




/****************************************************************************
 *
 * DoDelta
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	DWORD    pixelOffset = 0;
	DWORD    width;
	DWORD    height;
	DWORD    pixel, pixel3;
	DWORD* srcBuffer;
	DWORD* dstBuffer;
	BYTE    p1, p2, p3, p4;

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
	srcBuffer = (DWORD*)m_pWriteBuffer8;
	dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;
}




/****************************************************************************
 *
 * DoDelta_x86 - experimental merge of CPP and x86
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	DWORD   pixelOffset = 0;
	DWORD   p1, p2;
	DWORD   pixel, pixel3;
	DWORD* srcBuffer;
	DWORD* dstBuffer;
	BYTE* pReadBuffer = m_pReadBuffer8;
	BYTE* pWriteBuffer = m_pWriteBuffer8;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth; // faster than using local, becaus it's already CONST
		pixel = *(WORD*)(&(pReadBuffer[pixel]));
		pixel3 = *(WORD*)(&(pReadBuffer[pixel3]));

		p1 = (pixel & 0x00FF) + (pixel3 & 0x00FF);

		p2 = (pixel & 0xFF00) + (pixel3 & 0xFF00);

		p1 = (p1 + (p2 >> 8)) DECAY8;

		//m_pWriteBuffer8[pixelOffset] = (BYTE)p1;
		pWriteBuffer[pixelOffset] = (BYTE)p1;


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

	/*
	pixelOffset /= sizeof(DWORD);

	for( i = 0; i <= m_nCacheLength; i++ )
	{
		((DWORD*)pWriteBuffer)[pixelOffset] = ((DWORD*)pWriteBuffer)[i];
		pixelOffset++;
	}/**/

	srcBuffer = (DWORD*)pWriteBuffer;
	dstBuffer = (DWORD*)(pWriteBuffer + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;
}


/****************************************************************************
 *
 * DoDelta_MMX - experimental merge of CPP and x86
 *
 ****************************************************************************/
void BitCanvas8::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	DWORD   pixelOffset = 0;
	DWORD   p1, p2;
	DWORD   pixel, pixel3;
	DWORD* srcBuffer;
	DWORD* dstBuffer;
	BYTE* pReadBuffer = m_pReadBuffer8;
	BYTE* pWriteBuffer = m_pWriteBuffer8;

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth; // faster than using local, becaus it's already CONST
		pixel = *(WORD*)(&(pReadBuffer[pixel]));
		pixel3 = *(WORD*)(&(pReadBuffer[pixel3]));

		p1 = (pixel & 0x00FF) + (pixel3 & 0x00FF);

		p2 = (pixel & 0xFF00) + (pixel3 & 0xFF00);

		//        p1 = (p1 + (p2 >> 8)) DECAY8;
		p1 = (p1 + (p2 >> 8)) >> 2;

#if 0
#if EXTREME_DEBUGGING
		/* DEBUG CODE, REMOVE IMMEDIATELY */
		if (p1 > 255)
			DumpToFile("error.txt", p1, "\n");
		//pWriteBuffer[pixelOffset] = (pixelOffset & 0x1) ? 255 : 0;
#endif
#endif


		//m_pWriteBuffer8[pixelOffset] = (BYTE)p1;
		pWriteBuffer[pixelOffset] = decay256[p1];


		pixelOffset++;
	} while (pixelOffset < m_dwBufferByteLength);

	/*
	pixelOffset /= sizeof(DWORD);

	for( i = 0; i <= m_nCacheLength; i++ )
	{
		((DWORD*)pWriteBuffer)[pixelOffset] = ((DWORD*)pWriteBuffer)[i];
		pixelOffset++;
	}/**/

	srcBuffer = (DWORD*)pWriteBuffer;
	dstBuffer = (DWORD*)(pWriteBuffer + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;
}

/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas8::CopyTo8(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	BYTE* lpBuffer = m_pReadBuffer8;


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
void    BitCanvas8::CopyTo16(void* lpSurface, const DWORD nDeadSpace)
{
	WORD* lpSurface2 = (WORD*)lpSurface;
	BYTE* lpBuffer = m_pReadBuffer8;


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
void    BitCanvas8::CopyTo24(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	BYTE* lpBuffer = m_pReadBuffer8;


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
void    BitCanvas8::CopyTo32(void* lpSurface, const DWORD nDeadSpace)
{
	DWORD* lpSurface2 = (DWORD*)lpSurface;
	BYTE* lpBuffer = m_pReadBuffer8;


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
BitCanvas16::BitCanvas16(const DWORD dwWidth, const DWORD dwHeight,
	const TWEENDESCRIPTION& td)
	: BitCanvas(dwWidth, dwHeight, 16, td)
{
}


/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas16::PutPixel(const DWORD pixelOffset, const BYTE color)
{
	m_pWriteBuffer16[pixelOffset] = (WORD)RGB16(r32to16(GetRed(color)),
		g32to16(GetGreen(color)),
		b32to16(GetBlue(color)));
}

/****************************************************************************
 *
 * PutPixel
 *
 ****************************************************************************/
void BitCanvas16::PutPixel(const DWORD pixelOffset, const WORD pixel_width, const BYTE color)
{
	WORD    i;

	for (i = 0; i < pixel_width; i++)
		m_pWriteBuffer16[pixelOffset + i] = (WORD)RGB16(r32to16(GetRed(color)),
			g32to16(GetGreen(color)),
			b32to16(GetBlue(color)));
}



/****************************************************************************
 *
 * GetPixel
 *
 ****************************************************************************/
DWORD BitCanvas16::GetPixel(const DWORD pixelOffset)
{
	return m_pReadBuffer16[pixelOffset];
}




/****************************************************************************
 *
 * DoDelta -
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas16::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	DWORD   pixelOffset = 0;
	DWORD   pixel, pixel3;
	WORD    p1, p2, p3, p4;
	DWORD   bufferLength = m_dwBufferWidth * m_dwBufferHeight;
	DWORD* srcBuffer;
	DWORD* dstBuffer;

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
				((RED16(p1) + RED16(p2) + RED16(p3) + RED16(p4)) DECAY16),
				((GREEN16(p1) + GREEN16(p2) + GREEN16(p3) + GREEN16(p4)) DECAY16),// & ~0x1, //16bit greenshift correction
				((BLUE16(p1) + BLUE16(p2) + BLUE16(p3) + BLUE16(p4)) DECAY16)
			);

		pixelOffset++;
	} while (pixelOffset < bufferLength);

	//    CopyLastLine();
	srcBuffer = (DWORD*)m_pWriteBuffer8;
	dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;

}




/****************************************************************************
 *
 * DoDelta - with decay
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void    BitCanvas16::DoDelta_x86(const PIXELMAP* lpTransitionTable)
{
	DWORD   destPixelOffset = 0;  /* initialize destination pixel offset */
	DWORD   srcPixelOffset, srcPixelOffset3;
	DWORD   srcPixel12, srcPixel34;
	DWORD   redHi, redLow;
	DWORD   greenHi, greenLow;
	DWORD   blueHi, blueLow;
	DWORD   destPixel;
	DWORD* srcBuffer;
	DWORD* dstBuffer;


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
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;   /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(DWORD*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(DWORD*)(&(m_pReadBuffer16[srcPixelOffset3]));

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
		destPixel = (((redLow + redHi) * 511) >> 9) & 0xF800;
		//        destPixel = (destPixel * 255) >> 8;
		//        destPixel = (destPixel * 127) >> 7;
		//        destPixel = (destPixel * 63) >> 6;
		//        destPixel = (destPixel * 15) >> 4;


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
		greenLow = (((srcPixel12 & 0x07E007E0)) +
			((srcPixel34 & 0x07E007E0))) >> 2;
		greenHi = greenLow >> 16;  /* unpack the high order green pixel data */

		/*
		 * sum the hi and low green data,
		 * mask it to the green bits only,
		 * and then or the green bits with the red in the destination pixel
		 */
		 //        destPixel |= (((greenLow + greenHi)*255)>>8) & 0x07E0;
		destPixel |= (((greenLow + greenHi) * 127) >> 7) & 0x07E0;
		//        destPixel |= (((greenLow + greenHi)*63)>>6) & 0x07E0;
		//        destPixel |= (((greenLow + greenHi)*15)>>4) & 0x07E0;


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
		destPixel |= (((blueLow + blueHi) * 511) >> 11) & 0x001F;
		//        destPixel |= (((blueLow + blueHi)*255) >> 10) & 0x001F;
		//        destPixel |= (((blueLow + blueHi)*127) >> 9) & 0x001F;
		//        destPixel |= (((blueLow + blueHi)*63) >> 8) & 0x001F;
		//        destPixel |= (((blueLow + blueHi)*15) >> 6) & 0x001F;



				/*
				 * write the destination pixel data to the destination pixel
				 */
		m_pWriteBuffer16[destPixelOffset] = (WORD)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);


	srcBuffer = (DWORD*)m_pWriteBuffer8;
	dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;


	/*    for( srcPixelOffset = 0;  srcPixelOffset <= m_nBufferWidth;  srcPixelOffset++ )
		{
	//        m_pWriteBuffer16[destPixelOffset+srcPixelOffset] = m_pWriteBuffer16[srcPixelOffset];
			m_pWriteBuffer16[destPixelOffset] = m_pWriteBuffer16[srcPixelOffset];
			destPixelOffset++;
		}
	/**/
}


/****************************************************************************
 *
 * DoDelta - no decay
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas16::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	DWORD   destPixelOffset = 0;  /* initialize destination pixel offset */
	DWORD   srcPixelOffset, srcPixelOffset3;
	DWORD   srcPixel12, srcPixel34;
	DWORD   redHi, redLow;
	DWORD   greenHi, greenLow;
	DWORD   blueHi, blueLow;
	DWORD   destPixel;
	DWORD* srcBuffer;
	DWORD* dstBuffer;


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
		srcPixelOffset3 = srcPixelOffset + m_dwBufferWidth;   /* p3 offset */

		/* get the packed p1p2 and p3p4 values */
		srcPixel12 = *(DWORD*)(&(m_pReadBuffer16[srcPixelOffset]));
		srcPixel34 = *(DWORD*)(&(m_pReadBuffer16[srcPixelOffset3]));

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
		m_pWriteBuffer16[destPixelOffset] = (WORD)destPixel;


		/* progress to the next pixel */
		destPixelOffset++;
	} while (destPixelOffset < m_dwBufferLength);


	srcBuffer = (DWORD*)m_pWriteBuffer8;
	dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;

	/*    for( srcPixelOffset = 0;  srcPixelOffset <= m_nBufferWidth;  srcPixelOffset++ )
		{
	//        m_pWriteBuffer16[destPixelOffset+srcPixelOffset] = m_pWriteBuffer16[srcPixelOffset];
			m_pWriteBuffer16[destPixelOffset] = m_pWriteBuffer16[srcPixelOffset];
			destPixelOffset++;
		}
	/**/
}


/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas16::CopyTo8(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	WORD* lpBuffer = m_pReadBuffer16;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (
							r16to32(RED16(*lpBuffer)) +
							g16to32(GREEN16(*lpBuffer)) +
							b16to32(BLUE16(*lpBuffer++))
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
void    BitCanvas16::CopyTo16(void* lpSurface, const DWORD nDeadSpace)
{
	WORD* lpSurface2 = (WORD*)lpSurface;
	WORD* lpBuffer = m_pReadBuffer16;



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
void    BitCanvas16::CopyTo24(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	WORD* lpBuffer = m_pReadBuffer16;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = r16to32(RED16(*lpBuffer));
			*lpSurface2++ = g16to32(GREEN16(*lpBuffer));
			*lpSurface2++ = b16to32(BLUE16(*lpBuffer++));
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
void    BitCanvas16::CopyTo32(void* lpSurface, const DWORD nDeadSpace)
{
	DWORD* lpSurface2 = (DWORD*)lpSurface;
	WORD* lpBuffer = m_pReadBuffer16;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = RGB32(
								r16to32(RED16(*lpBuffer)),
								g16to32(GREEN16(*lpBuffer)),
								b16to32(BLUE16(*lpBuffer++))
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
BitCanvas32::BitCanvas32(const DWORD dwWidth, const DWORD dwHeight, const TWEENDESCRIPTION& td)
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
void BitCanvas32::PutPixel(const DWORD pixelOffset, const BYTE color)
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
void BitCanvas32::PutPixel(const DWORD pixelOffset, const WORD pixel_width, const BYTE color)
{
	WORD    i;

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
DWORD BitCanvas32::GetPixel(const DWORD pixelOffset)
{
	return m_pReadBuffer32[pixelOffset];
}


/****************************************************************************
 *
 * DoDelta
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_cpp(const PIXELMAP* lpTransitionTable)
{
	DWORD    pixelOffset = 0;
	DWORD    width;
	DWORD    height;
	DWORD    p1, p2, p3, p4;
	DWORD    pixel, pixel3;

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
					((RED32(p1) + RED32(p2) + RED32(p3) + RED32(p4)) DECAY8),
					((GREEN32(p1) + GREEN32(p2) + GREEN32(p3) + GREEN32(p4)) DECAY8),
					((BLUE32(p1) + BLUE32(p2) + BLUE32(p3) + BLUE32(p4)) DECAY8)
				);
			pixelOffset++;
		}
	}

	for (DWORD i = 0; i <= m_dwBufferWidth; i++)
	{
		m_pWriteBuffer32[pixelOffset] = m_pWriteBuffer32[i];
		pixelOffset++;
	}

}



#ifndef ASM_DECAY
#define ASM_DECAY(d) 0x##d##d##d##d
#endif
#ifndef DECAY_SHIFT
#define DECAY_SHIFT 6  //2^6 - 1 == 003F
#endif


#ifdef USE_MMX_INTRINSICS
/****************************************************************************
 *
 * DoDelta_x86
 *
 * pre: delta fields are precalculated
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_x86(const PIXELMAP* lpTransitionTable)
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
	const DWORD* readBase = m_pReadBuffer32;
	DWORD* writeBase = m_pWriteBuffer32;
	//    const DWORD    bufferWidth = m_nBufferWidth;
	const DWORD    bufferLength = m_dwBufferWidth * m_dwBufferHeight;
	DWORD    pixelOffset = 0;
	DWORD    pixel;
	DWORD    pixel3;
	DWORD* srcBuffer;
	DWORD* dstBuffer;
#pragma message( "hehe" )

	do
	{
		pixel = lpTransitionTable[pixelOffset];
		pixel3 = pixel + m_dwBufferWidth;

		pixels_12 = *((__m64*) & (readBase[pixel]));    /* read in pixels 1 & 2 using a single memory read operation */
		pixels_34 = *((__m64*) & (readBase[pixel3]));    /* read in pixels 3 & 4 using a single memory read operation */

		px1 = _m_punpckhbw(pixels_12, m_zero);    /* unpack pixel 1 */
		px2 = _m_punpcklbw(pixels_12, m_zero);  /* unpack pixel 1 */
		upper_pixel_sum = _m_paddw(px1, px2);    /* sum the top 2 pixels */


		px3 = _m_punpckhbw(pixels_34, m_zero);  /* unpack pixel 3 */
		px4 = _m_punpcklbw(pixels_34, m_zero);  /* unpack pixel 4 */
		lower_pixel_sum = _m_paddw(px3, px4);    /* sum the bottom two pixels */


		final_pixel = _m_paddw(upper_pixel_sum, lower_pixel_sum);    /* sum the 2 sums */

		/* decay the buffer (ie: make it slightly darker than the average) */
		final_pixel = _m_pmullw(final_pixel, decay_factor);
		final_pixel = _m_psrlwi(final_pixel, DECAY_SHIFT + 2);

		final_pixel = _m_packuswb(final_pixel, m_zero);    /* pack the average back into pixel format */


		writeBase[pixelOffset] = _m_to_int(final_pixel);

		pixelOffset++;
	} while (pixelOffset < bufferLength);

	//    CopyLastLine();

	srcBuffer = (DWORD*)m_pWriteBuffer8;
	dstBuffer = (DWORD*)(m_pWriteBuffer8 + m_dwBufferByteLength);

	for (DWORD i = m_dwCacheLength; i != 0; i--)
		*dstBuffer++ = *srcBuffer++;


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
 * DoDelta_MMX
 *
 ****************************************************************************/
void BitCanvas32::DoDelta_MMX(const PIXELMAP* lpTransitionTable)
{
	const   DWORD    widthPixels = m_dwBufferWidth;
	const   DWORD    bufferLength = widthPixels * m_dwBufferHeight;
	DWORD* readBase = m_pReadBuffer32;
	DWORD* writeBase = m_pWriteBuffer32;
	DWORD    espBackup;

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
		paddw     mmx_p1, mmx_p2;    /* sum top two pixels */

		punpcklbw mmx_p3, mmx_zero;    /* unpack pixel 3 */
		punpckhbw mmx_p4, mmx_zero;    /* unpack pixel 4 */
		paddw     mmx_p3, mmx_p4;    /* sum bottom two pixels */

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
		mov       esi, write_base;
		lea       edi, [write_base + buffer_length * 4];
		lea       ecx, [buffer_width + 1];
		rep       movsd; /* copy ecx dwords from [esi] to [edi]*/
	}
}
#endif

/****************************************************************************
 *
 * CopyTo8
 *
 ****************************************************************************/
void    BitCanvas32::CopyTo8(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	DWORD* lpBuffer = m_pReadBuffer32;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (BYTE)((
								RED32(*lpBuffer) +
								GREEN32(*lpBuffer) +
								BLUE32(*lpBuffer++)
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
void    BitCanvas32::CopyTo16(void* lpSurface, const DWORD nDeadSpace)
{
	WORD* lpSurface2 = (WORD*)lpSurface;
	DWORD* lpBuffer = m_pReadBuffer32;


	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = (WORD)RGB16(
										r32to16(RED32(*lpBuffer)),
										g32to16(GREEN32(*lpBuffer)),
										b32to16(BLUE32(*lpBuffer++))
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
void    BitCanvas32::CopyTo24(void* lpSurface, const DWORD nDeadSpace)
{
	BYTE* lpSurface2 = (BYTE*)lpSurface;
	BYTE* lpBuffer = m_pReadBuffer8;


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
void    BitCanvas32::CopyTo32(void* lpSurface, const DWORD nDeadSpace)
{
	DWORD* lpSurface2 = (DWORD*)lpSurface;
	DWORD* lpBuffer = m_pReadBuffer32;

	COPYTO_HEIGHT_LOOP
	{
		COPYTO_WIDTH_LOOP
		{
			//assign the value to the surface
			*lpSurface2++ = *lpBuffer++;
		}

		//skip over the cache area
		(DWORD*)lpSurface2 += nDeadSpace;
	}
}

