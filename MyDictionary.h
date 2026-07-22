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
  * MyDictionary - set of associations of two objects
  *
  ****************************************************************************/

#ifndef _MyDictionary_H_
#define _MyDictionary_H_

#include "CollectionsAbstract.h"
#include "Association.h"
#include <string.h>



  /* should this be a void pointer?  No, the template helps
   * catch errors by differentiating between the data I store in them
   *
   * I would have used the wrong MyDictionary many times if it weren't for this.
   * I doubt it's a good idea to interpret a pointer to a float as a atring...
   */





   /****************************************************************************
	*
	* Set
	*
	****************************************************************************/
template<class DataType>
class Set : public Collection<DataType, NULL>
{
protected:
	//    DataType    *m_pArray;
public:
	Set() {};
	virtual     ~Set() {};



protected:


	/****************************************************************************
	 *
	 * MoveContents
	 *
	 * when it becomes necessary to resize the array,
	 * this is how the old data gets moved to the new array
	 *
	 ****************************************************************************/
	virtual error_t MoveContents(DataType* pNewArray,
		DataType* pOldArray,
		const DWORD dwNewSize)
	{
		DWORD   i;

		/*
		 * copy all the existing elements to the new array
		 */
		for (i = 0; i < Capacity(); i++)
			pNewArray[i] = pOldArray[i];

		/*
		 * NULL the remaining elements
		 */
		for (i = Capacity(); i < dwNewSize; i++)
			pNewArray[i] = NULL;


		return SUCCESS;
	};



	/****************************************************************************
	 *
	 * IndexOf - default, should be overridden by derived classes
	 *
	 * determine where inValue would be stored if it were in the array
	 *
	 * return SUCCESS if it is found
	 *
	 * return ERR_NOTFOUND if it does not exist
	 *
	 * if outIndex == NULL,
	 *        then return value of SUCCESS means inValue is in the Collection
	 *        return value != SUCCESS means inValue is not in the Collection
	 *
	 ****************************************************************************/
	 /*    virtual error_t    IndexOf( DataType inValue, DWORD *outIndex )
		 {
			 error_t    err;
			 DWORD    i;

			 for( i = 0; i < Capacity(); i++ )
				 if( m_pArray[i] == inValue )
				 {
					 if( outIndex != NULL )
						 *outIndex = i;
					 return SUCCESS;
				 }

			 err = IndexOf( NULL, outIndex );
			 if( err == SUCCESS )
				 return ERR_NOTFOUND;
			 else
				 return err;
		 };/**/

};




/****************************************************************************
 *
 * MyDictionary
 *
 * I represent a set of elements that can be viewed
 * from one of two perspectives: a set of associations,
 * or a set of values that are externally named where
 * the name can be any object that responds to =.
 * The external name is referred to as the key.
 *
 * hmmm... you cannot remove vars... oops!
 *
 ****************************************************************************/
template<class DataType>
class MyDictionary : public Set< Association<DataType>* >
{
protected:
	DWORD                   m_dwAliasCount;
	MyDictionary<char*>* m_pAlias;
	MyDictionary<DataType>* m_pAlternate;
	bool                    m_bCaseSensitive;  /* comparisons are case sensitive */

	typedef    Association<DataType>* data_t;

public:
	MyDictionary()
	{
		m_bCaseSensitive = false;
		m_bDeleteElements = true;
		m_pAlias = NULL;
		m_pAlternate = NULL;
		m_dwAliasCount = 0;
	};
	virtual ~MyDictionary()
	{
	};


	/****************************************************************************
	 *
	 * SetAlias - set the alias lookup MyDictionary
	 *
	 ****************************************************************************/
	void    SetAlias(MyDictionary<char*>* pAlias)
	{
		m_pAlias = pAlias;
	};

	/****************************************************************************
	 *
	 * SetAlternate - set the alternate lookup MyDictionary
	 *
	 ****************************************************************************/
	void    SetAlternate(MyDictionary<DataType>* pAlternate)
	{
		m_pAlternate = pAlternate;
	};

	/****************************************************************************
	 *
	 * Import - import the contents of another MyDictionary
	 *
	 ****************************************************************************/
	error_t    Import(MyDictionary<DataType>& srcMyDictionary)
	{
		error_t    err;
		DWORD    i;
		DWORD    dwNumElements;
		Association<DataType>** pArray;

		err = srcMyDictionary.AsArray(&pArray, &dwNumElements);
		if (err != SUCCESS)
			return err;


		for (i = 0; i < dwNumElements; i++)
		{
			err = this->SetValue(pArray[i]->GetKey(), pArray[i]->GetValue());
			if (err != SUCCESS)
				break;
		}

		delete[] pArray;


		if (err != SUCCESS)
			return err;

		//            m_bCaseSensitive = srcMyDictionary->m_bCaseSensitive;
		m_bCaseSensitive = false;
		m_bDeleteElements = true;

		return SUCCESS;
	};



