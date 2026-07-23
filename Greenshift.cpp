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
  * Greenshift
  *
  ****************************************************************************/

#include "Greenshift.h"

#include "frontend.h"
#include <time.h>


  //#include <stdlib.h>

  /*

  #include <stdlib.h>

  char* initstate( unsigned int seed,
				   char* state,
				   size_t size );

  random();

	*/

	/*
	 * Greenshift is the central entity that manages/coordinates/delegates the vis
	 *
	 * It shouldn't DO too much.  All it should really do is create objects and
	 * coordinate/facilitate their communication.
	 */


	 /****************************************************************************
	  *
	  * Greenshift - constructor
	  *
	  ****************************************************************************/
Greenshift::Greenshift()
{
	//    long lRand;

	//    initstate();

	//    lRand = random();

	m_pGlobals = NULL;
	m_hInstance = NULL;
	m_hParentWindow = NULL;
	m_pWindowDevice = NULL;
	m_pBitCanvas = NULL;
	m_pValues = NULL;

	m_nSecondsPerFrame = 0.0f;    /* initialize to something */
	m_nFramesPerSecond = 1000.0f;    /* keep code from releasing all
									 * particles the first time through */

	m_nBASS = 1.0f;
	//    m_nNumParticles      = 0.0f;

	m_dwNumDeltaFields = 0;
	m_dwNumWaveShapes = 0;
	m_dwNumParticles = 0;
	m_dwNumPalettes = 0;


	m_pDeltaFieldConfigs = NULL;
	m_pWaveShapeConfigs = NULL;
	m_pParticleConfigs = NULL;
	m_pPaletteConfigs = NULL;


	m_pFFTTransform = NULL;
	m_nRawFFTValue = 0.0f;
	m_nFrequency = 0.0f;

	m_nDesiredFramerate = 0.0f;
	m_nMinimumFramerate = 0.0f;

	m_dwDesiredMSecondsPerFrame = 0;
	m_dwMaximumMSecondsPerFrame = 0;
}




/****************************************************************************
 *
 * ~Greenshift - destructor
 *
 ****************************************************************************/
Greenshift::~Greenshift()
{
	Shutdown();
}

/****************************************************************************
 *
 * Config - open the configuration editor
 *
 ****************************************************************************/
void    Greenshift::SetParentWindow(HWND hWindow)
{
	m_hParentWindow = hWindow;
}


/****************************************************************************
 *
 * Config - open the configuration editor
 *
 ****************************************************************************/
void    Greenshift::Config(void)
{
	/* sorry, no graphical editor yet... */
}


/****************************************************************************
 *
 * SetSoundData - sets the waveform and spectrum data
 *
 ****************************************************************************/
error_t Greenshift::SetSoundData(const DWORD nWfCh, sound_data_t pWaveform,
	const DWORD nSpCh, sound_data_t pSpectrum)
{
	DWORD    i;
	DWORD    dwSum;
#define NUM_SUM 32

	for (i = 0; i < SOUND_DATA_LENGTH; i++)
	{
		if (nWfCh > 0)
		{
			m_cWaveformData[0][i] = (unsigned char)pWaveform[0][i];
			if (nWfCh > 1)
				m_cWaveformData[1][i] = (unsigned char)pWaveform[1][i];
		}
		if (nSpCh > 0)
		{
#ifdef PRECALC_SPECTRUM
			m_nSpectrumData[0][i] =
#else
			m_cSpectrumData[0][i] = (unsigned char)pSpectrum[0][i];
			if (nSpCh > 1)
				m_cSpectrumData[1][i] = (unsigned char)pSpectrum[1][i];
#endif
		}
	}

	//    DumpToFile( "BASSlog.txt", "start -- \n", "\n" );
	dwSum = 0;
	for (i = 0; i < NUM_SUM; i++)
	{
		//        DumpToFile( "BASSlog.txt", m_cSpectrumData[0][i], "\n" );
		dwSum += m_cSpectrumData[0][i];
	}
	//    DumpToFile( "BASSlog.txt", "stop --\n", "\n" );

	//    m_nBASS = GetSpectrumData(0.1f); /* fake the BASS variable for now */
	m_nBASS = dwSum / (value_t)(NUM_SUM * 128);

	if (m_nBASS > 1.0f)
		m_nBASS = 1.0f;

	//    DumpToFile( "BASSlog.txt", m_nBASS, "\n" );

	return SUCCESS;
}

/*
FFTT="1.1 * log( 0.008 * x * ( 1.3 + w ) + 1 )", /* used to adjust the
												  * spectrum data so it has
												  * "prettier" values
												  *
												  * it works, but is not
												  * implemented correctly yet
The variable W is the frequency,
X is the raw fft value for that W,
and the whole expression will be the new value of X.
		  */

		  /****************************************************************************
		   *
		   * GetSpectrumData - The variable W is the frequency,
		   *                    X is the raw fft value for that W,
		   *                    and the whole expression will be the new value of X.
		   *
		   ****************************************************************************/
