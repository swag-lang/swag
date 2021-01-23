#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"

void Workspace::newCommand()
{
    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error("fatal error: missing workspace folder '--workspace'");
        exit(-1);
    }

    if (fs::exists(workspacePath))
    {
        g_Log.error(format("fatal error: workspace folder '%s' already exists", workspacePath.string().c_str()));
        exit(-1);
    }

    // Create workspace folders
    error_code errorCode;
    if (!fs::create_directories(workspacePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(examplesPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", examplesPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(testsPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", testsPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(modulesPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulesPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(dependenciesPath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", dependenciesPath.string().c_str()));
        exit(-1);
    }

    // Create one module folder
    auto modulePath = modulesPath;
    modulePath.append(workspacePath.filename());
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        g_Log.error(format("fatal error: cannot create file '%s'", cfgFileName.string().c_str()));
        exit(-1);
    }

    const char* oneCfg = R"(
// setup module build configuration
#run
{
    itf := @compiler()
    cfg := itf.getBuildCfg()

    cfg.moduleVersion  = 0
    cfg.moduleRevision = 0
    cfg.moduleBuildNum = 0
}
)";
    fileCfg << oneCfg;

    // Create an hello world file
    modulePath.append(SWAG_SRC_FOLDER);
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format("fatal error: cannot create directory '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        g_Log.error(format("fatal error: cannot create file '%s'", modulePath.string().c_str()));
        exit(-1);
    }

    const char* oneMain = R"(
#main
{
	@print("Hello world!\n")
}
)";
    file << oneMain;

    g_Log.message(format("workspace '%s' has been created", workspacePath.string().c_str()));
    exit(0);
}
