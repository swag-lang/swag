#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "Context.h"
#include "AstNode.h"

bool BackendC::emitDataSegment(DataSegment* dataSegment)
{
    if (dataSegment == &module->mutableSegment)
        emitSeparator(bufferC, "MUTABLE SEGMENT");
    else
        emitSeparator(bufferC, "CONSTANT SEGMENT");

    if (dataSegment->buckets.size())
    {
        if (dataSegment == &module->mutableSegment)
            CONCAT_FIXED_STR(bufferC, "static swag_uint8_t __mutableseg[] = {\n");
        else
            CONCAT_FIXED_STR(bufferC, "static swag_uint8_t __constantseg[] = {\n");

        for (int bucket = 0; bucket < dataSegment->buckets.size(); bucket++)
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

    bufferC.addEol();
    return true;
}

bool BackendC::emitStrings()
{
    emitSeparator(bufferC, "STRINGS");

    int index = 0;
    for (const auto& str : module->strBuffer)
    {
        CONCAT_FIXED_STR(bufferC, "static swag_uint8_t __string");
        bufferC.addStringFormat("%d[] = {", index);

        const uint8_t* pz = (const uint8_t*) str.c_str();
        while (*pz)
        {
            bufferC.addString(to_string(*pz));
            bufferC.addChar(',');
            pz++;
        }

        CONCAT_FIXED_STR(bufferC, "0};\n");
        index++;
    }

    bufferC.addEol();
    return true;
}

bool BackendC::emitGlobalInit()
{
    // Init of data segment
    CONCAT_FIXED_STR(bufferC, "static void initDataSeg() {\n");
    for (auto& k : module->mutableSegment.initString)
    {
        for (auto& o : k.second)
            bufferC.addStringFormat("*(void**) (__mutableseg + %d) = __string%d;\n", o, k.first);
    }

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
    for (auto& k : module->constantSegment.initString)
    {
        for (auto& o : k.second)
            bufferC.addStringFormat("*(void**) (__constantseg + %d) = __string%d;\n", o, k.first);
    }

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
    bufferC.addStringFormat("void %s_globalInit(swag_process_infos_t *processInfos)\n", module->nameDown.c_str());
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

    bufferH.addEol();
    bufferH.addStringFormat("SWAG_EXTERN SWAG_IMPEXP void %s_globalInit(struct swag_process_infos_t *processInfos);\n", module->nameDown.c_str());
    return true;
}

bool BackendC::emitGlobalDrop()
{
    // Main init fct
    bufferC.addStringFormat("void %s_globalDrop()\n", module->nameDown.c_str());
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

    bufferH.addStringFormat("SWAG_EXTERN SWAG_IMPEXP void %s_globalDrop();\n", module->nameDown.c_str());
    return true;
}