value_t    Greenshift::GetSpectrumData(const value_t nValue)
{
	value_t      FNum = 93.0f;
	value_t      FRge = 2.0f;
	value_t      FSrt = 14.0f;
	value_t      nFrequency = nValue * FRge * FNum + FSrt;
	DWORD        nIndex = (DWORD)(GForce_clip(nValue) * FNum);

	if (nIndex >= SOUND_DATA_LENGTH)
		nIndex = SOUND_DATA_LENGTH - 1;

	m_nFrequency = nFrequency;
	m_nRawFFTValue = (unsigned char)m_cSpectrumData[0][nIndex];
	return m_pFFTTransform->Evaluate();
}


/****************************************************************************
 *
 * GetWaveformData - gets the Waveform Data
 *
 ****************************************************************************/
value_t Greenshift::GetWaveformData(const value_t nValue)
{
	DWORD nIndex = (int)(GForce_clip(nValue) * SOUND_DATA_LENGTH);
	signed char scAmplitude;

	if (nIndex >= SOUND_DATA_LENGTH)
		nIndex = SOUND_DATA_LENGTH - 1;


	scAmplitude = (signed char)m_cWaveformData[0][nIndex];

	return (scAmplitude > 0) ? (scAmplitude / 128.0f) : (scAmplitude / 127.0f);
}


#if EXTREME_DEBUGGING

DWORD getWindowBitDepth(void) {
	DWORD    bpp;
	HDC      hdc;
	// retrieves device-specific information about 
	// the specified device. 

	// Get the windows device context
	hdc = GetDC(NULL);
	// Get the number of adjacent color bits for each pixel.
	bpp = GetDeviceCaps(hdc, BITSPIXEL);
	// Release the Device Context
	ReleaseDC(NULL, hdc);
	return bpp;


}

#endif


/****************************************************************************
 *
 * Initialize - initializes Greenshift
 *
 ****************************************************************************/
