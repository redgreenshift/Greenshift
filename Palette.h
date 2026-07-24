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
  * Palette - abstracts palette creation/management
  *
  ****************************************************************************/

#ifndef _Palette_H_
#define _Palette_H_

#include "MyDictionary.h"
#include "Expression.h"  /* for HSV color */
#include "PhaseFunction.h"
#include "HighResolutionTimer.h"
#include <fstream>
#include <io.h>
#include <windows.h> /* for PALETTEENTRY */
#include <stdlib.h>  /* for atoi() */
  //#include <stdlib.h>
  //    ifstream myFile;

#define PALETTE_SIZE    256

enum {
	PALETTE_UNKNOWN = -1,
	PALETTE_STATIC_COLOR_MAP = 0,
	PALETTE_RGB,
	PALETTE_HSV,
	PALETTE_HLS,
	PALETTE_CMY,
	PALETTE_CMYK
};

class Palette
{
public:
	Palette();
	virtual     ~Palette();

	error_t     Initialize(Palette* palette);
	error_t     Initialize(Palette* p1, Palette* p2, const value_t nPercent);
	error_t     Initialize(MyDictionary<mychar_t*>* pConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* pUserDefined);

	void        SetPaletteEntry(const DWORD nIndex, const COLORREF crColor);
	void        SetPalette(value_t* nIndex, Expression* pHue,
		Expression* pSaturation,
		Expression* pValue);

	/* Does not re-evaluate the palette */
	inline LOGPALETTE* PGetLogicalPalette(void) { return &(GetPalette()); };
	LOGPALETTE& GetLogicalPalette(void);
	void          ResetTimer(void) /* restart the timer for time dependent palettes */
	{
		m_hrTimer.Start();
		m_nTime = 0.0f;
		m_pfValues.EvaluatePhase(0);
	};
protected:
	inline LOGPALETTE& GetPalette(void) { return m_union.LogicalPalette; };

	COLORREF    HSVToRGB(const value_t nnHue,
		const value_t nnSaturation,
		const value_t nnValue);
	COLORREF    HLSToRGB(const value_t nnHue,
		const value_t nnLightness,
		const value_t nnSaturation);

	void        Clip(value_t* nValue);
	BYTE        PercentToByte(const value_t nValue);

private:
	union {
		LOGPALETTE      LogicalPalette;
		BYTE            reserved[sizeof(LOGPALETTE) +
			(PALETTE_SIZE - 1) * sizeof(PALETTEENTRY)];
	} m_union;

	value_t                 m_nIntensity; /* 0.0 to 1.0 represent 0 to 255 */
	value_t                 m_nTime;      /* for animated palettes, range 0 to 1? */
	int                     m_nPaletteType;
	PhaseFunction           m_pfValues;
	MyDictionary<value_t*>    m_dValues;
	HighResolutionTimer     m_hrTimer;

	/* these three expressions are for each of the */
	/* components, whether it be                   */
	Expression* m_pExp1;   /* (RGB) Red-Green-Blue,                       */
	Expression* m_pExp2;   /* (HSV) Hue-Saturation-Value,                 */
	Expression* m_pExp3;   /* (HLS) Hue-Lightness-Saturation, or          */
	/* another representation not supported yet    */
};


#endif  /* _Palette_H_ */

