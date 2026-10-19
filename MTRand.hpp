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

// Current implementation selection.
#if 0
#include "MersenneTwister.h"
using MTRand = MTRand2001;
#else
#include "SFMTRand.hpp"
using MTRand = SFMTRand;
#endif


#include <concepts>
#include <cstdint>

// Enforce a particular contract for the MTRand implementation
template<typename T>
concept MTRandLike =
    requires(T rng,
            uint32_t ui32, const uint32_t cui32, const uint32_t * pcui32, const uint32_t & rcui32, uint32_t * const cpui32, uint32_t * pui32,
            uint64_t ui64, const uint64_t cui64, const uint64_t * pcui64, const uint32_t & rcui64,
            float f32,
            double f64)
{
    { T(ui32) };
    { T() };

    { rng.rand() } -> std::same_as<double>;
    { rng.rand(f64) } -> std::same_as<double>;
    { rng.randExc() } -> std::same_as<double>;
    { rng.randExc(f64) } -> std::same_as<double>;
    { rng.randInt() } -> std::same_as<uint32_t>;
    { rng.randInt(ui32) } -> std::same_as<uint32_t>;
    { rng.operator()() } -> std::same_as<double>;

    { rng.seed(ui32) } -> std::same_as<void>;
    { rng.seed() } -> std::same_as<void>;
    { rng.unique_seed() } -> std::same_as<void>;
    { rng.deterministic_seed_u64(ui64) } -> std::same_as<void>;
    { rng.deterministic_seed_f64(f64) } -> std::same_as<void>;
    { rng.deterministic_seed_u32(ui32) } -> std::same_as<void>;
    { rng.deterministic_seed_f32(f32) } -> std::same_as<void>;
};

static_assert(MTRandLike<MTRand>, "Missing a member of the MTRand interface!");

