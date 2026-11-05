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

#include "CpuFeatures.hpp"

#include "adetect.h"
#include <cstdint>
#include <immintrin.h> // _xgetbv

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif


namespace // CPUID Helpers
{
	bool cpuid_has_sse2()
	{
#if defined(_M_X64) || defined(_M_AMD64)
		return true;  // SSE2 is guaranteed on x64
#else
		int regs[4];
		__cpuid(regs, 1);  // CPUID leaf 1
		return (regs[3] & (1 << 26)) != 0;  // EDX bit 26 = SSE2
#endif
	}

	bool cpuid_has_ssse3()
	{
#if defined(_M_X64) || defined(_M_IX86) || \
    defined(__x86_64__) || defined(__i386__)

#if defined(_MSC_VER)
		unsigned int ecx;
		int regs[4];
		__cpuid(regs, 0);
		if (regs[0] < 1)
			return false;

		__cpuid(regs, 1);
		ecx = static_cast<unsigned int>(regs[2]);
#else
		unsigned int eax, ebx, ecx, edx;
		if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx))
			return false;
#endif

		return (ecx & (1u << 9)) != 0; // CPUID leaf 1, ECX bit 9
#else
		return false;
#endif
	}

	bool cpuid_has_sse41() {
		int regs[4];
		__cpuid(regs, 1);
		return (regs[2] & (1 << 19)) != 0; // ECX bit 19
	}

	// With MSVC, AVX2 requires both CPU support and operating-system support for saving AVX state. Check those before executing AVX2 instructions.
	// Current required features: AVX + OSXSAVE + XMM state + YMM state + AVX2
	bool cpuid_has_avx2_required_features()
	{
		int cpuInfo[4];

		__cpuid(cpuInfo, 1);

		const bool osxsave =
			(cpuInfo[2] & (1 << 27)) != 0;

		const bool avx =
			(cpuInfo[2] & (1 << 28)) != 0;

		if (!osxsave || !avx)
			return false;

		const unsigned long long xcr0 =
			_xgetbv(0);

		const bool xmmEnabled =
			(xcr0 & 0x2) != 0;

		const bool ymmEnabled =
			(xcr0 & 0x4) != 0;

		if (!xmmEnabled || !ymmEnabled)
			return false;

		__cpuidex(cpuInfo, 7, 0);

		constexpr unsigned int AVX2 = 1u << 5;
		const bool has_avx2 =
			(cpuInfo[1] & AVX2) != 0;

		return has_avx2;
	}

	/*
	AVX512DQ - additional 32- and 64-bit integer operations
	AVX512CD - conflict-detection operations
	AVX512IFMA - fused multiply-add operations for 52-bit integers
	AVX512VBMI / VBMI2 - byte manipulation and permute operations
	AVX512VNNI - integer dot-product operations
	AVX512BITALG / VPOPCNTDQ - bit algorithms and population counts
	AVX512BF16 / FP16 - bfloat16 and half-precision floating-point operations
	AVX512VP2INTERSECT - vector intersection operations
	AVX512PF / ER / 4VNNIW / 4FMAPS - more specialized subsets, largely associated with Xeon Phi
	*/

	// This detects AVX-512F, the foundation feature. Other AVX-512 extensions-such as AVX512BW or AVX512VL-need their own CPUID-bit checks.
	bool cpuid_has_avx512_foundation()
	{
#if defined(_M_X64) || defined(_M_IX86) || \
    defined(__x86_64__) || defined(__i386__)

		unsigned int max_leaf;

		// Check that CPUID leaf 7 is available.
#if defined(_MSC_VER)
		unsigned int ebx, ecx;
		int regs[4];
		__cpuid(regs, 0);
		max_leaf = static_cast<unsigned int>(regs[0]);
#else
		unsigned int eax, ebx, ecx, edx;
		max_leaf = __get_cpuid_max(0, nullptr);
#endif

		if (max_leaf < 7)
			return false;

		// Leaf 1: OSXSAVE must be set before XGETBV can be used.
#if defined(_MSC_VER)
		__cpuidex(regs, 1, 0);
		ecx = static_cast<unsigned int>(regs[2]);
#else
		if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx))
			return false;
#endif

		constexpr unsigned int OSXSAVE = 1u << 27;
		if ((ecx & OSXSAVE) == 0)
			return false;

		// XCR0 must enable XMM, YMM, opmask, and ZMM state.
		std::uint64_t xcr0;
#if defined(_MSC_VER)
		xcr0 = _xgetbv(0);
#else
		unsigned int xcr0_lo, xcr0_hi;
		__asm__ volatile("xgetbv" : "=a"(xcr0_lo), "=d"(xcr0_hi) : "c"(0));
		xcr0 = (static_cast<std::uint64_t>(xcr0_hi) << 32) | xcr0_lo;
#endif

		constexpr std::uint64_t AVX512_XCR0_MASK = 0xE6;
		if ((xcr0 & AVX512_XCR0_MASK) != AVX512_XCR0_MASK)
			return false;

		// Leaf 7, subleaf 0: AVX512F is EBX bit 16.
#if defined(_MSC_VER)
		__cpuidex(regs, 7, 0);
		ebx = static_cast<unsigned int>(regs[1]);
#else
		__cpuid_count(7, 0, eax, ebx, ecx, edx);
#endif

		constexpr unsigned int AVX512F = 1u << 16;
		return (ebx & AVX512F) != 0;

#else
		return false; // Not an x86/x64 target.
#endif
	}

	// @brief Returns true if AVX-512 Byte and Word instructions are available,
	// and the OS has enabled the required AVX-512 state. Requires AVX512F.
	//
	// AVX512BW adds SIMD operations on packed 8-bit and 16-bit integers,
	// including arithmetic, comparisons, and bitwise operations.
	//
	// AVX512F alone does not provide the full 8/16-bit vector instruction set.
	bool cpuid_has_avx512bw()
	{
		if (!cpuid_has_avx512_foundation())
			return false;

#if defined(_MSC_VER)
		int regs[4];
		__cpuidex(regs, 7, 0);
		const unsigned int ebx = static_cast<unsigned int>(regs[1]);
#else
		unsigned int eax, ebx, ecx, edx;
		__cpuid_count(7, 0, eax, ebx, ecx, edx);
#endif

		constexpr unsigned int AVX512BW = 1u << 30; // Leaf 7, subleaf 0, EBX bit 30
		return (ebx & AVX512BW) != 0;
	}
}

bool SupportsMMX()
{
	return GetCPUCaps(HAS_MMX);
}

bool SupportsSSE2() {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_cpu_supports("sse2");
#else
	return cpuid_has_sse2(); // Use the MSVC CPUID check above, or another platform-specific check.
#endif
}

bool SupportsSSSE3()
{
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_cpu_supports("ssse3");
#else
	// Use the CPUID implementation for other compilers.
	return cpuid_has_ssse3();
#endif
}

bool SupportsSSE41() {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_cpu_supports("sse4.1");
#else
	return cpuid_has_sse41(); // Use the MSVC CPUID version above, or another platform-specific check.
#endif
}

bool SupportsAVX2() {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_cpu_supports("avx2");
#else
	return cpuid_has_avx2_required_features(); // Use the MSVC check above, or another platform-specific check.
#endif
}

// Greenshift currently treats AVX-512 Foundation
// support as sufficient to enable the AVX-512 path.
// Additional feature checks may be required as the
// implementation evolves.
bool SupportsAVX512()
{
	return cpuid_has_avx512bw();
}

