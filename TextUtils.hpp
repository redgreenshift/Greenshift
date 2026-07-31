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

#include <string>

enum class Utf8Certainty
{
	NotUtf8,	// invalid UTF-8 byte sequence
	MaybeUtf8,	// only ASCII bytes: valid UTF-8 but ambiguous intent/encoding
	Utf8,		// valid UTF-8 with at least one byte >= 0x80
};

// BEGIN -- Delete with codecvt_utf8_utf16
#ifdef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
std::wstring CODECVT_utf8_to_wstring(const std::string& s);
#endif // _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
// END -- Delete with codecvt_utf8_utf16


#ifdef WIN32
std::wstring WINDOWS_utf8_to_utf16(const std::string& utf8);
#endif

std::wstring utf8_to_wstring(const std::string& s);

std::wstring ATTEMPT1_utf8_to_wstring(const std::string& s);
bool isValidUtf8(const uint8_t* data, size_t len);
Utf8Certainty classifyUtf8(const uint8_t* data, size_t len);

// Replacement Attempt 3:
inline std::wstring utf8ToUtf16(std::string_view sv);
inline std::wstring utf8ToUtf16(const char* s, size_t len);
inline std::wstring utf8ToUtf16(const std::string& s);
inline std::wstring utf8ToUtf16(const char* s);
