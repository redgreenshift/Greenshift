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

#include "MetaWaveShape.h"

/****************************************************************************
 *
 * MetaWaveShape - constructor
 *
 ****************************************************************************/
MetaWaveShape::MetaWaveShape()
{
    m_dwCurrent      = 0xBADC0DE;
    m_dwNext         = 0xBADC0DE;
    m_nIntervalTime  = 0.0f;
    m_nTweenDuration = 0.0f;
    m_pWaveShapes    = NULL;
    m_pIntervalTime  = NULL;
    m_pTweenDuration = NULL;

    m_bTweening      = false;
    m_bConnected     = false;
}

/****************************************************************************
 *
 * ~MetaWaveShape - destructor
 *
 ****************************************************************************/
MetaWaveShape::~MetaWaveShape()
{
    SAFE_DELETE( m_pIntervalTime );
    SAFE_DELETE( m_pTweenDuration );
    SAFE_DELETE_ARRAY( m_pWaveShapes );
}



/****************************************************************************
 *
 * InitializeDerived - NULL is not passed
 *
 ****************************************************************************/
error_t    MetaWaveShape::InitializeDerived(
                            MyDictionary<char*> *inMainConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals )
{
    DWORD   i;
    error_t err;

//    value_t  nDefaultAspect;

    m_pWaveShapes = new WaveShape[ m_dwNumConfigs ];
    if( m_pWaveShapes == NULL )
        return ERR_MALLOC;


//    nDefaultAspect = Expression::Evaluate("canvas_aspect", 640.0f / 380.0f, &m_dValues, inGlobals);
//    nDefaultAspect = Expression::Evaluate("default_aspect", 640.0f / 380.0f, &m_dValues, inGlobals);

//    sprintf( m_strDefaultAspect, "%f", nDefaultAspect );



    /*
     * set variables
     */
    if(    //(err = m_dValues.SetValue( "NUM_PARTICLES", &m_nNumParticles )) != SUCCESS        ||
        (err = m_dValues.SetValue( "s", &m_nStep )) != SUCCESS
        || (err = m_dValues.SetValue( "x", &m_nX )) != SUCCESS
        || (err = m_dValues.SetValue( "z", &m_nZ )) != SUCCESS )
    {
        return err;
    }

    for( i = 0; i < m_dwNumConfigs; i++ )
        if( (err = m_pWaveShapes[i].Initialize( &m_pConfigs[i], &m_dValues, inGlobals )) != SUCCESS )
        {
#if EXTREME_DEBUGGING
    if( err != SUCCESS )
    {
        DumpToFile( "error.txt", "-=- BEGIN DICTIONARY DUMP -=-", "\n" );
        m_pConfigs[i].DebugDumpContents( "error.txt" );
        DumpToFile( "error.txt", "-=- END DICTIONARY DUMP -=-", "\n" );
    }
#endif
       
            if( i > 0 )
            {
                m_pConfigs[i].WipeContents();
                err = m_pConfigs[i].Import( m_pConfigs[i-1] );
            }

            if( err != SUCCESS ||
                (err = m_pWaveShapes[i].Initialize( &m_pConfigs[i], &m_dValues, inGlobals )) != SUCCESS )
                return err;
        }



#if EXTREME_DEBUGGING
    DumpToFile( "error.txt", "Begin Compiling WaveShape interval and tween", "\n" );
#endif
       
        if( (err = Expression::Compile(
                inMainConfig->GetValue( "waveshape_interval" ),
                &m_pIntervalTime, &m_dValues, inGlobals )) != SUCCESS
        || (err = Expression::Compile(
                inMainConfig->GetValue( "waveshape_tween" ),
                &m_pTweenDuration, &m_dValues, inGlobals )) != SUCCESS
    )
        return err;

#if EXTREME_DEBUGGING
    DumpToFile( "error.txt", "Success Compiling WaveShape interval and tween", "\n" );
#endif


    return SUCCESS;
}


/****************************************************************************
 *
 * UpdateDerived - pWindowDevice is not NULL
 *
 ****************************************************************************/
error_t    MetaWaveShape::UpdateDerived( WindowDevice *pWindowDevice )
{
    if( m_dwCurrent != 0xBADC0DE )
        DisplayString( 8, "Current WaveShape ", m_pConfigs[m_dwCurrent].GetValue("NAME"), pWindowDevice );
    else
        DisplayString( 8, "Current WaveShape ", "                                        ", pWindowDevice );

    if( m_dwNext != 0xBADC0DE )
        DisplayString( 9, "Next     WaveShape ", m_pConfigs[m_dwNext].GetValue("NAME"), pWindowDevice );
    else
        DisplayString( 9, "Next     WaveShape ", "                                          ", pWindowDevice );

    return SUCCESS;
}


#ifdef UNDEFINED

/****************************************************************************
 *
 * UpdateDerived - pBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    MetaWaveShape::UpdateDerived( BitCanvas *pBitCanvas )
{
    IntervalCheck();

    if( m_pWaveShapes[m_dwCurrent].HackGetMode() == VG_PARAMETRIC )
        Draw2d( pBitCanvas );
    else
        Draw4d( pBitCanvas );

    return SUCCESS;
}

#else

/****************************************************************************
 *
 * UpdateDerived - pBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    MetaWaveShape::UpdateDerived( BitCanvas *pBitCanvas )
{
//    DWORD    dwRun;

    IntervalCheck();

    /* I don't need this time, the particles keep track of the time THEMSELVES */
