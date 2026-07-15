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

#include "MetaParticle.h"


/****************************************************************************
 *
 * MetaParticle - constructor
 *
 ****************************************************************************/
MetaParticle::MetaParticle()
{
    DWORD    i;

    for( i = 0; i < MAX_RUNNING_PARTICLES; i++ )
        m_dwRunningList[i] = 0xBADC0DE;

    m_nIntervalTime        = 0.0f;
//    m_nTweenDuration       = 0.0f;

    m_pLifetime            = NULL;
    m_pParticles           = NULL;
    m_pIntervalTime        = NULL;
    m_pParticleProbability = NULL;
    m_bTweening            = false;
    m_bConnected           = false;
}

/****************************************************************************
 *
 * ~MetaParticle - destructor
 *
 ****************************************************************************/
MetaParticle::~MetaParticle()
{
    SAFE_DELETE( m_pLifetime );
    SAFE_DELETE( m_pIntervalTime );
    SAFE_DELETE( m_pParticleProbability );
//    SAFE_DELETE( m_pTweenDuration );
    SAFE_DELETE_ARRAY( m_pParticles );
}



/****************************************************************************
 *
 * InitializeDerived - NULL is not passed
 *
 ****************************************************************************/
error_t    MetaParticle::InitializeDerived(
                            MyDictionary<char*> *inMainConfig,
                            MyDictionary<EXPRESSIONDESCRIPTION*> *inGlobals )
{
    DWORD    i;
    error_t    err;

    m_pParticles = new Particle[ m_dwNumConfigs ];
    if( m_pParticles == NULL )
        return ERR_MALLOC;

    /*
     * set variables
     */
    if(    (err = m_dValues.SetValue( "NUM_PARTICLES", &m_nNumParticles )) != SUCCESS
        || (err = m_dValues.SetValue( "s", &m_nStep )) != SUCCESS
        || (err = m_dValues.SetValue( "x", &m_nX )) != SUCCESS
        || (err = m_dValues.SetValue( "z", &m_nZ )) != SUCCESS )
    {
        return err;
    }


    for( i = 0; i < m_dwNumConfigs; i++ )
        if( (err = m_pParticles[i].Initialize( &m_pConfigs[i], &m_dValues, inGlobals )) != SUCCESS )
        {
#if EXTREME_DEBUGGING
    if( err != SUCCESS )
    {
        DumpToFile( "error.txt", "-=- BEGIN MyDictionary DUMP -=-", "\n" );
        m_pConfigs[i].DebugDumpContents( "error.txt" );
        DumpToFile( "error.txt", "-=- END MyDictionary DUMP -=-", "\n" );
    }
#endif
            if( i > 0 )
            {
                m_pConfigs[i].WipeContents();
                err = m_pConfigs[i].Import( m_pConfigs[i-1] );
            }

            if( err != SUCCESS ||
                (err = m_pParticles[i].Initialize( &m_pConfigs[i], &m_dValues, inGlobals )) != SUCCESS )
                return err;
        }


#if EXTREME_DEBUGGING
    DumpToFile( "error.txt", "Begin Compiling Particle Interval lifetime and probability", "\n" );
#endif

    if( (err = Expression::Compile(
                inMainConfig->GetValue( "particle_interval" ),
                &m_pIntervalTime, &m_dValues, inGlobals )) != SUCCESS
        || (err = Expression::Compile(
                inMainConfig->GetValue( "particle_lifetime" ),
                &m_pLifetime, &m_dValues, inGlobals )) != SUCCESS
        || (err = Expression::Compile(
                inMainConfig->GetValue( "particle_probability" ),
                &m_pParticleProbability, &m_dValues, inGlobals )) != SUCCESS
//        || (err = Expression::Compile(
//                inMainConfig->GetValue( "particle_tween" ),
//                &m_pTweenDuration, NULL, inGlobals )) != SUCCESS
    )
        return err;

#if EXTREME_DEBUGGING
    DumpToFile( "error.txt", "Success Compiling Particle Interval lifetime and probability", "\n" );
#endif

    return SUCCESS;
}



