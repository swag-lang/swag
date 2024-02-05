#pragma once
struct SymbolName;

struct SymTableHash
{
    void        clone(const SymTableHash* from);
    SymbolName* find(const Utf8& str, uint32_t crc = 0) const;
    void        addElem(SymbolName* data, uint32_t crc = 0);
    void        add(SymbolName* data);

    struct Entry
    {
        SymbolName* symbolName;
        uint32_t    hash;
    };

    Entry*   buffer     = nullptr;
    uint32_t allocated  = 0;
    uint32_t count      = 0;
    uint32_t fastReject = 0;
    uint32_t maxLength  = 0;
};
