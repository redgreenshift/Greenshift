/****************************************************************************\
 *
 * Welcome to Greenshift v0.4b.  Copyright (c) 2001-2026 Jared Ivey
 *
\****************************************************************************/

This software is OSI Certified Open Source Software.
OSI Certified is a certification mark of the Open Source Initiative.

Greenshift is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; version 2 only.

Greenshift is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Greenshift; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/****************************************************************************/
/****************************************************************************/

Source code for Greenshift and updates are available at:

    https://github.com/redgreenshift/greenshift

If Greenshift is used for more than your own private use, feel free
to drop me an email and tell me about how Greenshift is being used.
I'm just curious, and like to hear from people who use my program.


/****************************************************************************/
/****************************************************************************/

A goal in writing Greenshift is to achieve the speed of Geiss without
resorting to assembly, and surpass the expandibility of G-Force without
copying code.  Oh, and the 4D functions... can't forget the 4D functions
as that is the original reason for writing Greenshift... I only got
sidetracked writing this Winamp visualization thingy, and in writing the
visualization came up with the name Greenshift.

/****************************************************************************/
/****************************************************************************/

0.  What are the hotkeys?!
1.  Greenshift looks an awful lot like G-Force,
    are you sure you didn't just copy it?
2.  I already have G-Force.  Why should I use Greenshift?
3.  What in the world is a 4D function?  That makes my head hurt!
4.  I think Winamp is a Windows only player, will there be a standalone
    version, or maybe a different plug-in for Mac compatible programs?
5.  Ok, so if you didn't copy G-Force, why did you write Greenshift?
6.  I thought you said you developed Greenshift in an attempt
    to outdo Geiss.  What happened to that?
7.  What?  No idea what a green shift is.



/****************************************************************************\
 *
 * FAQ
 *
\****************************************************************************/

0.  What are the hotkeys?!

D = toggle display text
F = toggle framerate display
Esc = close Greenshift
Alt+Enter = toggle fullscreen
double clicking inside the window also toggles fullscreen.
The maximize button maximizes the window, it does not make it fullscreen.
------

1.  Greenshift looks an awful lot like G-Force,
    are you sure you didn't just copy it?

Yes.  I'm sure.  Check out the source code, it's different.  The main reason they look so similar is that Greenshift can run G-Force configs.  Greenshift is an entirely separate entity, completely redesigned, and written from scratch.

Try dragging the window while Greenshift is running.  Notice the picture is still changing.  Try it with G-Force.  The picture stops while the window is dragged.

One more thing is that Greenshift maintains its current FPS while changing between delta fields.  Notice, G-Force slows down.

Greenshift is not G-Force.  G-Force is not Greenshift.
----------

2.  I already have G-Force.  Why should I use Greenshift?

Nothing says you can't use more than one visualization.
Personally, I use Greenshift, G-Force, and Geiss.

However, if you want some sort of comparison:

  Greenshift has 4D function graphing capabilities.

  G-Force v1.1.6 is open source.
  G-Force v2.0 is closed source.
  Greenshift is open source, and will *remain* open source.

  Greenshift supports hyperbolic trig functions in expressions.
  G-Force does not support hyperbolic trig.

  G-Force supports RGB and HSV palettes.
  Greenshift supports RGB, HSV, HLS, and CMY palettes.

  G-Force is available for the Mac, as well as Windows.
  Greenshift is (currently) only available for Windows.

  G-Force "has scripting services that allow you to repeat" performances.
  Greenshift does not support scripting yet.

  G-Force suports text, bitmap, and video particles/sprites.
  Greenshift doesn't yet.

  G-Force has been out longer, and has had more time to improve.
  Greenshift is relatively new, and is improving.
----------

3.  What in the world is a 4D function?  That makes my head hurt!

4D graphs are not as complicated as it sounds.  Really, I mean it.  If you understand the other configs, it should be easy to see how 4D works.

