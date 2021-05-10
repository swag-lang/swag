#include "pch.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"
#include "ErrorIds.h"

void printExamples()
{
    g_Log.message("\n");
    g_Log.message("Examples\n");
    g_Log.message("--------\n");
}

void printVersion()
{
    g_Log.message(format("swag compiler version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
}

void help(CommandLineParser& cmdParser, const string& cmd)
{
    ////////////////////////////////////////////////////////
    if (cmd == "version")
    {
        printVersion();
        return;
    }

    ////////////////////////////////////////////////////////
    if (cmd == "env")
    {
        g_Log.message("\n");
        g_Log.message("Command 'env' must be used to set the environnment variable 'SWAG_FOLDER'.\n");
        g_Log.message("It is mandatory to set that variable before using the swag compiler.\n");

        string folder;
        OS::getSwagFolder(folder);

        if (folder.empty())
            g_Log.message("Current folder is NOT DEFINED");
        else
            g_Log.message(format("Current folder is '%s'", folder.c_str()));
    }

    ////////////////////////////////////////////////////////
    if (cmd == "build")
    {
        g_Log.message("\n");
        g_Log.message("Command 'build' compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.message("It will compile all the modules located in '/modules' and '/examples', and their dependencies.\n");
        g_Log.message("If you want to compile only one module, add '--module:moduleName'.\n");
        g_Log.message("Build results are located in the '/output' folder.\n");

        printExamples();
        g_Log.message("swag build --workspace:c:/myWorkspace\n");
        g_Log.message("swag build -w:c:/myWorkspace --rebuild\n");
        g_Log.message("swag build -w:c:/myWorkspace -m:myModule\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "run")
    {
        g_Log.message("\n");
        g_Log.message("Command 'run' behaves like 'build' and compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.message("It will then run all compiled executables.\n");
        g_Log.message("If you want to compile and run only one module, add '--module:moduleName'.\n");

        printExamples();
        g_Log.message("swag run -w:c:/myWorkspace -m:myModuleToRun\n");
        g_Log.message("swag myScript.swgs\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "test")
    {
        g_Log.message("\n");
        g_Log.message("Command 'test' behaves like 'build' and compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.message("But unlike 'build' and 'run', it will also compile and run the modules located in the '/tests' folder.\n");

        printExamples();
        g_Log.message("swag test -w:c:/myWorkspace\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "script")
    {
        g_Log.message("\n");
        g_Log.message("Command 'script' compiles and run the given script file.\n");
        g_Log.message("All dependencies will be compiled in the cache folder.\n");
        g_Log.message("Type 'swag clean --script' to clean the cache used by script execution.\n");

        printExamples();
        g_Log.message("swag script -f:myScript.swgs\n");
        g_Log.message("swag script -file:c:/myScript.swgs\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "clean")
    {
        g_Log.message("\n");
        g_Log.message("Command 'clean' erases the cache of temporary files for the given workspace and configuration.\n");

        printExamples();
        g_Log.message("swag clean --workspace:c:/myWorkspace\n");
        g_Log.message("swag clean --workspace:c:/myWorkspace --cfg:fast-debug\n");
        g_Log.message("swag clean --script\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "new")
    {
        g_Log.message("\n");
        g_Log.message("Command 'new' creates a new workspace, a new module in an existing workspace or a new script file.\n");

        printExamples();
        g_Log.message("swag new -w:c:/newWorkspace\n");
        g_Log.message("swag new -w:c:/workspace -m:newModule\n");
        g_Log.message("swag new -f:newScriptFile\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "list")
    {
        g_Log.message("\n");
        g_Log.message("Command 'list' logs all the workspace modules and their dependencies.\n");

        printExamples();
        g_Log.message("swag list --workspace:c:/myWorkspace\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "get")
    {
        g_Log.message("\n");
        g_Log.message("Command 'get' synchronize all the workspace dependencies.\n");

        printExamples();
        g_Log.message("swag get --workspace:c:/myWorkspace --rebuild\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "watch")
    {
        printExamples();
        g_Log.message("swag watch --workspace:c:/myWorkspace --rebuild\n");
        g_Log.message("\n");
        cmdParser.logArguments(cmd);
    }
}

void help(CommandLineParser& cmdParser)
{
    printVersion();

    g_Log.message("Usage:\n");
    g_Log.message("        swag <command> [arguments]\n");

    g_Log.message("\n");
    g_Log.message("Commands:\n");
    g_Log.message("        version      print swag version\n");
    g_Log.message("        env          creates an environment variable 'SWAG_FOLDER' with the folder location of the compiler\n");
    g_Log.message("        build        build the specified workspace\n");
    g_Log.message("        run          build and run the specified workspace\n");
    g_Log.message("        test         build and test the specified workspace\n");
    g_Log.message("        clean        clean the specified workspace of cache files, binaries and dependencies (fresh start)\n");
    g_Log.message("        new          creates a new workspace\n");
    g_Log.message("        watch        spy workspace and check it at each file change (never ends)\n");
    g_Log.message("        get          synchronize dependencies\n");
    g_Log.message("        list         list all modules and their dependencies\n");
    g_Log.message("        script       build and run a script file\n");

    g_Log.message("\n");
    g_Log.message("To get the list of all arguments for a given command, type 'swag <command> --help'\n");
    g_Log.message("For example: swag build --help\n");
}

int main(int argc, const char* argv[])
{
    OS::setup();
    g_Log.setup();

    // Arguments
    CommandLineParser cmdParser;
    cmdParser.setup(&g_CommandLine);

    // Log default help
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
        command != "get" &&
        command != "version" &&
        command != "script" &&
        command != "env")
    {
        g_Log.error(format(Msg0000, argv[1]));
        exit(-1);
    }

    // Log help for a given command
    if (argc == 3 && string(argv[2]) == "--help")
    {
        help(cmdParser, command);
        exit(0);
    }

    // Verify that the swag folder has been registered
    string swagFolder;
    if (!OS::getSwagFolder(swagFolder))
    {
        if (command != "env")
        {
            g_Log.message(Msg0165);
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
