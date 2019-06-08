#include "pch.h"
#include "CommandLine.h"
#include "CommandLineParser.h"
#include "Log.h"

void CommandLineParser::setup(CommandLine* cmdLine)
{
    addArg("--verbose", "-v", CommandLineType::Bool, &cmdLine->verbose);
    addArg("--stats", nullptr, CommandLineType::Bool, &cmdLine->stats);
    addArg("--syntax-only", nullptr, CommandLineType::Bool, &cmdLine->syntaxOnly);
    addArg("--error-out-source", "-eos", CommandLineType::Bool, &cmdLine->errorSourceOut);
    addArg("--error-out-note", "-eon", CommandLineType::Bool, &cmdLine->errorNoteOut);

    addArg("--tab-size", nullptr, CommandLineType::Int, &cmdLine->tabSize);
    addArg("--num-cores", nullptr, CommandLineType::Int, &cmdLine->numCores);

    cmdLine->stats = true;
    //cmdLine->fileFilter = "216";
}

void CommandLineParser::addArg(const char* longName, const char* shortName, CommandLineType type, void* address)
{
    if (longName)
        longNameArgs[longName] = new CommandLineArgument{type, address};
    if (shortName)
        shortNameArgs[shortName] = new CommandLineArgument{type, address};
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
                continue;
        }

        auto arg = it->second;
        switch (arg->type)
        {
        case CommandLineType::Bool:
            if (argument == "true" || argument.empty())
                *(bool*) arg->buffer = true;
            else if (argument == "false")
                *(bool*) arg->buffer = false;
            else
            {
                g_Log.setColor(LogColor::Red);
                g_Log.print("command line error: ");
                g_Log.setDefaultColor();
                g_Log.print(format("argument '%s' must be followed by 'true' or 'false' ('%s')", it->first.c_str(), argument.c_str()));
                g_Log.eol();
                result = false;
                continue;
            }
            break;

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
                g_Log.setColor(LogColor::Red);
                g_Log.print("command line error: ");
                g_Log.setDefaultColor();
                if (argument.empty())
                    g_Log.print(format("argument '%s' must be followed by an integer value", it->first.c_str()));
                else
                    g_Log.print(format("argument '%s' must be followed by an integer value ('%s')", it->first.c_str(), argument.c_str()));
                g_Log.eol();
                continue;
            }

            *(int*) arg->buffer = atoi(argument.c_str());
            break;
        }
        }
    }

    return result;
}