error_t    Greenshift::Initialize(HINSTANCE hInstance)
{
	static EXPRESSIONDESCRIPTION    edGForceFunctions[] =
	{
		{ ED_FUNCTION,  "pos",        1, GForce_pos  }, /* pos(x)  == x if x >= 0 and 0 otherwise */
		{ ED_FUNCTION,  "abs",        1, GForce_abs  }, /* abs(x)  == |x| */
		//{ ED_FUNCTION,  "sqr",        1, GForce_sqr  }, /* sqr(x)  == x * x */
		{ ED_FUNCTION,  "sgn",        1, GForce_sgn  }, /* sgn(x)  == 1 if x >= 0 and -1 if x < 0 */
		{ ED_FUNCTION,  "rand",       1, GForce_rnd  }, /* rnd(x)  == a random real number from 0 to x */
		{ ED_FUNCTION,  "clip",       1, GForce_clip }, /* clip(x) == 0 when x < 0, 1 when x > 1, and x when  0 <= x <= 1 */
		//{ ED_FUNCTION,  "trnc",       1, GForce_trnc }, /* trnc(x) == x with everything right of the decimal point dropped */
		{ ED_FUNCTION,  "trunc",      1, GForce_trnc }, /* trnc(x) == x with everything right of the decimal point dropped */
		{ ED_FUNCTION,  "floor",      1, GForce_flor }, /* flor(x) == the largest integer that's also less than x (ex, flor(3.2) == 3, flor(-2.7) == -3 ) */
		//{ ED_FUNCTION,  "flor",       1, GForce_flor }, /* flor(x) == the largest integer that's also less than x (ex, flor(3.2) == 3, flor(-2.7) == -3 ) */
		{ ED_FUNCTION,  "wrap",       1, GForce_wrap }, /* wrap(x) == x - flor( x )  (ex: wrap( .3 ) = .3, wrap( 4.12 ) = .12, wrap( - 2.7 ) = .3 ) */
		{ ED_FUNCTION,  "sqwv",       1, GForce_sqwv }, /* sqwv(x) == 1 if |x| <= 1.0 and 0 otherwise. */
		{ ED_FUNCTION,  "trwv",       1, GForce_trwv }, /* trwv(x) == Triangle shape that passes thru (0,0), (1,1), (2,0) and repeats in both directions */
		{ ED_FUNCTION,  "seed",       1, GForce_seed }, /* seed(x) seeds the random number generator based on the bits of x, returns x */

		{ ED_FUNCTIONEX,"mag",        1, GForce_mag, this }, /* waveshape data */
		{ ED_FUNCTIONEX,"fft",        1, GForce_fft, this }, /* sound data */

		{ ED_VARIABLE,  "BASS",       0, &m_nBASS },


		{ ED_VARIABLE,  "FPS",        0, &m_nFramesPerSecond }, /* the frames per second */
		{ ED_VARIABLE,  "x",          0, &m_nRawFFTValue }, /* for FFTT */
		{ ED_VARIABLE,  "w",          0, &m_nFrequency }, /* for FFTT */
		{ ED_CONSTANT,  "Parametric", VG_PARAMETRIC, NULL },
		{ ED_CONSTANT,  "FourD",      VG_4D, NULL },
		{ ED_CONSTANT,  "default_aspect", 640.0f / 380.0f, NULL },


		{ ED_NULL,NULL, 0, NULL }, /* the NULL terminator */

	};

	error_t    err;
	//error_t err2;
	DWORD    i;

#ifdef FORMAT_OF_LOGFILE_HEADER

	/************************************************************/
	* Greenshift v0.4.2b - DEBUG VERSION Thu Jun 28 09:01 : 12 2001
	*
	*Desktop bit depth : 32
	* Winamp version : 2.06
	/************************************************************/

#endif

#if EXTREME_DEBUGGING
//    int version = SendMessage(m_hParentWindow,WM_WA_IPC,0,IPC_GETVERSION);

DumpToFile("error.txt", "Entering Greenshift::Initialize()", "\n");
	DumpToFile("error.txt", "/************************************************************/", "\n");
	DumpToFile("error.txt", " * " VIS_TITLE " " VIS_VERSION " " __TIMESTAMP__, "\n");
	DumpToFile("error.txt", " *", "\n");
	DumpToFile("error.txt", " * Desktop bit depth: ", getWindowBitDepth(), "\n");

	if (m_hParentWindow != NULL)
	{
		char strVersion[16];
		int winampVersion;
		int vMajor;
		int vMinor;
		int vLeast;

		winampVersion = SendMessage(m_hParentWindow, WM_USER, 0, 0);

		vMajor = (winampVersion >> 12) & 0xF;
		if (vMajor == 1)
		{
			vMinor = (winampVersion >> 4) & 0xF;
			vLeast = (winampVersion) & 0xF;
		}
		else
		{
			vMinor = (winampVersion >> 8) & 0xF;
			vLeast = (winampVersion) & 0xF;
		}

		sprintf(strVersion, "%d.%d%d\n", vMajor, vMinor, vLeast);

		DumpToFile("error.txt", " * Winamp version: ", strVersion);
		//        DumpToFile( "error.txt", " * Winamp version: ", vMajor, "" );
		//        DumpToFile( "error.txt", ".", vMinor, "" );
		//        DumpToFile( "error.txt", ".", vMinor, "" );
	}


	DumpToFile("error.txt", "/************************************************************/", "\n");

	//    __FILE__  __LINE__
#endif


	for (i = 0; i < SOUND_DATA_LENGTH; i++)
	{
		m_cSpectrumData[0][i] = 0;
		m_cSpectrumData[1][i] = 0;
		m_cWaveformData[0][i] = 0;
		m_cWaveformData[1][i] = 0;
		m_nSpectrumData[0][i] = 0.0f;
		m_nSpectrumData[1][i] = 0.0f;
		m_nWaveformData[0][i] = 0.0f;
		m_nWaveformData[1][i] = 0.0f;
	}

	m_hInstance = hInstance;


	/************************************************************************
	 *
	 * Load Global MyDictionary with GForce functions to maintain compatibility
	 *
	 ************************************************************************/
	for (i = 0; edGForceFunctions[i].edtType != ED_NULL; i++)
	{
		err = m_dGlobals.SetValue(edGForceFunctions[i].strName,
			&edGForceFunctions[i]);
		if (err != SUCCESS)
			return err;
	}

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "About to LoadConfigs.", "\n");
#endif
	if ((err = LoadConfigs()) != SUCCESS)
	{
		return err;
	}
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "### Configs successfully loaded.", "\n");
#endif


#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "-=- Start Compiling DeltaFields.", "\n");
#endif
	err = m_dfMetaDeltaField.Initialize(m_pDeltaFieldConfigs,
		m_dwNumDeltaFields,
		&m_dMainConfig, &m_dGlobals);
	if (err != SUCCESS)
		return err;
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "### DeltaFields successfully compiled.", "\n");
	DumpToFile("error.txt", "-=- Start Compiling WaveShapes.", "\n");
#endif

	err = m_wsMetaWaveShape.Initialize(m_pWaveShapeConfigs,
		m_dwNumWaveShapes,
		&m_dMainConfig, &m_dGlobals);
	if (err != SUCCESS)
		return err;
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "### WaveShapes successfully compiled.", "\n");
	DumpToFile("error.txt", "-=- Start Compiling Particles.", "\n");
#endif

	err = m_mpMetaParticle.Initialize(m_pParticleConfigs, m_dwNumParticles,
		&m_dMainConfig, &m_dGlobals);
	if (err != SUCCESS)
		return err;
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "### Successfully Compiled Particles.", "\n");
	DumpToFile("error.txt", "-=- Start Compiling Palettes.", "\n");
