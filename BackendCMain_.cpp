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

    bufferC.addString(format("\tstatic swag_uint64_t argumentsStr[%d];\n", module->buildParameters.target.backendC.maxApplicationArguments));
    bufferC.addString(format("\t__assert(argc <= %d, __FILE__, __LINE__, \"too many application arguments\");\n", module->buildParameters.target.backendC.maxApplicationArguments));
    bufferC.addString("\n");

    bufferC.addString("\tfor(int i = 0; i < argc; i++) {\n");
    bufferC.addString("\t\targumentsStr[i * 2] = (swag_int64_t) argv[i];\n");
    bufferC.addString("\t\targumentsStr[(i * 2) + 1] = (swag_int64_t) strlen(argv[i]);\n");
    bufferC.addString("\t}\n");
    bufferC.addString("\n");
    bufferC.addString("\t__argumentsSlice[0] = (swag_uint64_t) &argumentsStr[0];\n");
    bufferC.addString("\t__argumentsSlice[1] = (swag_uint64_t) argc;\n");

    bufferC.addString("}\n\n");
}

bool BackendC::emitMain()
{
    emitSeparator(bufferC, "MAIN");

    bufferC.addString("#ifdef SWAG_IS_BINARY\n");
    emitArgcArgv();
    bufferC.addString("void main(int argc, char *argv[])\n");
    bufferC.addString("{\n");

    // Main context
    bufferC.addString("\tstatic swag_context_t mainContext;\n");
    SWAG_ASSERT(g_defaultContext.allocator);
    bufferC.addString(format("\tmainContext.allocator = &%s;\n", g_defaultContext.allocator->callName().c_str()));
    bufferC.addString("\t__contextTlsId = __tlsAlloc();\n");
	bufferC.addString("\t__defaultContext = &mainContext;\n");
    bufferC.addString("\t__tlsSetValue(__contextTlsId, __defaultContext);\n");

    // Arguments
    bufferC.addString("\n");
    bufferC.addString("\tconvertArgcArgv(argc, argv);\n");
    bufferC.addString("\n");

    // Call to global init of this module, and dependencies
    bufferC.addString(format("\t%s_globalInit(__contextTlsId, __defaultContext);\n", module->name.c_str()));
    for (auto& k : module->moduleDependenciesNames)
    {
        bufferC.addString(format("\t__loadDynamicLibrary(\"%s\");\n", k.c_str()));
        bufferC.addString(format("\t%s_globalInit(__contextTlsId, __defaultContext);\n", k.c_str()));
    }

    bufferC.addString("\n");

    // Generate call to test functions
    if (!module->byteCodeTestFunc.empty())
    {
        bufferC.addString("#ifdef SWAG_HAS_TEST\n");
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            bufferC.addString(format("\t%s();\n", bc->callName().c_str()));
        }
        bufferC.addString("#endif\n");
        bufferC.addString("\n");
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        bufferC.addString(format("\t%s();\n", module->byteCodeMainFunc->callName().c_str()));
        bufferC.addString("\n");
    }

    // Call to global drop of this module, and dependencies
    bufferC.addString(format("\t%s_globalDrop();\n", module->name.c_str()));
    for (auto& k : module->moduleDependenciesNames)
        bufferC.addString(format("\t%s_globalDrop();\n", k.c_str()));

    bufferC.addString("}\n");
    bufferC.addString("#endif /* SWAG_IS_BINARY */\n");
    bufferC.addString("\n");

    return true;
}
