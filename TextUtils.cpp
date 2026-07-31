#include "Project Greenshift.h"
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

// TODO: Investigate various approaches to detect/convert UTF8
// Want an IsUTF8 function with yes/no/maybe
// Unit Tests for ShiftJIS, include encodings that do and DO NOT conform to UTF8.
// https://en.wikipedia.org/wiki/UTF-8
// https://en.wikipedia.org/wiki/Shift-JIS

#include <string> // append_codepoint_to_wstring
#include <cstdint> // isValidUtf8, append_codepoint_to_wstring
#include <limits> // append_codepoint_to_wstring
#include <cstddef> // isValidUtf8
#include <string_view>
#include <stdexcept>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // Delete with codecvt_utf8_utf16
#include "TextUtils.hpp"

#ifdef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // Delete with codecvt_utf8_utf16
#include <codecvt> // Delete with codecvt_utf8_utf16

// This used to be the easy way to convert, then it was deprecated,
// and the suggestion was to use MultiByteToWideChar, but that is Windows specific.
// I want to at least try to keep as much of Greenshift agnostic about the platform as possible.
std::wstring CODECVT_utf8_to_wstring(const std::string & s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(s);
}
#endif // _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


// As I explore implementations, I want to be able to swap them out,
// so this is the point to swap out implementations
std::wstring utf8_to_wstring(const std::string& s)
{
	return WINDOWS_utf8_to_utf16(s);
}

// 'std::codecvt_utf8_utf16<wchar_t,1114111,(std::codecvt_mode)0>': warning STL4017 :
// std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17.
// (The std::codecvt class template is NOT deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead.
// You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to suppress this warning.
#ifdef WIN32
#include <windows.h>

// This is the most straightforward replacement, as I don't have to _implement_ the feature myself... but it's platform specific, which is less than ideal.
std::wstring WINDOWS_utf8_to_utf16(const std::string & utf8)
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

constexpr wchar_t InvalidSequenceReplacementChar = static_cast<wchar_t>(0xFFFD); // standard empty box character signifying an invalid byte sequence

// TODO: I don't like this silently converts invalid sequences, consider adding a flag or option to fail/throw on invalid input.
static void append_codepoint_to_wstring(uint32_t cp, std::wstring & out) {
	// Reject invalid scalar values
	if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
		cp = InvalidSequenceReplacementChar; // replacement char
	}

	if (sizeof(wchar_t) == 2) {
		// Encode as UTF-16
		if (cp <= 0xFFFF) {
			out.push_back(static_cast<wchar_t>(cp));
		}
		else {
			cp -= 0x10000;
			wchar_t high = static_cast<wchar_t>(0xD800 + (cp >> 10));
			wchar_t low = static_cast<wchar_t>(0xDC00 + (cp & 0x3FF));
			out.push_back(high);
			out.push_back(low);
		}
	}
	else {
		// wchar_t is typically 4 bytes => UTF-32
		out.push_back(static_cast<wchar_t>(cp));
	}
}

std::wstring ATTEMPT1_utf8_to_wstring(const std::string & s) {
	std::wstring out;
	out.reserve(s.size()); // rough heuristic

	const unsigned char* bytes =
		reinterpret_cast<const unsigned char*>(s.data());

	size_t i = 0;
	while (i < s.size()) {
		unsigned char b0 = bytes[i];

		uint32_t cp = 0;
		size_t needed = 0;

		if (b0 <= 0x7F) {             // 0xxxxxxx
			cp = b0;
			needed = 1;
		}
		else if ((b0 & 0xE0) == 0xC0) { // 110xxxxx
			cp = b0 & 0x1F;
			needed = 2;
		}
		else if ((b0 & 0xF0) == 0xE0) { // 1110xxxx
			cp = b0 & 0x0F;
			needed = 3;
		}
		else if ((b0 & 0xF8) == 0xF0) { // 11110xxx
			cp = b0 & 0x07;
			needed = 4;
		}
		else {
			// Invalid leading byte
			append_codepoint_to_wstring(InvalidSequenceReplacementChar, out);
			++i;
			continue;
		}

		if (i + needed > s.size()) {
			append_codepoint_to_wstring(InvalidSequenceReplacementChar, out);
			break;
		}

		bool ok = true;
		for (size_t k = 1; k < needed; ++k) {
			unsigned char bx = bytes[i + k];
			if ((bx & 0xC0) != 0x80) { // not 10xxxxxx
				ok = false;
				break;
			}
			cp = (cp << 6) | (bx & 0x3F);
		}

		// Additional validity checks for overlongs and range:
		if (ok) {
			// Overlong checks:
			if ((needed == 2 && cp < 0x80) ||
				(needed == 3 && cp < 0x800) ||
				(needed == 4 && cp < 0x10000)) {
				ok = false;
			}
			// Max code point
			if (cp > 0x10FFFF) ok = false;
			// Surrogate range
			if (cp >= 0xD800 && cp <= 0xDFFF) ok = false;
		}

		if (!ok) cp = InvalidSequenceReplacementChar;
		append_codepoint_to_wstring(cp, out);
		i += needed;
	}

	return out;
}

