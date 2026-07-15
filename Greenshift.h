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
 * Greenshift - the visualization class
 *
 ****************************************************************************/

#ifndef _Greenshift_H_
#define _Greenshift_H_

#include "HighResolutionTimer.h"
#include "GForceFunctions.h"
#include "WindowDeviceDX.h"
#include "FilingClerk.h"
#include "Expression.h"
#include "BitCanvas.h"
#include "MyDictionary.h"
#include "MetaConfig.h"
#include "MetaDeltaField.h"
#include "MetaWaveShape.h"
#include "MetaParticle.h"
#include "MetaPalette.h"

#include "MersenneTwister.h"  /* pseudo-random number generator */

#define VIS_TITLE "Greenshift"
#define VIS_VER "v0.4b"

#if EXTREME_DEBUGGING
#define VIS_VERSION VIS_VER " - DEBUG VERSION"
#else
#define VIS_VERSION VIS_VER
#endif

#ifndef SOUND_DATA_LENGTH
#define SOUND_DATA_LENGTH 576
#endif

typedef unsigned char sound_data_t[2][SOUND_DATA_LENGTH];

/****************************************************************************
 *
 * Greenshift
 *
 * the visualization class
 *
 ****************************************************************************/
class Greenshift
{
public:
                                Greenshift();  /* default constructor */
                                ~Greenshift(); /* default destructor */

    /*
     * the interface to the outside world
     */
    void                        Config( void );
    error_t                     Initialize( HINSTANCE hInstance );
    error_t                     Render( void );
    void                        Shutdown( void );


    void                        SetParentWindow( HWND hWindow );
    error_t                     SetSoundData( const DWORD nWfCh,
                                              sound_data_t pWaveform,
                                              const DWORD nSpCh,
                                              sound_data_t pSpectrum );
//    inline
    value_t                     GetWaveformData( const value_t nValue );
//    inline
    value_t                     GetSpectrumData( const value_t nValue );


protected:
    error_t                     LoadConfigs( void );

    inline error_t              RegulateFramerate( void );

    inline DWORD                NumDeltaFields( void )    { return m_dwNumDeltaFields; };
    inline DWORD                NumWaveShapes( void )    { return m_dwNumWaveShapes;  };
    inline DWORD                NumParticles( void )    { return m_dwNumParticles;   };
    inline DWORD                NumPalettes( void )        { return m_dwNumPalettes;    };

    void                        UpdateDisplayText( void );
    error_t                     DisplayString( const int position,
                                               const char *strName,
                                               const char *string );



protected:
    MTRand                      m_mtRand;

    /* ?!  I have two forms?!  I should remove one! */
    unsigned char               m_cWaveformData[2][SOUND_DATA_LENGTH];
    unsigned char               m_cSpectrumData[2][SOUND_DATA_LENGTH];
    value_t                     m_nWaveformData[2][SOUND_DATA_LENGTH];
    value_t                     m_nSpectrumData[2][SOUND_DATA_LENGTH];
    HINSTANCE                   m_hInstance;    /* reference to module instance */
    HWND                        m_hParentWindow;
    FilingClerk                 m_fcFilingClerk;
    WindowDevice                *m_pWindowDevice;
    BitCanvas                   *m_pBitCanvas;

    EXPRESSIONDESCRIPTION       *m_pGlobals;

    /* MetaConfigs */
    MetaDeltaField              m_dfMetaDeltaField;
    MetaWaveShape               m_wsMetaWaveShape;
    MetaParticle                m_mpMetaParticle;
    MetaPalette                 m_mpMetaPalette;


    MyDictionary<char*>           m_dMainConfig; /* settings found in the main config file */
    MyDictionary<MyDictionary<char*>*>
                                m_dExtraConfig;
    MyDictionary<EXPRESSIONDESCRIPTION*>    /* variables and functions allowed */
                                m_dGlobals; /* in all Greenshift expressions */
    MyDictionary<char*>           m_dAlias;
    MyDictionary<char*>           m_dConstant;
    MyDictionary<char*>           m_dDefaults;
    MyDictionary<char*>           m_dFunction;


    HighResolutionTimer         m_hrFramerateTimer;    /* timer used in framerate regulation */
    HighResolutionTimer         m_hrDisplayTimer;    /* time since last info display */
//    HighResolutionTimer            m_hrSeedTimer;        /* time since last random seed */

    /*
     * named variables accessable from any expression
     */
    value_t                     m_nBASS;                /* BASS */

