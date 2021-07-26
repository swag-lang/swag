#include "pch.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"
#include "ErrorIds.h"

bool g_Exiting = false;

extern void help(CommandLineParser& cmdParser, const string& cmd);
extern void help(CommandLineParser& cmdParser);

int main(int argc, const char* argv[])
{
    OS::setup();

    // Arguments
    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);

    // Log default help
    if (argc <= 1)
    {
        help(cmdParser);
        OS::exit(0);
    }

    // Command
    string command = argv[1];
    if (command != "build" &&
        command != "run" &&
        command != "test" &&
        command != "watch" &&
        command != "new" &&
        command != "clean" &&
        command != "list" &&
        command != "get" &&
        command != "version" &&
        command != "script" &&
        command != "env")
    {
        g_Log.error(Utf8::format(Msg0000, argv[1]));
        OS::exit(-1);
    }

    // Log help for a given command
    if (argc == 3 && string(argv[2]) == "--help")
    {
        help(cmdParser, command);
        OS::exit(0);
    }

    // Verify that the swag folder has been registered
    string swagFolder;
    if (!OS::getSwagFolder(swagFolder))
    {
        if (command != "env")
        {
            g_Log.message(Msg0165);
            OS::exit(-1);
        }
    }
    else
    {
        g_CommandLine.exePath = swagFolder;
        g_CommandLine.exePath += "\\";
        fs::path pathF = fs::absolute(argv[0]).string();
        g_CommandLine.exePath += pathF.filename();
    }

    // Process all arguments
    if (!cmdParser.process(command, argc - 2, argv + 2))
        OS::exit(-1);
    if (!g_CommandLine.check())
        OS::exit(-1);

    // Output command line in verbose mode
    if (g_CommandLine.verboseCmdLine)
        g_Log.verbose(cmdParser.buildString(true));

    // Deal with the main command
    if (command == "script")
    {
        g_Workspace.scriptCommand();
    }
    else if (command == "build")
    {
        g_Workspace.build();
    }
    else if (command == "run")
    {
        g_CommandLine.run = true;
        g_Workspace.build();
    }
    else if (command == "test")
    {
        g_CommandLine.test = true;
        g_Workspace.build();
    }
    else if (command == "list")
    {
        g_CommandLine.listDepCmd = true;
        g_CommandLine.computeDep = true;
        g_CommandLine.fetchDep   = false;
        g_Workspace.build();
    }
    else if (command == "get")
    {
        g_CommandLine.getDepCmd  = true;
        g_CommandLine.computeDep = true;
        g_CommandLine.fetchDep   = true;
        g_Workspace.build();
    }
    else if (command == "watch")
    {
        g_Workspace.watchCommand();
    }
    else if (command == "clean")
    {
        g_Workspace.cleanCommand();
    }
    else if (command == "version")
    {
        g_Log.message(Utf8::format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    }
    else if (command == "env")
    {
        g_CommandLine.exePath = fs::absolute(argv[0]).string();
        OS::setSwagFolder(g_CommandLine.exePath.parent_path().string());
    }
    else if (command == "new")
    {
        g_Workspace.newCommand();
    }

    // Prints stats, then exit
    g_Stats.print();

    // To avoid freeing some stuff, and have a fast exit
    g_Exiting = true;
    OS::exit(g_Workspace.numErrors > 0 ? -1 : 0);

    return 0;
}
