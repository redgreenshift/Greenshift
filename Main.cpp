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
#define HIDE_FROM_WINDOWS
//#include "..\\..\\cs\\cs4451a\\cs4451a\\cs4451_a1.c"

//#define CS4451a_project_3 // I don't even remember why I used code from a college project.... it was to test an algorithm. JRDV: Clean this up

#ifdef CS4451a_project_3
BitCanvas* gpBitCanvas = NULL;
BYTE         grColor = 0xff;
BYTE         ggColor = 0xff;
BYTE         gbColor = 0xff;
#undef WINMAIN
#undef WINMAIN2
#define WINMAIN        WinMain1
#define WINMAIN2    WinMain2
#define glBegin(ignore)   /* ignore, assume GL_POINTS */
#define glEnd()     /* ignore */
#define glColor3b(r,g,b)   grColor = r; ggColor = g; gbColor = b;
#define glColor3f(r,g,b)   grColor = (BYTE)((r)*255); ggColor = (BYTE)((g)*255); gbColor = (BYTE)((b)*255);
#define glVertex2i(x, y)    gpBitCanvas->BlahPutPixel((x), vpd-(y)-1, grColor, ggColor, gbColor);
/*    GLvoid glVertex2i(GLint x, GLint y)
	{
		DWORD r=0xff, g=0xff, b=0xff;
		gpBitCanvas->BlahPutPixel(x, vpd-y-1, r, g, b);
	}
	*/
#include "..\\..\\cs\\cs4451a\\p3\\cs4451_a3.c"

int PASCAL WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	error_t err = 0;

	WindowDevice* window_device;
	Dictionary<char*> dConfig;
	FILE* datafile;

	dConfig.SetValue("screen_depth", "32");
	dConfig.SetValue("canvas_depth", "32");
	dConfig.SetValue("buffer_width", "800");
	dConfig.SetValue("buffer_height", "600");
	dConfig.SetValue("canvas_width", "512");
	dConfig.SetValue("canvas_height", "512");
	dConfig.SetValue("canvas_aspect", "1");
	dConfig.SetValue("window_width", "800");
	dConfig.SetValue("window_height", "600");
	dConfig.SetValue("screen_width", "800");
	dConfig.SetValue("screen_height", "600");
	dConfig.SetValue("show_debug_info", "0");
	dConfig.SetValue("show_framerate", "0");
	dConfig.SetValue("fullscreen", "0");
	dConfig.SetValue("maximum_frames", "22");
	dConfig.SetValue("default_frames", "22");
	dConfig.SetValue("tween_repeat", "4");
	dConfig.SetValue("plain_repeat", "1");
	dConfig.SetValue("overlay_mode", "0");
	dConfig.SetValue("overlay_color_key", "0");

	WindowDevice::New(hInstance, &window_device, "CS4451_a3 - Ray Tracer", &dConfig, NULL, NULL, NULL);
	BitCanvas::New(&gpBitCanvas, &dConfig, NULL, NULL);



	//    DumpToFile("error.txt", "blah?!asd: ", read_file( fopen( "d:\\cs\\cs4451a\\cs4451a\\testdata", "r" ) ) );
	vpd = 512;
	//    datafile = fopen( "d:\\cs\\cs4451a\\p3\\testdata", "r" );
	datafile = fopen("d:\\cs\\cs4451a\\p3\\input4", "r");
	init_scene(datafile);
	fclose(datafile);

	//    DumpToFile( "error.txt", "vpd: ", vpd );
	//    DumpToFile( "error.txt", "numSpheres: ", numSpheres );


	//glVertex2i
	//call draw()
	while (window_device != NULL
		&& window_device->IsActive())
	{
		draw_scene();

		gpBitCanvas->FlipBuffers();
		window_device->Flip(gpBitCanvas);
	}

	return 0;
}

#endif  /* CS4451a_project_3 */
/*****************************/