    DWORD                       m_dwNumDeltaFields;        /* size of the deltafield array */
    DWORD                       m_dwNumWaveShapes;        /* size of the waveshape array */
    DWORD                       m_dwNumParticles;        /* size of the particle array */
    DWORD                       m_dwNumPalettes;        /* size of the palette array */

    MyDictionary<char*>           *m_pDeltaFieldConfigs;
    MyDictionary<char*>           *m_pWaveShapeConfigs;
    MyDictionary<char*>           *m_pParticleConfigs;
    MyDictionary<char*>           *m_pPaletteConfigs;

    Expression                  *m_pFFTTransform;

    value_t                     m_nRawFFTValue;
    value_t                     m_nFrequency;


    value_t                     m_nFramesPerSecond;
    value_t                     m_nSecondsPerFrame;
    value_t                     m_nDesiredFramerate;
    value_t                     m_nMinimumFramerate;

    DWORD                       m_dwDesiredMSecondsPerFrame;
    DWORD                       m_dwMaximumMSecondsPerFrame;

};



#ifdef UNDEFINED_DEFAULT_CONFIG

/****************************************************************************
 * Welcome to Greenshift.  Copyright (c) 2001, Jared Ivey
 *
 * This is the main configuration settings file.
 *
 * Greenshift is provided as is, no warranty either expressed or implied.
 * Use at your own risk.  I cannot be held liable for anything bad that
 * happens due to the use of Greenshift.  Nothing bad should happen, but
 * it hasn't been fully tested on many machines.
 *
 *
 * You may download the source code free of charge and use it for any purpose
 * you like as long as the source code remains free.  However, you may not
 * sell anything based on my source code, using my source, or the like,
 * without my written permission.
 *
 * If the source code is used for purposes other than personal curiosity
 * (ie: just exploring the source code to see how it works),
 * please contact me first.
 *
 * If any modifications/additions are made to the code (other than personal
 * twiddling with the source for fun), please send a copy to me.
 *
 * Greenshift is open source, and freeware, for non-commercial / educational
 * use. Explicit permission is required for any other use.  It's not that I
 * want to restrict use of Greenshift, quite the opposite, I just would like
 * to know who uses it and what they're using it for.
 *
 *
 *
 *
 * Sorry about this file being a mess... I'm not finished writing Greenshift
 * and as such I haven't decided exactly what the config file will contain
 * so a lot of the ideas I'm playing with are included so I remember to
 * work on them when I finish whatever I'm currently working on.
 *
 * A goal in writing Greenshift is to achieve the speed of Geiss without
 * resorting to assembly, and surpass the expandibility of G-Force without
 * copying code.  Oh, and the 4D functions... can't forget the 4D functions
 * as that is the original reason for writing Greenshift... I only got
 * sidetracked writing this Winamp visualization thingy, and in writing the
 * visualization came up with the name Greenshift.
 *
 * https://github.com/redgreenshift/greenshift
 *
 ****************************************************************************/

GForcePath = "G-Force Files", /* specifies which subdirectory to look in for
                               * G-Force configs.  Greenshift is compatible
                               * with all G-Force configs, and I thought it
                               * would be wrong to distribute parts of
                               * G-Force along with Greenshift.  Get G-Force
                               * from http://www.55ware.com/
                               *
                               * Although compatible with G-Force configs,
                               * Greenshift is ENTIRELY my creation.  I
                               * designed it from the ground up without
                               * copying any code.  Greenshift is now
                               * open source (like G-Force) since I have
                               * completed most of the base functionality.
                               *
                               * If you do install G-Force, be sure to copy
                               * "Wave" in the "plugins\Greenshift" directory
                               * into the "plugins\G-Force Files\WaveShapes"
                               * directory.  It has an error, and Greenshift
                               * will not run unless this error is fixed.
                               *
                               */

/****************************************************************************
 * NOTE: you may be confused why some numbers have quotes around them, and
 * others don't.  There really is no reason, other than Greenshift supports
 * both, and I wasn't consistent in the way I typed the numbers.
 ****************************************************************************/



  /* "fullscreen_depth" - valid values include 8, 16, 24, 32
   *
   * this is dependent on your hardware, so if your video card doesn't support
   * 32 bit color, don't use it.  Greenshift should automatically try 24 bit
   * color if 32 bit color fails.  Actually, 24 bit color looks exactly the
   * same since both represent 16 Million colors.  32 bit color also includes
   * an 8 bit alpha channel, and since alpha is not used in Greenshift, 32 and
   * 24 bit are pretty much identical, except 24 bit sometimes displays faster
   * since one less byte needs to be copied for each pixel
   *
   * "calculated_depth" - valid values include 8, 16, 32
   *
   * the visualization data is rendered and manipulated in this bit depth.
   * this is NOT hardware dependent, and does not have to be the same as the
   * fullscreen depth, so any value should work on any system.
   */

       /*
        * the visible dimensions must be even,
        * because some code assumes a multiple of 2
        * if an odd height is used, the image will be backwards
        */
