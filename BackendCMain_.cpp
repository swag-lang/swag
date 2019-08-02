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
    bufferC.addString("#ifdef SWAG_IS_EXE\n");
    bufferC.addString("void main() {\n");

    // Call to global init of this module, and dependencies
    bufferC.addString(format("%s_globalInit();\n", module->name.c_str()));
    for (auto& k : module->moduleDependenciesNames)
        bufferC.addString(format("%s_globalInit();\n", k.c_str()));

    // Generate call to test functions
    bufferC.addString("#ifdef SWAG_IS_UNITTEST\n");
    for (auto bc : module->byteCodeTestFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        bufferC.addString(format("%s();\n", node->fullname.c_str()));
    }
    bufferC.addString("#endif\n");

    bufferC.addString("}\n");
    bufferC.addString("#endif\n");
    bufferC.addString("\n");
	
    return true;
}
