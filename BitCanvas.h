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
  * BitCanvas - internal representation of screen data
  *
  ****************************************************************************/

#define USE_MMX_INTRINSICS  1
#define USE_MMX_ASSEMBLY    1

#include "ColorUtils.hpp"
#include "DeltaField.h"
#include "DeltaTween.h"
#include "Expression.h"
#include "Interval.h"
#include "MyDictionary.h"
#include "Palette.h"
#include "TweenThread.h"


//#define MAX_LINE_WIDTH  32
//#define MAX_LINE_WIDTH_FLOAT  MAX_LINE_WIDTH##.0f

/****************************************************************************
 *
 * begin defines for BitCanvas
 *
 ****************************************************************************/

 //#define FORCE_LINE_WIDTH_TO_1 1

 //#define USE_BITMAP 1
 //#define USE_DOUBLE_BITMAP 1
 //#define BOTTOM_UP_DIB 1
#define TOP_DOWN_DIB 1
/*
 * bit depth conversion macros
 */

 /*
  * 16 bit to 32 bit
  */
#define _5_to_8( px )  ((px) << 3)
#define _6_to_8( px )  ((px) << 2)
#define r16to32( px )  (_5_to_8( px ))
#define g16to32( px )  (_6_to_8( px ))
#define b16to32( px )  (_5_to_8( px ))

  /*
   * 32 bit to 16 bit
   */
#define _8_to_5( px )  (((px) >> 3) & 0x1F)
#define _8_to_6( px )  (((px) >> 2) & 0x3F)
#define r32to16( px )  (_8_to_5( px ))
#define g32to16( px )  (_8_to_6( px ))
#define b32to16( px )  (_8_to_5( px ))


   //#define DECAY8 * 31 >> 7
   //#define DECAY8 * 63 >> 8
#define DECAY8 * 127 >> 9
//#define DECAY8 * 255 >> 10
//#define DECAY8 * 511 >> 11
//#define DECAY8 * 1023 >> 12  /* decay is too slow */
//#define DECAY8 * 0xFFFFFE >> 26

#define DECAY256( x ) (((x) * 127) >> 7 )


//#define DECAY16 * 31 >> 7
//#define DECAY16 * 255 >> 10
//#define DECAY16 * 511 >> 11
//#define DECAY16 * 0xFFFFFE >> 26
#define DECAY16 >> 2


//#define DECAY32 * 31 >> 7
//#define DECAY32 * 63 >> 8
#define DECAY32 * 127 >> 9
//#define DECAY32 * 255 >> 10
//#define DECAY32 * 511 >> 11
//#define DECAY32 * 1023 >> 12  /* decay is too slow */
//#define DECAY32 * 0xFFFFFE >> 26


#define COPYTO_HEIGHT_LOOP for( dword_t j = m_dwBufferHeight; j != 0; --j )
#define COPYTO_WIDTH_LOOP  for( dword_t i = m_dwBufferWidth;  i != 0; --i )

/****************************************************************************
 *
 * end defines
 *
 ****************************************************************************/

