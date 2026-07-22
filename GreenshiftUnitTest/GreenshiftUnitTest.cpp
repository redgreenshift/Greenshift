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
#include "..\LinearMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(GreenshiftUnitTest)
	{
	public:

		TEST_METHOD(Test1)
		{
			Assert::AreEqual("TODO", "TODO");
		}

		TEST_METHOD(TestDictionary)
		{
			LinearMap<std::string, std::string> dConfig;
			//MyDictionary<char*> dConfig;
			//dict.Add(new Association<char*>());

			//cConfig.Add("screen_depth", "32");

			//dConfig.Grow(20);
			// 2026-07-21: I was planning to change the various Collections to use Capacity vs Size correctly.
			// 
			// Actually, changing this may lead to instability. Let me rephrase, this Dictionary code is so *VERY* wrong, that it "works by accident." ;-)
			// Or rather it has "worked" without crashing for so long that I fear /incremental/ changes will break some reason the code "works".
			// I need to *completely* revamp the collection code. So trying to _incrementally_ change this from Capacity() to Size() may destabalize the code.
			// Yes, it's *semantically* correct to change it, but this code was "boring" so (25 YEARS AGO) I coded it to be "barely good enough"
			// so I could concentrate on the interesting code in Greenshift (Expression, DeltaField, BitCanvas, etc...)
			//
			// What I need to do is rewrite the Dictionary class (preserving the feature where everything is serialized as some code relies on NOT changing the order of the elements.)
			//
			// If we store a value for insertion order, and then we can reconstruct that order in the AsArray method.
			// I can have the benefit of both modern code, and the legacy need to produce a list of the associations in insertion order. :P

			dConfig.SetValue("screen_depth", "32");
			dConfig.SetValue("canvas_depth", "32");
			dConfig.SetValue("buffer_width", "800");
			dConfig.SetValue("buffer_height", "600");
			dConfig.SetValue("canvas_width", "512");
			dConfig.SetValue("canvas_height", "512");
			dConfig.SetValue("canvas_aspect", "1");
			dConfig.SetValue("window_width", "800");
			dConfig.SetValue("window_height", "600");
			dConfig.SetValue("screen_width", "800");
			dConfig.SetValue("screen_height", "600");
			dConfig.SetValue("show_debug_info", "0");
			dConfig.SetValue("show_framerate", "0");
			dConfig.SetValue("fullscreen", "0");
			dConfig.SetValue("maximum_frames", "22");
			dConfig.SetValue("default_frames", "22");
			dConfig.SetValue("tween_repeat", "4");
			dConfig.SetValue("plain_repeat", "1");
			dConfig.SetValue("overlay_mode", "0");
			dConfig.SetValue("overlay_color_key", "0");

			Assert::AreEqual((int)dConfig.Size(), 20);

			std::string key = "canvas_aspect";
			std::string val = dConfig.GetValue(key);

			Assert::AreEqual(std::string{ "1" }, val);

			std::vector < std::tuple<std::string, std::string> > vec;
			dConfig.AsArray(vec);

			Assert::AreEqual((int)vec.size(), 20);
		}
	};
}
