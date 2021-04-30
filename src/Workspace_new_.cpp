#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "ErrorIds.h"

void Workspace::newCommand()
{
    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error(Msg0540);
        exit(-1);
    }

    if (fs::exists(workspacePath))
    {
        g_Log.error(format(Msg0817, workspacePath.string().c_str()));
        exit(-1);
    }

    // Create workspace folders
    error_code errorCode;
    if (!fs::create_directories(workspacePath, errorCode))
    {
        g_Log.error(format(Msg0818, workspacePath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(examplesPath, errorCode))
    {
        g_Log.error(format(Msg0818, examplesPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(testsPath, errorCode))
    {
        g_Log.error(format(Msg0818, testsPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(modulesPath, errorCode))
    {
        g_Log.error(format(Msg0818, modulesPath.string().c_str()));
        exit(-1);
    }

    if (!fs::create_directories(dependenciesPath, errorCode))
    {
        g_Log.error(format(Msg0818, dependenciesPath.string().c_str()));
        exit(-1);
    }

    // Create one module folder
    auto modulePath = modulesPath;
    modulePath.append(workspacePath.filename());
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format(Msg0818, modulePath.string().c_str()));
        exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        g_Log.error(format(Msg0824, cfgFileName.string().c_str()));
        exit(-1);
    }

    const char* oneCfg = R"(// Swag generated module
#dependencies
{
    // Add external dependencies
    // #import "core" location="swag@std"

    // Setup the build configuration
    #run
    {
        itf := @compiler()
        cfg := itf.getBuildCfg()
        cfg.moduleVersion  = 0
        cfg.moduleRevision = 0
        cfg.moduleBuildNum = 0
        cfg.backendKind    = .Executable
    }
}
)";
    fileCfg << oneCfg;

    // Create an hello world file
    modulePath.append(SWAG_SRC_FOLDER);
    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.error(format(Msg0818, modulePath.string().c_str()));
        exit(-1);
    }

    modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        g_Log.error(format(Msg0824, modulePath.string().c_str()));
        exit(-1);
    }

    const char* oneMain = R"(
#main
{
	@print("Hello world!\n")
}
)";
    file << oneMain;

    g_Log.message(format("=> workspace '%s' has been created", workspacePath.string().c_str()));
    g_Log.message(format("=> a simple executable module '%s' has also been created", workspacePath.filename().string().c_str()));
    g_Log.message(format("=> type 'swag run -w:%s' to build and run that module", workspacePath.string().c_str()));
    exit(0);
}
