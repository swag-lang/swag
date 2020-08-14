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
            CONCAT_FIXED_STR(bufferC, "extern __u8_t __ms[];\n");
        else if (dataSegment == &module->constantSegment)
            CONCAT_FIXED_STR(bufferC, "extern __u8_t __bs[];\n");
        else if (dataSegment == &module->typeSegment)
            CONCAT_FIXED_STR(bufferC, "extern __u8_t __ts[];\n");
        else
            CONCAT_FIXED_STR(bufferC, "extern __u8_t __cs[];\n");
        return true;
    }

    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    // BSS, no init (0)
    if (dataSegment == &module->bssSegment)
    {
        CONCAT_STR_1(bufferC, "__u8_t __bs[", dataSegment->totalCount, "];\n");
        return true;
    }

    // Constant & data
    if (dataSegment == &module->mutableSegment)
        CONCAT_FIXED_STR(bufferC, "__u64_t __ms[]={\n");
    else if (dataSegment == &module->typeSegment)
        CONCAT_FIXED_STR(bufferC, "__u64_t __ts[]={\n");
    else
        CONCAT_FIXED_STR(bufferC, "__u64_t __cs[]={\n");

    DataSegment::Seek seek;
    uint64_t          value = 0;
    int               cpt   = 0;
    char              buf[50];
    while (dataSegment->readU64(seek, value))
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

bool BackendC::emitInitMutableSeg(OutputFile& bufferC)
{
    bool firstMS = true;
    bool firstCS = true;
    bool firstTS = true;

    CONCAT_FIXED_STR(bufferC, "static void initMutableSeg(){\n");
    for (auto& k : module->mutableSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Constant)
        {
            if (firstMS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ms8=(__u8_t*)__ms;\n");
                firstMS = false;
            }

            if (firstCS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__cs8=(__u8_t*)__cs;\n");
                firstCS = false;
            }

            bufferC.addStringFormat("*(void**)(__ms8+%d)=__cs8+%d;\n", k.patchOffset, k.srcOffset);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);

            if (firstMS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ms8=(__u8_t*)__ms;\n");
                firstMS = false;
            }

            if (firstTS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ts8=(__u8_t*)__ts;\n");
                firstTS = false;
            }

            bufferC.addStringFormat("*(void**)(__ms8+%d)=__ts8+%d;\n", k.patchOffset, k.srcOffset);
        }
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}

bool BackendC::emitInitTypeSeg(OutputFile& bufferC)
{
    bool firstTS = true;
    bool firstCS = true;

    CONCAT_FIXED_STR(bufferC, "static void initTypeSeg(){\n");

    for (auto& k : module->typeSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Me)
        {
            if (firstTS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ts8=(__u8_t*)__ts;\n");
                firstTS = false;
            }

            bufferC.addStringFormat("*(void**)(__ts8+%d)=__ts8+%d;\n", k.patchOffset, k.srcOffset);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Constant);
            if (firstTS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ts8=(__u8_t*)__ts;\n");
                firstTS = false;
            }
            if (firstCS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__cs8=(__u8_t*)__cs;\n");
                firstCS = false;
            }
            bufferC.addStringFormat("*(void**)(__ts8+%d)=__cs8+%d;\n", k.patchOffset, k.srcOffset);
        }
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}

bool BackendC::emitInitConstantSeg(OutputFile& bufferC)
{
    bool firstCS = true;
    bool firstTS = true;

    CONCAT_FIXED_STR(bufferC, "static void initConstantSeg(){\n");

    for (auto& k : module->constantSegment.initPtr)
    {
        if (k.fromSegment == SegmentKind::Me || k.fromSegment == SegmentKind::Constant)
        {
            if (firstCS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__cs8=(__u8_t*)__cs;\n");
                firstCS = false;
            }

            bufferC.addStringFormat("*(void**)(__cs8+%d)=__cs8+%d;\n", k.patchOffset, k.srcOffset);
        }
        else
        {
            SWAG_ASSERT(k.fromSegment == SegmentKind::Type);

            if (firstTS)
            {
                CONCAT_FIXED_STR(bufferC, "__u8_t*__ts8=(__u8_t*)__ts;\n");
                firstTS = false;
            }

            bufferC.addStringFormat("*(void**)(__cs8+%d)=__ts8+%d;\n", k.patchOffset, k.srcOffset);
        }
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        emitFuncSignatureInternalC(bufferC, k.second, false);
        CONCAT_FIXED_STR(bufferC, ";\n");

        if (firstCS)
        {
            CONCAT_FIXED_STR(bufferC, "__u8_t*__cs8=(__u8_t*)__cs;\n");
            firstCS = false;
        }

        bufferC.addStringFormat("*(void**)(__cs8+%d)=%s;\n", k.first, k.second->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}