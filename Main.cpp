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

#include "Greenshift.h"

//#include "Interval.h"
//#include "TextStream.h"

//#include <vector>
//#include "adetect.h"
/*
 *
 You only have to catch the WM_SETTEXT Messages that Winamp gets. 
Because Winamp adds to the windowcaption a [stopped], [pause] or anything else,
Windows will tell with the WM_SETTEXT Message that the Caption has changed. 
And when a new song is played, the Caption will change also. 

 */

Greenshift gGreenshift;
unsigned char cSpectrumData[2][SOUND_DATA_LENGTH];
unsigned char cWaveformData[2][SOUND_DATA_LENGTH];

//#define CRASH_THE_PROGRAM
#ifndef CRASH_THE_PROGRAM
#define WINMAIN        WinMain
#define WINMAIN2    WinMain2
#else
#define WINMAIN        WinMain2
#define WINMAIN2    WinMain
#endif

#include "MersenneTwister.h"

int PASCAL WINMAIN(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    error_t err = 0;
    int i;
    const value_t    pi = 3.14159265358979323846264338327950288419716939937510582097494f;
    value_t
        angularFrequency = 0.0625f,
        time = 0.0f;

    FILE    *outFile;

    MyDictionary<char*>    ddTest;

    MTRand mtRand;

//    DumpToFile( "error.txt", MAX_LINE_WIDTH_FLOAT, "\n" );
//    return 0;

/*    long square;

    for( square = 0; square < 32; square++ )
    {
//        DumpToFile( "error.txt", "sqrt( ", square, " ) = "  );
//        DumpToFile( "error.txt", long_sqrt( square ), "\n" );
        DumpToFile( "error.txt", "sqrt_x2( ", square, " ) = "  );
        DumpToFile( "error.txt", long_sqrt_x2( square ), "\n" );
    }

    return 0;/**/

/*    DWORD   dw = 9;

    struct {
        long    lValue;
        union {
            value_t    nValue;
            DWORD    dwValue;
        };
    } yadda = { 7, *(value_t*)&dw };

    DebugMessage( "", yadda.dwValue, "" );

    return 0;*/



//    outFile = fopen( "error.txt", "w" );

//    fprintf( outFile, "%f\n", mtRand.rand() );

//    fprintf( outFile, "%.40g\n", 3.1415926535897932f); //3846264338327950288419716939937510582097494f );
//    fprintf( outFile, "%.40g\n", 3.1415926535897932); //3846264338327950288419716939937510582097494 );

//    fclose( outFile );

    // output generated:
    // 3.1415927410125732
    // 3.1415926535897931


//    return 0;

    /*****************************/


    err = gGreenshift.Initialize( hInstance ); // TODO: JRDV: This is failing because I forgot to make a copy of the settings file... need to extract on a Windows 98 VM or something...
    if( err != SUCCESS )
    {
        outFile = fopen( "error.txt", "a" );
        fprintf( outFile, "%s%s", ErrorString(err), " from Main.cpp in greenshift.Initialize()\n" );
        fclose(outFile);
        return err;
    }

    while( (err = gGreenshift.Render()) == SUCCESS )
    {
        /*
         * horribly fake the wave data :P
         */
//        for( channel = 0; channel < this_mod->nCh; channel++ )
        {
            for( i = 0;  i < SOUND_DATA_LENGTH;  i++ )
            {
//                gSpectrumData[0][i] = (unsigned char)127;
//                gWaveformData[0x][i] = (unsigned char)0;
//                gSpectrumData[0][i] = (unsigned char)RANDOM(255);
//                gWaveformData[0][i] = (unsigned char)RANDOM(255);
    //            gSpectrumData[0][i] = (unsigned char)(127.0f * cos( (i/4.244 - time) / 8.0f ) + 127.0f);
                cSpectrumData[0][i] = (signed char)(127.0f * cos( (i/4.244 - time) / 8.0f ) + 127.0f);
//                gWaveformData[0][i] = (unsigned char)(127.0f * cos( (i/4.244 - time) / 8.0f ) + 127.0f);
/*                gWaveformData[0][i] = (unsigned char)(64+127.0f*GForce_clip(0.5f + (-1.0f/pi * sin(i/100.0f*time*8.0f)
                                                        -0.5f/pi * sin(i/100.0f*time*4.0f)
                                                        -0.25f/pi * sin(i/100.0f*time*2.0f))) );/**/
                cWaveformData[0][i] = (unsigned char)(64+64.0f*GForce_clip(0.5f + (-1.0f/pi * (value_t)sin(i/100.0f*time*8.0f)
                                                        -0.5f/pi * (value_t)sin(i/100.0f*time*4.0f)
                                                        -0.25f/pi * (value_t)sin(i/100.0f*time*2.0f))) );/**/
/*                gWaveformData[0][i] = (unsigned char)(65.0f+63.0f*
                                                    //    (1.3f/pi+
                                                        (-1.0f/pi * sin(i/50.0f*time*8.0f)
                                                         -0.5f/pi * sin(i/50.0f*time*4.0f))//8.0f )
                                                    );/**/

//                DumpToFile("error.txt", -1.0f/pi * sin((i/4.244 - time)/8.0f)
//                                                        -0.5f/pi * sin((i/4.244 - time)/4.0f)
//                                                        -0.25f/pi * sin((i/4.244 - time)/2.0f), "\n");
//    y = ymax * cos( kx + wt )
//    (yMax * cos( k * width - angularFrequency * (time *2) ) + (pBitCanvas->BufferHeight() / 2))
//    90 * cos( (width/4.244 - time) / 8 ) + height)

//                127.0f * cos( (i/4.244 - time) / 8.0f );

            }
        }

        gGreenshift.SetSoundData( 1, cWaveformData, 1, cSpectrumData );
        time += 1;
    }


    if( err > SUCCESS && err < ERR_UNDEFINED )
    {
        outFile = fopen( "error.txt", "a" );
        fprintf(outFile, "%s%s", ErrorString(err), " from Main.cpp in greenshift.Render()\n" );
        fclose(outFile);
    }


    return err;
//    return greenshift.DoYourThing(hInstance);
//    return greenshift.DoBitmap(hInstance);

}


