#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"

bool BackendC::emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int preCompileIndex)
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

    if (preCompileIndex != 0)
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
        }
    }

    return true;
}

bool BackendC::emitGlobalInit(OutputFile& bufferC)
{
    // Init of data segment
    CONCAT_FIXED_STR(bufferC, "static void initDataSeg() {\n");
    for (auto& k : module->mutableSegment.initPtr)
    {
        auto kind = k.destSeg;
        if (kind == SegmentKind::Me || kind == SegmentKind::Data)
            bufferC.addStringFormat("*(void**) (__mutableseg + %d) = __mutableseg + %d;\n", k.sourceOffset, k.destOffset);
        else
            bufferC.addStringFormat("*(void**) (__mutableseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset);
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");

    // Init of constant segment
    CONCAT_FIXED_STR(bufferC, "static void initConstantSeg() {\n");
    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        bufferC.addStringFormat("*(void**) (__constantseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset);
    }

    for (auto& k : module->constantSegment.initFuncPtr)
    {
        bufferC.addStringFormat("*(void**) (__constantseg + %d) = %s;\n", k.first, k.second->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n\n");

    // Main init fct
    bufferC.addStringFormat("SWAG_EXPORT void %s_globalInit(swag_process_infos_t *processInfos)\n", module->nameDown.c_str());
    CONCAT_FIXED_STR(bufferC, "{\n");
    CONCAT_FIXED_STR(bufferC, "\t__process_infos = *processInfos;\n");

    bufferC.addEol();

    CONCAT_FIXED_STR(bufferC, "\tinitDataSeg();\n");
    CONCAT_FIXED_STR(bufferC, "\tinitConstantSeg();\n");

    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        bufferC.addStringFormat("\t%s();\n", bc->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n");
    bufferC.addEol();

    return true;
}

bool BackendC::emitGlobalDrop(OutputFile& bufferC)
{
    // Main init fct
    bufferC.addStringFormat("SWAG_EXPORT void %s_globalDrop()\n", module->nameDown.c_str());
    CONCAT_FIXED_STR(bufferC, "{\n");

    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        bufferC.addStringFormat("\t%s();\n", bc->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n");
    bufferC.addEol();

    return true;
}