#include "pch.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"

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
    g_Stats.print();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
