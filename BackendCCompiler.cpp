#include "pch.h"
#include "BackendCCompiler.h"
#include "Os.h"
#include "BackendC.h"
#include "Workspace.h"

string BackendCCompiler::getResultFile(const BuildParameters& buildParameters)
{
    string destFile = g_Workspace.targetPath.string() + buildParameters.destFile;
    string resultFile;
    switch (buildParameters.type)
    {
    case BackendOutputType::StaticLib:
        resultFile = destFile + buildParameters.postFix + ".lib";
        break;

    case BackendOutputType::DynamicLib:
        resultFile = destFile + buildParameters.postFix + ".dll";
        break;

    case BackendOutputType::Binary:
        resultFile = destFile + buildParameters.postFix + ".exe";
        break;
    }

    return resultFile;
}
