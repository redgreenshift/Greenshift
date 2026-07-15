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

#ifndef _HighResolutionTimer_H_
#define _HighResolutionTimer_H_

#include <windows.h>

#ifndef UNDEFINED

/****************************************************************************
 *
 * HighResolutionTimer - abstract the high resolution timer as a class
 *
 ****************************************************************************/
class HighResolutionTimer
{
public:
    HighResolutionTimer()
    {
        QueryPerformanceFrequency( &nFrequency );
        Start();
        Stop();
//        DumpToFile("hrTimer.txt", nFrequency.QuadPart, "\n");
    };

    inline void Start( void )
    {
        QueryPerformanceCounter( &nStart );
    };
    inline void Stop( void )
    {
        QueryPerformanceCounter( &nStop );
    };
    inline value_t    Seconds( void )
    {
        Stop();
//        return (float)(nStop.QuadPart - nStart.QuadPart) / (float)nFrequency.QuadPart;
        return (value_t)(nStop.LowPart - nStart.LowPart) / (value_t)nFrequency.LowPart;
    };
    inline value_t    Milliseconds( void )
    {
        return 1000.0f * Seconds();
    };

protected:
LARGE_INTEGER    nFrequency;
LARGE_INTEGER    nStart;
LARGE_INTEGER    nStop;

};

#else
/*
#include <time.h>

class HighResolutionTimer
{
public:
    HighResolutionTimer()
    {
        Start();
        Stop();
    };

    inline void Start( void )
    {
        nStart = clock();
    };
    inline void Stop( void )
    {
        nStop = clock();
    };
    inline value_t Seconds( void )
    {
        Stop();
        return (value_t)(nStop - nStart) / (value_t)CLOCKS_PER_SEC;
    };
    inline value_t Milliseconds( void )
    {
        return 1000.0f * Seconds();
    };

protected:
clock_t        nStart;
clock_t        nStop;
};
*/
#endif  /* UNDEFINED */

#endif  /* _HighResolutionTimer_H_ */

