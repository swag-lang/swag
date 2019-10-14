#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Version.h"
#include "Workspace.h"

bool BackendC::emitHeader()
{
    bufferH.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    bufferC.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    bufferSwg.addString(format("/* GENERATED WITH SWAG VERSION %d.%d.%d */\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));

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

bool BackendC::preCompile()
{
    auto targetPath    = module->fromTests ? g_Workspace.targetTestPath.string() : g_Workspace.targetPath.string();
    bufferH.fileName   = targetPath + module->name + ".h";
    bufferC.fileName   = targetPath + module->name + ".c";
    bufferSwg.fileName = targetPath + module->name + ".swg";

    bool ok = true;
    ok &= emitHeader();
    ok &= emitRuntime();
    ok &= emitDataSegment(&module->mutableSegment);
    ok &= emitDataSegment(&module->constantSegment);
    ok &= emitStrings();
    ok &= emitFuncSignatures();
    ok &= emitFunctions();
    ok &= emitGlobalInit();
    ok &= emitGlobalDrop();
    ok &= emitMain();
    ok &= emitFooter();

    ok &= bufferH.flush();
    ok &= bufferC.flush();
    ok &= bufferSwg.flush();

    return ok;
}

bool BackendC::compile(const BuildParameters& buildParameters)
{
#ifdef _WIN32
    BackendCCompilerVS compiler(this, &buildParameters);
#endif
    return compiler.compile();
}
