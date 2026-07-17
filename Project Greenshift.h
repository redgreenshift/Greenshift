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
#ifndef __Project_Greenshift_H__
#define __Project_Greenshift_H__

/*
 * this is a toggle so I can put an insane amount of debugging code
 * and not have to toggle each individually using comments
 */
#define EXTREME_DEBUGGING 0

#define HIDE_INIT_TRACE 1

/****************************************************************************
 *
 * Project Greenshift, the common include file.
 *
 *
 * _Everything_ in Project Greenshift must include this file in the first line.
 *
 * Yes, it seems redundant and unnecessary, but this is for a couple reasons.
 *
 * The first is to identify the file as part of Project Greenshift.
 * This is more important if the code is reused by someone else in some other
 * program, so it's easy to tell which files are mine and which are not.
 *
 * The second reason is to ensure that no matter what, certain assumptions
 * always apply to any code in Project Greenshift.
 * Namely the error_t and value_t data types, and NULL.
 *
 * Third, if something needs to be accessed from anywhere, this is the place.
 *
 *
 *
 *
 * I ran into problems between the compile and link stage when using macros for conditional compilation
 * those macros should have been defined at the lowest level.
 *
 * When compiling, the included files saw the defined macros,
 * thus making code for everything it was supposed to.
 *
 * but the linker (only looking at the header files) could not see the
 * macros defined in the C++ file, thus the linker calculated offset values
 * for the member functions as if half the code hadn't been compiled.
 *
 * The result was that the wrong functions were being called.
 * Example:
 *        printf( "%s\n", expression->PrintString() );
 *        was dereferencing a "NULL" pointer, which was especially puzzling since I wrote
 *        PrintString in such a manner that it should NEVER return NULL.
 *        (ok, checking my code, this is not true, but it probably wouldn't ever return NULL =)
 *
 *        It sometimes returns a string constant stating the error ("ERROR: malloc")
 *
 *        What should be done is use out parameters, and return an error_t
 *        (PrintString isn't a core function, and is mainly intended for
 *        debugging purposes, as differentiation isn't implemented yet,
 *        so it's not a priority at the moment)
 *
 *        Using the debugger, I realized Evaluate() was actually called, NOT PrintString.
 *        Evaluate happened to return 0 (zero) for the given values of the expression.
 *        The PrintString protocol was imposed (by the linker) on a call to Evaluate,
 *        thus the zero value was interpreted as a NULL pointer.
 *
 */


/****************************************************************************
 *
 * conditional compile #defines
 *
 ****************************************************************************/
//#define USE_FAST_BLIT 1
//#define USE_FLIP 1


//#define USE_FASTER_LINE_DRAW
#define USE_TIMER_TO_HIDE_MOUSE


/*
 * signal for Expression to use a union to store members
 * should be removed before release.
 */


#include <math.h>

/****************************************************************************
 *
 * Standard Macros valid anywhere in Project Greenshift
 *
 ****************************************************************************/
#define BITS_PER_BYTE    8

// ROUND is the only one I use, the others I was just experimenting with
//#define ROUND_ZERO(num)    ( ((num) < 0.0) ? ceil((num) - 0.5) : floor((num) + 0.5) )
/* DUH!  toward_zero is simply a TRUNCATE or typecast to integer */
#define TOWARD_ZERO(num)    ( ((num) < 0.0f) ? ceil(num) : floor(num) )
#define ROUND_ZERO(num)    ( ((num) < 0.0f) ? floor((num) - 0.5f) : floor((num) + 0.5f) )
#define ROUND(num)    ( ((num)-floor(num) < 0.5f) ? floor(num) : floor((num) + 1.0f) )
#define ROUND_SPECIAL(num, origin)    ( ((num) < (origin)) ? ceil((num) - 0.5f) : floor((num) + 0.5f) )
#define ROUND_UP(num)    ( ((num) < 0.0f) ? floor(num) :  ceil(num) )
#define ROUND_DOWN(num)    ( ((num) < 0.0f) ?  ceil(num) : floor(num) )

