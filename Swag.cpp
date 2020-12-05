#include "pch.h"
#include "Stats.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"
#include "Timer.h"
#include "Module.h"

void printStats()
{
    if (!g_CommandLine.stats || g_CommandLine.silent)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.print("\n");

    g_Log.messageHeaderDot("swag version", format("%d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    g_Log.messageHeaderDot("workers", format("%d", g_Stats.numWorkers));
    g_Log.print("\n");

    g_Log.messageHeaderDot("modules", format("%d", g_Stats.numModules.load()));
    g_Log.messageHeaderDot("files", format("%d", g_Stats.numFiles.load()));
    g_Log.messageHeaderDot("source lines", format("%d", g_Stats.numLines.load()));
    g_Log.messageHeaderDot("lines/s", format("%d", (int) (g_Stats.numLines.load() / g_Stats.totalTime.load())));
    g_Log.messageHeaderDot("open files", format("%d", g_Stats.maxOpenFiles.load()));
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", format("%d", g_Stats.numGenModules.load()));
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", format("%d", g_Stats.testFunctions.load()));
    g_Log.messageHeaderDot("executed #run", format("%d", g_Stats.runFunctions.load()));
    if (g_Workspace.numErrors)
        g_Log.messageHeaderDot("errors", format("%d", g_Workspace.numErrors.load()), LogColor::Red);
    g_Log.print("\n");

    g_Log.messageHeaderDot("instructions", format("%d", g_Stats.numInstructions.load()));
    float pc = (g_Stats.totalOptimsBC.load() * 100.0f) / (g_Stats.numInstructions.load());
    g_Log.messageHeaderDot("kicked", format("%d %.1f%%", g_Stats.totalOptimsBC.load(), pc));
    g_Log.messageHeaderDot("concrete types", format("%d", g_Stats.totalConcreteTypes.load()));
    g_Log.print("\n");

    g_Log.messageHeaderDot("syntax time", format("%.3fs", g_Stats.syntaxTime.load()));
    g_Log.messageHeaderDot("read files", format("%.3fs", g_Stats.readFilesTime.load()));
    g_Log.messageHeaderDot("semantic comp time", format("%.3fs", g_Stats.semanticCompilerTime.load()));
    g_Log.messageHeaderDot("semantic mod time", format("%.3fs", g_Stats.semanticModuleTime.load()));
    g_Log.messageHeaderDot("run time", format("%.3fs", g_Stats.runTime.load()));
    g_Log.messageHeaderDot("output time", format("%.3fs", g_Stats.outputTime.load()));
    g_Log.messageHeaderDot("prep out time", format("%.3fs %.3fs", g_Stats.prepOutputTimePass.load(), g_Stats.prepOutputTimeJob.load()));
    g_Log.messageHeaderDot("gen out time", format("%.3fs %.3fs", g_Stats.genOutputTimePass.load(), g_Stats.genOutputTimeJob.load()));
    g_Log.messageHeaderDot("run test time", format("%.3fs", g_Stats.runTestTime.load()));
    g_Log.messageHeaderDot("optim bc time", format("%.3fs", g_Stats.optimBCTime.load()));
    g_Log.print("\n");

    g_Log.messageHeaderDot("allocator memory", format("%dMb", g_Stats.allocatorMemory.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem wasted", format("%dMb", g_Stats.wastedMemory.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem nodes", format("%dMb", g_Stats.memNodes.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem scopes", format("%dMb", g_Stats.memScopes.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem seg", format("%dMb", g_Stats.memSeg.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem concat", format("%dMb", g_Stats.memConcat.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem types", format("%dMb", g_Stats.memTypes.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem instr", format("%dMb", g_Stats.memInstructions.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem symname", format("%dMb", g_Stats.memSymName.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem symover", format("%dMb", g_Stats.memSymOver.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem symtable", format("%dMb", g_Stats.memSymTable.load() / (1024 * 1024)));
    g_Log.messageHeaderDot("mem utf8", format("%dMb", g_Stats.memUtf8.load() / (1024 * 1024)));

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
    g_Log.message("run          build and run the specified workspace\n");
    g_Log.message("test         build and test the specified workspace\n");
    g_Log.message("new          creates a new workspace\n");
    g_Log.message("watch        spy workspace and check it at each file change (never ends)\n");

    g_Log.message("\n");
    cmdParser.logArguments();

    g_Log.message("\n");
    g_Log.message("examples\n");
    g_Log.message("--------\n");
    g_Log.message("swag build --workspace:c:/myWorkspace --rebuild\n");
    g_Log.message("swag run -w:c:/myWorkspace -m:myModuleToRun\n");
    g_Log.message("swag test -w:c:/myWorkspace --output:false\n");
    g_Log.message("swag new -w:newWorkspace\n");

    exit(0);
}

int main(int argc, const char* argv[])
{
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

    // Verify that the swag folder has been registered
    string swagFolder;
    if (!OS::getSwagFolder(swagFolder))
    {
        if (command != "env")
        {
            g_Log.message("cannot find 'SWAG_FOLDER' in the environment. You must run 'swag env' first at the swag.exe location to register its path.\n");
            exit(-1);
        }
    }
    else
    {
        g_CommandLine.exePath = swagFolder;
        g_CommandLine.exePath += "\\";
        fs::path pathF = fs::absolute(argv[0]).string();
        g_CommandLine.exePath += pathF.filename();
    }

    // Deal with the main command
    if (command == "run")
    {
        g_CommandLine.run = true;
    }
    else if (command == "build" || command == "new")
    {
    }
    else if (command == "version")
    {
        g_Log.message(format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
        exit(0);
    }
    else if (command == "env")
    {
        g_CommandLine.exePath = fs::absolute(argv[0]).string();
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
    else
    {
        g_Log.error(format("fatal error: invalid swag command '%s'", argv[1]));
        exit(-1);
    }

    // Process all arguments
    if (!cmdParser.process(argc - 2, argv + 2))
        exit(-1);

    // [devmode] stuff
    if (g_CommandLine.devMode)
    {
        g_Log.setColor(LogColor::DarkBlue);
        g_Log.print("[devmode] is activated\n");
        g_Log.setDefaultColor();
    }

    if (g_CommandLine.randomize)
    {
        if (!g_CommandLine.randSeed)
        {
            using namespace std::chrono;
            milliseconds ms        = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            g_CommandLine.randSeed = (int) ms.count() & 0x7FFFFFFF;
            srand(g_CommandLine.randSeed);
            g_CommandLine.randSeed = rand() & 0x7FFFFFFF;
        }

        srand(g_CommandLine.randSeed);
        g_Log.setColor(LogColor::DarkBlue);
        g_Log.print(format("[devmode] randomize seed is %d\n", g_CommandLine.randSeed));
        g_Log.setDefaultColor();
    }

    // Output command line in verbose mode
    if (g_CommandLine.verbose && g_CommandLine.verboseCmdLine)
    {
        g_Log.verbose(cmdParser.buildString(true));
    }

    // Creates a new workspace, and exit
    if (command == "new")
    {
        g_Workspace.createNew();
        exit(0);
    }

    // User arguments that can be retrieved with '@args'
    pair<void*, void*> oneArg;
    g_CommandLine.exePathStr = g_CommandLine.exePath.string();
    oneArg.first             = (void*) g_CommandLine.exePathStr.c_str();
    oneArg.second            = (void*) g_CommandLine.exePathStr.size();
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
    printStats();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
