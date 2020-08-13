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
        SWAG_ASSERT(k.destOffset < dataSegment->totalCount - sizeof(void*));
        if (k.fromSegment == SegmentKind::Constant)
        {
            sym = getOrAddSymbol(pp, format("__csr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexCS);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);
            sym = getOrAddSymbol(pp, format("__tsr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexTS);
        }

        *(void**) dataSegment->address(k.destOffset) = 0;

        reloc.virtualAddress = k.destOffset;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}

bool BackendX64::buildRelocTypeSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable)
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
        SWAG_ASSERT(k.destOffset < dataSegment->totalCount - sizeof(void*));
        if (k.fromSegment == SegmentKind::Me)
        {
            sym = getOrAddSymbol(pp, format("__tsr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexTS);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Constant);
            sym = getOrAddSymbol(pp, format("__csr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexCS);
        }

        *(void**) dataSegment->address(k.destOffset) = 0;

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
        CoffSymbol* sym;
        SWAG_ASSERT(k.destOffset < dataSegment->totalCount - sizeof(void*));
        if (k.fromSegment == SegmentKind::Me || k.fromSegment == SegmentKind::Constant)
        {
            sym = getOrAddSymbol(pp, format("__csr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexCS);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);
            sym = getOrAddSymbol(pp, format("__tsr%d", k.srcOffset), CoffSymbolKind::Custom, k.srcOffset, pp.sectionIndexTS);
        }

        *(void**) dataSegment->address(k.destOffset) = 0;

        reloc.virtualAddress = k.destOffset;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        auto sym = getOrAddSymbol(pp, k.second->callName(), CoffSymbolKind::Extern, pp.sectionIndexText);

        *(void**) dataSegment->address(k.first) = 0;

        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}
