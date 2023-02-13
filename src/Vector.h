#pragma once

template<typename T>
struct Vector : public vector<T>
{
    Vector()
        : vector<T>()
    {
    }

    Vector(const Vector& other)
        : vector<T>(other)
    {
    }

    Vector(const initializer_list<T>& other)
        : vector<T>(other)
    {
    }

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