// Validates that the entire byte sequence is well-formed UTF-8.
// Rejects: invalid leading bytes, wrong continuation bytes,
// overlong encodings, surrogate code points, and values > U+10FFFF.
bool isValidUtf8(const uint8_t * data, size_t len)
{
	size_t i = 0;

	while (i < len) {
		uint8_t b0 = data[i];

		// 1-byte ASCII
		if (b0 <= 0x7F) {
			i++;
			continue;
		}

		// Determine expected length and seed code point bits
		uint32_t codepoint = 0;
		size_t need = 0;

		if (b0 >= 0xC2 && b0 <= 0xDF) {          // 110xxxxx
			need = 1;                           // total 2 bytes
			codepoint = b0 & 0x1F;
		}
		else if (b0 >= 0xE0 && b0 <= 0xEF) {   // 1110xxxx
			need = 2;                           // total 3 bytes
			codepoint = b0 & 0x0F;
		}
		else if (b0 >= 0xF0 && b0 <= 0xF4) {   // 11110xxx (UTF-8 max is F4 8F BF BF)
			need = 3;                           // total 4 bytes
			codepoint = b0 & 0x07;
		}
		else {
			return false; // includes 0xC0/0xC1 (overlong starters) and 0xF5-0xFF
		}

		if (i + need >= len) return false; // not enough bytes

		// Read continuation bytes
		for (size_t j = 1; j <= need; j++) {
			uint8_t bj = data[i + j];
			if ((bj & 0xC0) != 0x80) return false; // must be 10xxxxxx
			codepoint = (codepoint << 6) | (bj & 0x3F);
		}

		// Extra validity checks for overlong / surrogates / range
		if (need == 1) {
			// 2-byte sequences must be >= 0x80 (already ensured by C2..DF), so ok
		}
		else if (need == 2) {
			// 3-byte sequences must be >= 0x800
			if (codepoint < 0x800) return false;
			// Reject surrogate halves U+D800..U+DFFF
			if (codepoint >= 0xD800 && codepoint <= 0xDFFF) return false;
		}
		else { // need == 3
			// 4-byte sequences must be >= 0x10000
			if (codepoint < 0x10000) return false;
		}

		if (codepoint > 0x10FFFF) return false;

		// All good; advance
		i += need + 1;
	}

	return true;
}

static bool isValidUtf8WellFormed(const uint8_t * data, size_t len)
{
	size_t i = 0;

	while (i < len) {
		uint8_t b0 = data[i];

		// ASCII
		if (b0 <= 0x7F) {
			++i;
			continue;
		}

		uint32_t codepoint = 0;
		size_t need = 0;

		// Leading byte determines length (and excludes overlong starters)
		if (b0 >= 0xC2 && b0 <= 0xDF) {          // 110xxxxx, total 2 bytes
			need = 1;
			codepoint = b0 & 0x1F;
		}
		else if (b0 >= 0xE0 && b0 <= 0xEF) { // 1110xxxx, total 3 bytes
			need = 2;
			codepoint = b0 & 0x0F;
		}
		else if (b0 >= 0xF0 && b0 <= 0xF4) { // 11110xxx, total 4 bytes (max F4..)
			need = 3;
			codepoint = b0 & 0x07;
		}
		else {
			return false; // includes 0xC0/0xC1 and 0xF5..0xFF
		}

		if (i + need >= len) return false; // not enough continuation bytes

		// Continuation bytes: must be 10xxxxxx
		for (size_t j = 1; j <= need; ++j) {
			uint8_t bj = data[i + j];
			if ((bj & 0xC0) != 0x80) return false;
			codepoint = (codepoint << 6) | (bj & 0x3F);
		}

		// Reject overlong encodings and invalid code points
		if (need == 1) {
			// 2-byte sequence: automatically not overlong due to C2..DF rule
		}
		else if (need == 2) {
			// 3-byte: must be >= 0x800 and not a surrogate
			if (codepoint < 0x800) return false;
			if (codepoint >= 0xD800 && codepoint <= 0xDFFF) return false;
		}
		else { // need == 3
			// 4-byte: must be >= 0x10000
			if (codepoint < 0x10000) return false;
		}

		if (codepoint > 0x10FFFF) return false;

		i += need + 1;
	}

	return true;
}

Utf8Certainty classifyUtf8(const uint8_t * data, size_t len)
{
	if (!data && len != 0) return Utf8Certainty::NotUtf8;

	// If it isn't well-formed UTF-8, it can't be UTF-8.
	if (!isValidUtf8WellFormed(data, len)) {
		return Utf8Certainty::NotUtf8;
	}

	// It is well-formed UTF-8. Now check whether it's only ASCII.
	for (size_t i = 0; i < len; ++i) {
		if (data[i] >= 0x80) {
			return Utf8Certainty::Utf8; // non-ASCII present => unambiguous in intent
		}
	}

	return Utf8Certainty::MaybeUtf8; // ASCII-only => ambiguous intent
}

