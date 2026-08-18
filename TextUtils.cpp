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
// DONE: Want an IsUTF8 function with yes/no/maybe
// DONE: Unit Tests for ShiftJIS, include encodings that do and DO NOT conform to UTF8.
// https://en.wikipedia.org/wiki/UTF-8
// https://en.wikipedia.org/wiki/Shift-JIS

#include <cstddef> // is_valid_utf8
#include <cstdint> // is_valid_utf8, append_codepoint_as_wchar
#include <istream>
#include <limits> // append_codepoint_as_wchar
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string> // append_codepoint_as_wchar
#include <string_view>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // Delete with codecvt_utf8_utf16
#include "TextUtils.hpp"

#ifdef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // Delete with codecvt_utf8_utf16
#include <codecvt> // Delete with codecvt_utf8_utf16

// This used to be the easy way to convert, then it was deprecated,
// and the suggestion was to use MultiByteToWideChar, but that is Windows specific.
// I want to at least try to keep as much of Greenshift agnostic about the platform as possible.
std::wstring CODECVT_utf8_to_wstring(const std::string & s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(s);
}
#endif // _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


static constexpr Utf8ErrorPolicy Utf8ErrorDefaultPolicy = Utf8ErrorPolicy::Return;
static inline std::wstring ATTEMPT4_utf8_to_wstring(std::istream& in, Utf8ErrorPolicy policy);

// CORE: As I explore implementations, I want to be able to swap them out,
// so this is the point to swap out implementations
inline std::wstring utf8_to_wstring(std::istream& stm, Utf8ErrorPolicy policy)
{
	return ATTEMPT4_utf8_to_wstring(stm, policy);
}

inline std::wstring utf8_to_wstring(const std::string& strUtf8, Utf8ErrorPolicy policy)
{
	// istringstream needs an owning string (string is owning).
	// No conversion necessary
	std::istringstream in(strUtf8);

	return utf8_to_wstring(in, policy);
}

inline std::wstring utf8_to_wstring(const char* pszUtf8, Utf8ErrorPolicy policy)
{
	// istringstream needs an owning string (char* is owning).
	std::istringstream in(pszUtf8);

	return utf8_to_wstring(in, policy);
}

// Accept std::istream
inline std::wstring utf8_to_wstring(std::istream& stm)
{
	return utf8_to_wstring(stm, Utf8ErrorDefaultPolicy);
}

// Accept std::string
inline std::wstring utf8_to_wstring(const std::string& strUtf8)
{
	return utf8_to_wstring(strUtf8, Utf8ErrorDefaultPolicy);
}

// Accept null-terminated const char*
inline std::wstring utf8_to_wstring(const char* pszUtf8)
{
	return utf8_to_wstring(pszUtf8, Utf8ErrorDefaultPolicy);
}


class utf8_bytes_rangebuf : public std::streambuf
{
public:
	utf8_bytes_rangebuf(const char* data, std::size_t len)
	{
		// streambuf wants char*; reading doesn't modify the buffer.
		char* b = const_cast<char*>(data); // streambuf uses char* internally
		char* e = b + len;
		setg(b, b, e); // [gptr(), egptr()) is the readable range
	}
	// No overrides required for peek/get-based decoding.
};

// Accept const char* + length (safe for embedded NULs if you pass length)
inline std::wstring utf8_to_wstring(const char* pszUtf8, size_t cchUtf8)
{
	utf8_bytes_rangebuf buf(pszUtf8, cchUtf8); // Important lifetime rule, this avoids an extra std::string copy, pszUtf8 must remain valid (and unchanged) for the duration of `utf8_to_wstring(in)`
	std::istream in(&buf);
	return utf8_to_wstring(in);
}


// 'std::codecvt_utf8_utf16<wchar_t,1114111,(std::codecvt_mode)0>': warning STL4017 :
// std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17.
// (The std::codecvt class template is NOT deprecated.) The C++ Standard doesn't provide equivalent non-deprecated functionality; consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead.
// You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to suppress this warning.
#ifdef WIN32
#include <windows.h>

// This is the most straightforward replacement, as I don't have to _implement_ the feature myself... but it's platform specific, which is less than ideal.
std::wstring WINDOWS_utf8_to_utf16(const std::string_view utf8)
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


/****************************************************************************
 * Helpers for decoding UTF-8 sequences and appending to std::wstring
 */


 /**
  * @brief Appends a Unicode codepoint to a wide string, handling encoding based on platform wchar_t size.
  *
  * Encodes the given codepoint into the output's character representation. If the codepoint is
  * invalid (above U+10FFFF), it is replaced with the replacement character.
  *
  * On platforms where `wchar_t` is 2 bytes, this performs UTF-16 encoding (surrogate pairs for non-BMP).
  *
  * On platforms where `wchar_t` is 4 bytes, the codepoint is stored directly as a single unit.
  *
  * @param[in,out] out  The destination wide string buffer to which the codepoint is appended.
  * @param[in]     cp   The Unicode codepoint to encode.
  */
