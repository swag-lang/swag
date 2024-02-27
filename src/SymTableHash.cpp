#include "pch.h"
#include "SymTableHash.h"
#include "Symbol.h"

void SymTableHash::clone(const SymTableHash* from)
{
    if (!from->allocated)
        return;
    fastReject = from->fastReject;
    allocated  = from->allocated;
    count      = from->count;
    maxLength  = from->maxLength;
    buffer     = Allocator::alloc_n<Entry>(from->allocated);
    std::copy_n(from->buffer, from->allocated, buffer);
}

SymbolName* SymTableHash::find(const Utf8& str, uint32_t crc) const
{
    if (!allocated)
        return nullptr;

    const uint32_t a = (str[0] | 0x20) - 'a';
    if (a < 32 && !(fastReject & 1 << a))
        return nullptr;
    if (str.length() > maxLength)
        return nullptr;

    if (!crc)
        crc = str.hash();

    uint32_t idx = crc % allocated;
    while (buffer[idx].hash)
    {
        if (buffer[idx].hash == crc && buffer[idx].symbolName->name == str)
            return buffer[idx].symbolName;
        idx = (idx + 1) % allocated;
    }

    return nullptr;
}

void SymTableHash::addElem(SymbolName* data, uint32_t crc)
{
    const uint32_t a = (data->name[0] | 0x20) - 'a';
    if (a < 32)
        fastReject |= 1 << a;

    if (!crc)
        crc = data->name.hash();

    // Find a free slot
    uint32_t idx = crc % allocated;
    while (buffer[idx].hash)
    {
        idx = (idx + 1) % allocated;
    }

    buffer[idx].hash       = crc;
    buffer[idx].symbolName = data;
    maxLength              = max(maxLength, data->name.length());
    count += 1;
}

void SymTableHash::add(SymbolName* data)
{
    // First allocation
    if (!allocated)
    {
        count     = 0;
        allocated = 4;
        buffer    = Allocator::alloc_n<Entry>(allocated);
        memset(buffer, 0, allocated * sizeof(Entry));
#ifdef SWAG_STATS
        g_Stats.memSymTable += allocated * sizeof(Entry);
#endif
    }

    // Need to grow the hash table, and add back the old values
    else if (count >= allocated / 2)
    {
        const auto oldAllocated = allocated;
        const auto oldBuffer    = buffer;
        auto       oldCount     = count;

        allocated *= 2;
        buffer = Allocator::alloc_n<Entry>(allocated);
        memset(buffer, 0, allocated * sizeof(Entry));

        count = 0;
        for (uint32_t i = 0; i < oldAllocated; i++)
        {
            if (oldBuffer[i].hash)
            {
                oldCount--;
                addElem(oldBuffer[i].symbolName, oldBuffer[i].hash);
                if (!oldCount)
                    break;
            }
        }

        Allocator::free(oldBuffer, oldAllocated * sizeof(Entry));

#ifdef SWAG_STATS
        g_Stats.memSymTable -= oldAllocated * sizeof(Entry);
        g_Stats.memSymTable += allocated * sizeof(Entry);
#endif
    }

    // Find a free slot
    addElem(data);
}
