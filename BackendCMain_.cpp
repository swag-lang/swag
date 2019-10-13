#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Context.h"
#include "Target.h"

void BackendC::emitArgcArgv()
{
    bufferC.addString("static void convertArgcArgv(int argc, char *argv[])\n");
    bufferC.addString("{\n");

    bufferC.addString(format("static swag_uint64_t argumentsStr[%d];\n", module->buildParameters.target.backendC.maxApplicationArguments));
    bufferC.addString(format("__assert(argc <= %d, __FILE__, __LINE__, \"too many application arguments\");\n", module->buildParameters.target.backendC.maxApplicationArguments));
    bufferC.addString("for(int i = 0; i < argc; i++) {\n");
    bufferC.addString("argumentsStr[i * 2] = (swag_int64_t) argv[i];\n");
    bufferC.addString("argumentsStr[(i * 2) + 1] = (swag_int64_t) strlen(argv[i]);\n");
    bufferC.addString("}\n");
    bufferC.addString("__argumentsSlice[0] = (swag_uint64_t) &argumentsStr[0];\n");
	bufferC.addString("__argumentsSlice[1] = (swag_uint64_t) argc;\n");

    bufferC.addString("}\n\n");
}

bool BackendC::emitMain()
{
    emitSeparator(bufferC, "MAIN");

    bufferC.addString("#ifdef SWAG_IS_EXE\n");
    emitArgcArgv();
    bufferC.addString("void main(int argc, char *argv[]) {\n");

    // Main context
    bufferC.addString("static swag_context_t mainContext;\n");
    SWAG_ASSERT(g_defaultContext.allocator);
    bufferC.addString(format("mainContext.allocator = &%s;\n", g_defaultContext.allocator->callName().c_str()));
    bufferC.addString("swag_tls_id_t contextTlsId = TlsAlloc();\n");
    bufferC.addString("TlsSetValue(contextTlsId, &mainContext);\n");

	// Arguments
	bufferC.addString("convertArgcArgv(argc, argv);\n");

    // Call to global init of this module, and dependencies
    bufferC.addString(format("%s_globalInit(contextTlsId);\n", module->name.c_str()));
    for (auto& k : module->moduleDependenciesNames)
        bufferC.addString(format("%s_globalInit(contextTlsId);\n", k.c_str()));

    // Generate call to test functions
    if (!module->byteCodeTestFunc.empty())
    {
        bufferC.addString("#ifdef SWAG_IS_UNITTEST\n");
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            bufferC.addString(format("%s();\n", bc->callName().c_str()));
        }
        bufferC.addString("#endif\n");
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        bufferC.addString(format("%s();\n", module->byteCodeMainFunc->callName().c_str()));
    }

    // Call to global drop of this module, and dependencies
    bufferC.addString(format("%s_globalDrop();\n", module->name.c_str()));
    for (auto& k : module->moduleDependenciesNames)
        bufferC.addString(format("%s_globalDrop();\n", k.c_str()));

    bufferC.addString("}\n");
    bufferC.addString("#endif\n");
    bufferC.addString("\n");

    return true;
}