#define max(x, y)    (((x) > (y)) ? (x) : (y))
#define min(x, y)    (((x) < (y)) ? (x) : (y))
//max(max(x,y),z) /* expands to */ ((x>y)?x:y) > z?(x>y)?x:y):z
//evaluating 3 conditionals if x or y are picked, and 2 if z is picked
//this should be reduced
#define max3(x, y, z)    (((x)>(y)) ? \
                         (((x)>(z)) ? (x) : (z)) : \
                         (((y)>(z)) ? (y) : (z)))
#define min3(x, y, z)    (((x)<(y)) ? \
                         (((x)<(z)) ? (x) : (z)) : \
                         (((y)<(z)) ? (y) : (z)))



#define SAFE_FREE( buf )         {if( (buf) != NULL ) { free( buf ); buf = NULL; }}
#define SAFE_DELETE( buf )       {if( (buf) != NULL ) { delete( buf ); buf = NULL; }}
#define SAFE_DELETE_ARRAY( buf ) {if( (buf) != NULL ) { delete[]( buf ); buf = NULL; }}
#define SAFE_DELETE_ARRAY_OF_POINTERS( buf, len )   \
    {   \
        for(int sdaopIndex=0; sdaopIndex < len; sdaopIndex++)   \
        {   \
            SAFE_DELETE_ARRAY( (buf) [sdaopIndex] );\
        }   \
    }


#define LCLIP( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )
#define DWORD_TO_LONG( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )
#define LONG_TO_DWORD( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )

/* when typecasting a large DWORD to a long, a negative value is possible
 * or if a negative long is typecast to a DWORD, then the value is quite large
 * this macro is to prevent this from happening
 */
#define RESTRICT_TO_POSITIVE( num )    ( ( (signed)(num) > 0 ) ? (signed)(num) : 1 )


/****************************************************************************
 *
 * Standard types valid anywhere in Project Greenshift
 *
 ****************************************************************************/

/*
 * pixel map is a value that maps a pixel to another pixel
 * its value is the offset into the buffer (x + y * width)
 */
typedef unsigned long   PIXELMAP;
typedef unsigned long* PPIXELMAP;

/*
 * the error type (error codes are listed below)
 */
typedef int     error_t;
 
/*
 * the floating point value type
 */
/*
typedef double    value_t;
/*/
typedef float   value_t;
/**/

/*
 * everything in Project Greenshift should use
 * value_t as the floating point type.
 * This permits global changes between the
 * double and float types.
 *
 * I did this because I wanted to see the
 * performance difference between
 * 32bit and 64bit numbers
 */
typedef value_t*    pvalue_t;


/*
 * I know I shouldn't have these typedef's and define's, but I don't want to
 * have everything include windows.h just for these things
 */
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#ifndef _COLORREF_DEFINED
#define _COLORREF_DEFINED
typedef DWORD COLORREF;

#define RGB(r,g,b)      ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define GetRValue(rgb)  ((BYTE)(rgb))
#define GetGValue(rgb)  ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)  ((BYTE)((rgb)>>16))

#endif 





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






/*
 * random routines
 */


#include <stdlib.h> /* rand() */
#ifdef HIDE_RANDOM_ROUTINES_SO_I_CAN_REMOVE_CALLS_TO_THEM

#define RANDOM    Random_Number_Between_Zero_And_N_Minus_One

/* random number between 0 and 1, but NOT including 1 */
static float Random01( void )
{
    return rand() / (float)(RAND_MAX + 1);
}

/* random number between 0 and N, not including N */
static unsigned int Random_Number_Between_Zero_And_N_Minus_One( const unsigned int N = 1 )
{
    return (int)( N * Random01() );
}
#endif

