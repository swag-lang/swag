#include "pch.h"
#include "CommandLineParser.h"
#include "Version.h"
#include "Os.h"

static void printExamples()
{
    g_Log.messageInfo("\n");
    g_Log.messageInfo("Examples\n");
    g_Log.messageInfo("--------\n");
}

static void printVersion()
{
    g_Log.messageInfo(Fmt("swag compiler version %d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
}

void help(CommandLineParser& cmdParser, const Utf8& cmd)
{
    ////////////////////////////////////////////////////////
    if (cmd == "version")
    {
        printVersion();
        return;
    }

    ////////////////////////////////////////////////////////
    if (cmd == "build")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'build' compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.messageInfo("It will compile all the modules located in '/modules' and '/examples', and their dependencies.\n");
        g_Log.messageInfo("If you want to compile only one module, add '--module:moduleName'.\n");
        g_Log.messageInfo("Build results are located in the workspace '/output' folder.\n");

        printExamples();
        g_Log.messageInfo("swag build --workspace:c:/myWorkspace\n");
        g_Log.messageInfo("swag build -w:c:/myWorkspace --rebuild\n");
        g_Log.messageInfo("swag build -w:c:/myWorkspace -m:myModule\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "run")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'run' behaves like 'build' and compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.messageInfo("It will then run all compiled executables.\n");
        g_Log.messageInfo("If you want to compile and run only one module, add '--module:moduleName'.\n");

        printExamples();
        g_Log.messageInfo("swag run -w:c:/myWorkspace -m:myModuleToRun\n");
        g_Log.messageInfo("swag myScript.swgs\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "test")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'test' behaves like 'build' and compiles the specified workspace, or the current folder if it's a valid workspace.\n");
        g_Log.messageInfo("But unlike 'build' and 'run', it will also compile and run the modules located in the '/tests' folder.\n");

        printExamples();
        g_Log.messageInfo("swag test -w:c:/myWorkspace\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "doc")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'doc' generates the documentation of each module.\n");

        printExamples();
        g_Log.messageInfo("swag doc --workspace:c:/myWorkspace\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "script")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'script' compiles and run the given script file.\n");
        g_Log.messageInfo("All dependencies will be compiled in a cache folder.\n");
        g_Log.messageInfo("Type 'swag clean --script' to clean all caches used by script execution.\n");

        printExamples();
        g_Log.messageInfo("swag script -f:myScript.swgs\n");
        g_Log.messageInfo("swag script -file:c:/myScript.swgs\n");
        g_Log.messageInfo("swag myScript.swgs\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "clean")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'clean' erases the cache of temporary files for the given workspace and configuration.\n");

        printExamples();
        g_Log.messageInfo("swag clean --workspace:c:/myWorkspace\n");
        g_Log.messageInfo("swag clean --workspace:c:/myWorkspace --cfg:fast-debug\n");
        g_Log.messageInfo("swag clean --script\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "new")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'new' creates a new workspace, a new module in an existing workspace or a new script file.\n");

        printExamples();
        g_Log.messageInfo("swag new -w:c:/newWorkspace\n");
        g_Log.messageInfo("swag new -w:c:/workspace -m:newModule\n");
        g_Log.messageInfo("swag new -f:newScriptFile\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "list")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'list' logs all the workspace modules and their dependencies.\n");

        printExamples();
        g_Log.messageInfo("swag list --workspace:c:/myWorkspace\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }

    ////////////////////////////////////////////////////////
    if (cmd == "get")
    {
        g_Log.messageInfo("\n");
        g_Log.messageInfo("Command 'get' synchronize all the workspace dependencies.\n");

        printExamples();
        g_Log.messageInfo("swag get --workspace:c:/myWorkspace\n");
        g_Log.messageInfo("\n");
        cmdParser.logArguments(cmd);
    }
}

void help(CommandLineParser& cmdParser)
{
    printVersion();

    g_Log.messageInfo("Usage:\n");
    g_Log.messageInfo("        swag <command> [arguments]\n");

    g_Log.messageInfo("\n");
    g_Log.messageInfo("Commands:\n");
    g_Log.messageInfo("        version      print swag version\n");
    g_Log.messageInfo("        build        build the specified workspace\n");
    g_Log.messageInfo("        run          build and run the specified workspace\n");
    g_Log.messageInfo("        test         build and test the specified workspace\n");
    g_Log.messageInfo("        clean        clean the specified workspace of cache files, binaries and dependencies (fresh start)\n");
    g_Log.messageInfo("        new          creates a new workspace/module/script file\n");
    g_Log.messageInfo("        get          synchronize dependencies\n");
    g_Log.messageInfo("        list         list all modules and their dependencies\n");
    g_Log.messageInfo("        script       build and run a script file\n");
    g_Log.messageInfo("        doc          generate documentation of library modules\n");

    g_Log.messageInfo("\n");
    g_Log.messageInfo("To get the list of all arguments for a given command, type 'swag <command> --help'\n");
    g_Log.messageInfo("For example: swag build --help\n");
}
