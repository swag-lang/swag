#include "pch.h"
#include "BackendX64.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendX64::buildRelocSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable, SegmentKind me)
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
        case SegmentKind::Type:
            sym = pp.symTSIndex;
            break;
        case SegmentKind::Tls:
            sym = pp.symTLSIndex;
            break;
        default:
            SWAG_ASSERT(false);
            sym = 0;
            break;
        }

        *(uint64_t*) dataSegment->address(k.patchOffset) = k.srcOffset;

        reloc.virtualAddress = k.patchOffset;
        reloc.symbolIndex    = sym;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        auto relocType                          = k.second.second;
        *(void**) dataSegment->address(k.first) = 0;

        // Will be done dynamically during the module init, because we need to patch the pointer with
        // the foreign marker
        if (relocType == DataSegment::RelocType::Foreign)
            continue;

        auto sym             = getOrAddSymbol(pp, k.second.first, CoffSymbolKind::Extern, pp.sectionIndexText);
        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}