#define DECAY256_TENS( x ) \
DECAY256( x##0 ),\
DECAY256( x##1 ),\
DECAY256( x##2 ),\
DECAY256( x##3 ),\
DECAY256( x##4 ),\
DECAY256( x##5 ),\
DECAY256( x##6 ),\
DECAY256( x##7 ),\
DECAY256( x##8 ),\
DECAY256( x##9 ),


static const byte_t decay256[256] = {
DECAY256_TENS(0x)
DECAY256_TENS(1)
DECAY256_TENS(2)
DECAY256_TENS(3)
DECAY256_TENS(4)
DECAY256_TENS(5)
DECAY256_TENS(6)
DECAY256_TENS(7)
DECAY256_TENS(8)
DECAY256_TENS(9)
DECAY256_TENS(10)
DECAY256_TENS(11)
DECAY256_TENS(12)
DECAY256_TENS(13)
DECAY256_TENS(14)
DECAY256_TENS(15)
DECAY256_TENS(16)
DECAY256_TENS(17)
DECAY256_TENS(18)
DECAY256_TENS(19)
DECAY256_TENS(20)
DECAY256_TENS(21)
DECAY256_TENS(22)
DECAY256_TENS(23)
DECAY256_TENS(24)
DECAY256(250),
DECAY256(251),
DECAY256(252),
DECAY256(253),
DECAY256(254),
DECAY256(255)
};
/*
10 DECAY8,
11 DECAY8,
12 DECAY8,
13 DECAY8,
14 DECAY8,
15 DECAY8,
16 DECAY8,
17 DECAY8,
18 DECAY8,
19 DECAY8,
20 DECAY8,
21 DECAY8,
22 DECAY8,
23 DECAY8,
24 DECAY8,
25 DECAY8,
26 DECAY8,
27 DECAY8,
28 DECAY8,
29 DECAY8,
30 DECAY8,
31 DECAY8,
32 DECAY8,
33 DECAY8,
34 DECAY8,
35 DECAY8,
36 DECAY8,
37 DECAY8,
38 DECAY8,
39 DECAY8,
40 DECAY8,
41 DECAY8,
42 DECAY8,
43 DECAY8,
44 DECAY8,
45 DECAY8,
46 DECAY8,
47 DECAY8,
48 DECAY8,
49 DECAY8,
50 DECAY8,
51 DECAY8,
52 DECAY8,
53 DECAY8,
54 DECAY8,
55 DECAY8,
56 DECAY8,
57 DECAY8,
58 DECAY8,
59 DECAY8,
60 DECAY8,
61 DECAY8,
62 DECAY8,
63 DECAY8,
64 DECAY8,
65 DECAY8,
66 DECAY8,
67 DECAY8,
68 DECAY8,
69 DECAY8,
70 DECAY8,
71 DECAY8,
72 DECAY8,
73 DECAY8,
74 DECAY8,
75 DECAY8,
76 DECAY8,
77 DECAY8,
78 DECAY8,
79 DECAY8,
80 DECAY8,
81 DECAY8,
82 DECAY8,
83 DECAY8,
84 DECAY8,
85 DECAY8,
86 DECAY8,
87 DECAY8,
88 DECAY8,
89 DECAY8,
90 DECAY8,
91 DECAY8,
92 DECAY8,
93 DECAY8,
94 DECAY8,
95 DECAY8,
96 DECAY8,
97 DECAY8,
98 DECAY8,
99 DECAY8,
100 DECAY8,
1 DECAY8,
2 DECAY8,
3 DECAY8,
4 DECAY8,
5 DECAY8,
6 DECAY8,
7 DECAY8,
8 DECAY8,
9 DECAY8,
 };/**/


 /****************************************************************************
  * @brief BitCanvas is a raster graphics surface that stores pixel data and provides low-level drawing operations.
  *
  * Neither a screen nor a device, BitCanvas is the target for a 2D-raster-graphics-renderer.
  * It serves as a sink for the processes of geometry-driven-visuals, facilitating smooth,
  * delta-based transitions between frames.
  *
  * Designed for efficient, high-performance manipulation, it provides specialized
  * handling of various bit depths (8, 16, or 32 bits) as the final buffer for
  * the visual rendering pipeline.
  ****************************************************************************/
class BitCanvas
{
private:
	value_t     m_nDrawingAspect;
	value_t     m_nWidthFactor;
	value_t     m_nHeightFactor;
	//value_t     m_nWidthFactorInverse;
	//value_t     m_nHeightFactorInverse;

	//value_t     m_nWidthOffset;
	//value_t     m_nHeightOffset;


public:
	/*
	 * Optimization Level Type
	 */
	typedef enum { OL_CPP, OL_x86, OL_MMX, OL_SSE2 } opt_level_t;


	BitCanvas(const dword_t dwWidth,
		const dword_t dwHeight,
		const dword_t dwBitDepth,
		const TWEENDESCRIPTION& tween_description);
	virtual ~BitCanvas();
	// BitCanvas owns substantial pixel memory.
	// Copy and move semantics are intentionally disabled.
	BitCanvas(const BitCanvas&) = delete; // no copy ctor
	BitCanvas& operator=(const BitCanvas&) = delete; // no assignment
	BitCanvas(BitCanvas&&) = delete; // no move ctor
	BitCanvas& operator=(BitCanvas&&) = delete; // no move assignment

	/*
	 * instance creation
	 */
	static error_t  New(BitCanvas** outBitCanvas,
		MyDictionary<mychar_t*>* inConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals,
		MyDictionary<value_t*>* inValues);

	error_t         Initialize(void);

	/*
	 * change the optimization level
	 */
	void            SetOptimizationLevel(const opt_level_t enumOptLevel);
	opt_level_t		GetOptimizationLevel() const { return m_nDeltaOptimizationLevel; };

	/*
	 * accessors
	 *
	 * see above comment for corresponding variable
	 */
	inline dword_t    BufferWidth(void) const { return m_dwBufferWidth; };
	inline dword_t    BufferHeight(void) const { return m_dwBufferHeight; };
	inline dword_t    BitDepth(void) const { return m_dwBitDepth; };

	/*
	 * swap the buffer pointers
	 */
	void FlipBuffers(void);

	void FramerateWarning(void)
	{
		m_hTransitionTable.m_bFramerateTooLow = true;
	};
	error_t SetDelta(DeltaField* pDelta);
	void    DoDelta(void)
	{
		DoDelta(m_hTransitionTable.GetPixelMap());
	};

	void    SetDrawingAspect(value_t nAspect, const bool bZoom, const value_t nAspect2, const bool bZoom2, const value_t nPercent);
	void    SetDrawingAspect(value_t nAspect, const bool bZoom);

protected:
	/*
	 * apply the pixel transition
	 */
	void            DoDelta(const PIXELMAP* lpTransitionTable);
	/*
	 * copy buffer contents to
	 * the alternate pixel medium
	 * perfoming bit depth
	 * conversion if necessary
	 */
public:
#ifdef USE_BITMAP
	void            CopyTo(const DWORD nWidth,
		const DWORD nHeight,
		HDC hDC);
#endif
	void            CopyTo(void* lpSurface,
		const dword_t   nWidth,
		const dword_t   nHeight,
		const dword_t   nPitch,
		const dword_t   nPixelFormat);
protected:
	/// <summary>
	/// Copies pixel data from the object's internal 32-bit read buffer to the
	/// provided surface buffer assuming the same bit depth and buffer dimensions;
	/// skips surface padding per-line using the provided pitch.
	/// </summary>
	/// <param name="lpSurface">Pointer to the destination surface memory where pixel data will be written (treated as DWORD*).</param>
	/// <param name="dwPitch">Surface scanline pitch in bytes (total bytes between starts of consecutive rows).</param>
	void            CopyToSameBitdepth(void* lpSurface,
		const dword_t   dwPitch)
		//const dword_t   dwPixelFormat )
	{
		static const dword_t dwBufferDWORDPitch = m_dwBufferWidth * BitDepth() / BITS_PER_BYTE / sizeof(dword_t);
		const dword_t dwSurfaceDWORDPitch = dwPitch / sizeof(dword_t);
		const dword_t dwDeadSpace = dwSurfaceDWORDPitch - dwBufferDWORDPitch;
		dword_t* lpSurface2 = (dword_t*)lpSurface;
		dword_t* lpBuffer = m_pReadBuffer32;


		for (dword_t j = m_dwBufferHeight; j != 0; j--)
		{
			//for( i = 0;  i < dwBufferDWORDPitch;  i++ )
			for (dword_t i = dwBufferDWORDPitch; i != 0; i--)
			{
				//assign the value to the surface
				*lpSurface2++ = *lpBuffer++;
			}

			//skip over the cache area
			lpSurface2 += dwDeadSpace;
		}
	};

	// NOTE: Yes, m_pReadOnlyBuffer is correct in all cases. It's an alias for whatever buffer we're using for the current bit depth
	inline void SynchronizeReadBufferCache() { CopyFirstRowToCache((byte_t*)m_pReadOnlyBuffer); }
	// NOTE: Yes, m_pWriteOnlyBuffer is correct in all cases. It's an alias for whatever buffer we're using for the current bit depth
	inline void SynchronizeWriteBufferCache() { CopyFirstRowToCache((byte_t*)m_pWriteOnlyBuffer); }
	inline void CopyFirstRowToCache(byte_t* pBuffer)
	{
		dword_t* srcBuffer = (dword_t*)pBuffer;
		dword_t* dstBuffer = (dword_t*)(pBuffer + m_dwBufferByteLength);

		for (dword_t i = m_dwCacheLength; i != 0; --i)
			*dstBuffer++ = *srcBuffer++;
	}

public:
	inline void PrepareForDoDelta()
	{
		SynchronizeReadBufferCache();
	}

	inline void FinalizeDoDelta()
	{
		SynchronizeWriteBufferCache();
	}
	//inline void     CopyLastLine( void );

	/*
	 * line drawing routine (only worry about 2D!)
	 */
	void DrawLine(const value_t x1,
		const value_t y1,
		const value_t x2,
		const value_t y2,
		const value_t line_width_parameter,
		const value_t color);
	void DrawDot(const value_t x,
		const value_t y,
		const value_t dot_size,
		const value_t color);

protected:
	void PlotLineBresenham(
		long x0,
		long y0,
		const long x1,
		long y1,
		const byte_t color);
	void PlotLineBresenhamThick(
		long x0,
		long y0,
		long x1,
		long y1,
		const long line_width,
		const byte_t color);
	void PlotDot(const value_t x,
		const value_t y,
		const value_t dot_size,
		const value_t color);
public:

	error_t         SetPalette(Palette* pPalette);
	error_t         SetPalette(Palette* pPalette1,
		Palette* pPalette2,
		const value_t nPercent);

	/****************************************************************************
	 *
	 * GetPalette - return the logical palette so a device context can use it
	 *
	 ****************************************************************************/
	inline
		logpalette_t* PGetPalette(void)
	{
		return m_pPalette.PGetLogicalPalette();
	}



#if UNDEFINED
	/*
	 * TODO: JRDV: Why did I define BlahPutPixel? there is a comment "hack for graphics project"
	 * AHA! That was for CS4451a_project_3, so this was intended to be temporary, and I can clean it up
	 */
	inline void     BlahPutPixel(const long x, const long y,
		const byte_t r, const byte_t g, const byte_t b)
	{
		const dword_t    pixelOffset = BufferWidth() * y + x;

		if (y >= 0 &&/**/ y < (long)BufferHeight() && x >= 0 &&/**/ x < (long)BufferWidth())
			BlahPutPixel(pixelOffset, r, g, b);
	};
#endif
	inline void     PutPixel(const long x, const long y,
		const byte_t color)
	{
		const dword_t    pixelOffset = BufferWidth() * y + x;

		if (y >= 0 &&/**/ y < (long)BufferHeight() && x >= 0 &&/**/ x < (long)BufferWidth())
			PutPixel(pixelOffset, color);
	};
	inline void     PutPixel(const long x, const long y,
		const word_t pixel_width, const byte_t color)
	{
		const dword_t    pixelOffset = BufferWidth() * y + x;

		//if( y >= 0 &&/**/ y < (long)BufferHeight() && x >= 0 &&/**/ x + pixel_width < (long)BufferWidth() )
		if (y >= 0 &&/**/ y < (long)BufferHeight() && x >= 0 &&/**/ x < (long)BufferWidth())
		{
			const long wid = pixel_width - (long)BufferWidth() + x;
			if (wid <= 0)
				PutPixel(pixelOffset, pixel_width, color);
			else
				PutPixel(pixelOffset, (word_t)wid - 1, color);
		}
	};
//	inline DWORD    GetPixel(const long x, const long y);


	/*
	 * hack for OpenGL.... it needs direct access...
	 */
	void* GetPixelMemory(dword_t* nWidth, dword_t* nHeight, dword_t* nPixelFormat);

#if !UNIT_TEST
protected:
#endif
	/************************************************************************
	 *
	 * the following functions should be defined in derived classes
	 *
	 ************************************************************************/
	inline virtual dword_t    GetPixel(const dword_t pixelOffset) = 0;
	inline virtual void     PutPixel(const dword_t pixelOffset,
		const byte_t color) = 0;
	inline virtual void     BlahPutPixel(const dword_t pixelOffset,
		const byte_t r,
		const byte_t g,
		const byte_t b) = 0;
	inline virtual void     PutPixel(const dword_t pixelOffset,
		const word_t pixel_width,
		const byte_t color) = 0;

	virtual void    DoDelta_cpp(const PIXELMAP* lpTransitionTable) = 0;
	virtual void    DoDelta_x86(const PIXELMAP* lpTransitionTable);
	virtual void    DoDelta_MMX(const PIXELMAP* lpTransitionTable);
	virtual void    DoDelta_SSE(const PIXELMAP* lpTransitionTable);
	virtual void    CopyTo8(void* lpSurface, const dword_t nDeadSpace) = 0;
	virtual void    CopyTo16(void* lpSurface, const dword_t nDeadSpace) = 0;
	virtual void    CopyTo24(void* lpSurface, const dword_t nDeadSpace) = 0;
	virtual void    CopyTo32(void* lpSurface, const dword_t nDeadSpace) = 0;




#ifndef UNDEFINED
	/****************************************************************************
	 *
	 * GetRed -
	 *
	 ****************************************************************************/
	inline byte_t     GetRed(const byte_t nIndex)
	{
		return PGetPalette()->palPalEntry[nIndex].peRed;
	};


	/****************************************************************************
	 *
	 * GetGreen -
	 *
	 ****************************************************************************/
	inline byte_t     GetGreen(const byte_t nIndex)
	{
		return PGetPalette()->palPalEntry[nIndex].peGreen;
	};


	/****************************************************************************
	 *
	 * GetBlue -
	 *
	 ****************************************************************************/
	inline byte_t     GetBlue(const byte_t nIndex)
	{
		return PGetPalette()->palPalEntry[nIndex].peBlue;
	};
#else

	/****************************************************************************
	 *
	 * GetRed -
	 *
	 ****************************************************************************/
	inline byte_t     GetRed(const byte_t nIndex)
	{
		return m_pPaletteEntry[nIndex].peRed;
	};


	/****************************************************************************
	 *
	 * GetGreen -
	 *
	 ****************************************************************************/
	inline byte_t     GetGreen(const byte_t nIndex)
	{
		return m_pPaletteEntry[nIndex].peGreen;
	};


	/****************************************************************************
	 *
	 * GetBlue -
	 *
	 ****************************************************************************/
	inline byte_t     GetBlue(const byte_t nIndex)
	{
		return m_pPaletteEntry[nIndex].peBlue;
	};

#ifdef UNDEFINED
	/****************************************************************************
	 *
	 * GetEntry -
	 *
	 ****************************************************************************/
	inline paletteentry_t GetEntry(const byte_t nIndex)
	{
		return m_pPaletteEntry[nIndex];
	};

	/****************************************************************************
	 *
	 * GetRGB -
	 *
	 ****************************************************************************/
	inline void GetRGB(const BYTE nIndex, BYTE* outRed, BYTE* outGreen, BYTE* outBlue)
	{
		const paletteentry_t  tmpPE = m_pPaletteEntry[nIndex];

		*outRed = tmpPE.peRed;
		*outGreen = tmpPE.peGreen;
		*outBlue = tmpPE.peBlue;
	};
#endif
#endif


private:
	opt_level_t		m_nDeltaOptimizationLevel;
	Interval		m_logicalX;
	Interval		m_logicalY;
	Interval		m_visibleX;
	Interval		m_visibleY;
	DeltaField*		m_pDelta;
	DeltaTween		m_hTransitionTable;

	void* m_pUnalignedDoubleBuffer;
	void ReleaseBuffers(void);    /* free allocated memory */

protected:

	const dword_t	m_dwBitDepth;    /* number of bits per pixel
									 */
	const dword_t	m_dwBufferWidth; /* the width in pixels
									 * of ReadOnlyBuffer and WriteOnlyBuffer
									 */
	const dword_t	m_dwBufferHeight;/* the height in pixels
									 * of ReadOnlyBuffer and WriteOnlyBuffer
									 */
	const dword_t	m_dwCacheLength; /* the number of DWORD's allocated past
									 * the end of the buffer.  The extra
									 * space is used to cache the first line
									 * of pixels.
									 */
									 /* Width * Height + CacheLength should
									  * also be an even multiple of DQWORD
									  */
	const dword_t	m_dwBufferByteLength;
	const dword_t	m_dwBufferLength;

	const dword_t	m_dwHalfWidth;
	const dword_t	m_dwHalfHeight;

	const value_t   m_nLineWidthFactor;


	union {
		void* m_pWriteOnlyBuffer;/* the frame currently being drawn
								  */
		byte_t* m_pWriteBuffer8;   /* accessor for BitCanvas8  */
		word_t* m_pWriteBuffer16;  /* accessor for BitCanvas16 */
		dword_t* m_pWriteBuffer32; /* accessor for BitCanvas32 */
	};

	union {
		void* m_pReadOnlyBuffer; /* the completed frame
								  */
		byte_t* m_pReadBuffer8;    /* accessor for BitCanvas8  */
		word_t* m_pReadBuffer16;   /* accessor for BitCanvas16 */
		dword_t* m_pReadBuffer32;  /* accessor for BitCanvas32 */
	};

	Palette         m_pPalette;
	//paletteentry_t    *m_pPaletteEntry; /* accessor to static buffer in m_pPalette */


#ifdef USE_BITMAP
	HDC             m_hDC;
#ifdef USE_DOUBLE_BITMAP
	BITMAPINFO      m_hDoubleBitmapInfo;
	HBITMAP         m_hDoubleBitmap;
#else // Double Bitmap
	BITMAPINFO      m_hReadOnlyBitmapInfo;
	BITMAPINFO      m_hWriteOnlyBitmapInfo;
	HBITMAP         m_hWriteOnlyBitmap;
	HBITMAP         m_hReadOnlyBitmap;
#endif // Double Bitmap
#endif // Use Bitmap
};



class BitCanvas8 : public BitCanvas
{
public:
	BitCanvas8(const dword_t dwWidth, const dword_t dwHeight, const TWEENDESCRIPTION& td);

protected:
	/*
	 * removed inline next to Get/PutPixel since it's in the base class
	 */
	virtual dword_t           GetPixel(const dword_t pixelOffset) override;
	virtual void            PutPixel(const dword_t pixelOffset, const byte_t color) override;
	virtual void            PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color) override;
	virtual void     BlahPutPixel(const dword_t pixelOffset,
		const byte_t r,
		const byte_t g,
		const byte_t b) override
	{   /* hack for graphics project */
		m_pWriteBuffer8[pixelOffset] = (unsigned char)RGB32(r, g, b);
	};

	// DISPATCH:
	virtual void            DoDelta_cpp(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_x86(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_MMX(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_SSE( const PIXELMAP *lpTransitionTable ) override;

	// IMPLEMENTATIONS:
	void					DoDelta_cpp_BASELINE(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_OPTIMIZED(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_experiment1(const PIXELMAP* lpTransitionTable);
	void					DoDelta_SSE2_experiment1(const PIXELMAP* lpTransitionTable);

	virtual void            CopyTo8(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo16(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo24(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo32(void* lpSurface, const dword_t nDeadSpace) override;
};



class BitCanvas16 : public BitCanvas
{
public:
	BitCanvas16(const dword_t dwWidth, const dword_t dwHeight, const TWEENDESCRIPTION& td);

protected:
	virtual dword_t         GetPixel(const dword_t pixelOffset) override;
	virtual void            PutPixel(const dword_t pixelOffset, const byte_t color) override;
	virtual void            PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color) override;
	virtual void     BlahPutPixel(const dword_t pixelOffset,
		const byte_t r,
		const byte_t g,
		const byte_t b) override
	{
		m_pWriteBuffer16[pixelOffset] = (unsigned short)RGB16(r, g, b);
	};

	// DISPATCH:
	virtual void            DoDelta_cpp(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_x86(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_MMX(const PIXELMAP* lpTransitionTable) override;
	virtual void            DoDelta_SSE( const PIXELMAP *lpTransitionTable ) override;

	// IMPLEMENTATIONS:
	void					DoDelta_cpp_BASELINE_NO_DECAY(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_BASELINE_WITH_DECAY_31(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_BASELINE_WITH_DECAY_63(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_OPTIMIZED_NO_DECAY(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_OPTIMIZED_WITH_DECAY_63(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_OPTIMIZED_WITH_DECAY_63_AND_GREENSHIFT_CORRECTION(const PIXELMAP* lpTransitionTable);
	void					DoDelta_cpp_OPTIMIZED_NO_DECAY_GreenCorrected_experiment1(const PIXELMAP* lpTransitionTable);
	void					DoDelta_SSE2_experiment1(const PIXELMAP* lpTransitionTable);

	virtual void            CopyTo8(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo16(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo24(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo32(void* lpSurface, const dword_t nDeadSpace) override;
};



class BitCanvas32 : public BitCanvas
{
public:
	BitCanvas32(const dword_t dwWidth, const dword_t dwHeight, const TWEENDESCRIPTION& td);

protected:
	virtual dword_t           GetPixel(const dword_t pixelOffset) override;
	virtual void            PutPixel(const dword_t pixelOffset, const byte_t color) override;
	virtual void            PutPixel(const dword_t pixelOffset, const word_t pixel_width, const byte_t color) override;
	virtual void     BlahPutPixel(const dword_t pixelOffset,
		const byte_t r,
		const byte_t g,
		const byte_t b) override
	{
		m_pWriteBuffer32[pixelOffset] = RGB32(r, g, b);
		//m_pWriteBuffer32[pixelOffset] = 0xFFFFFFFF;
		//DumpToFile( "error.txt", "blah!", "");
	};

	// DISPATCH:
	virtual void            DoDelta_cpp(const PIXELMAP* lpTransitionTable) override;
#ifdef USE_MMX_INTRINSICS
	virtual void            DoDelta_x86(const PIXELMAP* lpTransitionTable) override;
#endif
#ifdef USE_MMX_ASSEMBLY
	virtual void            DoDelta_MMX(const PIXELMAP* lpTransitionTable) override;
#endif
	virtual void            DoDelta_SSE( const PIXELMAP *lpTransitionTable ) override;

	// IMPLEMENTATIONS:
	void					DoDelta_cpp_BASELINE(const PIXELMAP* lpTransitionTable);
#ifdef USE_MMX_INTRINSICS
	void					DoDelta_MMX_Intrinsics(const PIXELMAP* lpTransitionTable);
#endif
#ifdef USE_MMX_ASSEMBLY
	void					DoDelta_MMX_Assembly(const PIXELMAP* lpTransitionTable);
#endif
	void					DoDelta_SSE2_experiment1_FIXED(const PIXELMAP* lpTransitionTable);
	void					DoDelta_SSE2_Intrinsics_FIXED(const PIXELMAP* lpTransitionTable);

	virtual void            CopyTo8(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo16(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo24(void* lpSurface, const dword_t nDeadSpace) override;
	virtual void            CopyTo32(void* lpSurface, const dword_t nDeadSpace) override;
};

