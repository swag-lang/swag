#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Os.h"

void newScriptFile()
{
    ofstream file(g_CommandLine.scriptName);
    if (!file.is_open())
    {
        Report::errorOS(Fmt(Err(CEr0007), g_CommandLine.scriptName.c_str()));
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

    g_Log.message(Fmt("=> script file `%s` has been created", g_CommandLine.scriptName.c_str()));
    g_Log.message(Fmt("=> type 'swag script -f:%s' to run that script", g_CommandLine.scriptName.c_str()));
}

void Workspace::newModule(string moduleName)
{
    error_code errorCode;

    // Create one module folder
    auto modulePath = g_CommandLine.test ? testsPath : modulesPath;
    modulePath.append(moduleName);

    if (fs::exists(modulePath))
    {
        Report::errorOS(Fmt(Err(CEr0018), moduleName.c_str()));
        OS::exit(-1);
    }

    if (!fs::create_directories(modulePath, errorCode))
    {
        Report::errorOS(Fmt(Err(CEr0004), modulePath.string().c_str()));
        OS::exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        Report::errorOS(Fmt(Err(CEr0005), cfgFileName.string().c_str()));
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
        Report::errorOS(Fmt(Err(CEr0004), modulePath.string().c_str()));
        OS::exit(-1);
    }

    const char* contentMain = R"(
#main
{
	@print("Hello world!\n")
}
)";

    const char* contentTest = R"(
#test
{
	const X = (2 * 5) + 3
    @assert(X == 13)
}
)";

    if (g_CommandLine.test)
        modulePath.append("test1.swg");
    else
        modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        Report::errorOS(Fmt(Err(CEr0005), modulePath.string().c_str()));
        OS::exit(-1);
    }
    if (g_CommandLine.test)
        file << contentTest;
    else
        file << contentMain;
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
        Report::error(Err(CEr0011));
        OS::exit(-1);
    }

    // Create workspace
    error_code errorCode;
    string     moduleName;
    if (g_CommandLine.moduleName.empty())
    {
        if (fs::exists(workspacePath))
        {
            Report::error(Fmt(Err(CEr0025), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        // Create workspace folders
        if (!fs::create_directories(workspacePath, errorCode))
        {
            Report::errorOS(Fmt(Err(CEr0004), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(examplesPath.parent_path(), errorCode))
        {
            Report::errorOS(Fmt(Err(CEr0004), examplesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(testsPath.parent_path(), errorCode))
        {
            Report::errorOS(Fmt(Err(CEr0004), testsPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(modulesPath.parent_path(), errorCode))
        {
            Report::errorOS(Fmt(Err(CEr0004), modulesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(dependenciesPath.parent_path(), errorCode))
        {
            Report::errorOS(Fmt(Err(CEr0004), dependenciesPath.string().c_str()));
            OS::exit(-1);
        }

        g_Log.message(Fmt("=> workspace `%s` has been created", workspacePath.string().c_str()));
        moduleName = workspacePath.filename().string();
    }

    // Use an existing workspace to create a new module
    else
    {
        if (!fs::exists(workspacePath))
        {
            Report::error(Fmt(Err(CEr0026), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        moduleName = g_CommandLine.moduleName;
    }

    // Create module
    newModule(moduleName);

    if (g_CommandLine.test)
    {
        g_Log.message(Fmt("=> test module `%s` has been created", moduleName.c_str()));
        g_Log.message(Fmt("=> type 'swag test -w:%s -m:%s' to test that module only", workspacePath.string().c_str(), moduleName.c_str()));
        g_Log.message(Fmt("=> type 'swag test -w:%s to test all modules", workspacePath.string().c_str(), moduleName.c_str()));
    }
    else
    {
        g_Log.message(Fmt("=> module `%s` has been created", moduleName.c_str()));
        g_Log.message(Fmt("=> type 'swag run -w:%s -m:%s' to build and run that module", workspacePath.string().c_str(), moduleName.c_str()));
    }

    OS::exit(0);
}
