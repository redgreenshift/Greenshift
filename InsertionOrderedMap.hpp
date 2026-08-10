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

#include "CollectionsAbstract.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#include <map>
#include <optional>
#include <string>
#include <vector>


struct CaseInsensitiveLess {
	bool operator()(const std::string& a, const std::string& b) const {
		size_t i = 0;
		size_t n = std::min(a.size(), b.size());

		for (; i < n; ++i) {
			unsigned char ca = static_cast<unsigned char>(a[i]);
			unsigned char cb = static_cast<unsigned char>(b[i]);
			ca = static_cast<unsigned char>(std::tolower(ca));
			cb = static_cast<unsigned char>(std::tolower(cb));
			if (ca != cb) return ca < cb;
		}
		return a.size() < b.size();
	}
};

/****************************************************************************
 *
 * InsertionOrderedMap
 *
 ****************************************************************************/
 /****************************************************************************
  *
  * InsertionOrderedMap
  *
  * I represent a set of elements that can be viewed as
  * 1) a map from Keys to a Values, or
  * 2) a list of Key/Value pairs in the order they were added.
  *
  * NOW you can remove vars
  *
  ****************************************************************************/
/**
 * @brief Key/value container that preserves insertion order for ordered export.
 *
 * Stores unique keys and associated values addressable by key. The container does
 * not provide native iteration; callers observe the insertion order only through
 * AsArray(...), which outputs key/value pairs in the order the keys were originally
 * inserted (FIFO by insertions).
 *
 * Duplicate keys are not allowed:
 * - Reinserting an existing key updates the stored value but must not change the
 *   key's position in the exported insertion order.
 * - If a key is removed and later reinserted, it will appear at the end of the
 *   exported insertion order.
 *
 * @tparam KeyType         Key type.
 * @tparam DataType        Value type.
 * @tparam LessThanCompare Comparison functor used to define key ordering
 *                          (e.g., CaseInsensitiveLess).
 */
template<class KeyType, class DataType, class LessThanCompare = CaseInsensitiveLess>
class InsertionOrderedMap : public ICollectionContract<std::tuple<KeyType, DataType> >
{
	typedef int SequenceType;
	typedef std::tuple<KeyType, DataType> KeyValueTuple_t;
	typedef InsertionOrderedMap<KeyType, DataType > this_t;
	std::map<KeyType, std::tuple<DataType, SequenceType>, LessThanCompare > m_map;
	SequenceType m_sequence = 0;

public:
	/**
	 * @brief Inserts or updates a key-value pair while preserving insertion order.
	 *
	 * For existing keys, only the value is updated; the original position in the
	 * sequence remains unchanged. New keys are appended to the end of the order.
	 *
	 * @param inKey - The key to add/update.
	 * @param inData - The data to be stored.
	 * @return error_t: SUCCESS upon successful insertion or update.
	 */
	error_t Add(const KeyType& key, const DataType& data)
	{
		auto const it = m_map.find(key);
		if (it != m_map.cend())
		{
			m_map.insert_or_assign(key, std::make_tuple(data, std::get<1>(it->second)));
		}
		else
		{
			m_map.emplace(key, std::make_tuple(data, m_sequence++));
		}

		return SUCCESS;
	}

	virtual error_t Add(const KeyValueTuple_t& kv) override
	{
		return Add(std::get<0>(kv), std::get<1>(kv));
	}

	virtual bool Contains(const KeyValueTuple_t& kv) const override
	{
		// "Contains" for a map specifically means the KEY exists with any value
		return Contains(std::get<0>(kv));
	}

	virtual error_t Remove(const KeyValueTuple_t& kv) override
	{
		return RemoveValue(std::get<0>(kv));
	}

	bool Contains(const KeyType& key) const
	{
		return m_map.contains(key);
	}


protected:
	DWORD							m_dwAliasCount = 0;
	InsertionOrderedMap<KeyType, KeyType>*	m_mapAlias = nullptr;
	InsertionOrderedMap<KeyType, DataType>*	m_mapAlternate = nullptr;

public:
	InsertionOrderedMap()
	{
		m_mapAlias = nullptr;
		m_mapAlternate = nullptr;
		m_dwAliasCount = 0;
	};
	virtual ~InsertionOrderedMap() override
	{
	};


	/**
	 * @brief Set the alias lookup collection
	 *
	 * @param mapAlias - collection containing mappings
	 */
	void SetAlias(this_t * mapAlias)
	{
		m_mapAlias = mapAlias;
	};

	/**
	 * @brief Set the alternate lookup collection
	 *
	 * @param mapAlternate - alternate collection to search when
	 * failing to find the value in "this" collection.
	 */
	void SetAlternate(this_t * mapAlternate)
	{
		m_mapAlternate = mapAlternate;
	};

