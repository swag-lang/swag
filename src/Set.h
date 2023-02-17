#pragma once

template<typename T>
struct Set : public set<T, less<T>, StdAllocator<T>>
{
    Set()
        : set<T, less<T>, StdAllocator<T>>()
    {
    }

    Set(const Set& other)
        : set<T, less<T>, StdAllocator<T>>(other)
    {
    }
};
