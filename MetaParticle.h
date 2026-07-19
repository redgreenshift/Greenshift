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
  * MetaParticle -
  *
  ****************************************************************************/

#ifndef _MetaParticle_H_
#define _MetaParticle_H_

#include "MetaConfig.h"
#include "Particle.h"
#include "PointRotation.h"


#define MAX_RUNNING_PARTICLES 4

  /*
   * MetaParticleAbstract provides the smarts for drawing and tweening
   */
   /*
	* MetaParticle and MetaWaveShape provide the smarts for _selection_
	*/

class MetaParticleAbstract : public MetaConfig
{
protected:
	value_t             m_nNumParticles;  /* NUM_PARTICLES  __RUNNING__ */
	value_t             m_nStep;          /* s */
	value_t             m_nX;             /* x */
	value_t             m_nZ;             /* z */

	DWORD               m_dwMode;
	DWORD               m_dwNumInstances;
	DWORD               m_dwNumFunctions;
	DWORD               m_dwNumSteps;

	//    value_t                m_nTime;
	value_t             m_nPercent;

	Particle* m_pCurrent;
	Particle* m_pNext;
	value_t* m_pLastX;
	value_t* m_pLastY;

	//    char                m_strDefaultAspect[32];

	bool                m_bTweening;
	bool                m_bConnected;

public:
	MetaParticleAbstract();
	virtual             ~MetaParticleAbstract();

protected:
	inline void         GetNums(DWORD* outInstances, DWORD* outFunctions, DWORD* outSteps)
	{
		const DWORD    dwInstances = m_pCurrent->GetNumInstances();
		const DWORD    dwFunctions = m_pCurrent->GetNumFunctions();
		const DWORD    dwSteps = m_pCurrent->GetNumSteps();
		DWORD    dwInstances2, dwFunctions2, dwSteps2;

		/* need to return each of the running particles */
	//    *outInstances = m_pCurrent->HackGetNumInstances();
	//    *outFunctions = m_pCurrent->HackGetNumFunctions();
	//    *outSteps     = m_pCurrent->HackGetNumSteps();
		m_bConnected = m_pCurrent->GetConnected();

		if (m_bTweening)
			//    {
			//        *outInstances = dwInstances;
			//        *outFunctions = dwFunctions;
			//        *outSteps     = dwSteps;
			//    }
			//    else
		{
			dwInstances2 = m_pNext->GetNumInstances();
			dwFunctions2 = m_pNext->GetNumFunctions();
			dwSteps2 = m_pNext->GetNumSteps();
			*outInstances = max(dwInstances, dwInstances2);
			*outFunctions = max(dwFunctions, dwFunctions2);
			//        *outSteps     = max( dwSteps, dwSteps2 );
			//        *outInstances = dwInstances * (1.0f - m_nPercent) + m_nPercent * dwInstances2;
			//        *outFunctions = dwFunctions * (1.0f - m_nPercent) + m_nPercent * dwFunctions2;
			*outSteps = LCLIP(dwSteps * (1.0f - m_nPercent) + m_nPercent * dwSteps2);
			if (m_nPercent > 0.5f)
				m_bConnected = m_pNext->GetConnected();
		}
		else
		{
			*outInstances = dwInstances;
			*outFunctions = dwFunctions;
			*outSteps = dwSteps;
		}
	}
	inline void         Get(const value_t nPercent,
		const DWORD dwInstance,
		const DWORD dwFunction,
		value_t* outX,
		value_t* outY,
		value_t* outLineWidth,
		value_t* outColor);

	void        DrawParametric(BitCanvas* pBitCanvas);
	void        Draw4d(BitCanvas* pBitCanvas);

};


class MetaParticle : public MetaParticleAbstract
{
protected:
	//    DWORD                m_dwCurrent;
	//    DWORD                m_dwNext;
	value_t             m_nIntervalTime;
	//    value_t                m_nTweenDuration;
	DWORD               m_dwRunningList[MAX_RUNNING_PARTICLES];
	Particle* m_pParticles;
	Expression* m_pIntervalTime;
	Expression* m_pParticleProbability;
	Expression* m_pLifetime;
	//    Expression          *m_pTweenDuration;
	//    bool                m_bTweening;
	//    bool                m_bConnected;

	HighResolutionTimer     m_hrParticleTimer;
	MyDictionary<value_t*>    m_dValues;



public:
	MetaParticle();
	virtual             ~MetaParticle();

	virtual error_t     InitializeDerived(
		MyDictionary<char*>* inMainConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals);

	virtual error_t     ReduceTime(void);

protected:
	virtual error_t     UpdateDerived(BitCanvas* pBitCanvas);
	virtual error_t     UpdateDerived(WindowDevice* pWindowDevice);

	DWORD               CountParticles(void);

	error_t             IntervalCheck(void);

};





#ifdef UNDEFINED

typedef struct tagPARTICLE_INFO {
	DWORD               dwMode;
	DWORD               dwNumInstances;
	DWORD               dwNumFunctions;
	DWORD               dwNumSteps;
	Particle* pParticle;
} PARTICLE_INFO;



typedef struct tagTWEEN_INFO {
	DWORD               dwMode;
	Particle* pCurrent;
	Particle* pNext;
	HighResolutionTimer hrTween;
	HighResolutionTimer hrLife;
	value_t             nTweenTime;
	value_t             nLifeTime;
	bool                bActive;
} TWEEN_INFO;

#endif





#endif  /* _MetaParticle_H_ */

