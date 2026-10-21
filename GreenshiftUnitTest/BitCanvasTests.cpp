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
#include "..\MyDictionary.h"
#include "..\Expression.h"
#include "..\GForceFunctions.cpp"
#include "..\Greenshift.cpp"
#include "..\FilingClerk.cpp"
//#include "..\BitCanvas.h"
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
#include "..\Palette.cpp"
#include "..\MetaPalette.cpp"
#include "..\WindowDevice.cpp"
#include "..\WindowDeviceDX.cpp"
#include "..\BitCanvas.cpp"
#include "../detect.c"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(BitCanvasUT)
	{
	public:

		TEST_METHOD(TestBitCanvasDraw1)
		{
#if !UNDEFINED
			BitCanvas* gpBitCanvas = NULL;
			error_t err = SUCCESS;

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
				BitCanvas::New(&gpBitCanvas, &dConfig, NULL, NULL);


				Palette palette;
				for (int i = 0; i < 256; ++i)
				{
					palette.SetPaletteEntry(i, RGB(i, i, i));
				}
				gpBitCanvas->SetPalette(&palette);
				gpBitCanvas->PutPixel(0, 0, 32, 128);

				dword_t dwWidth = 0;
				dword_t dwHeight = 0;
				dword_t dwPixelFormat = 0;
				void * pPixelData = gpBitCanvas->GetPixelMemory(&dwWidth, &dwHeight, &dwPixelFormat);
				//dword_t dwPixelData = RGB(128, 128, 128);
				//void* pPixelData = &dwPixelData;
#else
				dword_t dwPixelData = RGB(128, 128, 128);
				void* pPixelData = &dwPixelData;
#endif

				Assert::AreEqual(RGB32(128, 128, 128), *(rgb32_t*)pPixelData);
		}
	};
}
