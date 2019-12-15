#include "pch.h"
#include "BackendCCompiler.h"
#include "Os.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"

string BackendCCompiler::getResultFile()
{
    string resultFile;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".lib";
        break;

    case BackendOutputType::DynamicLib:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".dll";
        break;

    case BackendOutputType::Binary:
        resultFile = buildParameters->destFile + buildParameters->postFix + ".exe";
        break;
    }

    return resultFile;
}

bool BackendCCompiler::mustCompile()
{
    if (g_CommandLine.rebuild)
        return true;

    if (!fs::exists(backend->bufferC.fileName))
        return true;
    auto resultFile = getResultFile();
    if (!fs::exists(resultFile))
        return true;

    auto t1 = OS::getFileWriteTime(resultFile);
    auto t2 = OS::getFileWriteTime(backend->bufferC.fileName);
    if (t1 >= t2)
        return false;

    return true;
}