display_width  = "640", /* width of the window or fullscreen */
display_height = "480", /* height of the window or fullscreen */
visible_width  = "640", /* width of calculated area, ignored if greater than display_width */
//visible_height = "480", /* height of calculated area, ignored if greater than display_height */
//visible_height = "360", /* height of calculated area, ignored if greater than display_height */
visible_height = "300", /* height of calculated area, ignored if greater than display_height */
//visible_height = "192", /* height of calculated area, ignored if greater than display_height */
fullscreen_depth = "8", /* display mode when in fullscreen */
calculated_depth = "8", /* number of bits to render the vis */


max_framerate  = "33",  /* desired sustained framerate */
min_framerate  = "10",  /* if the framerate drops below min_framerate,
                         * Greenshift takes measures to increase fps instantly
                         * (ie: reduce the number of particles)
                         */
/*
 * the algorithm goes like this:
 * if framerate is too low
 * then
 *     if the current waveshape took longer to draw than all the particles combined
 *     then
 *         set a new waveshape immediately, without tweening from the current
 *     else
 *         remove the slowest particle
 */

show_framerate  = "1", /* F key toggles at runtime  */
show_debug_info = "1", /* D key toggles at runtime */

fullscreen      = "0", /* 0 = start windowed,
                        * 1 = fullscreen mode
                        *
                        * Alt+Enter, or double clicking toggles at runtime
                        * toggling at runtime does not change this setting
                        */



/****************************************************************************
 * Timing and Tweening Settings
 ****************************************************************************/

max_frames      = "16", /* maximum num frames used to tween delta fields  */
default_frames  = "16", /* default num frames used to tween delta fields  */
tween_repeat    = "4",  /* number of times to repeat each tween frame.    */
plain_repeat    = "1",  /* number of times to repeat each of the normal
                         * delta field frames.                            */

                       /* Setting tween_repeat to 2 is almost like setting
                        * default_frames twice as high,
                        * setting to 4 is like 4 times as high... etc..
                        * except precalculation doesn't take twice as long
                        */
                       /* Time dependent delta fields aren't supported yet
                        * so plain_repeat has no effect (leave it at 1)
                        */

   /*
    * the default G-Force timing expressions seem
    * to do a good job, so I borrowed some of them
    *
    * PPrb=".04/((NUM_PARTICLES+1)^1.55)",
    * PDur=" 6 + rnd( 18 )",
    * WInt="15 + rnd( 15 )",
    * CInt="10 + rnd( 20 )",
    * DInt="20 + rnd( 20 )",
    * WMor="6 + rnd( 13 )",
    * CMor="3 + rnd( 18 )",
    * DMor="3 + rnd( 7 )",
    */

FFTT="0.5 * log( 0.00390625 * x * ( 1.3 + w ) + 1 )",
/*FFTT="1.1 * log( 0.0078125 * x * ( 1.3 + w ) + 1 )", /* used to adjust the
                                                  * spectrum data so it has
                                                  * "prettier" values
                                                  *
                                                  * it works, but is not
                                                  * implemented correctly yet
                                                  */


       /* a random number between 0 and 1 is picked every
        * "particle_interval" seconds, if it is less than
        * "particle_probability", then a new particle is started
        */
particle_probability = ".09/((NUM_PARTICLES+1)^1.55)",

deltafield_interval  = "20 + rnd( 20 )",
waveshape_interval   = "13 + rnd( 11 )",
particle_lifetime    = "10 + rnd( 32 )",
particle_interval    = "1",     /* time between particle probability checks */
palette_interval     = "10 + rnd( 20 )",
                          /* <blah>_interval specifies the number of seconds
                           * between changes of the respective "things"
                           */
deltafield_tween     = "3 + rnd( 7 )",  /* seconds to morph delta fields */
waveshape_tween      = "6 + rnd( 13 )", /* seconds to morph waveshapes   */
palette_tween        = "3 + rnd( 18 )", /* seconds to morph palettes     */
palette_tween_repeat_value = "4", /* specifies how many frames to wait between
                                   * each change in the palette while tweening
                                    */
                              /* currently palette_tween is the only
                               * tween duration expression used.
                               * WaveShapes do not tween yet, and
                               * DeltaFields tween in a specified
                               * number of frames, not an amount of time
                               */


