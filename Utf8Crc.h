#pragma once
#include "Utf8.h"
#include "Runtime.h"

struct Utf8Crc : public Utf8
{
    Utf8Crc()
    {
    }

    Utf8Crc(const char* from)
        : Utf8(from)
    {
        computeCrc();
    }

    Utf8Crc(const Utf8& from)
        : Utf8(from)
    {
        computeCrc();
    }

    Utf8Crc(const string& from)
        : Utf8(from)
    {
        computeCrc();
    }

    Utf8Crc(string&& from)
        : Utf8(move(from))
    {
        computeCrc();
    }

    Utf8Crc(const Utf8Crc& from)
        : Utf8(from)
    {
        crc = from.crc;
    }

    Utf8Crc(Utf8Crc&& from)
        : Utf8(move(from))
    {
        crc = from.crc;
    }

    bool compare(const Utf8Crc& from) const
    {
        if (crc != from.crc)
            return false;
        if (count != from.count)
            return false;
        return !strcmp(buffer, from.buffer);
    }

    void operator=(const Utf8Crc& from)
    {
        (Utf8&) * this = from;
        crc            = from.crc;
    }

    void operator=(const string& from)
    {
        (Utf8&) * this = from;
        computeCrc();
    }

    void operator=(Utf8Crc&& from)
    {
        count     = from.count;
        allocated = from.allocated;

        if (from.allocated == UTF8_SMALL_SIZE)
        {
            static_assert(UTF8_SMALL_SIZE == 8);
            buffer              = padding;
            *(uint64_t*) buffer = *(uint64_t*) from.buffer;
        }
        else
        {
            buffer      = from.buffer;
            from.buffer = nullptr;
            from.reset();
        }

        crc = from.crc;
    }

    void operator=(Utf8&& from)
    {
        count     = from.count;
        allocated = from.allocated;

        if (from.allocated == UTF8_SMALL_SIZE)
        {
            static_assert(UTF8_SMALL_SIZE == 8);
            buffer              = padding;
            *(uint64_t*) buffer = *(uint64_t*) from.buffer;
        }
        else
        {
            buffer      = from.buffer;
            from.buffer = nullptr;
            from.reset();
        }

        computeCrc();
    }

    uint32_t hash_string(const char* s)
    {
        uint32_t hash = 0;

        for (; *s; ++s)
        {
            hash += *s;
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);

        return hash;
    }

    void computeCrc()
    {
        if (!count)
            crc = 0;
        else
            crc = hash_string(buffer);
    }

    uint32_t crc = 0;
};

struct Utf8CrcKeyHash
{
    size_t operator()(const Utf8Crc& k) const
    {
        return k.crc;
    }
};

struct Utf8CrcKeyEqual
{
    bool operator()(const Utf8Crc& lhs, const Utf8Crc& rhs) const
    {
        return lhs.compare(rhs);
    }
};