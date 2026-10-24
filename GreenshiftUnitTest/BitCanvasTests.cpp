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


			Palette palette;
			for (int i = 0; i < 256; ++i)
			{
				palette.SetPaletteEntry(i, RGB32(static_cast<byte_t>(255 - i), static_cast<byte_t>(i), ((255 - i) + 127) % 256));
			}
			(*ppBitCanvas)->SetPalette(&palette);
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

					pBitCanvas->PrepareForDoDelta();
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
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
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
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

					pBitCanvas->PrepareForDoDelta();
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
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
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
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

					pBitCanvas->PrepareForDoDelta();
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
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

					pBitCanvas->PrepareForDoDelta();
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
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
					pBitCanvas->DoDelta_cpp(pPixelMap.get()); // Read from ReadBuffer
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
					pBitCanvas->DoDelta_x86(pPixelMap.get());
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
					pBitCanvas->DoDelta_MMX(pPixelMap.get());
					pBitCanvas->FinalizeDoDelta();
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

					pBitCanvas->PrepareForDoDelta();
					pBitCanvas->DoDelta_SSE(pPixelMap.get());
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

			Palette palette;
			for (int i = 0; i < 256; ++i)
			{
				palette.SetPaletteEntry(i, RGB32(i, i, i));
			}
			pBitCanvas->SetPalette(&palette);
			FillCanvas(pBitCanvas, 255);
			pBitCanvas->FlipBuffers();
			FillCanvas(pBitCanvas, 255);
			//pBitCanvas->PutPixel(0, 0);
			//pBitCanvas->PutPixel(1, 0);

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
					pBitCanvas->DoDelta_cpp(pPixelMap.get());
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

		TEST_METHOD(TestBitCanvas8FadeCurve)
		{
			constexpr size_t SAMPLE_INTERVAL = 10;
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
			Palette palette;
			for (int i = 0; i < 256; ++i)
			{
				palette.SetPaletteEntry(i, RGB32(i, i, i));
			}
			pBitCanvas->SetPalette(&palette);

			//
			// Fill everything white.
			//
			FillCanvas(pBitCanvas, 255);
			pBitCanvas->FlipBuffers();
			FillCanvas(pBitCanvas, 255);

			auto pTransitionTable =
				CreateTransitionTable(c_width, c_height);

			double totalIntensity = 0.0;

			dword_t width = 0;
			dword_t height = 0;
			dword_t pixelFormat = 0;
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
				pBitCanvas->DoDelta_cpp(
					pTransitionTable.get());
				pBitCanvas->FinalizeDoDelta();

				if ((frame % SAMPLE_INTERVAL) == 0)
				{
					GetTotalMinMaxIntensity(pBitCanvas, totalIntensity, valMin, valMax, pixelCount);

					if (frame == 0)
					{
						valAvg = totalIntensity /
							static_cast<double>(pixelCount);
						std::string message = std::format("Avg:{0}, Min:{1}, Max:{2}, Total:{3}\n", valAvg, valMin, valMax, totalIntensity);
						Logger::WriteMessage(message.c_str());
					}


					intensityCurve.push_back(
						totalIntensity /
						static_cast<double>(pixelCount));
				}
			}

			std::wstring report;

			report += L"8-bit fade curve\n";

			for (size_t i = 0; i < intensityCurve.size(); ++i)
			{
				report +=
					std::to_wstring(i * SAMPLE_INTERVAL) +
					L"," +
					std::to_wstring(intensityCurve[i]) +
					L"\n";
			}

			Logger::WriteMessage(report.c_str());
		}
	};

	BitCanvas* BitCanvasUT::m_pBitCanvas32 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas16Drift = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8 = nullptr;
	BitCanvas* BitCanvasUT::m_pBitCanvas8BitCurve = nullptr;
}