int PASCAL WINMAIN(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	error_t err = 0;
	//    int i;
	DWORD i;
	//    DWORD j;
	const value_t    pi = 3.14159265358979323846264338327950288419716939937510582097494f;
	value_t
		angularFrequency = 0.0625f,
		time = 0.0f;

	FILE* outFile;

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

#ifdef HIDE_TIMER
	HighResolutionTimer hrTimer;
	value_t the_time = 1.0f;
	//    value_t increment = 1.0f / (0xFFFFF + 1);
	long increment = 7;
	long t_time = 2;
	long r = rand();
	long s = rand();
	//    float r = rand();
	//    float s = rand();


	hrTimer.Start();
	//    for( j = 0; j < 149147; j++ )
	//    for( j = 0; j < 0xFFFFFFFF; j++ )
	for (i = 0; i < 0xFFFFF; i++)
	{
		//        the_time += 2.0f;
		//        t_time++;

		//        the_time += increment;

		t_time += increment;

		//        the_time = i * increment;
		//        the_time = (i + 1.0f) * increment;

		//        t_time = r + (s-r) * t_time;  // integer multiply

		//        the_time = r + (s - r) * the_time; // float multiply

		//        the_time = r + (s - r) / the_time; // float divide

		//        t_time = r + (s-r) / t_time;       // integer divide
		//        t_time = r + (s-r) % t_time;       // integer mod

		//        the_time = r + fmod((s - r), the_time);  // fmod

	}
	the_time = hrTimer.Milliseconds();

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "time = ", the_time, "\n");
#endif

	return 0; // JRDV: NOTE: I'm not sure WHY I put this in v0.4.1b, other than I probably forgot that the "test harness" bailed out, but now it's fine in v0.4.2b with HIDE_TIMER
#endif

	/*****************************/

#ifdef CS4451a_project_3
	dConfig.SetValue("screen_depth", "32");
	dConfig.SetValue("canvas_depth", "32");
	dConfig.SetValue("buffer_width", "640");
	dConfig.SetValue("buffer_height", "480");
	dConfig.SetValue("canvas_width", "400");
	dConfig.SetValue("canvas_height", "400");
	dConfig.SetValue("canvas_aspect", "1");
	dConfig.SetValue("window_width", "640");
	dConfig.SetValue("window_height", "480");
	dConfig.SetValue("screen_width", "640");
	dConfig.SetValue("screen_height", "480");
	dConfig.SetValue("show_debug_info", "0");
	dConfig.SetValue("show_framerate", "0");
	dConfig.SetValue("fullscreen", "0");
	dConfig.SetValue("maximum_frames", "22");
	dConfig.SetValue("default_frames", "22");
	dConfig.SetValue("tween_repeat", "4");
	dConfig.SetValue("plain_repeat", "1");
	dConfig.SetValue("overlay_mode", "0");
	dConfig.SetValue("overlay_color_key", "0");

	WindowDevice::New(hInstance, &window_device, "CS4451_a1 - Ray Tracer", &dConfig, NULL, NULL, NULL);
	BitCanvas::New(&bit_canvas, &dConfig, NULL, NULL);

	//    DDSURFACEDESC2 *ddsd;
	//    DWORD *surface;
	//    DWORD width, height, format;


	//    DumpToFile("error.txt", "blah?!asd: ", read_file( fopen( "d:\\cs\\cs4451a\\cs4451a\\testdata", "r" ) ) );
	vpd = 400;
	init_scene(fopen("d:\\cs\\cs4451a\\cs4451a\\testdata", "r"));
	//    read_file( stdin );

	//    DumpToFile( "error.txt", "vpd: ", vpd );
	//    DumpToFile( "error.txt", "numSpheres: ", numSpheres );



	while (window_device != NULL
		&& window_device->IsActive())
	{
		for (y = 0; y < vpd; y++) {
			for (x = 0; x < vpd; x++) {
				shade_pixel(&fr, &fg, &fb, x, y);
				r = (DWORD)(fr * 255.0);
				g = (DWORD)(fg * 255.0);
				b = (DWORD)(fb * 255.0);

				bit_canvas->BlahPutPixel(x, vpd - y - 1, r, g, b);
				//              bit_canvas->BlahPutPixel(x, y, r, g, b);
			}
		}


		bit_canvas->FlipBuffers();
		window_device->Flip(bit_canvas);
	}

	return 0;
