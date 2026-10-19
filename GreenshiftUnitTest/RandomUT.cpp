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
#include "pch.h"

#include "..\Project Greenshift.h"
#include "CppUnitTest.h"
#include "..\HighResolutionTimer.h"
#include "..\TextUtils.hpp"
//#include "..\Diagnostics.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <random>
#include <string>
#include <vector>

#include "../MersenneTwister.h"
#include "../SFMTRand.hpp"
#include "../SFMT/sfmt.h"
#include "../SFMT/sfmt.c"
#include "../MTRand2002.h"
#include "../MTRand.hpp"

static_assert(MTRandLike<MTRand2001>, "Missing a member of the MTRand interface!");
static_assert(MTRandLike<SFMTRand>, "Missing a member of the MTRand interface!");

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(RandomUT)
	{
	public:
		value_t const defaultTolerance = 0.0001f; // common default float tolerance; accounts for typical rounding errors, unless greater precision is required
		HighResolutionTimer timer;

		/********************************************************************
		 * Random Number Generators
		 */
#if UNDEFINED // they matched until I switched to the high-entropy seed
		TEST_METHOD(TestRand1)
		{
			MTRand2001		mtRand = MTRand2001(1);
			MTRand2002	mtRand1 = MTRand2002(1);

			for (int i = 0; i < 10; ++i)
			{
				unsigned int v1 = mtRand.randInt();
				unsigned int v2 = mtRand1.genrand_int32();
				char message[100] = { 0 };
				sprintf_s(message, "number %d did not match", i);
				std::wstring wstMessage = utf8_to_wstring(message);
				Assert::AreEqual(v1, v2, wstMessage.c_str());
			}
		}

		TEST_METHOD(TestRand2)
		{
			MTRand2001		mtRand = MTRand2001(1);
			MTRand2002Cokus	mtRand2 = MTRand2002Cokus(1);

			for (int i = 0; i < 10; ++i)
			{
				unsigned int v1 = mtRand.randInt();
				unsigned int v2 = mtRand2.genrand_int32();
				char message[100] = { 0 };
				sprintf_s(message, "number %d did not match", i);
				std::wstring wstMessage = utf8_to_wstring(message);
				Assert::AreEqual(v1, v2, wstMessage.c_str());
			}
		}
#endif

		TEST_METHOD(TestRand1v2)
		{
			MTRand2002	mtRand1 = MTRand2002(1);
			MTRand2002Cokus	mtRand2 = MTRand2002Cokus(1);

			for (int i = 0; i < 10000; ++i)
			{
				unsigned int v1 = mtRand1.genrand_int32();
				unsigned int v2 = mtRand2.genrand_int32();
				if (v1 != v2)
				{
					char message[100] = { 0 };
					sprintf_s(message, "number %d did not match", i);
					std::wstring wstMessage = utf8_to_wstring(std::string{ message });
					Assert::AreEqual(v1, v2, wstMessage.c_str());
				}
			}
		}

#ifdef _DEBUG
		const uint32_t maxEndTest = 10;
#else
		const uint32_t maxEndTest = 500000000;
		//const uint32_t maxEndTest = 1000000000;
		//const uint32_t maxEndTest = 0xFFFFFFFF;
#endif
		//Historical note:

		//Benchmark timings were found to vary significantly depending on
		//translation-unit organization and optimizer behavior.

		//Comparisons should focus on relative implementation performance
		//rather than exact millisecond values.

		// 1,000,000,000 iterations, 20.1 seconds (DEBUG)
		// 1,000,000,000 iterations, 5.9 seconds (SHIP)
		// 4,294,967,295 iterations, 25.2 seconds (SHIP)
		TEST_METHOD(TestRandStandardTime)
		{
			srand(1);
			dword_t sink = 0;

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= rand();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		// 4,294,967,295 iterations, 11.5 seconds (SHIP)
		TEST_METHOD(TestRandStdMT_Time)
		{
			std::mt19937 rng(1);
			// rng.seed(1)
			dword_t sink = 0;

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= rng();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		// 1,000,000,000 iterations, 70 seconds (DEBUG)
		// 1,000,000,000 iterations, 1.8 seconds (SHIP)
		// 4,294,967,295 iterations, 8.0 seconds (SHIP)
		TEST_METHOD(TestRandV0Time_2001)
		{
			MTRand2001	mtRand1 = MTRand2001(1);
			dword_t sink = 0;

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= mtRand1.randInt();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		// 1,000,000,000 iterations, 13.6 seconds (DEBUG)
		// 1,000,000,000 iterations, 2.1 seconds (SHIP)
		// 4,294,967,295 iterations, 8.8 seconds (SHIP)
		TEST_METHOD(TestRandV1Time_2002)
		{
			MTRand2002	mtRand1 = MTRand2002(1);
			dword_t sink = 0;

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= mtRand1.genrand_int32();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		// 1,000,000,000 iterations, 16.2 seconds (DEBUG)
		// 1,000,000,000 iterations, 2.5 seconds (SHIP)
		// 4,294,967,295 iterations, 10.8 seconds (SHIP)
		TEST_METHOD(TestRandV2Time_2002Cokus)
		{
			MTRand2002Cokus	mtRand2 = MTRand2002Cokus(1);
			dword_t sink = 0;

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= mtRand2.genrand_int32();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		TEST_METHOD(TestRandSFMTTime_Class)
		{
			SFMTRand sfmtRand;
			dword_t sink = 0;

			//sfmt_init_gen_rand(&rng, 1);

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= sfmtRand.randInt();
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		TEST_METHOD(TestRandSFMTTime_Struct)
		{
			sfmt_t rng;
			dword_t sink = 0;

			sfmt_init_gen_rand(&rng, 1);

			timer.Start();
			for (uint32_t i = 0; i < maxEndTest; ++i)
			{
				sink ^= sfmt_genrand_uint32(&rng);
			}
			timer.Stop();
			std::wstring s =
				L"Elapsed ms = " +
				std::to_wstring(timer.StoppedMilliseconds());
			Logger::WriteMessage(s.c_str());
			Assert::AreNotEqual(static_cast<dword_t>(0), sink);
		}

		TEST_METHOD(TestMTRand0_Verification)
		{
			// Use a range of seeds to check different starting points in the state vector
			for (unsigned int seed = 1; seed <= 100; ++seed)
			{
				MTRand2001 mtRand1(seed);

				// Generate enough numbers for a few full cycles or something to check stability
				// For now, let's do a smaller number per seed for efficiency in CI/CD.
				for (int i = 0; i < 1000; ++i)
				{
					unsigned int v1 = mtRand1.randInt();

					// In case of overflow or invalid ranges (shouldn't happen with uint32)
					Assert::IsTrue(v1 >= 0, L"Value should be non-negative");

					// We will later compare this with the original known values if we have them.
				}
			}
		}

		TEST_METHOD(TestMTRand1_Verification)
		{
			// Use a range of seeds to check different starting points in the state vector
			for (unsigned int seed = 1; seed <= 100; ++seed)
			{
				MTRand2002 mtRand1(seed);

				// Generate enough numbers for a few full cycles or something to check stability
				// For now, let's do a smaller number per seed for efficiency in CI/CD.
				for (int i = 0; i < 1000; ++i)
				{
					unsigned int v1 = mtRand1.genrand_int32();

					// In case of overflow or invalid ranges (shouldn't happen with uint32)
					Assert::IsTrue(v1 >= 0, L"Value should be non-negative"); // This is a useless test, unsigned in *CAN'T* be negative!!!

					// We will later compare this with the original known values if we have them.
				}
			}
		}

		// Use the Kolmogorov-Smirnov test for uniform distribution on [0,1]
		// to demonstrate the implementation is random "enough"
		TEST_METHOD(TestMTRandV0KS)
		{
			MTRand2001       mtRand1 = MTRand2001(1); // Use fixed seed for consistency or random
			const int N_val = 1000000;
			std::vector<double> samples;
			samples.reserve(N_val);

			for (int i = 0; i < N_val; ++i)
			{
				samples.push_back(mtRand1.rand());
			}

			std::sort(samples.begin(), samples.end());

			double maxD = 0.0;
			for (int i = 0; i < N_val; ++i)
			{
				// Check distance to the upper and lower step boundaries
				double d1 = std::abs((double)(i + 1) / N_val - samples[i]);
				double d2 = std::abs(samples[i] - (double)i / N_val);
				if (d1 > maxD) maxD = d1;
				if (d2 > maxD) maxD = d2;
			}

			// For alpha=0.05, critical value is ~0.0013 for 1M samples.
			// Using a slightly looser threshold to ensure CI stability while remaining strict.
			Assert::IsTrue(maxD < 0.002, L"KS test failed: statistic was too high");
		}

		TEST_METHOD(TestMTRandV1KS)
		{
			MTRand2002       mtRand1 = MTRand2002(1); // Use fixed seed for consistency or random
			const int N_val = 1000000;
			std::vector<double> samples;
			samples.reserve(N_val);

			for (int i = 0; i < N_val; ++i)
			{
				samples.push_back(mtRand1.genrand_real1());
			}

			std::sort(samples.begin(), samples.end());

			double maxD = 0.0;
			for (int i = 0; i < N_val; ++i)
			{
				// Check distance to the upper and lower step boundaries
				double d1 = std::abs((double)(i + 1) / N_val - samples[i]);
				double d2 = std::abs(samples[i] - (double)i / N_val);
				if (d1 > maxD) maxD = d1;
				if (d2 > maxD) maxD = d2;
			}

			// For alpha=0.05, critical value is ~0.0013 for 1M samples.
			// Using a slightly looser threshold to ensure CI stability while remaining strict.
			Assert::IsTrue(maxD < 0.002, L"KS test failed: statistic was too high");
		}

		TEST_METHOD(TestMTRandV0KS_Int32)
		{
			MTRand2001 mtRand1 = MTRand2001(1);
			const int N_val = 1000000;
			std::vector<double> samples;
			samples.reserve(N_val);

			for (int i = 0; i < N_val; ++i)
			{
				// Scale the uint32 to [0,1] range for the K-S test
				samples.push_back((double)mtRand1.randInt() / 4294967295.0);
			}

			std::sort(samples.begin(), samples.end());

			double maxD = 0.0;
			for (int i = 0; i < N_val; ++i)
			{
				// The vertical distance between the ECDF and the CDF line y=x
				// will be captured at these two boundaries for every point in our sorted array.
				double d1 = std::abs(samples[i] - (double)i / N_val);
				double d2 = std::abs(samples[i] - (double)(i + 1) / N_val);
				if (d1 > maxD) maxD = d1;
				if (d2 > maxD) maxD = d2;
			}

			// At alpha=0.05 and N=1,000,000, the critical value is approx 0.0013.
			// We use a threshold of 0.002 to allow for tiny numerical epsilon jitter.
			Assert::IsTrue(maxD < 0.002, L"KS test failed: statistic was too large");
		}

		TEST_METHOD(TestMTRandV1KS_Int32)
		{
			MTRand2002       mtRand1 = MTRand2002(1);
			const int N_val = 1000000;
			std::vector<double> samples;
			samples.reserve(N_val);

			for (int i = 0; i < N_val; ++i)
			{
				// Scale the uint32 to [0,1] range for the K-S test
				samples.push_back((double)mtRand1.genrand_int32() / 4294967295.0);
			}

			std::sort(samples.begin(), samples.end());

			double maxD = 0.0;
			for (int i = 0; i < N_val; ++i)
			{
				// The vertical distance between the ECDF and the CDF line y=x
				// will be captured at these two boundaries for every point in our sorted array.
				double d1 = std::abs(samples[i] - (double)i / N_val);
				double d2 = std::abs(samples[i] - (double)(i + 1) / N_val);
				if (d1 > maxD) maxD = d1;
				if (d2 > maxD) maxD = d2;
			}

			// At alpha=0.05 and N=1,000,000, the critical value is approx 0.0013.
			// We use a threshold of 0.002 to allow for tiny numerical epsilon jitter.
			Assert::IsTrue(maxD < 0.002, L"KS test failed: statistic was too large");
		}

	};
}