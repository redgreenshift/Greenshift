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
  * DeltaField - parametric equation for pixel transition (the brief comment)
  *
  ****************************************************************************/

#ifndef _DeltaField_H_
#define _DeltaField_H_

#include "Expression.h"
#include "PhaseFunction.h"


  /****************************************************************************
   *
   * DeltaField
   *
   ****************************************************************************/
class DeltaField
{
public:
	DeltaField();
	~DeltaField();

	void        GetDeltaXY(const value_t destX,
		const value_t destY,
		value_t* deltaX,
		value_t* deltaY);
	value_t     GetAspect(void) { return m_nAspect; };
	bool        GetEdgeWrap(void) { return m_bEdgeWrap; };  /* should probably be bounds type in case I want other types other than clip and wrap */
	bool        GetZoom(void) { return m_bZoom; };

	error_t     Initialize(MyDictionary<char*>* inDeltaConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals);

#if EXTREME_DEBUGGING
	void DebugDump(const char* strFile)
	{
		DumpToFile(strFile, (m_bIsPolar ? "polar" : "cartesian"), "\n");
		DumpToFile(strFile, m_pSource1->PrintString(), "\n");
		DumpToFile(strFile, m_pSource2->PrintString(), "\n");

		/*        m_pfValues.DebugDump();*/
	};
#endif

private:
	value_t                 m_nX;
	value_t                 m_nY;
	value_t                 m_nRadius;
	value_t                 m_nTheta;
	value_t                 m_nAspect;

	Expression* m_pSource1 = nullptr;
	Expression* m_pSource2 = nullptr;

	PhaseFunction           m_pfValues;
	MyDictionary<value_t*>  m_dictValues;

	bool                    m_bIsPolar = false;
	bool                    m_bEdgeWrap = false;
	bool                    m_bZoom = false;
};


#endif  /* _DeltaField_H_ */