#endif // #ifdef CS4451a_project_3




	err = gGreenshift.Initialize(hInstance);
	if (err != SUCCESS)
	{
		outFile = fopen("error.txt", "a");
		fprintf(outFile, "%s%s", ErrorString(err), " from Main.cpp in greenshift.Initialize()\n");
		fclose(outFile);
		return err;
	}

	while ((err = gGreenshift.Render()) == SUCCESS)
	{
		/*
		 * horribly fake the wave data :P
		 */
		 //        for( channel = 0; channel < this_mod->nCh; channel++ )
		{
			for (i = 0; i < SOUND_DATA_LENGTH; i++)
			{
				//                gSpectrumData[0][i] = (unsigned char)127;
				//                gWaveformData[0x][i] = (unsigned char)0;
				//                gSpectrumData[0][i] = (unsigned char)RANDOM(255);
				//                gWaveformData[0][i] = (unsigned char)RANDOM(255);
					//            gSpectrumData[0][i] = (unsigned char)(127.0f * cos( (i/4.244 - time) / 8.0f ) + 127.0f);
				cSpectrumData[0][i] = (signed char)(127.0f * cos((i / 4.244 - time) / 8.0f) + 127.0f);
				//                gWaveformData[0][i] = (unsigned char)(127.0f * cos( (i/4.244 - time) / 8.0f ) + 127.0f);
				/*                gWaveformData[0][i] = (unsigned char)(64+127.0f*GForce_clip(0.5f + (-1.0f/pi * sin(i/100.0f*time*8.0f)
																		-0.5f/pi * sin(i/100.0f*time*4.0f)
																		-0.25f/pi * sin(i/100.0f*time*2.0f))) );/**/
				cWaveformData[0][i] = (unsigned char)(64 + 64.0f * GForce_clip(0.5f + (-1.0f / pi * (value_t)sin(i / 100.0f * time * 8.0f)
					- 0.5f / pi * (value_t)sin(i / 100.0f * time * 4.0f)
					- 0.25f / pi * (value_t)sin(i / 100.0f * time * 2.0f))));/**/
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

		gGreenshift.SetSoundData(1, cWaveformData, 1, cSpectrumData);
		time += 1;
	}


	if (err > SUCCESS && err < ERR_UNDEFINED)
	{
		outFile = fopen("error.txt", "a");
		fprintf(outFile, "%s%s", ErrorString(err), " from Main.cpp in greenshift.Render()\n");
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
	BitCanvas* bitCanvas;
	MyDictionary<char*> dConfig;

	dConfig.SetValue("display_width", "640");
	dConfig.SetValue("display_height", "480");
	dConfig.SetValue("visible_width", "640");
	dConfig.SetValue("visible_height", "480");
	dConfig.SetValue("fullscreen_depth", "8");
	dConfig.SetValue("calculated_depth", "8");

	if (BitCanvas::New(&bitCanvas, &dConfig, NULL, NULL) == SUCCESS)
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
	BitCanvas* bitCanvas;

	if (BitCanvas::New(640, 360, 32, &bitCanvas) == SUCCESS)
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

percentX.SetBounds(0, 1);
logicalX.SetBounds(-1.0, 1.0);
physicalX.SetBounds(0, 640);

Expression* test = NULL;

//    std::vector<void*>    vec;
	/*
	 * it won't compile "x" because I didn't define the variable x
	 */
if ((err = Expression::Compile("#('4'|'3'&'x')", &test)) == SUCCESS)
{
	int matchLength = 0;

	//        DumpToFile("testing something.txt", test->PrintString(), test->Evaluate(), "\n" );


			/* it's calling printstring! */
	//        DumpToFile("regexps.txt", test->Match( "3x4", &matchLength ) ? "true ":"false ", matchLength, "\n" );
	SAFE_DELETE(test);
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
