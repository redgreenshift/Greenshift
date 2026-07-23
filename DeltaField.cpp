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
  * DeltaField
  *
  ****************************************************************************/

#include "DeltaField.h"


  /****************************************************************************
   *
   * DeltaField - initialize all members to something
   *
   ****************************************************************************/
DeltaField::DeltaField()
{
	m_bEdgeWrap = false;
	m_bIsPolar = false;
	m_pSource1 = NULL;
	m_pSource2 = NULL;
	m_nX = 0.0f;
	m_nY = 0.0f;
	m_nAspect = 0.0f;
	m_nRadius = 0.0f;
	m_nTheta = 0.0f;

	/*
	 * Why does it not work unless I put this here...
	 * and why am I not checking and dumping to error file when it is returned?
	 */
	m_dictValues.SetValue("x", &m_nX);
	m_dictValues.SetValue("y", &m_nY);
	m_dictValues.SetValue("r", &m_nRadius);
	m_dictValues.SetValue("theta", &m_nTheta);

	/*
	 * Should r be a separate variable?  I see it used in some delta fields
	 * like "Sonic Boom" and "Twist of Fate".
	 * Could "r" mean sqrt(x*x + y*y)  ?
	 */
}


/****************************************************************************
 *
 * ~DeltaField - release any allocated memory
 *
 ****************************************************************************/
DeltaField::~DeltaField()
{
	SAFE_DELETE(m_pSource1);
	SAFE_DELETE(m_pSource2);
}


#ifndef UNDEFINED
/****************************************************************************
 *
 * GetDeltaXY -
 *
 ****************************************************************************/
void    DeltaField::GetDeltaXY(const value_t destX,
	const value_t destY,
	value_t* deltaX,
	value_t* deltaY)
{
	value_t nSource1;
	value_t nSource2;

	if (m_pSource1 != NULL && m_pSource2 != NULL)
	{
		m_nX = destX;
		m_nY = destY;
		m_nRadius = (value_t)_hypot(destX, destY);
		m_nTheta = (value_t)atan2(destY, destX);

		m_pfValues.EvaluatePhase(2);
		m_pfValues.EvaluatePhase(3);

		nSource1 = m_pSource1->Evaluate();
		nSource2 = m_pSource2->Evaluate();

		if (m_bIsPolar)
		{
			value_t nTempSource1;

			nTempSource1 = nSource1 * (value_t)cos(nSource2);
			nSource2 = nSource1 * (value_t)sin(nSource2);
			nSource1 = nTempSource1;
		}

		*deltaX = nSource1 - destX;
		*deltaY = nSource2 - destY;
	}
}
#endif



/****************************************************************************
 *
 * SetDelta -
 *
 ****************************************************************************/
error_t    DeltaField::Initialize(MyDictionary<char*>* inDeltaConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	error_t     err = SUCCESS;
	value_t     deltaVersion;
	//    value_t     deltaAspect;
	value_t     dispFactor;
	char* strSource1 = NULL;
	char* strSource2 = NULL;



	deltaVersion = (value_t)atof(inDeltaConfig->GetValue("Version", "100")) / 100.0f;
	m_bEdgeWrap =
		Expression::Evaluate(inDeltaConfig->GetValue("EdgeWrap", "0"), 0.0f, NULL, inGlobals)
		? false : true;
	//    m_bEdgeWrap  = (atof(inDeltaConfig->GetValue( "EdgeWrap", "0" )) == 0) ? false : true;
	m_bZoom =
		(Expression::Evaluate(inDeltaConfig->GetValue("Zoom", "0"), 0.0f, NULL, inGlobals) == 0.0f)
		? false : true;

	/*
	 * get the displacement factor, default to 1
	 */
	dispFactor = Expression::Evaluate(
		inDeltaConfig->GetValue("DispFactor", "1.0"),
		1.0f,
		NULL,
		inGlobals);

	/*
	 * get the aspect ratio, default to 1, account for both G-Force and Greenshift naming schemes
	 */
	m_nAspect = Expression::Evaluate(
		inDeltaConfig->GetValue("Aspect", "-1.0"),
		-1.0f,
		NULL,
		inGlobals);
	if (m_nAspect < 0.0f)
		m_nAspect = 1.0f;



	if ((strSource1 = inDeltaConfig->GetValue("SourceX")) != NULL
		&& (strSource2 = inDeltaConfig->GetValue("SourceY")) != NULL)
	{
		m_bIsPolar = false;
	}
	else
		if ((strSource1 = inDeltaConfig->GetValue("SourceRadius")) != NULL
			&& (strSource2 = inDeltaConfig->GetValue("SourceTheta")) != NULL)
		{
			m_bIsPolar = true;
		}
		else
		{
			err = ERR_NOTDELTAFIELD;
			return ERR_NOTDELTAFIELD;
		}

	if (err == SUCCESS)
		err = m_pfValues.Initialize("ABCD",
			"",  /* DeltaF's don't have multiple XY vars,
				  * but PhaseFunction needs something */
			inDeltaConfig,
			&m_dictValues,
			inGlobals);

#if EXTREME_DEBUGGING
	if (err != SUCCESS)
	{
		DumpToFile("error.txt", ErrorString(err), "\n");  /* REMOVE THIS */
	}
#endif
	if (err != SUCCESS)
		return err;

	m_pfValues.EvaluatePhase(0);
	m_pfValues.EvaluatePhase(1); /* this is wrong, but I'm not sure how to tell the DF how many frames have elapsed */


	err = Expression::Compile(strSource1,
		&m_pSource1,
		&m_dictValues,
		inGlobals);
	if (err == SUCCESS)
		err = Expression::Compile(strSource2,
			&m_pSource2,
			&m_dictValues,
			inGlobals);

	return err;
}