static void append_codepoint_as_wchar(std::wstring& out, uint32_t cp)
{
	if (cp > 0x10FFFF) cp = InvalidSequenceReplacementChar;

#if WCHAR_MAX <= 0xFFFF
	static_assert(sizeof(wchar_t) == 2, "Expected 2-byte wchar_t on this platform");

	// Encode into UTF-16 code units
	if (cp <= 0xFFFF)
	{
		out.push_back(static_cast<wchar_t>(cp));
	}
	else
	{
		cp -= 0x10000;
		wchar_t high = static_cast<wchar_t>(0xD800 + (cp >> 10));
		wchar_t low = static_cast<wchar_t>(0xDC00 + (cp & 0x3FF));
		out.push_back(high);
		out.push_back(low);
	}
#else
	// On UTF-8 systems where wchar_t is >= 4 bytes, full Unicode scalars are preserved directly.
	static_assert(sizeof(wchar_t) >= 4, "Expected at least 4-byte wchar_t on this platform");

	// wchar_t wide enough: store the code point directly
	// If wchar_t is 32-bit, std::wstring can hold full code points directly.
	out.push_back(static_cast<wchar_t>(cp));
#endif
}

// continuation byte
static inline bool is_cont_byte(uint8_t b)
{
	return (b & 0xC0) == 0x80; // 10xxxxxx
}

static inline bool is_valid_unicode_scalar(uint32_t cp)
{
	if (cp > 0x10FFFF) return false;
	// Reject surrogate code points (Unicode scalar values exclude them)
	if (cp >= 0xD800 && cp <= 0xDFFF) return false;
	return true;
}

static inline bool peek_byte(std::istream& in, uint8_t& outByte)
{
	int ch = in.peek();
	if (ch == EOF) return false;
	outByte = static_cast<uint8_t>(ch);
	return true;
}

static inline uint8_t get_byte(std::istream& in)
{
	int ch = in.get();
	// Caller must ensure in.peek() != EOF before calling.
	return static_cast<uint8_t>(ch);
}

enum class DecodeOneResult {
	Ok,		// cpOut is valid
	Eof,	// no byte was available to decode
	Invalid	// invalid sequence encountered (cpOut set only for Replace)
};

static DecodeOneResult handle_invalid(
	uint32_t& cpOut, // Out parameter for the invalid codepoint result
	Utf8ErrorPolicy policy,
	const char* message)
{
	if (policy == Utf8ErrorPolicy::Throw)
	{
		throw std::runtime_error(message);
	}
	if (policy == Utf8ErrorPolicy::Replace)
	{
		cpOut = InvalidSequenceReplacementChar;
	}
	return DecodeOneResult::Invalid; // Caller handles replacing U+FFFD if policy is Replace
}

#pragma region ATTEMPT4_utf8_to_wstring

/**
 * @brief Decodes a single UTF-8 code point from the provided input stream.
 *
 * Consumes one or more bytes from the input stream to produce a Unicode scalar value.
 * The stream is advanced at least once if an invalid sequence is encountered.
 *
 * @param[out] cpOut    Receives the decoded code point on success, or the
 *                      replacement character (U+FFFD) if policy is Replace.
 * @param[in]  inStream The input stream to read bytes from.
 * @param[in]  policy   The error handling strategy used when a sequence is invalid.
 * @return DecodeOneResult:
 *         - Ok: Success; `cpOut` contains the valid Unicode scalar value.
 *         - Eof: No bytes were available in the stream at the start of the call.
 *         - Invalid: The sequence was malformed or truncated; the stream has been
 *                    advanced past the invalid leading byte(s).
 */
