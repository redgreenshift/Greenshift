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
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // (Delete if we ever remove codecvt_utf8_utf16)
#include <codecvt> // for codecvt_utf8_utf16 (Delete if we ever remove codecvt_utf8_utf16)
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <locale>
#include <string>
#include <vector>

#include "..\Project Greenshift.h"
#include "CppUnitTest.h"
#include "..\TextUtils.hpp"
#include "..\TextUtils.cpp"

// DEPRECATED
// 'std::codecvt_utf8_utf16<wchar_t,1114111,(std::codecvt_mode)0>': warning STL4017 :
// std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17.
// (The std::codecvt class template is NOT deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead.
// You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to suppress this warning.
//
// This used to be the easy way to convert, then it was deprecated,
// and the suggestion was to use MultiByteToWideChar, but that is Windows specific.
// I want to at least *try* to keep Greenshift agnostic about the platform, as much as reasonably possible.
// We will just TEST that my replacement matches the behavior of codecvt_utf8_utf16 and MultiByteToWideChar.
static std::wstring CODECVT_utf8_to_wstring(const std::string& s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(s);
}

#ifdef WIN32
#include <string_view>
#include <Windows.h> // The C++ Standard doesn't provide equivalent non-deprecated functionality for std::wstring_convert<std::codecvt_utf8_utf16; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead.
// This is the most straightforward replacement... but it's platform specific, which is less than ideal.
static std::wstring WINDOWS_utf8_to_utf16(const std::string_view utf8)
{
	if (utf8.empty()) return {};

	const int needed = MultiByteToWideChar(
		CP_UTF8, MB_ERR_INVALID_CHARS,
		utf8.data(), (int)utf8.size(),
		nullptr, 0
	);

	if (needed <= 0) return {}; // or throw / handle error

	std::wstring out(needed, L'\0');
	MultiByteToWideChar(
		CP_UTF8, MB_ERR_INVALID_CHARS,
		utf8.data(), (int)utf8.size(),
		out.data(), needed
	);
	return out;
}
#endif // WIN32


 // Helpers
static std::wstring make_wstring_from_utf16_code_units(const std::vector<char16_t>& units)
{
	std::wstring out;
	out.resize(units.size());
	for (size_t i = 0; i < units.size(); ++i)
	{
		out[i] = static_cast<wchar_t>(units[i]);
	}
	return out;
}

static std::string utf8_bytes(std::initializer_list<unsigned char> bytes)
{
	return std::string(reinterpret_cast<const char*>(bytes.begin()), bytes.size());
}

