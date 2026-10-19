#pragma once
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

#include <stdio.h>
#include <atomic>
#include <cstdint>
#include "RandUtils.hpp"

extern "C++"
{
	static std::atomic<uint64_t> g_instance_counter{ 0 };

	inline unsigned int hash(time_t t, clock_t c)
	{
		// Get a uint32 from t and c
		// Better than uint32(x) in case x is floating point in [0,1]
		// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

		static unsigned int differ = 0;  // guarantee time-based seeds will change

		unsigned int h1 = 0;
		unsigned char* p = (unsigned char*)&t;
		for (size_t i = 0; i < sizeof(t); ++i)
		{
			h1 *= UCHAR_MAX + 2U;
			h1 += p[i];
		}
		unsigned int h2 = 0;
		p = (unsigned char*)&c;
		for (size_t j = 0; j < sizeof(c); ++j)
		{
			h2 *= UCHAR_MAX + 2U;
			h2 += p[j];
		}
		return (h1 + differ++) ^ h2;
	}

	inline uint32_t hash_u64_u32(uint64_t a, uint32_t b) {
		// Mix a with SplitMix64-style finalizer (64 -> 64)
		uint64_t z = a;
		z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
		z = z ^ (z >> 31);

		// Fold to 32 bits and combine with b
		uint32_t x = (uint32_t)z ^ (uint32_t)(z >> 32) ^ b;

		// MurmurHash3 32-bit finalizer (avalanche)
		x ^= x >> 16;
		x *= 0x7feb352dU;
		x ^= x >> 15;
		x *= 0x846ca68bU;
		x ^= x >> 16;

		return x;
	}

	/*
	   From a C-program for MT19937, with initialization improved 2002/1/26.
	   Coded by Takuji Nishimura and Makoto Matsumoto.

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

	/* Period parameters */
#define N 624					/* MT19937 internal state words */
#define M 397					/* period parameter */
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

	class MTRand2002
	{
	public:
		MTRand2002()
			: MTRand2002(hash(time(NULL), clock()))
		{
		}

		MTRand2002(uint32_t seed)
		{
			deterministic_seed_u32(seed);
		}

		uint32_t mt[N] = { 0 }; /* the array for the state vector  */
		int mti = N + 1; /* mti==N+1 means mt[N] is not initialized */

		/* initializes mt[N] with a seed */
		void init_genrand(uint32_t s)
		{
			mt[0] = s & 0xffffffffUL;
			for (mti = 1; mti < N; mti++) {
				mt[mti] =
					(1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
				/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
				/* In the previous versions, MSBs of the seed affect   */
				/* only MSBs of the array mt[].                        */
				/* 2002/01/09 modified by Makoto Matsumoto             */
				mt[mti] &= 0xffffffffUL;
				/* for >32 bit machines */
			}
		}

		/* initialize by an array with array-length */
		/* init_key is the array for initializing keys */
		/* key_length is its length */
		/* slight change for C++, 2004/2/26 */
		void init_by_array(uint32_t init_key[], int key_length)
		{
			int i, j, k;
			init_genrand(19650218UL);
			i = 1; j = 0;
			k = (N > key_length ? N : key_length);
			for (; k; k--) {
				mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
					+ init_key[j] + j; /* non linear */
				mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
				i++; j++;
				if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
				if (j >= key_length) j = 0;
			}
			for (k = N - 1; k; k--) {
				mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL))
					- i; /* non linear */
				mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
				i++;
				if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
			}

			mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
		}


		uint64_t reseed_counter = 0; // guarantee unique seeds

		/**
		 * @brief Completely seed the generator with high-entropy values.
		 *
		 * Generates a unique, high-entropy seed array derived from a combination of system time,
		 * global/local counters, instance identity, and thread context. This ensures that no two
		 * calls produce identical seeds, even when executed simultaneously on multiple threads.
		 */
		inline void unique_seed()
		{
			deterministic_seed_u64(unique_seedU64(this, reseed_counter));
		}


		/**
		 * @brief Completely seed the generator with high-entropy values, using a single integer as input.
		 *
		 * Deterministically generates a high-entropy seed array derived from a single integer.
		 *
		 * NOTE: Using SplitMix64 to fill the Mersenne Twister (MT) array is the gold standard for
		 * this process because it solves a known weakness of the MT: if seeded with a simple
		 * integer or a value with too many zeros, the MT can take a long time to "warm up" and
		 * produce statistically sound numbers. SplitMix64 ensures the initial state is thoroughly
		 * "shuffled."
		 */
		inline void deterministic_seed_u64(uint64_t seed64)
		{
			uint32_t seed_array[N];

			uint64_t x = seed64;
			for (int i = 0; i < N; ++i)
			{
				// NOTE: I considered splitting the 64bit value into 2 UINT32 values,
				// but truncation is the superior choice.
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

		inline void deterministic_seed_u32(uint32_t seed32)
		{
			// SplitMix64 is the superior algorithm, so just call that instead of SplitMix32
			deterministic_seed_u64(static_cast<uint32_t>(seed32));
		}

		inline void deterministic_seed_f32(float seedFloat)
		{
			const uint32_t seed32 = float_to_u32_bits(seedFloat);

			return deterministic_seed_u32(seed32);
		}

		inline void deterministic_seed_f64(double seedFloat)
		{
			const uint64_t seed64 = double_to_u64_bits(seedFloat);

			return deterministic_seed_u64(seed64);
		}

		void seed_full_array_by_time() {
			//constexpr int N = 624;                 // MT19937 internal state words
			uint32_t seed_array[N];

			// Collision-resistant per-instance base:
			uint64_t t = static_cast<uint64_t>(
				std::chrono::steady_clock::now().time_since_epoch().count()
				);
			uint64_t c = g_instance_counter.fetch_add(1, std::memory_order_relaxed);

			// Optional: include object address to diversify further
			uint64_t addr = reinterpret_cast<uintptr_t>(this);

			uint64_t base = t ^ (c * 0x9e3779b97f4a7c15ULL) ^ (addr * 0xbf58476d1ce4e5b9ULL);

			// Expand base into a full well-mixed seed array
			uint64_t x = base;
			for (int i = 0; i < N; i++) {
				seed_array[i] = static_cast<uint32_t>(SplitMix64(x));
			}

			this->init_by_array(seed_array, N);
		}
		/* generates a random number on [0,0xffffffff]-interval */
		uint32_t genrand_int32(void)
		{
			uint32_t y;
			// TODO: REVIEW(JRDV): This static makes it unsafe to use across multiple instances!!!! Must move it to member if I want to use the implementation beyond the Unit Tests.
			static uint32_t mag01[2] = { 0x0UL, MATRIX_A };
			/* mag01[x] = x * MATRIX_A  for x=0,1 */

			if (mti >= N) { /* generate N words at one time */
				int kk;

				if (mti == N + 1)   /* if init_genrand() has not been called, */
					init_genrand(5489UL); /* a default initial seed is used */

				for (kk = 0; kk < N - M; kk++) {
					y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
					mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
				}
				for (; kk < N - 1; kk++) {
					y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
					mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
				}
				y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
				mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

				mti = 0;
			}

			y = mt[mti++];

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return y;
		}

		/* generates a random number on [0,0x7fffffff]-interval */
		long genrand_int31(void)
		{
			return (long)(genrand_int32() >> 1);
		}

		/* generates a random number on [0,1]-real-interval */
		double genrand_real1(void)
		{
			return genrand_int32() * (1.0 / 4294967295.0);
			/* divided by 2^32-1 */
		}

		/* generates a random number on [0,1)-real-interval */
		double genrand_real2(void)
		{
			return genrand_int32() * (1.0 / 4294967296.0);
			/* divided by 2^32 */
		}

		/* generates a random number on (0,1)-real-interval */
		double genrand_real3(void)
		{
			return (((double)genrand_int32()) + 0.5) * (1.0 / 4294967296.0);
			/* divided by 2^32 */
		}

		/* generates a random number on [0,1) with 53-bit resolution*/
		double genrand_res53(void)
		{
			unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;
			return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
		}
		/* These real versions are due to Isaku Wada, 2002/01/09 added */
	};


	/*
	   From a C-program for MT19937, with initialization improved 2002/2/10.
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

	/* Period parameters */
	//#define N 624
	//#define M 397
	//#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

	class MTRand2002Cokus
	{
	public:
		MTRand2002Cokus()
			: MTRand2002Cokus(hash(time(NULL), clock()))
		{
		}

		MTRand2002Cokus(uint32_t seed)
		{
			deterministic_seed_u32(seed);
		}

		uint32_t state[N] = { 0 }; /* the array for the state vector  */
		int left = 1;
		int initf = 0;
		uint32_t* next = nullptr;

		/* initializes state[N] with a seed */
		void init_genrand(uint32_t s)
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
			left = 1; initf = 1;
		}

		/* initialize by an array with array-length */
		/* init_key is the array for initializing keys */
		/* key_length is its length */
		/* slight change for C++, 2004/2/26 */
		void init_by_array(uint32_t init_key[], int key_length)
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
			left = 1; initf = 1;
		}

		void next_state(void)
		{
			uint32_t* p = state;
			int j;

			/* if init_genrand() has not been called, */
			/* a default initial seed is used         */
			if (initf == 0) init_genrand(5489UL);

			left = N;
			next = state;

			for (j = N - M + 1; --j; p++)
				*p = p[M] ^ TWIST(p[0], p[1]);

			for (j = M; --j; p++)
				*p = p[M - N] ^ TWIST(p[0], p[1]);

			*p = p[M - N] ^ TWIST(p[0], state[0]);
		}

		/* generates a random number on [0,0xffffffff]-interval */
		uint32_t genrand_int32(void)
		{
			uint32_t y;

			if (--left == 0) next_state();
			y = *next++;

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return y;
		}

		/* generates a random number on [0,0x7fffffff]-interval */
		long genrand_int31(void)
		{
			unsigned long y;

			if (--left == 0) next_state();
			y = *next++;

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return (long)(y >> 1);
		}

		/* generates a random number on [0,1]-real-interval */
		double genrand_real1(void)
		{
			unsigned long y;

			if (--left == 0) next_state();
			y = *next++;

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return (double)y * (1.0 / 4294967295.0);
			/* divided by 2^32-1 */
		}

		/* generates a random number on [0,1)-real-interval */
		double genrand_real2(void)
		{
			unsigned long y;

			if (--left == 0) next_state();
			y = *next++;

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return (double)y * (1.0 / 4294967296.0);
			/* divided by 2^32 */
		}

		/* generates a random number on (0,1)-real-interval */
		double genrand_real3(void)
		{
			unsigned long y;

			if (--left == 0) next_state();
			y = *next++;

			/* Tempering */
			y ^= (y >> 11);
			y ^= (y << 7) & 0x9d2c5680UL;
			y ^= (y << 15) & 0xefc60000UL;
			y ^= (y >> 18);

			return ((double)y + 0.5) * (1.0 / 4294967296.0);
			/* divided by 2^32 */
		}

		/* generates a random number on [0,1) with 53-bit resolution*/
		double genrand_res53(void)
		{
			unsigned long a = genrand_int32() >> 5, b = genrand_int32() >> 6;
			return(a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
		}
		/* These real versions are due to Isaku Wada, 2002/01/09 added */

		uint64_t reseed_counter = 0; // guarantee unique seeds

		/**
		 * @brief Completely seed the generator with high-entropy values.
		 *
		 * Generates a unique, high-entropy seed array derived from a combination of system time,
		 * global/local counters, instance identity, and thread context. This ensures that no two
		 * calls produce identical seeds, even when executed simultaneously on multiple threads.
		 */
		inline void unique_seed()
		{
			deterministic_seed_u64(unique_seedU64(this, reseed_counter));
		}


		/**
		 * @brief Completely seed the generator with high-entropy values, using a single integer as input.
		 *
		 * Deterministically generates a high-entropy seed array derived from a single integer.
		 *
		 * NOTE: Using SplitMix64 to fill the Mersenne Twister (MT) array is the gold standard for
		 * this process because it solves a known weakness of the MT: if seeded with a simple
		 * integer or a value with too many zeros, the MT can take a long time to "warm up" and
		 * produce statistically sound numbers. SplitMix64 ensures the initial state is thoroughly
		 * "shuffled."
		 */
		inline void deterministic_seed_u64(uint64_t seed64)
		{
			uint32_t seed_array[N];

			uint64_t x = seed64;
			for (int i = 0; i < N; ++i)
			{
				// NOTE: I considered splitting the 64bit value into 2 UINT32 values,
				// but truncation is the superior choice.
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

		inline void deterministic_seed_u32(uint32_t seed32)
		{
			// SplitMix64 is the superior algorithm, so just call that instead of SplitMix32
			deterministic_seed_u64(static_cast<uint32_t>(seed32));
		}

		inline void deterministic_seed_f32(float seedFloat)
		{
			const uint32_t seed32 = float_to_u32_bits(seedFloat);

			return deterministic_seed_u32(seed32);
		}

		inline void deterministic_seed_f64(double seedFloat)
		{
			const uint64_t seed64 = double_to_u64_bits(seedFloat);

			return deterministic_seed_u64(seed64);
		}

	};

}

#undef N
#undef M
#undef MATRIX_A
#undef UPPER_MASK
#undef LOWER_MASK
#undef UMASK
#undef LMASK
#undef MIXBITS
#undef TWIST