/****************************************************************************
 * Use Overlay mode at your own risk.  I have only seen it work on my machine.
 * Testing on other people's machines has either simply failed, and reverted
 * to non-overlay mode, or bluescreened.
 *
 * There is almost zero error checking, and could possibly bluescreen very
 * easily, depending on if your video card likes overlays
 *
 * the worst it should do is bluescreen
 ****************************************************************************/
overlay_mode     = "0", /* 0 = don't use overlay */
                        /* 1 = start in overlay mode */
                        /* double right click toggles at runtime */
overlay_color_key = "0",  /* the color the overlay replaces */
                        /* suggestion: set desktop background to this color,
                         * maximize the window, and then minimize the window
                         */


/****************************************************************************
 *
 * This is the end of the main settings section,
 * and the beginning of the special settings section
 *
 ****************************************************************************/


[alias]
  /* an alias is a way to map one string onto another
   * (say a less descriptive name to a more descriptive one)
   * G-Force uses a lot of hard to remember abbreviations.
   * to make my life easier, I include the ability to map the
   * 4 character or less G-Force abbreviations to my set of variable
   * names which may have any length without affecting speed
   */
/*
 * internal alias's - for things to the left of an equals sign in a config
 *
 * used by my code internally
 * to convert from what I look for to the G-Force variable names
 * the value to the right of the = is what's in G-Force configs
 */
Version         = Vers,
Steps           = Stps,
Aspect          = Aspc,

/*
 * external alias's - for things to the right of an equals sign in a config
 *
 * used by expressions in configs to convert from
 * G-Force functions/variables, to the actual function/variable names I use
 * the value to the left of the = is what's in G-Force configs
 */
NUM_SAMPLE_BINS = Default_samples,
NUM_FFT_BINS    = Default_FFT_Samples,
flor            = floor,
trnc            = trunc,
rnd             = rand,



[constant]
  /* a constant is kindof like an alias, except it represents a
   * constant value.  If a constant is defined with the same name
   * as a config variable constant, then the value will be used if
   * the variable is not defined in the config file.
   */
Default_FFT_Samples= 93,
Default_Samples    = 260,
Version            = 100,
Aspect             = 1,
false              = 0,
true               = 1,
pi                 = 3.1415926535897932384626433832795,
e                  = 2.71828182845904523536,
radians_per_degree = "pi / 180",
Parametric_steps   = Default_Samples,
FourD_steps        = "flor(sqrt(Parametric_steps))-1",

        /* FourD_steps is for both x AND z directions,
         * so that's why the default should be close to
         * the square root of the default number of steps
         *
         * There is a little bit of extra overhead for 4D
         * so that's why an extra 1 is subtracted
         */

/* Parametric mode is the default (all G-Force files are Parametric)
 * and s ranges from 0 to 1.   In FourD mode, you use two other variables
 * x and z, which range from -1 to 1.  I should probably provide a function
 * to convert the -1 to 1 range to the 0 to 1 range, but for now s will
 * be used to represent a number from 0 to 1 in FourD mode, where
 * s equals distance from origin
 *
 * wait, so s should *really* be s = sqrt(sqr(x)+sqr(y)+sqr(z))
 * but for now I'll leave it as s = sqrt(sqr(x) + sqr(z))
 *
 * I'm thinking of adding a few variables like rx, ry, rz, to have access
 * to the rotated values.  I'm not sure yet, these are just ideas right now.
 */
        /*
         * when in FourD mode, the s variable is defined as
         * the pythagorean distance from (0,0) to (x,z)
         * so, s = sqrt(sqr(x)+sqr(z))
         */


[defaults]
 /*
  * these are the defaults for variable expressions in configs
  */
POV                = "5",
rotation_order     = "yxz",
x_rotation         = "-0.125*pi", //"-pi/8",
y_rotation         = "0.25*pi", //"pi/4",
z_rotation         = "0",



/****************************************************************************
 *
 * Everything else below in the file is ignored
 *
 * I mean it's all read in, it's just not used yet.
 *
 * this file just contains ideas I had about
 * what I might put in a config file, and
 * the file format is not finished, so this config
 * probably will not work with any other builds of Greenshift.
 *
 ****************************************************************************/



[function]

rad2deg(r) = "r / radians_per_degree",
deg2rad(d) = "d * radians_per_degree",


[default]
virtual_aspect_ratio = 1.0,
edge_wrap            = true,
displacement_factor  = 1.0,


[version]
Vers = 100


#endif




#endif  /* _Greenshift_H_ */

