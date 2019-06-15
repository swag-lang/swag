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

	bufferC.addString("/****************************** STRINGS *******************************/\n");
    for (auto one : module->byteCodeFunc)
    {
        for (auto str : one->strBuffer)
        {
			bufferC.addString("static char* __string");
			bufferC.addString(format("%d = \"", index));
			bufferC.addString(str);
			bufferC.addString("\";\n");
			index++;
        }
    }

	bufferC.addString("\n");
    return true;
}
