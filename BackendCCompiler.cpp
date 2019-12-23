#include "pch.h"
#include "BackendCCompiler.h"
#include "Os.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Workspace.h"

string BackendCCompiler::getResultFile()
{
    string destFile = g_Workspace.targetPath.string() + buildParameters->destFile;
    string resultFile;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
        resultFile = destFile + buildParameters->postFix + ".lib";
        break;

    case BackendOutputType::DynamicLib:
        resultFile = destFile + buildParameters->postFix + ".dll";
        break;

    case BackendOutputType::Binary:
        resultFile = destFile + buildParameters->postFix + ".exe";
        break;
    }

    return resultFile;
}

bool BackendCCompiler::mustCompile()
{
    if (g_CommandLine.rebuild)
        return true;

    if (!fs::exists(backend->bufferC.path))
        return true;
    auto resultFile = getResultFile();
    if (!fs::exists(resultFile))
        return true;

    auto t1 = OS::getFileWriteTime(resultFile);
    auto t2 = OS::getFileWriteTime(backend->bufferC.path);
    if (t1 >= t2)
        return false;

    return true;
}