/*
 * this is so I can hopefully see the disassembly for DoDelta_cpp
 *
 * Note, to get it to crash... you have to make the DoDelta functions public
 */
//*
int PASCAL WINMAIN2(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    error_t err = 0;
    BitCanvas *bitCanvas;
    MyDictionary<char*> dConfig;

    dConfig.SetValue( "display_width", "640" );
    dConfig.SetValue( "display_height", "480" );
    dConfig.SetValue( "visible_width", "640" );
    dConfig.SetValue( "visible_height", "480" );
    dConfig.SetValue( "fullscreen_depth", "8" );
    dConfig.SetValue( "calculated_depth", "8" );

    if( BitCanvas::New( &bitCanvas, &dConfig ) == SUCCESS )
    {
//        bitCanvas->DoDelta( (PIXELMAP*)NULL );
//        bitCanvas->CopyTo( NULL, 640, 480, 640*2, 1 );
//        bitCanvas->DoDelta_SSE( NULL );
//        bitCanvas->CopyTo16( NULL, NULL );
//        bitCanvas->DoDelta_x86( NULL );
//        bitCanvas->DoDelta_MMX( NULL );
    }

    return 1;
}
/**/


#ifdef UNDEFINED
/*
 * this is so I can hopefully see the disassembly for DoDelta_cpp
 *
 * Note, to get it to crash... you have to make the DoDelta functions public
 */
//*
int PASCAL WINMAIN2(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    error_t err = 0;
    BitCanvas *bitCanvas;

    if( BitCanvas::New( 640, 360, 32, &bitCanvas ) == SUCCESS )
    {
//        bitCanvas->DoDelta_SSE( NULL );
//        bitCanvas->CopyTo16( NULL, NULL );
//        bitCanvas->DoDelta_x86( NULL );
//        bitCanvas->DoDelta_MMX( NULL );
    }

    return 1;
}
/**/

#endif




#ifdef UNDEFINED
    Interval    percentX;
    Interval    logicalX;
    Interval    physicalX;

    percentX.SetBounds( 0, 1 );
    logicalX.SetBounds( -1.0, 1.0 );
    physicalX.SetBounds( 0, 640 );

    Expression *test = NULL;

//    std::vector<void*>    vec;
    /*
     * it won't compile "x" because I didn't define the variable x
     */
    if( (err = Expression::Compile("#('4'|'3'&'x')", &test )) == SUCCESS )
    {
        int matchLength = 0;

//        DumpToFile("testing something.txt", test->PrintString(), test->Evaluate(), "\n" );


        /* it's calling printstring! */
//        DumpToFile("regexps.txt", test->Match( "3x4", &matchLength ) ? "true ":"false ", matchLength, "\n" );
        SAFE_DELETE( test );
    }
    else
        return err;
#endif

//    DumpToFile( "cpucaps.txt", GetCPUCaps(HAS_CPUID));
//    DumpToFile( "cpucaps.txt", (char*)GetCPUCaps(CPU_VENDOR_STRING), "\n");
//    DumpToFile( "cpucaps.txt", (char*)GetCPUCaps(CPU_NAME_STRING), "\n");


//    HH_WINTYPE seven;
//    HtmlHelp();
//    HelpContextID
//    WinHelp(NULL, "C:\\samples\\helpproject\\helpproject.hlp", HELP_FORCEFILE, 0);
