#pragma once
#include "Crc32.h"

template<typename V, int N>
struct LangHashTable
{
    struct Entry
    {
        const char* key;
        uint32_t    keyLen;
        uint32_t    hash;
        V           value;
    };

    LangHashTable()
    {
        memset(buffer, 0, sizeof(buffer));
    }

    const V* find(const Utf8& key)
    {
        return find(key.buffer, key.count, 0);
    }

    const V* find(const char* key, uint32_t keyLen, uint32_t crc)
    {
        if (keyLen < minLetters || keyLen > maxLetters || !firstLetter[static_cast<uint32_t>(key[0])])
            return nullptr;

        if (!crc)
            crc = Crc32::compute(reinterpret_cast<const uint8_t*>(key), keyLen);

        uint32_t idx = crc % N;
        while (buffer[idx].hash)
        {
            if (buffer[idx].hash == crc && buffer[idx].keyLen == keyLen && !strncmp(buffer[idx].key, key, keyLen))
                return &buffer[idx].value;
            idx = (idx + 1) % N;
        }

        return nullptr;
    }

    void add(const char* key, const V& value)
    {
        SWAG_ASSERT(count != N);

        uint32_t keyLen = static_cast<uint32_t>(strlen(key));
        auto     crc    = Crc32::compute(reinterpret_cast<const uint8_t*>(key), keyLen);

        // Find a free slot
        uint32_t idx = crc % N;
        while (buffer[idx].hash)
        {
            idx = (idx + 1) % N;
        }

        firstLetter[static_cast<uint32_t>(key[0])] = true;
        minLetters                                 = min(minLetters, keyLen);
        maxLetters                                 = max(maxLetters, keyLen);
        buffer[idx].hash                           = crc;
        buffer[idx].key                            = key;
        buffer[idx].keyLen                         = keyLen;
        buffer[idx].value                          = value;
        count += 1;
    }

    bool     firstLetter[256] = {false};
    Entry    buffer[N];
    uint32_t count      = 0;
    uint32_t allocated  = N;
    uint32_t minLetters = UINT32_MAX;
    uint32_t maxLetters = 0;
};
