#pragma once
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

#include <cstdint>
#include <string>
#include <string_view>


// BEGIN -- Delete with codecvt_utf8_utf16
#ifdef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
std::wstring CODECVT_utf8_to_wstring(const std::string& s);
#endif // _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
// END -- Delete with codecvt_utf8_utf16


#ifdef WIN32
std::wstring WINDOWS_utf8_to_utf16(const std::string_view utf8);
#endif

constexpr wchar_t InvalidSequenceReplacementChar = static_cast<wchar_t>(0xFFFD); // standard empty box character signifying an invalid byte sequence
enum class Utf8ErrorPolicy {
	Replace,	// append U+FFFD and continue
	Return,		// return false on first invalid sequence, or empty string if using an overload that just returns a string
	Throw,		// throw an exception on first invalid sequence
};

inline std::wstring utf8_to_wstring(std::istream& utf8, Utf8ErrorPolicy policy); // core
// helpers:
inline std::wstring utf8_to_wstring(const std::string& utf8, Utf8ErrorPolicy policy);
inline std::wstring utf8_to_wstring(const char* utf8, Utf8ErrorPolicy policy);

inline std::wstring utf8_to_wstring(std::istream& utf8);
inline std::wstring utf8_to_wstring(const std::string& utf8);
inline std::wstring utf8_to_wstring(const char* utf8);
inline std::wstring utf8_to_wstring(const char* pszUtf8, size_t cchUtf8);

enum class Utf8Certainty
{
	NotUtf8,	// invalid UTF-8 byte sequence
	MaybeUtf8,	// only ASCII bytes: valid UTF-8 but ambiguous intent/encoding
	Utf8,		// valid UTF-8 with at least one byte >= 0x80
};

Utf8Certainty is_valid_utf8(const uint8_t* data, size_t len);