// If your implementation throws on invalid input,
// or returns false/empty,
// or silently replaces invalid sequences with 0xFFFD,
// set this appropriately.
static constexpr Utf8ErrorPolicy ERROR_POLICY = Utf8ErrorPolicy::Return;


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GreenshiftUnitTest
{
	TEST_CLASS(TextUtilsUT)
	{
	public:
		TEST_METHOD(TestEnglish)
		{
			const char original[] = "This is a test in ASCII English!";
			const std::string original2 = original;
			const std::wstring expected{ original2.begin(), original2.end()};
			std::wstring result;

			// Original code
			const std::wstring cvtResult = CODECVT_utf8_to_wstring(original);
			Assert::AreEqual(expected, cvtResult, L"Failed to match CODECVT");

			// Suggested replacement (WIN32 specific)
			const std::wstring winResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(expected, winResult, L"Failed to match MultiByteToWideChar");

			// Reimplementation exploration
			result = utf8_to_wstring(original, Utf8ErrorPolicy::Replace /*ERROR_POLICY*/);
			Assert::AreEqual(expected, result, L"Replaced chars it shouldn't have");

			// Classification check
			Assert::IsTrue(Utf8Certainty::MaybeUtf8 == is_valid_utf8((uint8_t*)original, _countof(original) - 1), L"Failed to detect ASCII is 'maybe' valid UTF8");
		}

		TEST_METHOD(TestShiftJIS)
		{
			// https://uvlio.com/en/guides/utf8-vs-shift-jis-browser-encoding-checks
			const char original[] = "\x82\xA0";
			//const std::wstring expected = {L"\xE3\x81\x82"};
			const std::wstring expected = { L"\xFFFD\xFFFD" }; // InvalidSequenceReplacementChar
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

			bool utf8_to_wstring_Threw = false;
			try {
				result = utf8_to_wstring(original, ERROR_POLICY);
			}
			catch (...)
			{
				utf8_to_wstring_Threw = true;
			}
			if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Throw)
				Assert::IsTrue(utf8_to_wstring_Threw, L"Failed to detect invalid sequence and reject conversion via throw");
			else if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Replace)
				Assert::AreEqual(expected, result, L"Failed to convert to 2 `UnicodeReplacementChar`");
			else
				Assert::AreEqual(std::wstring{/*empty*/}, result, L"Failed to detect invalid sequence and reject conversion");

			Assert::IsTrue(Utf8Certainty::NotUtf8 == is_valid_utf8((uint8_t*)original, _countof(original) - 1), L"Failed to detect Invalid UTF8");
		}

		// What is BMP?
		// BMP = Basic Multilingual Plane
		// Unicode code points U+0000 through U+FFFF
		// (excluding surrogate range U+D800-U+DFFF).
		TEST_METHOD(UTF8_EQUALS_REFERENCES_BMPCharacters)
		{
			// é ü ñ £ € Ω 中 日 本
			// This test exercises multiple varied features without introducing surrogate pairs:
			//  - Latin-1 Supplement(é, ü, ñ, £)
			//  - BMP 2-byte UTF-8 sequences
			//  - BMP 3-byte UTF-8 sequences(€, 中, 日, 本)
			//  - Multiple Unicode blocks(Latin, Greek, Currency Symbols, CJK)
			const std::string original = utf8_bytes({
				0xC3, 0xA9,             // é U+00E9
				0xC3, 0xBC,             // ü U+00FC
				0xC3, 0xB1,             // ñ U+00F1
				0xC2, 0xA3,             // £ U+00A3
				0xE2, 0x82, 0xAC,       // € U+20AC
				0xCE, 0xA9,             // Ω U+03A9
				0xE4, 0xB8, 0xAD,       // 中 U+4E2D
				0xE6, 0x97, 0xA5,       // 日 U+65E5
				0xE6, 0x9C, 0xAC        // 本 U+672C
				});
			const std::vector<char16_t> expectedUtf16 = {
				0x00E9, // é U+00E9
				0x00FC, // ü U+00FC
				0x00F1, // ñ U+00F1
				0x00A3, // £ U+00A3
				0x20AC, // € U+20AC
				0x03A9, // Ω U+03A9
				0x4E2D, // 中 U+4E2D
				0x65E5, // 日 U+65E5
				0x672C, // 本 U+672C
			};
			const std::wstring expected =
				make_wstring_from_utf16_code_units(expectedUtf16);

			const std::wstring result = utf8_to_wstring(original);
			Assert::AreEqual(expected, result);

			// Validate matches REFERENCE implementations
			const std::wstring windowsResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(expected, windowsResult);
			const std::wstring codecvtResult = CODECVT_utf8_to_wstring(original);
			Assert::AreEqual(expected, codecvtResult);
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_Emoji)
		{
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			const std::string original = utf8_bytes({
				0xF0, 0x9F, 0x98, 0x80, // 😀 U+1F600
				0xF0, 0x9F, 0x9A, 0x80, // 🚀 U+1F680
				0xF0, 0x9F, 0x8E, 0xB5, // 🎵 U+1F3B5
				0xF0, 0x9F, 0x8C, 0x8E  // 🌎
				}); //u8"😀🚀🎵🌎";
			const std::vector<char16_t> expectedUtf16 = {
				0xD83D, 0xDE00, // 😀 U+1F600
				0xD83D, 0xDE80, // 🚀 U+1F680
				0xd83c, 0xdfb5, // 🎵 U+1F3B5
				0xd83c, 0xdf0e, // 🌎 U+1F30E
			};
			const std::wstring expected =
				make_wstring_from_utf16_code_units(expectedUtf16);

			const std::wstring result = utf8_to_wstring(original);
			Assert::AreEqual(expected, result);

			// Validate matches REFERENCE implementations
			const std::wstring windowsResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(expected, windowsResult);
			const std::wstring codecvtResult = CODECVT_utf8_to_wstring(original);
			Assert::AreEqual(expected, codecvtResult);
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_MixedLanguage)
		{
			// Hello 世界 😀 Καλημέρα
			const std::string original = utf8_bytes({
				// Hello
				'H', 'e', 'l', 'l', 'o', ' ',

				// 世 U+4E16
				0xE4, 0xB8, 0x96,

				// 界 U+754C
				0xE7, 0x95, 0x8C,

				' ',

				// 😀 U+1F600
				0xF0, 0x9F, 0x98, 0x80,

				' ',

				// Καλημέρα
				0xCE, 0x9A, // Κ
				0xCE, 0xB1, // α
				0xCE, 0xBB, // λ
				0xCE, 0xB7, // η
				0xCE, 0xBC, // μ
				0xCE, 0xAD, // έ
				0xCF, 0x81, // ρ
				0xCE, 0xB1  // α
				});
			const std::vector<char16_t> expectedUtf16 =
			{
				// "Hello "
				0x0048, // H
				0x0065, // e
				0x006C, // l
				0x006C, // l
				0x006F, // o
				0x0020, // space

				// 世 U+4E16
				0x4E16,

				// 界 U+754C
				0x754C,

				0x0020,

				// 😀 U+1F600
				0xD83D,
				0xDE00,

				0x0020,

				// Καλημέρα
				0x039A, // Κ
				0x03B1, // α
				0x03BB, // λ
				0x03B7, // η
				0x03BC, // μ
				0x03AD, // έ
				0x03C1, // ρ
				0x03B1, // α
			};
			const std::wstring expected =
				make_wstring_from_utf16_code_units(expectedUtf16);

			const std::wstring result = utf8_to_wstring(original);
			Assert::AreEqual(expected, result);

			// Validate matches REFERENCE implementations
			const std::wstring windowsResult = WINDOWS_utf8_to_utf16(original);
			Assert::AreEqual(expected, windowsResult);
			const std::wstring codecvtResult = CODECVT_utf8_to_wstring(original);
			Assert::AreEqual(expected, codecvtResult);
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_EmbeddedNul)
		{
			// Verify all implementations preserve embedded NUL code units.
			//
			// This helps detect implementations that accidentally treat UTF-8 input
			// as a null-terminated string rather than a byte sequence.

			// "A\0B"
			const std::string original({ 'A', '\0', 'B' });
			const std::wstring expected({ 'A', '\0', 'B' });

			Assert::AreEqual(
				expected,
				WINDOWS_utf8_to_utf16(original));

			Assert::AreEqual(
				expected,
				CODECVT_utf8_to_wstring(original));

			Assert::AreEqual(
				expected,
				utf8_to_wstring(original));
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_BoundaryCodePoints)
		{
			// Important UTF-8 / Unicode transition boundaries:
			//
			// U+007F   Largest ASCII code point
			// U+0080   First 2-byte UTF-8 sequence
			// U+07FF   Largest 2-byte UTF-8 sequence
			// U+0800   First 3-byte UTF-8 sequence
			// U+FFFF   Largest BMP code point
			// U+10000  First supplementary code point
			// U+10FFFF Largest valid Unicode code point
			//
			// This test exercises every major UTF-8 encoding boundary.

			const std::string original = utf8_bytes({

				// U+007F
				0x7F,

				// U+0080
				0xC2, 0x80,

				// U+07FF
				0xDF, 0xBF,

				// U+0800
				0xE0, 0xA0, 0x80,

				// U+FFFF
				0xEF, 0xBF, 0xBF,

				// U+10000
				0xF0, 0x90, 0x80, 0x80,

				// U+10FFFF
				0xF4, 0x8F, 0xBF, 0xBF
				});

			const std::wstring expected =
				WINDOWS_utf8_to_utf16(original);

			Assert::AreEqual(
				expected,
				CODECVT_utf8_to_wstring(original));

			Assert::AreEqual(
				expected,
				utf8_to_wstring(original));
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_RealWorldFilename)
		{
			// Realistic filename containing multiple scripts and an emoji.
			//
			// Exercises:
			//  - ASCII
			//  - Latin-1 accented characters
			//  - CJK characters
			//  - Supplementary-plane emoji
			//  - Ordinary punctuation

			const std::string original = utf8_bytes({

				// "Caf"
				'C', 'a', 'f',

				// é
				0xC3, 0xA9,

				'_',

				// 日
				0xE6, 0x97, 0xA5,

				// 本
				0xE6, 0x9C, 0xAC,

				// 語
				0xE8, 0xAA, 0x9E,

				'_',

				// 😀
				0xF0, 0x9F, 0x98, 0x80,

				'.', 't', 'x', 't'
				});

			const std::wstring expected =
				WINDOWS_utf8_to_utf16(original);

			Assert::AreEqual(
				expected,
				CODECVT_utf8_to_wstring(original));

			Assert::AreEqual(
				expected,
				utf8_to_wstring(original));
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_VeryLongMixedString)
		{
			// Long mixed-language string.
			//
			// Intended to exercise:
			//  - repeated buffer growth
			//  - repeated transitions between UTF-8 sequence lengths
			//  - repeated surrogate-pair generation
			//  - large input sizes
			//
			// A surprising number of bugs only appear after growing the destination
			// buffer and reallocating *many* times. This test attempts to catch issues
			// that may not be visible with smaller strings, including incorrect length
			// calculations, stale pointers, invalidated iterators, and other
			// buffer-management or state-management mistakes.

			std::string original;

			const std::string fragment = utf8_bytes({

				// "Hello "
				'H', 'e', 'l', 'l', 'o', ' ',

				// 世界
				0xE4, 0xB8, 0x96,
				0xE7, 0x95, 0x8C,

				' ',

				// 😀
				0xF0, 0x9F, 0x98, 0x80,

				' '
				});

			// 1000 repetitions is intentionally large enough to force repeated
			// buffer growth and reallocations while keeping unit test runtime low.
			//
			// NOTE(2026-10-13):
			// MSVC STL growth pattern was observed where string capacity grew by
			// approximately 1.5x after exceeding the small-string optimization buffer,
			// triggering roughly sixteen reallocations.
			//
			//     15 -> 31
			//     31 -> 47
			//     47 -> 70
			//     70 -> 105
			//     105 -> 157
			//     157 -> 235
			//     235 -> 352
			//     352 -> 528
			//     528 -> 792
			//     792 -> 1188
			//     1188 -> 1782
			//     1782 -> 2673
			//     2673 -> 4009
			//     4009 -> 6013
			//     6013 -> 9019
			//     9019 -> 13528
			//     13528 -> 20292
			//
			// This test intentionally forces multiple buffer growth operations and
			// reallocations to help detect bugs that only appear during repeated
			// container expansion.
			for (int i = 0; i < 1000; ++i)
			{
				original += fragment;
			}

			//const std::vector<char16_t> expectedUtf16 =
			//{
			//	0x0041,       // A
			//	0x0000,       // NUL
			//	0x0042,       // B
			//	0x00E9,       // é
			//	0xD83D,       // 😀 high surrogate
			//	0xDE00,       // 😀 low surrogate
			//	0x0043        // C
			//};
			const std::wstring expected =
				WINDOWS_utf8_to_utf16(original);

			Assert::AreEqual(
				expected,
				CODECVT_utf8_to_wstring(original));

			Assert::AreEqual(
				expected,
				utf8_to_wstring(original));
		}

		TEST_METHOD(UTF8_EQUALS_REFERENCES_EmbeddedNulAndEmoji)
		{
			// Exercise several tricky cases simultaneously:
			//
			//  - ASCII
			//  - Embedded NUL
			//  - BMP character
			//  - Supplementary character (surrogate pair)
			//  - Additional text after all of the above
			//
			// This helps detect implementations that incorrectly treat UTF-8 as a
			// null-terminated string or fail to resume decoding correctly after
			// complex sequences.

			const std::string original = utf8_bytes({

				// A
				'A',

				// Embedded NUL
				0x00,

				// B
				'B',

				// é U+00E9
				0xC3, 0xA9,

				// 😀 U+1F600
				0xF0, 0x9F, 0x98, 0x80,

				// C
				'C'
				});

			const std::wstring expected =
				WINDOWS_utf8_to_utf16(original);

			Assert::AreEqual(
				expected,
				CODECVT_utf8_to_wstring(original));

			Assert::AreEqual(
				expected,
				utf8_to_wstring(original));
		}


		// --------------------
		// Core correctness
		// --------------------

		TEST_METHOD(EmptyString_ReturnsEmptyWString)
		{
			const std::string original;
			const std::wstring result = utf8_to_wstring(original);
			Assert::IsTrue(result.empty());
		}

		TEST_METHOD(AsciiOnly_RoundTripCharactersMatch)
		{
			const std::string original = "Hello, world! 12345";
			const std::wstring expected{ original.begin(), original.end() };
			const std::wstring result = utf8_to_wstring(original);

			// ASCII characters map 1:1 to UTF-16 code units.
			Assert::AreEqual(expected, result, L"Failed to match exactly with no modification");
		}

		TEST_METHOD(EmbeddedNul_IsPreservedAsCodeUnitZero_STRING)
		{
			// "A\0B"
			std::string s;
			s.push_back('A');
			s.push_back('\0');
			s.push_back('B');

			const std::wstring result = utf8_to_wstring(s);

			Assert::AreEqual(3u, result.size());
			Assert::AreEqual(L'A', result[0]);
			Assert::AreEqual(L'\0', result[1]);
			Assert::AreEqual(L'B', result[2]);
		}

		TEST_METHOD(EmbeddedNul_IsPreservedAsCodeUnitZero_CHAR)
		{
			// "A\0B"
			const char s[] = {'A', '\0', 'B', '\0'};

			const std::wstring result = utf8_to_wstring(s, _countof(s) - 1);

			Assert::AreEqual(3u, result.size());
			Assert::AreEqual(L'A', result[0]);
			Assert::AreEqual(L'\0', result[1]);
			Assert::AreEqual(L'B', result[2]);
		}

		TEST_METHOD(Utf8Bom_ReturnsBomInUtf16)
		{
			// UTF-8 BOM: EF BB BF -> U+FEFF
			const std::string original = std::string("\xEF\xBB\xBF", 3);
			const std::wstring result = utf8_to_wstring(original);

			Assert::AreEqual(1u, result.size());
			Assert::AreEqual(static_cast<wchar_t>(0xFEFF), result[0]);
		}

		// What is BMP? Basic Multilingual Plane.
		// BMP characters fit in a single UTF-16 code unit.
		// Supplementary characters require surrogate pairs.
		TEST_METHOD(BMPCharacters_CommonExamples)
		{
			// "café € 𝄞" but split into BMP + supplementary:
			// cafe: U+0063 U+0061 U+0066 U+00E9
			// Euro: U+20AC
			// We'll test café € separately from supplementary music symbol.
			const std::string original = utf8_bytes({
								0x63, 0x61, 0x66, 0xC3, 0xA9,  // café
								0x20,
								0xE2, 0x82, 0xAC                  // €
				});
			const std::wstring result = utf8_to_wstring(original);

			// U+00E9 and U+20AC must be present as single UTF-16 code units.
			// Expected wide string (UTF-16 on Windows):
			Assert::AreEqual(L"café €", result.c_str());
		}

		TEST_METHOD(Emoji_SurrogatePair_IsProducedCorrectly)
		{
			// U+1F600 GRINNING FACE: surrogate pair D83D DE00 in UTF-16.
			const std::string original = utf8_bytes({
								0xF0, 0x9F, 0x98, 0x80
							}); // u8"😀";
			const std::wstring result = utf8_to_wstring(original);

			// Validate by UTF-16 code units explicitly (works regardless of platform wchar_t size).
			const std::vector<char16_t> expected = { 0xD83D, 0xDE00 };
			Assert::AreEqual(make_wstring_from_utf16_code_units(expected), result);
		}

		TEST_METHOD(MultipleSupplementaries_ConcatenatedPairs)
		{
			// 😀 (1F600) and 🚀 (1F680)
			const std::string original = utf8_bytes({
								0xF0, 0x9F, 0x98, 0x80, // 😀
								0xF0, 0x9F, 0x9A, 0x80  // 🚀
							}); //u8"😀🚀";
			const std::wstring result = utf8_to_wstring(original);

			// U+1F600 => D83D DE00
			// U+1F680 => D83D DE80
			const std::vector<char16_t> expected = { 0xD83D, 0xDE00, 0xD83D, 0xDE80 };
			Assert::AreEqual(make_wstring_from_utf16_code_units(expected), result);
		}

		TEST_METHOD(MixedAsciiBmpAndSupplementary)
		{
			// "A € 😀 Z"
			const std::string original = utf8_bytes({
								0x41, 0x20,
								0xE2, 0x82, 0xAC,
								0x20,
								0xF0, 0x9F, 0x98, 0x80,
								0x20,
								0x5A
							}); //u8"A € 😀 Z";
			const std::wstring result = utf8_to_wstring(original);

			// Expected UTF-16 code units:
			// 'A' (0041), ' ' (0020), '€' (20AC), ' ' (0020),
			// 😀 (D83D DE00), ' ' (0020), 'Z' (005A)
			const std::vector<char16_t> expected = {
				0x0041, 0x0020, 0x20AC, 0x0020,
				0xD83D, 0xDE00,
				0x0020, 0x005A
			};
			Assert::AreEqual(make_wstring_from_utf16_code_units(expected), result);
		}

		TEST_METHOD(Boundary_MaxValidCodePoint_Supplementary)
		{
			// U+10FFFF is the max valid Unicode scalar value.
			// UTF-8 for U+10FFFF: F4 8F BF BF
			const std::string original = std::string("\xF4\x8F\xBF\xBF", 4);
			const std::wstring result = utf8_to_wstring(original);

			// U+10FFFF UTF-16 surrogates: DBFF DFFF
			const std::vector<char16_t> expected = { 0xDBFF, 0xDFFF };
			Assert::AreEqual(make_wstring_from_utf16_code_units(expected), result);
		}

		TEST_METHOD(Boundary_MinValidCodePoint)
		{
			// U+0000: UTF-8 = 00
			const std::string original = std::string("\x00", 1);
			const std::wstring result = utf8_to_wstring(original);
			Assert::AreEqual(1u, result.size());
			Assert::AreEqual(static_cast<wchar_t>(0x0000), result[0]);
		}

		// --------------------
		// Invalid UTF-8 cases
		// --------------------

		static void ExpectInvalidInput(const std::string& original)
		{
			if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Throw)
			{
				bool threw = false;
				std::wstring result;
				try
				{
					result = utf8_to_wstring(original);
				}
				catch (...)
				{
					threw = true;
				}
				Assert::IsTrue(threw, L"Failed to throw");
				Assert::IsTrue(result.empty(), L"Failed to keep empty");
			}
			else
			{
				// Common policy: replace invalid sequences with U+FFFD.
				// This may differ depending on your implementation.
				const std::wstring result = utf8_to_wstring(original);

				if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Return)
				{
					Assert::IsTrue(result.empty());
				}
				else if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Replace)
				{
					Assert::IsFalse(result.empty());

					// Often at least one U+FFFD appears.
					bool hasReplacement = false;
					for (wchar_t ch : result)
					{
						if (static_cast<uint16_t>(ch) == InvalidSequenceReplacementChar)
						{
							hasReplacement = true;
							break;
						}
					}
					Assert::IsTrue(hasReplacement);
				}
			}
		}

		TEST_METHOD(Invalid_LeadingByteOnly_ThrowsOrReplaces)
		{
			// 0xC2 expects one continuation byte (10xxxxxx), but none provided.
			const std::string original = std::string("\xC2", 1);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_ContinuationByteAtTopLevel_ThrowsOrReplaces)
		{
			// Continuation byte 0x80 without a leading byte is invalid.
			const std::string original = std::string("\x80", 1);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_TruncatedThreeByteSequence_ThrowsOrReplaces)
		{
			// 0xE2 0x82 expects a third continuation byte but missing.
			const std::string original = std::string("\xE2\x82", 2);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_TruncatedFourByteSequence_ThrowsOrReplaces)
		{
			// 0xF0 0x9F 0x98 expects one more continuation byte but missing.
			const std::string original = std::string("\xF0\x9F\x98", 3);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_BogusContinuationPattern_ThrowsOrReplaces)
		{
			// 0xC2 followed by 0x41 ('A') which is not a continuation byte (10xxxxxx).
			const std::string original = std::string("\xC2\x41", 2);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_OverlongEncoding_TwoByteSequenceForAscii_ThrowsOrReplaces)
		{
			// Overlong encoding of '/' (U+002F) is invalid:
			// U+002F should be encoded as 2F, but overlong would be C0 AF.
			const std::string original = std::string("\xC0\xAF", 2);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_OverlongEncoding_ThreeByteSequenceForBMP_ThrowsOrReplaces)
		{
			// Overlong for 'A' (U+0041) using 3-byte form is invalid.
			// Valid: 41
			// Overlong example: E0 81 81 (encodes U+0041 overlong)
			const std::string original = std::string("\xE0\x81\x81", 3);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_CodePointTooHigh_ThrowsOrReplaces)
		{
			// UTF-8 for a value above U+10FFFF is invalid.
			// Example byte sequence: F4 90 80 80 corresponds to > U+10FFFF.
			const std::string original = std::string("\xF4\x90\x80\x80", 4);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(Invalid_SurrogateCodePointsInUTF8_ThrowsOrReplaces)
		{
			// UTF-8 should not encode surrogate halves (D800-DFFF).
			// Example: ED A0 80 encodes U+D800 (high surrogate), invalid.
			const std::string original = std::string("\xED\xA0\x80", 3);
			ExpectInvalidInput(original);
		}

		TEST_METHOD(MixedValidAndInvalid_ProducesValidPrefixAndErrorHandling)
		{
			// "A" + invalid byte + "B"
			std::string original;
			original.push_back('A');         // 0x41
			original.push_back(static_cast<char>(0xC0));  // invalid leading byte (overlong prefix)
			original.push_back('B');         // 0x42

			if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Throw)
			{
				ExpectInvalidInput(original);
				return;
			}

			std::wstring result = utf8_to_wstring(original);

			if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Return)
			{
				Assert::IsTrue(result.empty(), L"expected empty (currently no value to indicate failure)");
			}
			else if constexpr (ERROR_POLICY == Utf8ErrorPolicy::Replace)
			{
				// At minimum, we expect the 'A' and 'B' to appear somewhere in order,
				// or at least that the output length isn't nonsense.
				// Exact replacement strategy varies, so we validate a weaker property:
				Assert::IsTrue(result.size() >= 2u, L"Length should be at least 2");
				Assert::AreEqual(L'A', result.front(), L"Expected 'A'");
				Assert::AreEqual(L'B', result.back(), L"Expected 'B'");
			}
		}
	};
}
