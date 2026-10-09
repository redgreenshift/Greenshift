#pragma once
/*
 *  Copyright (C) 2001-2026 Jared Ivey
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

// ---------------------------------------------------------------------------
// Fundamental integer types
// ---------------------------------------------------------------------------

using byte_t = std::uint8_t;
using word_t = std::uint16_t;
using dword_t = std::uint32_t;

// ---------------------------------------------------------------------------
// Project-wide types
// ---------------------------------------------------------------------------

//enum error_t;

const char* ErrorString(error_t error);

// ---------------------------------------------------------------------------
// Numeric precision configuration
// ---------------------------------------------------------------------------

#define USE_HIGH_PRECISION_FLOAT 0

#if USE_HIGH_PRECISION_FLOAT
using value_t = double;
constexpr std::size_t VALUE_T_SIZE{ 8 };
#else

using value_t = float;
//constexpr std::size_t VALUE_T_SIZE{ 4 };
#define VALUE_T_SIZE 4

#endif // !USE_HIGH_PRECISION_FLOAT

// ---------------------------------------------------------------------------
// Legacy string types
// ---------------------------------------------------------------------------

using mychar_t = const char;
using char_t = const char;
using string_t = mychar_t*;

// Future possibility:
// using string_t = std::string;

// ---------------------------------------------------------------------------
// Pixel storage
// ---------------------------------------------------------------------------

using pixelmap_t = std::uint32_t;
using ppixelmap_t = pixelmap_t*;
