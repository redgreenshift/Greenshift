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
 * Particle - Abstract class
 *
 ****************************************************************************/

#ifndef _Particle_H_
#define _Particle_H_

//#include "GraphicalEntity.h"
//#include "BitCanvas.h"
#include "Expression.h"
#include "MyDictionary.h"
//#include "Interval.h"
#include "HighResolutionTimer.h"
#include "VectorGraphic.h"


/****************************************************************************
 *
 * Particle - root class for dynamic display objects?
 *
 ****************************************************************************/
//class Particle : public GraphicalEntity
class Particle
{
public:
                         Particle();
    virtual             ~Particle();

    virtual error_t     Initialize( MyDictionary<char*> *inConfig,
                            MyDictionary<value_t*> *inValues,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals );

protected:
    inline value_t      Lifetime( void ) { return m_nLifetime; };

public:
    inline value_t      DrawingTime( void ) { return m_nDrawingTime; };

    bool    Expired( void ) {return m_nLifetime < m_hrRunningTime.Seconds();};

    DWORD GetNumInstances(void){ return m_dwNumInstances; };
    DWORD GetNumFunctions(void){ return m_pVectorGraphics[0].GetNumFunctions(); };
    DWORD GetNumSteps( void )  { return m_pVectorGraphics[0].GetNumSteps(); };
    bool  GetConnected( void ) { return m_pVectorGraphics[0].GetConnected(); };

    DWORD               GetMode( void )
    {
        return m_pVectorGraphics[0].GetMode();
    };
    void                BeginFrame( void )
    {
        m_nTime = m_hrRunningTime.Seconds();
        m_hrDrawingTime.Start();
    };
    void                EndFrame( void )
    {
        m_nDrawingTime = m_hrDrawingTime.Seconds();
    };
    void                SetInstance( const DWORD dwInstance )
    {
        m_nID = (value_t)(dwInstance % m_dwNumInstances);
        m_pVectorGraphics[(dwInstance % m_dwNumInstances)].BVars();
        m_pVectorGraphics[(dwInstance % m_dwNumInstances)].BeginFrame();
    };

    inline void         Get( const DWORD dwInstance,
             const DWORD dwFunction,
             value_t    *outX,
             value_t    *outY,
             value_t    *outLineWidth,
             value_t    *outColor )
    {
        m_pVectorGraphics[dwInstance % m_dwNumInstances].Get( dwFunction, outX, outY, outLineWidth, outColor );
    };
    inline void         Get4d( const DWORD dwInstance,
             const DWORD dwFunction,
             const value_t inX,
             const value_t inZ,
             value_t    *outX,
             value_t    *outY,
             value_t    *outLineWidth,
             value_t    *outColor )
    {
        m_pVectorGraphics[dwInstance % m_dwNumInstances].Get4d( dwFunction, inX, inZ, outX, outY, outLineWidth, outColor );
    };

    void            GetLastBuffers( value_t **outLastX, value_t **outLastY )
    {
        m_pVectorGraphics[ (DWORD)m_nID ].GetLastBuffers( outLastX, outLastY );
    };

protected:
    MyDictionary<value_t*>    m_dValues;
    HighResolutionTimer     m_hrRunningTime;
    HighResolutionTimer     m_hrDrawingTime;

    value_t                 m_nNumInstances;/* the NUM var */
    DWORD                   m_dwNumInstances;
    value_t                 m_nID;          /* the ID var */

    value_t                 m_nTime;        /* the t var */
    value_t                 m_nLifetime;    /* the END_TIME var */
    value_t                 m_nDrawingTime; /* time taken to draw one frame */


//    PhaseFunction           m_pfValues;
//    inline error_t          Evaluate_A_Vars( void ) { return m_pfValues.EvaluatePhase(0); };
//    inline error_t          Evaluate_B_Vars( void ) { return m_pfValues.EvaluatePhase(1); };
//    inline error_t          Evaluate_C_Vars( void ) { return m_pfValues.EvaluatePhase(2); };
//    inline error_t          Evaluate_D_Vars( void ) { return m_pfValues.EvaluatePhase(3); };

public:
    void                    SetLifetime( const value_t nLifetime );


private:
    VectorGraphic           *m_pVectorGraphics;

};



/****************************************************************************
 *
 * WaveShape - special particle, only one onscreen at a time, and has a "running" flag so Greenshift knows when to replace it?
 *
 ****************************************************************************/
class WaveShape : public Particle
{
public:
/****************************************************************************
 *
 * WaveShape - is IDENTICAL to a Particle
 *
 ****************************************************************************/
                         WaveShape() {};
    virtual             ~WaveShape() {};
};


#endif  /* _Particle_H_ */

