#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "CommandLine.h"
#include "AstNode.h"
#include "TypeInfo.h"

bool BackendC::emitMain()
{
    outputC.addString("\nvoid main() {\n");

	// Generate call to test functions
    if (g_CommandLine.test)
    {
        for (auto bc : module->byteCodeTestFunc)
        {
			auto node = bc->node;
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
			outputC.addString(format("__%s();\n", node->name.c_str()));
        }
    }

    outputC.addString("}\n");
    return true;
}
