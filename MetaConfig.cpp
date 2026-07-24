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

#include "MetaConfig.h"


  /****************************************************************************
   *
   * MetaConfig - constructor
   *
   ****************************************************************************/
MetaConfig::MetaConfig()
{
	m_pConfigs = NULL;
	m_nUpdateTime = 0.0f;
}


/****************************************************************************
 *
 * ~MetaConfig - destructor
 *
 ****************************************************************************/
MetaConfig::~MetaConfig()
{
	//    SAFE_DELETE_ARRAY( m_pConfigs );
}



/****************************************************************************
 *
 * Initialize
 *
 ****************************************************************************/
error_t    MetaConfig::Initialize(
	MyDictionary<mychar_t*>* pConfigs,
	const DWORD dwNumConfigs,
	MyDictionary<mychar_t*>* inMainConfig,
	MyDictionary<EXPRESSIONDESCRIPTION*>* inGlobals)
{

	if (pConfigs == NULL || dwNumConfigs == 0)
		return ERR_NULL;

	m_pConfigs = pConfigs;
	m_dwNumConfigs = dwNumConfigs;

	m_fsRecentList.RestrictSize(m_dwNumConfigs / 2);


	return InitializeDerived(inMainConfig, inGlobals);
}



#ifdef UNDEFINED
/****************************************************************************
 *
 * DisplayString
 *
 ****************************************************************************/
error_t    MetaConfig::DisplayString(const int position, const char* strName, const char* string, WindowDevice* pWindowDevice)
{
	char strPrintString[256];
	int x;
	int y;

	if (strName != NULL && string != NULL && pWindowDevice != NULL)
	{
		strcpy(strPrintString, strName);
		if (strlen(strName) > 0)
			strcat(strPrintString, " : ");
		strcat(strPrintString, string);

		while (strlen(strPrintString) < 200)
			strcat(strPrintString, " ");

		switch (position)
		{
		case 0:  /* deltafield */
			x = 60;
			y = 0;
			break;
		case 1:  /* waveshape */
			x = 0;
			y = 20;
			break;
		case 2: /* particle */
			x = 0;
			y = 40;
			break;
		case 3: /* nothing */
			x = -1;
			y = 0;
			break;
		case 4: /* palette */
			x = -1;
			y = 20;
			break;
		case 5: /* palette */
			x = -1;
			y = 40;
			break;
			/*        case 6: // unused
						x = 320;
						y = 40;
						break;
					case 7: // unused
						x = 320;
						y = 60;
						break;/**/
		case 8:  /* deltafield */
			x = 0;
			y = -60;
			break;
		case 9:  /* waveshape */
			x = 0;
			y = -40;
			break;
		case 10: /* particle */
			x = 0;
			y = -20;
			break;
		case 11: /* palette */
			x = -1;
			y = -60;
			break;
		case 12: /* palette */
			x = -1;
			y = -40;
			break;
		case 13: /* palette */
			x = -1;
			y = -20;
			break;
			/*        case 6: // unused
						x = 320;
						y = 460;
						break;
					case 7: // unused
						x = 320;
						y = 460;
						break;/**/
		default:
			x = 0;
			y = 0;
			break;
		}

		return pWindowDevice->Print(x, y, strPrintString);
	}
	else
		return ERR_NULL;
}

#else

#ifdef UNDEFINED
/****************************************************************************
 *
 * DisplayString
 *
 ****************************************************************************/
error_t    MetaConfig::DisplayString(const int position, const char* strName, const char* string, WindowDevice* pWindowDevice)
{
	char strPrintString[256];
	int x;
	int y;

	if (strName != NULL && string != NULL && pWindowDevice != NULL)
	{
		strcpy(strPrintString, strName);
		if (strlen(strName) > 0)
			strcat(strPrintString, " : ");
		strcat(strPrintString, string);

		while (strlen(strPrintString) < 200)
			strcat(strPrintString, " ");

		switch (position)
		{
		case 0:  /* deltafield */
			x = 60;
			y = 0;
			break;
		case 1:  /* waveshape */
			x = 0;
			y = 17;
			break;
		case 2: /* particle */
			x = 0;
			y = 34;
			break;
		case 3: /* nothing */
			x = 350;
			y = 0;
			break;
		case 4: /* palette */
			x = 320;
			y = 17;
			break;
		case 5: /* palette */
			x = 320;
			y = 34;
			break;
			/*        case 6: // unused
						x = 320;
						y = 40;
						break;
					case 7: // unused
						x = 320;
						y = 60;
						break;/**/
		case 8:  /* deltafield */
			x = 0;
			y = 430;
			break;
		case 9:  /* waveshape */
			x = 0;
			y = 447;
			break;
		case 10: /* particle */
			x = 0;
			y = 464;
			break;
		case 11: /* palette */
			x = 320;
			y = 430;
			break;
		case 12: /* palette */
			x = 320;
			y = 447;
			break;
		case 13: /* palette */
			x = 320;
			y = 464;
			break;
			/*        case 6: // unused
						x = 320;
						y = 460;
						break;
					case 7: // unused
						x = 320;
						y = 460;
						break;/**/
		default:
			x = 0;
			y = 0;
			break;
		}

		return pWindowDevice->Print(x, y, strPrintString);
	}
	else
		return ERR_NULL;
}

#else

/****************************************************************************
 *
 * DisplayString
 *
 ****************************************************************************/
error_t    MetaConfig::DisplayString(const int position, const char* strName, const char* string, WindowDevice* pWindowDevice)
{
	char strPrintString[256];
	int x;
	int y;

	if (strName != NULL && string != NULL && pWindowDevice != NULL)
	{
		if (strcpy_s(strPrintString, _countof(strPrintString), strName) != 0)
			return FAILURE;
		if (strlen(strName) > 0 && strcat_s(strPrintString, _countof(strPrintString), " : ") != 0)
			return FAILURE;
		if (strcat_s(strPrintString, _countof(strPrintString), string) != 0)
			return FAILURE;

		while (strlen(strPrintString) < 200)
		{
			if (strcat_s(strPrintString, _countof(strPrintString), " ") != 0)
				return FAILURE;
		}

		switch (position)
		{
		case 0:  /* deltafield */
			x = 60;
			y = 0;
			break;
		case 1:  /* waveshape */
			x = 0;
			y = 17;
			break;
		case 2: /* particle */
			x = 0;
			y = 34;
			break;
		case 3: /* nothing */
			x = -1; // 350
			y = 0;
			break;
		case 4: /* palette */
			x = -1; // 320
			y = 17;
			break;
		case 5: /* palette */
			x = -1;
			y = 34;
			break;
/*        case 6: // unused
			x = 320;
			y = 40;
			break;
		case 7: // unused
			x = 320;
			y = 60;
			break;/**/
		case 8:  /* deltafield */
			x = 0;
			y = -50;
			break;
		case 9:  /* waveshape */
			x = 0;
			y = -33;
			break;
		case 10: /* particle */
			x = 0;
			y = -16;
			break;
		case 11: /* palette */
			x = -1;
			y = -50;
			break;
		case 12: /* palette */
			x = -1;
			y = -33;
			break;
		case 13: /* palette */
			x = -1;
			y = -16;
			break;
/*        case 6: // unused
			x = 320;
			y = 460;
			break;
		case 7: // unused
			x = 320;
			y = 460;
			break;/**/
		default:
			x = 0;
			y = 0;
			break;
		}

		return pWindowDevice->Print(x, y, strPrintString);
	}
	else
		return ERR_NULL;
}

#endif


#endif

