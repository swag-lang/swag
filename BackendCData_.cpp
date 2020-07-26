#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"

bool BackendC::emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int precompileIndex)
{
    if (precompileIndex != 0)
    {
        if (dataSegment == &module->mutableSegment)
            CONCAT_FIXED_STR(bufferC, "extern __ui8_t __ms[];\n");
        else if (dataSegment == &module->constantSegment)
            CONCAT_FIXED_STR(bufferC, "extern __ui8_t __bs[];\n");
        else
            CONCAT_FIXED_STR(bufferC, "extern __ui8_t __cs[];\n");
        return true;
    }

    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    auto segSize = dataSegment->buckets.size();

    // BSS, no init (0)
    if (dataSegment == &module->bssSegment)
    {
        CONCAT_STR_1(bufferC, "__ui8_t __bs[", dataSegment->totalCount, "];\n");
        return true;
    }

    // Constant & data
    if (dataSegment == &module->mutableSegment)
        CONCAT_FIXED_STR(bufferC, "__ui8_t __ms[]={\n");
    else
        CONCAT_FIXED_STR(bufferC, "__ui8_t __cs[]={\n");

    for (int bucket = 0; bucket < segSize; bucket++)
    {
        int  count = (int) dataSegment->buckets[bucket].count;
        auto pz    = dataSegment->buckets[bucket].buffer;
        int  cpt   = 0;
        while (count--)
        {
            bufferC.addString(to_string(*pz));
            bufferC.addChar(',');
            pz++;
            cpt = (cpt + 1) % 20;
            if (cpt == 0)
                bufferC.addEol();
        }
    }

    CONCAT_FIXED_STR(bufferC, "\n};\n");

    return true;
}

bool BackendC::emitInitDataSeg(OutputFile& bufferC)
{
    CONCAT_FIXED_STR(bufferC, "static void initDataSeg(){\n");
    for (auto& k : module->mutableSegment.initPtr)
    {
        auto kind = k.destSeg;
        if (kind == SegmentKind::Me || kind == SegmentKind::Data)
            bufferC.addStringFormat("*(void**)(__ms+%d)=__ms+%d;\n", k.destOffset, k.srcOffset);
        else
            bufferC.addStringFormat("*(void**)(__ms+%d)=__cs+%d;\n", k.destOffset, k.srcOffset);
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");

    return true;
}

bool BackendC::emitInitConstantSeg(OutputFile& bufferC)
{
    CONCAT_FIXED_STR(bufferC, "static void initConstantSeg(){\n");
    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        bufferC.addStringFormat("*(void**)(__cs+%d)=__cs+%d;\n", k.destOffset, k.srcOffset);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        emitFuncSignatureInternalC(bufferC, k.second, false);
        CONCAT_FIXED_STR(bufferC, ";\n");
        bufferC.addStringFormat("*(void**)(__cs+%d)=%s;\n", k.first, k.second->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}