	/**
	 * @brief import the contents of another collection
	 *
	 * @param src - the source collection
	 * @return error_t: SUCCESS upon successful import; otherwise failure.
	 */
	error_t Import(this_t& src)
	{
		error_t    err;
		DWORD    i;
		std::vector < std::tuple<KeyType, DataType> > pArray;

		err = src.AsArray(pArray);
		if (err != SUCCESS)
			return err;


		DWORD dwNumElements = pArray.size();

		for (i = 0; i < dwNumElements; i++)
		{
			err = this->SetValue(std::get<0>(pArray[i]), std::get<1>(pArray[i]));
			if (err != SUCCESS)
				break;
		}

		if (err != SUCCESS)
			return err;

		return SUCCESS;
	};



	/**
	 * @brief add a value to the collection, or update an existing associated
	 * value for a key
	 *
	 * @param inKey - The key to add/update.
	 * @param inValue - The data to be stored.
	 * @return error_t: SUCCESS upon successful insertion or update.
	 */
	error_t SetValue(const KeyType & inKey, const DataType & inValue)
	{
		return Add(inKey, inValue);
	}

	virtual size_t Size() const override
	{
		return m_map.size();
	}

	virtual size_t Capacity() const override
	{
		return m_map.max_size();
	}

	virtual bool Empty() const override
	{
		return m_map.empty();
	}

	virtual void Clear(void) override
	{
		m_map.clear();
	}

	/**
	 * @brief remove a value from the collection by key. If the key does
	 * not exist, it is considered a success.
	 *
	 * @param inKey - the key to remove
	 * @return error_t: SUCCESS upon finding and deleting the value; ERR_NOTFOUND otherwise.
	 */
	error_t RemoveValue(const KeyType & inKey)
	{
		auto it = m_map.find(inKey);
		if (it != m_map.cend())
		{
			m_map.erase(it);
			return SUCCESS;
		}

		return ERR_NOTFOUND;
	}

	/**
	 * @brief find a value stored in the collection by key. If the key does
	 * not exist, return the specified default value (std::nullopt if unspecified).
	 *
	 * @param inKey - the key identifying the value to retrieve.
	 * @param returnValueIfNotFound - default value to return if key is not found.
	 * @return std::optional<DataType>: the associated value if found;
	 * returnValueIfNotFound/std::nullopt otherwise.
	 */
	std::optional<DataType> GetValue(const KeyType& inKey,
		std::optional<DataType> returnValueIfNotFound = std::nullopt)
	{
		m_dwAliasCount = 0;
		return GetValue_Helper(inKey, returnValueIfNotFound);
	};

protected:
	/**
	 * @brief find a value stored in the collection by key. If the key does
	 * not exist, return the specified default value (std::nullopt if unspecified).
	 *
	 * @param inKey - the key identifying the value to retrieve.
	 * @param returnValueIfNotFound - default value to return if key is not found.
	 * @return std::optional<DataType>: the associated value if found;
	 * returnValueIfNotFound/std::nullopt otherwise.
	 */
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
			m_dwAliasCount++; // REVIEW: legacy behavior; we're incrementing twice,
							  // net effect is we support 16 hops, which is enough.
		}

		if (!dtTemp.has_value()
			&& m_dwAliasCount <= 33 /* keep it from checking 32 times as the recursion unwinds */
			&& m_mapAlternate != nullptr)
			dtTemp = m_mapAlternate->GetValue_Helper(inKey, returnValueIfNotFound);

		return dtTemp;
	};



public:
	/**
	 * @brief create a vector representing my contents
	 *
	 * @param outArray - receives the data exported in original insertion order
	 * @return error_t: SUCCESS upon exporting the contents; failure otherwise.
	 */
	virtual error_t AsArray(std::vector < std::tuple<KeyType, DataType> > & outArray)
	{
		std::vector < std::tuple<KeyType, DataType> > retVal;
		const DWORD size = m_map.size();

		/*
		 * sort by sequence number (via map)
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

		/*
		 * iterate across in sequence order, building the final collection to copy the contents
		 */
		retVal.reserve(size);
		for (/*intentionally not-const*/auto& kvPair : sequenceMap)
		{
			std::tuple<KeyType, DataType>& value = kvPair.second;

			retVal.emplace_back(std::move(value));
		}

		if (retVal.size() != size)
			return ERR_BOUNDS;

		outArray = std::move(retVal);
		return SUCCESS;
	}
};

#pragma pop_macro("max")
#pragma pop_macro("min")

