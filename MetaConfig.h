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
  * MetaConfig - abstract class for managing configs
  *
  ****************************************************************************/

#ifndef _MetaConfig_H_
#define _MetaConfig_H_

#include "BitCanvas.h"
#include "WindowDevice.h"
#include "HighResolutionTimer.h"
#include "MyDictionary.h"
#include "Particle.h"
#include "Palette.h"

#include "MersenneTwister.h"

class MetaConfig
{
private:
	HighResolutionTimer m_hrTimer;
protected:
	MTRand              m_mtRand;
	//    MTRand::uint32      m_dwNumConfigs;
	DWORD               m_dwNumConfigs;
private:
	FiniteSet<void*, 61> m_fsRecentList;  /* hmm, tryint to make it private */
protected:
	value_t             m_nUpdateTime;
	MyDictionary<char*>* m_pConfigs; /* array of configs */


public:
	MetaConfig();
	virtual             ~MetaConfig();

	error_t             Initialize(
		MyDictionary<char*>* pConfigs,
		const DWORD dwNumConfigs,
		MyDictionary<char*>* inMainConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals); /* constants are placed in globals */

	value_t             GetUpdateTime(void) { return m_nUpdateTime; };
	virtual error_t     ReduceTime(void) /* need a better name! */
	{
		/* default - ignore it 'cause I don't wanna reduce my update time */
		/* actually, it's because whatever caused the slowdown was a
		 * one time deal and won't cause a slowdown next frame
		 */
		return SUCCESS;
	};

	/*
	 * Update - does some error checking before calling update derived
	 */
	error_t             Update(BitCanvas* pBitCanvas)
	{
		error_t    err;

		m_hrTimer.Start();

		if (pBitCanvas == NULL)
			return ERR_NULL;

		err = UpdateDerived(pBitCanvas);

		m_nUpdateTime = m_hrTimer.Seconds();

		return err;
	};

	error_t             Update(WindowDevice* pWindowDevice)
	{
		error_t    err;

		if (pWindowDevice == NULL)
			return ERR_NULL;

		err = UpdateDerived(pWindowDevice);

		return err;
	};


private:
	DWORD               RandomConfigNum(void)
	{
		MTRand::uint32  ui32_NumConfigs = (m_dwNumConfigs > 0) ? (m_dwNumConfigs - 1) : 0;

		return m_mtRand.randInt(ui32_NumConfigs);
	};/**/
protected:
	DWORD               GetRandomConfig(void)
	{
		DWORD               dwNewConfigNumber;
		MyDictionary<char*>* pNewConfig;

		//        do {} while( (dwReturn = m_mtRand.randInt( m_dwNumConfigs ) ) >= m_dwNumConfigs );

		do {
			dwNewConfigNumber = RandomConfigNum();
			pNewConfig = &m_pConfigs[dwNewConfigNumber];
		} while (m_fsRecentList.Includes(pNewConfig));

		m_fsRecentList.Add(pNewConfig);

		return dwNewConfigNumber;
	};
	error_t             DisplayString(const int position,
		const char* strName,
		const char* string,
		WindowDevice* pWindowDevice);

	virtual error_t     InitializeDerived(
		MyDictionary<char*>* inMainConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals) = 0;

	virtual error_t     UpdateDerived(BitCanvas* pBitCanvas) = 0;
	virtual error_t     UpdateDerived(WindowDevice* pWindowDevice) = 0;
};

#endif  /* _MetaConfig_H_ */

