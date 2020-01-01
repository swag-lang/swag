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
    g_Log.messageHeaderDot("open files", format("%d", g_Stats.maxOpenFiles.load()));
    g_Log.messageHeaderDot("lines", format("%d", g_Stats.numLines.load()));
    g_Log.messageHeaderDot("lines/s", format("%d", (int) (g_Stats.numLines.load() / g_Stats.totalTime.count())));
    g_Log.messageHeaderDot("instructions", format("%d", g_Stats.numInstructions.load()));
    g_Log.messageHeaderDot("allocator memory", format("%dMb", g_Stats.allocatorMemory.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("wasted memory", format("%dMb", g_Stats.wastedAllocatorMemory.load() / (1024 * 1024)));
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
    g_Log.message("command\n");
    g_Log.message("-------\n");
    g_Log.message("version      print swag version\n");
    g_Log.message("env          creates an environment variable 'SWAG_FOLDER' with the folder location of the compiler\n");
    g_Log.message("build        build the specified workspace\n");
    g_Log.message("test         build and test the specified workspace\n");
    g_Log.message("doc          generate documentation for the specified workspace\n");
    g_Log.message("new          creates a new workspace\n");
    g_Log.message("watch        spy workspace and check it at each file change (never ends)\n");

    g_Log.message("\n");
    cmdParser.logArguments();

    g_Log.message("\n");
    g_Log.message("examples\n");
    g_Log.message("--------\n");
    g_Log.message("swag build --workspace:c:/myWorkspace --rebuild\n");
    g_Log.message("swag test -w:c:/myWorkspace --output:false\n");
    g_Log.message("swag doc -w:c:/myWorkspace --clean\n");
    g_Log.message("swag new -w:newWorkspace\n");

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
    g_CommandLine.exePath = fs::absolute(argv[0]).string();
    string command        = argv[1];
    if (command == "build" || command == "new")
    {
    }
    else if (command == "version")
    {
        g_Log.message(format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
        exit(0);
    }
    else if (command == "env")
    {
        OS::setSwagFolder(g_CommandLine.exePath.parent_path().string());
        exit(0);
    }
    else if (command == "test")
    {
        g_CommandLine.test = true;
    }
    else if (command == "watch")
    {
        g_CommandLine.watch = true;
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

    if (!cmdParser.process(argc - 2, argv + 2))
    {
        exit(-1);
    }

    // Output command line
    if (g_CommandLine.verbose)
    {
        auto str = cmdParser.buildString(true);
        g_Log.verbose("=> command line is " + str);
    }

    // Creates a new workspace, and exit
    if (command == "new")
    {
        g_Workspace.createNew();
        exit(0);
    }

    // User arguments
    pair<void*, void*> oneArg;
    oneArg.first  = (void*) g_CommandLine.exePath.string().c_str();
    oneArg.second = (void*) g_CommandLine.exePath.string().size();
    g_CommandLine.userArgumentsStr.push_back(oneArg);

    tokenizeBlanks(g_CommandLine.userArguments.c_str(), g_CommandLine.userArgumentsVec);
    for (auto& arg : g_CommandLine.userArgumentsVec)
    {
        oneArg.first  = (void*) arg.c_str();
        oneArg.second = (void*) (size_t) arg.length();
        g_CommandLine.userArgumentsStr.push_back(oneArg);
    }

    g_CommandLine.userArgumentsSlice.first  = &g_CommandLine.userArgumentsStr[0];
    g_CommandLine.userArgumentsSlice.second = (void*) g_CommandLine.userArgumentsStr.size();

    // Setup
    g_Global.setup();

    // Let's go...
    if (g_CommandLine.watch)
        g_Workspace.watch();
    else
        g_Workspace.build();

    // Prints stats, then exit
    auto timeAfter    = chrono::high_resolution_clock::now();
    g_Stats.totalTime = timeAfter - timeBefore;
    printStats();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
