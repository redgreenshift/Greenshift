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
 * GreenshiftWinamp
 *
 ****************************************************************************/

#include "GreenshiftWinamp.h"


#ifdef GREENSHIFT_DLL


/****************************************************************************
 *
 * config - thing Winamp calls to tell Greenshift to open a GUI prefs editor
 *
 ****************************************************************************/
void config(struct winampVisModule *this_mod)
{
    Greenshift *greenshift;

    greenshift = (Greenshift*)this_mod->userData;


    MessageBox(this_mod->hwndParent,
        "This module is Copyright (c) 2001-2026, Jared Ivey\n"
//        "This is the first official release of the Greenshift visualization dll\n"
        "There are no settings to configure here.  See settings.txt\n\n"
        "Updates at:\n"
        "    https://github.com/redgreenshift/greenshift",
                                    VIS_TITLE " " VIS_VERSION ,MB_OK);

    if( greenshift != NULL )
        greenshift->Config();
}

/****************************************************************************
 *
 * init - the thing Winamp calls to tell Greenshift to initialize
 *
 ****************************************************************************/
int init(struct winampVisModule *this_mod)
{
    Greenshift  *greenshift;
//    HWND        hWindow;

    greenshift = (Greenshift*)this_mod->userData;

//    hWindow = FindWindow( "Winamp v1.x", NULL );

//    MessageBox( NULL, (hWindow == NULL)?"not found":"found", "ack!", MB_OK );
//    MessageBox( NULL, (hWindow == this_mod->hwndParent)?"equal":"not equal",
    //"ack!", MB_OK );

    if( greenshift != NULL )
    {
#ifdef EXTREME_DEBUGGING
        error_t err;

        greenshift->SetParentWindow( this_mod->hwndParent );
        DumpToFile( "error.txt", "About to Initialize Greenshift.", "\n" );
        err = greenshift->Initialize( this_mod->hDllInstance );
        if( err == SUCCESS )
            DumpToFile("error.txt", "Greenshift successfully Initialized.\n");
        else
        {
            DumpToFile( "error.txt",
                "An ERROR occured while initializing Greenshift!", "\n" );
            DumpToFile( "error.txt", ErrorString( err ), "\n" );
        }
        return err;
#else
        greenshift->SetParentWindow( this_mod->hwndParent );
        return greenshift->Initialize( this_mod->hDllInstance );
#endif
    }
    else
        return -1;
}


/****************************************************************************
 *
 * render - the thing Winamp calls to tell Greenshift to render a frame
 *
 ****************************************************************************/
int render(struct winampVisModule *this_mod)
{
//    int channel;
//    int i;
    Greenshift    *greenshift;

    greenshift = (Greenshift*)this_mod->userData;

    if( greenshift != NULL )
    {
#ifdef EXTREME_DEBUGGING
        error_t err;

        DumpToFile( "error.txt", "About to set sound data.", "\n" );
        err = greenshift->SetSoundData( this_mod->waveformNch,
                                  this_mod->waveformData,
                                  this_mod->spectrumNch,
                                  this_mod->spectrumData );
        if( err == SUCCESS )
            DumpToFile( "error.txt", "Successfully set sound data.", "\n" );
        else
        {
            DumpToFile( "error.txt", "ERROR: ", "" );
            DumpToFile( "error.txt", ErrorString( err ), "\n" );
        }

        if( err == SUCCESS )
        {
            DumpToFile( "error.txt", "About to render frame.", "\n" );
            err = greenshift->Render();
            if( err == SUCCESS )
                DumpToFile("error.txt", "Successfully rendered the frame.\n");
            else
            {
                DumpToFile( "error.txt", "ERROR: ", "" );
                DumpToFile( "error.txt", ErrorString( err ), "\n" );
            }
        }
        return err;
#else
        greenshift->SetSoundData( this_mod->waveformNch,
                                  this_mod->waveformData,
                                  this_mod->spectrumNch,
                                  this_mod->spectrumData );

        return greenshift->Render();
#endif
    }
    else
        return -1;
}

/****************************************************************************
 *
 * quit - the thing Winamp calls to tell Greenshift to cleanup
 *
 ****************************************************************************/
void quit(struct winampVisModule *this_mod)
{
/* Greenshift cleans up after itself when the Greenshift object is destroyed
 * if crashes if you tell it to cean up here too */
}





// this is the only exported symbol. returns our main header.
//if you are compiling C++, the extern "C" { is necessary, so we just #ifdef it
#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) winampVisHeader *winampVisGetHeader()
{
    return &hdr;
}
#ifdef __cplusplus
}
#endif



// getmodule routine from the main header. Returns NULL if an
// invalid module was requested,
// otherwise return the only module
winampVisModule *getModule(int which)
{
    switch (which)
    {
        case 0: return &modMain;
        default:return NULL;
    }
}

#endif  /* GREENSHIFT_DLL */

