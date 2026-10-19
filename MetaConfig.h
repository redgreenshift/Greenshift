#pragma once
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

#include "BitCanvas.h"
#include "Expression.h"
#include "HighResolutionTimer.h"
#include "MTRand.hpp"
#include "MyDictionary.h"
#include "WindowDevice.h"

/**
 * @brief MetaConfig is an abstract base class designed to manage multiple sets
 * of config data through its derived subclasses. It provides common
 * functionality such as random selection of configs, timing management via a
 * high-resolution timer, and interfaces for initializing and updating
 * specific system components (like the BitCanvas or WindowDevice) with
 * provided config dictionaries.
 */
class MetaConfig
{
private:
	HighResolutionTimer m_hrTimer;
protected:
	MTRand              m_mtRand;
	//    MTRand::uint32      m_dwNumConfigs;
	dword_t             m_dwNumConfigs = 0;
private:
	FiniteSet<void*, 61> m_fsRecentList;  /* hmm, tryint to make it private */
protected:
	value_t             m_nUpdateTime;
	MyDictionary<mychar_t*>* m_pConfigs; /* array of configs */


public:
	MetaConfig();
	virtual             ~MetaConfig();

	error_t             Initialize(
		MyDictionary<mychar_t*>* pConfigs,
		const dword_t dwNumConfigs,
		MyDictionary<mychar_t*>* inMainConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals); /* constants are placed in globals */

	value_t             GetUpdateTime(void) const { return m_nUpdateTime; };
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

		m_nUpdateTime = m_hrTimer.ElapsedSeconds();

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
	dword_t               RandomConfigNum(void)
	{
		uint32_t  ui32_NumConfigs = (m_dwNumConfigs > 0) ? (m_dwNumConfigs - 1) : 0;

		return m_mtRand.randInt(ui32_NumConfigs);
	};/**/
protected:
	dword_t               GetRandomConfig(void)
	{
		dword_t               dwNewConfigNumber;
		MyDictionary<mychar_t*>* pNewConfig;

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
		MyDictionary<mychar_t*>* inMainConfig,
		MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals) = 0;

	virtual error_t     UpdateDerived(BitCanvas* pBitCanvas) = 0;
	virtual error_t     UpdateDerived(WindowDevice* pWindowDevice) = 0;
};

