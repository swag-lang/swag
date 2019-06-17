#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "CommandLine.h"
#include "Version.h"
#include "ThreadManager.h"
#include "Workspace.h"
#include "ModuleCompileJob.h"

bool BackendC::writeFile(const char* fileName, Concat& concat)
{
    FILE* file = nullptr;
    fopen_s(&file, fileName, "wt");
    SWAG_VERIFY(file, module->error(format("can't open file '%s' for writing'", fileName)));

    auto bucket = concat.firstBucket;
    while (bucket)
    {
        int count = bucket->count;
        fwrite(bucket->datas, 1, count, file);
        bucket = bucket->nextBucket;
    }

    fclose(file);
    return true;
}

bool BackendC::emitHeader()
{
    bufferH.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_VERSION, SWAG_REVISION, SWAG_BUILD));
    bufferC.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_VERSION, SWAG_REVISION, SWAG_BUILD));
    bufferSwg.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_VERSION, SWAG_REVISION, SWAG_BUILD));

    // My include file. Need to export for dlls
    bufferC.addString("#ifdef SWAG_IS_DLL\n");
    bufferC.addString("#define SWAG_EXPORT\n");
    bufferC.addString("#endif\n");
    bufferC.addString(format("#include \"%s.h\"\n", module->name.c_str()));

    // My dependencies. Need to import for dlls
    for (auto depName : module->moduleDependenciesNames)
    {
        bufferC.addString("#undef SWAG_EXPORT\n");
        bufferC.addString(format("#include \"%s.h\"\n", depName.c_str()));
    }

    bufferH.addString(format("#ifndef __SWAG_%s__\n", module->nameUp.c_str()));
    bufferH.addString(format("#define __SWAG_%s__\n", module->nameUp.c_str()));

    bufferSwg.addString(format("namespace %s {\n\n", module->name.c_str()));

    return true;
}

bool BackendC::emitFooter()
{
    bufferH.addString(format("#endif /* __SWAG_%s__ */\n", module->nameUp.c_str()));
    bufferSwg.addString(format("\n} // namespace %s\n", module->name.c_str()));
    return true;
}

void BackendC::emitSeparator(Concat& buffer, const char* title)
{
    int len = (int) strlen(title);
    buffer.addString("/*");
    int maxLen = 80;

    int i = 0;
    for (; i < 4; i++)
        buffer.addString("#");
    buffer.addString(" ");
    buffer.addString(title);
    buffer.addString(" ");
    i += len + 2;

    for (; i < maxLen; i++)
        buffer.addString("#");
    buffer.addString("*/\n");
}

bool BackendC::compile(const BackendParameters& backendParameters)
{
    if (module->buildPass != BuildPass::Full)
        return true;

    BackendCCompilerVS compiler(this);
    compiler.backendParameters = backendParameters;
    SWAG_CHECK(compiler.compile());

    // Compile a specific version, to test it
    if (g_CommandLine.unittest && g_CommandLine.runBackendTests && !module->byteCodeTestFunc.empty())
    {
        compiler.backendParameters.type    = BackendType::Exe;
        compiler.backendParameters.postFix = ".test";
        compiler.backendParameters.defines.clear();
        compiler.backendParameters.defines.push_back("SWAG_IS_UNITTEST");
        SWAG_CHECK(compiler.compile());
        SWAG_CHECK(compiler.runTests());
    }

    return true;
}

bool BackendC::generate()
{
    bool ok = true;

    ok &= emitHeader();
    ok &= emitRuntime();
    ok &= emitDataSegment();
    ok &= emitFuncSignatures();
    ok &= emitFunctions();
    ok &= emitMain();
    ok &= emitFooter();

    auto workspace = module->workspace;
    destFileH      = workspace->cachePath.string() + module->name + ".h";
    destFileC      = workspace->cachePath.string() + module->name + ".c";
    destFileSwg    = workspace->cachePath.string() + module->name + ".swg";
    destFile       = workspace->cachePath.string() + module->name;
    SWAG_CHECK(writeFile(destFileH.c_str(), bufferH));
    SWAG_CHECK(writeFile(destFileC.c_str(), bufferC));
    SWAG_CHECK(writeFile(destFileSwg.c_str(), bufferSwg));

    return true;
}