#endif

	err = m_mpMetaPalette.Initialize(m_pPaletteConfigs, m_dwNumPalettes,
		&m_dMainConfig, &m_dGlobals);


	/*#if EXTREME_DEBUGGING
		if( err != SUCCESS )
		{
			DumpToFile( "error.txt", "-=- BEGIN DICTIONARY DUMP -=-", "\n" );
			this->DebugDumpContents( "error.txt" );
			DumpToFile( "error.txt", "-=- END DICTIONARY DUMP -=-", "\n" );
		}
	#endif/**/

	if (err != SUCCESS)
		return err;

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "### Successfully Compiled Palettes.", "\n");
#endif


	/*
	 * Should Query Display Modes
	 */
	 /*
	 DEVMODE        dmDisplaySettings;

	 ZeroMemory( &dmDisplaySettings, sizeof( dmDisplaySettings ) );
	 dmDisplaySettings.dmSize = sizeof( dmDisplaySettings );
	 dmDisplaySettings.dmSpecVersion = DM_SPECVERSION;
	 dmDisplaySettings.dmBitsPerPel = 16;
	 dmDisplaySettings.dmPelsWidth = 640;
	 dmDisplaySettings.dmPelsHeight = 480;

	 ChangeDisplaySettings( NULL, 0 );
	 ChangeDisplaySettings( NULL, CDS_TEST );
	 /**/

	{
		Association<char*>** pMainConfig;
		DWORD   dwSize = 0;

		if ((err = m_dMainConfig.AsArray(&pMainConfig, &dwSize)) == SUCCESS
			&& (m_pValues = new value_t[dwSize]) != NULL)
		{
			for (i = 0; i < dwSize; i++)
			{
				char* variable = pMainConfig[i]->GetKey();
				char* expression = pMainConfig[i]->GetValue();
//                    value_t *value   = (value_t*)malloc(sizeof(*value));

//                    if(value==NULL)
//                        return ERR_MALLOC;

					/* this is where the NUM_PARTICLES isn't found while evaluating */
//                    *value = Expression::Evaluate(expression,
				m_pValues[i] = Expression::Evaluate(expression,
					(value_t)atol(expression), &m_dValues, &m_dGlobals);

				err = m_dValues.SetValue(variable, &m_pValues[i]);
				if (err != SUCCESS)
					return err;
			}
		}
	}




#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "#################### About to create the WindowDevice", "\n");
#endif

	/*
	 * Create the WindowDevice
	 */
	err = WindowDevice::New(m_hInstance, &m_pWindowDevice,
		VIS_TITLE " " VIS_VERSION ", by Jared Ivey",
		&m_dMainConfig, &m_dGlobals, &m_dValues, m_hParentWindow);

	if (err != SUCCESS)
	{
		m_pWindowDevice = NULL;
		return err;
	}
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Successfully created the WindowDevice.", "\n");
	DumpToFile("error.txt", "About to create the BitCanvas.", "\n");
#endif

	/*
	 * Create the BitCanvas
	 */
	err = BitCanvas::New(&m_pBitCanvas, &m_dMainConfig, &m_dGlobals, &m_dValues);

	if (err != SUCCESS)
	{
		m_pBitCanvas = NULL;
		return err;
	}
#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Successfully created the BitCanvas.", "\n");
#endif


	//delta doesn't concern itself with edge wrapping
	//however, it *would* be in the delta field file
	//deltaFieldData.SetValue( "edgeWrap", "1" );

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Leaving Greenshift::Initialize()", "\n");
#endif


	return SUCCESS;
}


/****************************************************************************
 *
 * LoadConfigs - must not crash if config file doesn't exist
 *
 ****************************************************************************/
