#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendC::emitDataSegment(DataSegment* dataSegment)
{
    if (dataSegment == &module->dataSegment)
        emitSeparator(bufferC, "DATA SEGMENT");
    else
        emitSeparator(bufferC, "CONTANT SEGMENT");

    if (dataSegment->buckets.size())
    {
        if (dataSegment == &module->dataSegment)
            bufferC.addString("static swag_uint8_t __dataseg[] = {\n");
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
    // Data segment
    bufferC.addString("static void initDataSeg() {\n");
	for (auto& k : module->dataSegment.initString)
	{
		bufferC.addString(format("*(void**) (__dataseg + %d) = __string%d;\n", k.second, k.first));
	}

    for (auto& k : module->dataSegment.initPtr)
    {
		auto kind = k.destSeg;
		if(kind == SegmentKind::Me || kind == SegmentKind::Data)
			bufferC.addString(format("*(void**) (__dataseg + %d) = __dataseg + %d;\n", k.sourceOffset, k.destOffset));
		else
			bufferC.addString(format("*(void**) (__dataseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset));
    }

    bufferC.addString("}\n\n");

    // Constant segment
    bufferC.addString("static void initConstantSeg() {\n");
    for (auto& k : module->constantSegment.initString)
        bufferC.addString(format("*(void**) (__constantseg + %d) = __string%d;\n", k.second, k.first));

	for (auto& k : module->constantSegment.initPtr)
	{
		SWAG_ASSERT(k.destSeg == SegmentKind::Me || k.destSeg == SegmentKind::Constant);
		bufferC.addString(format("*(void**) (__constantseg + %d) = __constantseg + %d;\n", k.sourceOffset, k.destOffset));
	}

    bufferC.addString("}\n\n");

    // Main init fct
    bufferC.addString(format("void %s_globalInit() {\n", module->name.c_str()));
    bufferC.addString("initDataSeg();\n");
    bufferC.addString("initConstantSeg();\n");
    bufferC.addString("}\n\n");

    bufferH.addString(format("SWAG_EXTERN SWAG_IMPEXP void %s_globalInit();\n", module->name.c_str()));
    return true;
}