static inline long long_sqrt( const long inSquare )
{
    long square      = 1L;
    long square_root = 1L;

    /*
     * 1^2 == 1
     * 2^2 == 1 + 3
     * 3^2 == 1 + 3 + 5
     * 4^2 == 1 + 3 + 5 + 7
     * 5^2 == 1 + 3 + 5 + 7 + 9
     *
     * 1^2 == 1
     * 2^2 == 1 + 1 + 2
     * 3^2 == 1 + 1 + 2 + 2 + 3
     * 4^2 == 1 + 1 + 2 + 2 + 3 + 3 + 4
     * 5^2 == 1 + 1 + 2 + 2 + 3 + 3 + 4 + 4 + 5
     */

    while( square <= inSquare )
    {
        square += square_root;
        square_root++;
        square += square_root;
    }

    return square_root - 1L;
}

/*
 * this is just some experimental code, to see if it works... it doesn't :P
 */
static inline long long_sqrt_x2( const long inSquare )
{
    long square      = 1L;
    long square_root = 1L;

    /* 1 2 2 2
     * 3 4 4 4
     * 5 6 6 6
     * 7 8 8 8
     * 9 10 10 10
     * 11 12 12 12
     * 13 14 14 14
     * 15 16 16 16
     */
    while( square < inSquare )
    {
        square += square_root;
        square_root++;
        if( square > inSquare )
        {
            return ((square_root - 1L) << 1);
        }
        square += square_root;
    }

//    return (square_root - 1L) << 1;
    return (square_root << 1) - 1L;
}

//#define LONG_SQRT_X2( lValue, outValue ) outValue = (long)(2.0f * sqrt( (value_t)lValue ) + 0.5f )
#define LONG_SQRT_X2( lValue, outValue ) {         \
    switch( lValue )                            \
    {                                           \
    case 0L:    outValue = 0L;  break;  \
    case 1L:    outValue = 2L;  break;  \
    case 2L:    outValue = 3L;  break;  \
    case 3L:    outValue = 3L;  break;  \
    case 4L:    outValue = 4L;  break;  \
    case 5L:    outValue = 4L;  break;  \
    case 6L:    outValue = 5L;  break;  \
    case 7L:    outValue = 5L;  break;  \
    case 8L:    outValue = 6L;  break;  \
    case 9L:    outValue = 6L;  break;  \
    case 10L:   outValue = 6L;  break;  \
    case 11L:   outValue = 7L;  break;  \
    case 12L:   outValue = 7L;  break;  \
    case 13L:   outValue = 7L;  break;  \
    case 14L:   outValue = 7L;  break;  \
    case 15L:   outValue = 8L;  break;  \
/*    case 16L:   outValue = 8L;  break;  \
    case 17L:   outValue = 8L;  break;  \
    case 18L:   outValue = 8L;  break;  \
    case 19L:   outValue = 9L;  break;  \
    case 20L:   outValue = 9L;  break;  \
    case 21L:   outValue = 9L;  break;  \
    case 22L:   outValue = 9L;  break;  \
    case 23L:   outValue = 10L;  break;  \
    case 24L:   outValue = 10L;  break;  \
    case 25L:   outValue = 10L;  break;  \
    case 26L:   outValue = 10L;  break;  \
    case 27L:   outValue = 10L;  break;  \
    case 28L:   outValue = 11L;  break;  \
    case 29L:   outValue = 11L;  break;  \
    case 30L:   outValue = 11L;  break;  \
    case 31L:   outValue = 11L;  break;  \
/*    case 32L:   outValue = 11L;  break;  \
    case 33L:   outValue = 11L;  break;  \
    case 34L:   outValue = 12L;  break;  \
    case 35L:   outValue = 12L;  break;  \
    case 36L:   outValue = 12L;  break;  \
    case 37L:   outValue = 12L;  break;  \
    case 38L:   outValue = 12L;  break;  \
    case 39L:   outValue = 12L;  break;  \
    case 40L:   outValue = 13L;  break;  \
    case 41L:   outValue = 13L;  break;  \
    case 42L:   outValue = 13L;  break;  \
    case 43L:   outValue = 13L;  break;  \
    case 44L:   outValue = 13L;  break;  \
    case 45L:   outValue = 13L;  break;  \
    case 46L:   outValue = 14L;  break;  \
    case 47L:   outValue = 14L;  break;  \
    case 48L:   outValue = 14L;  break;  \
    case 49L:   outValue = 14L;  break;  \
    case 50L:   outValue = 14L;  break;  \
    case 51L:   outValue = 14L;  break;  \
    case 52L:   outValue = 14L;  break;  \
    case 53L:   outValue = 15L;  break;  \
    case 54L:   outValue = 15L;  break;  \
    case 55L:   outValue = 15L;  break;  \
    case 56L:   outValue = 15L;  break;  \
    case 57L:   outValue = 15L;  break;  \
    case 58L:   outValue = 15L;  break;  \
    case 59L:   outValue = 15L;  break;  \
    case 60L:   outValue = 15L;  break;  \
    case 61L:   outValue = 16L;  break;  \
    case 62L:   outValue = 16L;  break;  \
    case 63L:   outValue = 16L;  break;/**/  \
    default:    outValue = (long)(2.0f * (value_t)sqrt( lValue ) + 0.5f); break; \
    }                                          \
}


