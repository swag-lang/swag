#include "pch.h"
#include "Stats.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"

void printStats()
{
    if (!g_CommandLine.stats || g_CommandLine.silent)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.messageHeaderDot("swag version", format("%d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    g_Log.messageHeaderDot("total time", format("%.3fs", g_Stats.totalTime.count()));
    g_Log.messageHeaderDot("workers", format("%d", g_Stats.numWorkers));
    g_Log.messageHeaderDot("modules", format("%d", g_Stats.numModules.load()));
    g_Log.messageHeaderDot("files", format("%d", g_Stats.numFiles.load()));
    g_Log.messageHeaderDot("lines", format("%d", g_Stats.numLines.load()));
    g_Log.messageHeaderDot("lines/s", format("%d", (int) (g_Stats.numLines.load() / g_Stats.totalTime.count())));
    g_Log.messageHeaderDot("instructions", format("%d", g_Stats.numInstructions.load()));
    if (g_CommandLine.backendOutput)
        g_Log.messageHeaderDot("output modules", format("%d", g_Stats.numGenModules.load()));
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("test functions", format("%d", g_Stats.testFunctions.load()));
    if (g_Workspace.numErrors)
        g_Log.messageHeaderDot("errors", format("%d", g_Workspace.numErrors.load()), LogColor::Red);
    g_Log.setDefaultColor();
}

void help(CommandLineParser& cmdParser)
{
    g_Log.message(format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));

    g_Log.message("usage: swag <command> [arguments]\n");

    g_Log.message("\n");
    g_Log.message("commands\n");
    g_Log.message("--------\n");
    g_Log.message("version      print swag version\n");
    g_Log.message("build        build the specified workspace\n");
    g_Log.message("test         build and test the specified workspace\n");
    g_Log.message("doc          generate documentation for the specified workspace\n");

    g_Log.message("\n");
    g_Log.message("Arguments\n");
    g_Log.message("---------\n");
    cmdParser.logArguments();
    exit(0);
}

int main(int argc, const char* argv[])
{
    auto timeBefore = chrono::high_resolution_clock::now();

    OS::setup();
    g_Log.setup();

    // Arguments
    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);

    // Log all arguments
    if (argc <= 1)
    {
        help(cmdParser);
    }

    // Command
    string command = argv[1];
    if (command == "build")
    {
    }
    else if (command == "version")
    {
        g_Log.message(format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
        exit(0);
    }
    else if (command == "test")
    {
        g_CommandLine.test = true;
    }
    else if (command == "doc")
    {
        g_CommandLine.backendOutput = false;
        g_CommandLine.generateDoc   = true;
    }
    else
    {
        g_Log.error(format("fatal error: invalid swag command '%s'", argv[1]));
        exit(-1);
    }

    g_CommandLine.exePath = fs::absolute(argv[0]).string();
    if (!cmdParser.process(argc - 2, argv + 2))
        return -2;

    // User arguments
    pair<void*, void*> oneArg;
    oneArg.first  = (void*) g_CommandLine.exePath.c_str();
    oneArg.second = (void*) g_CommandLine.exePath.size();
    g_CommandLine.userArgumentsStr.push_back(oneArg);

    tokenizeBlanks(g_CommandLine.userArguments.c_str(), g_CommandLine.userArgumentsVec);
    for (auto& arg : g_CommandLine.userArgumentsVec)
    {
        oneArg.first  = (void*) arg.c_str();
        oneArg.second = (void*) arg.size();
        g_CommandLine.userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine.userArgumentsSlice.first  = &g_CommandLine.userArgumentsStr[0];
    g_CommandLine.userArgumentsSlice.second = (void*) g_CommandLine.userArgumentsStr.size();

    // Setup
    g_Global.setup();

    // Let's go...
    g_Workspace.build();

    // Prints stats, then exit
    auto timeAfter    = chrono::high_resolution_clock::now();
    g_Stats.totalTime = timeAfter - timeBefore;
    printStats();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