//    m_nTime    = m_hrWaveShapeTimer.Seconds();
//    m_nPercent = m_nTime / m_nTweenDuration;
    if( m_bTweening )
//        m_nPercent = m_hrWaveShapeTimer.Seconds() / m_nTweenDuration;
        m_nPercent = m_hrTweenTimer.Seconds() / m_nTweenDuration;
//    m_nPercent = 0.0f;



//    for( dwRun = 0; dwRun < MAX_RUNNING_PARTICLES; dwRun++ )
    {
//        if( m_dwRunningList[dwRun] == 0xBADC0DE )
//            continue;

        m_pCurrent = &m_pWaveShapes[ m_dwCurrent ];

        GetNums( &m_dwNumInstances, &m_dwNumFunctions, &m_dwNumSteps );

        m_pCurrent->GetLastBuffers( &m_pLastX, &m_pLastY );

        m_pCurrent->BeginFrame();
        if( m_bTweening )
            m_pNext->BeginFrame();

        m_dwMode = m_pCurrent->GetMode();

        if( m_dwMode == VG_4D )
            Draw4d( pBitCanvas );
        else
            DrawParametric( pBitCanvas );

        m_pCurrent->EndFrame();
        if( m_bTweening )
            m_pNext->EndFrame();
    }

    return SUCCESS;
}
#endif



/****************************************************************************
 *
 * ReduceTime - reduce the time required to draw.
 *     for wave shapes this means kill the current waveshape(s) and select a new one
 *
 ****************************************************************************/
error_t    MetaWaveShape::ReduceTime( void )
{
    DWORD    dwNewWaveShape;

/*    do
    {
        dwNewWaveShape = RANDOM( m_dwNumConfigs );
    } while( m_fsRecentList.Includes( &m_pWaveShapes[dwNewWaveShape] ) );

    m_fsRecentList.Add( &m_pWaveShapes[dwNewWaveShape] );/**/

    dwNewWaveShape = GetRandomConfig();

    m_bTweening = false;
    m_dwCurrent = dwNewWaveShape;
    m_nIntervalTime = m_pIntervalTime->Evaluate();
    m_hrIntervalTimer.Start();  /* Do I need this? yes, because earlier without it, tweening would start right away */
    m_dwNext = 0xBADC0DE; /* safety precaution in case it tries to tween */

    return SUCCESS;
}


/****************************************************************************
 *
 * IntervalCheck - doesn't need an error return
 *
 ****************************************************************************/
error_t    MetaWaveShape::IntervalCheck( void )
{
    value_t nNewTweenDuration;
    value_t nLifetime;

    if( ! m_bTweening && m_nIntervalTime < m_hrIntervalTimer.Seconds() )
    {

/*        do {
//            m_dwNext = RANDOM( m_dwNumConfigs );
            m_dwNext = RandomConfigNum();
            m_pNext  = &m_pWaveShapes[m_dwNext];
        }    while( m_fsRecentList.Includes( m_pNext ) );
        m_fsRecentList.Add( m_pNext );/**/

        m_dwNext = GetRandomConfig();
        m_pNext  = &m_pWaveShapes[m_dwNext];

        m_bTweening      = true;
        m_hrTweenTimer.Start();

        nNewTweenDuration = m_pTweenDuration->Evaluate();
        m_nIntervalTime   = m_pIntervalTime->Evaluate();

        nLifetime = m_nTweenDuration + m_nIntervalTime + nNewTweenDuration;
        /*
         * lifetime needs to extend from the initial intro tween,
         * through its lifetime interval, and then the out tween
         */
        m_pNext->SetLifetime( nLifetime );
//        m_hrWaveShapeTimer.Start();

        /*
         * FourD particles can't tween, so if one's involved, skip the tween process
         */
        if( m_dwCurrent == 0xBADC0DE || m_pWaveShapes[m_dwCurrent].GetMode() != VG_PARAMETRIC )
        {
            m_pCurrent  = m_pNext;
            m_pNext     = NULL;
            m_dwCurrent = m_dwNext;
            m_dwNext    = 0xBADC0DE;
            m_pWaveShapes[m_dwCurrent].SetLifetime( m_nIntervalTime + m_nTweenDuration ); // should just be interval time
            m_bTweening = false;
            m_hrIntervalTimer.Start();
        }
        else
        if( m_pWaveShapes[m_dwNext].GetMode() != VG_PARAMETRIC )
        {
            m_pCurrent  = m_pNext;
            m_pNext     = NULL;
            m_dwCurrent = m_dwNext;
            m_dwNext    = 0xBADC0DE;
            m_pWaveShapes[m_dwCurrent].SetLifetime( m_nIntervalTime + m_nTweenDuration );
            m_bTweening = false;
            m_hrIntervalTimer.Start();
        }

        m_nTweenDuration = nNewTweenDuration;
    }
    else
    if( m_bTweening && m_nTweenDuration < m_hrTweenTimer.Seconds() )
    {
//        if( m_pNext != NULL && m_dwNext != 0xBADC0DE )
//        {
            m_pCurrent      = m_pNext;
            m_pNext         = NULL;
            m_dwCurrent     = m_dwNext;
            m_dwNext        = 0xBADC0DE;
//        }
        m_bTweening     = false;

//        m_nIntervalTime = m_pIntervalTime->Evaluate();
//        m_hrWaveShapeTimer.Start();
        m_hrIntervalTimer.Start();
    }

    return SUCCESS;
}

