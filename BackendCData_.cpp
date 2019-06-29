#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendC::emitDataSegment()
{
    emitSeparator(bufferC, "DATA SEGMENT");
    if (module->dataSegment.size())
    {
        int count = (int) module->dataSegment.size();
        bufferC.addString(format("static swag_uint8_t __dataseg[%d] = {\n", count));

        const uint8_t* pz  = (const uint8_t*) &module->dataSegment[0];
        int            cpt = 0;
        while (count--)
        {
            bufferC.addString(to_string(*pz));
            bufferC.addString(",");
            pz++;
            cpt = (cpt + 1) % 20;
            if (cpt == 0)
                bufferC.addString("\n");
        }

        bufferC.addString("\n};\n");
    }

    bufferC.addString("\n");
    return true;
}

bool BackendC::emitConstantSegment()
{
    emitSeparator(bufferC, "CONSTANT SEGMENT");
    if (module->constantSegment.size())
    {
        int count = (int) module->constantSegment.size();
        bufferC.addString(format("static swag_uint8_t __constantseg[%d] = {\n", count));

        const uint8_t* pz  = (const uint8_t*) &module->constantSegment[0];
        int            cpt = 0;
        while (count--)
        {
            bufferC.addString(to_string(*pz));
            bufferC.addString(",");
            pz++;
            cpt = (cpt + 1) % 20;
            if (cpt == 0)
                bufferC.addString("\n");
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
    bufferC.addString("static void __initDataSeg() {\n");
    for (auto& k : module->strBufferInit)
    {
        bufferC.addString(format("*(void**) (__dataseg + %d) = __string%d;\n", k.second, k.first));
    }

    bufferC.addString("}\n\n");

    bufferC.addString(format("void __%s_globalInit() {\n", module->name.c_str()));
    bufferC.addString("__initDataSeg();\n");
    bufferC.addString("}\n\n");

	bufferH.addString(format("SWAG_EXTERN SWAG_IMPEXP void __%s_globalInit();\n", module->name.c_str()));
    return true;
}