error_t    Greenshift::LoadConfigs(void)
{
	error_t err = SUCCESS;
	DWORD   i;
	DWORD   dwSize;
	Association<char*>** pArray = NULL;
	char    strModulePath[MAX_PATH];
	char    strRoot[MAX_PATH];
	char*   strGForce = NULL;
	int     index;


#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Entering Greenshift::LoadConfigs()", "\n");
#endif

	/*
	 * Load the main config file
	 */

	if (m_hInstance == NULL
		|| GetModuleFileName(m_hInstance, strModulePath, MAX_PATH) == 0)
	{
		//        return FAILURE;
		if (strcpy_s(strModulePath, _countof(strModulePath), "\0") != 0)
			return FAILURE;
	}

	index = strlen(strModulePath);

	while (index > 0 && strModulePath[--index] != '\\')
	{
		strModulePath[index] = '\0';
	}

	if (   strcpy_s(strRoot, _countof(strRoot), strModulePath) != 0
		|| strcat_s(strRoot, _countof(strRoot), "Greenshift") != 0)
	{
		return FAILURE;
	}



	/*
	 * Initialize the MyDictionary for the extra data
	 */
	if ((err = m_dExtraConfig.SetValue("[alias]", &m_dAlias)) != SUCCESS
		|| (err = m_dExtraConfig.SetValue("[constant]", &m_dConstant)) != SUCCESS
		|| (err = m_dExtraConfig.SetValue("[defaults]", &m_dDefaults)) != SUCCESS
		|| (err = m_dExtraConfig.SetValue("[function]", &m_dFunction)) != SUCCESS
		)
		return err;


	if ((err = m_fcFilingClerk.SetRoot(strRoot)) == SUCCESS
		&& (err = m_fcFilingClerk.SetFolder("")) == SUCCESS
		&& (err = m_fcFilingClerk.GetData("settings.txt",
			&m_dMainConfig,
			&m_dExtraConfig
		)) == SUCCESS
		&& (strGForce = m_dMainConfig.GetValue("GForcePath")) != NULL

		/*************************
		 * load Greenshift configs
		 *************************/

		 /** Load Delta Fields **/
		&& (err = m_fcFilingClerk.SetFolder("DeltaFields")) == SUCCESS
		&& (err = m_fcFilingClerk.LoadConfig(&m_pDeltaFieldConfigs,
			&m_dwNumDeltaFields)) == SUCCESS
		/** Load WaveShapes **/
		&& (err = m_fcFilingClerk.SetFolder("WaveShapes")) == SUCCESS
		&& (err = m_fcFilingClerk.LoadConfig(&m_pWaveShapeConfigs,
			&m_dwNumWaveShapes)) == SUCCESS
		/** Load Particles **/
		&& (err = m_fcFilingClerk.SetFolder("Particles")) == SUCCESS
		&& (err = m_fcFilingClerk.LoadConfig(&m_pParticleConfigs,
			&m_dwNumParticles)) == SUCCESS
		/** Load Palettes **/
		&& (err = m_fcFilingClerk.SetFolder("ColorMaps")) == SUCCESS
		&& (err = m_fcFilingClerk.LoadColorMaps(&m_pPaletteConfigs,
			&m_dwNumPalettes)) == SUCCESS
		)
	{
		/*
		 * load G-Force configs
		 */
		if (   strcpy_s(strRoot, _countof(strRoot), strModulePath) != 0
			|| strcat_s(strRoot, _countof(strRoot), strGForce) != 0)
		{
			return FAILURE;
		}

		if ((err = m_fcFilingClerk.SetRoot(strRoot)) == SUCCESS)
		{
			/* I don't care about the error return values,
			 * because the only way anything is changed is
			 * if SUCCESS is returned
			 *
			 * if an error occurs while loading GForce configs
			 * then they won't be added, and just the Greenshift
			 * configs will be used
			 */
			 /** Load Delta Fields **/
			err = m_fcFilingClerk.SetFolder("DeltaFields");
			err = m_fcFilingClerk.LoadConfig(&m_pDeltaFieldConfigs,
				&m_dwNumDeltaFields);
			/** Load WaveShapes **/
			err = m_fcFilingClerk.SetFolder("WaveShapes");
			err = m_fcFilingClerk.LoadConfig(&m_pWaveShapeConfigs,
				&m_dwNumWaveShapes);
			/** Load Particles **/
			err = m_fcFilingClerk.SetFolder("Particles");
			err = m_fcFilingClerk.LoadConfig(&m_pParticleConfigs,
				&m_dwNumParticles);
			/** Load Palettes **/
			err = m_fcFilingClerk.SetFolder("ColorMaps");
			err = m_fcFilingClerk.LoadColorMaps(&m_pPaletteConfigs,
				&m_dwNumPalettes);
		}
	}
	else
		return err;

#if EXTREME_DEBUGGING
#ifdef UNDEFINED
	m_dAlias.DebugDumpContents("error.txt");
	m_dConstant.DebugDumpContents("error.txt");
	m_dDefaults.DebugDumpContents("error.txt");
	m_dFunction.DebugDumpContents("error.txt");
	m_dMainConfig.DebugDumpContents("error.txt");
#endif
#endif

	m_dMainConfig.SetAlias(&m_dAlias);
	m_dGlobals.SetAlias(&m_dAlias);
	m_dMainConfig.SetAlternate(&m_dDefaults);


	/*
	 * they aren't given the alias!  ok, now they are... much better
	 */
	for (i = 0; i < NumDeltaFields(); i++)
	{
		m_pDeltaFieldConfigs[i].SetAlias(&m_dAlias);
		m_pDeltaFieldConfigs[i].SetAlternate(&m_dDefaults);
	}
	for (i = 0; i < NumWaveShapes(); i++)
	{
		m_pWaveShapeConfigs[i].SetAlias(&m_dAlias);
		m_pWaveShapeConfigs[i].SetAlternate(&m_dDefaults);
	}
	for (i = 0; i < NumParticles(); i++)
	{
		m_pParticleConfigs[i].SetAlias(&m_dAlias);
		m_pParticleConfigs[i].SetAlternate(&m_dDefaults);
	}
	for (i = 0; i < NumPalettes(); i++)
	{
		m_pPaletteConfigs[i].SetAlias(&m_dAlias);
		m_pPaletteConfigs[i].SetAlternate(&m_dDefaults);
	}

	err = m_dConstant.AsArray(&pArray, &dwSize);
	//    DumpToFile( "error.txt", "array size = ", dwSize, "\n" );
	if (err == SUCCESS)
	{
		m_pGlobals = new EXPRESSIONDESCRIPTION[dwSize];
		if (m_pGlobals == NULL)
			return ERR_MALLOC;

		for (i = 0; i < dwSize; i++)
		{
			m_pGlobals[i].edtType = ED_CONSTANT;
			m_pGlobals[i].strName = pArray[i]->GetKey();
			m_pGlobals[i].value = Expression::Evaluate(
				pArray[i]->GetValue(),
				0.0f, NULL, &m_dGlobals);
			m_pGlobals[i].pointer.generic_data = NULL;
			m_pGlobals[i].pExtraData = NULL;

			err = m_dGlobals.SetValue(pArray[i]->GetKey(), &m_pGlobals[i]);
			if (err != SUCCESS)
				return err;
		}

		delete[] pArray;
	}/**/

#if EXTREME_DEBUGGING
#ifndef HIDE_INIT_TRACE
	m_dGlobals.DebugDumpContents("error.txt");
#endif
#endif


	/*
	 * MUST have at least one of each config
	 */
	if (NumDeltaFields() < 1 || NumWaveShapes() < 1
		|| NumParticles() < 1 || NumPalettes() < 1)
	{
#if EXTREME_DEBUGGING
		DumpToFile("error.txt",
			"ERROR: Must have at least one of each type of config to run.\n");
#endif
		return FAILURE;
	}







	/*************************************
	 *
	 * Now Compile the Main Configs
	 *
	 *************************************/

	 /*
	  * Probability / Interval / Tween Time Variables
	  */
	if ((err = Expression::Compile(m_dMainConfig.GetValue("FFTT"),
		&m_pFFTTransform, NULL, &m_dGlobals)) != SUCCESS

		)
	{
		return err;
	}/**/






	m_nDesiredFramerate = Expression::Evaluate(
		m_dMainConfig.GetValue("max_framerate"), 33, NULL, NULL);
	m_nMinimumFramerate = Expression::Evaluate(
		m_dMainConfig.GetValue("min_framerate"), 17, NULL, NULL);


	if (m_nMinimumFramerate <= 0.0f)
		m_nMinimumFramerate = 0.001f;

	if (m_nDesiredFramerate < 0.0f
		|| m_nDesiredFramerate < m_nMinimumFramerate)
		m_nDesiredFramerate = m_nMinimumFramerate + 10.0f;

	m_dwDesiredMSecondsPerFrame = (DWORD)floor(1000.0f / m_nDesiredFramerate);
	m_dwMaximumMSecondsPerFrame = (DWORD)ceil(1000.0f / m_nMinimumFramerate);

	if (m_dwDesiredMSecondsPerFrame > 1)
		m_dwDesiredMSecondsPerFrame -= 2;


#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Leaving Greenshift::LoadConfigs()", "\n");
#endif

	return SUCCESS;
}




