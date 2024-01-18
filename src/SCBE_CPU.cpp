#include "pch.h"
#include "SCBE_CPU.h"

void SCBE_CPU::clearInstructionCache()
{
    storageRegCount = UINT32_MAX;
    storageRegStack = 0;
    storageRegBits  = 0;
    storageMemReg   = RAX;
    storageReg      = RAX;
}

CoffSymbol* SCBE_CPU::getSymbol(const Utf8& name)
{
    auto it = mapSymbols.find(name);
    if (it != mapSymbols.end())
        return &allSymbols[it->second];
    return nullptr;
}

CoffSymbol* SCBE_CPU::getOrAddSymbol(const Utf8& name, CoffSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    auto it = getSymbol(name);
    if (it)
    {
        if (it->kind == kind)
            return it;
        if (kind == CoffSymbolKind::Extern)
            return it;
        if (kind == CoffSymbolKind::Function && it->kind == CoffSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CoffSymbol sym;
    sym.name       = name;
    sym.kind       = kind;
    sym.value      = value;
    sym.sectionIdx = sectionIdx;
    SWAG_ASSERT(allSymbols.size() < UINT32_MAX);
    sym.index = (uint32_t) allSymbols.size();
    allSymbols.emplace_back(std::move(sym));
    mapSymbols[name] = (uint32_t) allSymbols.size() - 1;
    return &allSymbols.back();
}

void SCBE_CPU::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CoffRelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

CoffSymbol* SCBE_CPU ::getOrCreateGlobalString(const Utf8& str)
{
    auto        it  = globalStrings.find(str);
    CoffSymbol* sym = nullptr;
    if (it != globalStrings.end())
        sym = &allSymbols[it->second];
    else
    {
        Utf8 symName       = Fmt("__str%u", (uint32_t) globalStrings.size());
        sym                = getOrAddSymbol(symName, CoffSymbolKind::GlobalString);
        globalStrings[str] = sym->index;
        sym->value         = stringSegment.addStringNoLock(str);
    }

    return sym;
}

uint32_t SCBE_CPU ::getOrCreateLabel(uint32_t ip)
{
    auto it = labels.find(ip);
    if (it == labels.end())
    {
        auto count = concat.totalCount();
        labels[ip] = count;
        return count;
    }

    return it->second;
}
