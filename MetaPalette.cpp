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
  * MetaPalette -
  *
  ****************************************************************************/

#include "MetaPalette.h"


  /****************************************************************************
   *
   * MetaPalette - constructor
   *
   ****************************************************************************/
MetaPalette::MetaPalette()
{
	m_pPalettes = NULL;
	m_pInterval = NULL;
	m_pTweenTime = NULL;
	//    m_dwCurrent  = 0xBADC0DE;
	//    m_dwLast     = 0xBADC0DE;
	m_dwCurrent = 0xBADC0DE;
	m_dwNext = 0xBADC0DE;
	m_bTweening = false;
	m_nInterval = 0.0f;
	m_nTween = 0.0f;
	m_nPercent = 0.0f;
}

/****************************************************************************
 *
 * ~MetaWaveShape - destructor
 *
 ****************************************************************************/
MetaPalette::~MetaPalette()
{
	SAFE_DELETE(m_pInterval);
	SAFE_DELETE(m_pTweenTime);
	SAFE_DELETE_ARRAY(m_pPalettes);
}

/****************************************************************************
 *
 * InitializeDerived - NULL is not passed
 *
 ****************************************************************************/
error_t    MetaPalette::InitializeDerived(
	MyDictionary<char*>* inMainConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	DWORD    i;
	error_t    err;

	m_pPalettes = new Palette[m_dwNumConfigs];
	if (m_pPalettes == NULL)
		return ERR_MALLOC;

	for (i = 0; i < m_dwNumConfigs; i++)
		if ((err = m_pPalettes[i].Initialize(&m_pConfigs[i], inGlobals)) != SUCCESS)
		{
#if EXTREME_DEBUGGING
			if (err != SUCCESS)
			{
				DumpToFile("error.txt", ErrorString(err), "\n");
				DumpToFile("error.txt", "-=- BEGIN DICTIONARY DUMP -=-", "\n");
				m_pConfigs[i].DebugDumpContents("error.txt");
				DumpToFile("error.txt", "-=- END DICTIONARY DUMP -=-", "\n");
			}
#endif


			if (i > 0)
			{
				m_pConfigs[i].WipeContents();
				err = m_pConfigs[i].Import(m_pConfigs[i - 1]);
			}

			if (err != SUCCESS ||
				(err = m_pPalettes[i].Initialize(&m_pConfigs[i], inGlobals)) != SUCCESS)
				return err;
		}


#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Begin Compiling Palette interval and tween", "\n");
#endif

	if ((err = Expression::Compile(
		inMainConfig->GetValue("palette_interval"),
		&m_pInterval, NULL, inGlobals)) != SUCCESS
		|| (err = Expression::Compile(
			inMainConfig->GetValue("palette_tween"),
			&m_pTweenTime, NULL, inGlobals)) != SUCCESS
		)
		return err;

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Success Compiling Palette interval and tween", "\n");
#endif

	return SUCCESS;
}


/****************************************************************************
 *
 * UpdateDerived - pWindowDevice is not NULL
 *
 ****************************************************************************/
error_t    MetaPalette::UpdateDerived(WindowDevice* pWindowDevice)
{
	error_t    err = SUCCESS;

	if (m_dwCurrent != 0xBADC0DE)
		DisplayString(4, "Current Palette ", m_pConfigs[m_dwCurrent].GetValue("NAME"), pWindowDevice);
	else
		DisplayString(4, "Current Palette ", "                                        ", pWindowDevice);

	if (m_dwNext != 0xBADC0DE)
		DisplayString(5, "Next     Palette ", m_pConfigs[m_dwNext].GetValue("NAME"), pWindowDevice);
	else
		DisplayString(5, "Next     Palette ", "                                          ", pWindowDevice);

	return SUCCESS;
}

/****************************************************************************
 *
 * UpdateDerived - pBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    MetaPalette::UpdateDerived(BitCanvas* pBitCanvas)
{
	////    static DWORD    dwFrames = 0;
	error_t    err = SUCCESS;
	DWORD    dwNext;
	value_t    nTweenPercent;
	//    IntervalCheck();

		/*
		 * this code is so BLEH!  I need to clean it.
		 */

		 /* I think the error may be in here */

	if (m_bTweening)
	{
		nTweenPercent = m_hrTweenTimer.Seconds() / m_nTween;
		if (nTweenPercent < 1.0f)
		{
			//            DumpToFile( "error.txt", nTweenPercent, "\n" );
			//            if( m_dwNext != 0xBADC0DE && m_dwCurrent != 0xBADC0DE )
			////            if( dwFrames++ > 2 )
			{
				err = pBitCanvas->SetPalette(&m_pPalettes[m_dwCurrent],
					&m_pPalettes[m_dwNext],
					nTweenPercent);
				////            dwFrames = 0;
			}
		}
		else
		{
			m_bTweening = false;            /* finished tweening... */
			m_dwCurrent = m_dwNext;
			m_dwNext = 0xBADC0DE;
			m_nInterval = m_pInterval->Evaluate();
			m_hrIntervalTimer.Start();            /* ...and restart the timer */
		}



	}
	else


		if (m_nInterval < m_hrIntervalTimer.Seconds())
		{
			/*        do {
			//            dwNext = RANDOM( m_dwNumConfigs );
						dwNext = RandomConfigNum();
					} while( m_fsRecentList.Includes( &m_pPalettes[dwNext] ) );

					m_fsRecentList.Add( &m_pPalettes[dwNext] );/**/

			dwNext = GetRandomConfig();

			m_pPalettes[dwNext].ResetTimer(); /* restart the timer in case it's a time dependent palette */

			if (m_dwCurrent == 0xBADC0DE)
				err = pBitCanvas->SetPalette(&m_pPalettes[dwNext]);
			else
				if (m_dwCurrent != 0xBADC0DE && m_dwNext == 0xBADC0DE)
				{
					m_dwNext = dwNext;
					m_bTweening = true;
					m_nTween = m_pTweenTime->Evaluate();
					m_hrTweenTimer.Start();
					err = SUCCESS;
				}

			if (err == SUCCESS)
			{
				if (m_dwCurrent == 0xBADC0DE)
				{
					m_dwCurrent = dwNext;
					m_bTweening = false;
				}
			}

			//        m_nInterval = m_pInterval->Evaluate();
			//        m_hrIntervalTimer.Start();

			return err;
		}
		else
			err = pBitCanvas->SetPalette(&m_pPalettes[m_dwCurrent]);

	return err; /* should err be returned? */
}


#ifdef UNDEFINED
/****************************************************************************
 *
 * IntervalCheck - doesn't need an error return
 *
 ****************************************************************************/
void    MetaPalette::IntervalCheck(void)
{
	if (!m_bTweening && m_nInterval < m_hrIntervalTimer.Seconds())
	{
		m_dwLast = m_dwCurrent;
		do {
			m_dwCurrent = RANDOM(m_dwNumConfigs);
		} while (m_fsRecentList.Includes(&m_pPalettes[m_dwCurrent]));

		m_bTweening = true;
		m_nTween = m_pTweenTime->Evaluate();  /* set a new tween time */
		m_hrTweenTimer.Start();
	}
	else
		if (m_bTweening && m_nTween < m_hrTweenTimer.Seconds())
		{
			m_bTweening = false;
			m_nInterval = m_pInterval->Evaluate(); /* set a new interval */
			m_hrIntervalTimer.Start();
		}
}
#endif

