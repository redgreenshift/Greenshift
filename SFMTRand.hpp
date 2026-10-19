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

 // `SFMT_MEXP` must be one of 607, 1279, 2281, 4253, 11213, 19937, 44497, 86243, 132049, 216091.
#define SFMT_MEXP 216091
#define HAVE_SSE2
#include "SFMT/SFMT.h"
#include "SFMT/SFMT-params.h"

#include "RandUtils.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>


class SFMTRand
{
private:
	sfmt_t rng;
	uint64_t reseed_counter = 0; // guarantee unique seeds

public:
	SFMTRand();  // HIGH-ENTROPY: auto-initialize with /dev/urandom or unique_seed() -- RECOMMENDED for new code
	SFMTRand(uint32_t oneSeed);  // low-entropy - LEGACY constructor: initialize with a simple uint32 (prefer other constructors)
	SFMTRand(uint32_t* bigSeed, int length);  // initialize with an array of `length` uint32's

	//inline dword_t sfmt_genrand_uint32();
	double rand();                      // real number in [0,1]
	double rand(double n);     // real number in [0,n]
	double randExc();                   // real number in [0,1)
	double randExc(double n);  // real number in [0,n)
	uint32_t randInt();                       // integer in [0,2^32-1]
	uint32_t randInt(uint32_t n);      // integer in [0,n] for n < 2^32
	uint64_t randInt64();                       // integer in [0,2^64-1]
	double operator()();

	// Re-seeding functions with same behavior as initializers

	void seed(uint32_t oneSeed);               // low-entropy - LEGACY - prefer deterministic_seed_u64() for full entropy
	void seed(uint32_t* init_key, int key_length);
	void seed();                      // DEFAULT: HIGH-ENTROPY: auto-seed with /dev/urandom (if available) or system entropy -- Recommended for new code
	void unique_seed();                        // HIGH-ENTROPY: nondeterministic seed interface based on system state
	void deterministic_seed_u64(uint64_t seed);// HIGH-ENTROPY: deterministic seed interface
	void deterministic_seed_f64(double seed);  // HIGH-ENTROPY: deterministic seed interface
	void deterministic_seed_u32(uint32_t seed);  // low-entropy : LEGACY INTEGRATION ONLY - prefer deterministic_seed_u64() for full entropy
	void deterministic_seed_f32(float seed);   // low-entropy : LEGACY INTEGRATION ONLY - prefer deterministic_seed_f64() for full entropy
};

inline SFMTRand::SFMTRand(uint32_t oneSeed)
	{ seed(oneSeed); }

inline SFMTRand::SFMTRand(uint32_t* bigSeed, int length)
	{ seed(bigSeed, length); }

inline SFMTRand::SFMTRand()
	{ seed(); }

inline double SFMTRand::rand()                      // real number in [0,1]
{
	return ::sfmt_genrand_real1(&rng);
}

inline double SFMTRand::rand(double n)     // real number in [0,n]
{
	return rand() * n;
}

// @returns real number in [0,1)
inline double SFMTRand::randExc()
{
	return ::sfmt_genrand_real2(&rng);
}

// @returns real number in [0,n)
inline double SFMTRand::randExc(double n)
{
	return randExc() * n;
}

inline uint32_t SFMTRand::randInt()                       // integer in [0,2^32-1]
{
	return ::sfmt_genrand_uint32(&rng);
}

inline uint64_t SFMTRand::randInt64()                     // integer in [0,2^64-1]
{
	return ::sfmt_genrand_uint64(&rng);
}

// @returns integer in [0,n] for n < 2^32
inline uint32_t SFMTRand::randInt(uint32_t n)      // integer in [0,n] for n < 2^32
{	// Find which bits are used in n
	uint32_t mask = 1;

	while (mask < n)
	{
		mask = (mask << 1) | 1;
	}

	// Draw numbers until one is found in [0,n]
	uint32_t value;
	do
	{
		value = randInt() & mask; // toss unused bits to shorten search
	} while (value > n);

	return value;
}

inline double SFMTRand::operator()()
	{ return rand(); }

inline void SFMTRand::seed(uint32_t oneSeed)               // low-entropy - LEGACY - prefer deterministic_seed_u64() for full entropy
{
	::sfmt_init_gen_rand(&rng, oneSeed);
}

inline void SFMTRand::seed(uint32_t* init_key, int key_length)
{
	::sfmt_init_by_array(&rng, init_key, key_length);
}

inline void SFMTRand::seed() // DEFAULT: HIGH-ENTROPY: auto-seed with /dev/urandom (if available) or system entropy -- Recommended for new code
{
	// First try getting an array from /dev/urandom
	FILE* urandom = fopen("/dev/urandom", "rb");
	if (urandom)
	{
		uint32_t seed_array[SFMT_N32];
		uint32_t* s = seed_array;
		int i = SFMT_N32;
		bool success = true;
		while (success && i--)
		{
			success = fread(s, sizeof(*s), 1, urandom);
			*s++ &= 0xffffffff;  // filter in case uint32 > 32 bits
		}
		fclose(urandom);
		if (success)
		{
			// There is a 1 in 2^19937 chance that a working urandom gave
			// 19937 consecutive zeroes and will make the generator fail
			// Ignore that case and continue with initialization
			seed(seed_array, _countof(seed_array));
			return;
		}
	}

	// Was not successful; seed with a stronger unique guarantee than
	// seed( hash( time(NULL), clock() ) );
	unique_seed();
}

inline void SFMTRand::unique_seed() // HIGH-ENTROPY: nondeterministic seed interface based on system state
{
	deterministic_seed_u64(unique_seedU64(this, reseed_counter));
}

inline void SFMTRand::deterministic_seed_u64(uint64_t seed64) // HIGH-ENTROPY: deterministic seed interface
{
	uint32_t seed_array[SFMT_N32]; // Intentionally uninitialized.
	// The loop below assigns every element before use.
	// Zero-initialization significantly degrades benchmark performance.

	uint64_t x = seed64;
	for (int i = 0; i < _countof(seed_array); ++i)
	{
		// NOTE: Do not split the 64bit value into 2 UINT32 values, truncation is the superior choice.
		//
		// In a mathematically perfect random sequence, every single 32-bit word in the
		// 624-element array should be an independent and identically distributed (i.i.d.) sample.
		//
		// With Truncation: We're taking 624 distinct samples from the SplitMix64 generator.
		//
		// With Splitting: We would take only 312 samples and split them in half. Each half is coupled;
		// they are two different views of the same 64-bit transformation.
		//
		// Therefore truncation is the best option, as it's the only way to maintain the independence of the samples.
		seed_array[i] = (uint32_t)SplitMix64(x);
	}

	seed(seed_array, _countof(seed_array));
}

inline void SFMTRand::deterministic_seed_f64(double seedF64)  // HIGH-ENTROPY: deterministic seed interface
{
	const uint64_t seedU64 = double_to_u64_bits(seedF64);

	return deterministic_seed_u64(seedU64);
}

inline void SFMTRand::deterministic_seed_u32(uint32_t seedU32)  // low-entropy : LEGACY INTEGRATION ONLY - prefer deterministic_seed_u64() for full entropy
{
	deterministic_seed_u64(seedU32);
}

inline void SFMTRand::deterministic_seed_f32(float seedF32)   // low-entropy : LEGACY INTEGRATION ONLY - prefer deterministic_seed_f64() for full entropy
{
	const uint32_t seedU32 = float_to_u32_bits(seedF32);

	return deterministic_seed_u32(seedU32);
}

