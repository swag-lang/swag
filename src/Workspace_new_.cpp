#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Os.h"

void newScriptFile()
{
    ofstream file(g_CommandLine.scriptName);
    if (!file.is_open())
    {
        g_Log.errorOS(Utf8::format(Msg0347, g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    const char* content = R"(
// Swag script file
#dependencies
{
    // Here you can add your external dependencies
    // #import "core" location="swag@std"
}

#run
{
    @print("Hello world !\n")
}

)";

    file << content;

    g_Log.message(Utf8::format("=> script file '%s' has been created", g_CommandLine.scriptName.c_str()));
    g_Log.message(Utf8::format("=> type 'swag script -f:%s' to run that script", g_CommandLine.scriptName.c_str()));
}

void Workspace::newModule(string moduleName)
{
    error_code errorCode;

    // Create one module folder
    auto modulePath = modulesPath;
    modulePath.append(moduleName);

    if (fs::exists(modulePath))
    {
        g_Log.errorOS(Utf8::format(Msg0397, moduleName.c_str()));
        OS::exit(-1);
    }

    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.errorOS(Utf8::format(Msg0818, modulePath.string().c_str()));
        OS::exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        g_Log.errorOS(Utf8::format(Msg0824, cfgFileName.string().c_str()));
        OS::exit(-1);
    }

    const char* oneCfg = R"(
// Swag module configuration file
#dependencies
{
    // Here you can add your external dependencies
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
        g_Log.errorOS(Utf8::format(Msg0818, modulePath.string().c_str()));
        OS::exit(-1);
    }

    modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        g_Log.errorOS(Utf8::format(Msg0824, modulePath.string().c_str()));
        OS::exit(-1);
    }

    const char* oneMain = R"(
#main
{
	@print("Hello world!\n")
}
)";

    file << oneMain;
}

void Workspace::newCommand()
{
    // Create a script file
    if (workspacePath.empty() && !g_CommandLine.scriptName.empty())
    {
        newScriptFile();
        OS::exit(0);
    }

    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error(Msg0540);
        OS::exit(-1);
    }

    // Create workspace
    error_code errorCode;
    string     moduleName;
    if (g_CommandLine.moduleName.empty())
    {
        if (fs::exists(workspacePath))
        {
            g_Log.error(Utf8::format(Msg0817, workspacePath.string().c_str()));
            OS::exit(-1);
        }

        // Create workspace folders
        if (!fs::create_directories(workspacePath, errorCode))
        {
            g_Log.errorOS(Utf8::format(Msg0818, workspacePath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(examplesPath, errorCode))
        {
            g_Log.errorOS(Utf8::format(Msg0818, examplesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(testsPath, errorCode))
        {
            g_Log.errorOS(Utf8::format(Msg0818, testsPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(modulesPath, errorCode))
        {
            g_Log.errorOS(Utf8::format(Msg0818, modulesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(dependenciesPath, errorCode))
        {
            g_Log.errorOS(Utf8::format(Msg0818, dependenciesPath.string().c_str()));
            OS::exit(-1);
        }

        g_Log.message(Utf8::format("=> workspace '%s' has been created", workspacePath.string().c_str()));
        moduleName = workspacePath.filename().string();
    }

    // Use an existing workspace to create a new module
    else
    {
        if (!fs::exists(workspacePath))
        {
            g_Log.error(Utf8::format(Msg0541, workspacePath.string().c_str()));
            OS::exit(-1);
        }

        moduleName = g_CommandLine.moduleName;
    }

    // Create module
    newModule(moduleName);

    g_Log.message(Utf8::format("=> module '%s' has been created", moduleName.c_str()));
    g_Log.message(Utf8::format("=> type 'swag run -w:%s -m:%s' to build and run that module", workspacePath.string().c_str(), moduleName.c_str()));
    OS::exit(0);
}
