#pragma once
#include "Core/Path.h"

template<typename T>
struct Set : std::unordered_set<T, std::hash<T>, std::equal_to<T>, StdAllocator<T>>
{
    Set() :
        std::unordered_set<T, std::hash<T>, std::equal_to<T>, StdAllocator<T>>()
    {
    }

    Set(const Set& other) :
        std::unordered_set<T, std::hash<T>, std::equal_to<T>, StdAllocator<T>>(other)
    {
    }
};

struct SetUtf8 : std::unordered_set<Utf8, HashUtf8, std::equal_to<>, StdAllocator<Utf8>>
{
    SetUtf8() = default;
};

struct SetPath : std::unordered_set<Path, HashPath, std::equal_to<>, StdAllocator<Path>>
{
    SetPath() = default;
};
