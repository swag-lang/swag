#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendC::emitMain()
{
    outputC.addString("\nvoid main() {\n");
	outputC.addString("__main();\n");
	outputC.addString("}\n");
    return true;
}
