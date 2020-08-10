#include "pch.h"
#include "BackendX64.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendX64::buildRelocDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    for (auto& k : dataSegment->initFuncPtr)
    {
        auto sym = getOrAddSymbol(pp, k.second->callName(), CoffSymbolKind::Extern);

        CoffRelocation reloc;
        SWAG_ASSERT(k.first < dataSegment->totalCount);
        reloc.virtualAddress = k.first;
        reloc.symbolIndex    = sym->index;
        reloc.type           = IMAGE_REL_AMD64_ADDR64;
        relocTable.table.push_back(reloc);

        //bufferC.addStringFormat("*(void**)(__cs8+%d)=%s;\n", k.first, k.second->callName().c_str());
    }

    return true;
}
