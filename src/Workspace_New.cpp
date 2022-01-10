#include "pch.h"
#include "Workspace.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "Os.h"

void newScriptFile()
{
    ofstream file(g_CommandLine->scriptName);
    if (!file.is_open())
    {
        g_Log.errorOS(Fmt(g_E[Err0347], g_CommandLine->scriptName.c_str()));
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

    g_Log.message(Fmt("=> script file `%s` has been created", g_CommandLine->scriptName.c_str()));
    g_Log.message(Fmt("=> type 'swag script -f:%s' to run that script", g_CommandLine->scriptName.c_str()));
}

void Workspace::newModule(string moduleName)
{
    error_code errorCode;

    // Create one module folder
    auto modulePath = g_CommandLine->test ? testsPath : modulesPath;
    modulePath.append(moduleName);

    if (fs::exists(modulePath))
    {
        g_Log.errorOS(Fmt(g_E[Err0397], moduleName.c_str()));
        OS::exit(-1);
    }

    if (!fs::create_directories(modulePath, errorCode))
    {
        g_Log.errorOS(Fmt(g_E[Err0818], modulePath.string().c_str()));
        OS::exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        g_Log.errorOS(Fmt(g_E[Err0824], cfgFileName.string().c_str()));
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
        g_Log.errorOS(Fmt(g_E[Err0818], modulePath.string().c_str()));
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

    if (g_CommandLine->test)
        modulePath.append("test1.swg");
    else
        modulePath.append("main.swg");
    ofstream file(modulePath);
    if (!file.is_open())
    {
        g_Log.errorOS(Fmt(g_E[Err0824], modulePath.string().c_str()));
        OS::exit(-1);
    }
    if (g_CommandLine->test)
        file << contentTest;
    else
        file << contentMain;
}

void Workspace::newCommand()
{
    // Create a script file
    if (workspacePath.empty() && !g_CommandLine->scriptName.empty())
    {
        newScriptFile();
        OS::exit(0);
    }

    setupPaths();

    if (workspacePath.empty())
    {
        g_Log.error(g_E[Err0540]);
        OS::exit(-1);
    }

    // Create workspace
    error_code errorCode;
    string     moduleName;
    if (g_CommandLine->moduleName.empty())
    {
        if (fs::exists(workspacePath))
        {
            g_Log.error(Fmt(g_E[Err0817], workspacePath.string().c_str()));
            OS::exit(-1);
        }

        // Create workspace folders
        if (!fs::create_directories(workspacePath, errorCode))
        {
            g_Log.errorOS(Fmt(g_E[Err0818], workspacePath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(examplesPath, errorCode))
        {
            g_Log.errorOS(Fmt(g_E[Err0818], examplesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(testsPath, errorCode))
        {
            g_Log.errorOS(Fmt(g_E[Err0818], testsPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(modulesPath, errorCode))
        {
            g_Log.errorOS(Fmt(g_E[Err0818], modulesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!fs::create_directories(dependenciesPath, errorCode))
        {
            g_Log.errorOS(Fmt(g_E[Err0818], dependenciesPath.string().c_str()));
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
            g_Log.error(Fmt(g_E[Err0541], workspacePath.string().c_str()));
            OS::exit(-1);
        }

        moduleName = g_CommandLine->moduleName;
    }

    // Create module
    newModule(moduleName);

    if (g_CommandLine->test)
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
