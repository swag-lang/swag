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
#include "Attribute.h"

bool BackendC::emitMain()
{
	emitSeparator(bufferC, "MAIN");
    bufferC.addString("void main() {\n");

	// Generate call to test functions
    if (g_CommandLine.test)
    {
        for (auto bc : module->byteCodeTestFunc)
        {
			auto node = bc->node;
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
			bufferC.addString(format("__%s();\n", node->name.c_str()));
        }
    }

    bufferC.addString("}\n");
    return true;
}
