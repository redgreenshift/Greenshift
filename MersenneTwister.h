#pragma once
// MersenneTwister.h
// Mersenne Twister random number generator -- a C++ class MTRand
// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
// Richard J. Wagner  v0.6  14 February 2001  rjwagner@writeme.com

// The Mersenne Twister is an algorithm for generating random numbers.  It
// was designed with consideration of the flaws in various other generators.
// The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
// are far greater.  The generator is also fast; it avoids multiplication and
// division, and it benefits from caches and pipelines.  For more information
// see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

// Reference
// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
// Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
// Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

// Copyright (C) 2001  Richard J. Wagner
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// The original code included the following notice:
//
//     Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura.
//     When you use this, send an email to: matumoto@math.keio.ac.jp
//     with an appropriate reference to your work.
//
// It would be nice to CC: rjwagner@writeme.com and Cokus@math.washington.edu
// when you write.

#ifndef MERSENNETWISTER_H
#define MERSENNETWISTER_H

// Not thread safe (unless auto-initialization is avoided and each thread has
// its own MTRand object)

#include <stdio.h>
#include <time.h>
#include <limits.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint> // uint32_t
#if __cpp_lib_bit_cast < 201806L
#include <cstring>  // memcpy if needed
#endif
#include <iostream>
#include <thread>   // std::thread::id

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 5033) // "'register' is no longer a supported storage class"
#endif

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

inline std::atomic<uint64_t> g_seed_event_counter{ 0 };

/**
 * @brief Generates a 64-bit pseudorandom value using the SplitMix64 algorithm;
 * a high-performance, non-cryptographic pseudorandom number generator (PRNG)
 * that functions as a counter-based generator.
 *
 * This function updates the provided state and applies a mixing function
 * to produce a high-quality 64-bit hash.
 *
 * These specific operations are chosen to maximize the avalanche effect,
 * ensuring that even a difference of a single bit in the input state results
 * in an output where roughly half of the bits flip.
 *
 * @param x The current state of the generator, updated in-place.
 * @return A mixed 64-bit unsigned integer.
 *
 * NOTE: SplitMix64 is widely respected for its balance of speed and statistical quality,
 * making it a common choice for seeding more complex generators (such as the Xoshiro family)
 * or for use in environments where multiple independent streams of random numbers are needed.
 */
static inline uint64_t SplitMix64(uint64_t& x) {
	uint64_t z = (x += 0x9e3779b97f4a7c15ULL);   // increment state by a value derived from the golden ratio, so it will not repeat a sequence until every possible 64-bit value has been used
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; /* 1011111101011000010001110110110100011100111001001110010110111001 */
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; /* 1001010011010000010010011011101100010011001100010001000111101011 */
	return z ^ (z >> 31);
}

/**
 * @brief Generates a 32-bit pseudorandom value using the SplitMix32 algorithm.
 *
 * This function updates the provided state and applies a mixing function
 * to produce a high-quality 32-bit hash.
 *
 * @param x The current state of the generator, updated in-place.
 * @return A mixed 32-bit unsigned integer.
 *
 * IMPORTANT NOTE: If you are aiming for statistical purity, you should almost
 * always prefer SplitMix64 with truncation over SplitMix32.
 */
static inline uint32_t SplitMix32(uint32_t& x) {
	x += 0x9e3779b9u;
	uint32_t z = x;
	z = (z ^ (z >> 16)) * 0x85ebca6bu; // 10000101111010111100101001101011
	z = (z ^ (z >> 13)) * 0xc2b2ae35u; // 11000010101100101010111000110101
	z = z ^ (z >> 16);
	return z;
}

/**
 * @brief Hashes a thread identifier into a 64-bit unsigned integer.
 *
 * Converts the opaque std::thread::id into a numeric representation and
 * applies an avalanche function to ensure a uniform distribution of bits.
 *
 * NOTE: This uses the MurmurHash3 64-bit finalizer, which is better than
 * trying to use SplitMix64 to generate a single hash value.
 *
 * @param tid The thread identifier to hash.
 * @return A 64-bit hash of the thread ID.
 */