#define LONG_SQRT_X22( lValue, outValue ) {         \
    switch( lValue )                            \
    {                                           \
    case 0L:    outValue = 0L;       break;  \
    case 1L:    outValue = 2L;       break;  \
    case 2L:\
    case 3L:    outValue = 3L;  break;  \
    case 4L:\
    case 5L:    outValue = 4L;  break;  \
    case 6L:\
    case 7L:    outValue = 5L;  break;  \
    case 8L:\
    case 9L:\
    case 10L:   outValue = 6L;  break;  \
    case 11L:\
    case 12L:\
    case 13L:\
    case 14L:   outValue = 7L;  break;  \
/*    case 15L:\
    case 16L:\
    case 17L:\
    case 18L:   outValue = 8L;  break;  \
    case 19L:\
    case 20L:\
    case 21L:\
    case 22L:   outValue = 9L;  break;  \
    case 23L:\
    case 24L:\
    case 25L:\
    case 26L:\
    case 27L:   outValue = 10L;  break;  \
    case 28L:\
    case 29L:\
    case 30L:\
    case 31L:\
    case 32L:\
    case 33L:   outValue = 11L;  break;  \
    case 34L:\
    case 35L:\
    case 36L:\
    case 37L:\
    case 38L:\
    case 39L:   outValue = 12L;  break;  \
    case 40L:\
    case 41L:\
    case 42L:\
    case 43L:\
    case 44L:\
    case 45L:   outValue = 13L;  break;  \
    case 46L:\
    case 47L:\
    case 48L:\
    case 49L:\
    case 50L:\
    case 51L:\
    case 52L:   outValue = 14L;  break;  \
    case 53L:\
    case 54L:\
    case 55L:\
    case 56L:\
    case 57L:\
    case 58L:\
    case 59L:\
    case 60L:   outValue = 15L;  break;  \
    case 61L:\
    case 62L:\
    case 63L:   outValue = 16L;  break;/**/  \
    default:    outValue = (value_t)(2.0f * sqrt( lValue ) + 0.5f);   break;    \
    }                                          \
}

