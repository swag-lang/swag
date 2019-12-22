#pragma once
#include "Utf8.h"

struct Utf8Crc : public Utf8
{
    Utf8Crc()
    {
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
        return *(const Utf8*) this == (const Utf8&) from;
    }

    void operator=(const Utf8Crc& from)
    {
        (Utf8&) * this = from;
        crc            = from.crc;
    }

    void operator=(Utf8Crc&& from)
    {
        (Utf8&) * this = move(from);
        crc            = from.crc;
    }

    Utf8Crc(const char* from)
        : Utf8(from)
    {
        computeCrc();
    }

    void computeCrc()
    {
        crc = static_cast<uint32_t>(hash<string>{}(*this));
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