static inline uint64_t hash_thread_id(std::thread::id tid) {
	// Use the standard-defined way to get a numeric representation of the thread ID.
	// We cast to uint64_t to ensure we have a consistent starting width for the mixer.
	uint64_t x = static_cast<uint64_t>(std::hash<std::thread::id>{}(tid));

	// MurmurHash3 64-bit finalizer (The "Avalanche" stage)
	// This ensures that even if std::hash is a simple pass-through,
	// the resulting seed is uniformly distributed.
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccdULL;
	x ^= x >> 33;
	x *= 0xc4ceb9fe1a85ec53ULL;
	x ^= x >> 33;
	return x;
}

// Deterministic float->u32 bitcast (no numeric conversion).
static inline uint32_t float_to_u32_bits(float f) {
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<uint32_t>(f);
#else
	uint32_t u;
	std::memcpy(&u, &f, sizeof(u));
	return u;
#endif
}

// Deterministic double->u64 bitcast (no numeric conversion).
static inline uint64_t double_to_u64_bits(double f) {
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<uint64_t>(f);
#else
	uint64_t u;
	std::memcpy(&u, &f, sizeof(u));
	return u;
#endif
}

// Q: Why not reinterpret cast?
// ANSWER: https://stackoverflow.com/questions/53401654/why-was-stdbit-cast-added-if-reinterpret-cast-could-do-the-same#53402217

class MTRand {
// Data
public:
	typedef uint32_t uint32;  // unsigned integer type, at least 32 bits

	enum { N = 624 };              // length of state vector -- MT19937 internal state words
	enum { SAVE = N + 1 };         // length of array for save()

protected:
	enum { M = 397 };              // period parameter
	enum { MAGIC = 0x9908b0dfU };  // magic constant

	uint32 state[N];       // internal state
	uint32 *pNext = state; // next value to get from state
	int left = 0;          // number of values left before reload needed

	uint64_t reseed_counter = 0; // guarantee unique seeds

//Methods
public:
	MTRand( const uint32& oneSeed );  // initialize with a simple uint32
	MTRand( uint32 *const bigSeed );  // initialize with an array of N uint32's
	MTRand();  // auto-initialize with /dev/urandom or time() and clock()

	// Access to 32-bit random numbers
	// Do NOT use for CRYPTOGRAPHY without securely hashing several returned
	// values together, otherwise the generator state can be learned after
	// reading 624 consecutive values.
	double rand();                      // real number in [0,1]
	double rand( const double& n );     // real number in [0,n]
	double randExc();                   // real number in [0,1)
	double randExc( const double& n );  // real number in [0,n)
	uint32 randInt();                       // integer in [0,2^32-1]
	uint32 randInt( const uint32& n );      // integer in [0,n] for n < 2^32
	double operator()() { return rand(); }  // same as rand()

	// Re-seeding functions with same behavior as initializers
	void seed( uint32 oneSeed );
	void seed( uint32 *const bigSeed );
	void seed();
	void unique_seed(); // nondeterministically seeds the whole array with high entropy
	void deterministic_seed_f32(float seed);   // deterministically seed with high entropy
	void deterministic_seed_u32(uint32 seed);  // deterministically seed with high entropy
	void deterministic_seed_f64(double seed);  // deterministically seed with high entropy
	void deterministic_seed_u64(uint64_t seed);// deterministically seed with high entropy
	void init_genrand(uint32 s);
	void init_by_array(uint32 init_key[], int key_length);

	// Saving and loading generator state
	void save( uint32* saveArray ) const;  // to array of size SAVE
	void load( uint32 *const loadArray );  // from such array
	friend std::ostream& operator<<( std::ostream& os, const MTRand& mtrand );
	friend std::istream& operator>>( std::istream& is, MTRand& mtrand );

protected:
	void reload();
	uint32 hiBit( const uint32& u ) const { return u & 0x80000000U; }
	uint32 loBit( const uint32& u ) const { return u & 0x00000001U; }
	uint32 loBits( const uint32& u ) const { return u & 0x7fffffffU; }
	uint32 mixBits( const uint32& u, const uint32& v ) const
		{ return hiBit(u) | loBits(v); }
	uint32 twist( const uint32& m, const uint32& s0, const uint32& s1 ) const
		{ return m ^ (mixBits(s0,s1)>>1) ^ (loBit(s1) ? MAGIC : 0U); }
};