/*
static inline void long_sqrt( const long lValue, float *outValue )
{
    switch( lValue )
    {
    case 0L:    *outValue = 0.0f;       break;
    case 1L:    *outValue = 1.0f;       break;
    case 2L:    *outValue = 1.414214f;  break;
    case 3L:    *outValue = 1.732051f;  break;
    case 4L:    *outValue = 2.0f;       break;
    case 5L:    *outValue = 2.236068f;  break;
    case 6L:    *outValue = 2.449490f;  break;
    case 7L:    *outValue = 2.645751f;  break;
    case 8L:    *outValue = 2.828427f;  break;
    case 9L:    *outValue = 3.0f;       break;
    case 10L:   *outValue = 3.162278f;  break;
    case 11L:   *outValue = 3.316624f;  break;
    case 12L:   *outValue = 3.464102f;  break;
    case 13L:   *outValue = 3.605551f;  break;
    case 14L:   *outValue = 3.741657f;  break;
    case 15L:   *outValue = 3.872983f;  break;
    case 16L:   *outValue = 4.0f;       break;
    case 17L:   *outValue = 4.123106f;  break;
    case 18L:   *outValue = 4.242641f;  break;
    case 19L:   *outValue = 4.358899f;  break;
    case 20L:   *outValue = 4.472136f;  break;
    case 21L:   *outValue = 4.582576f;  break;
    case 22L:   *outValue = 4.690416f;  break;
    case 23L:   *outValue = 4.795832f;  break;
    case 24L:   *outValue = 4.898979f;  break;
    case 25L:   *outValue = 5.0f;       break;
    case 26L:   *outValue = 5.099020f;  break;
    case 27L:   *outValue = 5.196152f;  break;
    case 28L:   *outValue = 5.291503f;  break;
    case 29L:   *outValue = 5.385165f;  break;
    case 30L:   *outValue = 5.477226f;  break;
    case 31L:   *outValue = 5.567764f;  break;
    case 32L:   *outValue = 5.656854f;  break;
    case 33L:   *outValue = 5.744563f;  break;
    case 34L:   *outValue = 5.830952f;  break;
    case 35L:   *outValue = 5.916080f;  break;
    case 36L:   *outValue = 6.0f;       break;
    default:    *outValue = sqrt( lValue );   break;
    }
};
/**/

static inline void long_sqrt_rounded( const long lValue, long *outValue )
{
#ifdef UNDEFINED
    if( lValue < 1L )
        *outValue = 0L;
    else
    if( lValue < 4L )
        *outValue = 1L;
    else
    if( lValue < 9L )
        *outValue = 2L;
    else
    if( lValue < 16L )
        *outValue = 3L;
    else
    if( lValue < 25L )
        *outValue = 4L;
    else
    if( lValue < 36L )
        *outValue = 5L;
#else
    switch( lValue )
    {
    case 0L:    *outValue = 0L;     break;
    case 1L:
    case 2L:    *outValue = 1L;     break;
    case 3L:
    case 4L:
    case 5L:
    case 6L:    *outValue = 2L;     break;
    case 7L:
    case 8L:
    case 9L:
    case 10L:
    case 11L:
    case 12L:   *outValue = 3L;     break;
    case 13L:
    case 14L:
    case 15L:
    case 16L:
    case 17L:
    case 18L:
    case 19L:
    case 20L:   *outValue = 4L;     break;
    case 21L:
    case 22L:
    case 23L:
    case 24L:
    case 25L:
    case 26L:
    case 27L:
    case 28L:
    case 29L:
    case 30L:   *outValue = 5L;     break;
    case 31L:
    case 32L:
    case 33L:
    case 34L:
    case 35L:
    case 36L:   *outValue = 6L;     break;
    default:    *outValue = long_sqrt( lValue );   break;
    }
#endif
};








/****************************************************************************
 *
 * #defines I probably shouldn't have here
 *
 ****************************************************************************/

#ifdef USE_TIMER_TO_HIDE_MOUSE
#define WINVER 0x0500
#define _WIN32_WINDOWS    0x0410
#endif


#ifndef WIN32
#define WIN32
#endif

//#ifndef STRICT
//#define STRICT 1
//#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

/* define NULL pointer value */
#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#if EXTREME_DEBUGGING
#define DebugMessage    ProjectGreenshiftDebugMessageBox
#else
#define DebugMessage(a,b,c)    /* nothing */
#endif

#if EXTREME_DEBUGGING

#include <stdio.h>
#include <windows.h>
static void    ProjectGreenshiftDebugMessageBox( const char *aString, value_t aNumber, char *anotherString = "" )
{
    char    strDisplay[2048];

    sprintf( strDisplay, "%s%.40g%s", aString, aNumber, anotherString );
    MessageBox( NULL, strDisplay, "Debug MessageBox", MB_OK );
}

