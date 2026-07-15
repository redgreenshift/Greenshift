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
 * MetaWaveShape - 
 *
 ****************************************************************************/

#ifndef _MetaWaveShape_H_
#define _MetaWaveShape_H_

#include "MetaParticle.h"
#include "Particle.h"



class MetaWaveShape : public MetaParticleAbstract
{
protected:
//    value_t                m_nPercent;
    DWORD               m_dwCurrent;
    DWORD               m_dwNext;
    value_t             m_nIntervalTime;
    value_t             m_nTweenDuration;
//    HighResolutionTimer m_hrWaveShapeTimer;
    WaveShape           *m_pWaveShapes;
    Expression          *m_pIntervalTime;
    Expression          *m_pTweenDuration;
    HighResolutionTimer m_hrIntervalTimer;
    HighResolutionTimer m_hrTweenTimer;
//    bool                m_bTweening;
//    bool                m_bConnected;

//    value_t                *m_pLastX;
//    value_t                *m_pLastY;


//    value_t                    m_nStep;
//    value_t                    m_nX;
//    value_t                    m_nZ;
    MyDictionary<value_t*>    m_dValues;

public:
                        MetaWaveShape();
    virtual             ~MetaWaveShape();

    virtual error_t     InitializeDerived(
                            MyDictionary<char*> *inMainConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );


    virtual error_t     ReduceTime( void );

protected:
    virtual error_t     UpdateDerived( BitCanvas * pBitCanvas );
    virtual error_t     UpdateDerived( WindowDevice *pWindowDevice );

    error_t             IntervalCheck( void );

};


#endif  /* _MetaWaveShape_H_ */


