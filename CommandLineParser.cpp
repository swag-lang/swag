#include "pch.h"
#include "CommandLine.h"
#include "CommandLineParser.h"
#include "Log.h"

void CommandLineParser::setup(CommandLine* cmdLine)
{
    addArg("--stats", nullptr, CommandLineType::Bool, &cmdLine->stats, nullptr, "display build statistics at the end");
    addArg("--silent", "-s", CommandLineType::Bool, &cmdLine->silent, nullptr, "do not log messages");
    addArg("--verbose", "-v", CommandLineType::Bool, &cmdLine->verbose, nullptr, "verbose mode, log all kind of informations");
    addArg("--error-out-source", nullptr, CommandLineType::Bool, &cmdLine->errorSourceOut, nullptr, "display source code when an error is raised");
    addArg("--error-out-note", nullptr, CommandLineType::Bool, &cmdLine->errorNoteOut, nullptr, "display notes when an error is raised");

    addArg("--workspace", "-w", CommandLineType::String, &cmdLine->workspacePath, nullptr, "the workspace to compile/document/create");
    addArg("--cache", "-t", CommandLineType::String, &cmdLine->cachePath, nullptr, "specify the cache folder (same as the output folder if empty)");

    addArg("--output", "-o", CommandLineType::Bool, &cmdLine->backendOutput, nullptr, "output backend");
    addArg("--output-legit", "-ol", CommandLineType::Bool, &cmdLine->backendOutputLegit, nullptr, "output native backend");
    addArg("--output-test", "-ot", CommandLineType::Bool, &cmdLine->backendOutputTest, nullptr, "output test backend");

    addArg("--test", nullptr, CommandLineType::Bool, &cmdLine->test, nullptr, "test mode (compile the ./tests folder and run all #test)");
    addArg("--run-test-bytecode", "-rb", CommandLineType::Bool, &cmdLine->runByteCodeTests, nullptr, "run #test functions as bytecode");
    addArg("--run-test-backend", "-ro", CommandLineType::Bool, &cmdLine->runBackendTests, nullptr, "run #test functions as native");

    addArg("--clean", nullptr, CommandLineType::Bool, &cmdLine->clean, nullptr, "clean the cache and target folder");
    addArg("--rebuild", nullptr, CommandLineType::Bool, &cmdLine->rebuild, nullptr, "full rebuild");
    addArg("--doc", nullptr, CommandLineType::Bool, &cmdLine->generateDoc, nullptr, "generate documentation");

    addArg("--file-filter", nullptr, CommandLineType::String, &cmdLine->fileFilter, nullptr, nullptr);
    addArg("--test-filter", nullptr, CommandLineType::String, &cmdLine->testFilter, nullptr, nullptr);
    addArg("--tab-size", nullptr, CommandLineType::Int, &cmdLine->tabSize, nullptr, "input size (in blanks) of a source file tabulation");
    addArg("--num-cores", nullptr, CommandLineType::Int, &cmdLine->numCores, nullptr, "max number of cpu to use (0 = automatic)");
    addArg("--user-args", nullptr, CommandLineType::String, &cmdLine->userArguments, nullptr, "pass some specific arguments to the user code");

    addArg("--config", nullptr, CommandLineType::String, &cmdLine->config, nullptr, "set the build config");
    addArg("--arch", nullptr, CommandLineType::Enum, &cmdLine->arch, "win64", "set the build architecture");

    addArg("--debug", nullptr, CommandLineType::Bool, &cmdLine->debug, nullptr, "force to compile in debug mode");
}

void CommandLineParser::logArguments()
{
    string line0, line1;

    static const int COL_SHORT_NAME = 20;
    static const int COL_VALUE      = 35;
    static const int COL_DEFAULT    = 50;
    static const int COL_HELP       = 60;

    line0 = "argument";
    line1 = "--------";
    while (line0.length() < COL_SHORT_NAME)
        line0 += " ", line1 += " ";
    line0 += "short";
    line1 += "-----";
    while (line0.length() < COL_VALUE)
        line0 += " ", line1 += " ";
    line0 += "value";
    line1 += "-----";
    while (line0.length() < COL_DEFAULT)
        line0 += " ", line1 += " ";
    line0 += "default";
    line1 += "-------";
    while (line0.length() < COL_HELP)
        line0 += " ", line1 += " ";
    line0 += "help";
    line1 += "----";

    line0 += "\n";
    line1 += "\n";
    g_Log.message(line0);
    g_Log.message(line1);

    int cpt = 0;
    for (auto arg : longNameArgs)
    {
        auto oneArg = arg.second;

        line0 = arg.first;
        while (line0.length() < COL_SHORT_NAME)
            line0 += " ";
        line0 += oneArg->shortName;
        while (line0.length() < COL_VALUE)
            line0 += " ";

        switch (oneArg->type)
        {
        case CommandLineType::Bool:
            line0 += "true|false";
            while (line0.length() < COL_DEFAULT)
                line0 += " ";
            if (*(bool*) oneArg->buffer)
                line0 += "true";
            else
                line0 += "false";
            break;
        case CommandLineType::Int:
            line0 += "<integer>";
            while (line0.length() < COL_DEFAULT)
                line0 += " ";
            line0 += to_string(*(int*) oneArg->buffer);
            break;
        case CommandLineType::String:
            line0 += "<string>";
            while (line0.length() < COL_DEFAULT)
                line0 += " ";
            line0 += *(string*) oneArg->buffer;
            break;
        case CommandLineType::Enum:
            line0 += oneArg->param;
            while (line0.length() < COL_DEFAULT)
                line0 += " ";
            line0 += *(string*) oneArg->buffer;
            break;
        }

        if (oneArg->help)
        {
            while (line0.length() < COL_HELP)
                line0 += " ";
            line0 += oneArg->help;
        }

        g_Log.message(line0);
        cpt++;
    }
}