#endif

#if EXTREME_DEBUGGING

//#ifndef UNDEFINED

/****************************************************************************
 *
 * Start of Debug Methods
 *
 ****************************************************************************/
/* extern void * memcpy_amd(void *dest, const void *src, size_t n); */

/*
 * make a Debug Class for the error types, wait... errors aren't necessarily
 * limited to debugging it's the DumpToFile which should go in Debug
 */
#include <stdio.h>
#include <float.h>
static void DumpToFile(const char *fileName, char *aString, value_t aNumber, char *anotherString = "")
{
    FILE *outFile =    fopen( fileName, "a" );

    fprintf( outFile, "%s%g%s", aString, aNumber, anotherString);

    fclose( outFile );

}

static void DumpToFile(const char *fileName, value_t aNumber, char *anotherString = "")
{
    FILE *outFile =    fopen( fileName, "a" );

    fprintf( outFile, "%g%s", aNumber, anotherString);

    fclose( outFile );

}

static void DumpToFile(const char *fileName, value_t *aNumber, char *anotherString = "")
{
    FILE *outFile =    fopen( fileName, "a" );

    fprintf( outFile, "%g%s", *aNumber, anotherString);

    fclose( outFile );

}
#include "Expression.h"
static void DumpToFile(const char *fileName, EXPRESSIONDESCRIPTION *pED, char *anotherString = "")
{
    FILE *outFile =    fopen( fileName, "a" );

    fprintf( outFile, "%s %d", pED->strName, pED->edtType );

    if( pED->edtType == ED_CONSTANT )
        fprintf( outFile, " %g", pED->value );


    fprintf( outFile, " %s", anotherString );

    fclose( outFile );

}

static void DumpToFile(const char *fileName, MyDictionary<char*> *pDict, char *anotherString = "")
{
    FILE *outFile;

//    pDict->DebugDumpContents( fileName );


    outFile =     fopen( fileName, "a" );

    fprintf( outFile, " %s", anotherString );

    fclose( outFile );
}

static void DumpToFile(const char *fileName, char *aString, char *anotherString = "")
{
    FILE *outFile =    fopen( fileName, "a" );

    fprintf( outFile, "%s%s", aString, anotherString); 

    fclose( outFile );

}

/****************************************************************************
 *
 * End of Debug Methods
 *
 ****************************************************************************/

#endif  /* EXTREME_DEBUGGING */



/****************************************************************************
 *
 * Error Codes
 *
 ****************************************************************************/
enum {
    /* Error Name */        /* thing that returns the specific error */

    SUCCESS  = 0,           /* anything - means no error */

    ERR_FAKE = 1,           /* placeholder to catch things not using an
                             * enumerated error code and doing something
                             * like "return 1"
                             */

    FAILURE,                /* anything - general failure only used until
                             * a custom error code and message are added
                             */

    ERR_NULL,               /* anything - NULL passed as a parameter.
                             * if NULL is a valid value for the function,
                             * you don't have to return this.
                             */

    ERR_MALLOC,             /* anything */
    ERR_REALLOC,            /* anything */
    ERR_BOUNDS,             /* anything - array access out of bounds */

    ERR_ALREADYDEPENDENT,   /* Model */