	/****************************************************************************
	 *
	 * SetValue - add a value to the MyDictionary
	 *
	 ****************************************************************************/
	error_t        SetValue(const char* inKey, DataType inValue)
	{
		error_t    err;
		Association<DataType>* pTmp;


		if (inKey == NULL)
			return ERR_NULL;

		if ((pTmp = new Association<DataType>) == NULL)
			return ERR_MALLOC;

		err = pTmp->Initialize(inKey, inValue);
		if (err == SUCCESS)
		{
			err = Add(pTmp);
			if (err != SUCCESS)
				delete pTmp;
		}

		return err;
	};


	/****************************************************************************
	 *
	 * RemoveValue - remove a value from the MyDictionary
	 *
	 ****************************************************************************/
	error_t        RemoveValue(const char* inKey)
	{
		DWORD    i;

		if (inKey == NULL)
			return ERR_NULL;

		for (i = 0; i < Capacity(); i++)
		{
			if (m_pArray[i] != NULL
				&& StringsMatch(inKey, m_pArray[i]->GetKey()))
			{
				return Remove(m_pArray[i]);
			}
		}

		return SUCCESS; /* if not found, it was "successfully" removed */
	};

	/****************************************************************************
	 *
	 * GetValue - find a value stored in the MyDictionary
	 *
	 ****************************************************************************/
	DataType    GetValue(const char* inKey,
		DataType returnValueIfNotFound = NULL)
	{
		m_dwAliasCount = 0;
		return GetValue_Helper(inKey, returnValueIfNotFound);
	};

	DataType    GetValue_Helper(const char* inKey,
		DataType returnValueIfNotFound = NULL)
	{
		DWORD    i;
		DataType dtTemp = returnValueIfNotFound;

		/*
		 * if for some reason an alias chain is circular,
		 * stop the infinite recursion at 32
		 */
		if (inKey == NULL || m_dwAliasCount > 32)
			return returnValueIfNotFound;

		for (i = 0; i < Capacity(); i++)
		{
			if (m_pArray[i] != NULL
				&& StringsMatch(inKey, m_pArray[i]->GetKey()))
				return m_pArray[i]->GetValue();
		}

		if (m_pAlias != NULL)
		{
			m_dwAliasCount++;
			dtTemp = GetValue(m_pAlias->GetValue_Helper(inKey), returnValueIfNotFound);
			m_dwAliasCount++;
		}

		if (dtTemp == returnValueIfNotFound
			&& m_dwAliasCount <= 33 /* keep it from checking 32 times as the recursion unwinds */
			&& m_pAlternate != NULL)
			dtTemp = m_pAlternate->GetValue(inKey, returnValueIfNotFound);

		return dtTemp;
	};

#ifdef UNDEFINED
	/****************************************************************************
	 *
	 * GetValue - find a value stored in the MyDictionary
	 *
	 ****************************************************************************/
	error_t        GetValue(const char* inKey, DataType* outValue)
	{
		if (outValue == NULL)
			return ERR_NULL;

		if ((*outValue = GetValue(inKey)) != NULL)
			return SUCCESS;
		else
		{
#if EXTREME_DEBUGGING
			DumpToFile("error.txt", (char*)inKey, "<--?!\n");
#endif
			return ERR_NOTFOUND;
		}
	};
#endif

protected:
	/****************************************************************************
	 *
	 * StringsMatch - determine if two strings are equal
	 *
	 ****************************************************************************/
	bool        StringsMatch(const char* inStr1, const char* inStr2)
	{
		if (m_bCaseSensitive)
			return (strcmp(inStr1, inStr2) == 0) ? true : false;
		else
			return (_stricmp(inStr1, inStr2) == 0) ? true : false;
	};


	/****************************************************************************
	 *
	 * Compare - determine if two DataTypes are equal
	 *
	 ****************************************************************************/
	virtual bool    Compare(data_t inValue1, data_t inValue2)
	{
		if (IsValid(inValue1) && IsValid(inValue2))
			return StringsMatch(inValue1->GetKey(), inValue2->GetKey());
		else
			return inValue1 == inValue2;  //***************/ IS THIS RIGHT?
	};


