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

// 0x00RRGGBB
using rgb32_t = dword_t;

// 0xAARRGGBB
using argb32_t = dword_t;

// ---------------------------------------------------------------------------
// 0x00BBGGRR (compatible with Windows COLORREF)
// Legacy COLORREF-compatible color type.
//
// Layout remains compatible with RGB/GetRValue/GetGValue/GetBValue.
// ---------------------------------------------------------------------------
using bgr32_t = dword_t;

// 0bRRRRRGGGGGGBBBBB
using rgb16_t = word_t;

// 0bBBBBBGGGGGGRRRRR
using bgr16_t = word_t;


inline constexpr dword_t REDSHIFT32 = 16;
inline constexpr dword_t GREENSHIFT32 = 8;
inline constexpr dword_t BLUESHIFT32 = 0;
inline constexpr dword_t REDMASK32 = 0xFF;
inline constexpr dword_t GREENMASK32 = 0xFF;
inline constexpr dword_t BLUEMASK32 = 0xFF;
inline constexpr dword_t _REDMASK32 = 0xFF00;
inline constexpr dword_t _GREENMASK32 = 0x0FF0;
inline constexpr dword_t _BLUEMASK32 = 0x00FF;

inline constexpr rgb32_t RGB32(byte_t r, byte_t g, byte_t b)
{
    return
        (static_cast<rgb32_t>(r) << REDSHIFT32) |
        (static_cast<rgb32_t>(g) << GREENSHIFT32) |
        (static_cast<rgb32_t>(b) << BLUESHIFT32);
}

inline constexpr byte_t RFromRGB32(rgb32_t rgb)
{
    return static_cast<byte_t>((rgb >> REDSHIFT32) & REDMASK32);
}

inline constexpr byte_t GFromRGB32(rgb32_t rgb)
{
    return static_cast<byte_t>((rgb >> GREENSHIFT32) & GREENMASK32);
}

inline constexpr byte_t BFromRGB32(rgb32_t rgb)
{
    return static_cast<byte_t>((rgb >> BLUESHIFT32) & BLUEMASK32);
}

inline constexpr bgr32_t BGR32(byte_t r, byte_t g, byte_t b)
{
    return
        (static_cast<bgr32_t>(r) << 0) |
        (static_cast<bgr32_t>(g) << 8) |
        (static_cast<bgr32_t>(b) << 16);
}

inline constexpr byte_t RFromBGR32(bgr32_t rgb)
{
    return static_cast<byte_t>((rgb >> 0) & REDMASK32);
}

inline constexpr byte_t GFromBGR32(bgr32_t rgb)
{
    return static_cast<byte_t>((rgb >> 8) & GREENMASK32);
}

inline constexpr byte_t BFromBGR32(bgr32_t rgb)
{
    return static_cast<byte_t>((rgb >> 16) & BLUEMASK32);
}

inline constexpr word_t REDSHIFT16 = 11;
inline constexpr word_t GREENSHIFT16 = 5;
inline constexpr word_t BLUESHIFT16 = 0;
inline constexpr word_t REDMASK16 = 0x001F;
inline constexpr word_t GREENMASK16 = 0x003F;
inline constexpr word_t BLUEMASK16 = 0x001F;
inline constexpr word_t _REDMASK16 = 0xF800;
inline constexpr word_t _GREENMASK16 = 0x07E0;
inline constexpr word_t _BLUEMASK16 = 0x001F;

inline constexpr rgb16_t RGB16(byte_t r, byte_t g, byte_t b)
{
    return
        (static_cast<rgb16_t>(r) << REDSHIFT16) |
        (static_cast<rgb16_t>(g) << GREENSHIFT16) |
        (static_cast<rgb16_t>(b) << BLUESHIFT16);
}

constexpr byte_t RFromRGB16(rgb16_t rgb)
{
    return static_cast<byte_t>((rgb >> REDSHIFT16) & REDMASK16);
}

constexpr byte_t GFromRGB16(rgb16_t rgb)
{
    return static_cast<byte_t>((rgb >> GREENSHIFT16) & GREENMASK16);
}

constexpr byte_t BFromRGB16(rgb16_t rgb)
{
    return static_cast<byte_t>((rgb >> BLUESHIFT16) & BLUEMASK16);
}

inline constexpr bgr16_t BGR16(byte_t r, byte_t g, byte_t b)
{
    return
        (static_cast<bgr16_t>(r) << 0) |
        (static_cast<bgr16_t>(g) << 5) |
        (static_cast<bgr16_t>(b) << 11);
}

constexpr byte_t RFromBGR16(bgr16_t rgb)
{
    return static_cast<byte_t>((rgb >> 0) & REDMASK16);
}

constexpr byte_t GFromBGR16(bgr16_t rgb)
{
    return static_cast<byte_t>((rgb >> 5) & GREENMASK16);
}

constexpr byte_t BFromBGR16(bgr16_t rgb)
{
    return static_cast<byte_t>((rgb >> 11) & BLUEMASK16);
}


/* Palette Entry RGB */
struct paletteentry_t {
    byte_t          peRed;
    byte_t          peGreen;
    byte_t          peBlue;
    byte_t          peFlags;
};

/* Logical Palette RGB */
struct logpalette_t {
    word_t          palVersion;
    word_t          palNumEntries;
    paletteentry_t  palPalEntry[1];
};



/****************************************************************************
 *
 * color manipulation macros
 *
 ****************************************************************************/

//#define RGB16m(red, green, blue)    ( \
//                        ( ((red  ) & REDMASK16  ) << REDSHIFT16  ) |\
//                        ( ((green) & GREENMASK16) << GREENSHIFT16) |\
//                        ( ((blue ) & BLUEMASK16 ) << BLUESHIFT16 ) )
//#define RED16(color)    (((color) >> REDSHIFT16  ) & REDMASK16   )
//#define GREEN16(color)  (((color) >> GREENSHIFT16) & GREENMASK16 )
//#define BLUE16(color)   (((color) >> BLUESHIFT16 ) & BLUEMASK16  )
//
//
//
//#define RGB32m(red, green, blue)    ( \
//                        ( ((red  ) & REDMASK32  ) << REDSHIFT32  ) |\
//                        ( ((green) & GREENMASK32) << GREENSHIFT32) |\
//                        ( ((blue ) & BLUEMASK32 ) << BLUESHIFT32 ) )
//#define RED32(color)    (((color) >> REDSHIFT32  ) & REDMASK32   )
//#define GREEN32(color)  (((color) >> GREENSHIFT32) & GREENMASK32 )
//#define BLUE32(color)   (((color) >> BLUESHIFT32 ) & BLUEMASK32  )

//#define c32to16(color)  RGB16( r32to16(color), r32to16(color), b32to16(color) )

 //#define _COLORREF_DEFINED
 //#ifndef _COLORREF_DEFINED
 //#define _COLORREF_DEFINED
 //typedef DWORD COLORREF;
 //
 //#define RGB(r,g,b)      ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
 //
 //#ifndef GetRValue
 //#define GetRValue(rgb)  ((BYTE)(rgb))
 //#endif
 //
 //#ifndef GetGValue
 //#define GetGValue(rgb)  ((BYTE)(((WORD)(rgb)) >> 8))
 //#endif
 //
 //#ifndef GetBValue
 //#define GetBValue(rgb)  ((BYTE)((rgb)>>16))
 //#endif
 //
 ////#define GetRValue(rgb)      (LOBYTE(rgb))
 ////#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
 ////#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
 //
 //
 //#endif // _COLORREF_DEFINED



