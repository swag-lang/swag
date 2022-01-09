#pragma once
#include "Utf8.h"

template<typename T>
struct Array : public vector<T>
{
    bool contains(const T& value)
    {
        for (auto& it : *this)
        {
            if (it == value)
                return true;
        }

        return false;
    }

    int find(const T& value)
    {
        int idx = 0;
        for (auto& it : *this)
        {
            if (it == value)
                return idx;
            idx++;
        }

        return -1;
    }
};