	/****************************************************************************
	 *
	 * MoveContents
	 *
	 * when it becomes necessary to resize the array,
	 * this is how the old data gets moved to the new array
	 *
	 ****************************************************************************/
	virtual error_t    MoveContents(data_t* pNewArray,
		data_t* pOldArray,
		const DWORD dwNewSize)
	{
		DWORD    i;

		/*
		 * copy all the existing elements to the new array
		 */
		for (i = 0; i < Capacity(); i++)
			pNewArray[i] = pOldArray[i];

		/*
		 * NULL the remaining elements
		 */
		for (i = Capacity(); i < dwNewSize; i++)
			pNewArray[i] = NULL;


		return SUCCESS;
	};



	/****************************************************************************
	 *
	 * IndexOf - default, should be overridden by derived classes
	 *
	 * determine where inValue would be stored if it were in the array
	 *
	 * return SUCCESS if it is found
	 *
	 * return ERR_NOTFOUND if it does not exist
	 *
	 * if outIndex == NULL,
	 *        then return value of SUCCESS means inValue is in the Collection
	 *        return value != SUCCESS means inValue is not in the Collection
	 *
	 ****************************************************************************/
	 /*    virtual error_t    IndexOf( data_t inValue, DWORD *outIndex )
		 {
			 error_t    err;
			 DWORD    i;

			 for( i = 0; i < Capacity(); i++ )
				 if( m_pArray[i] == inValue )
				 {
					 if( outIndex != NULL )
						 *outIndex = i;
					 return SUCCESS;
				 }

			 err = IndexOf( NULL, outIndex );
			 if( err == SUCCESS )
				 return ERR_NOTFOUND;
			 else
				 return err;
		 };/**/


public:
#if EXTREME_DEBUGGING
	/****************************************************************************
	 *
	 * DebugDumpContents - quick way to see what's in the MyDictionary
	 *
	 ****************************************************************************/
	void        DebugDumpContents(const char* strFile)
	{
		unsigned int i;

		for (i = 0; i < Capacity(); i++)
		{
			if (m_pArray[i] != NULL)
			{
				DumpToFile(strFile, m_pArray[i]->GetKey(), "\n");
				DumpToFile(strFile, (DataType)m_pArray[i]->GetValue(), "\n\n");
			}
		}
	};
#endif

};












/****************************************************************************
 *
 * FiniteSet - has a predetermined size
 *
 ****************************************************************************/
template<class DataType, DWORD MaximumSize>
//class FiniteSet : public ArrayedCollection<DataType, FiniteSize>, public Set<DataType>
class FiniteSet : public Set<DataType>
{
protected:
	DataType    m_pData[MaximumSize];


public:
	FiniteSet()
	{
		DWORD    i;

		for (i = 0; i < MaximumSize; i++)
			m_pData[i] = NULL;

		m_bGrowable = false;
		m_dwCapacity = MaximumSize;
		m_pArray = m_pData;
	};

	virtual         ~FiniteSet()
	{
	};

	/*
	 * if less than MaximumSize, restrict the finite set to the new size,
	 * otherwise use MaximumSize
	 */
	void            RestrictSize(const DWORD dwSize)
	{
		DWORD    i;

		m_dwCapacity = min(MaximumSize, dwSize);
		//        m_dwCapacity = 1;

		for (i = 0; i < MaximumSize; i++)
			m_pData[i] = NULL;

		m_dwNumElements = 0;
	};


protected:
	virtual error_t FullCheck(void)
	{
		DWORD    i;

		if (Size() < Capacity() || Capacity() == 0)
			return SUCCESS;

		Remove(m_pData[0]);

		for (i = 0; i < Capacity() - 1; i++)
		{
			while (m_pData[i] == NULL)
				ShiftLeft(&m_pData[i], Capacity() - i);
		}

		m_pData[Capacity() - 1] = NULL;
		return SUCCESS;
	};

	void    ShiftLeft(DataType* inoutArray, DWORD inLength)
	{
		DWORD    i;

		for (i = 1; i < inLength; i++)
			inoutArray[i - 1] = inoutArray[i];
	};



};


/****************************************************************************
 *
 * OrderedFiniteSet - imposes a limit on the size of a Set
 *
 ****************************************************************************/
 /*template<class DataType, DWORD dwMaxSize>
 class OrderedSet : public Set<DataType>
 {
 protected:
	 DWORD    m_dwQueueLength;

 public:
				 OrderedSet()
	 {
		 m_bDeleteEntries = false;
	 };

	 virtual        ~OrderedSet()
	 {
	 };



 protected:




 };


 /**/





#endif    /* _MyDictionary_H_ */

