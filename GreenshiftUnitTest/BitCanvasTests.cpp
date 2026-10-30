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
#define UNIT_TEST 1 // Conditionally change BitCanvas so we can call private/protected methods
#include "..\Project Greenshift.h"
#include "CppUnitTest.h"
#include <cstdint>
#include <format>
#include <functional>
#include "..\MyDictionary.h"
#include "..\Expression.h"
#include "../ColorUtils.hpp"
#include "../HighResolutionTimer.h"
#include "../MTRand.hpp"
#include "..\GForceFunctions.cpp"
#include "..\Greenshift.cpp"
#include "..\FilingClerk.cpp"
#include "..\BitCanvas.h"
#include "..\MetaConfig.cpp"
#include "..\DeltaField.cpp"
#include "..\MetaDeltaField.cpp"
#include "..\VectorGraphic.cpp"
#include "..\MetaWaveShape.cpp"
#include "..\Particle.cpp"
#include "..\MetaParticle.cpp"
#include "..\ThreadedEntity.cpp"
#include "..\TweenThread.cpp"
#include "..\DeltaTween.cpp"
#include "..\Palette.h"
#include "..\Palette.cpp"
#include "..\MetaPalette.cpp"
#include "..\WindowDevice.cpp"
#include "..\WindowDeviceDX.cpp"
#include "..\BitCanvas.cpp"
#include "../detect.c"

#include <memory>
#include <string>
#include <string.h>
#include <vector>
#include <Windows.h>

