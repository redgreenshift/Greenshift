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

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<>
			std::wstring ToString<error_t>(const error_t& v)
			{
				return std::to_wstring(v);
			}
		}
	}
}

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
			//MyDictionary<mychar_t*> dConfig;
			//dict.Add(new Association<mychar_t*>());

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


			//dConfig.SetValue("screen_depth", "32");
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
			std::array< std::tuple<std::string, std::string>, 20> initialValues =
			{
				{
					{"screen_depth", "32"},
					{"canvas_depth", "32"},
					{"buffer_width", "800"},
					{"buffer_height", "600"},
					{"canvas_width", "512"},
					{"canvas_height", "512"},
					{"canvas_aspect", "1"},
					{"window_width", "800"},
					{"window_height", "600"},
					{"screen_width", "800"},
					{"screen_height", "600"},
					{"show_debug_info", "0"},
					{"show_framerate", "0"},
					{"fullscreen", "0"},
					{"maximum_frames", "22"},
					{"default_frames", "22"},
					{"tween_repeat", "4"},
					{"plain_repeat", "1"},
					{"overlay_mode", "0"},
					{"overlay_color_key", "0"},
				}
			};

			for (const std::tuple<std::string, std::string> & pair : initialValues)
			{
				const std::string& key = std::get<0>(pair);
				const std::string& value = std::get<1>(pair);
				dConfig.SetValue(key, value);
			}
			Assert::AreEqual((int)dConfig.Size(), 20, L"Expected to set values");


			std::optional<std::string> val = dConfig.GetValue("canvas_aspect");
			Assert::IsTrue(val.has_value(), L"Picking a value from the middle ought to produce correct results");
			Assert::AreEqual(std::string{ "1" }, val.value(), L"Picking a value from the middle ought to produce correct results");


			for (const std::tuple<std::string, std::string>& pair : initialValues)
			{
				const std::string& key = std::get<0>(pair);
				const std::string& expected_value = std::get<1>(pair);

				std::optional<std::string> result_value = dConfig.GetValue(key);
				Assert::IsTrue(result_value.has_value(), L"Failed to retrieve value");
				Assert::AreEqual(expected_value, result_value.value(), L"Failed to retrieve value");
			}


			std::vector < std::tuple<std::string, std::string> > vecAsArrayResult;
			Assert::AreEqual(SUCCESS, dConfig.AsArray(vecAsArrayResult), L"Failed to serialize back to a sequential list");

			Assert::AreEqual((int)vecAsArrayResult.size(), 20, L"Serializing back to an array/vector should produce the original length");

			// Should be in the SAME ORDER
			for (uint32_t i = 0; i < vecAsArrayResult.size(); ++i)
			{
				const std::tuple<std::string, std::string>& pair = initialValues[i];
				const std::string& expected_key = std::get<0>(pair);
				const std::string& expected_value = std::get<1>(pair);
				const std::string& result_key = std::get<0>(vecAsArrayResult[i]);
				const std::string& result_value = std::get<1>(vecAsArrayResult[i]);

				Assert::AreEqual(expected_key, result_key, L"BAD KEY: Serializing back to an array/vector should produce the original order");
				Assert::AreEqual(expected_value, result_value, L"BAD VALUE: Serializing back to an array/vector should produce the original order");
			}

			// Removing a value and adding another value should result in
			Assert::AreEqual(SUCCESS, dConfig.RemoveValue("canvas_aspect"), L"Failed to remove a value");
			Assert::AreEqual(SUCCESS, dConfig.SetValue("canvas_aspect", "2"), L"Failed to Add a value");

			Assert::AreEqual(SUCCESS, dConfig.AsArray(vecAsArrayResult), L"Failed to serialize back to a sequential list");
			Assert::AreEqual((int)vecAsArrayResult.size(), 20, L"Removing and Adding again should produce the original length");


			// Should be in the SAME *ORDER*, up to the removed value, then that value should be at the end

			// Items prior to the moved item should NOT change index
			const int iMovedItem = 6;
			for (int i = 0; i < iMovedItem; ++i)
			{
				const std::tuple<std::string, std::string>& pair = initialValues[i];
				const std::string& expected_key = std::get<0>(pair);
				const std::string& expected_value = std::get<1>(pair);
				const std::string& result_key = std::get<0>(vecAsArrayResult[i]);
				const std::string& result_value = std::get<1>(vecAsArrayResult[i]);

				Assert::AreEqual(expected_key, result_key, L"BAD KEY: Moving an item should NOT disturb prior items");
				Assert::AreEqual(expected_value, result_value, L"BAD VALUE: Moving an item should NOT disturb prior items");
			}

			// Subsequent items should shift indicies by -1
			for (uint32_t i = iMovedItem + 1; i < vecAsArrayResult.size() - 1; ++i)
			{
				const std::tuple<std::string, std::string>& pair = initialValues[i];
				const std::string& expected_key = std::get<0>(pair);
				const std::string& expected_value = std::get<1>(pair);
				const std::string& result_key = std::get<0>(vecAsArrayResult[i-1]);
				const std::string& result_value = std::get<1>(vecAsArrayResult[i-1]);

				Assert::AreEqual(expected_key, result_key, L"BAD KEY: Moving an item SHOULD move subsequent items");
				Assert::AreEqual(expected_value, result_value, L"BAD VALUE: Moving an item SHOULD move subsequent items");
			}

			// The moved item should now be at the end
			{
				const std::tuple<std::string, std::string>& pair = initialValues[iMovedItem];
				const std::string& expected_key = std::get<0>(pair);
				const std::string& expected_value = "2";
				const std::string& result_key = std::get<0>(vecAsArrayResult[19]);
				const std::string& result_value = std::get<1>(vecAsArrayResult[19]);

				Assert::AreEqual(expected_key, result_key, L"BAD KEY: The moved item should appear last");
				Assert::AreEqual(expected_value, result_value, L"BAD VALUE: The moved item should appear last");
			}

			dConfig.WipeContents();
			Assert::AreEqual((int)dConfig.Size(), 0, L"Expected to clear values");
		}
	};
}
