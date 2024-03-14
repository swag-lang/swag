#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Semantic/Type/TypeInfo.h"

void SCBE_CPU::clearInstructionCache()
{
    storageRegCount = UINT32_MAX;
    storageRegStack = 0;
    storageRegBits  = 0;
    storageMemReg   = RAX;
    storageReg      = RAX;
}

CPUSymbol* SCBE_CPU::getSymbol(const Utf8& name)
{
    const auto it = mapSymbols.find(name);
    if (it != mapSymbols.end())
        return &allSymbols[it->second];
    return nullptr;
}

CPUSymbol* SCBE_CPU::getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    const auto it = getSymbol(name);
    if (it)
    {
        if (it->kind == kind)
            return it;
        if (kind == CPUSymbolKind::Extern)
            return it;
        if (kind == CPUSymbolKind::Function && it->kind == CPUSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CPUSymbol sym;
    sym.name       = name;
    sym.kind       = kind;
    sym.value      = value;
    sym.sectionIdx = sectionIdx;
    SWAG_ASSERT(allSymbols.size() < UINT32_MAX);
    sym.index = allSymbols.size();
    allSymbols.emplace_back(std::move(sym));
    mapSymbols[name] = allSymbols.size() - 1;
    return &allSymbols.back();
}

void SCBE_CPU::addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type)
{
    CPURelocation reloc;
    reloc.virtualAddress = virtualAddr;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = type;
    relocTableTextSection.table.push_back(reloc);
}

CPUSymbol* SCBE_CPU::getOrCreateGlobalString(const Utf8& str)
{
    const auto it = globalStrings.find(str);
    if (it != globalStrings.end())
        return &allSymbols[it->second];

    const Utf8 symName = form("__str%u", static_cast<uint32_t>(globalStrings.size()));
    const auto sym     = getOrAddSymbol(symName, CPUSymbolKind::GlobalString);
    globalStrings[str] = sym->index;
    sym->value         = stringSegment.addStringNoLock(str);

    return sym;
}

uint32_t SCBE_CPU::getOrCreateLabel(uint32_t ip)
{
    const auto it = labels.find(ip);
    if (it == labels.end())
    {
        const auto count = concat.totalCount();
        labels[ip]       = static_cast<int32_t>(count);
        return count;
    }

    return it->second;
}

CPUFunction* SCBE_CPU::registerFunction(AstNode* node, uint32_t symbolIndex)
{
    CPUFunction cf;
    cf.node        = node;
    cf.symbolIndex = symbolIndex;
    functions.push_back(cf);
    return &functions.back();
}

uint32_t SCBE_CPU::getParamStackOffset(const CPUFunction* cpuFct, uint32_t paramIdx)
{
    const auto& cc = cpuFct->typeFunc->getCallConv();

    // If this was passed as a register, then get the value from storeS4 (where input registers have been saved)
    // instead of value from the stack
    if (paramIdx < cc.paramByRegisterCount)
        return REG_OFFSET(paramIdx) + cpuFct->offsetLocalStackParams;

    // Value from the caller stack
    return REG_OFFSET(paramIdx) + cpuFct->offsetCallerStackParams;
}
