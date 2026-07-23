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
  * Collection - ABSTRACT
  *
  * I am a generic Collection of things.  The "things" are currently assumed
  * to be pointers.  I know collection should be generic enough to not make
  * this assumption.
  *
  ****************************************************************************/

#ifndef _CollectionsAbstract_H_
#define _CollectionsAbstract_H_

#include "Association.h"

  /****************************************************************************
   ****************************************************************************
   ****************************************************************************
   *
   * Collection - ABSTRACT
   *
   * I am a generic Collection of things.  The "things" are currently assumed
   * to be pointers.  I know collection should be generic enough to not make
   * this assumption.
   *
   ****************************************************************************
   ****************************************************************************
   ****************************************************************************/
template<class DataType, DataType InvalidValue>
class Collection
{
protected:
	DWORD           m_dwNumElements;   /* number of elements stored */
	DWORD           m_dwCapacity;      /* num of allocated spaces in array */
	DataType*		m_pArray;          /* data stored in the Collection */
	bool            m_bGrowable;       /* true if m_pArray may be resized,
										* false if has a fixed size */
	bool            m_bDeleteElements; /* if true, the elements in the array
										* are delete'd when Collection is
										* deleted
										*/


	inline DWORD    Capacity(void) { return m_dwCapacity; };


public:
	/****************************************************************************
	 *
	 * Collection - default constructor
	 *
	 ****************************************************************************/
	Collection()
	{
		m_dwNumElements = 0;
		m_dwCapacity = 0;
		m_pArray = NULL;
		m_bDeleteElements = false;
		m_bGrowable = true;
	};
	/****************************************************************************
	 *
	 * ~Collection - destructor
	 *
	 ****************************************************************************/
	virtual ~Collection()
	{
		WipeContents();

		if (m_bGrowable)
			SAFE_DELETE_ARRAY(m_pArray); /* release the array */
	};
#ifdef UNDEFINED
	/****************************************************************************
	 *
	 * DeepCopy - make a copy of a MyDictionary, including all Associations
	 *
	 ****************************************************************************/
	friend error_t  DeepCopy(Collection<DataType, InvalidValue>* destCollection,
		Collection<DataType, InvalidValue>* srcCollection)
	{
		error_t  err = SUCCESS;
		DWORD    i;

		destCollection->WipeContents();

		destCollection->m_dwNumElements = srcCollection->m_dwNumElements;
		destCollection->m_dwCapacity = srcCollection->m_dwCapacity;
		destCollection->m_bGrowable = srcCollection->m_bGrowable;
		destCollection->m_bDeleteElements = srcCollection->m_bDeleteElements;

		destCollection->m_pArray = new DataType[destCollection->m_dwCapacity];
		if (destCollection->m_pArray == NULL)
			return ERR_MALLOC;

		for (i = 0; i < destCollection->m_dwCapacity; i++)
		{
			if (srcCollection->m_pArray[i] != NULL)
			{
				err = DeepCopyAssociation(destCollection->m_pArray[i],
					srcCollection->m_pArray[i]);
				if (err != SUCCESS)
					return err;
			}
			else
				destCollection->m_pArray[i] = NULL;
		}

		return SUCCESS;
	};
#endif

	/****************************************************************************
	 *
	 * WipeContents - free all the elements
	 *
	 ****************************************************************************/
	virtual void    WipeContents(void)
	{
		DWORD    i;

		/*
		 * if should free the elements, do so before delete[]'ing the array
		 */
		if (m_bDeleteElements)
		{
			for (i = 0; i < Capacity(); i++)
			{
				if (m_pArray[i] != InvalidValue)
				{
					delete m_pArray[i];
					m_pArray[i] = InvalidValue;
				}
			}
		}

		m_dwNumElements = 0;
	};


	/****************************************************************************
	 *
	 * Size - return the number of elements stored in Collection
	 *
	 ****************************************************************************/
	virtual DWORD   Size(void)
	{
		return m_dwNumElements;
	};

	/****************************************************************************
	 *
	 * Includes - determine whether Collection contains inValue
	 *
	 ****************************************************************************/
	virtual bool    Includes(DataType inValue)
	{
		return (IndexOf(inValue, NULL) == SUCCESS) ? true : false;
	};