/****************************************************************************
 *
 * Render - the thing Winamp calls every waveframe update
 *
 ****************************************************************************/
error_t    Greenshift::Render(void)
{
	error_t err;
	if (m_pWindowDevice == NULL || !m_pWindowDevice->IsActive())
	{
		return ERR_DD_FAILURE; //  -1;
	}

	m_hrFramerateTimer.Start();  /* start timer, see how long drawing takes */

	err = m_dfMetaDeltaField.Update(m_pBitCanvas);
	if (err != SUCCESS)
		return err;
	//        m_pBitCanvas->FlipBuffers();
	err = m_wsMetaWaveShape.Update(m_pBitCanvas);
	if (err != SUCCESS)
		return err;
	err = m_mpMetaParticle.Update(m_pBitCanvas);
	if (err != SUCCESS)
		return err;
	//        m_pBitCanvas->FlipBuffers();
	//        err = m_dfMetaDeltaField.Update( m_pBitCanvas );
	//        if( err != SUCCESS )
	//            return err;
	err = m_mpMetaPalette.Update(m_pBitCanvas);
	if (err != SUCCESS)
		return err;
	/**/

	m_pBitCanvas->FlipBuffers();        /* swap internal buffers */

	/* Don't bother trying to update the window if it's switching modes */
	if (!m_pWindowDevice->IsSwitchingModes())
	{
		m_pWindowDevice->SetPalette(m_pBitCanvas->PGetPalette());
		m_pWindowDevice->Flip(m_pBitCanvas);
	}

	m_nSecondsPerFrame = m_hrFramerateTimer.Seconds(); /* stop the timer */

	m_nFramesPerSecond = 1.0f / m_nSecondsPerFrame;  /* update the FRAMES_PER_SECOND variable */


	//    UpdateDisplayText(); /* put it before so I can see what particles get removed */
		/* Don't worry about low framerates while switching modes */
	if (!m_pWindowDevice->IsSwitchingModes())
	{
		RegulateFramerate();
		UpdateDisplayText(); /* or should I put it after so I can see what waveshape is chosen instead? */
	}

	return SUCCESS;
}