static DecodeOneResult decode_one_utf8_cp(
	uint32_t& cpOut,
	std::istream& inStream,
	Utf8ErrorPolicy policy)
{
	cpOut = 0;

	uint8_t b0 = 0;
	if (!peek_byte(inStream, b0))
	{
		return DecodeOneResult::Eof;
	}

	// Consume leading byte
	b0 = get_byte(inStream);

	// ASCII fast path
	if (b0 <= 0x7F)
	{
		cpOut = b0;
		return DecodeOneResult::Ok;
	}

	// Reject continuation bytes as leading bytes (0x80..0xBF)
	if ((b0 & 0xC0) == 0x80)
	{
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: invalid leading byte (found continuation byte 10xxxxxx)");
	}

	int needed = 0;
	uint32_t acc = 0;

	if ((b0 & 0xE0) == 0xC0)		// 110xxxxx
	{
		needed = 2;
		acc = b0 & 0x1Fu;
	}
	else if ((b0 & 0xF0) == 0xE0)	// 1110xxxx
	{
		needed = 3;
		acc = b0 & 0x0Fu;
	}
	else if ((b0 & 0xF8) == 0xF0)	// 11110xxx
	{
		needed = 4;
		acc = b0 & 0x07u;
	}
	else
	{
		// Includes patterns like 11111xxx, 111110xx etc, i.e., > 0xF4
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: invalid leading byte (unsupported above U+10FFFF)");
	}

	// Additional immediate leading-byte restrictions
	// These are the standard overlong-avoidance bounds:
	// - For 2-byte sequences, b0 must be >= 0xC2 (reject C0/C1 overlongs)
	// - For 3-byte sequences:
	//     b0=0xE0 requires b1>=0xA0
	//     b0=0xED requires b1<=0x9F (to avoid surrogates)
	// - For 4-byte sequences:
	//     b0=0xF0 requires b1>=0x90
	//     b0=0xF4 requires b1<=0x8F
	//     b0>0xF4 is out of range
	if (needed == 2 && b0 < 0xC2)
	{
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: overlong 2-byte sequence (leading byte too small)");
	}
	if (needed == 4 && b0 > 0xF4)
	{
		// Note: This should have been caught and rejected above in the invalid leading byte check;
		// theoretically this code block is dead code, but included as a safety net.
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: code point out of range (leading byte > 0xF4)");
	}

	// Read continuation bytes
	uint8_t b1 = 0, b2 = 0, b3 = 0;
	uint8_t cont[3] = { 0,0,0 };

	for (int i = 1; i < needed; ++i)
	{
		uint8_t bi = 0;
		if (!peek_byte(inStream, bi))
		{
			// We already consumed b0; stream advanced past at least b0.
			//	NOTE: we do NOT return DecodeOneResult::Eof here because
			//	we already consumed the leading byte (EOF means nothing
			//	consumed), so we must return Invalid here and let the caller
			//	handle it (potentially emitting the replacement character).
			//	We'll return EOF on the next call.
			return handle_invalid(cpOut, policy,
				"Invalid UTF-8: truncated sequence (EOF in the middle of a code point)");
		}
		bi = get_byte(inStream);

		if (!is_cont_byte(bi))
		{
			return handle_invalid(cpOut, policy,
				"Invalid UTF-8: invalid continuation byte (expected 10xxxxxx)");
		}

		cont[i - 1] = bi;
		acc = (acc << 6) | (static_cast<uint32_t>(bi & 0x3F));
	}

	// Now apply overlong + surrogate + range constraints using the actual decoded scalar.
	uint32_t cp = acc;

	// Overlong checks based on final cp value:
	if (needed == 2 && cp < 0x80)
	{
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: overlong 2-byte sequence");
	}
	if (needed == 3 && cp < 0x800)
	{
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: overlong 3-byte sequence");
	}
	if (needed == 4 && cp < 0x10000)
	{
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: overlong 4-byte sequence");
	}

	if (!is_valid_unicode_scalar(cp))
	{
		// Reject above max code point > U+10FFFF
		if (cp > 0x10FFFF)
		{
			return handle_invalid(cpOut, policy,
				"Invalid UTF-8: decoded code point is out of range (> U+10FFFF)");
		}
		// Reject surrogate code points (D800-DFFF)
		return handle_invalid(cpOut, policy,
			"Invalid UTF-8: decoded value is a surrogate code point (D800-DFFF)");
	}

	// NOTE:
	// Here is where to (optionally) reject U+FFFE/U+FFFF; since they're
	// typically allowed as scalar values, we'll leave them for now.

	// If needed==3 or 4, we can also enforce the special b0/b1 boundary checks
	// that prevent surrogates and additional overlong forms.
	// (These checks are redundant with the scalar + overlong checks above, but they give
	// more precise rejection points if you want them.)
	//
	// For correctness they are not required here because the overlong + surrogate checks
	// already reject all invalid code points. So we keep this simple.

	cpOut = cp;
	return DecodeOneResult::Ok;
}

// UTF-8 -> UTF-16 (as stored in std::wstring: UTF-16 code units if wchar_t is 16-bit)


