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
  * GForceFunctions - in order to maintain compatibility with G-Force configs
  *
  ****************************************************************************/

#include "GForceFunctions.h"
#include "Greenshift.h"


  /****************************************************************************
   *
   * mag - Waveform Data
   *
   ****************************************************************************/
value_t GForce_mag(value_t nValue, void* pData)
{
	return ((Greenshift*)pData)->GetWaveformData(nValue);
}


/****************************************************************************
 *
 * fft - Spectrum Data
 *
 ****************************************************************************/
value_t GForce_fft(value_t nValue, void* pData)
{
	return ((Greenshift*)pData)->GetSpectrumData(nValue);
}


/****************************************************************************
 *
 * pos(x)  == x if x >= 0 and 0 otherwise
 *
 ****************************************************************************/
value_t    GForce_pos(value_t nValue)
{
	return (nValue > 0.0f) ? nValue : 0.0f;
}


/****************************************************************************
 *
 * abs(x)  == |x|
 *
 ****************************************************************************/
value_t GForce_abs(value_t nValue)
{
	return (value_t)fabs(nValue);
}


/****************************************************************************
 *
 * sqr(x)  == x * x
 *
 ****************************************************************************/
value_t GForce_sqr(value_t nValue)
{
	return nValue * nValue;
}


/****************************************************************************
 *
 * sgn(x)  == 1 if x >= 0 and -1 if x < 0
 *
 ****************************************************************************/
value_t GForce_sgn(value_t nValue)
{
	return (nValue >= 0.0f) ? 1.0f : -1.0f;
}


/****************************************************************************
 *
 * rnd(x)  == a random real number from 0 to nValue
 *
 ****************************************************************************/
value_t    GForce_rnd(value_t nValue)
{
	return nValue * rand() / (value_t)RAND_MAX;
}


/****************************************************************************
 *
 * wrap(x) == x - flor( x )  (ex: wrap( .3 ) = .3, wrap( 4.12 ) = .12, wrap( - 2.7 ) = .3 )
 *
 ****************************************************************************/
value_t GForce_wrap(value_t nValue)
{
#ifdef UNDEFINED
	const value_t nRetVal = nValue - (value_t)floor(nValue);

	if (nRetVal == 0.0f && nValue > 0.0f)
		return 1.0f;
	else
		return nRetVal;
#else
	return nValue - (value_t)floor(nValue);
#endif
}


/****************************************************************************
 *
 * clip(x) == 0 when x < 0, 1 when x > 1, and x when  0 <= x <= 1
 *
 ****************************************************************************/
value_t GForce_clip(value_t nValue)
{
	if (nValue < 0.0f)
		return 0.0f;
	else
		if (nValue > 1.0f)
			return 1.0f;
		else
			return nValue;
}


/****************************************************************************
 *
 * trnc(x) == x with everything right of the decimal point dropped
 *
 ****************************************************************************/
value_t GForce_trnc(value_t nValue)
{
	return (value_t)((long)nValue);

}


/****************************************************************************
 *
 * flor(x) == the largest integer that's also less than x (ex, flor(3.2) == 3, flor(-2.7) == -3 )
 *
 ****************************************************************************/
value_t GForce_flor(value_t nValue)
{
	return (value_t)floor(nValue);
}


/****************************************************************************
 *
 * sqwv(x) == 1 if |x| <= 1.0 and 0 otherwise.
 *
 ****************************************************************************/
value_t GForce_sqwv(value_t nValue)
{
	if (nValue >= -1.0f && nValue <= 1.0f)
		return 1.0f;
	else
		return 0.0f;
}


/****************************************************************************
 *
 * trwv(x) == Triangle shape that passes thru (0,0), (1,1), (2,0) and repeats in both directions
 *
 ****************************************************************************/
value_t GForce_trwv(value_t nValue)
{
	nValue = (value_t)fabs(0.5f * nValue);
	nValue = 2.0f * (nValue - (value_t)floor(nValue));
	if (nValue > 1.0f) nValue = 2.0f - nValue;
	return nValue;
}


/****************************************************************************
 *
 * Generates a deterministic RNG seed based on the binary representation of a value.
 *
 * This function ensures deterministic seeding by narrowing the input to a 32-bit float
 * and reinterpreting its raw bits as an integer. It applies a circular bit rotation
 * to the resulting integer to maximize the entropy of the seed, ensuring that small
 * variations in the input value lead to significantly different random sequences.
 *
 * While the code is changed from the G-Force implementation, the idea is to remain
 * failthful to the way it generates a seed, even if we widen our floating point type.
 *
 * @param input_value The value used to derive the RNG seed.
 * @return uint32_t: a pseudo-random seed derived from a floating-point value.
 ****************************************************************************/
uint32_t Get_GForce_seed32(value_t input_value)
{
	// Narrow input to a 32-bit float to ensure a consistent bit-width for the seed
	float float_val = static_cast<float>(input_value);

	// Reinterpret the raw binary representation of the float as an unsigned 32-bit integer
	uint32_t raw_bits = float_to_u32_bits(float_val);

	// Calculate a rotation offset based on the bit pattern
	uint32_t shift = raw_bits % 31;

	// Perform a circular left rotation to shuffle the bits for seeding the RNG,
	// any bits that "fall off" the left side are wrapped around to the right side.
	// This ensures that even small changes in the input float result in a significantly different seed.
	//
	// This circular rotation is designed to compensate for the fact that the old srand()
	// is very weak and doesn't do any internal shuffling. Today, we have much better options
	// like SplitMix64, but I'm keeping this to maintain compatibility with G-Force.
	// (even though this shift is unnecessary if we upgrade Greenshift to use Mersenne Twister for configs)
	return ((raw_bits << shift) | (raw_bits >> (32 - shift)));
}

/**
 * "seed(x)" Seeds the global pseudo-random number generator based on the binary representation of a value.
 *
 * @param nValue - The value used to derive the RNG seed.
 * @return value_t: The original input value, allowing the function to be used in expression chains.
 */
value_t GForce_seed(value_t nValue)
{
	srand(Get_GForce_seed32(nValue));

	return nValue;
}



/****************************************************************************
 *
 * tan
 *
 ****************************************************************************/
value_t GForce_tan(value_t nValue)
{
	return (value_t)tan(nValue);
}


/****************************************************************************
 *
 * atan
 *
 ****************************************************************************/
value_t GForce_atan(value_t nValue)
{
	return (value_t)atan(nValue);
}


/****************************************************************************
 *
 * log
 *
 ****************************************************************************/
value_t GForce_log(value_t nValue)
{
	return (value_t)log(nValue);
}


/****************************************************************************
 *
 * exp
 *
 ****************************************************************************/
value_t GForce_exp(value_t nValue)
{
	return (value_t)exp(nValue);
}


/****************************************************************************
 *
 * sqrt
 *
 ****************************************************************************/
value_t GForce_sqrt(value_t nValue)
{
	return (value_t)sqrt(nValue);
}

