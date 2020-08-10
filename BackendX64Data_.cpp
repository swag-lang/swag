#include "pch.h"
#include "BackendX64.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendX64::buildRelocMutableSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int            ct              = buildParameters.compileType;
    int            precompileIndex = buildParameters.precompileIndex;
    auto&          pp              = perThread[ct][precompileIndex];
    CoffRelocation reloc;

    for (auto& k : dataSegment->initPtr)
    {
        CoffSymbol* sym;
        SWAG_ASSERT(k.destOffset < dataSegment->totalCount);
        if (k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Data)
            sym = getOrAddSymbol(pp, format("__dsr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset);
        else
            sym = getOrAddSymbol(pp, format("__csr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset);

        reloc.virtualAddress = k.destOffset;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}

bool BackendX64::buildRelocConstantSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int            ct              = buildParameters.compileType;
    int            precompileIndex = buildParameters.precompileIndex;
    auto&          pp              = perThread[ct][precompileIndex];
    CoffRelocation reloc;

    for (auto& k : dataSegment->initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);

        auto sym = getOrAddSymbol(pp, format("__csr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset);

        reloc.virtualAddress = k.destOffset;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        auto sym = getOrAddSymbol(pp, k.second->callName(), CoffSymbolKind::Extern);

        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}