inline MTRand::MTRand( const uint32& oneSeed )
	{ seed(oneSeed); }

inline MTRand::MTRand( uint32 *const bigSeed )
	{ seed(bigSeed); }

inline MTRand::MTRand()
	{ seed(); }

inline double MTRand::rand()
	{ return double(randInt()) * 2.3283064370807974e-10; }

inline double MTRand::rand( const double& n )
	{ return rand() * n; }

inline double MTRand::randExc()
	{ return double(randInt()) * 2.3283064365386963e-10; }

inline double MTRand::randExc( const double& n )
	{ return randExc() * n; }

inline MTRand::uint32 MTRand::randInt()
{
	if( left == 0 ) reload();
	--left;

	register uint32 s1;
	s1 = *pNext++;
	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}


inline MTRand::uint32 MTRand::randInt( const uint32& n )
{
	// Find which bits are used in n
	uint32 used = ~0;
	for( uint32 m = n; m; used <<= 1, m >>= 1 ) {}
	used = ~used;

	// Draw numbers until one is found in [0,n]
	uint32 i;
	do
		i = randInt() & used;  // toss unused bits to shorten search
	while( i > n );
	return i;
}


inline void MTRand::seed( uint32 oneSeed )
{
	// Seed the generator with a simple uint32
	deterministic_seed_u32(oneSeed);
}


inline void MTRand::seed( uint32 *const bigSeed )
{
	// Seed the generator with an array of 624 uint32's
	// There are 2^19937-1 possible initial states.  This function allows
	// any one of those to be chosen by providing 19937 bits.  The lower
	// 31 bits of the first element, bigSeed[0], are discarded.  Any bits
	// above the lower 32 in each element are also discarded.  Theoretically,
	// the rest of the array can contain any values except all zeroes.
	// Just call seed() if you want to get array from /dev/urandom
	register uint32 *s = state, *b = bigSeed;
	register int i = N;
	for( ; i--; *s++ = *b++ & 0xffffffff ) {}
	reload();
}


inline void MTRand::seed()
{
	// Seed the generator with an array from /dev/urandom if available
	// Otherwise use a fallback

	// First try getting an array from /dev/urandom
	FILE* urandom = fopen( "/dev/urandom", "rb" );
	if( urandom )
	{
		register uint32 *s = state;
		register int i = N;
		register bool success = true;
		while( success && i-- )
		{
			success = fread( s, sizeof(*s), 1, urandom );
			*s++ &= 0xffffffff;  // filter in case uint32 > 32 bits
		}
		fclose(urandom);
		if( success )
		{
			// There is a 1 in 2^19937 chance that a working urandom gave
			// 19937 consecutive zeroes and will make the generator fail
			// Ignore that case and continue with initialization
			reload();
			return;
		}
	}

	// Was not successful; seed with a stronger unique guarantee than
	// seed( hash( time(NULL), clock() ) );
	unique_seed();
}


/**
 * @brief Completely seed the generator with high-entropy values.
 *
 * Generates a unique, high-entropy seed array derived from a combination of system time,
 * global/local counters, instance identity, and thread context. This ensures that no two
 * calls produce identical seeds, even when executed simultaneously on multiple threads.
 */
