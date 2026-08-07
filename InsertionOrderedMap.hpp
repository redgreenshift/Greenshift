#pragma once
#include "Project Greenshift.h"
/*
 *  Copyright (C) 2026 Jared Ivey
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

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>


/****************************************************************************
 *
 * LinearMap
 *
 ****************************************************************************/
 /****************************************************************************
  *
  * LinearMap
  *
  * I represent a set of elements that can be viewed as
  * 1) a map from Keys to a Values, or
  * 2) a list of Key/Value pairs in the order they were added.
  *
  * NOW you can remove vars
  *
  ****************************************************************************/
template<class KeyType, class DataType>
class LinearMap
{
	typedef int SequenceType;
	typedef LinearMap<KeyType, DataType /*, DefaultValue, DefaultKey*/ > this_t;
	std::unordered_map<KeyType, std::tuple<DataType, SequenceType> > m_map;
	SequenceType m_sequence = 0;

public:
	error_t Add(const KeyType& key, const DataType& data)
	{
		m_map.emplace(key, std::make_tuple(data, m_sequence++));

		return SUCCESS;
	}

protected:
	DWORD							m_dwAliasCount = 0;
	LinearMap<KeyType, KeyType>*	m_mapAlias = nullptr;
	LinearMap<KeyType, DataType>*	m_mapAlternate = nullptr;
	bool							m_bCaseSensitive = false;  /* comparisons are case sensitive */

	//typedef    Association<DataType>* data_t;

public:
	LinearMap()
	{
		m_mapAlias = nullptr;
		m_mapAlternate = nullptr;
		m_bCaseSensitive = false;
		m_dwAliasCount = 0;
	};
	virtual ~LinearMap()
	{
	};


	/****************************************************************************
	 *
	 * SetAlias - set the alias lookup MyDictionary
	 *
	 ****************************************************************************/
	void    SetAlias(this_t * mapAlias)
	{
		m_mapAlias = mapAlias;
	};

	/****************************************************************************
	 *
	 * SetAlternate - set the alternate lookup MyDictionary
	 *
	 ****************************************************************************/
	void    SetAlternate(this_t * mapAlternate)
	{
		m_mapAlternate = mapAlternate;
	};

	/****************************************************************************
	 *
	 * Import - import the contents of another MyDictionary
	 *
	 ****************************************************************************/
	error_t    Import(this_t& srcMyDictionary)
	{
		error_t    err;
		DWORD    i;
		std::vector < std::tuple<KeyType, DataType> > pArray;

		err = srcMyDictionary.AsArray(pArray);
		if (err != SUCCESS)
			return err;


		DWORD    dwNumElements = pArray.size();

		for (i = 0; i < dwNumElements; i++)
		{
			err = this->SetValue(std::get<0>(pArray[i]), std::get<1>(pArray[i]));
			if (err != SUCCESS)
				break;
		}

		if (err != SUCCESS)
			return err;

		//m_bCaseSensitive = srcMyDictionary->m_bCaseSensitive;
		m_bCaseSensitive = false;

		return SUCCESS;
	};



	/****************************************************************************
	 *
	 * SetValue - add a value to the MyDictionary
	 *
	 ****************************************************************************/
	error_t        SetValue(const KeyType & inKey, const DataType & inValue)
	{
		error_t    err;

		err = Add(inKey, inValue);

		return err;
	};


	size_t Size()
	{
		return m_map.size();
	}


	/****************************************************************************
	 *
	 * RemoveValue - remove a value from the MyDictionary
	 *
	 ****************************************************************************/
	error_t        RemoveValue(const KeyType & inKey)
	{
		auto it = m_map.find(inKey);
		if (it != m_map.cend())
			m_map.erase(it);

		return SUCCESS; /* if not found, it was "successfully" removed */
	};

	/****************************************************************************
	 *
	 * GetValue - find a value stored in the MyDictionary
	 *
	 ****************************************************************************/
	std::optional<DataType> GetValue(const KeyType& inKey,
		std::optional<DataType> returnValueIfNotFound = std::nullopt)
	{
		m_dwAliasCount = 0;
		return GetValue_Helper(inKey, returnValueIfNotFound);
	};

	std::optional<DataType> GetValue_Helper(const KeyType& inKey,
		std::optional<DataType> returnValueIfNotFound = std::nullopt)
	{
		std::optional<DataType> dtTemp = returnValueIfNotFound;

		/*
		 * if for some reason an alias chain is circular,
		 * stop the infinite recursion at 32
		 */
		if (m_dwAliasCount > 32)
			return dtTemp;

		auto it = m_map.find(inKey);
		if (it != m_map.cend())
		{
			return std::get<0>(it->second);
		}

		if (m_mapAlias != nullptr)
		{
			m_dwAliasCount++;
			auto valAlias = m_mapAlias->GetValue_Helper(inKey);
			if (valAlias.has_value())
				dtTemp = GetValue_Helper(valAlias.value(), returnValueIfNotFound);
			m_dwAliasCount++;
		}

		if (!dtTemp.has_value()
			&& m_dwAliasCount <= 33 /* keep it from checking 32 times as the recursion unwinds */
			&& m_mapAlternate != NULL)
			dtTemp = m_mapAlternate->GetValue_Helper(inKey, returnValueIfNotFound);

		return dtTemp;
	};




public:
	/****************************************************************************
	 *
	 * AsArray - create an array representing my contents, and return the size
	 *
	 ****************************************************************************/
	virtual error_t AsArray(std::vector < std::tuple<KeyType, DataType> > & outArray)
	{
		std::vector < std::tuple<KeyType, DataType> > retVal;

		const DWORD size = m_map.size();

		//retVal.reserve(size);
		retVal.resize(size);

		/*
		 * create the array to copy the contents
		 */
		std::map< SequenceType, std::tuple<KeyType, DataType> > sequenceMap;

		for (auto const & kvPair : m_map)
		{
			const KeyType& key = kvPair.first;
			const std::tuple<DataType, SequenceType>& value = kvPair.second;
			const DataType& data = std::get<0>(value);
			SequenceType sequence = std::get<1>(value);

			sequenceMap.emplace(sequence, std::make_tuple(key, data));
		}

		int index = 0;
		for (auto& kvPair : sequenceMap)
		{
			std::tuple<KeyType, DataType>& value = kvPair.second;

			retVal[index++] = std::move(value);
		}

		if (index != size)
			return FAILURE;

		outArray = std::move(retVal);
		return SUCCESS;
	}

	virtual void    WipeContents(void)
	{
		/*
		 * if should free the elements, do so before delete[]'ing the array
		 */
		m_map.clear();
	}
};

#pragma pop_macro("max")
#pragma pop_macro("min")