	/****************************************************************************
	 *
	 * Add - add a value to the collection
	 *
	 ****************************************************************************/
	virtual error_t Add(DataType inValue)
	{
		error_t err;
		DWORD   i = -1;

		if (!IsValid(inValue))
			return ERR_NULL;

		/*
		 * make sure there's enough space to add pValue
		 */
		if ((err = FullCheck()) != SUCCESS)
			return err;


		/*
		 * find a suitable index
		 */
		err = IndexOf(inValue, &i);
		if (err == ERR_NOTFOUND && i < Capacity())
		{
			m_pArray[i] = inValue;
			m_dwNumElements++;
			return SUCCESS;
		}
		else
			if (err == SUCCESS && i < Capacity())
			{
				m_pArray[i] = inValue;
			}

		return err;
	};


	/****************************************************************************
	 *
	 * Remove - remove a value from the collection
	 *
	 ****************************************************************************/
	virtual error_t Remove(DataType inValue)
	{
		error_t err;
		DWORD   i;

		/*
		 * verify inValue is something that could be stored in collection
		 */
		if (!IsValid(inValue))
			return ERR_NULL;

		/*
		 * find where inValue is stored
		 */
		err = IndexOf(inValue, &i);
		if (err == SUCCESS && i < Capacity())
		{
			if (m_bDeleteElements)
			{
				SAFE_DELETE(m_pArray[i]);
			}
			else
				m_pArray[i] = NULL;

			m_dwNumElements--;
		}

		return err;
	};


protected:

	/****************************************************************************
	 *
	 * IsValid - determine if inValue is valid as an entry in the collection
	 *
	 ****************************************************************************/
	virtual bool    IsValid(DataType inValue)
	{
		/*
		 * I know this means collection assumes its DataType is a pointer
		 * and this shouldn't be here, but I need to check for NULL
		 */
		if (inValue != InvalidValue)
			return true;
		else
			return false;
	};


	/****************************************************************************
	 *
	 * Compare - compare two DataType's
	 *
	 ****************************************************************************/
	virtual bool    Compare(DataType inValue1, DataType inValue2)
	{
		return inValue1 == inValue2;
	};


	/****************************************************************************
	 *
	 * GrowSize - return the amount the collection should grow when necessary to
	 *                increase the size of the internal array
	 *
	 ****************************************************************************/
	virtual DWORD   GrowSize(void)
	{
		return max(Capacity(), 2);
	};

	/****************************************************************************
	 *
	 * FullCheck - if running low on space, make the internal array larger
	 *
	 ****************************************************************************/
	virtual error_t FullCheck(void)
	{
		if (ShouldGrow())
			return Grow();
		else
			return SUCCESS;
	};

	/****************************************************************************
	 *
	 * ShouldGrow - determine if the collection needs more space, and should
	 *                increase the allocation size for the internal array,
	 *                considering the amount of free space left.
	 *
	 ****************************************************************************/
	virtual bool    ShouldGrow(void)
	{
		/*
		 * if the unused space is smaller than 1/4 the total space available
		 */
		if (Capacity() - Size() < Capacity() / 4 + 1)
			return true;
		else
			return false;
	};


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


	/************************************************************************
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
	 ************************************************************************/
	virtual error_t IndexOf(DataType inValue, DWORD* outIndex)
	{
		error_t err;
		DWORD   i;


		for (i = 0; i < Capacity(); i++)
			if (Compare(m_pArray[i], inValue))
			{
				if (outIndex != NULL)
					*outIndex = i;
				return SUCCESS;
			}

		/*
		 * keep from recursing forever
		 */
		if (Size() < Capacity() && !Compare(inValue, InvalidValue))
		{
			err = IndexOf(InvalidValue, outIndex);
			if (err == SUCCESS)
				return ERR_NOTFOUND;
			else
				return err;
		}

		return ERR_NOTFOUND;
	};