dword_t CalculateExpectedPackedDomainPixel(dword_t  srcPixel12, dword_t srcPixel34);

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	using PaletteInitializer =
		void (*)(BitCanvas*);

	using DoDeltaMethod8 =
		void (BitCanvas8::*)(const PIXELMAP*);
	using DoDeltaMethod16 =
		void (BitCanvas16::*)(const PIXELMAP*);
	using DoDeltaMethod32 =
		void (BitCanvas32::*)(const PIXELMAP*);

	TEST_CLASS(BitCanvasUT)
	{
		static BitCanvas* m_pBitCanvas32;
		static BitCanvas* m_pBitCanvas16;
		static BitCanvas* m_pBitCanvas16Drift;
		static BitCanvas* m_pBitCanvas8;
		static BitCanvas* m_pBitCanvas8BitCurve;
		HighResolutionTimer timer;

		static void InitCanvas(BitCanvas** ppBitCanvas, size_t bitDepth, size_t width, size_t height)
		{
			//error_t err = SUCCESS;

			MyDictionary<mychar_t*> dConfig;

			//dConfig.SetValue("screen_depth", strDepth.c_str());
			//dConfig.SetValue("canvas_depth", strDepth.c_str());
			//dConfig.SetValue("buffer_width", "800");
			//dConfig.SetValue("buffer_height", "600");
			//dConfig.SetValue("canvas_width", "512");
			//dConfig.SetValue("canvas_height", "512");
			//dConfig.SetValue("canvas_aspect", "1");
			//dConfig.SetValue("window_width", "800");
			//dConfig.SetValue("window_height", "600");
			//dConfig.SetValue("screen_width", "800");
			//dConfig.SetValue("screen_height", "600");
			//dConfig.SetValue("show_debug_info", "0");
			//dConfig.SetValue("show_framerate", "0");
			//dConfig.SetValue("fullscreen", "0");
			//dConfig.SetValue("maximum_frames", "22");
			//dConfig.SetValue("default_frames", "22");
			//dConfig.SetValue("tween_repeat", "4");
			//dConfig.SetValue("plain_repeat", "1");
			//dConfig.SetValue("overlay_mode", "0");
			//dConfig.SetValue("overlay_color_key", "0");

			std::string strDepth = std::format("{0}", bitDepth);
			std::string strWidth = std::format("{0}", width);
			std::string strHeight = std::format("{0}", height);

			dConfig.SetValue("screen_depth", strdup(strDepth.c_str()));
			dConfig.SetValue("canvas_depth", strdup(strDepth.c_str()));
			dConfig.SetValue("buffer_width", strdup("800"));
			dConfig.SetValue("buffer_height", strdup("600"));
			dConfig.SetValue("canvas_width", strdup(strWidth.c_str()));
			dConfig.SetValue("canvas_height", strdup(strHeight.c_str()));
			dConfig.SetValue("canvas_aspect", strdup("1"));
			dConfig.SetValue("window_width", strdup("800"));
			dConfig.SetValue("window_height", strdup("600"));
			dConfig.SetValue("screen_width", strdup("800"));
			dConfig.SetValue("screen_height", strdup("600"));
			dConfig.SetValue("show_debug_info", strdup("0"));
			dConfig.SetValue("show_framerate", strdup("0"));
			dConfig.SetValue("fullscreen", strdup("0"));
			dConfig.SetValue("maximum_frames", strdup("22"));
			dConfig.SetValue("default_frames", strdup("22"));
			dConfig.SetValue("tween_repeat", strdup("4"));
			dConfig.SetValue("plain_repeat", strdup("1"));
			dConfig.SetValue("overlay_mode", strdup("0"));
			dConfig.SetValue("overlay_color_key", strdup("0"));

			//WindowDevice::New(hInstance, &window_device, "CS4451_a3 - Ray Tracer", &dConfig, NULL, NULL, NULL);
			BitCanvas::New(ppBitCanvas, &dConfig, NULL, NULL);


			SetDistinctChannelPalette(*ppBitCanvas);
		}

		/**@brief
		 * Initializes a palette for validating color-channel correctness.
		 *
		 * Grayscale palettes (R == G == B) are useful when measuring fade
		 * curves and decay behavior, but can conceal bugs such as swapped
		 * channels, incorrect masks, or incorrect bit shifts because all
		 * channels contain the same value.
		 *
		 * This palette intentionally assigns different red, green, and blue
		 * values to every entry so channel-related bugs become obvious.
		 * It is primarily used when verifying that baseline, optimized,
		 * intrinsic, and assembly implementations produce identical results.
		 */
		static void SetDistinctChannelPalette(BitCanvas* pBitCanvas)
		{
			Palette palette;
			byte_t i = 0;
			do
			{
				byte_t r = static_cast<byte_t>(255 - i);
				byte_t g = static_cast<byte_t>(i);
				byte_t b = ((255 - i) + 127) % 256;
				Assert::AreNotEqual(r, g, L"Every channel should be distinct");
				Assert::AreNotEqual(r, b, L"Every channel should be distinct");
				palette.SetPaletteEntry(i, RGB32(r, g, b));
			} while (i++ != 255);
			pBitCanvas->SetPalette(&palette);
		}

		static void SetGrayscalePalette(BitCanvas* pBitCanvas)
		{
			Palette palette;
			byte_t i = 0;
			do
			{
				palette.SetPaletteEntry(i, RGB32(i, i, i));
			} while (i++ != 255);
			pBitCanvas->SetPalette(&palette);
		}

		static void SetPrimaryColorCyclePalette(BitCanvas* pBitCanvas)
		{
			Palette palette;
			byte_t i = 0;
			do
			{
				switch (i % 3)
				{
				case 0:
					palette.SetPaletteEntry(i, RGB32(255, 0, 0));
					break;

				case 1:
					palette.SetPaletteEntry(i, RGB32(0, 255, 0));
					break;

				case 2:
					palette.SetPaletteEntry(i, RGB32(0, 0, 255));
					break;
				}
			} while (i++ != 255);
			pBitCanvas->SetPalette(&palette);
		}

		static void SetBitBoundaryPalette(BitCanvas* pBitCanvas)
		{
			Palette palette;

			static const rgb32_t testColors[] =
			{
				RGB32(0,   0,   0),   // all bits clear
				RGB32(255, 255, 255),   // all bits set

				RGB32(248,   0,   0),   // max 5-bit red
				RGB32(255,   0,   0),   // red saturation

				RGB32(0, 252,   0),   // max 6-bit green
				RGB32(0, 255,   0),   // green saturation

				RGB32(0,   0, 248),   // max 5-bit blue
				RGB32(0,   0, 255),   // blue saturation

				RGB32(128, 128, 128),   // middle values

				RGB32(8,   4,   8),   // low nonzero values

				RGB32(248, 252, 248),   // RGB565 max values

				RGB32(248, 252,   0),   // red+green
				RGB32(248,   0, 248),   // red+blue
				RGB32(0, 252, 248),   // green+blue

				RGB32(255, 127,   0),
				RGB32(127, 255,   0),
			};

			constexpr size_t cColors =
				sizeof(testColors) /
				sizeof(testColors[0]);

			byte_t i = 0;
			do
			{
				palette.SetPaletteEntry(
					i,
					testColors[i % cColors]);
			} while (i++ != 255);

			pBitCanvas->SetPalette(&palette);
		}

		static void InitCanvas(BitCanvas** ppBitCanvas, size_t bitDepth)
		{
			InitCanvas(ppBitCanvas, bitDepth, 512, 512);
		}

		static void InitCanvas32()
		{
			InitCanvas(&m_pBitCanvas32, 32);
		}

		static void InitCanvas16()
		{
			InitCanvas(&m_pBitCanvas16, 16);
		}

		static void InitCanvas8()
		{
			InitCanvas(&m_pBitCanvas8, 8);
		}

	public:
		TEST_CLASS_INITIALIZE(ClassSetup)
		{
			// Runs ONCE before any test in this class
			Logger::WriteMessage("Class setup...\n");
			InitCanvas32();
			InitCanvas16();
			InitCanvas8();
		}

		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Runs ONCE after all tests in this class
			Logger::WriteMessage("Class cleanup...\n");
			delete m_pBitCanvas32;
			m_pBitCanvas32 = nullptr;
			delete m_pBitCanvas16;
			m_pBitCanvas16 = nullptr;
			delete m_pBitCanvas8;
			m_pBitCanvas8 = nullptr;
			delete m_pBitCanvas16Drift;
			m_pBitCanvas16Drift = nullptr;
			delete m_pBitCanvas8BitCurve;
			m_pBitCanvas8BitCurve = nullptr;
		}

		TEST_METHOD(TestBitCanvas32ADrawing1)
		{
			SetDistinctChannelPalette(m_pBitCanvas32);
			m_pBitCanvas32->PutPixel(0, 0, 32, 128);

			dword_t dwWidth = 0;
			dword_t dwHeight = 0;
			dword_t dwPixelFormat = 0;
			void * pPixelData = m_pBitCanvas32->GetPixelMemory(&dwWidth, &dwHeight, &dwPixelFormat); // This gets the WRITE BUFFER

			Assert::AreEqual(RGB32(255 - 128, 128, ((255 - 128) + 127) % 256), *(rgb32_t*)pPixelData);
		}

		std::unique_ptr<PIXELMAP[]> CreateTransitionTable(size_t width, size_t height)
		{
			size_t cPixelMap = width * height;
			auto spTransitionTable =
				std::make_unique<PIXELMAP[]>(cPixelMap);

			for (dword_t c = 0; c < cPixelMap; ++c)
			{
				spTransitionTable[c] = (c + width + 1) % cPixelMap;
			}

			return spTransitionTable;
		}

		void BenchmarkDoDelta(
			const wchar_t* name,
			std::function<void()> fn)
		{
			timer.Start();
			for (uint32_t i = 0; i < maxBenchmarkIterations; ++i)
			{
				fn();
			}
			timer.Stop();

			std::wstring s =
				std::format(L"{0} Elapsed ms = {1}\n",
					name,
					std::to_wstring(timer.StoppedMilliseconds()));
			Logger::WriteMessage(s.c_str());
		}

		void ResetBenchmarkCanvas(BitCanvas* pBitCanvas, MTRand& rng)
		{
			size_t bufferLength = pBitCanvas->BufferWidth() * pBitCanvas->BufferHeight();

			for (dword_t pixelOffset = 0; pixelOffset < bufferLength; ++pixelOffset)
			{
				byte_t c = static_cast<byte_t>(rng.randInt(255));
				pBitCanvas->PutPixel(pixelOffset, c);
			}
		}

		void FillCanvas(BitCanvas* pBitCanvas, byte_t color)
		{
			size_t bufferLength = pBitCanvas->BufferWidth() * pBitCanvas->BufferHeight();

			for (dword_t pixelOffset = 0; pixelOffset < bufferLength; ++pixelOffset)
			{
				pBitCanvas->PutPixel(pixelOffset, color);
			}
		}

		void DrawRandomPixels(BitCanvas* pBitCanvas32, MTRand& rng)
		{
			constexpr size_t c_MinLines = 25;
			constexpr size_t c_AdditionalLines = 25;
			size_t numLines = c_MinLines + rng.randInt(c_AdditionalLines);

			for (size_t i = 0; i < numLines; ++i)
			{
				value_t x1 = static_cast<value_t>((rng.rand() * 2.0) - 1.0),
					y1 = static_cast<value_t>((rng.rand() * 2.0) - 1.0),
					x2 = static_cast<value_t>((rng.rand() * 2.0) - 1.0),
					y2 = static_cast<value_t>((rng.rand() * 2.0) - 1.0),
					line_width = static_cast<value_t>(rng.randInt(5));
				byte_t c = static_cast<byte_t>(rng.randInt(255));
				pBitCanvas32->DrawLine(x1, y1, x2, y2, line_width, c);
			}
		}

		/*********************************************************
		* THIRTY TWO BIT BENCHMARKS
		*/
		const size_t maxBenchmarkIterations = 100;
		const uint32_t c_seed = 12345;

		TEST_METHOD(TestBitCanvas32BenchmarkCPP)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas32;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"32bit CPP_BASELINE",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng); // Draw to Write buffer
					pBitCanvas->FlipBuffers(); // Swap the read and write buffers

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas32*>(pBitCanvas)->DoDelta_cpp_BASELINE(pPixelMap.get()); // Read from ReadBuffer
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkMMXint)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas32;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"32bit MMX_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas32*>(pBitCanvas)->DoDelta_MMX_Intrinsics(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkMMXasm)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas32;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"32bit MMX_ASSEMBLY",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas32*>(pBitCanvas)->DoDelta_MMX_Assembly(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkSSE2int)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas32;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"32bit SSE2_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas32*>(pBitCanvas)->DoDelta_SSE2_Intrinsics_FIXED(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		/************************************************************************
		* SIXTEEN BIT BENCHMARKS
		*/
		TEST_METHOD(TestBitCanvas16BenchmarkCPP)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit CPP_BASELINE",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng); // Draw to Write buffer
					pBitCanvas->FlipBuffers(); // Swap the read and write buffers

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_cpp_BASELINE_WITH_DECAY_63(pPixelMap.get()); // Read from ReadBuffer
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkCPPOptimized)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit CPP_OPTIMIZED",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_cpp_OPTIMIZED_WITH_DECAY_63(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkCPPGreenshiftCorrection)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit Opt Green Correct",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_cpp_OPTIMIZED_WITH_DECAY_63_AND_GREENSHIFT_CORRECTION(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}


		TEST_METHOD(TestBitCanvas16BenchmarkSSE2Exp1)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE2_EXPERIMENT1",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE2_experiment1(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSE2Exp2)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE2_EXPERIMENT2",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE2_experiment2(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSE2Exp4)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE2_EXPERIMENT4",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE2_experiment4(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSE41Exp5)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE41_EXPERIMENT5",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE41_experiment5(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSE2Exp5b)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE2_EXPERIMENT5b",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE2_experiment5b(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSE41Exp6)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE41_EXPERIMENT6",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_SSE41_experiment6(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkAVX2Exp7a)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit AVX2_EXPERIMENT7a",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)->DoDelta_AVX2_experiment7a(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}


		/*************************************************************************
		* EIGHT BIT BENCHMARKS
		*/

		TEST_METHOD(TestBitCanvas8BenchmarkCPP)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas8;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"8bit CPP_BASELINE",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng); // Draw to Write buffer
					pBitCanvas->FlipBuffers(); // Swap the read and write buffers

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas8*>(pBitCanvas)->DoDelta_cpp_BASELINE(pPixelMap.get()); // Read from ReadBuffer
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas8BenchmarkCPPOptimized)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas8;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"8bit CPP_OPTIMIZED",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas8*>(pBitCanvas)->DoDelta_cpp_OPTIMIZED(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas8BenchmarkCPPExperiment1)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas8;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"8bit CPP_Exp1",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas8*>(pBitCanvas)->DoDelta_cpp_experiment1(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		TEST_METHOD(TestBitCanvas8BenchmarkSSE2int)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas8;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"8bit SSE2_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas8*>(pBitCanvas)->DoDelta_SSE2_experiment1(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});
		}

		/// <summary>
		/// Performs a drift analysis by simulating the lifetime of 5-bit and 6-bit values under repeated averaging, then logs the average lifetimes and their ratio.
		/// 
		///     5-bit average lifetime = 4.161290
		///     6 - bit average lifetime = 5.095238
		///     Ratio = 1.224437
		/// </summary>
		TEST_METHOD(GreenshiftDriftAnalysis)
		{
			auto ComputeLifetime =
				[](uint32_t initialValue)
				{
					uint32_t value = initialValue;
					uint32_t iterations = 0;

					while (value != 0)
					{
						// Simulate averaging:
						// two pixels have this value,
						// two pixels are black.
						value = (value + value) >> 2;

						++iterations;
					}

					return iterations;
				};

			uint64_t normal5Total = 0;
			uint64_t normal6Total = 0;

			for (uint32_t v = 1; v <= 31; ++v)
			{
				normal5Total += ComputeLifetime(v);
			}

			for (uint32_t v = 1; v <= 63; ++v)
			{
				normal6Total += ComputeLifetime(v);
			}

			double avg5 =
				static_cast<double>(normal5Total) / 31.0;

			double avg6 =
				static_cast<double>(normal6Total) / 63.0;

			double ratio = avg6 / avg5;

			std::wstring s =
				L"5-bit average lifetime = " +
				std::to_wstring(avg5) +
				L"\n6-bit average lifetime = " +
				std::to_wstring(avg6) +
				L"\nRatio = " +
				std::to_wstring(ratio);

			Logger::WriteMessage(s.c_str());
		}

		/// <summary>
		/// Unit test method that computes and logs the 'lifetime' of two values using a custom ComputeLifetime lambda function.
		/// 
		///     Red lifetime = 5
		///     Green lifetime = 6
		///     Ratio = 1.200000
		/// </summary>
		TEST_METHOD(GreenshiftExtremes)
		{
			auto ComputeLifetime =
				[](uint32_t value)
				{
					uint32_t iterations = 0;

					while (value != 0)
					{
						value = (value + value) >> 2;
						++iterations;
					}

					return iterations;
				};

			uint32_t redLifetime =
				ComputeLifetime(31);

			uint32_t greenLifetime =
				ComputeLifetime(63);

			std::wstring s =
				L"Red lifetime = " +
				std::to_wstring(redLifetime) +
				L"\nGreen lifetime = " +
				std::to_wstring(greenLifetime) +
				L"\nRatio = " +
				std::to_wstring(
					static_cast<double>(greenLifetime) /
					static_cast<double>(redLifetime));

			Logger::WriteMessage(s.c_str());
		}

		TEST_METHOD(TestBitCanvas16FadeToBlack)
		{
			//MTRand rng(c_seed);
			delete m_pBitCanvas16Drift;
			m_pBitCanvas16Drift = nullptr;
			InitCanvas(&m_pBitCanvas16Drift, 16, 4, 4);
			BitCanvas* pBitCanvas = m_pBitCanvas16Drift;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			SetGrayscalePalette(pBitCanvas);

			FillCanvas(pBitCanvas, 255);
			pBitCanvas->FlipBuffers();
			FillCanvas(pBitCanvas, 255);

			size_t frame_count = 0;

			BenchmarkDoDelta(
				L"16bit Fade to Black",
				[&]()
				{
					//DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();
					dword_t px = pBitCanvas->GetPixel(0); // This gets the READ BUFFER
					if (RGB32(0, 0, 0) != px)
						++frame_count;

					pBitCanvas->PrepareForDoDelta();
					static_cast<BitCanvas16*>(pBitCanvas)
						->DoDelta_cpp_BASELINE_WITH_DECAY_63(
							pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
				});

			pBitCanvas->FlipBuffers();
			dword_t px = pBitCanvas->GetPixel(0); // This gets the READ BUFFER

			Assert::AreEqual(RGB32(0, 0, 0), px);

			Assert::AreEqual(63u, frame_count);
		}

		void GetTotalMinMaxIntensity(BitCanvas* pBitCanvas, double& totalIntensity, byte_t& valMin, byte_t& valMax, size_t& pixelCount)
		{
			dword_t width; dword_t height; dword_t pixelFormat;

			byte_t* pPixels =
				static_cast<byte_t*>(
					pBitCanvas->GetPixelMemory(
						&width,
						&height,
						&pixelFormat));

			pixelCount =
				width * height;

			totalIntensity = 0.0;
			valMin = valMax = pPixels[0];

			for (size_t i = 0; i < pixelCount; ++i)
			{
				const byte pixel = pPixels[i];
				if (pixel > valMax)
					valMax = pixel;
				if (pixel < valMin)
					valMin = pixel;
				totalIntensity += pixel;
			}
		}

		/*
			What I'd really add is a helper that extracts the milestone frames:

			Frame when intensity first falls below:

			75%
			50%
			25%
			10%
			5%
			1%

			For example:

			8-bit:

			75% -> frame 48
			50% -> frame 102
			25% -> frame 193
			10% -> frame 311
			5% -> frame 405
			1% -> frame 622

			Then compare those against:

			16-bit 63/64
			16-bit 127/128
			16-bit 511/512

			Whichever set of milestone crossings is closest to the 8-bit reference is your best decay ratio.

			-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
			-=-=-=- Long story short:  I'm abandoning this for now. -=-=-=-
			-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

			BitCanvas8::DoDelta_With_Decay127 takes around 190 frames to fade to black.
			BitCanvas16::DoDelta_With_Decay63 takes around 62 frames to fade to black.
			BitCanvas16::DoDelta_With_Decay1023 takes around 62 frames to fade to black.
			There is no benefit to smaller decay rates, because we're limited by the bit precision in RGB565.
			I can't approximate the 8bit experience in 16bit mode.

			What I did find is that in RGB565 using Decay rate of 63/64 and starting with a white filled buffer:
				1) the RED and BLUE channels fade to 0 in 30 frames.
				2) the GREEN channel fades to 0 in 62 frames.

			If I use a decay rate of 14/15 for the GREEN channel,
			then it will fade at the same rate as the other channels,
			effectively eliminating the "greenshift" bug that the project is named after.

			The 14/15 rate created some awful visual artifacts, so I tried 15/16 instead.

			The 15/16 rate, GREEN fades to 0 after 32 frames, so fairly close. It looks
			correct, and is more easy to optimize with bit shifts, so that is the current
			contender for the "greenshift correction" algorithm.

			The biggest downside is that this further restricts the "time to 0" down to
			~30 frames, so straying even further from the desired 190 frames to match 8bit.

			It was a fun experiment, and now I have the data for "correcting"
			the greenshift effect, so I'll offer that as an option.
		*/
		TEST_METHOD(TestBitCanvas8FadeCurve)
		{
			constexpr size_t SAMPLE_INTERVAL = 1;
			constexpr size_t MAX_FRAMES = 200;
			constexpr size_t c_width = 4;
			constexpr size_t c_height = 4;

			std::vector<double> intensityCurve;

			//
			// Small canvas.
			//
			delete m_pBitCanvas8BitCurve;
			m_pBitCanvas8BitCurve = nullptr;
			InitCanvas(&m_pBitCanvas8BitCurve, 8, c_width, c_height);
			BitCanvas* pBitCanvas = m_pBitCanvas8BitCurve;

			//
			// Grayscale palette.
			//
			SetGrayscalePalette(pBitCanvas);

			//
			// Fill everything white.
			//
			FillCanvas(pBitCanvas, 255);
			pBitCanvas->FlipBuffers();
			FillCanvas(pBitCanvas, 255);

			auto pTransitionTable =
				CreateTransitionTable(c_width, c_height);

			double totalIntensity = 0.0;

			byte_t valMin, valMax;
			size_t pixelCount = 0;

			GetTotalMinMaxIntensity(pBitCanvas, totalIntensity, valMin, valMax, pixelCount);

			double valAvg = totalIntensity /
				static_cast<double>(pixelCount);

			std::string message = std::format("Avg:{0}, Min:{1}, Max:{2}, Total:{3}\n", valAvg, valMin, valMax, totalIntensity);
			Logger::WriteMessage(message.c_str());


			for (size_t frame = 0; frame < MAX_FRAMES; ++frame)
			{
				pBitCanvas->FlipBuffers();

				pBitCanvas->PrepareForDoDelta();
				static_cast<BitCanvas8*>(pBitCanvas)
					->DoDelta_cpp_BASELINE(
						pTransitionTable.get());
				pBitCanvas->FinalizeDoDelta();

				if ((frame % SAMPLE_INTERVAL) == 0)
				{
					GetTotalMinMaxIntensity(pBitCanvas, totalIntensity, valMin, valMax, pixelCount);

					//if (frame == 0)
					{
						valAvg = totalIntensity /
							static_cast<double>(pixelCount);
						message = std::format("Frame{4}, Avg:{0}, Min:{1}, Max:{2}, Total:{3}\n", valAvg, valMin, valMax, totalIntensity, frame);
						Logger::WriteMessage(message.c_str());
					}


					//dword_t px = pBitCanvas->GetPixel(0);
					//intensityCurve.push_back(
					//	double(px) /
					//	static_cast<double>(255));
					intensityCurve.push_back(
						totalIntensity /
						static_cast<double>(pixelCount));
				}
			}

			std::wstring report;

			report += L"8-bit fade curve\n";

			for (size_t i = 0; i < intensityCurve.size(); ++i)
			{
				//if (i < 116)
				//	continue;

				report +=
					std::to_wstring(i * SAMPLE_INTERVAL) +
					L"," +
					std::to_wstring(intensityCurve[i]) +
					L"\n";
			}

			Logger::WriteMessage(report.c_str());
		}


		void Run16BitFadeCurveTest(
			const wchar_t* pName,
			uint32_t decayNumerator,
			uint32_t decayDenominator)
		{
			constexpr size_t SAMPLE_INTERVAL = 1;
			constexpr size_t MAX_FRAMES = 70;
			constexpr size_t c_width = 4;
			constexpr size_t c_height = 4;

			delete m_pBitCanvas16Drift;
			m_pBitCanvas16Drift = nullptr;

			InitCanvas(
				&m_pBitCanvas16Drift,
				16,
				c_width,
				c_height);

			BitCanvas16* pCanvas =
				dynamic_cast<BitCanvas16*>(m_pBitCanvas16Drift);

			Assert::IsNotNull(pCanvas);

			//
			// Grayscale palette.
			//
			SetGrayscalePalette(pCanvas);

			FillCanvas(pCanvas, 255);

			pCanvas->FlipBuffers();

			FillCanvas(pCanvas, 255);

			auto spTransitionTable =
				CreateTransitionTable(c_width, c_height);

			std::wstring s =
				std::wstring(pName) +
				L"\n";

			for (size_t frame = 0; frame < MAX_FRAMES; ++frame)
			{
				pCanvas->PrepareForDoDelta();

				pCanvas->DoDelta_cpp_BASELINE_WITH_DECAY(
					spTransitionTable.get(),
					decayNumerator,
					decayDenominator
				);

				pCanvas->FinalizeDoDelta();

				if ((frame % SAMPLE_INTERVAL) == 0)
				{
					double avgRed = 0.0;
					double avgGreen = 0.0;
					double avgBlue = 0.0;

					// Scan buffer and accumulate channels here.
					double totalRed = 0.0;
					double totalGreen = 0.0;
					double totalBlue = 0.0;

					dword_t width = 0;
					dword_t height = 0;
					dword_t pixelFormat = 0;

					rgb16_t* pPixels =
						static_cast<rgb16_t*>(
							pCanvas->GetPixelMemory(
								&width,
								&height,
								&pixelFormat));

					const size_t pixelCount =
						static_cast<size_t>(width) *
						static_cast<size_t>(height);

					for (size_t i = 0; i < pixelCount; ++i)
					{
						rgb16_t pixel = pPixels[i];

						totalRed += RFromRGB16(pixel);
						totalGreen += GFromRGB16(pixel);
						totalBlue += BFromRGB16(pixel);
					}

					avgRed =
						totalRed / static_cast<double>(pixelCount);

					avgGreen =
						totalGreen / static_cast<double>(pixelCount);

					avgBlue =
						totalBlue / static_cast<double>(pixelCount);

					double avgIntensity =
						((avgRed / 31.0) +
							(avgGreen / 63.0) +
							(avgBlue / 31.0)) / 3.0;

					s +=
						std::to_wstring(frame) +
						L"," +
						std::to_wstring(avgRed) +
						L"," +
						std::to_wstring(avgGreen) +
						L"," +
						std::to_wstring(avgBlue) +
						L"," +
						std::to_wstring(avgIntensity) +
						L"\n";
				}

				pCanvas->FlipBuffers();
			}

			Logger::WriteMessage(s.c_str());
		}

		//TEST_METHOD(TestBitCanvas16FadeCurve_7)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (7/8)",
		//		7,
		//		8);
		//}

		//TEST_METHOD(TestBitCanvas16FadeCurve_13)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (13/14)",
		//		13,
		//		14);
		//}

		TEST_METHOD(TestBitCanvas16FadeCurve_14)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (14/15)",
				14,
				15);
		}

		TEST_METHOD(TestBitCanvas16FadeCurve_15)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (15/16)",
				15,
				16);
		}

		TEST_METHOD(TestBitCanvas16FadeCurve_31)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (31/32)",
				31,
				32);
		}

		// No difference between 63 through 1023, so no benefit from higher values.
		TEST_METHOD(TestBitCanvas16FadeCurve_63)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (63/64)",
				63,
				64);
		}

		//TEST_METHOD(TestBitCanvas16FadeCurve_127)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (127/128)",
		//		127,
		//		128);
		//}

		//TEST_METHOD(TestBitCanvas16FadeCurve_255)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (255/256)",
		//		255,
		//		256);
		//}

		//TEST_METHOD(TestBitCanvas16FadeCurve_511)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (511/512)",
		//		511,
		//		512);
		//}

		TEST_METHOD(TestBitCanvas16FadeCurve_1023)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (1023/1024)",
				1023,
				1024);
		}

		// This one just gets stuck on full WHITE and never reduces the intensity
		//TEST_METHOD(TestBitCanvas16FadeCurve_NoDecay)
		//{
		//	Run16BitFadeCurveTest(
		//		L"16-bit fade curve (NoDecay)",
		//		1,
		//		1);
		//}

		TEST_METHOD(TestBitCanvas16FadeCurve_OneDecay)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (OneDecay)",
				1,
				0);
		}

		TEST_METHOD(TestBitCanvas16FadeCurve_GreenshiftCorrection)
		{
			Run16BitFadeCurveTest(
				L"16-bit fade curve (GreenshiftCorrection)",
				0,
				0);
		}


		void CompareCanvases8(size_t frame, const wchar_t* pszTestName, BitCanvas* pBaselineCanvas, BitCanvas* pOptimizedCanvas)
		{
			dword_t width1 = 0;
			dword_t height1 = 0;
			dword_t pixelFormat1 = 0;

			dword_t width2 = 0;
			dword_t height2 = 0;
			dword_t pixelFormat2 = 0;

			auto* pBaselinePixels =
				static_cast<byte_t*>(
					pBaselineCanvas->GetPixelMemory(
						&width1,
						&height1,
						&pixelFormat1));

			auto* pOptimizedPixels =
				static_cast<byte_t*>(
					pOptimizedCanvas->GetPixelMemory(
						&width2,
						&height2,
						&pixelFormat2));

			const size_t pixelCount =
				static_cast<size_t>(width1) *
				static_cast<size_t>(height1);

			for (size_t i = 0; i < pixelCount; ++i)
			{
				if (pBaselinePixels[i] != pOptimizedPixels[i])
				{
					std::wstring s = std::format(
						L"Test {0} Frame {1}, Pixel {2}: baseline={3}, optimized={4}", pszTestName, frame, i, pBaselinePixels[i], pOptimizedPixels[i]);

					Assert::Fail(s.c_str());
				}
			}
		}

		void CompareCanvases16(size_t frame, const wchar_t* pszTestName, BitCanvas* pBaselineCanvas, BitCanvas* pOptimizedCanvas)
		{
			dword_t width1 = 0;
			dword_t height1 = 0;
			dword_t pixelFormat1 = 0;

			dword_t width2 = 0;
			dword_t height2 = 0;
			dword_t pixelFormat2 = 0;

			auto* pBaselinePixels =
				static_cast<rgb16_t*>(
					pBaselineCanvas->GetPixelMemory(
						&width1,
						&height1,
						&pixelFormat1));

			auto* pOptimizedPixels =
				static_cast<rgb16_t*>(
					pOptimizedCanvas->GetPixelMemory(
						&width2,
						&height2,
						&pixelFormat2));

			const size_t pixelCount =
				static_cast<size_t>(width1) *
				static_cast<size_t>(height1);

			for (size_t i = 0; i < pixelCount; ++i)
			{
				if (pBaselinePixels[i] != pOptimizedPixels[i])
				{
					std::wstring s = std::format(
						L"Test {0} Frame {1}, Pixel {2}: baseline={3}, optimized={4}", pszTestName, frame, i, pBaselinePixels[i], pOptimizedPixels[i]);

					Assert::Fail(s.c_str());
				}
			}
		}

		void CompareCanvases32(size_t frame, const wchar_t* pszTestName, BitCanvas* pBaselineCanvas, BitCanvas* pOptimizedCanvas)
		{
			dword_t width1 = 0;
			dword_t height1 = 0;
			dword_t pixelFormat1 = 0;

			dword_t width2 = 0;
			dword_t height2 = 0;
			dword_t pixelFormat2 = 0;

			auto* pBaselinePixels =
				static_cast<rgb32_t*>(
					pBaselineCanvas->GetPixelMemory(
						&width1,
						&height1,
						&pixelFormat1));

			auto* pOptimizedPixels =
				static_cast<rgb32_t*>(
					pOptimizedCanvas->GetPixelMemory(
						&width2,
						&height2,
						&pixelFormat2));

			const size_t pixelCount =
				static_cast<size_t>(width1) *
				static_cast<size_t>(height1);

			for (size_t i = 0; i < pixelCount; ++i)
			{
				if (pBaselinePixels[i] != pOptimizedPixels[i])
				{
					std::wstring s = std::format(
						L"Test {0} Frame {1}, Pixel {2}: baseline={3}, optimized={4}", pszTestName, frame, i, pBaselinePixels[i], pOptimizedPixels[i]);

					Assert::Fail(s.c_str());
				}
			}
		}

		void VerifyEquivalence8(
			const wchar_t* pszTestName,
			PaletteInitializer pfnPalette,
			DoDeltaMethod8 pfnBaseline,
			DoDeltaMethod8 pfnOptimized)
		{
			constexpr size_t c_width = 32;
			constexpr size_t c_height = 32;
			constexpr size_t c_frames = 1000;

			BitCanvas* pBaselineCanvas = nullptr;
			BitCanvas* pOptimizedCanvas = nullptr;

			InitCanvas(&pBaselineCanvas, 8, c_width, c_height);
			InitCanvas(&pOptimizedCanvas, 8, c_width, c_height);

			pfnPalette(pBaselineCanvas);
			pfnPalette(pOptimizedCanvas);

			//
			// Fill both canvases identically.
			//
			MTRand rng(12345);

			for (size_t y = 0; y < c_height; ++y)
			{
				for (size_t x = 0; x < c_width; ++x)
				{
					byte_t iColor = static_cast<byte_t>(rng.randInt(255));

					pBaselineCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);

					pOptimizedCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);
				}
			}

			pBaselineCanvas->FlipBuffers();
			pOptimizedCanvas->FlipBuffers();

			auto spTransitionTable =
				CreateTransitionTable(c_width, c_height);

			for (size_t frame = 0; frame < c_frames; ++frame)
			{
				pBaselineCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas8*>(pBaselineCanvas)
					->*pfnBaseline)(
						spTransitionTable.get());
				pBaselineCanvas->FinalizeDoDelta();

				pOptimizedCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas8*>(pOptimizedCanvas)
					->*pfnOptimized)(
						spTransitionTable.get());
				pOptimizedCanvas->FinalizeDoDelta();

				CompareCanvases8(
					frame,
					pszTestName,
					pBaselineCanvas,
					pOptimizedCanvas);

				pBaselineCanvas->FlipBuffers();
				pOptimizedCanvas->FlipBuffers();
			}

			delete pBaselineCanvas;
			delete pOptimizedCanvas;
		}

		void VerifyEquivalence16(
			const wchar_t* pszTestName,
			PaletteInitializer pfnPalette,
			DoDeltaMethod16 pfnBaseline,
			DoDeltaMethod16 pfnOptimized)
		{
			constexpr size_t c_width = 32;
			constexpr size_t c_height = 32;
			constexpr size_t c_frames = 1000;

			BitCanvas* pBaselineCanvas = nullptr;
			BitCanvas* pOptimizedCanvas = nullptr;

			InitCanvas(&pBaselineCanvas, 16, c_width, c_height);
			InitCanvas(&pOptimizedCanvas, 16, c_width, c_height);

			pfnPalette(pBaselineCanvas);
			pfnPalette(pOptimizedCanvas);

			//
			// Fill both canvases identically.
			//
			MTRand rng(12345);

			for (size_t y = 0; y < c_height; ++y)
			{
				for (size_t x = 0; x < c_width; ++x)
				{
					byte_t iColor = static_cast<byte_t>(rng.randInt(255));

					pBaselineCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);

					pOptimizedCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);
				}
			}

			pBaselineCanvas->FlipBuffers();
			pOptimizedCanvas->FlipBuffers();

			auto spTransitionTable =
				CreateTransitionTable(c_width, c_height);

			for (size_t frame = 0; frame < c_frames; ++frame)
			{
				pBaselineCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas16*>(pBaselineCanvas)
					->*pfnBaseline)(
						spTransitionTable.get());
				pBaselineCanvas->FinalizeDoDelta();

				pOptimizedCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas16*>(pOptimizedCanvas)
					->*pfnOptimized)(
						spTransitionTable.get());
				pOptimizedCanvas->FinalizeDoDelta();

				CompareCanvases16(
					frame,
					pszTestName,
					pBaselineCanvas,
					pOptimizedCanvas);

				pBaselineCanvas->FlipBuffers();
				pOptimizedCanvas->FlipBuffers();
			}

			delete pBaselineCanvas;
			delete pOptimizedCanvas;
		}

		void VerifyEquivalence32(
			const wchar_t* pszTestName,
			PaletteInitializer pfnPalette,
			DoDeltaMethod32 pfnBaseline,
			DoDeltaMethod32 pfnOptimized)
		{
			constexpr size_t c_width = 32;
			constexpr size_t c_height = 32;
			constexpr size_t c_frames = 1000;

			BitCanvas* pBaselineCanvas = nullptr;
			BitCanvas* pOptimizedCanvas = nullptr;

			InitCanvas(&pBaselineCanvas, 32, c_width, c_height);
			InitCanvas(&pOptimizedCanvas, 32, c_width, c_height);

			pfnPalette(pBaselineCanvas);
			pfnPalette(pOptimizedCanvas);

			//
			// Fill both canvases identically.
			//
			MTRand rng(12345);

			for (size_t y = 0; y < c_height; ++y)
			{
				for (size_t x = 0; x < c_width; ++x)
				{
					byte_t iColor = static_cast<byte_t>(rng.randInt(255));

					pBaselineCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);

					pOptimizedCanvas->PutPixel(
						static_cast<long>(x),
						static_cast<long>(y),
						iColor);
				}
			}

			pBaselineCanvas->FlipBuffers();
			pOptimizedCanvas->FlipBuffers();

			auto spTransitionTable =
				CreateTransitionTable(c_width, c_height);

			for (size_t frame = 0; frame < c_frames; ++frame)
			{
				pBaselineCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas32*>(pBaselineCanvas)
					->*pfnBaseline)(
						spTransitionTable.get());
				pBaselineCanvas->FinalizeDoDelta();

				pOptimizedCanvas->PrepareForDoDelta();
				(static_cast<BitCanvas32*>(pOptimizedCanvas)
					->*pfnOptimized)(
						spTransitionTable.get());
				pOptimizedCanvas->FinalizeDoDelta();

				CompareCanvases32(
					frame,
					pszTestName,
					pBaselineCanvas,
					pOptimizedCanvas);

				pBaselineCanvas->FlipBuffers();
				pOptimizedCanvas->FlipBuffers();
			}

			delete pBaselineCanvas;
			delete pOptimizedCanvas;
		}

		/************************************
		* VERIFY 8 BIT!
		* Only need to test grayscale palettes, since we're only averaging intensity values, NOT the underlying colors in 8bit
		*/
		TEST_METHOD(TestBitCanvas8_Optimized_Grayscale)
		{
			VerifyEquivalence8(
				L"Optimized Grayscale",
				SetGrayscalePalette,
				&BitCanvas8::DoDelta_cpp_BASELINE, // Decay 127
				&BitCanvas8::DoDelta_cpp_OPTIMIZED); // Decay 127
		}
		TEST_METHOD(TestBitCanvas8_SSE2_Grayscale)
		{
			VerifyEquivalence8(
				L"SSE2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas8::DoDelta_cpp_BASELINE, // Decay 127
				&BitCanvas8::DoDelta_SSE2_experiment1); // Decay 127
		}



		/************************************
		 * VERIFY 16 BIT!
		 */

		TEST_METHOD(TestBitCanvas16_NoDecay_Grayscale)
		{
			VerifyEquivalence16(
				L"NoDecay Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY);
		}
		TEST_METHOD(TestBitCanvas16_NoDecay_DistinctChannels)
		{
			VerifyEquivalence16(
				L"NoDecay DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY);
		}
		TEST_METHOD(TestBitCanvas16_NoDecay_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"NoDecay PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY);
		}
		TEST_METHOD(TestBitCanvas16_NoDecay_BitBoundary)
		{
			VerifyEquivalence16(
				L"NoDecay BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_NO_DECAY,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_NO_DECAY);
		}


		TEST_METHOD(TestBitCanvas16_Decay63_Grayscale)
		{
			VerifyEquivalence16(
				L"Decay63 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63);
		}
		TEST_METHOD(TestBitCanvas16_Decay63_DistinctChannels)
		{
			VerifyEquivalence16(
				L"Decay63 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63);
		}
		TEST_METHOD(TestBitCanvas16_Decay63_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"Decay63 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63);
		}
		TEST_METHOD(TestBitCanvas16_Decay63_BitBoundary)
		{
			VerifyEquivalence16(
				L"Decay63 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_BASELINE_WITH_DECAY_63,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63);
		}

		TEST_METHOD(TestBitCanvas16_SSE2_Exp1_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment1);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp1_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE2 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment1);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp1_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE2 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment1);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp1_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE2 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment1);
		}

		TEST_METHOD(TestBitCanvas16_SSE2_Exp2_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment2);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp2_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE2 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment2);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp2_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE2 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment2);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp2_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE2 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment2);
		}

		TEST_METHOD(TestBitCanvas16_SSE2_Exp4_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment4);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp4_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE2 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment4);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp4_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE2 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment4);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp4_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE2 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment4);
		}


		TEST_METHOD(TestBitCanvas16_SSE41_Exp5_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE41 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment5);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp5_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE41 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment5);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp5_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE41 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment5);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp5_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE41 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment5);
		}


		TEST_METHOD(TestBitCanvas16_SSE2_Exp5b_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment5b);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp5b_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE2 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment5b);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp5b_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE2 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment5b);
		}
		TEST_METHOD(TestBitCanvas16_SSE2_Exp5b_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE2 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE2_experiment5b);
		}


		TEST_METHOD(TestBitCanvas16_SSE41_Exp6_Grayscale)
		{
			VerifyEquivalence16(
				L"SSE41 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment6);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp6_DistinctChannels)
		{
			VerifyEquivalence16(
				L"SSE41 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment6);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp6_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"SSE41 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment6);
		}
		TEST_METHOD(TestBitCanvas16_SSE41_Exp6_BitBoundary)
		{
			VerifyEquivalence16(
				L"SSE41 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_SSE41_experiment6);
		}


		TEST_METHOD(TestBitCanvas16_AVX2_Exp7_Grayscale)
		{
			VerifyEquivalence16(
				L"AVX2 Grayscale",
				SetGrayscalePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_AVX2_experiment7a);
		}
		TEST_METHOD(TestBitCanvas16_AVX2_Exp7_DistinctChannels)
		{
			VerifyEquivalence16(
				L"AVX2 DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_AVX2_experiment7a);
		}
		TEST_METHOD(TestBitCanvas16_AVX2_Exp7_PrimaryColorCycle)
		{
			VerifyEquivalence16(
				L"AVX2 PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_AVX2_experiment7a);
		}
		TEST_METHOD(TestBitCanvas16_AVX2_Exp7_BitBoundary)
		{
			VerifyEquivalence16(
				L"AVX2 BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas16::DoDelta_cpp_OPTIMIZED_WITH_DECAY_63,
				&BitCanvas16::DoDelta_AVX2_experiment7a);
		}


		TEST_METHOD(MathTestAVXMath)
		{
			// White (THIS WORKS)
			dword_t src12 = 0xFFFFFFFF;
			dword_t src34 = 0xFFFFFFFF;
			// Constant intensity within each channel, different intensity between channels WORKS!? (I tried several variations, but I should try the full range)
			//word_t singlePixel = RGB16(11, 21, 0);
			//src12 = (singlePixel << 16) | singlePixel;
			//src34 = (singlePixel << 16) | singlePixel;

			// Varying the intensity within the channels, THIS FAILS! :-(
			src12 = (RGB16(0, 63, 31) << 16) | RGB16(31, 63, 0);
			src34 = (RGB16(31, 0, 31) << 16) | RGB16(0, 63, 31);
			const dword_t expectedDestPixel = CalculateExpectedPackedDomainPixel(src12, src34);
			//__m256i red = _mm256_set1_epi32(0x7c007c00);
			//__m256i green = _mm256_set1_epi32(0x0fc00fc0);
			//__m256i blue = _mm256_set1_epi32(0x003e003e);

			const __m256i redMask =
				_mm256_set1_epi32(0xF800F800);

			const __m256i greenMask =
				_mm256_set1_epi32(0x07E007E0);

			const __m256i blueMask =
				_mm256_set1_epi32(0x001F001F);
			__m256i decay63 = _mm256_set1_epi32(63);

			__m256i srcA /*12_256*/ = _mm256_set1_epi32(src12);
			__m256i srcB /*34_256*/ = _mm256_set1_epi32(src34);

			//
			// Exp5 packed-domain arithmetic.
			//
			__m256i red =
				_mm256_and_si256(srcA, redMask); // get packed red bits for pixels 1 and 2

			red =
				_mm256_srli_epi32(red, 2); // divide by 4, so the next operation(s) don't overflow

			__m256i redPackedSum =
				_mm256_add_epi32(
					red,
					_mm256_srli_epi32(
						_mm256_and_si256(srcB, redMask), // get packed red bits for pixels 3 and 4
						2) // divide those by 4 so the next operations don't overflow
				); // sum the src12 and src34 red values, that have already been divided by 4, to produce the packed average

			//
			// Green path.
			//
			__m256i greenPackedSum =
				_mm256_add_epi32(
					_mm256_and_si256(srcA, greenMask),
					_mm256_and_si256(srcB, greenMask));

			//
			// Blue path.
			//
			__m256i bluePackedSum =
				_mm256_add_epi32(
					_mm256_and_si256(srcA, blueMask),
					_mm256_and_si256(srcB, blueMask));


	/*
	 * sum the high order sum and low order sum, and mask to red bits only
	 */
	//destPixel = (((redLow + redHi) * 63) >> 6) & 0xF800;

			__m256i redHi = _mm256_srli_epi32(redPackedSum, 16);
			__m256i redAverage = _mm256_add_epi32(redPackedSum, redHi);
			//__m256i redDecayedIntermediate1 = _mm256_mullo_epi32(redAverage, decay63);
			//__m256i redDecayedIntermediate2 = _mm256_srli_epi32(redDecayedIntermediate1, 6);

			__m256i greenHi = _mm256_srli_epi32(greenPackedSum, 16);
			__m256i greenSum = _mm256_add_epi32(greenPackedSum, greenHi);
			__m256i blueHi = _mm256_srli_epi32(bluePackedSum, 16);
			__m256i blueSum = _mm256_add_epi32(bluePackedSum, blueHi);

			//
			// Apply decay.
			//
			const __m256i redDecayed =
				_mm256_and_si256(
					_mm256_srli_epi32(
						_mm256_mullo_epi32(redAverage, decay63),
						6),
					_mm256_set1_epi32(0x0000F800));

			const __m256i greenDecayed =
				_mm256_and_si256(
					_mm256_srli_epi32(
						_mm256_mullo_epi32(greenSum, decay63),
						8),
					_mm256_set1_epi32(0x000007E0));

			const __m256i blueDecayed =
				_mm256_and_si256(
					_mm256_srli_epi32(
						_mm256_mullo_epi32(blueSum, decay63),
						8),
					_mm256_set1_epi32(0x0000001F));

			__m256i result = _mm256_or_si256(
				_mm256_or_si256(
					redDecayed,
					greenDecayed),
				blueDecayed);


			union {
				__m128i pixel128;
				dword_t pixel32 = 0;
			};

			__m128i finalPixels = _mm256_castsi256_si128(result);
			_mm_store_si128(&pixel128, finalPixels);

			Assert::AreEqual(expectedDestPixel, pixel32, L"should be equal");
		}


		/************************************
		* VERIFY 32 BIT!
		*/

		TEST_METHOD(TestBitCanvas32_MMX_Intrinsics_Grayscale)
		{
			VerifyEquivalence32(
				L"MMX_Intrinsics Grayscale",
				SetGrayscalePalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_MMX_Intrinsics); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_MMX_Intrinsics_DistinctChannels)
		{
			VerifyEquivalence32(
				L"MMX_Intrinsics DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_MMX_Intrinsics); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_MMX_Intrinsics_PrimaryColorCycle)
		{
			VerifyEquivalence32(
				L"MMX_Intrinsics PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_MMX_Intrinsics); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_MMX_Intrinsics_BitBoundary)
		{
			VerifyEquivalence32(
				L"MMX_Intrinsics BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_MMX_Intrinsics); // Decay 63
		}

		TEST_METHOD(TestBitCanvas32_SSE2_Intrinsics_Grayscale)
		{
			VerifyEquivalence32(
				L"SSE2_Int Grayscale",
				SetGrayscalePalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_SSE2_Intrinsics_FIXED); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_SSE2_Intrinsics_DistinctChannels)
		{
			VerifyEquivalence32(
				L"SSE2_Int DistinctChannels",
				SetDistinctChannelPalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_SSE2_Intrinsics_FIXED); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_SSE2_Intrinsics_PrimaryColorCycle)
		{
			VerifyEquivalence32(
				L"SSE2_Int PrimaryColorCycle",
				SetPrimaryColorCyclePalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_SSE2_Intrinsics_FIXED); // Decay 63
		}
		TEST_METHOD(TestBitCanvas32_SSE2_Intrinsics_BitBoundary)
		{
			VerifyEquivalence32(
				L"SSE2_Int BitBoundary",
				SetBitBoundaryPalette,
				&BitCanvas32::DoDelta_MMX_Assembly, // Decay 63
				&BitCanvas32::DoDelta_SSE2_Intrinsics_FIXED); // Decay 63
		}
	};

	BitCanvas* BitCanvasUT::m_pBitCanvas32 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16Drift = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8BitCurve = nullptr;
}
