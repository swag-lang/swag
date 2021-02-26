#pragma once
#include "DependentJobs.h"
#include "Pool.h"
#include "Register.h"
#include "RegisterList.h"
#include "SourceLocation.h"

template<typename V, int N>
struct HashTable
{
    struct Entry
    {
        const char* key;
        int         keyLen;
        V           value;
        uint32_t    hash;
    };

    Entry*   buffer;
    uint32_t allocated = 0;
    uint32_t count;
    bool     firstLetter[256] = {0};

    const V* find(const Utf8& key)
    {
        if (!firstLetter[key.buffer[0]])
            return nullptr;

        auto crc = key.hash();

        uint32_t idx = crc % allocated;
        while (buffer[idx].hash)
        {
            if (buffer[idx].hash == crc && buffer[idx].keyLen == key.count && !strncmp(buffer[idx].key, key.buffer, key.count))
                return &buffer[idx].value;
            idx = idx + 1;
            if (idx == allocated)
                idx = 0;
        }

        return nullptr;
    }

    void addElem(const char* key, int keyLen, const V& value, uint32_t crc = 0)
    {
        if (!crc)
            crc = Utf8::hash(key, keyLen);

        // Find a free slot
        uint32_t idx = crc % allocated;
        while (buffer[idx].hash)
        {
            idx = idx + 1;
            if (idx == allocated)
                idx = 0;
        }

        firstLetter[key[0]] = true;
        buffer[idx].hash    = crc;
        buffer[idx].key     = key;
        buffer[idx].keyLen  = keyLen;
        buffer[idx].value   = value;
        count += 1;
    }

    void add(const char* key, const V& value)
    {
        // First allocation
        if (!allocated)
        {
            count     = 0;
            allocated = N;
            buffer    = (Entry*) g_Allocator.alloc(allocated * sizeof(Entry));
            memset(buffer, 0, allocated * sizeof(Entry));
        }

        // Need to grow the hash table, and add back the old values
        else if (count >= allocated / 2)
        {
            auto oldAllocated = allocated;
            auto oldBuffer    = buffer;
            auto oldCount     = count;

            allocated *= 2;
            buffer = (Entry*) g_Allocator.alloc(allocated * sizeof(Entry));
            memset(buffer, 0, allocated * sizeof(Entry));

            count = 0;
            for (uint32_t i = 0; i < oldAllocated; i++)
            {
                if (oldBuffer[i].hash)
                {
                    oldCount--;
                    addElem(oldBuffer[i].key, oldBuffer[i].keyLen, oldBuffer[i].value, oldBuffer[i].hash);
                    if (!oldCount)
                        break;
                }
            }

            g_Allocator.free(oldBuffer, oldAllocated * sizeof(Entry));
        }

        // Find a free slot
        addElem(key, (int) strlen(key), value);
    }
};
