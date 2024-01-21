#include "pch.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "Os.h"
#include "Report.h"
#include "Workspace.h"

void newScriptFile()
{
    ofstream file(g_CommandLine.scriptName);
    if (!file.is_open())
    {
        Report::errorOS(Fmt(Err(Fat0020), g_CommandLine.scriptName.c_str()));
        OS::exit(-1);
    }

    const char* content = R"(
// Swag script file
#dependencies
{
    // Here you can add your external dependencies
    // #import "core" location="swag@std"
}

#main
{
    @print("Hello world !\n")
}

)";

    file << content;

    g_Log.messageInfo(Fmt("=> script file [[%s]] has been created", g_CommandLine.scriptName.c_str()));
    g_Log.messageInfo(Fmt("=> type [[swag script -f:%s]] or [[swag %s]] to run that script", g_CommandLine.scriptName.c_str(), g_CommandLine.scriptName.c_str()));
}

void Workspace::newModule(const Utf8& moduleName)
{
    error_code err;

    // Create one module folder
    auto modulePath = g_CommandLine.test ? testsPath : modulesPath;
    modulePath.append(moduleName.c_str());

    if (filesystem::exists(modulePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0028), moduleName.c_str()));
        OS::exit(-1);
    }

    if (!filesystem::create_directories(modulePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0017), modulePath.string().c_str()));
        OS::exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    ofstream fileCfg(cfgFileName);
    if (!fileCfg.is_open())
    {
        Report::errorOS(Fmt(Err(Fat0018), cfgFileName.string().c_str()));
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
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
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
    if (!filesystem::create_directories(modulePath, err))
    {
        Report::errorOS(Fmt(Err(Fat0017), modulePath.string().c_str()));
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
        Report::errorOS(Fmt(Err(Fat0018), modulePath.string().c_str()));
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
        Report::error(Err(Fat0013));
        OS::exit(-1);
    }

    // Create workspace
    error_code err;
    Utf8       moduleName;
    if (g_CommandLine.moduleName.empty())
    {
        if (filesystem::exists(workspacePath, err))
        {
            Report::error(Fmt(Err(Fat0034), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        // Create workspace folders
        if (!filesystem::create_directories(workspacePath, err))
        {
            Report::errorOS(Fmt(Err(Fat0017), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        if (!filesystem::create_directories(testsPath, err))
        {
            Report::errorOS(Fmt(Err(Fat0017), testsPath.string().c_str()));
            OS::exit(-1);
        }

        if (!filesystem::create_directories(modulesPath, err))
        {
            Report::errorOS(Fmt(Err(Fat0017), modulesPath.string().c_str()));
            OS::exit(-1);
        }

        if (!filesystem::create_directories(dependenciesPath, err))
        {
            Report::errorOS(Fmt(Err(Fat0017), dependenciesPath.string().c_str()));
            OS::exit(-1);
        }

        g_Log.messageInfo(Fmt("=> workspace [[%s]] has been created", workspacePath.string().c_str()));
        moduleName = workspacePath.filename().string();
    }

    // Use an existing workspace to create a new module
    else
    {
        if (!filesystem::exists(workspacePath, err))
        {
            Report::error(Fmt(Err(Fat0035), workspacePath.string().c_str()));
            OS::exit(-1);
        }

        moduleName = g_CommandLine.moduleName;
    }

    // Create module
    newModule(moduleName);

    if (g_CommandLine.test)
    {
        g_Log.messageInfo(Fmt("=> test module [[%s]] has been created", moduleName.c_str()));
        g_Log.messageInfo(Fmt("=> type [[swag test -w:%s -m:%s]] to test that module only", workspacePath.string().c_str(), moduleName.c_str()));
        g_Log.messageInfo(Fmt("=> type [[swag test -w:%s]] to test all modules", workspacePath.string().c_str(), moduleName.c_str()));
    }
    else
    {
        g_Log.messageInfo(Fmt("=> module [[%s]] has been created", moduleName.c_str()));
        g_Log.messageInfo(Fmt("=> type [[swag run -w:%s -m:%s]] to build and run that module", workspacePath.string().c_str(), moduleName.c_str()));
    }

    OS::exit(0);
}
