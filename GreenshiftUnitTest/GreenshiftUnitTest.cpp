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
#include "..\PhaseFunction.h"
#include "..\LinearMap.h"

#include "..\StaticFifoSet.hpp"
#include <array>
#include <optional>
#include <string>
#include <vector>

value_t My_abs(value_t nValue);
value_t My_wrap(value_t nValue);

value_t My_mag(value_t nValue)
{
	return My_abs(nValue);
}

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
		MyDictionary<EXPRESSIONDESCRIPTION*> m_dGlobals;

		void InitGlobals()
		{
			EXPRESSIONDESCRIPTION    edGForceFunctions[] =
			{
				{ ED_FUNCTION,  "abs",        1, My_abs  }, /* abs(x)  == |x| */
				{ ED_FUNCTION,  "mag",        1, My_mag  }, /* waveshape data (fake it for UT) */
				{ ED_FUNCTION,  "wrap",       1, My_wrap }, /* wrap(x) == x - flor( x )  (ex: wrap( .3 ) = .3, wrap( 4.12 ) = .12, wrap( - 2.7 ) = .3 ) */

				{ ED_NULL,NULL, 0, NULL }, /* the NULL terminator */
			};

			/************************************************************************
			 *
			 * Load Global MyDictionary with GForce functions to maintain compatibility
			 *
			 ************************************************************************/
			for (size_t i = 0; edGForceFunctions[i].edtType != ED_NULL; i++)
			{
				error_t err = m_dGlobals.SetValue(edGForceFunctions[i].strName,
					&edGForceFunctions[i]);
				if (err != SUCCESS)
					break;
			}
		}

		TEST_METHOD(Test1)
		{
			Assert::AreEqual("TODO", "TODO");
		}

		TEST_METHOD(TestFiniteSet)
		{
			FiniteSet<const char*, 10> set;

			Assert::AreEqual(set.Size(), 0ul, L"Failed to initialize to empty.");

			set.Add("1");
			set.Add("2");
			set.Add("3");
			set.Add("4");
			set.Add("5");
			set.Add("6");
			set.Add("7");
			set.Add("8");
			set.Add("9");
			set.Add("10");
			Assert::AreEqual(set.Size(), 10ul, L"Failed to add all values.");

			set.Add("11");
			Assert::AreEqual(set.Size(), 10ul, L"Failed to maintain specified size");
			Assert::IsTrue(set.Includes("11"), L"Failed to add the new value");
			Assert::IsTrue(set.Includes("2"), L"Failed to purge oldest value first");
			Assert::IsFalse(set.Includes("1"), L"Failed to purge oldest value first");

			set.Add("12");
			Assert::AreEqual(set.Size(), 10ul, L"Failed to maintain specified size");
			Assert::IsTrue(set.Includes("12"), L"Failed to add the new value");
			Assert::IsFalse(set.Includes("2"), L"Failed to purge oldest value first");
		}

		TEST_METHOD(TestRollingSet)
		{
			StaticFifoSet<const char*, 10> set;

			Assert::AreEqual(set.Size(), 0u, L"Failed to initialize to empty.");

			set.Add("1");
			set.Add("2");
			set.Add("3");
			set.Add("4");
			set.Add("5");
			set.Add("6");
			set.Add("7");
			set.Add("8");
			set.Add("9");
			set.Add("10");
			Assert::AreEqual(set.Size(), 10u, L"Failed to add all values.");

			set.Add("11");
			Assert::AreEqual(set.Size(), 10u, L"Failed to maintain specified size");
			Assert::IsTrue(set.Includes("11"), L"Failed to add the new value");
			Assert::IsTrue(set.Includes("2"), L"Failed to purge oldest value first");
			Assert::IsFalse(set.Includes("1"), L"Failed to purge oldest value first");

			set.Add("12");
			Assert::AreEqual(set.Size(), 10u, L"Failed to maintain specified size");
			Assert::IsTrue(set.Includes("12"), L"Failed to add the new value");
			Assert::IsFalse(set.Includes("2"), L"Failed to purge oldest value first");
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

			Assert::AreEqual(20, (int)vecAsArrayResult.size(), L"Serializing back to an array/vector should produce the original length");

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
			Assert::AreEqual(20, (int)vecAsArrayResult.size(), L"Removing and Adding again should produce the original length");


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

			Assert::AreEqual(SUCCESS, dConfig.SetValue("CANVAS_ASPECT", "3"), L"Failed to Add a value");
			Assert::AreEqual(SUCCESS, dConfig.AsArray(vecAsArrayResult), L"Failed to serialize back to a sequential list");
			Assert::AreEqual(20, (int)vecAsArrayResult.size(), L"Setting the same value with different case should produce the original length");

			dConfig.WipeContents();
			Assert::AreEqual(0, (int)dConfig.Size(), L"Expected to clear values");
		}
		TEST_METHOD(TestPhaseFunctionQuestion)
		{
			value_t s = 2;
			value_t t = 3;
			value_t pi = 3.1415926535859f;
			MyDictionary<value_t*> dict;
			MyDictionary<mychar_t*> dConfig;
			PhaseFunction pf;
			//LinearMap<std::string, std::string> dConfig;

			/*  Question Mark  */
			std::array< std::tuple<std::string, std::string>, 42> initialValues =
			{
				{
					{"NAME", "UnitTestQuestionMark"},
					{"ConB", "1"},
					{"Aspc", "1"},
					{"Pen", "1"},
					{"LWdt", "1"},
					{"Stps", "100"},
					{"A0", "2*pi"},
					{"A1", "0.5*pi"},
					//Endpoints for "blobs"
					{"B0", "0.5+ 0.2*cos(-A1)"},
					{"B1", "0.3+ 0.2*sin(-A1)"},

					{"B2", "0.5+ 0.2*cos(A0*0.75-A1)"},
					{"B3", "0.3+ 0.2*sin(A0*0.75-A1)"},
					{"B4", "      0.2*cos(A1)"},
					{"B5", "-.15+ 0.2*sin(A1)"},
					{"B6", "      0.2*cos(A0*0.75+A1)"},
					{"B7", "-.15+ 0.2*sin(A0*0.75+A1)"},
					//cris cross
					{"B8", "(B0+B4)*0.5+0.2"},
					{"B9", "(B1+B5)*0.5"},

					{"C0", "A0*s"},
					{"C1", "0.1*mag(s)"},
					{"C2", "0.2*cos(C0)+C1"},
					{"C3", "0.2*sin(C0)+C1"},
					//Big surrounding circle
					{"X0", "0.85*cos(C0)+ C1"},
					{"Y0", "0.85*sin(C0)+ C1"},
					//Top left circle
					{"X1", "-.5+ C2"},
					{"Y1", "0.3+ C3"},
					//Top middle circle
					{"X2", "     C2"},
					{"Y2", "0.3+ C3"},
					//Blobs
						//Top right circle
						{"X3", "0.5+ 0.2*cos(C0*0.75-A1)+ C1"},
						{"Y3", "0.3+ 0.2*sin(C0*0.75-A1)+ C1"},

						//Middle circle
						{"X4", "      0.2*cos(C0*0.75+A1)+ C1"},
						{"Y4", "-.15+ 0.2*sin(C0*0.75+A1)+ C1+a15"}, // Adding an undefined A15

					//Bottom circle
					{"X5", "     C2"},
					{"Y5", "-.6+ C3"},

					{"C4", "(1-s)^2"},
					{"C5", "(1-s)*s"},
					{"C6", "s*s"},

					//Bezier curve
					{"X6", "B0*C4 + B8*C5 + B6*C6 + C1"},
					{"Y6", "B1*C4 + B9*C5 + B7*C6 + C1"},
					{"X7", "B2*C4 + B8*C5 + B4*C6 + C1"},
					{"Y7", "B3*C4 + B9*C5 + B5*C6 + C1"},

					{"Vers", "100"},
				}
			};

			for (const std::tuple<std::string, std::string>& pair : initialValues)
			{
				const std::string& key = std::get<0>(pair);
				const std::string& value = std::get<1>(pair);
				auto assoc = new Association<mychar_t*>();
				assoc->Initialize(key.c_str(), strdup(value.c_str()));
				dConfig.Add(assoc);
				//dConfig.SetValue(key, value);
			}
			Assert::AreEqual(42, (int)dConfig.Size(), L"Expected to set values");


			dict.SetValue("s", &s);
			dict.SetValue("t", &t);
			dict.SetValue("pi", &pi);

			InitGlobals();

			Assert::AreEqual(SUCCESS, pf.Initialize("ABCD", "XYZ", &dConfig, &dict, &m_dGlobals), L"Initialization failed");

			Assert::AreEqual(4ul, pf.NumPhases(), L"Failed to initialize functions");
			Assert::AreEqual(2ul, pf.NumDimensions(), L"Failed to initialize dimensions");
			Assert::AreEqual(8ul, pf.NumFunctions(), L"Failed to initialize functions");

			Assert::AreEqual(36, (int)dict.Size(), L"Failed to add all expected functions, and/or eliminate unused omitted values");
		}

		TEST_METHOD(TestPhaseFunctionBezier)
		{
			value_t s = 2;
			value_t t = 3;
			value_t pi = 3.1415926535859f;
			value_t BASS = 0;
			MyDictionary<value_t*> dict;
			MyDictionary<mychar_t*> dConfig;
			PhaseFunction pf;
			//LinearMap<std::string, std::string> dConfig;

			/*  Bezier.txt  */
			std::array< std::tuple<std::string, std::string>, 29> initialValues =
			{
				{
					{"NAME", "UnitTestBezier"},
					{"ConB", "1"},
					{"Aspc", "1"},
					{"Pen", "1"},
					{"LWdt", "1"},
					{"Stps", "100"},
					{"A0", "2*pi"},
					{"A1", "0.5*pi"},
					//Endpoints for "blobs"
					{"B0", "-.75+ 0.2*cos(pi*t)-BASS"},
					{"B1", "-.3+ 0.5*sin(pi*t)"},

					{"B2", "-.25+ 0.7*cos(A0*0.75*t-A1)"},
					{"B3", "0.6+ 0.2*sin(A0*0.75*t-A1)+BASS"},
					{"B4", "0.25+ 0.2*cos(A0*t)"},
					{"B5", "-.15+ 0.5*sin(A0*t)-BASS"},
					{"B6", "0.75 + 0.2*cos(A0*0.75*t+A1)+BASS"},
					{"B7", "0.15+ 0.2*sin(A0*0.75*t+A1)"},
					//cris cross
					{"B8", "(B0+B4)*0.5+0.2"},
					{"B9", "(B1+B5)*0.5"},

					{"C0", "0"},
					{"C1", "0.2*mag(s)"},
					{"C2", "0"},
					{"C3", "0"},

					{"C4", "(1-s)^3"},
					{"C5", "(1-s)^2*s"},
					{"C6", "(1-s)*s*s"},
					{"C7", "s*s*s"},

					//Bezier curve
					{"X0", "B0*C4 + B2*C5 + B4*C6 + B6*C7 + C1"},
					{"Y0", "B1*C4 + B3*C5 + B5*C6 + B7*C7 + C1"},

					{"Vers", "100"},
				}
			};

			for (const std::tuple<std::string, std::string>& pair : initialValues)
			{
				const std::string& key = std::get<0>(pair);
				const std::string& value = std::get<1>(pair);
				auto assoc = new Association<mychar_t*>();
				assoc->Initialize(key.c_str(), strdup(value.c_str()));
				dConfig.Add(assoc);
				//dConfig.SetValue(key, value);
			}
			Assert::AreEqual(29, (int)dConfig.Size(), L"Expected to set values");


			dict.SetValue("s", &s);
			dict.SetValue("t", &t);
			dict.SetValue("pi", &pi);
			dict.SetValue("BASS", &BASS);

			InitGlobals();

			Assert::AreEqual(SUCCESS, pf.Initialize("ABCD", "XYZ", &dConfig, &dict, &m_dGlobals), L"Initialization failed");

			Assert::AreEqual(4ul, pf.NumPhases(), L"Failed to initialize phases");
			Assert::AreEqual(2ul, pf.NumDimensions(), L"Failed to initialize dimensions");
			Assert::AreEqual(1ul, pf.NumFunctions(), L"Failed to initialize functions");

			Assert::AreEqual(24, (int)dict.Size(), L"Failed to add all expected functions, and/or eliminate unused omitted values");
		}

	};
}
