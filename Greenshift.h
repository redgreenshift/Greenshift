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
#define VIS_VER "v0.4.2b"

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
	void                        Config(void);
	error_t                     Initialize(HINSTANCE hInstance);
	error_t                     Render(void);
	void                        Shutdown(void);


	void                        SetParentWindow(HWND hWindow);
	error_t                     SetSoundData(const DWORD nWfCh,
		sound_data_t pWaveform,
		const DWORD nSpCh,
		sound_data_t pSpectrum);
	//    inline
	value_t                     GetWaveformData(const value_t nValue);
	//    inline
	value_t                     GetSpectrumData(const value_t nValue);


protected:
	error_t                     LoadConfigs(void);

	inline error_t              RegulateFramerate(void);

	inline DWORD                NumDeltaFields(void) { return m_dwNumDeltaFields; };
	inline DWORD                NumWaveShapes(void) { return m_dwNumWaveShapes; };
	inline DWORD                NumParticles(void) { return m_dwNumParticles; };
	inline DWORD                NumPalettes(void) { return m_dwNumPalettes; };

	void                        UpdateDisplayText(void);
	error_t                     DisplayString(const int position,
		const char* strName,
		const char* string);



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
	WindowDevice* m_pWindowDevice;
	BitCanvas* m_pBitCanvas;

	EXPRESSIONDESCRIPTION* m_pGlobals;

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
	MyDictionary<value_t*>        m_dValues;
	value_t* m_pValues = nullptr;


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

	MyDictionary<char*>* m_pDeltaFieldConfigs;
	MyDictionary<char*>* m_pWaveShapeConfigs;
	MyDictionary<char*>* m_pParticleConfigs;
	MyDictionary<char*>* m_pPaletteConfigs;

	Expression* m_pFFTTransform;

	value_t                     m_nRawFFTValue;
	value_t                     m_nFrequency;


	value_t                     m_nFramesPerSecond;
	value_t                     m_nSecondsPerFrame;
	value_t                     m_nDesiredFramerate;
	value_t                     m_nMinimumFramerate;

	DWORD                       m_dwDesiredMSecondsPerFrame;
	DWORD                       m_dwMaximumMSecondsPerFrame;

};


#endif  /* _Greenshift_H_ */

