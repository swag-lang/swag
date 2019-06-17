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
    bufferC.addString("#ifdef SWAG_HAS_MAIN\n");
    bufferC.addString("void main() {\n");

	// Generate call to test functions
	bufferC.addString("#ifdef SWAG_IS_UNITTEST\n");
    for (auto bc : module->byteCodeTestFunc)
    {
		auto node = bc->node;
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
		bufferC.addString(format("__%s();\n", node->name.c_str()));
    }
	bufferC.addString("#endif\n");

    bufferC.addString("}\n");
    bufferC.addString("#endif\n");
    return true;
}
