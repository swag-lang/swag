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
