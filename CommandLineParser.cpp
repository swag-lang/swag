#include "pch.h"
#include "CommandLine.h"
#include "CommandLineParser.h"
#include "Log.h"

void CommandLineParser::setup(CommandLine* cmdLine)
{
    addArg("--silent", "-s", CommandLineType::Bool, &cmdLine->silent);
    addArg("--verbose", "-v", CommandLineType::Bool, &cmdLine->verbose);
    addArg("--stats", nullptr, CommandLineType::Bool, &cmdLine->stats);
    addArg("--output", nullptr, CommandLineType::Bool, &cmdLine->output);
    addArg("--error-out-source", nullptr, CommandLineType::Bool, &cmdLine->errorSourceOut);
    addArg("--error-out-note", nullptr, CommandLineType::Bool, &cmdLine->errorNoteOut);
    addArg("--unittest", nullptr, CommandLineType::Bool, &cmdLine->unittest);
    addArg("--test", nullptr, CommandLineType::Bool, &cmdLine->test);
    addArg("--run-test-bytecode", nullptr, CommandLineType::Bool, &cmdLine->runByteCodeTests);
    addArg("--run-test-backend", nullptr, CommandLineType::Bool, &cmdLine->runBackendTests);
    addArg("--clean-cache", nullptr, CommandLineType::Bool, &cmdLine->cleanCache);
    addArg("--version", "-d", CommandLineType::StringList, &cmdLine->compileVersion);

    addArg("--file-filter", nullptr, CommandLineType::String, &cmdLine->fileFilter);
    addArg("--tab-size", nullptr, CommandLineType::Int, &cmdLine->tabSize);
    addArg("--num-cores", nullptr, CommandLineType::Int, &cmdLine->numCores);
    addArg("--pass", nullptr, CommandLineType::Enum, &cmdLine->buildPass, "lexer|syntax|semantic|backend|full");

    //cmdLine->runBackendTests = false;
    cmdLine->fileFilter       = "329";
    cmdLine->addRuntimeModule = false;
}

void CommandLineParser::addArg(const char* longName, const char* shortName, CommandLineType type, void* address, const char* param)
{
    if (longName)
        longNameArgs[longName] = new CommandLineArgument{type, address, param};
    if (shortName)
        shortNameArgs[shortName] = new CommandLineArgument{type, address, param};
}

bool CommandLineParser::process(int argc, const char* argv[])
{
    bool result = true;
    for (int i = 1; i < argc; i++)
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

        case CommandLineType::StringList:
        {
            if (argument.empty())
            {
                g_Log.error(format("command line error: argument '%s' must be followed by a string", it->first.c_str(), argument.c_str()));
                result = false;
                continue;
            }

            ((set<string>*) arg->buffer)->insert(argument);
            break;
        }

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