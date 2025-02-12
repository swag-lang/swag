#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

void SCBE_CPU::init(const BuildParameters& buildParameters)
{
    buildParams = buildParameters;
    clearInstructionCache();
    labels.clear();
    labelsToSolve.clear();
}

void SCBE_CPU::setOffsetFLT(CPUReg reg, uint32_t offset)
{
    offsetFLTReg = reg;
    offsetFLT = offset;   
}

void SCBE_CPU::clearInstructionCache()
{
    storageConcatCount = UINT32_MAX;
    storageMemOffset = 0;
    storageNumBits  = CPUBits::INVALID;
    storageMemReg   = CPUReg::RAX;
    storageReg      = CPUReg::RAX;
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

uint32_t SCBE_CPU::getBitsCount(CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            return 8;
        case CPUBits::B16:
            return 16;
        case CPUBits::B32:
        case CPUBits::F32:
            return 32;
        case CPUBits::B64:
        case CPUBits::F64:
            return 64;
    }

    SWAG_ASSERT(false);
    return 0;
}

CPUBits SCBE_CPU::getCPUBits(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
    if (flags.has(OPF_8))
        return CPUBits::B8;
    if (flags.has(OPF_16))
        return CPUBits::B16;
    if (flags.has(OPF_32) && flags.has(OPF_FLOAT))
        return CPUBits::F32;
    if (flags.has(OPF_32))
        return CPUBits::B32;
    if (flags.has(OPF_64) && flags.has(OPF_FLOAT))
        return CPUBits::F64;
    if (flags.has(OPF_64))
        return CPUBits::B64;
    SWAG_ASSERT(false);
    return CPUBits::B32;
}

TypeInfo* SCBE_CPU::getCPUType(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
    if (flags.has(OPF_SIGNED))
    {
        if (flags.has(OPF_8))
            return g_TypeMgr->typeInfoS8;
        if (flags.has(OPF_16))
            return g_TypeMgr->typeInfoS16;
        if (flags.has(OPF_32))
            return g_TypeMgr->typeInfoS32;
        if (flags.has(OPF_64))
            return g_TypeMgr->typeInfoS64;
        SWAG_ASSERT(false);
        return nullptr;
    }

    if (flags.has(OPF_8))
        return g_TypeMgr->typeInfoU8;
    if (flags.has(OPF_16))
        return g_TypeMgr->typeInfoU16;
    if (flags.has(OPF_32))
        return g_TypeMgr->typeInfoU32;
    if (flags.has(OPF_64))
        return g_TypeMgr->typeInfoU64;
    SWAG_ASSERT(false);
    return nullptr;
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
