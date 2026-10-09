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

#if EXTREME_DEBUGGING

#include "CoreTypes.hpp"

#include <cstdio>
#include <cstdlib>
#include <Windows.h> // MessageBoxA


void DumpToFile(const char* fileName, const char* aString, value_t aNumber, const char* anotherString = "");
void DumpToFile(const char* fileName, value_t aNumber, const char* anotherString = "");
void DumpToFile(const char* fileName, value_t* aNumber, const char* anotherString = "");
void DumpToFile(const char* fileName, const char* aString, const char* anotherString = "");


inline void    ProjectGreenshiftDebugMessageBox(const char* aString, value_t aNumber, const char* anotherString = "")
{
	char    strDisplay[2048];

	int ret = snprintf(strDisplay, _countof(strDisplay), "%s%.40g%s", aString, aNumber, anotherString);
	if (ret < 0 || (size_t)ret >= _countof(strDisplay))
	{
		// truncation or encoding error
		return;
	}

	MessageBoxA(NULL, strDisplay, "Debug MessageBox", MB_OK);
}

#endif  /* EXTREME_DEBUGGING */

#if EXTREME_DEBUGGING
#define DebugMessage    ProjectGreenshiftDebugMessageBox
#else
#define DebugMessage(a,b,c)    /* nothing */
#endif /* EXTREME_DEBUGGING */
