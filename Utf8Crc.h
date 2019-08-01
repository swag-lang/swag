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
