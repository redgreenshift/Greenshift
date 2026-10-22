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

#include <string.h>
#include <Windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(BitCanvasUT)
	{
		static BitCanvas* m_pBitCanvas32;
		static BitCanvas* m_pBitCanvas16;
		static BitCanvas* m_pBitCanvas8;
		HighResolutionTimer timer;

		static void InitCanvas(BitCanvas** ppBitCanvas, size_t bitDepth)
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

			dConfig.SetValue("screen_depth", strdup(strDepth.c_str()));
			dConfig.SetValue("canvas_depth", strdup(strDepth.c_str()));
			dConfig.SetValue("buffer_width", strdup("800"));
			dConfig.SetValue("buffer_height", strdup("600"));
			dConfig.SetValue("canvas_width", strdup("512"));
			dConfig.SetValue("canvas_height", strdup("512"));
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


			Palette palette;
			for (int i = 0; i < 256; ++i)
			{
				palette.SetPaletteEntry(i, RGB32(static_cast<byte_t>(255 - i), static_cast<byte_t>(i), ((255 - i) + 127) % 256));
			}
			(*ppBitCanvas)->SetPalette(&palette);
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
		}

		TEST_METHOD(TestBitCanvas32ADrawing1)
		{
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
			for (uint32_t i = 0; i < maxEndTest; ++i)
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
		const size_t maxEndTest = 10000;
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
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
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
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkSSEint)
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
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
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
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkCPPExperiment1)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit CPP_Exp1",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas16BenchmarkSSEint)
		{
			MTRand rng(c_seed);
			BitCanvas* pBitCanvas = m_pBitCanvas16;
			size_t dwWidth = pBitCanvas->BufferWidth();
			size_t dwHeight = pBitCanvas->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			ResetBenchmarkCanvas(pBitCanvas, rng);

			BenchmarkDoDelta(
				L"16bit SSE2_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(pBitCanvas, rng);
					pBitCanvas->FlipBuffers();
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
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
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
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
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas8BenchmarkSSEint)
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
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
				});
		}

	};

	BitCanvas* BitCanvasUT::m_pBitCanvas32 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8 = nullptr;
}