G-Force allows X and Y parametric equations.  I allow those, and X, Y, Z parametric... in effect a 3d line.  Also, Greenshift supports 4d functions.  If you know algebra, then I'm sure you're familiar with 2d functions like y(x)=x^2, and stuff like that.  4d is just y(x,z,t) where X and Z are axis in 3 dimensional space, and T is time.  Really the only difference between calculating 2d and 4d functions is that 4d has 3 parameters rather than 1.  The big difference is in plotting the function on screen.  Greenshift handles that part behind the scenes.
----------

4. I think Winamp is a Windows only player, will there be a standalone
   version, or maybe a different plug-in for Mac compatible programs?

Yup, it's only for the Windows platform right now.  I don't have a Mac, nor do I have access to one.  If someone would like to port it to the Mac, please contact me.
----------

5.  Ok, so if you didn't copy G-Force, why did you write Greenshift?

The *original* reason for developing Greenshift is to graph 4 dimensional functions, and I started coding it just before I found out about the existence of G-Force. I just got sidetracked on this visualization thing, and then integrated the 4D stuff into the visualization itself. :P

Last Summer (2000), I designed the 4D stuff in Squeak (the SmallTalk programming language).  Then, last Fall, decided to port it to C++ to make a screensaver.  And then thought about everything I would be writing to accomplish this, and realized it wouldn't take much more to make a Winamp visualization out of it.

In November (2000), I was looking for some winamp visualization source code so I could see how to go about doing that stuff, when I came across G-Force v1.1.6. I was shocked!  Someone had already done exactly what I was planning to do... write a Winamp vis that used expressions defined in external files... AND make it open source.  I almost dropped the project right then and there, since someone else had already done it, but I'm glad I didn't, because I have learned so much from making Greenshift.

The point is, I started designing Greenshift before I knew G-Force existed.  So it's more than just a recode... Andy and I had the same idea independently.  I have been designing Greenshift for 4 years, and coded it in the last 6 months.
----------

6.  I thought you said you developed Greenshift in an attempt
    to outdo Geiss.  What happened to that?

Yes, that is also true.  Ever since I saw Geiss 3+ years ago, I've been driven to somehow outdo it.  I don't think it's possible to beat that speed when using external configs the way G-Force and Greenshift do.  I'm certain that stuff is hardcoded into the source code of Geiss.  However, I think G-Force blows away Geiss on the expandability issue with the externally defined configs.

This is how I planned to outdo Geiss, with the symbolic manipulation stuff, using expressions defined in external files.  I never posted anything directly about Greenshift until it was pretty much done because I hate it when people hype something up way too early and then it doesn't live up to their claims.
----------

7.  What?  No idea what a green shift is.

It's what happens when you do all calculation in 16 bit color.  Greenshift was originally developed in 16 bit color, because that's what my desktop was set to, and I couldn't figure out how palettes worked.

Ok, to explain the 16 bit thing... try dividing 16 by 3, for the RGB components.  You can't, right?!  Well, 5 bits are for red, 5 for blue, and 6 for green.  When you average the color components of 4 adjacent pixels (anti-aliasing) and one of the components has one more bit than the others, that component tends to have less of a roundoff error, and therefore stays brighter slightly longer.  Thus a greenshift occurs.  All of the colors tend to shift toward the green "end" of the spectrum!

I was programming late at night, frustrated with this "greenshift" effect, and couldn't figure out why it was occuring, and then the 5-6-5 bit thing hit me.  I was also frustrated because I couldn't come up with a good name.  Nothing sounded right.  I had tons of comments all over my code complaining about this weird greenshift effect and then started thinking, "ya know, I like that name" and it's been called that since.

If you run greenshift in 16 bit color, I mean the calculated bit depth, then you'll see what I'm talking about.
----------

/****************************************************************************\
 *
 * For Future Development
 *
\****************************************************************************/

 - Particle tweening
 - expression symbolic derivatives
 - some sort of scripting
 - 2D to 4D wave shape tweening
 - 4D to 2D wave shape tweening
 - 4D to 4D wave shape tweening



