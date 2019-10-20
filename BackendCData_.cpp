#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
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
            bufferC.addString("static swag_uint8_t __mutableseg[] = {\n");
        else
            bufferC.addString("static swag_uint8_t __constantseg[] = {\n");

        for (int bucket = 0; bucket < dataSegment->buckets.size(); bucket++)
        {
            int  count = (int) dataSegment->buckets[bucket].count;
            auto pz    = dataSegment->buckets[bucket].buffer;
            int  cpt   = 0;
            while (count--)
            {
                bufferC.addString(to_string(*pz));
                bufferC.addString(",");
                pz++;
                cpt = (cpt + 1) % 20;
                if (cpt == 0)
                    bufferC.addString("\n");
            }
        }

        bufferC.addString("\n};\n");
    }

    bufferC.addString("\n");
    return true;
}

bool BackendC::emitStrings()
{
    emitSeparator(bufferC, "STRINGS");

    int index = 0;
    for (const auto& str : module->strBuffer)
    {
        bufferC.addString("static swag_uint8_t __string");
        bufferC.addString(format("%d[] = {", index));

        const uint8_t* pz = (const uint8_t*) str.c_str();
        while (*pz)
        {
            bufferC.addString(to_string(*pz));
            bufferC.addString(",");
            pz++;
        }

        bufferC.addString("0};\n");
        index++;
    }

    bufferC.addString("\n");
    return true;
}

bool BackendC::emitGlobalInit()
{
    // Init of data segment
    bufferC.addString("static void initDataSeg() {\n");
    for (auto& k : module->mutableSegment.initString)
    {
        for (auto& o : k.second)
            bufferC.addString(format("*(void**) (__mutableseg + %d) = __string%d;\n", o, k.first));
    }

    for (auto& k : module->mutableSegment.initPtr)
    {
        auto kind = k.destSeg;
        if (kind == SegmentKind::Me || kind == SegmentKind::Data)
            bufferC.addString(format("*(void**) (__mutableseg + %d) = __mutableseg + %d;\n", k.sourceOffset, k.destOffset));
        else
            bufferC.addString(format("*(void**) (__mutableseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset));
    }

    bufferC.addString("}\n\n");

    // Init of constant segment
    bufferC.addString("static void initConstantSeg() {\n");
    for (auto& k : module->constantSegment.initString)
    {
        for (auto& o : k.second)
            bufferC.addString(format("*(void**) (__constantseg + %d) = __string%d;\n", o, k.first));
    }

    for (auto& k : module->constantSegment.initPtr)
    {
        SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
        bufferC.addString(format("*(void**) (__constantseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset));
    }

    bufferC.addString("}\n\n");

    // Main init fct
    bufferC.addString(format("void %s_globalInit(swag_tls_id_t contextTlsID, swag_context_t* defaultContext)\n", module->name.c_str()));
	bufferC.addString("{\n");
    bufferC.addString("\t__contextTlsId = contextTlsID;\n");
	bufferC.addString("\t__defaultContext = defaultContext;\n");
	bufferC.addString("\n");

    bufferC.addString("\tinitDataSeg();\n");
    bufferC.addString("\tinitConstantSeg();\n");

	for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        bufferC.addString(format("\t%s();\n", bc->callName().c_str()));
    }

    bufferC.addString("}\n");
	bufferC.addString("\n");

	bufferH.addString("\n");
    bufferH.addString(format("SWAG_EXTERN SWAG_IMPEXP void %s_globalInit(swag_tls_id_t contextTlsID, struct swag_context_t* defaultContext);\n", module->name.c_str()));
    return true;
}

bool BackendC::emitGlobalDrop()
{
    // Main init fct
    bufferC.addString(format("void %s_globalDrop()\n", module->name.c_str()));
	bufferC.addString("{\n");

    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        bufferC.addString(format("\t%s();\n", bc->callName().c_str()));
    }

	bufferC.addString("}\n");
    bufferC.addString("\n");

    bufferH.addString(format("SWAG_EXTERN SWAG_IMPEXP void %s_globalDrop();\n", module->name.c_str()));
    return true;
}