/**
 * @brief Decodes an entire UTF-8 encoded input stream into a wide string (std::wstring).
 *
 * Iteratively reads bytes from the provided `in` stream, decoding them as UTF-8 code points.
 * Each decoded code point is appended to the output buffer via `append_codepoint_as_wchar`.
 *
 * The process terminates when:
 *
 * - EOF is reached in the input stream (the source is exhausted).
 *
 * - An invalid UTF-8 sequence is encountered, which is handled according to the provided `policy`;
 *       a 'Return' or 'Throw' policy triggers a termination of the decoding process due to an error state.
 *
 * @param[in,out] in     The input stream providing the UTF-8 encoded bytes.
 * @param[in]     policy The error handling strategy used when an invalid sequence is encountered;
 *                       a 'Replace' policy emits a replacement character and attempts to "resynchronize"
 *                       at the next valid leading byte.
 * @return The decoded wide string as a `std::wstring`. If a fatal error occurs
 *         (e.g., via the 'Return' policy), returns an empty string.
 */
static inline std::wstring ATTEMPT4_utf8_to_wstring(std::istream& in, Utf8ErrorPolicy policy)
{
	std::wstring out;

	uint32_t cp = 0;

	while (true)
	{
		DecodeOneResult r = decode_one_utf8_cp(cp, in, policy);
		if (r == DecodeOneResult::Eof)
		{
			break;
		}
		if (r == DecodeOneResult::Ok)
		{
			append_codepoint_as_wchar(out, cp);
			continue;
		}

		// r == Invalid:
		if (policy == Utf8ErrorPolicy::Replace)
		{
			// decode_one_utf8_cp should have set cp to U+FFFD
			append_codepoint_as_wchar(out, InvalidSequenceReplacementChar);
			continue;
		}

		// policy == Return: stop immediately (Throw would already be handled inside decode_one_utf8_cp)
		// Currently no value to indicate failure to caller, therefore we MUST NOT hand back silently truncated data.
		out.clear();
		break;
	}

	return out;
}

/**
 * @brief Validates whether a given buffer of bytes represents valid UTF-8.
 *
 * Iteratively decodes the provided byte range into Unicode code points using a stream interface.
 * If any invalid sequence is encountered, validation fails immediately.
 *
 * @param[in] pszUtf8 Pointer to the start of the UTF-8 encoded buffer.
 * @param[in] len     The size of the input buffer in bytes.
 * @return The certainty level of the result:
 *         - `Utf8Certainty::NotUtf8`: An invalid/malformed sequence was found within the buffer.
 *         - `Utf8Certainty::Utf8`: The buffer is valid UTF-8 and contains at least one non-ASCII character (detectable).
 *         - `Utf8Certainty::MaybeUtf8`: The buffer is valid UTF-8 and consists entirely of ASCII characters;
 *           it remains 'maybe' because without a byte >127, we cannot distinguish between pure ASCII text and
 *           an encoding like ANSI (which would require an external codepage to differentiate/decode properly).
 */
Utf8Certainty is_valid_utf8(const uint8_t* pszUtf8, size_t len)
{
	Utf8Certainty ret = Utf8Certainty::MaybeUtf8; // Assume ASCII-only until proven otherwise
	utf8_bytes_rangebuf buf(reinterpret_cast<const char*>(pszUtf8), len); // Important lifetime rule, this avoids an extra std::string copy, pszUtf8 must remain valid (and unchanged) for the duration of this call
	std::istream in(&buf);

	uint32_t cp = 0;

	while (true)
	{
		DecodeOneResult r = decode_one_utf8_cp(cp, in, Utf8ErrorPolicy::Return);
		if (r == DecodeOneResult::Eof)
		{
			break;
		}
		if (r == DecodeOneResult::Ok)
		{
			// So far, it is well-formed UTF-8.
			// If we see any non-ASCII code point, the result will be unambiguous:
			// either fully valid UTF-8, or Invalid ("Maybe" is no longer an option)
			if (cp > 0x7F)
				ret = Utf8Certainty::Utf8;
			continue;
		}

		// stop immediately, invalid UTF-8 sequence encountered
		return Utf8Certainty::NotUtf8;
	}

	return ret;
}

#pragma endregion // ATTEMPT4_utf8_to_wstring
//// TODO: JRDV: Consider more efficient approaches

/**
 * @brief Converts the given string to lowercase in-place.
 *
 * This function modifies the original string passed by reference.
 *
 * @param s - The string to be converted.
 * @return The transformed string (modifies the original input in-place).
 */
std::string makeLowercase(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char ch) { return std::tolower(ch); });
	return s;
}

/**
 * @brief Checks if the haystack contains the needle using case-insensitive matching.
 *
 * The function converts both the haystack and the needle to lowercase
 * before performing the search.
 *
 * @param pHaystack - A null-terminated C-string to search within.
 * @param pNeedle - A null-terminated C-string to search for.
 * @return true if the needle is found within the haystack; false otherwise.
 */
bool StrContainsStrI(const char* pHaystack, const char* pNeedle)
{
	std::string haystack = { pHaystack };
	std::string needle = { pNeedle };

	makeLowercase(haystack);
	makeLowercase(needle);

	return haystack.find(needle) != std::string::npos;
}

