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

    // BSS, no init (0)
    if (dataSegment == &module->bssSegment)
    {
        CONCAT_STR_1(bufferC, "__ui8_t __bs[", dataSegment->totalCount, "];\n");
        return true;
    }

    // Constant & data
    if (dataSegment == &module->mutableSegment)
        CONCAT_FIXED_STR(bufferC, "__ui64_t __ms[]={\n");
    else
        CONCAT_FIXED_STR(bufferC, "__ui64_t __cs[]={\n");

    dataSegment->rewindRead();
    uint64_t value = 0;
    int      cpt   = 0;
    char     buf[50];
    while (dataSegment->readU64(value))
    {
        if (value > 0xFFFF)
            sprintf_s(buf, "0x%llX", value);
        else
            sprintf_s(buf, "%lld", value);
        bufferC.addString(buf);
        bufferC.addChar(',');
        cpt = (cpt + 1) % 20;
        if (cpt == 0)
            bufferC.addEol();
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
            bufferC.addStringFormat("*(void**)((__ui8_t*)__ms+%d)=(__ui8_t*)__ms+%d;\n", k.destOffset, k.srcOffset);
        else
            bufferC.addStringFormat("*(void**)((__ui8_t*)__ms+%d)=(__ui8_t*)__cs+%d;\n", k.destOffset, k.srcOffset);
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
        bufferC.addStringFormat("*(void**)((__ui8_t*)__cs+%d)=(__ui8_t*)__cs+%d;\n", k.destOffset, k.srcOffset);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        emitFuncSignatureInternalC(bufferC, k.second, false);
        CONCAT_FIXED_STR(bufferC, ";\n");
        bufferC.addStringFormat("*(void**)((__ui8_t*)__cs+%d)=%s;\n", k.first, k.second->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}