/****************************************************************************
 *
 * RegulateFramerate - attempt to maintain a constant framerate
 *
 ****************************************************************************/
error_t    Greenshift::RegulateFramerate(void)
{
	const DWORD    nMSeconds = (DWORD)(1000.0f * m_nSecondsPerFrame);
	MetaConfig* pMetaConfig;
	value_t        nLongestTime;
	value_t        nTime;

	/*
	 * if the frame was drawn faster than we need, sleep for a bit
	 */
	if (nMSeconds < m_dwDesiredMSecondsPerFrame)
	{
		Sleep(m_dwDesiredMSecondsPerFrame - nMSeconds);
	}
	else/*
		 * if we're running a little slower than we'd like
		 */
		if (nMSeconds > m_dwMaximumMSecondsPerFrame)
		{
			m_pBitCanvas->FramerateWarning();

			/* ask the slowest meta config to reduce its time */
			nLongestTime = m_dfMetaDeltaField.GetUpdateTime();
			pMetaConfig = &m_dfMetaDeltaField;

			nTime = m_wsMetaWaveShape.GetUpdateTime();
			if (nTime > nLongestTime)
			{
				nLongestTime = nTime;
				pMetaConfig = &m_wsMetaWaveShape;
			}

			nTime = m_mpMetaParticle.GetUpdateTime();
			if (nTime > nLongestTime)
			{
				nLongestTime = nTime;
				pMetaConfig = &m_mpMetaParticle;
			}

			nTime = m_mpMetaPalette.GetUpdateTime();
			if (nTime > nLongestTime)
			{
				nLongestTime = nTime;
				pMetaConfig = &m_mpMetaPalette;
			}

			return pMetaConfig->ReduceTime();
		}
		else
			if (nMSeconds > ((m_dwMaximumMSecondsPerFrame + (m_dwDesiredMSecondsPerFrame << 2)) / 5))
				m_pBitCanvas->FramerateWarning();


	return SUCCESS;
}


/****************************************************************************
 *
 * UpdateDisplayText -
 *
 ****************************************************************************/
void    Greenshift::UpdateDisplayText(void)
{
	DWORD    i = 0;
	//    DWORD    dwDisplay;
	//    char    strParticle[16];
	char    strTitle[2048];
	char    strSearch[] = "- Winamp";
	//    LRESULT    lResult;

		/*
		 * check every 1/4th a second
		 */
	if (0.25f < m_hrDisplayTimer.Seconds())
	{
		//        lResult = SendMessage( m_hParentWindow,WM_USER, data, id);
		//        lResult = SendMessage( m_hParentWindow,WM_USER, NULL, IPC_GETPLAYLISTTITLE);
		if (m_hParentWindow != NULL)
			i = GetWindowText(m_hParentWindow, strTitle, sizeof(strTitle));
		else
			if (strcpy_s(strTitle, _countof(strTitle), "") != 0)
				return;


		if (i > 7)
		{
			//strTmp = &strTitle[ strlen(strTitle) - 8 ];
			i = strlen(strTitle) - 8;

			while (i > 0 && strnicmp(&strTitle[i], strSearch, sizeof(strSearch)) != 0)
				i--;

			while (i > 0 && strTitle[i - 1] == ' ')
				i--;

			strTitle[i] = '\0';

		}


		//*
		m_dfMetaDeltaField.Update(m_pWindowDevice);
		m_wsMetaWaveShape.Update(m_pWindowDevice);
		m_mpMetaParticle.Update(m_pWindowDevice);
		m_mpMetaPalette.Update(m_pWindowDevice);
		/**/

		DisplayString(0, "", strTitle);

		m_hrDisplayTimer.Start();
	}
}





/****************************************************************************
 *
 * DisplayString
 *
 ****************************************************************************/
