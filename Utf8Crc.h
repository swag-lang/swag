#pragma once
#include "Utf8.h"

struct utf8crc : public utf8
{
    utf8crc()
    {
    }

    utf8crc(const string& from)
        : utf8(from)
    {
    }

    void computeCrc()
    {
        crc = static_cast<uint32_t>(hash<string>{}(*this));
    }

    uint32_t crc = 0;
};
