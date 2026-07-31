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
#include "..\TextUtils.hpp"
#include "..\TextUtils.cpp"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(TextUtilsUT)
	{
	public:
		TEST_METHOD(TestEnglish)
		{
			const char original[] = "This is a test in ASCII English!";
			const std::wstring expected = { L"This is a test in ASCII English!" };
			std::wstring result;

			// Original code
			const std::wstring cvtResult = CODECVT_utf8_to_wstring(original);
			Assert::AreEqual(expected, cvtResult, L"Failed to match CODECVT");

			// Suggested replacement (WIN32 specific)
			const std::wstring winResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(expected, winResult, L"Failed to match MultiByteToWideChar");

			// Try1 -- First Reimplementation exploration
			result = ATTEMPT1_utf8_to_wstring(original);
			Assert::AreEqual(expected, result, L"Failed to convert to 2 `UnicodeReplacementChar`");

			// Try2 -- Classification check
			Assert::IsTrue(Utf8Certainty::MaybeUtf8 == classifyUtf8((uint8_t*)original, _countof(original) - 1), L"Maybe valid UTF8");
			Assert::IsTrue(isValidUtf8((uint8_t*)original, _countof(original) - 1), L"Invalid UTF8");

			// Try3 -- Second Reimplementation exploration
			result = utf8ToUtf16(original);
			Assert::AreEqual(expected, result, L"Try 3 Failed to convert to 2 `UnicodeReplacementChar`");
		}

		TEST_METHOD(TestShiftJIS)
		{
			// https://uvlio.com/en/guides/utf8-vs-shift-jis-browser-encoding-checks
			const char original[] = "\x82\xA0";
			//const std::wstring expected = {L"\xE3\x81\x82"};
			const std::wstring expected = { L"\xFFFD\xFFFD" };
			std::wstring result;

			bool codecvtThrew = false;
			try {
				const std::wstring cvtResult = CODECVT_utf8_to_wstring(original);
			}
			catch (...)
			{
				codecvtThrew = true;
			}
			Assert::IsTrue(codecvtThrew, L"CODECVT somehow did not throw");

			const std::wstring winResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(std::wstring{ L"" }, winResult, L"Failed to match MultiByteToWideChar");

			result = ATTEMPT1_utf8_to_wstring(original);
			Assert::AreEqual(expected, result, L"Failed to convert to 2 `UnicodeReplacementChar`");

			Assert::IsTrue(Utf8Certainty::NotUtf8 == classifyUtf8((uint8_t*)original, _countof(original) - 1), L"Maybe valid UTF8");
			Assert::IsFalse(isValidUtf8((uint8_t*)original, _countof(original) - 1), L"Invalid UTF8");

			bool utf8ToUtf16Threw = false;
			try {
				result = utf8ToUtf16(original);
			}
			catch (...)
			{
				utf8ToUtf16Threw = true;
			}
			Assert::IsTrue(utf8ToUtf16Threw, L"Try 3 Successfully converted?");
		}
	};
}
