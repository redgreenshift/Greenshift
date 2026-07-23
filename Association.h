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
  * Association - set of associations of two objects
  *
  ****************************************************************************/

#ifndef _Association_H_
#define _Association_H_

  /*
   * an Association represents a link between a string and some data
   *
   * the KeyType and ValueType should be arbitrary
   */


   /****************************************************************************
	*
	* Association
	*
	* I represent a pair of associated objects
	* --a key and a value. My instances can serve
	* as entries in a dictionary.
	*
	****************************************************************************/
template<class ValueType>
class Association
{
public:
	/****************************************************************************
	 *
	 * Association - default constructor
	 *
	 ****************************************************************************/
	Association()
	{
		m_strKey = NULL;
		//m_strKey = m_strDebugKey;
		m_pValue = NULL;
	};

	/****************************************************************************
	 *
	 * Association - default destructor
	 *
	 ****************************************************************************/
	~Association()
	{
		SAFE_DELETE_ARRAY(m_strKey);
	};


#ifdef UNUSED_STUFF
	/****************************************************************************
	 *
	 * Association - special constructor
	 *
	 ****************************************************************************/
	Association(const char* inKey, ValueType inValue)
	{
		m_strKey = inKey;
		m_pValue = inValue;
	};
#endif // UNUSED_STUFF

	/****************************************************************************
	 *
	 * Hash
	 *
	 ****************************************************************************/
	DWORD   Hash(void)
	{
		DWORD    i;
		DWORD    len = strlen(m_strKey);

		if (strlen(m_strKey) > 1)
			return m_strKey[0] ^ Key()[strlen(m_strKey)];
		else
			return m_strKey[0];
	};

#ifdef UNUSED_STUFF
	/****************************************************************************
	 *
	 * Key - sets the key
	 *
	 ****************************************************************************/
	void    Key(char* inKey) { m_strKey = inKey; };
#endif

	/****************************************************************************
	 *
	 * NewStrdup - uses the "new" operator to create a string
	 *
	 ****************************************************************************/
	char* NewStrdup(const char* inString)
	{
		char* strTmp;
		const size_t size = strlen(inString) + 1;

		if (inString != NULL
			&& (strTmp = new char[size]) != NULL)
		{
			strncpy_s(strTmp, size, inString, size);
			return strTmp;
		}
		else
			return NULL;
	};

	/****************************************************************************
	 *
	 * Initialize - sets the key and value
	 *
	 ****************************************************************************/
	error_t Initialize(const char* inKey, ValueType inValue)
	{
		if (inKey != NULL)
		{
			//if( strcpy( m_strDebugKey, inKey ) != NULL )
			if ((m_strKey = NewStrdup(inKey)) != NULL)
			{
				SetValue(inValue);
				return SUCCESS;
			}
			else
				return ERR_MALLOC;
		}
		else
			return ERR_NULL;
	};


	/****************************************************************************
	 *
	 * GetKey - gets the key
	 *
	 ****************************************************************************/
	char* GetKey(void) { return m_strKey; };


	/****************************************************************************
	 *
	 * SetValue - attaches the value
	 *
	 ****************************************************************************/
	void    SetValue(ValueType inValue) { m_pValue = inValue; };


	/****************************************************************************
	 *
	 * GetValue - gets the value
	 *
	 ****************************************************************************/
	ValueType   GetValue(void) { return m_pValue; };

protected:
	char* m_strKey;    /* the "index" */
	ValueType    m_pValue;    /* the associated value */

	//char        m_strDebugKey[256];
};

#endif  /* _Association_H_ */