/****************************************************************************\
 *
 * Known Issues
 *
\****************************************************************************/

 - mouse sometimes stays hidden even after moved inside the window
 - particles and waveshapes ignore aspect, and always use 1:1
 - overlay mode is only known to work on Voodoo3 cards
 - overlay mode only works in 16 bit color
 - Vers variable is ignored
 - Flow or DispFactor variable is ignored
 - some config comments are NOT commented out, and so may not work properly
      example:  Juliaish
 - evaluation of A#-D# vars for palettes may not be entirely correct.
 - line/dot drawing are not 100% correct, and could use more optimization
 - FFTT calculation is not correct.
 - BASS is also not correct, because it's based on the FFTT data.




/****************************************************************************\
 *
 * Revision History
 *
\****************************************************************************/

Final:
------
2001 / ?? / ?? - ??:??
v1.0
  - Hopefully I'll release the Final version soon.


Beta:
------
2001 / 07 / ?? - ??:??
v0.4b
  - Added: delta field aspect tweening
  - Fixed: A[] vars are no longer re-evaluated for each instance
      affected:  Miri's Ball, and Funky Caterpillar (among others)
  - Added: EdgeWrap is now used, and defaults to false
      defaults to clipping at the edge of the screen
  - Change: doubled the size of the "recent list" that is used to keep
       the same configs from being used over and over


2001 / 06 / 09 - 22:00
v0.3.1b
 - Fixed: DeltaField prerender thread is faster,
       hopefully this solves the periodic slowdown problem.
 - Fixed: Using a different pseudo-random number generator,
       so I don't have to seed the standard one every single frame.
       Configs still use the standard rand() generated numbers.
 - Fixed: a defect where the calculated bit depth could be assigned
       a bad bit depth value, if set incorrectly in the settings file.


Alpha:
------
2001 / 06 / 08 - 14:51
v0.3a
 - Added copyright stuff, that should have been in the initial release
 - Moved display text closer to edge of screen so height of 380 can be used.
 - Added time dependent palettes
 - Fixed palette 254 color restriction.
      Now all 256 colors are used.
      Black and white are not forced as entries 0 and 255 respectively.
 - Fixed HSV palettes.  affected palettes like Acid Gap.
 - Added HLS and CMY palettes
 - Fixed issues with delta fields:
      evaluation of A#-D# vars
      what r and theta mean when used in cartesian delta fields
      what x and y mean when used in polar delta fields
 - slightly sped up delta field rendering
      (you probably won't even notice a difference)


2001 / 05 / 31 - 21:03
v0.2a
 - Fixed a G-Force config compatibility problem
 - Fixed NUM_PARTICLES to actually represent what it is supposed to


2001 / 05 / 29 - 08:21
v0.1a debug 
 - Added TONS of debug log file stuff


2001 / 05 / 28 - 21:11
v0.1a
 - Fixed a couple things
 - Converted tabs to spaces in source code.


2001 / 05 / 27 - 00:44 
v0.0a
 - Initial ALPHA release


Pre-Alpha:
----------
2001-05-26-19:05  v5.5 pre-alpha
2001-05-25-17:04  v5.4 pre-alpha - Bresenham's algorithm for line drawing
2001-05-25-11:07  v5.3 pre-alpha - added wave shape tweening
2001-05-24-22:55  v5.2 pre-alpha
2001-05-24-20:07  v5.1 pre-alpha
                  v5.0 never released, thread deadlock problems

 - *Massive* code restructure between 4.3pa and 5.0pa

2001-05-21-14:24  v4.3 pre-alpha
2001-05-20-20:13  v4.2 pre-alpha
                  v4.1 never released, severe crashing problems
2001-05-20-19:49  v4.0 pre-alpha
2001-05-19-19:06  v3.0 pre-alpha
2001-05-18-17:42  v2.0 pre-alpha
2001-05-17-22:22  v1.0 pre-alpha
2001-05-16-23:00  v0.0 pre-alpha  - Initial PRE-ALPHA release


Pre-Release:
------------
2001-05-12-14:13  prototype 4d functions working
2001-05-09-20:40  hooked to Winamp for the first time


Contact Info:
-------------

  Jared Ivey

  https://github.com/redgreenshift/greenshift

