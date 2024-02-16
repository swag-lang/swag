#include "pch.h"
#include "Workspace.h"
#include "ErrorIds.h"
#include "Log.h"
#include "ModuleBuildJob.h"
#include "Os.h"
#include "Report.h"
#include "ThreadManager.h"
#include "TypeManager.h"

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
		Report::error(Err(Fat0013));
		OS::exit(-1);
	}

	error_code err;
	if (!exists(workspacePath, err))
	{
		Report::error(FMT(Err(Fat0035), workspacePath.string().c_str()));
		g_Log.messageInfo(Nte(Nte0190));
		OS::exit(-1);
	}

	if (!g_CommandLine.scriptCommand && !exists(modulesPath, err) && !exists(testsPath, err))
	{
		Report::error(FMT(Err(Fat0027), workspacePath.string().c_str()));
		g_Log.messageInfo(Nte(Nte0190));
		OS::exit(-1);
	}

	if (g_CommandLine.scriptCommand && !filesystem::exists(g_CommandLine.scriptName.c_str(), err))
	{
		Report::error(FMT(Err(Fat0030), g_CommandLine.scriptName.c_str()));
		OS::exit(-1);
	}

	g_ThreadMgr.init();
}

void Workspace::setupPaths()
{
	workspacePath = absolute(g_CommandLine.workspacePath);
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