/****************************************************************************
 *
 * UpdateDerived - pWindowDevice is not NULL
 *
 ****************************************************************************/
error_t    MetaParticle::UpdateDerived( WindowDevice *pWindowDevice )
{
    char    strName[16];
    DWORD    particle;
    DWORD    i;
    DWORD    dwNum = 0;

    for( i = 0; i < MAX_RUNNING_PARTICLES; i++ )
    {
        if( (particle = m_dwRunningList[i]) != 0xBADC0DE )
        {
            sprintf( strName, "Particle %d", dwNum );
            DisplayString( 10+dwNum, strName, m_pConfigs[particle].GetValue("NAME"), pWindowDevice );
            dwNum++;
        }
    }

    for( ; dwNum < MAX_RUNNING_PARTICLES; dwNum++ )
        DisplayString( 10+dwNum, "", "                                        ", pWindowDevice );


    return SUCCESS;
}


/****************************************************************************
 *
 * UpdateDerived - pBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    MetaParticle::UpdateDerived( BitCanvas *pBitCanvas )
{
    DWORD    dwRun;

//    m_nTime = m_hrParticleTimer.Seconds();
//    m_nPercent = m_nTime / m_nTweenDuration;
//    m_nPercent = 0.0f;


    IntervalCheck();

    for( dwRun = 0; dwRun < MAX_RUNNING_PARTICLES; dwRun++ )
    {
        if( m_dwRunningList[dwRun] == 0xBADC0DE )
            continue;

        m_pCurrent = &m_pParticles[m_dwRunningList[dwRun]];

        GetNums( &m_dwNumInstances, &m_dwNumFunctions, &m_dwNumSteps );

        m_pCurrent->GetLastBuffers( &m_pLastX, &m_pLastY );

        m_pCurrent->BeginFrame();

        m_dwMode = m_pCurrent->GetMode();

        if( m_dwMode == VG_4D )
            Draw4d( pBitCanvas );
        else
            DrawParametric( pBitCanvas );

        m_pCurrent->EndFrame();
    }

    return SUCCESS;
}



/****************************************************************************
 *
 * ReduceTime - reduce the time required to draw.
 *
 ****************************************************************************/
error_t    MetaParticle::ReduceTime( void )
{
    DWORD   i;
    DWORD   dwSlowestParticle = 0xBADC0DE;
    DWORD   particle;
    value_t nLongestTime = -1.0f;

    for( i = 0; i < MAX_RUNNING_PARTICLES; i++ )
    {
        particle = m_dwRunningList[i];
        if( particle != 0xBADC0DE && m_pParticles[particle].DrawingTime() > nLongestTime )
        {
            nLongestTime = m_pParticles[particle].DrawingTime();
            dwSlowestParticle = particle;
        }
    }

    if( dwSlowestParticle != 0xBADC0DE )
    {
        m_pParticles[dwSlowestParticle].SetLifetime(0.0f);
        m_nNumParticles = (value_t)CountParticles();
    }

    m_hrParticleTimer.Start();
//    m_nIntervalTime *= 2;  /* repeatedly multiplies by 2 EVERY frame?  BAD! */

    return SUCCESS;
}

/****************************************************************************
 *
 * IntervalCheck - doesn't need an error return
 *
 ****************************************************************************/
