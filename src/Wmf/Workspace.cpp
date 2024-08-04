#include "pch.h"
#include "Wmf/Workspace.h"
#include "Jobs/ModuleBuildJob.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Threading/ThreadManager.h"

Workspace* g_Workspace = nullptr;

void Workspace::setup()
{
    g_TypeMgr = Allocator::alloc<TypeManager>();
    g_TypeMgr->setup();

    setupPaths();
    setupInternalTags();
    setupUserTags();

    if (workspacePath.empty())
    {
        Report::error(toErr(Fat0013));
        OS::exit(-1);
    }

    std::error_code err;
    if (!std::filesystem::exists(workspacePath, err))
    {
        Report::error(formErr(Fat0035, workspacePath.c_str()));
        g_Log.messageInfo(toNte(Nte0205));
        OS::exit(-1);
    }

    if (!g_CommandLine.scriptCommand && !std::filesystem::exists(modulesPath, err) && !std::filesystem::exists(testsPath, err))
    {
        Report::error(formErr(Fat0027, workspacePath.c_str()));
        g_Log.messageInfo(toNte(Nte0205));
        OS::exit(-1);
    }

    if (g_CommandLine.scriptCommand && !std::filesystem::exists(g_CommandLine.fileName.c_str(), err))
    {
        Report::error(formErr(Fat0030, g_CommandLine.fileName.c_str()));
        OS::exit(-1);
    }

    g_ThreadMgr.init();
}

void Workspace::setupPaths()
{
    workspacePath = std::filesystem::absolute(g_CommandLine.workspacePath);
    if (workspacePath.empty())
        workspacePath = std::filesystem::current_path();

    testsPath = workspacePath;
    testsPath.append(SWAG_TESTS_FOLDER);

    modulesPath = workspacePath;
    modulesPath.append(SWAG_MODULES_FOLDER);

    dependenciesPath = workspacePath;
    dependenciesPath.append(SWAG_DEPENDENCIES_FOLDER);
}

void Workspace::setupCachePath()
{
    cachePath = g_CommandLine.cachePath;
    if (cachePath.empty())
    {
        cachePath = OS::getTemporaryFolder();
        if (cachePath.empty())
        {
            cachePath = workspacePath;
            cachePath.append(SWAG_OUTPUT_FOLDER);
        }
    }
}