    ERR_NOTFOUND,           /* MyDictionary */
    ERR_WINDOWDEVICE,       /* WindowDevice */
    ERR_BITCANVAS,          /* BitCanvas */
    ERR_OVERLAY,            /* BitCanvas */
    ERR_THREADBUSY,         /* ThreadedEntity */
    ERR_PALETTE,            /* Palette */
    ERR_UNKNOWNPALETTETYPE, /* Palette */
    ERR_NOTDELTAFIELD,      /* DeltaField */
//    ERR_SUBCLASS_RESPONSIBILITY,    /* Expression */
    ERR_SHOULD_NOT_IMPLEMENT,/* Expression */
    ERR_COMPILE,            /* Expression */
//    ERR_UNARY_NEGATION,     /* Expression */
//    ERR_OPERATION_NOT_DEFINED,    /* Expression */
    ERR_DIV_BY_ZERO,        /* Expression */
    ERR_CONSTANT,           /* Expression */
    ERR_SYMBOL,             /* Expression */
    ERR_VARIABLE,           /* Expression */
    ERR_USERDEFINED,        /* Expression */
    ERR_SQR,                /* Expression */
    ERR_SQRT,               /* Expression */
    ERR_EXP,                /* Expression */
    ERR_LN,                 /* Expression */
    ERR_LOG10,              /* Expression */
    ERR_COS,                /* Expression */
    ERR_SIN,                /* Expression */
    ERR_TAN,                /* Expression */
    ERR_ARCCOS,             /* Expression */
    ERR_ARCSIN,             /* Expression */
    ERR_ARCTAN,             /* Expression */
    ERR_COSH,               /* Expression */
    ERR_SINH,               /* Expression */
    ERR_TANH,               /* Expression */
    ERR_ARCCOSH,            /* Expression */
    ERR_ARCSINH,            /* Expression */
    ERR_ARCTANH,            /* Expression */
    ERR_ADD,                /* Expression */
    ERR_SUB,                /* Expression */
    ERR_MULT,               /* Expression */
    ERR_DIV,                /* Expression */
    ERR_MOD,                /* Expression */
    ERR_POW,                /* Expression */
    ERR_COMMA,              /* Expression */
    ERR_OR,                 /* Expression */
    ERR_AND,                /* Expression */
    ERR_STAR,               /* Expression */
//    ERR_UNKNOWNOPCODE,      /* Expression - when using the VM approach */

    ERR_UNDEFINED,          /* the error code right after the last error,
                             * so can use a check like:
                             *
                             * if( err > SUCCESS && err < ERR_UNDEFINED )
                             *        return err;
                             * else
                             *        return ERR_INVALIDERRORCODE;
                             */

    ERR_INVALIDERRORCODE    /* same thing as undefined error -
                             * I just couldn't decide between the two
                             */
};



//class ProjectGreenshift
//{
//public:
    /************************************************************************
     *
     * ErrorString - maps error code to a human readable message
     *
     ************************************************************************/
//    friend
#ifdef UNDEFINED
//#ifdef _DLL
    static char *ErrorString( error_t errCode )
    {
//        switch( errCode )
//        {
//        case SUCCESS:
//            return "success";
//        default:
            return "the descriptive error messages are removed from the DLL to save space";
//      }
    };
