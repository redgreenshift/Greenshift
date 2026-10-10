#pragma once
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

#include <algorithm>

// Do NOT define min/max macros! Use constexpr templates! Then I don't have to replace min->std::min in one go, and doesn't mess up std::min
//#define max(x, y)    (((x) > (y)) ? (x) : (y))
//#define min(x, y)    (((x) < (y)) ? (x) : (y))

template<typename T>
constexpr T min(T a, T b)
{
    return std::min(a, b);
}

template<typename T>
constexpr T max(T a, T b)
{
    return std::max(a, b);
}

template<typename T>
constexpr T min3(T a, T b, T c)
{
    return std::min(std::min(a, b), c);
}

template<typename T>
constexpr T max3(T a, T b, T c)
{
    return std::max(std::max(a, b), c);
}


#define LCLIP( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )
#define DWORD_TO_LONG( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )
#define LONG_TO_DWORD( longNum )    ( ( (long)(longNum) > 0L ) ? (long)(longNum) : 1L )

/* when typecasting a large DWORD to a long, a negative value is possible
 * or if a negative long is typecast to a DWORD, then the value is quite large
 * this macro is to prevent this from happening
 */
#define RESTRICT_TO_POSITIVE( num )    ( ( (signed)(num) > 0 ) ? (signed)(num) : 1 )



 // ROUND is the only one I use, the others I was just experimenting with
 //#define ROUND_ZERO(num)    ( ((num) < 0.0) ? ceil((num) - 0.5) : floor((num) + 0.5) )
 /* DUH!  toward_zero is simply a TRUNCATE or typecast to integer */
#define TOWARD_ZERO(num)    ( ((num) < 0.0f) ? ceil(num) : floor(num) )
#define ROUND_ZERO(num)    ( ((num) < 0.0f) ? floor((num) - 0.5f) : floor((num) + 0.5f) )
#define ROUND(num)    ( ((num)-floor(num) < 0.5f) ? floor(num) : floor((num) + 1.0f) )
#define ROUND_SPECIAL(num, origin)    ( ((num) < (origin)) ? ceil((num) - 0.5f) : floor((num) + 0.5f) )
#define ROUND_UP(num)    ( ((num) < 0.0f) ? floor(num) :  ceil(num) )
#define ROUND_DOWN(num)    ( ((num) < 0.0f) ?  ceil(num) : floor(num) )




// Move LONG_SQRT_X2 here

 /*
  * random routines
  */

#ifndef UNDEFINED

#ifdef HIDE_RANDOM_ROUTINES_SO_I_CAN_REMOVE_CALLS_TO_THEM
#include <stdlib.h> /* rand() */

#define RANDOM    Random_Number_Between_Zero_And_N_Minus_One

  /* random number between 0 and 1, but NOT including 1 */
static float Random01(void)
{
	return rand() / (float)(RAND_MAX + 1);
}

/* random number between 0 and N, not including N */
static unsigned int Random_Number_Between_Zero_And_N_Minus_One(const unsigned int N = 1)
{
	return (int)(N * Random01());
}
#endif // HIDE_RANDOM_ROUTINES_SO_I_CAN_REMOVE_CALLS_TO_THEM

static inline long long_sqrt(const long inSquare)
{
	long square = 1L;
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

	while (square <= inSquare)
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
static inline long long_sqrt_x2(const long inSquare)
{
	long square = 1L;
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
	while (square < inSquare)
	{
		square += square_root;
		square_root++;
		if (square > inSquare)
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

static inline void long_sqrt_rounded(const long lValue, long* outValue)
{
#ifdef UNDEFINED
	if (lValue < 1L)
		*outValue = 0L;
	else
		if (lValue < 4L)
			*outValue = 1L;
		else
			if (lValue < 9L)
				*outValue = 2L;
			else
				if (lValue < 16L)
					*outValue = 3L;
				else
					if (lValue < 25L)
						*outValue = 4L;
					else
						if (lValue < 36L)
							*outValue = 5L;
#else
	switch (lValue)
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
	default:    *outValue = long_sqrt(lValue);   break;
	}
#endif
};

#endif // UNDEFINED