void CommandLineParser::addArg(const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help)
{
    auto arg = new CommandLineArgument{type, address, param, help};

    if (longName)
        arg->longName = longName;
    if (shortName)
        arg->shortName = shortName;

    if (longName)
        longNameArgs[longName] = arg;
    if (shortName)
        shortNameArgs[shortName] = arg;
}

bool CommandLineParser::process(int argc, const char* argv[])
{
    bool result = true;
    for (int i = 0; i < argc; i++)
    {
        // Split in half, with the ':' delimiter
        string      command;
        string      argument;
        const char* pz = argv[i];
        while (*pz && *pz != ':')
            command += *pz++;
        if (*pz)
            pz++;
        while (*pz)
            argument += *pz++;

        // Find command
        auto it = longNameArgs.find(command);
        if (it == longNameArgs.end())
        {
            it = shortNameArgs.find(command);
            if (it == shortNameArgs.end())
            {
                g_Log.error(format("command line error: invalid argument '%s'", command.c_str()));
                result = false;
                continue;
            }
        }

        auto arg = it->second;
        switch (arg->type)
        {
        case CommandLineType::Enum:
        {
            vector<string> tokens;
            tokenize(arg->param, '|', tokens);

            int index = 0;
            for (auto one : tokens)
            {
                if (one == argument)
                {
                    *(int*) arg->buffer = index;
                    break;
                }

                index++;
            }

            if (index == tokens.size())
            {
                g_Log.error(format("command line error: argument '%s' must be followed by '%s'", it->first.c_str(), arg->param));
                result = false;
                continue;
            }
        }
        break;

        case CommandLineType::Bool:
            if (argument == "true" || argument.empty())
                *(bool*) arg->buffer = true;
            else if (argument == "false")
                *(bool*) arg->buffer = false;
            else
            {
                g_Log.error(format("command line error: argument '%s' must be followed by 'true' or 'false' ('%s')", it->first.c_str(), argument.c_str()));
                result = false;
                continue;
            }
            break;

        case CommandLineType::String:
        {
            if (argument.empty())
            {
                g_Log.error(format("command line error: argument '%s' must be followed by a string", it->first.c_str(), argument.c_str()));
                result = false;
                continue;
            }

            *((string*) arg->buffer) = argument;
            break;
        }

        case CommandLineType::Int:
        {
            pz               = argument.c_str();
            bool thisIsAnInt = argument.empty() ? false : true;

            while (*pz)
            {
                if (!isdigit(*pz))
                {
                    thisIsAnInt = false;
                    break;
                }

                pz++;
            }

            if (!thisIsAnInt)
            {
                if (argument.empty())
                    g_Log.error(format("command line error: argument '%s' must be followed by an integer value", it->first.c_str()));
                else
                    g_Log.error(format("command line error: argument '%s' must be followed by an integer value ('%s')", it->first.c_str(), argument.c_str()));
                result = false;
                continue;
            }

            *(int*) arg->buffer = atoi(argument.c_str());
            break;
        }
        }
    }

    return result;
}

string CommandLineParser::buildString()
{
    CommandLine       defaultValues;
    CommandLineParser defaultParser;
    defaultParser.setup(&defaultValues);

    string result;
    for (auto arg : longNameArgs)
    {
        auto itDefault  = defaultParser.longNameArgs.find(arg.first);
        auto defaultArg = itDefault->second;
        auto oneArg     = arg.second;

        switch (oneArg->type)
        {
        case CommandLineType::String:
        case CommandLineType::Enum:
            if (*(string*) oneArg->buffer != *(string*) defaultArg->buffer)
            {
                result += oneArg->longName + ":";
                result += *(string*) oneArg->buffer;
                result += " ";
            }
            break;
        case CommandLineType::Int:
            if (*(int*) oneArg->buffer != *(int*) defaultArg->buffer)
            {
                result += oneArg->longName + ":";
                result += to_string(*(int*) oneArg->buffer);
                result += " ";
            }
            break;
        case CommandLineType::Bool:
            if (*(bool*) oneArg->buffer != *(bool*) defaultArg->buffer)
            {
                result += oneArg->longName + ":";
                if (*(bool*) oneArg->buffer == true)
                    result += "true";
                else
                    result += "false";
                result += " ";
            }
            break;
        }
    }

    return result;
}
