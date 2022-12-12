#include "pch.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"
#include "Profiler.h"
#include "CommandLine.h"

bool g_Exiting = false;

extern void help(CommandLineParser& cmdParser, const string& cmd);
extern void help(CommandLineParser& cmdParser);

int main(int argc, const char* argv[])
{
    g_CommandLine = g_Allocator.alloc<CommandLine>();
    g_Workspace   = g_Allocator.alloc<Workspace>();

    OS::setup();
    initErrors();

    // Arguments
    CommandLineParser cmdParser;
    cmdParser.setup(g_CommandLine);

    // Log default help
    if (argc <= 1)
    {
        help(cmdParser);
        OS::exit(0);
    }

    // Special mode if the first argument is a script file with ".swgs" extension
    string   command = argv[1];
    fs::path p       = argv[1];
    if (p.extension() == ".swgs")
    {
        command                   = "script";
        g_CommandLine->scriptName = argv[1];
    }

    // Command
    if (command != "build" &&
        command != "run" &&
        command != "test" &&
        command != "new" &&
        command != "clean" &&
        command != "list" &&
        command != "get" &&
        command != "version" &&
        command != "script")
    {
        Report::error(Fmt(Err(Err0000), argv[1]));
        OS::exit(-1);
    }

    // Log help for a given command
    if (argc == 3 && string(argv[2]) == "--help")
    {
        help(cmdParser, command);
        OS::exit(0);
    }

    // Verify that the swag folder has been registered
    fs::path pathF         = fs::absolute(OS::getExePath().c_str()).string();
    g_CommandLine->exePath = pathF.string();

    // Process all arguments
    if (!cmdParser.process(command, argc - 2, argv + 2))
        OS::exit(-1);
    if (!g_CommandLine->check())
        OS::exit(-1);

    // Output command line in verbose mode
    if (g_CommandLine->verboseCmdLine)
        g_Log.verbose(cmdParser.buildString(true));

    // Deal with the main command
    if (command == "script")
    {
        g_Workspace->scriptCommand();
    }
    else if (command == "build")
    {
        g_Workspace->build();
    }
    else if (command == "run")
    {
        g_CommandLine->run = true;
        g_Workspace->build();
    }
    else if (command == "test")
    {
        g_CommandLine->test = true;
        g_Workspace->build();
    }
    else if (command == "list")
    {
        g_CommandLine->listDepCmd = true;
        g_CommandLine->computeDep = true;
        g_CommandLine->fetchDep   = false;
        g_Workspace->build();
    }
    else if (command == "get")
    {
        g_CommandLine->getDepCmd  = true;
        g_CommandLine->computeDep = true;
        g_CommandLine->fetchDep   = true;
        g_Workspace->build();
    }
    else if (command == "clean")
    {
        g_Workspace->cleanCommand();
    }
    else if (command == "version")
    {
        g_Log.message(Fmt("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    }
    else if (command == "new")
    {
        g_Workspace->newCommand();
    }

    // Prints stats, then exit
    g_Stats.print();
    profiler();

    // To avoid freeing some stuff, and have a fast exit
    g_Exiting = true;
    OS::exit(g_Workspace->numErrors > 0 ? -1 : 0);

    return 0;
}
