#include "pch.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "SemanticJob.h"
#include "ModuleBuildJob.h"
#include "TypeManager.h"
#include "Report.h"

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
        Report::error(Err(Fat0011));
        OS::exit(-1);
    }

    error_code err;
    if (!filesystem::exists(workspacePath, err))
    {
        Report::error(Fmt(Err(Fat0026), workspacePath.string().c_str()));
        g_Log.messageInfo(Nte(Nte0012));
        OS::exit(-1);
    }

    if (!g_CommandLine.scriptCommand && !filesystem::exists(modulesPath, err) && !filesystem::exists(testsPath, err))
    {
        Report::error(Fmt(Err(Fat0016), workspacePath.string().c_str()));
        g_Log.messageInfo(Nte(Nte0012));
        OS::exit(-1);
    }

    if (g_CommandLine.scriptCommand && !filesystem::exists(g_CommandLine.scriptName.c_str(), err))
    {
        Report::error(Fmt(Err(Fat0020), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    g_ThreadMgr.init();
}

void Workspace::setupPaths()
{
    workspacePath = filesystem::absolute(g_CommandLine.workspacePath);
    if (workspacePath.empty())
        workspacePath = filesystem::current_path();

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