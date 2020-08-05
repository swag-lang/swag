#include "pch.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Context.h"

bool BackendC::emitMain(OutputFile& bufferC)
{
    //CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_IS_BINARY\n");
    CONCAT_FIXED_STR(bufferC, "int main(int argc, char *argv[]){\n");

    // Main context
    CONCAT_FIXED_STR(bufferC, "static swag_context_t mainContext;\n");
    SWAG_ASSERT(g_defaultContext.allocator.itable);
    auto bcAlloc = (ByteCode*) undoByteCodeLambda(((void**) g_defaultContext.allocator.itable)[0]);
    SWAG_ASSERT(bcAlloc);
    bufferC.addStringFormat("static swag_allocator_t defaultAllocTable=&%s;\n", bcAlloc->callName().c_str());
    CONCAT_FIXED_STR(bufferC, "mainContext.allocator.itable=&defaultAllocTable;\n");
    CONCAT_FIXED_STR(bufferC, "__process_infos.contextTlsId=swag_runtime_tlsAlloc();\n");
    CONCAT_FIXED_STR(bufferC, "__process_infos.defaultContext=&mainContext;\n");
    CONCAT_FIXED_STR(bufferC, "swag_runtime_tlsSetValue(__process_infos.contextTlsId,__process_infos.defaultContext);\n");

    // Arguments
    CONCAT_FIXED_STR(bufferC, "swag_runtime_convertArgcArgv(&__process_infos.arguments,argc,(void**)argv);\n");

    // Call to global init of this module, and dependencies
    bufferC.addStringFormat("%s_globalInit(&__process_infos);\n", module->nameDown.c_str());
    for (const auto& dep : module->moduleDependencies)
    {
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        bufferC.addStringFormat("swag_runtime_loadDynamicLibrary(\"%s\");\n", nameDown.c_str());
        if (dep.second.generated)
        {
            bufferC.addStringFormat("extern SWAG_IMPORT void %s_globalInit(struct swag_process_infos_t*);\n", nameDown.c_str());
            bufferC.addStringFormat("%s_globalInit(&__process_infos);\n", nameDown.c_str());
        }
    }

    bufferC.addEol();

    // Call to test functions
    if (!module->byteCodeTestFunc.empty())
    {
        CONCAT_FIXED_STR(bufferC, "#ifdef SWAG_HAS_TEST\n");
        for (auto bc : module->byteCodeTestFunc)
        {
            auto node = bc->node;
            if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if (numPreCompileBuffers > 1)
                bufferC.addStringFormat("extern void %s();\n", bc->callName().c_str());
            bufferC.addStringFormat("%s();\n", bc->callName().c_str());
        }
        CONCAT_FIXED_STR(bufferC, "#endif\n");
        bufferC.addEol();
    }

    // Call to main
    if (module->byteCodeMainFunc)
    {
        if (numPreCompileBuffers > 1)
            bufferC.addStringFormat("extern void %s();\n", module->byteCodeMainFunc->callName().c_str());
        bufferC.addStringFormat("%s();\n", module->byteCodeMainFunc->callName().c_str());
    }

    // Call to global drop of this module
    bufferC.addStringFormat("%s_globalDrop();\n", module->nameDown.c_str());

    // Call to global drop of all dependencies
    for (const auto& dep : module->moduleDependencies)
    {
        if (!dep.second.generated)
            continue;
        auto nameDown = dep.first;
        nameDown.replaceAll('.', '_');
        bufferC.addStringFormat("extern SWAG_IMPORT void %s_globalDrop();\n", nameDown.c_str());
        bufferC.addStringFormat("%s_globalDrop();\n", nameDown.c_str());
    }

    CONCAT_FIXED_STR(bufferC, "return 0;\n");
    CONCAT_FIXED_STR(bufferC, "}\n");
    //CONCAT_FIXED_STR(bufferC, "#endif /* SWAG_IS_BINARY */\n");
    bufferC.addEol();

    return true;
}

bool BackendC::emitGlobalInit(OutputFile& bufferC)
{
    bufferC.addStringFormat("SWAG_EXPORT void %s_globalInit(swag_process_infos_t*processInfos){\n", module->nameDown.c_str());
    CONCAT_FIXED_STR(bufferC, "__process_infos=*processInfos;\n");

    // Initialize data segments
    CONCAT_FIXED_STR(bufferC, "initDataSeg();\n");
    CONCAT_FIXED_STR(bufferC, "initConstantSeg();\n");

    // Call to #init functions
    for (auto bc : module->byteCodeInitFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if (numPreCompileBuffers > 1)
            bufferC.addStringFormat("extern void %s();\n", bc->callName().c_str());
        bufferC.addStringFormat("%s();\n", bc->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n");
    bufferC.addEol();

    return true;
}

bool BackendC::emitGlobalDrop(OutputFile& bufferC)
{
    // Main init fct
    bufferC.addStringFormat("SWAG_EXPORT void %s_globalDrop(){\n", module->nameDown.c_str());

    for (auto bc : module->byteCodeDropFunc)
    {
        auto node = bc->node;
        if (node && node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if (numPreCompileBuffers > 1)
            bufferC.addStringFormat("extern void %s();\n", bc->callName().c_str());
        bufferC.addStringFormat("%s();\n", bc->callName().c_str());
    }

    CONCAT_FIXED_STR(bufferC, "}\n");
    bufferC.addEol();

    return true;
}