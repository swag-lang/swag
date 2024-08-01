#pragma once
#include "Backend/ByteCode/Register.h"

template<typename T>
struct MapRegTo
{
    static constexpr int CACHE = 64;
    bool                 here[CACHE];
    T                    val[CACHE];
    uint32_t             countCache = 0;
    Map<uint32_t, T>     map;

    MapRegTo()
    {
        memset(here, 0, sizeof(here));
    }

    uint32_t count() const
    {
        return countCache + static_cast<uint32_t>(map.size());
    }

    void clear()
    {
        if (countCache)
        {
            memset(here, 0, sizeof(here));
            countCache = 0;
        }

        map.clear();
    }

    bool contains(uint32_t reg) const
    {
        if (reg < CACHE)
            return here[reg];
        return map.contains(reg);
    }

    void remove(uint32_t reg)
    {
        if (reg < CACHE)
        {
            if(here[reg])
            {
                SWAG_ASSERT(countCache);
                countCache--;
                here[reg] = false;
            }
        }
        else
            map.erase(reg);
    }

    void set(uint32_t reg, const T &value)
    {
        if (reg < CACHE)
        {
            if(!here[reg])
            {
                here[reg] = true;
                countCache++;
            }
            
            val[reg]  = value;
        }
        else
        {
            map.emplace(reg, value);
        }
    }

    T* find(uint32_t reg)
    {
        if (reg < CACHE)
            return here[reg] ? &val[reg] : nullptr;

        auto it = map.find(reg);
        if (it == map.end())
            return nullptr;
        return &it->second;
    }
};
