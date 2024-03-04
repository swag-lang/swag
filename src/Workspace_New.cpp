#include "pch.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "Log.h"
#include "Os.h"
#include "Report.h"
#include "Workspace.h"

void newScriptFile()
{
    std::ofstream file(g_CommandLine.fileName);
    if (!file.is_open())
    {
        Report::errorOS(formErr(Fat0020, g_CommandLine.fileName.c_str()));
        OS::exit(-1);
    }

    const auto content = R"(
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

    g_Log.messageInfo(form("=> script file [[%s]] has been created", g_CommandLine.fileName.c_str()));
    g_Log.messageInfo(form("=> type [[swag script -f:%s]] or [[swag %s]] to run that script", g_CommandLine.fileName.c_str(), g_CommandLine.fileName.c_str()));
}

void Workspace::newModule(const Utf8& moduleName) const
{
    std::error_code err;

    // Create one module folder
    auto modulePath = g_CommandLine.test ? testsPath : modulesPath;
    modulePath.append(moduleName.c_str());

    if (std::filesystem::exists(modulePath, err))
    {
        Report::errorOS(formErr(Fat0028, moduleName.c_str()));
        OS::exit(-1);
    }

    if (!std::filesystem::create_directories(modulePath, err))
    {
        Report::errorOS(formErr(Fat0017, modulePath.c_str()));
        OS::exit(-1);
    }

    // Create a configuration file
    auto cfgFileName = modulePath;
    cfgFileName.append(SWAG_CFG_FILE);
    std::ofstream fileCfg(cfgFileName.c_str());
    if (!fileCfg.is_open())
    {
        Report::errorOS(formErr(Fat0018, cfgFileName.c_str()));
        OS::exit(-1);
    }

    const auto oneCfg = R"(
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
    if (!std::filesystem::create_directories(modulePath, err))
    {
        Report::errorOS(formErr(Fat0017, modulePath.c_str()));
        OS::exit(-1);
    }

    const auto contentMain = R"(
#main
{
	@print("Hello world!\n")
}
)";

    const auto contentTest = R"(
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
    std::ofstream file(modulePath.c_str());
    if (!file.is_open())
    {
        Report::errorOS(formErr(Fat0018, modulePath.c_str()));
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
    if (workspacePath.empty() && !g_CommandLine.fileName.empty())
    {
        newScriptFile();
        OS::exit(0);
    }

    setupPaths();

    if (workspacePath.empty())
    {
        Report::error(toErr(Fat0013));
        OS::exit(-1);
    }

    // Create workspace
    std::error_code err;
    Utf8            moduleName;
    if (g_CommandLine.moduleName.empty())
    {
        if (std::filesystem::exists(workspacePath, err))
        {
            Report::error(formErr(Fat0034, workspacePath.c_str()));
            OS::exit(-1);
        }

        // Create workspace folders
        if (!std::filesystem::create_directories(workspacePath, err))
        {
            Report::errorOS(formErr(Fat0017, workspacePath.c_str()));
            OS::exit(-1);
        }

        if (!std::filesystem::create_directories(testsPath, err))
        {
            Report::errorOS(formErr(Fat0017, testsPath.c_str()));
            OS::exit(-1);
        }

        if (!std::filesystem::create_directories(modulesPath, err))
        {
            Report::errorOS(formErr(Fat0017, modulesPath.c_str()));
            OS::exit(-1);
        }

        if (!std::filesystem::create_directories(dependenciesPath, err))
        {
            Report::errorOS(formErr(Fat0017, dependenciesPath.c_str()));
            OS::exit(-1);
        }

        g_Log.messageInfo(form("=> workspace [[%s]] has been created", workspacePath.c_str()));
        moduleName = workspacePath.filename();
    }

    // Use an existing workspace to create a new module
    else
    {
        if (!std::filesystem::exists(workspacePath, err))
        {
            Report::error(formErr(Fat0035, workspacePath.c_str()));
            OS::exit(-1);
        }

        moduleName = g_CommandLine.moduleName;
    }

    // Create module
    newModule(moduleName);

    if (g_CommandLine.test)
    {
        g_Log.messageInfo(form("=> test module [[%s]] has been created", moduleName.c_str()));
        g_Log.messageInfo(form("=> type [[swag test -w:%s -m:%s]] to test that module only", workspacePath.c_str(), moduleName.c_str()));
        g_Log.messageInfo(form("=> type [[swag test -w:%s]] to test all modules", workspacePath.c_str(), moduleName.c_str()));
    }
    else
    {
        g_Log.messageInfo(form("=> module [[%s]] has been created", moduleName.c_str()));
        g_Log.messageInfo(form("=> type [[swag run -w:%s -m:%s]] to build and run that module", workspacePath.c_str(), moduleName.c_str()));
    }

    OS::exit(0);
}
