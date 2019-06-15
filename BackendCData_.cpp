#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendC::emitDataSegment()
{
    int index = 0;

	outputC.addString("/************************** STRINGS **************************/\n");
    for (auto one : module->byteCodeFunc)
    {
        for (auto str : one->strBuffer)
        {
			outputC.addString("static char* __string");
			outputC.addString(format("%d = \"", index));
			outputC.addString(str);
			outputC.addString("\";\n");
			index++;
        }
    }

	outputC.addString("\n");
    return true;
}
