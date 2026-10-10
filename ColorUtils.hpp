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

#include <cstdint>

// ---------------------------------------------------------------------------
// Legacy COLORREF-compatible color type.
//
// Layout remains compatible with RGB/GetRValue/GetGValue/GetBValue.
// ---------------------------------------------------------------------------

//using color_t = dword_t;
//using colorref_t = dword_t;
typedef uint32_t color_t;
typedef uint32_t colorref_t;

//inline constexpr color_t GS_RGB(byte_t r, byte_t g, byte_t b)
//{
//    return
//        static_cast<color_t>(r) |
//        (static_cast<color_t>(g) << 8) |
//        (static_cast<color_t>(b) << 16);
//}
//
//constexpr byte_t GS_GetRValue(color_t rgb)
//{
//    return static_cast<byte_t>(rgb & 0xff);
//}
//
//constexpr byte_t GS_GetGValue(color_t rgb)
//{
//    return static_cast<byte_t>((rgb >> 8) & 0xff);
//}
//
//constexpr byte_t GS_GetBValue(color_t rgb)
//{
//    return static_cast<byte_t>((rgb >> 16) & 0xff);
//}


//typedef unsigned char BYTE;
////typedef unsigned short WORD;
////typedef unsigned long DWORD;
//
//#ifndef _PALETTEENTRY_DEFINED
//#define _PALETTEENTRY_DEFINED
//typedef struct tagPALETTEENTRY {
//    BYTE        peRed;
//    BYTE        peGreen;
//    BYTE        peBlue;
//    BYTE        peFlags;
//} PALETTEENTRY, * PPALETTEENTRY, * LPPALETTEENTRY;
//#endif // !_PALETTEENTRY_DEFINED

/****************************************************************************
 *
 * color manipulation macros
 *
 ****************************************************************************/
#define REDSHIFT16      11
#define GREENSHIFT16    5
#define BLUESHIFT16     0
#define REDMASK16       0x001F
#define GREENMASK16     0x003F
#define BLUEMASK16      0x001F
#define _REDMASK16      0xF800
#define _GREENMASK16    0x07E0
#define _BLUEMASK16     0x001F

#define RGB16(red, green, blue)    ( \
                        ( ((red  ) & REDMASK16  ) << REDSHIFT16  ) |\
                        ( ((green) & GREENMASK16) << GREENSHIFT16) |\
                        ( ((blue ) & BLUEMASK16 ) << BLUESHIFT16 ) )
#define RED16(color)    (((color) >> REDSHIFT16  ) & REDMASK16   )
#define GREEN16(color)  (((color) >> GREENSHIFT16) & GREENMASK16 )
#define BLUE16(color)   (((color) >> BLUESHIFT16 ) & BLUEMASK16  )



#define REDSHIFT32      16
#define GREENSHIFT32    8
#define BLUESHIFT32     0
#define REDMASK32       0xFF
#define GREENMASK32     0xFF
#define BLUEMASK32      0xFF

#define RGB32(red, green, blue)    ( \
                        ( ((red  ) & REDMASK32  ) << REDSHIFT32  ) |\
                        ( ((green) & GREENMASK32) << GREENSHIFT32) |\
                        ( ((blue ) & BLUEMASK32 ) << BLUESHIFT32 ) )
#define RED32(color)    (((color) >> REDSHIFT32  ) & REDMASK32   )
#define GREEN32(color)  (((color) >> GREENSHIFT32) & GREENMASK32 )
#define BLUE32(color)   (((color) >> BLUESHIFT32 ) & BLUEMASK32  )

#define c32to16(color)  RGB16( r32to16(color), r32to16(color), b32to16(color) )


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



