#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Context.h"

void BackendC::emitArgcArgv(OutputFile& bufferC)
{
    CONCAT_FIXED_STR(bufferC, "static void convertArgcArgv(int argc, char *argv[])\n");
    CONCAT_FIXED_STR(bufferC, "{\n");

    bufferC.addStringFormat("\tstatic swag_uint64_t argumentsStr[%d];\n", module->buildParameters.target.backendC.maxApplicationArguments);
    bufferC.addStringFormat("\t__assert(argc <= %d, __FILE__, __LINE__, \"too many application arguments\");\n", module->buildParameters.target.backendC.maxApplicationArguments);
    bufferC.addEol();

    CONCAT_FIXED_STR(bufferC, "\tfor(int i = 0; i < argc; i++) {\n");
    CONCAT_FIXED_STR(bufferC, "\t\targumentsStr[i * 2] = (swag_int64_t) argv[i];\n");
    CONCAT_FIXED_STR(bufferC, "\t\targumentsStr[(i * 2) + 1] = (swag_int64_t) __strlen(argv[i]);\n");
    CONCAT_FIXED_STR(bufferC, "\t}\n");
    bufferC.addEol();
    CONCAT_FIXED_STR(bufferC, "\t__process_infos.arguments.addr = &argumentsStr[0];\n");
    CONCAT_FIXED_STR(bufferC, "\t__process_infos.arguments.count = (swag_uint64_t) argc;\n");

    CONCAT_FIXED_STR(bufferC, "}\n\n");
}

bool BackendC::emitMain(OutputFile& bufferC)
{
    emitSeparator(bufferC, "MAIN");

    CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_IS_BINARY\n");
    emitArgcArgv(bufferC);
    CONCAT_FIXED_STR(bufferC, "int main(int argc, char *argv[])\n");
    CONCAT_FIXED_STR(bufferC, "{\n");

    // Main context
    CONCAT_FIXED_STR(bufferC, "\tstatic swag_context_t mainContext;\n");
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    bufferC.addStringFormat("\tstatic swag_allocator_t defaultAllocTable = &%s;\n", bcAlloc->callName().c_str());
    CONCAT_FIXED_STR(bufferC, "\tmainContext.allocator.itable = &defaultAllocTable;\n");
    CONCAT_FIXED_STR(bufferC, "\t__process_infos.contextTlsId = __tlsAlloc();\n");
    CONCAT_FIXED_STR(bufferC, "\t__process_infos.defaultContext = &mainContext;\n");
    CONCAT_FIXED_STR(bufferC, "\t__tlsSetValue(__process_infos.contextTlsId, __process_infos.defaultContext);\n");

    // Arguments
    bufferC.addEol();
    CONCAT_FIXED_STR(bufferC, "\tconvertArgcArgv(argc, argv);\n");
    bufferC.addEol();

    // Call to global init of this module, and dependencies
    bufferC.addStringFormat("\t%s_globalInit(&__process_infos);\n", module->nameDown.c_str());
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        bufferC.addStringFormat("\t__loadDynamicLibrary(\"%s\");\n", nameDown.c_str());
        if (dep.second.generated)
        {
            bufferC.addStringFormat("\textern SWAG_IMPORT void %s_globalInit(struct swag_process_infos_t *);\n", nameDown.c_str());
            bufferC.addStringFormat("\t%s_globalInit(&__process_infos);\n", nameDown.c_str());
        }
    }

    bufferC.addEol();

    // Generate call to test functions
    if (!module->byteCodeTestFunc.empty())
    {
        CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_HAS_TEST\n");
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            bufferC.addStringFormat("\t%s();\n", bc->callName().c_str());
        }
        CONCAT_FIXED_STR(bufferC, "#endif\n");
        bufferC.addEol();
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        bufferC.addStringFormat("\t%s();\n", module->byteCodeMainFunc->callName().c_str());
        bufferC.addEol();
    }

    // Call to global drop of this module, and dependencies
    bufferC.addStringFormat("\t%s_globalDrop();\n", module->nameDown.c_str());
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep.second.generated)
            continue;
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        bufferC.addStringFormat("\textern SWAG_IMPORT void %s_globalDrop();\n", nameDown.c_str());
        bufferC.addStringFormat("\t%s_globalDrop();\n", nameDown.c_str());
    }

    CONCAT_FIXED_STR(bufferC, "return 0;\n");
    CONCAT_FIXED_STR(bufferC, "}\n");
    CONCAT_FIXED_STR(bufferC, "#endif /* SWAG_IS_BINARY */\n");
    bufferC.addEol();

    return true;
}
