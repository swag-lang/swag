#pragma once
#include "Path.h"

template<typename T>
struct Set : public unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>
{
    Set()
        : unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>()
    {
    }

    Set(const Set& other)
        : unordered_set<T, hash<T>, equal_to<T>, StdAllocator<T>>(other)
    {
    }
};

struct SetUtf8 : public unordered_set<Utf8, HashUtf8, equal_to<Utf8>, StdAllocator<Utf8>>
{
    SetUtf8()
        : unordered_set<Utf8, HashUtf8, equal_to<Utf8>, StdAllocator<Utf8>>()
    {
    }
};

struct SetPath : public unordered_set<Path, HashPath, equal_to<Path>, StdAllocator<Path>>
{
    SetPath()
        : unordered_set<Path, HashPath, equal_to<Path>, StdAllocator<Path>>()
    {
    }
};
