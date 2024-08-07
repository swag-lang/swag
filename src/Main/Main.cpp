#include "pch.h"
#include "Main/CommandLine.h"
#include "Main/CommandLineParser.h"
#include "Main/Statistics.h"
#include "Main/Version.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Wmf/Workspace.h"
#ifdef SWAG_STATS
#include "Backend/ByteCode/Profiler/Profiler.h"
#endif

bool g_Exiting = false;

extern void help(const CommandLineParser& cmdParser, const Utf8& cmd);
extern void help();

int main(int argc, const char* argv[])
{
    OS::setup();
    initErrors();

    // Arguments
    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);

    // Log default help
    if (argc <= 1)
    {
        help();
        OS::exit(0);
    }

    // Special mode if the first argument is a script file with ".swgs" extension
    Utf8       command = argv[1];
    const Path p       = argv[1];
    if (p.extension() == ".swgs")
    {
        command                = "script";
        g_CommandLine.fileName = argv[1];
    }

    // Command
    if (command != "build" &&
        command != "run" &&
        command != "doc" &&
        command != "test" &&
        command != "new" &&
        command != "clean" &&
        command != "list" &&
        command != "get" &&
        command != "version" &&
        command != "format" &&
        command != "script")
    {
        Report::error(formErr(Fat0026, argv[1]));
        OS::exit(-1);
    }

    // Log help for a given command
    if (argc == 3 && std::string(argv[2]) == "--help")
    {
        help(cmdParser, command);
        OS::exit(0);
    }

    // Retrieve the compiler executable path
    g_CommandLine.exePath = std::filesystem::absolute(OS::getExePath());

    // Process all arguments
    if (!cmdParser.process(command, argc - 2, argv + 2))
        OS::exit(-1);
    if (!g_CommandLine.check())
        OS::exit(-1);

    // Output command line in verbose mode
    if (g_CommandLine.verboseCmdLine)
        g_Log.messageVerbose(cmdParser.buildString());

    g_Workspace = Allocator::alloc<Workspace>();

    // Deal with the main command
    if (command == "build")
    {
        g_Workspace->build();
    }
    else if (command == "run")
    {
        g_CommandLine.run = true;
        g_Workspace->build();
    }
    else if (command == "doc")
    {
        g_CommandLine.genDoc   = true;
        g_CommandLine.buildCfg = "fast-compile";
        g_CommandLine.rebuild  = true;
        g_Workspace->build();
    }
    else if (command == "test")
    {
        g_CommandLine.test = true;
        g_Workspace->build();
    }
    else if (command == "list")
    {
        g_CommandLine.listDepCmd = true;
        g_CommandLine.computeDep = true;
        g_CommandLine.fetchDep   = false;
        g_Workspace->build();
    }
    else if (command == "get")
    {
        g_CommandLine.getDepCmd  = true;
        g_CommandLine.computeDep = true;
        g_CommandLine.fetchDep   = true;
        g_Workspace->build();
    }
    else if (command == "script")
    {
        Workspace::scriptCommand();
    }
    else if (command == "clean")
    {
        g_Workspace->cleanCommand();
    }
    else if (command == "new")
    {
        g_Workspace->newCommand();
    }
    else if (command == "format")
    {
        Workspace::formatCommand();
    }
    else if (command == "version")
    {
        g_Log.messageInfo(form("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    }

    // Prints stats, then exit
#ifdef SWAG_STATS
    g_Stats.print();
    g_Stats.printFreq();
    profiler();
#endif

    // To avoid freeing some stuff, and have a fast exit
    g_Exiting = true;
    OS::exit(g_Workspace->numErrors > 0 ? -1 : 0);

    return 0;
}