error_t    MetaParticle::IntervalCheck( void )
{
    DWORD    i;
    DWORD    dwNewParticle;
    value_t    nRandom = -1.0f;

    if( m_nIntervalTime < m_hrParticleTimer.Seconds() )
    {
//        m_bTweening = true;
        m_nIntervalTime = m_pIntervalTime->Evaluate();
        m_hrParticleTimer.Start();

        if( CountParticles() < MAX_RUNNING_PARTICLES - 1 &&
            m_nNumParticles < m_dwNumConfigs &&
//            (nRandom = Random01()) < m_pParticleProbability->Evaluate() )
            m_mtRand.rand() < m_pParticleProbability->Evaluate() )
        {

/*            do {
//                dwNewParticle = RANDOM( m_dwNumConfigs );

                dwNewParticle = RandomConfigNum();
            }    while( m_fsRecentList.Includes( &m_pParticles[dwNewParticle] ) );

            m_fsRecentList.Add( &m_pParticles[dwNewParticle] );/**/

            dwNewParticle = GetRandomConfig();

            m_pParticles[dwNewParticle].SetLifetime( m_pLifetime->Evaluate() );
            for( i = 0; i < MAX_RUNNING_PARTICLES; i++ )
                if( m_dwRunningList[i] == 0xBADC0DE )
                {
                    m_dwRunningList[i] = dwNewParticle;
                    break; /* only add the particle ONCE! */
                }
        }

//        DumpToFile("error.txt", nRandom, "\n");
    }

    return SUCCESS;
}





/****************************************************************************
 *
 * CountParticles
 *
 ****************************************************************************/
DWORD    MetaParticle::CountParticles( void )
{
    DWORD    i;
    DWORD    particle;
    DWORD    dwNumParticles;


    dwNumParticles = 0;
    for( i = 0; i < MAX_RUNNING_PARTICLES; i++ )
    {
        particle = m_dwRunningList[i];

        if( particle != 0xBADC0DE && ! m_pParticles[particle].Expired() )
            dwNumParticles++;
        else
            m_dwRunningList[i] = 0xBADC0DE;
    }

    m_nNumParticles = (value_t)dwNumParticles;

    return dwNumParticles;
}







/****************************************************************************
 ****************************************************************************
 *
 * MetaParticleAbstract
 *
 ****************************************************************************
 ****************************************************************************/

/****************************************************************************
 *
 * MetaParticleAbstract - constructor
 *
 ****************************************************************************/
MetaParticleAbstract::MetaParticleAbstract()
{
    m_nNumParticles  = 0.0f;
    m_nStep          = 0.0f;
    m_nX             = 0.0f;
    m_nZ             = 0.0f;

    m_dwMode         = VG_PARAMETRIC;
    m_dwNumInstances = 0;
    m_dwNumFunctions = 0;
    m_dwNumSteps     = 0;

//    m_nTime          = 0.0f;
    m_nPercent       = 0.0f;

    m_pCurrent       = NULL;
    m_pNext          = NULL;
    m_pLastX         = NULL;
    m_pLastY         = NULL;

    m_bTweening      = false;
    m_bConnected     = false;
}

/****************************************************************************
 *
 * ~MetaParticleAbstract - destructor
 *
 ****************************************************************************/
MetaParticleAbstract::~MetaParticleAbstract()
{
}



/****************************************************************************
 *
 * Get - get the drawing characteristics
 *
 ****************************************************************************/
