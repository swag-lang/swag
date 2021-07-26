#include "pch.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"

static void printExamples()
{
    g_Log.message("\n");
    g_Log.message("Examples\n");
    g_Log.message("--------\n");
}

static void printVersion()
{
    g_Log.message(Utf8::format("swag compiler version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
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
            g_Log.message(Utf8::format("Current folder is '%s'", folder.c_str()));
    }

    ////////////////////////////////////////////////////////
    if (cmd == "build")
    {
        g_Log.message("\n");
        g_Log.message("Command 'build' compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.message("It will compile all the modules located in '/modules' and '/examples', and their dependencies.\n");
        g_Log.message("If you want to compile only one module, add '--module:moduleName'.\n");
        g_Log.message("Build results are located in the workspace '/output' folder.\n");

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
        g_Log.message("swag get --workspace:c:/myWorkspace\n");
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
    g_Log.message("        new          creates a new workspace/module/script file\n");
    g_Log.message("        get          synchronize dependencies\n");
    g_Log.message("        list         list all modules and their dependencies\n");
    g_Log.message("        script       build and run a script file\n");

    g_Log.message("\n");
    g_Log.message("To get the list of all arguments for a given command, type 'swag <command> --help'\n");
    g_Log.message("For example: swag build --help\n");
}