	/****************************************************************************
	 *
	 * Grow - increase the size of the internal array
	 *
	 ****************************************************************************/
	virtual error_t Grow(const DWORD dwGrowAtLeast = 0)
	{
		error_t     err;
		DWORD       i;
		DWORD       dwNewSize;
		DataType* tmpArray;

		dwNewSize = Capacity() + max(GrowSize(), dwGrowAtLeast);

		/*
		 * allocate a new larger array
		 */
		tmpArray = new DataType[dwNewSize];
		if (tmpArray == NULL)
			return ERR_MALLOC;

		/*
		 * NULL the array elements
		 */
		for (i = 0; i < dwNewSize; i++)
			tmpArray[i] = NULL;


		/*
		 * move the old data to the new array
		 */
		err = MoveContents(tmpArray, m_pArray, dwNewSize);
		if (err == SUCCESS)
		{
			SAFE_DELETE_ARRAY(m_pArray);  /* delete the old array */
			m_pArray = tmpArray;            /* update the new array */
			tmpArray = NULL;                /* unnecessary safety measure */

			m_dwCapacity = dwNewSize;        /* update the new array size */
		}
		else
			SAFE_DELETE_ARRAY(tmpArray);    /* error moving data, free the new
											 * leave the old data alone
											 */
		return err;
	};


public:
	/****************************************************************************
	 *
	 * AsArray - create an array representing my contents, and return the size
	 *
	 ****************************************************************************/
	virtual error_t AsArray(DataType** outArray, DWORD* outNumElements)
	{
		DWORD   i;
		DWORD   dwElement;

		if (outArray == NULL || outNumElements == NULL)
			return ERR_NULL;

		/*
		 * create the array to copy the contents
		 */
		if (((*outArray) = new DataType[Size()]) == NULL)
			return ERR_MALLOC;

		dwElement = 0;
		for (i = 0; i < Capacity(); i++)
		{
			if (m_pArray[i] != NULL)
				(*outArray)[dwElement++] = m_pArray[i];
		}

		*outNumElements = Size();

		return SUCCESS;
	};

};




/*
 * SequenceableCollection
 *
 * I am an abstract superclass for collections that have a well-defined order
 * associated with their elements. Thus each element is externally-named by
 * integers referred to as indices.
 */
 /*
 template<class DataType>
 class SequenceableCollection : public Collection<DataType, NULL>
 {
 public:
	 SequenceableCollection()
	 {
	 };


	 virtual    ~SequenceableCollection()
	 {
	 };

 };




 /*
  * ArrayedCollection
  *
  * I am an abstract collection of elements with a fixed range of integers
  * (from 1 to n>=1) as external keys.
  */
  /*
  template<class DataType, DWORD ArraySize>
  class ArrayedCollection : public SequenceableCollection<DataType>
  {
  protected:
  //    DataType    m_pArray;

	  public:
		  ArrayedCollection() : m_bGrowable(false)
		  {
		  };

		  virtual    ~ArrayedCollection()
		  {
		  };
  };






  /*
  template<class DataType>
  class OrderedCollection : public Collection<DataType, NULL>
  {
  public:
				  OrderedCollection() {};
	  virtual        ~OrderedCollection() {};



  };
  */





#ifdef UNDEFINED

template<class DataType, DataType InvalidValue>
class Collection
{
public:
	Collection()
	{
	};
	virtual    ~Collection()
	{
	};

protected:
	virtual    DWORD        Capacity(void) = 0;

public:
	virtual DWORD        Size(void) { return m_dwNumElements; };
	virtual bool        Includes(DataType inValue) = 0;
	virtual error_t        Add(DataType inValue) = 0;
	virtual error_t        Remove(DataType inValue) = 0;

	virtual void        WipeContents(void);

	virtual DWORD    AsArray(DataType** outArray) = 0;

protected:
	//    virtual bool    IsValid( DataType inValue );
	virtual bool    Compare(DataType inValue1, DataType inValue2)
	{
		return inValue1 == inValue2;
	};
	//    virtual    DWORD    GrowSize(void);
	//    virtual error_t    FullCheck( void );
	//    virtual bool    ShouldGrow( void );
	//    virtual error_t    MoveContents( DataType *pNewArray,
	//                                  DataType *pOldArray,
	//                                  const DWORD dwNewSize );
	//    virtual error_t    IndexOf( DataType inValue, DWORD *outIndex );
	//    virtual error_t    Grow( void );
};
#endif



#endif  /* _CollectionsAbstract_H_ */

