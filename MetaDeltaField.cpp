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
  * MetaDeltaField -
  *
  ****************************************************************************/

#include "MetaDeltaField.h"

  /****************************************************************************
   *
   * MetaDeltaField - constructor
   *
   ****************************************************************************/
MetaDeltaField::MetaDeltaField()
{
	m_dwNext = 0xBADC0DE;
	m_dwCurrent = 0xBADC0DE;
	m_nInterval = 0;
	m_pDeltaFields = NULL;
	m_pIntervalTime = NULL;
	m_pTweenDuration = NULL;
}

/****************************************************************************
 *
 * ~MetaDeltaField - destructor
 *
 ****************************************************************************/
MetaDeltaField::~MetaDeltaField()
{
	SAFE_DELETE(m_pIntervalTime);
	SAFE_DELETE(m_pTweenDuration);
	SAFE_DELETE_ARRAY(m_pDeltaFields);
}



/****************************************************************************
 *
 * InitializeDerived - NULL is not passed
 *
 ****************************************************************************/
error_t    MetaDeltaField::InitializeDerived(
	MyDictionary<char*>* inMainConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{
	DWORD    i;
	error_t    err;

	m_pDeltaFields = new DeltaField[m_dwNumConfigs];
	if (m_pDeltaFields == NULL)
		return ERR_MALLOC;

	for (i = 0; i < m_dwNumConfigs; i++)
	{
		if ((err = m_pDeltaFields[i].Initialize(&m_pConfigs[i], inGlobals)) != SUCCESS)
		{
#if EXTREME_DEBUGGING
			if (err != SUCCESS)
			{
				DumpToFile("error.txt", "ERROR: ", ErrorString(err));
				if (err != ERR_NOTDELTAFIELD)
				{
					DumpToFile("error.txt", "\n-=- BEGIN DICTIONARY DUMP -=-", "\n");
					m_pConfigs[i].DebugDumpContents("error.txt");
					DumpToFile("error.txt", "-=- END DICTIONARY DUMP -=-", "\n");
				}
				else
				{
					char* name = m_pConfigs[i].GetValue("Name");
					DumpToFile("error.txt", "\n", "Filename: ");
					if (name != NULL)
						DumpToFile("error.txt", name, "\n");

				}

			}
#endif

			if (i > 0)
			{
				m_pConfigs[i].WipeContents();
				err = m_pConfigs[i].Import(m_pConfigs[i - 1]);
			}

			if (err != SUCCESS ||
				(err = m_pDeltaFields[i].Initialize(&m_pConfigs[i], inGlobals)) != SUCCESS)/**/
				return err;
		}
#if EXTREME_DEBUGGING
#define CONFIG_DUMP_FILE "configdump.txt"
		DumpToFile(CONFIG_DUMP_FILE, "\n--------------------------------\nNAME: ");
		DumpToFile(CONFIG_DUMP_FILE, m_pConfigs[i].GetValue("NAME", "error: <unknown config name>"), "\n");
		m_pDeltaFields[i].DebugDump(CONFIG_DUMP_FILE);
#endif
	}

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Begin Compile DeltaField interval and tween", "\n");
#endif

	/* circumvent problems that occur when the interval time is zero */
	if (inMainConfig->GetValue("deltafield_interval") != NULL
		&& strcmp(inMainConfig->GetValue("deltafield_interval"), "0") == 0)
	{
		inMainConfig->SetValue("deltafield_interval", "0.1");
	}

	/* set timing expressions */
	if ((err = Expression::Compile(
		inMainConfig->GetValue("deltafield_interval"),
		&m_pIntervalTime, NULL, inGlobals)) != SUCCESS
		|| (err = Expression::Compile(
			inMainConfig->GetValue("deltafield_tween"),
			&m_pTweenDuration, NULL, inGlobals)) != SUCCESS
		)
		return err;

#if EXTREME_DEBUGGING
	DumpToFile("error.txt", "Success Compile DeltaField interval and tween", "\n");
#endif

	return SUCCESS;
}



/****************************************************************************
 *
 * UpdateDerived - pWindowDevice is not NULL
 *
 ****************************************************************************/
error_t    MetaDeltaField::UpdateDerived(WindowDevice* pWindowDevice)
{
	if (m_dwCurrent != 0xBADC0DE)
		DisplayString(1, "Current DeltaField ", m_pConfigs[m_dwCurrent].GetValue("NAME"), pWindowDevice);
	else
		DisplayString(1, "Current DeltaField ", "                                        ", pWindowDevice);

	if (m_dwNext != 0xBADC0DE)
		DisplayString(2, "Next     DeltaField ", m_pConfigs[m_dwNext].GetValue("NAME"), pWindowDevice);
	else
		DisplayString(2, "Next     DeltaField ", "                                          ", pWindowDevice);

	return SUCCESS;
}


/****************************************************************************
 *
 * UpdateDerived - pBitCanvas is not NULL
 *
 ****************************************************************************/
error_t    MetaDeltaField::UpdateDerived(BitCanvas* pBitCanvas)
{
	error_t    err;

	if (m_dwNext != 0xBADC0DE && (err = pBitCanvas->SetDelta(NULL)) == ERR_NULL)
	{
		m_dwCurrent = m_dwNext;
		m_dwNext = 0xBADC0DE;
	}

	if (m_nInterval < m_hrInterval.Seconds())
	{
		/*
		 * make sure BitCanvas isn't busy calculating the last delta field
		 */
		if ((err = pBitCanvas->SetDelta(NULL)) == ERR_NULL)
		{
			if (m_dwNext != 0xBADC0DE)
				m_dwCurrent = m_dwNext;
			/*
			 * pick a DeltaField that is different from the last few
			 */
			 /*            do
						 {
			 //                m_dwNext = RANDOM( m_dwNumConfigs );
							 m_dwNext = RandomConfigNum();
						 }
						 while( m_fsRecentList.Includes( &m_pDeltaFields[m_dwNext] ) );

						 m_fsRecentList.Add( &m_pDeltaFields[m_dwNext] );/**/

			m_dwNext = GetRandomConfig();

			err = pBitCanvas->SetDelta(&m_pDeltaFields[m_dwNext]);


			if (err != SUCCESS)
				m_dwNext = 0xBADC0DE;

		}

		/*
		 * condition is true only the first time through
		 */
		if (m_dwCurrent == 0xBADC0DE)
		{
			m_dwCurrent = m_dwNext;
			m_dwNext = 0xBADC0DE;
		}

		/*
		 * update the interval
		 */
		m_nInterval = m_pIntervalTime->Evaluate();
		m_hrInterval.Start();
	}


	pBitCanvas->DoDelta();

	return SUCCESS;
}

