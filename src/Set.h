#pragma once
#include "Path.h"

template<typename T>
struct Set : unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>
{
    Set() :
        unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>()
    {
    }

    Set(const Set& other) :
        unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>(other)
    {
    }
};

struct SetUtf8 : unordered_set<Utf8, HashUtf8, equal_to<Utf8>, StdAllocator<Utf8>>
{
    SetUtf8() = default;
};

struct SetPath : unordered_set<Path, HashPath, equal_to<Path>, StdAllocator<Path>>
{
    SetPath() = default;
};