error_t    Greenshift::DisplayString(const int position, const char* strName, const char* string)
{
	char strPrintString[256];
	int x;
	int y;

	if (strName != NULL && string != NULL && m_pWindowDevice != NULL)
	{
		if (strcpy_s(strPrintString, _countof(strPrintString), strName) != 0)
			return FAILURE;
		if (strlen(strName) > 0 && strcat_s(strPrintString, _countof(strPrintString), " : ") != 0)
			return FAILURE;
		if (strcat_s(strPrintString, _countof(strPrintString), string) != 0)
			return FAILURE;

		while (strlen(strPrintString) < 200)
		{
			if (strcat_s(strPrintString, _countof(strPrintString), " ") != 0)
				return FAILURE;
		}

		switch (position)
		{
		case 0:  /* deltafield */
			x = 60;
			y = 0;
			break;
		case 1:  /* waveshape */
			x = 0;
			y = 20;
			break;
		case 2: /* particle */
			x = 0;
			y = 40;
			break;
		case 3: /* palette */
			x = 350;
			y = 0;
			break;
		case 4: /* palette */
			x = 320;
			y = 20;
			break;
		case 5: /* palette */
			x = 320;
			y = 40;
			break;
/*        case 6: // unused
			x = 320;
			y = 40;
			break;
		case 7: // unused
			x = 320;
			y = 60;
			break;/**/
		case 8:  /* deltafield */
			x = 0;
			y = 420;
			break;
		case 9:  /* waveshape */
			x = 0;
			y = 440;
			break;
		case 10: /* particle */
			x = 0;
			y = 460;
			break;
		case 11: /* palette */
			x = 320;
			y = 420;
			break;
		case 12: /* palette */
			x = 320;
			y = 440;
			break;
		case 13: /* palette */
			x = 320;
			y = 460;
			break;
/*        case 6: // unused
			x = 320;
			y = 460;
			break;
		case 7: // unused
			x = 320;
			y = 460;
			break;/**/
		default:
			x = 0;
			y = 0;
			break;
		}

		return m_pWindowDevice->Print(x, y, strPrintString);
	}
	else
		return ERR_NULL;
}






/****************************************************************************
 *
 * Shutdown -
 *
 ****************************************************************************/
void    Greenshift::Shutdown(void)
{
	m_hInstance = NULL;

	SAFE_DELETE(m_pWindowDevice);
	SAFE_DELETE(m_pBitCanvas);

	SAFE_DELETE_ARRAY(m_pGlobals);

	SAFE_DELETE_ARRAY(m_pDeltaFieldConfigs);
	SAFE_DELETE_ARRAY(m_pWaveShapeConfigs);
	SAFE_DELETE_ARRAY(m_pParticleConfigs);
	SAFE_DELETE_ARRAY(m_pPaletteConfigs);


	SAFE_DELETE(m_pFFTTransform);

	m_dValues.WipeContents();
	SAFE_DELETE_ARRAY(m_pValues);
}



//    DWORD    window_bit_depth = getWindowBitDepth(); //do I need this?


	/*
	 * Partial Simplification isn't as useful in C++ because variable
	 * evaluation isn't that much slower than constant evaluation.
	 * In Squeak, the difference was much greater.
	 * hmmm... I wonder if a "number container" would speed up the Squeak code.
	 * In essence faking a number pointer.
	 * That would remove the parameter passing, and the variable check, AND
	 * would condense to a SINGLE evaluate statement... theoretically.
	 */



	 /*    DEVMODE        dmDisplaySettings;

		 ZeroMemory( &dmDisplaySettings, sizeof( dmDisplaySettings ) );
		 dmDisplaySettings.dmSize = sizeof( dmDisplaySettings );
		 dmDisplaySettings.dmSpecVersion = DM_SPECVERSION;
		 dmDisplaySettings.dmBitsPerPel = 16;
		 dmDisplaySettings.dmPelsWidth = 640;
		 dmDisplaySettings.dmPelsHeight = 480;





		 ChangeDisplaySettings( NULL, 0 );
		 ChangeDisplaySettings( NULL, CDS_TEST );
	 */


	 /*
	  * DeviceContexts overview
	  *
	  * BitBlt( windowDC, 0,0,screenWidth, screenHeight, bitcanvasDC, 0, 0, SRCCOPY)
	  * BitBlt(
			 windowDC,
			 (screenWidth - visibleWidth) / 2,
			 (screenHeight - visibleHeight) / 2,
			 visibleWidth,
			 visibleHeight,
			 bitcanvasDC,
			 0,
			 0,
			 SRCCOPY)
	  */

	  /*
	   * A better solution would probably be to try using that OpenGL thing
	   */

	   /*
		   DWORD getWindowBitDepth( void ) {
			   DWORD    bpp;
			   HDC        hdc;
		   // retrieves device-specific information about
		   // the specified device.

		   // Get the windows device context
		   hdc = GetDC(NULL);
		   // Get the number of adjacent color bits for each pixel.
		   bpp = GetDeviceCaps(hdc, BITSPIXEL);
		   // Release the Device Context
		   ReleaseDC(NULL, hdc);
			   return bpp;


		   }

	   */

