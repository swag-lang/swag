#include "pch.h"
#include "Backend/SCBE/Main/SCBE.h"

bool SCBE::buildRelocationSegment(SCBE_X64& pp, DataSegment* dataSegment, CPURelocationTable& relocTable, SegmentKind me)
{
    if (dataSegment->buckets.empty())
        return true;
    if (!dataSegment->totalCount)
        return true;

    CPURelocation reloc;

    for (const auto& k : dataSegment->initPtr)
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

        *reinterpret_cast<uint64_t*>(dataSegment->address(k.patchOffset)) = k.fromOffset;

        reloc.virtualAddress = k.patchOffset;
        reloc.symbolIndex    = sym;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    for (auto& k : dataSegment->initFuncPtr)
    {
        *reinterpret_cast<void**>(dataSegment->address(k.first)) = nullptr;

        const auto sym       = pp.getOrAddSymbol(k.second, CPUSymbolKind::Extern);
        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);
    }

    return true;
}
