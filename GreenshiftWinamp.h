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
 * Association - set of associations of two objects
 *
 ****************************************************************************/

#ifndef _GreenshiftWinamp_H_
#define _GreenshiftWinamp_H_

#ifndef GREENSHIFT_DLL
#define GREENSHIFT_DLL
#endif

#include "Vis.h"
#include "Greenshift.h"

#define TITLE   VIS_TITLE " " VIS_VERSION
#define NAME    VIS_TITLE


Greenshift      gVisGreenshift; /* the global Greenshift instance...
                                 * should allocate dynamically
                                 * rather than use a global
                                 */



// returns a winampVisModule when requested. Used in hdr, below
winampVisModule *getModule(int which);

// "member" functions
void config(struct winampVisModule *this_mod); // configuration dialog
int  init(struct winampVisModule *this_mod);   // initialization for module
int  render(struct winampVisModule *this_mod); // rendering for module 1
void quit(struct winampVisModule *this_mod);   // deinitialization for module




// our window procedure
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//HWND hMainWnd; // main window handle



// Module header, includes version, description, and address of the module retriever function
winampVisHeader hdr = { VIS_HDRVER, TITLE, getModule };



// first module (oscilliscope)
winampVisModule modMain =
{
    TITLE,
    NULL,    // hwndParent
    NULL,    // hDllInstance
    44100,        // sRate  ////what value goes here?!
    2,        // nCh
//    25,        // latencyMS
    0,      // latencyMS
    0,        // delayMS
    2,        // spectrumNch
    2,        // waveformNch
    { 0, },    // spectrumData
    { 0, },    // waveformData
    config,
    init,
    render, 
    quit,
    &gVisGreenshift
};




#endif  /* _GreenshiftWinamp_H_ */