void    MetaParticleAbstract::Get( const value_t nPercent,
                            const DWORD dwInstance,
                            const DWORD dwFunction,
                            value_t *outX,
                            value_t *outY,
                            value_t *outLineWidth,
                            value_t *outColor)
{
//    if( m_pCurrent->HackGetMode() == VG_4D )
//        m_pCurrent->HackGet4d(dwInstance, dwFunction, m_nX, m_nZ, outX, outY, outLineWidth, outColor );
//    else
//        m_pCurrent->HackGet(dwInstance, dwFunction, outX, outY, outLineWidth, outColor );
    value_t    x1, y1, lw1, c1;
    value_t x2, y2, lw2, c2;
//    value_t lnPercent = nPercent;
//    char    *strName = m_pConfigs[m_dwCurrent].GetValue( "NAME" );

//    if( m_pCurrent->HackGetMode() == VG_4D )
//        m_pCurrent->HackGet4d(dwInstance, dwFunction, m_nX, m_nZ, outX, outY, outLineWidth, outColor );
//    else
    if( m_bTweening )
    {
//        if( ! _finite(lnPercent) )
//        {
//            DumpToFile("error.txt", "infinite percent?!", "\n");
//        }
//        else
//        if( lnPercent < 0.0f )
//            lnPercent = 0.0f;
//        else
//        if( lnPercent >= 1.0f )
//            lnPercent = 0.9999f;
        m_pCurrent->Get(dwInstance, dwFunction, &x1, &y1, &lw1, &c1);
        m_pNext->Get(dwInstance, dwFunction, &x2, &y2, &lw2, &c2);

//        *outX         = x1  * (1.0f - nPercent) + x2  * nPercent;
//        *outY         = y1  * (1.0f - nPercent) + y2  * nPercent;
//        *outLineWidth = lw1 * (1.0f - nPercent) + lw2 * nPercent;
//        *outColor     = c1  * (1.0f - nPercent) + c2  * nPercent;
        *outX         = x1  + (x2 - x1)   * nPercent;
        *outY         = y1  + (y2 - y1)   * nPercent;
        *outLineWidth = lw1 + (lw2 - lw1) * nPercent;
        *outColor     = c1  + (c2 - c1)   * nPercent;
    }
    else
        m_pCurrent->Get(dwInstance, dwFunction, outX, outY, outLineWidth, outColor );
}



#ifdef UNDEFINED
/****************************************************************************
 *
 * GetNums
 *
 ****************************************************************************/
void    MetaParticleAbstract::GetNums( DWORD *outInstances, DWORD *outFunctions, DWORD *outSteps )
{
    const DWORD    dwInstances = m_pCurrent->GetNumInstances();
    const DWORD    dwFunctions = m_pCurrent->GetNumFunctions();
    const DWORD    dwSteps     = m_pCurrent->GetNumSteps();
    DWORD    dwInstances2, dwFunctions2, dwSteps2;

    /* need to return each of the running particles */
//    *outInstances = m_pCurrent->HackGetNumInstances();
//    *outFunctions = m_pCurrent->HackGetNumFunctions();
//    *outSteps     = m_pCurrent->HackGetNumSteps();
    m_bConnected  = m_pCurrent->GetConnected();

    if( m_bTweening )
//    {
//        *outInstances = dwInstances;
//        *outFunctions = dwFunctions;
//        *outSteps     = dwSteps;
//    }
//    else
    {
        dwInstances2 = m_pNext->GetNumInstances();
        dwFunctions2 = m_pNext->GetNumFunctions();
        dwSteps2     = m_pNext->GetNumSteps();
        *outInstances = max( dwInstances, dwInstances2 );
        *outFunctions = max( dwFunctions, dwFunctions2 );
//        *outSteps     = max( dwSteps, dwSteps2 );
//        *outInstances = dwInstances * (1.0f - m_nPercent) + m_nPercent * dwInstances2;
//        *outFunctions = dwFunctions * (1.0f - m_nPercent) + m_nPercent * dwFunctions2;
        *outSteps     = dwSteps     * (1.0f - m_nPercent) + m_nPercent * dwSteps2;
//        *outSteps     = dwSteps     + (dwSteps2 - dwSteps) * m_nPercent;
        if( m_nPercent > 0.5f )
            m_bConnected = m_pNext->GetConnected();
    }
    else
    {
        *outInstances = dwInstances;
        *outFunctions = dwFunctions;
        *outSteps     = dwSteps;
    }
}
#endif

/****************************************************************************
 *
 * DrawParametric - pBitCanvas is not NULL
 *
 ****************************************************************************/
