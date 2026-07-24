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
  * VectorGraphic - GraphicalEntity composed of lines and shapes
  *
  ****************************************************************************/

#ifndef _VectorGraphic_H_
#define _VectorGraphic_H_

  //#include "Graph.h"
#include "Expression.h"
#include "MyDictionary.h"
#include "PhaseFunction.h"
#include "PointRotation.h"



enum { VG_PARAMETRIC, VG_4D };

/****************************************************************************
 *
 * VectorGraphic - base for dynamic objects composed of points and lines
 *
 ****************************************************************************/
class VectorGraphic
{
public:
	VectorGraphic();
	virtual             ~VectorGraphic();

	virtual error_t     Initialize(MyDictionary<mychar_t*>* inConfig,
		MyDictionary<value_t*>* inValues,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals);

	//    virtual void    DrawFrameOn( Graph * lpGraph );

	void                Reset(void);  /* reevaluate the A vars */


	DWORD       GetNumFunctions(void) { return m_pfValues.NumFunctions(); };
	DWORD       GetNumSteps(void) { return m_dwResolution; };
	DWORD       GetMode(void) { return m_dwMode; };
	void        BVars(void) { Evaluate_B_Vars(); };
	bool        GetConnected(void) { return m_bConnected; };
	void        BeginFrame(void) { EvaluateRotation(); };
	inline void         Get(const DWORD dwFunction,
		value_t* outX,
		value_t* outY,
		value_t* outLineWidth,
		value_t* outColor)
	{
		value_t nX;
		value_t nY;
		value_t nZ;

		switch (m_pfValues.NumDimensions())
		{
		case 2:
			EvaluateXY_Pair(dwFunction % m_pfValues.NumFunctions(), outX, outY);
			break;
		case 3:
			EvaluateXYZ_Tuple(dwFunction % m_pfValues.NumFunctions(), &nX, &nY, &nZ);
			m_pr3dRotator.RotatePoint(nX, nY, nZ, &nX, &nY, &nZ);
			ConvertTo2d(nX, nY, nZ, outX, outY);
			break;
		default:
			break;
		}

		*outLineWidth = m_pLineWidth->Evaluate();
		if (*outLineWidth > 32.0f)
			*outLineWidth = 32.0f;
		else /* should I really allow negative line width... as a way to not draw the line */
			if (*outLineWidth < 0.5f)
				*outLineWidth = 0.5f;
		*outColor = m_pPen->Evaluate();
	};
	inline void         Get4d(const DWORD dwFunction,
		value_t  inX,
		value_t  inZ,
		value_t* outX,
		value_t* outY,
		value_t* outLineWidth,
		value_t* outColor)
	{
		value_t nX;
		value_t nY;
		value_t nZ;

		nY = m_pfValues.EvaluateFunction(dwFunction % m_pfValues.NumFunctions(), 0);
		m_pr3dRotator.RotatePoint(inX, nY, inZ, &nX, &nY, &nZ);
		ConvertTo2d(nX, nY, nZ, outX, outY);

		*outLineWidth = m_pLineWidth->Evaluate();
		if (*outLineWidth > 32.0f)
			*outLineWidth = 32.0f;
		else /* should I really allow negative line width... as a way to not draw the line */
			if (*outLineWidth < 0.5f)
				*outLineWidth = 0.5f;
		*outColor = m_pPen->Evaluate();
	};

	void                GetLastBuffers(value_t** outLastX, value_t** outLastY)
	{
		*outLastX = m_pLastX;
		*outLastY = m_pLastY;
	};


protected:
	inline void         ConvertTo2d(const value_t x,
		const value_t y,
		const value_t z,
		value_t* outX,
		value_t* outY)
	{
		const value_t nDepthPerceptionFactor
			= m_nPointOfView / (m_nPointOfView - z);

		*outX = x * nDepthPerceptionFactor;
		*outY = y * nDepthPerceptionFactor;
	};

	void                EvaluateRotation(void);

protected:
	bool                    m_bConnected;
	MyDictionary<value_t*>    m_dValues;

	//    value_t                 m_nStep;        /* the s var */
	value_t                 m_nResolution;  /* NUM_S_STEPS var */
	DWORD                   m_dwResolution;

	DWORD                   m_dwMode;

	//    value_t                 m_nX;
	//    value_t                 m_nZ;
	value_t* m_pLastX;
	value_t* m_pLastY;


	PointRotation           m_pr3dRotator;
	value_t                 m_nPointOfView;

	Expression* m_pAngleX;
	Expression* m_pAngleY;
	Expression* m_pAngleZ;
	char* m_strRotationOrder;

	value_t                 m_nScale;

	inline error_t          Evaluate_A_Vars(void) { return m_pfValues.EvaluatePhase(0); };
	inline error_t          Evaluate_B_Vars(void) { return m_pfValues.EvaluatePhase(1); };
	inline error_t          Evaluate_C_Vars(void) { return m_pfValues.EvaluatePhase(2); };
	inline error_t          Evaluate_D_Vars(void) { return m_pfValues.EvaluatePhase(3); };
	void                    EvaluateXY_Pair(const DWORD dwIndex,
		value_t* outX,
		value_t* outY);
	void                    EvaluateXYZ_Tuple(const DWORD dwIndex,
		value_t* outX,
		value_t* outY,
		value_t* outZ);

	friend error_t  ImportPhase(VectorGraphic& DestVG, VectorGraphic& SourceVG, const DWORD dwPhase)
	{
		return ImportPhase(DestVG.m_pfValues, SourceVG.m_pfValues, dwPhase);
	};

	Expression* m_pPen;
	Expression* m_pLineWidth;

private:
	PhaseFunction           m_pfValues;
};


#endif  /* _VectorGraphic_H_ */

