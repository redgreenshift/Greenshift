#pragma once
#include "Project Greenshift.h"
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

/****************************************************************************
 *
 * HighResolutionTimer - abstract the high resolution timer as a class
 *
 ****************************************************************************/

#include <chrono> // steady_clock wraps QueryPerformanceCounter (on Windows)
#include <ratio>

class HighResolutionTimer
{
public:
	HighResolutionTimer()
	{
		Start();
	};

	inline void Start(void)
	{
		nStart = nStop = std::chrono::steady_clock::now();
	};
	inline void Stop(void)
	{
		nStop = std::chrono::steady_clock::now();
	};

	inline value_t ElapsedSeconds(void) const
	{
		return std::chrono::duration<value_t>(
			std::chrono::steady_clock::now() - nStart).count();
	};
	inline value_t ElapsedMilliseconds(void) const
	{
		return std::chrono::duration<value_t, std::milli>(
			std::chrono::steady_clock::now() - nStart).count();
	};
	inline value_t StoppedSeconds(void) const
	{
		return std::chrono::duration<value_t>(nStop - nStart).count();
	};
	inline value_t StoppedMilliseconds(void) const
	{
		return std::chrono::duration<value_t, std::milli>(
			nStop - nStart).count();
	};

protected:
	std::chrono::steady_clock::time_point	nStart;
	std::chrono::steady_clock::time_point	nStop;
};

