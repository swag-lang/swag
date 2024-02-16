#pragma once
#include "Path.h"

template<typename K, typename V>
struct Map : unordered_map<K, V, hash<K>, equal_to<K>, StdAllocator<pair<const K, V>>>
{
	Map()
		: unordered_map<K, V, hash<K>, equal_to<K>, StdAllocator<pair<const K, V>>>()
	{
	}

	void release()
	{
		this->~Map<K, V>();
		::new(this) Map;
	}
};

template<typename V>
struct MapUtf8 : unordered_map<Utf8, V, HashUtf8, equal_to<Utf8>, StdAllocator<pair<const Utf8, V>>>
{
	MapUtf8()
		: unordered_map<Utf8, V, HashUtf8, equal_to<Utf8>, StdAllocator<pair<const Utf8, V>>>()
	{
	}

	void release()
	{
		this->~MapUtf8<V>();
		::new(this) MapUtf8;
	}
};

template<typename V>
struct MapPath : unordered_map<Path, V, HashPath, equal_to<Path>, StdAllocator<pair<const Path, V>>>
{
	MapPath()
		: unordered_map<Path, V, HashPath, equal_to<Path>, StdAllocator<pair<const Path, V>>>()
	{
	}

	MapPath(const MapPath& other)
		: unordered_map<Path, V, HashPath, equal_to<Path>, StdAllocator<pair<const Path, V>>>(other)
	{
	}

	void release()
	{
		this->~MapPath<V>();
		::new(this) MapPath;
	}
};
