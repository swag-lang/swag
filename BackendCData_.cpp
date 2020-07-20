#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"

bool BackendC::emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int precompileIndex)
{
    if (dataSegment == &module->mutableSegment)
        emitSeparator(bufferC, "MUTABLE SEGMENT");
    else if (dataSegment == &module->constantSegment)
        emitSeparator(bufferC, "CONSTANT SEGMENT");
    else
        emitSeparator(bufferC, "BSS SEGMENT");

    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;

    if (precompileIndex != 0)
    {
        if (dataSegment == &module->mutableSegment)
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __mutableseg[];\n");
        else if (dataSegment == &module->constantSegment)
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __bssseg[];\n");
        else
            CONCAT_FIXED_STR(bufferC, "extern swag_uint8_t __constantseg[];\n");
    }
    else
    {
        auto segSize = dataSegment->buckets.size();
        if (dataSegment == &module->bssSegment)
        {
            CONCAT_STR_1(bufferC, "swag_uint8_t __bssseg[", dataSegment->totalCount, "];\n");
        }
        else
        {
            if (dataSegment == &module->mutableSegment)
                CONCAT_FIXED_STR(bufferC, "swag_uint8_t __mutableseg[] = {\n");
            else
                CONCAT_FIXED_STR(bufferC, "swag_uint8_t __constantseg[] = {\n");

            for (int bucket = 0; bucket < segSize; bucket++)
            {
                int  count = (int)dataSegment->buckets[bucket].count;
                auto pz = dataSegment->buckets[bucket].buffer;
                int  cpt = 0;
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
        }
    }

    return true;
}

bool BackendC::emitInitDataSeg(OutputFile& bufferC)
{
    CONCAT_FIXED_STR(bufferC, "static void initDataSeg() {\n");
    for (auto& k : module->mutableSegment.initPtr)
    {
        auto kind = k.destSeg;
        if (kind == SegmentKind::Me || kind == SegmentKind::Data)
            bufferC.addStringFormat("*(void**) (__mutableseg + %d) = __mutableseg + %d;\n", k.destOffset, k.srcOffset);
        else
            bufferC.addStringFormat("*(void**) (__mutableseg + %d) = __constantseg + %d;\n", k.destOffset, k.srcOffset);
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");

    return true;
}

bool BackendC::emitInitConstantSeg(OutputFile& bufferC)
{
    CONCAT_FIXED_STR(bufferC, "static void initConstantSeg() {\n");
    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        bufferC.addStringFormat("*(void**) (__constantseg + %d) = __constantseg + %d;\n", k.destOffset, k.srcOffset);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        bufferC.addStringFormat("*(void**) (__constantseg + %d) = %s;\n", k.first, k.second->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");
    return true;
}