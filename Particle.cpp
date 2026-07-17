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
 * Particle
 *
 ****************************************************************************/

#include "Particle.h"

 
/****************************************************************************
 *
 * Particle - default constructor
 *
 ****************************************************************************/
Particle::Particle()
{
    m_pVectorGraphics = NULL;
    m_nNumInstances   = 0.0f;
    m_dwNumInstances  = 0;
    m_nTime           = 0.0f;
    m_nLifetime       = 0.0f;
    m_nID             = 0.0f;
    m_nDrawingTime    = 0.0f;
    m_nAspect         = 0.0f;
    m_bZoom           = false;

    /*
     * Particle keeps track of:
            Time
            Number of Instances (ie: number of VectorGraphics)
            EndTime

        VectorGraphic keeps track of:
            Dots vs lines
            Phase Function
            Color
            LineWidth

        VectorGraphic makes a copy of the MyDictionary passed to it

        Function keeps track of....
        RENAME IncrementalExpression to PhaseFunction!
     */
}

/****************************************************************************
 *
 * ~Particle - default destructor
 *
 ****************************************************************************/
Particle::~Particle()
{
    SAFE_DELETE_ARRAY( m_pVectorGraphics );
}


/****************************************************************************
 *
 * SetLifetime - reset time, set lifetime, restart timer, notify VectorGraphics
 *
 ****************************************************************************/
void    Particle::SetLifetime( const value_t nLifetime )
{
    DWORD    dwID;

    m_nTime        = 0.0f;
    m_nLifetime    = nLifetime;
    m_nDrawingTime = 0.0f;
    m_hrRunningTime.Start();
//    m_hrRunningTime.Start();

//    for( dwID = 0; dwID < m_dwNumInstances; dwID++ )
//        m_pVectorGraphics[dwID].Reset();
    m_pVectorGraphics[0].Reset();
    for( dwID = 1; dwID < m_dwNumInstances; dwID++ )
        ImportPhase( m_pVectorGraphics[dwID], m_pVectorGraphics[0], 0 );
}

/****************************************************************************
 *
 * Initialize - two stage initialization
 *
 ****************************************************************************/
error_t    Particle::Initialize( MyDictionary<char*> *inConfig,
                              MyDictionary<value_t*> *inValues,
                              MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals )
{
    error_t err;
    DWORD   dwID;
//    value_t nDefaultAspect = 1.0f;


    /*
     * set variables
     */
    if(    (err = m_dValues.SetValue( "t", &m_nTime )) != SUCCESS
        || (err = m_dValues.SetValue( "END_TIME", &m_nLifetime )) != SUCCESS
        || (err = m_dValues.SetValue( "NUM", &m_nNumInstances )) != SUCCESS
        || (err = m_dValues.SetValue( "ID", &m_nID )) != SUCCESS )
    {
        return err;
    }

    m_dValues.SetAlternate( inValues );

    /*
     * get NUM instances
     */
    m_nNumInstances = (value_t)Expression::Evaluate( inConfig->GetValue("NUM"),
                            1.0f, &m_dValues, inGlobals );
    
    if( m_nNumInstances < 1.0f )
        m_dwNumInstances = 1;
    else
        m_dwNumInstances = (DWORD)floor(m_nNumInstances);

    m_nNumInstances = (value_t)m_dwNumInstances;


//    nDefaultAspect = Expression::Evaluate("default_aspect", 1.0f, inValues, inGlobals);
//    nDefaultAspect = 0.0f;

    m_nAspect = (value_t)Expression::Evaluate( inConfig->GetValue("Aspect"),
                            0.0f, &m_dValues, inGlobals );

    if( m_nAspect < 0.0f )
        m_nAspect = 0.0f;

//    if( m_nAspect == 0.0f )
//        m_nAspect = nDefaultAspect;

    m_bZoom = (Expression::Evaluate( inConfig->GetValue("Zoom"),
        0.0f, &m_dValues, inGlobals ) == 0.0f) ? false : true;


    /*
     * this allocation works if VectorGraphic is not subclassed
     * if it is then I need to make an array of pointers
     */
    m_pVectorGraphics = new VectorGraphic[ m_dwNumInstances ];
    if( m_pVectorGraphics == NULL )
    {
        m_dwNumInstances = 0;
        return ERR_MALLOC;
    }

    for( dwID = 0; dwID < m_dwNumInstances; dwID++ )
    {
        m_nID = (value_t)dwID;

        err = m_pVectorGraphics[dwID].Initialize(
                                        inConfig, &m_dValues, inGlobals );
        if( err != SUCCESS )
            return err;

        if( dwID != 0 )
        err = ImportPhase( m_pVectorGraphics[dwID], m_pVectorGraphics[0], 0 );
    }

    return SUCCESS;
}