#else
    static char *ErrorString( error_t errCode )
    {
        switch( errCode )
        {
        case SUCCESS:
            return "success";
        case ERR_FAKE:
            return "ERROR: something is returning 1 instead of FAILURE";
        case FAILURE:
            return "failure";
        case ERR_NULL:
            return "silly programmer, NULL is for coredumps!";
        case ERR_MALLOC:
            return "malloc: error allocating memory";
        case ERR_REALLOC:
            return "realloc: error reallocating memory";
        case ERR_BOUNDS:
            return "array access out of bounds";
        case ERR_ALREADYDEPENDENT:
            return "already a dependent of the model";
        case ERR_NOTFOUND:
            return "var not found in dictionary";
        case ERR_WINDOWDEVICE:
            return "error creating the window device";
        case ERR_BITCANVAS:
            return "invalid bit depth";
        case ERR_OVERLAY:
            return "overlay error";
        case ERR_THREADBUSY:
            return "thread is busy... not accepting work at the moment";
        case ERR_PALETTE:
            return "error creating a palette";
        case ERR_UNKNOWNPALETTETYPE:
            return "unable to determine the type of palette";
        case ERR_NOTDELTAFIELD:
            return "config file is not a delta field";
//        case ERR_SUBCLASS_RESPONSIBILITY:
//            return "subclass responsibility";
        case ERR_SHOULD_NOT_IMPLEMENT:
            return "should not implement";
        case ERR_COMPILE:
            return "the entered expression could not be parsed, since it's not a valid expression";
//        case ERR_UNARY_NEGATION:
//            return "we are sorry, unary negation has not been implemented at this time";
//        case ERR_OPERATION_NOT_DEFINED:
//            return "matched operator has been declared in orderOfOperations, but the instance creation code has not been implemented yet";
        case ERR_DIV_BY_ZERO:
            return "division by zero";
        case ERR_CONSTANT:
            return "ExpressionConstant creation failed";
        case ERR_SYMBOL:
            return "ExpressionSymbol creation failed";
        case ERR_VARIABLE:
            return "ExpressionVariable creation failed";
        case ERR_USERDEFINED:
            return "ExpressionUserDefined creation failed";
        case ERR_SIN:
            return "ExpressionSin creation failed";
        case ERR_COS:
            return "ExpressionCos creation failed";
        case ERR_ARCSIN:
            return "ExpressionArcSin creation failed";
        case ERR_ADD:
            return "ExpressionAdd creation failed";
        case ERR_SUB:
            return "ExpressionSub creation failed";
        case ERR_MULT:
            return "ExpressionMult creation failed";
        case ERR_DIV:
            return "ExpressionDiv creation failed";
        case ERR_MOD:
            return "ExpressionMod creation failed";
        case ERR_POW:
            return "ExpressionPower creation failed";
        case ERR_COMMA:
            return "ExpressionComma creation failed";
        case ERR_OR:
            return "ExpressionOr creation failed";
        case ERR_AND:
            return "ExpressionAnd creation failed";
        case ERR_STAR:
            return "ExpressionStar creation failed";
//        case ERR_UNKNOWNOPCODE:
//            return "unknown opcode";
        case ERR_UNDEFINED:
        case ERR_INVALIDERRORCODE:
        default:
#ifndef EXTENDED_DEFAULT_ERROR_MESSAGE
            return "<< invalid error code >>";
#else
            return "<< invalid error code >>"
//                " : no message is defined for this error code.\n"
                "\n\n\tThis means one of two things:\n"
                " 1) no message is defined for this error code, or\n"
                " 2) something is returning an error code that does not exist.\n"
//                " 2) the error message isn't defined in \"Project Greenshift.h\""
                ;
#endif

        }
    };

//};

#endif

#ifdef UNDEFINED_MACRO_SO_THE_FOLLOWING_TEXT_IS_HIDDEN_FROM_THE_COMPILER

 to explain the below comment scheme, I like to be able to toggle between
 two bits of code using comments... but the catch is I want to be able to
 toggle by changing only one character.

 The first /* denotes the beginning of a comment (obviously)
 and the following /*/ is interpreted as the end of this comment.

 This is because the first slash of /*/ is part of the comment,
 thus ignored, and the */ part is seen as the end of the comment.

 Since that's the end of the comment, the lines following that are
 uncommented and the /**/ comment doesn't really do anything... yet...


 NOW, placing a forward-slash in front of the opening "/*"
 turns the multiline comment into a single line comment
 //* this in turn stops commenting anything past that line,
 and the previous end of comment /*/ is now interpreted as a begin comment.

 Finally, the /**/ comment that previously didn't seem to do anything
 now functions as the closing to the new comment.


 There is another catch... this does not work if multiline comments are used,
 so this scheme is mainly for use when debugging small sections of code.

 For larger sections of code, #ifdef conditional compiles are much better

#endif /* UNDEFINED_MACRO_SO_THE_FOLLOWING_TEXT_IS_HIDDEN_FROM_THE_COMPILER */




#ifdef UNDEFINED_SO_YOU_CAN_SEE_THE_COMMENT_SCHEME_IN_SYNTAX_HIGHLIGHT_EDITOR

/*
this line is a comment
/*/
this line is not a comment
/**/


//*
this line is not a comment
/*/
this line is a comment
/**/

#endif /* UNDEFINED_SO_YOU_CAN_SEE_THE_COMMENT_SCHEME */



#endif  /* __Project_Greenshift_H__ */