// Convert UTF-8 bytes to std::wstring.
// - If wchar_t is 16-bit (typical Windows), result is UTF-16 code units (surrogate pairs included).
// - If wchar_t is 32-bit, result is UTF-32 code points (no surrogate pairs).
// TODO: I don't like that this approach throws, consider returning a value to indicate failure.
static inline void appendCodepointToWString(std::wstring& out, uint32_t cp) {
	// Surrogates are invalid scalar values and should never be produced by our decoder.
#if WCHAR_MAX <= 0xFFFF
	if (cp <= 0xFFFF) {
		out.push_back(static_cast<wchar_t>(cp));
	}
	else {
		// UTF-16 surrogate pair
		cp -= 0x10000;
		wchar_t hi = static_cast<wchar_t>(0xD800u + (cp >> 10));
		wchar_t lo = static_cast<wchar_t>(0xDC00u + (cp & 0x3FFu));
		out.push_back(hi);
		out.push_back(lo);
	}
#else
	// wchar_t wide enough: store the code point directly
	// If wchar_t is 32-bit, std::wstring can hold full code points directly.
	out.push_back(static_cast<wchar_t>(cp));
#endif
}

static inline uint32_t decodeOneUtf8(const char*& p, const char* end) {
	if (p >= end) throw std::runtime_error("Invalid UTF-8: unexpected end");

	auto u = static_cast<unsigned char>(*p);
	uint32_t cp = 0;

	if (u <= 0x7F) { // 1-byte ASCII
		cp = u;
		++p;
		return cp;
	}

	auto need = uint32_t{ 0 };

	if (u >= 0xC2 && u <= 0xDF) {         // 2-byte (110xxxxx), excludes overlong starters C0/C1
		need = 1;
		cp = u & 0x1F;
	}
	else if (u >= 0xE0 && u <= 0xEF) { // 3-byte (1110xxxx)
		need = 2;
		cp = u & 0x0F;
	}
	else if (u >= 0xF0 && u <= 0xF4) { // 4-byte (11110xxx) max valid is F4 8F BF BF  ///??? (max U+10FFFF => F4 8F BF BF)
		need = 3;
		cp = u & 0x07;
	}
	else {
		throw std::runtime_error("Invalid UTF-8: invalid leading byte");
	}

	if (static_cast<size_t>(end - p) < 1 + need) {
		throw std::runtime_error("Invalid UTF-8: truncated sequence");
	}

	++p; // consume leading byte
	for (uint32_t i = 0; i < need; ++i) {
		auto ub = static_cast<unsigned char>(*p);
		if ((ub & 0xC0u) != 0x80u) throw std::runtime_error("Invalid UTF-8: invalid continuation byte");
		cp = (cp << 6) | (ub & 0x3Fu);
		++p;
	}

	// Reject surrogates and overlongs / out-of-range.
	// (These checks complement the leading-byte exclusions.)
	if (cp > 0x10FFFFu) throw std::runtime_error("Invalid UTF-8: code point out of range");
	if (cp >= 0xD800u && cp <= 0xDFFFu) throw std::runtime_error("Invalid UTF-8: surrogate code point");

	if (need == 1) {
		// 2-byte sequences must be >= 0x80
		if (cp < 0x80u) throw std::runtime_error("Invalid UTF-8: overlong 2-byte sequence");
	}
	else if (need == 2) {
		// 3-byte sequences must be >= 0x800
		if (cp < 0x800u) throw std::runtime_error("Invalid UTF-8: overlong 3-byte sequence");
	}
	else if (need == 3) {
		// 4-byte sequences must be >= 0x10000
		if (cp < 0x10000u) throw std::runtime_error("Invalid UTF-8: overlong 4-byte sequence");
	}

	return cp;
}

// UTF-8 -> UTF-16 (as stored in std::wstring: UTF-16 code units if wchar_t is 16-bit)
inline std::wstring utf8ToUtf16(std::string_view sv) {
	std::wstring out;
	out.reserve(sv.size()); // heuristic; may expand with surrogate pairs

	const char* p = sv.data();
	const char* end = p + sv.size();

	while (p < end) {
		uint32_t cp = decodeOneUtf8(p, end);
		appendCodepointToWString(out, cp);
	}
	return out;
}

// Accept const char* + length (safe for embedded NULs if you pass length)
inline std::wstring utf8ToUtf16(const char* s, size_t len) {
	if (!s && len != 0) throw std::runtime_error("utf8ToUtf16: null pointer");
	return utf8ToUtf16(std::string_view{ s, len });
}

// Accept std::string
inline std::wstring utf8ToUtf16(const std::string& s) {
	return utf8ToUtf16(std::string_view{ s });
}

// Accept null-terminated const char*
inline std::wstring utf8ToUtf16(const char* s) {
	if (!s) throw std::runtime_error("utf8ToUtf16: null pointer");
	return utf8ToUtf16(std::string_view{ s });
}
