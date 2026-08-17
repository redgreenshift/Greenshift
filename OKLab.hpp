#pragma once
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

#include <algorithm>
#include <cmath>
#include <iostream>

#include "Project Greenshift.h"
#include "Palette.h"

namespace OKLab
{
	// Björn Ottosson, the mathematician and engineer who created OKLab.
	// His code is available in public domain.
	// https://bottosson.github.io/posts/oklab/
	// https://en.wikipedia.org/wiki/Oklab_color_space

	struct RGB {
		value_t r, g, b;
	};

	typedef struct Lab {
		value_t L, a, b;
	} OKLab;

	inline Lab interpolate_oklab(const Lab& start, const Lab& end, value_t t)
	{
		// Clamp t to [0, 1] to prevent overshooting the colors
		t = std::clamp<value_t>(t, 0.0f, 1.0f);

		Lab result;

		// Using C++20 std::lerp
		result.L = std::lerp(start.L, end.L, t);
		result.a = std::lerp(start.a, end.a, t);
		result.b = std::lerp(start.b, end.b, t);

		/*
		// If not using C++20, use this manual formula:
		result.L = start.L + (end.L - start.L) * t;
		result.a = start.a + (end.a - start.a) * t;
		result.b = start.b + (end.b - start.b) * t;
		*/

		return result;
	}


	Lab linear_srgb_to_oklab(const RGB& c)
	{
		// 2. Linear RGB -> LMS Space
		// These coefficients are the standard OKLab matrices
		value_t l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
		value_t m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
		value_t s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

		// 3. Non-linear transformation (Cube Root)
#ifdef USE_HIGH_PRECISION
		value_t l_ = std::cbrt(l);
		value_t m_ = std::cbrt(m);
		value_t s_ = std::cbrt(s);
#else
		value_t l_ = cbrtf(l);
		value_t m_ = cbrtf(m);
		value_t s_ = cbrtf(s);
#endif

		// 4. Non-linear LMS -> OKLab
		return {
			0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
			1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
			0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_,
		};
	}

	// Helper to convert sRGB (non-linear) to Linear RGB
	inline value_t sRGB_to_linear(value_t color)
	{
		if (color <= 0.04045f) {
			return color / 12.92f;
		}
		else {
			return std::pow((color + 0.055f) / 1.055f, 2.4f);
		}
	}

	inline Lab rgb_to_oklab(const RGB& rgb)
	{
		// 1. Convert sRGB to Linear RGB
		RGB lin = { sRGB_to_linear(rgb.r),
					sRGB_to_linear(rgb.g),
					sRGB_to_linear(rgb.b)};

		return linear_srgb_to_oklab(lin);
	}





	RGB oklab_to_linear_srgb(const Lab& c)
	{
		// 1. OKLab -> Non-linear LMS
		value_t l_ = c.L + 0.3963377774f * c.a + 0.2158037573f * c.b;
		value_t m_ = c.L - 0.1055613458f * c.a - 0.0638541728f * c.b;
		value_t s_ = c.L - 0.0894841775f * c.a - 1.2914855480f * c.b;

		// 2. Non-linear LMS -> Linear LMS (Cube the values)
		value_t l = l_ * l_ * l_;
		value_t m = m_ * m_ * m_;
		value_t s = s_ * s_ * s_;

		// 3. Linear LMS -> Linear RGB
		return {
			+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
			-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
			-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
		};
	}

	// Helper to convert Linear RGB back to sRGB (gamma encoding)
	inline value_t linear_to_SRGB(value_t color)
	{
		color = std::clamp<value_t>(color, 0.0f, 1.0f);
		if (color <= 0.0031308f) {
			return 12.92f * color;
		}
		else {
			return 1.055f * std::pow(color, 1.0f / 2.4f) - 0.055f;
		}
	}

	inline RGB oklab_to_rgb(const Lab& c)
	{
		RGB lin = oklab_to_linear_srgb(c);
		// 4. Linear RGB -> sRGB (Gamma encoding) and Clamp to [0, 1]
		RGB result;
		result.r = std::clamp<value_t>(linear_to_SRGB(lin.r), 0.0f, 1.0f);
		result.g = std::clamp<value_t>(linear_to_SRGB(lin.g), 0.0f, 1.0f);
		result.b = std::clamp<value_t>(linear_to_SRGB(lin.b), 0.0f, 1.0f);

		return result;
	}

	static inline value_t fromU8(uint8_t b) {
		return static_cast<value_t>(b) / 255.0f;
	}
	static inline uint8_t toU8(value_t x01) {
		x01 = std::clamp<value_t>(x01, 0.0f, 1.0f);
		return static_cast<uint8_t>(std::lround(x01 * 255.0f));
	}

	/**
	 * @brief Good for natural looking smooth transitions between colors
	 * (i.e. avoid muddy colors that can occur when interpolating through RGB space)
	 */
	inline PALETTEENTRY interpolate(const PALETTEENTRY& rgbpal1, const PALETTEENTRY& rgbpal2, const value_t nPercent)
	{
		RGB rgb1 = { fromU8(rgbpal1.peRed), fromU8(rgbpal1.peGreen), fromU8(rgbpal1.peBlue) };
		RGB rgb2 = { fromU8(rgbpal2.peRed), fromU8(rgbpal2.peGreen), fromU8(rgbpal2.peBlue) };

		Lab oklab1 = rgb_to_oklab(rgb1);
		Lab oklab2 = rgb_to_oklab(rgb2);
		Lab tmpOK = interpolate_oklab(oklab1, oklab2, nPercent);

		RGB tmpRGB = oklab_to_rgb(tmpOK);

		PALETTEENTRY final;
		final.peRed = toU8(tmpRGB.r);
		final.peGreen = toU8(tmpRGB.g);
		final.peBlue = toU8(tmpRGB.b);

		return final;
	}
}