inline void MTRand::unique_seed()
{
	uint64_t t = (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
	uint64_t global_event = g_seed_event_counter.fetch_add(1, std::memory_order_acq_rel);
	uint64_t local_reseed = ++reseed_counter;

	uint64_t addr = (uint64_t)(uintptr_t)this; // extra decorrelation

	uint64_t tid_hash = hash_thread_id(std::this_thread::get_id());

	uint64_t base = t
		^ (global_event * 0x9e3779b97f4a7c15ULL)
		^ (local_reseed * 0xbf58476d1ce4e5b9ULL)
		^ (addr * 0x94d049bb133111ebULL)
		^ (tid_hash);

	deterministic_seed_u64(base);
}


/**
 * @brief Initializes the internal state array from a 64-bit seed.
 *
 * Deterministically expands the seed into 624 independent 32-bit samples
 * using the SplitMix64 algorithm. Truncation is used for each sample to
 * ensure that every element of the state vector is an independent and
 * identically distributed (i.i.d.) value.
 *
 * NOTE: Using SplitMix64 to fill the Mersenne Twister (MT) array is the gold standard for
 * this process because it solves a known weakness of the MT: if seeded with a simple
 * integer or a value with too many zeros, the MT can take a long time to "warm up" and
 * produce statistically sound numbers. SplitMix64 ensures the initial state is thoroughly
 * "shuffled."
 *
 * @param seed64 The 64-bit seed used to initialize the generator.
 */
inline void MTRand::deterministic_seed_u64(uint64_t seed64)
{
	uint32 seed_array[N];

	uint64_t x = seed64;
	for (int i = 0; i < N; ++i)
	{
		// NOTE: Do not split the 64bit value into 2 UINT32 values, truncation is the superior choice.
		//
		// In a mathematically perfect random sequence, every single 32-bit word in the
		// 624-element array should be an independent and identically distributed (i.i.d.) sample.
		//
		// With Truncation: We're taking 624 distinct samples from the SplitMix64 generator.
		//
		// With Splitting: We are taking only 312 samples and splitting them. Each half is coupled;
		// they are two different views of the same 64-bit transformation.
		//
		// Therefore truncation is the best option, as it's the only way to maintain the independence of the samples.
		seed_array[i] = (uint32_t)SplitMix64(x);
	}

	init_by_array(seed_array, N);
}

inline void MTRand::deterministic_seed_u32(uint32 seed32)
{
	// SplitMix64 is the superior algorithm, so just call that instead of SplitMix32;
	// in a 64-bit space, there is far more "room" for the avalanche effect to occur.
	deterministic_seed_u64(seed32);
}

inline void MTRand::deterministic_seed_f32(float seedFloat)
{
	const uint32_t seed32 = float_to_u32_bits(seedFloat);

	return deterministic_seed_u32(seed32);
}

inline void MTRand::deterministic_seed_f64(double seedFloat)
{
	const uint64_t seed64 = double_to_u64_bits(seedFloat);

	return deterministic_seed_u64(seed64);
}


#pragma region 2002-version mt19937ar-cok init implementations
// Both are based on https://web.archive.org/web/20040609124912/http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/MTARCOK/mt19937ar-cok.c
// "Until 2001/4/6, MT had been distributed under GNU Public License, but after
// 2001/4/6, we decided to let MT be used for any purpose, including commercial use.
// 2002-versions mt19937ar.c, mt19937ar-cok.c are considered to be usable freely."

/*
   A C-program for MT19937, with initialization improved 2002/2/10.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   This is a faster version by taking Shawn Cokus's optimization,
   Matthe Bellew's simplification, Isaku Wada's real version.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

	 1. Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

	 2. Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

	 3. The names of its contributors may not be used to endorse or promote
		products derived from this software without specific prior written
		permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

/* initializes state[N] with a seed */
inline void MTRand::init_genrand(uint32 s)
{
	int j;
	state[0] = s & 0xffffffffUL;
	for (j = 1; j < N; j++) {
		state[j] = (1812433253UL * (state[j - 1] ^ (state[j - 1] >> 30)) + j);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array state[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		state[j] &= 0xffffffffUL;  /* for >32 bit machines */
	}
	left = 0; // done initializing
	reload();
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
inline void MTRand::init_by_array(uint32 init_key[], int key_length)
{
	int i, j, k;
	init_genrand(19650218UL);
	i = 1; j = 0;
	k = (N > key_length ? N : key_length);
	for (; k; k--) {
		state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL))
			+ init_key[j] + j; /* non linear */
		state[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
		i++; j++;
		if (i >= N) { state[0] = state[N - 1]; i = 1; }
		if (j >= key_length) j = 0;
	}
	for (k = N - 1; k; k--) {
		state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL))
			- i; /* non linear */
		state[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
		i++;
		if (i >= N) { state[0] = state[N - 1]; i = 1; }
	}

	state[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
	left = 0; // done initializing
	reload();
}
#pragma endregion // 2002-version mt19937ar-cok init implementations


inline void MTRand::reload()
{
	// Generate N new values in state
	// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
	register uint32 *p = state;
	register int i;
	for( i = N - M; i--; )
		*p++ = twist( p[M], p[0], p[1] );
	for( i = M; --i; )
		*p++ = twist( p[M-N], p[0], p[1] );
	*p = twist( p[M-N], p[0], state[0] );

	left = N, pNext = state;
}


inline void MTRand::save( uint32* saveArray ) const
{
	register uint32 *sa = saveArray;
	register const uint32 *s = state;
	register int i = N;
	for( ; i--; *sa++ = *s++ ) {}
	*sa = left;
}


inline void MTRand::load( uint32 *const loadArray )
{
	register uint32 *s = state;
	register uint32 *la = loadArray;
	register int i = N;
	for( ; i--; *s++ = *la++ ) {}
	left = *la;
	pNext = &state[N-left];
}


inline std::ostream& operator<<( std::ostream& os, const MTRand& mtrand )
{
	register const MTRand::uint32 *s = mtrand.state;
	register int i = mtrand.N;
	for( ; i--; os << *s++ << "\t" ) {}
	return os << mtrand.left;
}


inline std::istream& operator>>( std::istream& is, MTRand& mtrand )
{
	register MTRand::uint32 *s = mtrand.state;
	register int i = mtrand.N;
	for( ; i--; is >> *s++ ) {}
	is >> mtrand.left;
	mtrand.pNext = &mtrand.state[mtrand.N-mtrand.left];
	return is;
}

#pragma pop_macro("max")
#pragma pop_macro("min")

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif  //MERSENNETWISTER_H

// Change log:
//
// v0.1 - First release on 15 May 2000
//      - Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
//      - Translated from C to C++
//      - Made completely ANSI compliant
//      - Designed convenient interface for initialization, seeding, and
//        obtaining numbers in default or user-defined ranges
//      - Added automatic seeding from /dev/urandom or time() and clock()
//      - Provided functions for saving and loading generator state
//
// v0.2 - Fixed bug which reloaded generator one step too late
//
// v0.3 - Switched to clearer, faster reload() code from Matthew Bellew
//
// v0.4 - Removed trailing newline in saved generator format to be consistent
//        with output format of built-in types
//
// v0.5 - Improved portability by replacing static const int's with enum's and
//        clarifying return values in seed(); suggested by Eric Heimburg
//      - Removed MAXINT constant; use 0xffffffff instead
//
// v0.6 - Eliminated seed overflow when uint32 is larger than 32 bits
//      - Changed integer [0,n] generator to give better uniformity
//
// v0.7 - use the seed init_genrand() / init_by_array() implementations from the
//        2002-version of mt19937ar-cok.c
//      - add SplitMix64() and unique_seed() : high-entropy seed ensures no two calls
//        produce identical seeds (use as fallback instead of hash(time(),clock()),
//        and guarantees no warmup period to produce statistically sound numbers.
//      - add 32-bit high-entropy deterministic seed functions (uint32 & float)
//      - add 64-bit high-entropy deterministic seed functions (uint64 & double)
//      - standardize on uint32_t instead of unsigned long, to guarantee 32bits.
//      - Delete the old MTRand::hash function (low entropy, poor avalanche effect,
//        and data race condition for `static uint32 differ`);
//      - Replace the body of the low entropy function `MTRand::seed(uint32 oneSeed)`
//        with the high-entropy `MTRand::deterministic_seed_u32(oneSeed);`