void    MetaParticleAbstract::DrawParametric( BitCanvas *pBitCanvas )
{
    DWORD    s;
    DWORD    f;
    DWORD    i;
    value_t lastX = 0.0f;
    value_t lastY = 0.0f;
    value_t x;
    value_t y;
    value_t line_width;
    value_t color;


        for( i = 0; i < m_dwNumInstances; i++ )
        {
            m_pCurrent->SetInstance( i );
            if( m_bTweening )
                m_pNext->SetInstance( i );

            for( f = 0; f < m_dwNumFunctions; f++  )
            {
                m_nStep = 0.0f;
                Get(m_nPercent, i, f, &x, &y, &line_width, &color);

//                pBitCanvas->DrawDot( x, -y, line_width, color );
                if( ! m_bConnected )
                    pBitCanvas->DrawDot( x, -y, line_width, color );

                for( s = 1; s < m_dwNumSteps; s++ )
                {
                    m_nStep = s / (value_t)(m_dwNumSteps-1);

                    lastX = x;
                    lastY = -y;

                    Get(m_nPercent, i, f, &x, &y, &line_width, &color);

                    if( m_bConnected )
                        pBitCanvas->DrawLine(lastX, lastY, x, -y, line_width, color );
                    else
                        pBitCanvas->DrawDot( x, -y, line_width, color );
                }

                /* if drawing lines, then make the end of the line rounded */
                if( m_bConnected )
                    pBitCanvas->DrawDot( x, -y, line_width, color );
            }
        }

}


/****************************************************************************
 *
 * Draw4d - pBitCanvas is not NULL
 *
 ****************************************************************************/
void    MetaParticleAbstract::Draw4d( BitCanvas *pBitCanvas )
{
    DWORD    dwX;
    DWORD    dwZ;
    DWORD    f;
    DWORD    i;
    value_t lastX = 0.0f;
    value_t lastY = 0.0f;
    value_t x;
    value_t y;
    value_t line_width;
    value_t color;


    for( i = 0; i < m_dwNumInstances; i++ )
    {
        m_pCurrent->SetInstance( i );
            if( m_bTweening )
                m_pNext->SetInstance( i );

        for( f = 0; f < m_dwNumFunctions; f++  )
        {
            for( dwZ = 0; dwZ < m_dwNumSteps; dwZ++ )
            {
                m_nX = -1.0f;
                m_nZ = dwZ * 2 / (value_t)(m_dwNumSteps-1) - 1.0f;
//                m_nStep = (value_t)sqrt( m_nX * m_nX + m_nZ * m_nZ );
                m_nStep = (value_t)_hypot( m_nX, m_nZ );
//                Get(m_nPercent, i, f, &x, &y, &line_width, &color);
                m_pCurrent->Get4d(i, f, m_nX, m_nZ, &x, &y, &line_width, &color );

                if( m_bConnected )
                {
                    if( dwZ != 0 )
                    {
                        pBitCanvas->DrawLine(m_pLastX[0], m_pLastY[0], x, -y, line_width, color );
                    }
                    m_pLastX[0] = x;
                    m_pLastY[0] = -y;
                }
                else
                    pBitCanvas->DrawDot( x, -y, line_width, color );

                m_pLastX[0] = x;
                m_pLastY[0] = -y;

                for( dwX = 1; dwX < m_dwNumSteps; dwX++ )
                {
                    m_nX = dwX * 2 / (value_t)(m_dwNumSteps-1) - 1.0f;
//                    m_nStep = (value_t)sqrt( m_nX * m_nX + m_nZ * m_nZ );
                    m_nStep = (value_t)_hypot( m_nX, m_nZ );

                    lastX = x;
                    lastY = -y;

//                    Get(m_nPercent, i, f, &x, &y, &line_width, &color);
                    m_pCurrent->Get4d(i, i, m_nX, m_nZ, &x, &y, &line_width, &color );

                    if( m_bConnected )
                    {
                        pBitCanvas->DrawLine(lastX, lastY, x, -y, line_width, color );
                        if( dwZ != 0 )
                        {
                            pBitCanvas->DrawLine(m_pLastX[dwX], m_pLastY[dwX], x, -y, line_width, color );
                        }
                        m_pLastX[dwX] = x;
                        m_pLastY[dwX] = -y;
                    }
                    else
                        pBitCanvas->DrawDot( x, -y, line_width, color );
                }
            }
        }
    }

}

