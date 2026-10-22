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

		static void InitCanvas32()
		{
			//error_t err = SUCCESS;

			MyDictionary<mychar_t*> dConfig;

			//dConfig.SetValue("screen_depth", strdup("32"));
			//dConfig.SetValue("canvas_depth", "32");
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

			dConfig.SetValue("screen_depth", strdup("32"));
			dConfig.SetValue("canvas_depth", strdup("32"));
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
			BitCanvas::New(&m_pBitCanvas32, &dConfig, NULL, NULL);


			Palette palette;
			for (int i = 0; i < 256; ++i)
			{
				palette.SetPaletteEntry(i, RGB32(static_cast<byte_t>(255 - i), static_cast<byte_t>(i), ((255 - i) + 127) % 256));
			}
			m_pBitCanvas32->SetPalette(&palette);
		}

	public:
		TEST_CLASS_INITIALIZE(ClassSetup)
		{
			// Runs ONCE before any test in this class
			Logger::WriteMessage("Class setup...\n");
			InitCanvas32();
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

		const size_t maxEndTest = 100;

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

		void InitializeBenchmarkCanvas32(BitCanvas* pBitCanvas32, MTRand& rng)
		{
			size_t bufferLength = pBitCanvas32->BufferWidth() * pBitCanvas32->BufferHeight();

			for (dword_t pixelOffset = 0; pixelOffset < bufferLength; ++pixelOffset)
			{
				byte_t c = static_cast<byte_t>(rng.randInt(255));
				pBitCanvas32->PutPixel(pixelOffset, c);
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

		TEST_METHOD(TestBitCanvas32BenchmarkCPP)
		{
			MTRand rng(12345);
			size_t dwWidth = m_pBitCanvas32->BufferWidth();
			size_t dwHeight = m_pBitCanvas32->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			InitializeBenchmarkCanvas32(m_pBitCanvas32, rng);

			BenchmarkDoDelta(
				L"CPP_BASELINE",
				[&]()
				{
					DrawRandomPixels(m_pBitCanvas32, rng); // Draw to Write buffer
					m_pBitCanvas32->FlipBuffers(); // Swap the read and write buffers
					m_pBitCanvas32->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkMMXint)
		{
			MTRand rng(12345);
			size_t dwWidth = m_pBitCanvas32->BufferWidth();
			size_t dwHeight = m_pBitCanvas32->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			InitializeBenchmarkCanvas32(m_pBitCanvas32, rng);

			BenchmarkDoDelta(
				L"MMX_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(m_pBitCanvas32, rng);
					m_pBitCanvas32->FlipBuffers();
					m_pBitCanvas32->DoDelta_x86(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkMMXasm)
		{
			MTRand rng(12345);
			size_t dwWidth = m_pBitCanvas32->BufferWidth();
			size_t dwHeight = m_pBitCanvas32->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			InitializeBenchmarkCanvas32(m_pBitCanvas32, rng);

			BenchmarkDoDelta(
				L"MMX_ASSEMBLY",
				[&]()
				{
					DrawRandomPixels(m_pBitCanvas32, rng);
					m_pBitCanvas32->FlipBuffers();
					m_pBitCanvas32->DoDelta_MMX(pPixelMap.get());
				});
		}

		TEST_METHOD(TestBitCanvas32BenchmarkSSEint)
		{
			MTRand rng(12345);
			size_t dwWidth = m_pBitCanvas32->BufferWidth();
			size_t dwHeight = m_pBitCanvas32->BufferHeight();
			std::unique_ptr<PIXELMAP[]> pPixelMap = CreateTransitionTable(dwWidth, dwHeight);

			InitializeBenchmarkCanvas32(m_pBitCanvas32, rng);

			BenchmarkDoDelta(
				L"SSE2_INTRINSICS",
				[&]()
				{
					DrawRandomPixels(m_pBitCanvas32, rng);
					m_pBitCanvas32->FlipBuffers();
					m_pBitCanvas32->DoDelta_SSE(pPixelMap.get());
				});
		}

	};

	BitCanvas* BitCanvasUT::m_pBitCanvas32 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8 = nullptr;
}
