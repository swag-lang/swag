#include "pch.h"
#include "SCBE.h"

bool SCBE::buildRelocSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable, SegmentKind me)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int            ct              = buildParameters.compileType;
    int            precompileIndex = buildParameters.precompileIndex;
    auto&          pp              = *perThread[ct][precompileIndex];
    CoffRelocation reloc;

    SWAG_ASSERT(precompileIndex == 0);
    for (auto& k : dataSegment->initPtr)
    {
        uint32_t sym;
        SWAG_ASSERT(k.patchOffset <= dataSegment->totalCount - sizeof(void*));

        auto fromSegment = k.fromSegment;
        if (fromSegment == SegmentKind::Me)
            fromSegment = me;
        switch (fromSegment)
        {
        case SegmentKind::Constant:
            sym = pp.symCSIndex;
            break;
        case SegmentKind::Tls:
            sym = pp.symTLSIndex;
            break;
        case SegmentKind::Bss:
            sym = pp.symBSIndex;
            break;
        case SegmentKind::Data:
            sym = pp.symMSIndex;
            break;
        default:
            SWAG_ASSERT(false);
            sym = 0;
            break;
        }

        *(uint64_t*) dataSegment->address(k.patchOffset) = k.fromOffset;

        reloc.virtualAddress = k.patchOffset;
        reloc.symbolIndex    = sym;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        *(void**) dataSegment->address(k.first) = 0;

        auto sym             = pp.getOrAddSymbol(k.second, CoffSymbolKind::Extern);
        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}
