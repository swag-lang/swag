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
    g_Log.message("clean        clean the specified workspace of cache files, binaries and dependencies (fresh start)\n");
    g_Log.message("new          creates a new workspace\n");
    g_Log.message("watch        spy workspace and check it at each file change (never ends)\n");
    g_Log.message("list         list all modules and their dependencies\n");

    g_Log.message("\n");
    cmdParser.logArguments();

    g_Log.message("\n");
    g_Log.message("examples\n");
    g_Log.message("--------\n");
    g_Log.message("swag build --workspace:c:/myWorkspace --rebuild\n");
    g_Log.message("swag run -w:c:/myWorkspace -m:myModuleToRun\n");
    g_Log.message("swag test -w:c:/myWorkspace --output:false\n");
    g_Log.message("swag new -w:newWorkspace\n");
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
        exit(0);
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
        command != "version" &&
        command != "env")
    {
        g_Log.error(format("fatal error: invalid swag command '%s'", argv[1]));
        exit(-1);
    }

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

    // Process all arguments
    if (!cmdParser.process(command, argc - 2, argv + 2))
        exit(-1);
    if (!g_CommandLine.check())
        exit(-1);

    // Output command line in verbose mode
    if (g_CommandLine.verboseCmdLine)
        g_Log.verbose(cmdParser.buildString(true));

    // Deal with the main command
    if (command == "build")
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
        g_CommandLine.listDep   = true;
        g_CommandLine.updateDep = true;
        g_CommandLine.fetchDep  = false;
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
        g_Log.message(format("swag version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
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
    g_Global.exiting = true;

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
