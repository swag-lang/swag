#pragma once
#include "Core/Path.h"

template<typename K, typename V>
struct Map : std::unordered_map<K, V, std::hash<K>, std::equal_to<>, StdAllocator<std::pair<const K, V>>>
{
    Map() :
        std::unordered_map<K, V, std::hash<K>, std::equal_to<>, StdAllocator<std::pair<const K, V>>>()
    {
    }

    void release()
    {
        this->~Map<K, V>();
        ::new (this) Map;
    }
};

template<typename V>
struct MapUtf8 : std::unordered_map<Utf8, V, HashUtf8, std::equal_to<Utf8>, StdAllocator<std::pair<const Utf8, V>>>
{
    MapUtf8() :
        std::unordered_map<Utf8, V, HashUtf8, std::equal_to<Utf8>, StdAllocator<std::pair<const Utf8, V>>>()
    {
    }

    void release()
    {
        this->~MapUtf8<V>();
        ::new (this) MapUtf8;
    }
};

template<typename V>
struct MapPath : std::unordered_map<Path, V, HashPath, std::equal_to<Path>, StdAllocator<std::pair<const Path, V>>>
{
    MapPath() :
        std::unordered_map<Path, V, HashPath, std::equal_to<Path>, StdAllocator<std::pair<const Path, V>>>()
    {
    }

    MapPath(const MapPath& other) :
        std::unordered_map<Path, V, HashPath, std::equal_to<Path>, StdAllocator<std::pair<const Path, V>>>(other)
    {
    }

    void release()
    {
        this->~MapPath<V>();
        ::new (this) MapPath;
    }
};
