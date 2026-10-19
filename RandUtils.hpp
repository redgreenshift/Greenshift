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

#include <atomic>
#if __cpp_lib_bit_cast >= 201806L
#include <bit>
#endif
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <thread>   // std::thread::id
#include <version>

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
inline uint64_t SplitMix64(uint64_t& x) {
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
inline uint32_t SplitMix32(uint32_t& x) {
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
inline uint64_t hash_thread_id(std::thread::id tid) {
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
inline uint32_t float_to_u32_bits(float f) {
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<uint32_t>(f);
#else
	uint32_t u;
	std::memcpy(&u, &f, sizeof(u));
	return u;
#endif
}

// Deterministic double->u64 bitcast (no numeric conversion).
inline uint64_t double_to_u64_bits(double f) {
#if __cpp_lib_bit_cast >= 201806L
	return std::bit_cast<uint64_t>(f);
#else
	uint64_t u;
	std::memcpy(&u, &f, sizeof(u));
	return u;
#endif
}

/// <summary>
/// Produces a high-entropy 64-bit seed derived from steady-clock time, a global event counter, a per-instance reseed counter, the caller pointer, and a thread-id hash.
/// </summary>
/// <param name="pThis">Pointer used to decorrelate the seed (typically the calling object address). May be null.</param>
/// <param name="reseed_counter">Reference to a per-instance reseed counter; it is incremented and incorporated into the seed.</param>
/// <returns>A 64-bit seed value computed from the current steady-clock time, global event counter, incremented reseed_counter, pointer address, and thread-id hash.</returns>
inline uint64_t unique_seedU64(const void* pThis, uint64_t& reseed_counter) // HIGH-ENTROPY: nondeterministic seed interface based on system state
{
	uint64_t t = (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
	uint64_t global_event = g_seed_event_counter.fetch_add(1, std::memory_order_acq_rel);
	uint64_t local_reseed = ++reseed_counter;

	uint64_t addr = (uint64_t)(uintptr_t)pThis; // extra decorrelation

	uint64_t tid_hash = hash_thread_id(std::this_thread::get_id());

	uint64_t base = t
		^ (global_event * 0x9e3779b97f4a7c15ULL)
		^ (local_reseed * 0xbf58476d1ce4e5b9ULL)
		^ (addr * 0x94d049bb133111ebULL)
		^ (tid_hash);

	return base;
}

