#pragma once

template<typename K, typename V>
struct Map : public map<K, V, less<K>, StdAllocator<pair<const K, V>>>
{
    Map()
        : map<K, V, less<K>, StdAllocator<pair<const K, V>>>()
    {
    }

    Map(const Map& other)
        : map<K, V, less<K>, StdAllocator<pair<const K, V>>>(other)
    {
    }
};
