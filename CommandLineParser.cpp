#include "pch.h"
#include "CommandLine.h"
#include "CommandLineParser.h"
#include "Log.h"

void CommandLineParser::setup(CommandLine* cmdLine)
{
    addArg("--verbose", "-v", CommandLineType::Bool, &cmdLine->verbose);
    addArg("--stats", nullptr, CommandLineType::Bool, &cmdLine->stats);
    addArg("--syntax-only", nullptr, CommandLineType::Bool, &cmdLine->syntaxOnly);

    addArg("--tab-size", nullptr, CommandLineType::Int, &cmdLine->tabSize);
    addArg("--num-cores", nullptr, CommandLineType::Int, &cmdLine->numCores);

	cmdLine->stats = true;
	//cmdLine->fileFilter = "147";
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
        auto it = longNameArgs.find(argv[i]);
        if (it == longNameArgs.end())
        {
            it = shortNameArgs.find(argv[i]);
            if (it == shortNameArgs.end())
                continue;
        }

        auto arg = it->second;
        switch (arg->type)
        {
        case CommandLineType::Bool:
            *(bool*) arg->buffer = true;
            break;

        case CommandLineType::Int:
        {
            if (i == argc - 1)
            {
                g_Log.setColor(LogColor::Red);
                g_Log.print("command line error: ");
                g_Log.setDefaultColor();
                g_Log.print(format("argument '%s' must be followed by an integer value", it->first.c_str()));
                result = false;
                continue;
            }

            const char* pz          = argv[i + 1];
            bool        thisIsAnInt = true;
            while (*pz)
            {
                if (!isdigit(*pz))
                {
                    g_Log.setColor(LogColor::Red);
                    g_Log.print("command line error: ");
                    g_Log.setDefaultColor();
                    g_Log.print(format("argument '%s' must be followed by an integer value ('%s')", it->first.c_str(), argv[i + 1]));
                    thisIsAnInt = false;
                    break;
                }

                pz++;
            }

            if (!thisIsAnInt)
                continue;

            *(int*) arg->buffer = atoi(argv[i + 1]);
            i++;
            break;
        }

        case CommandLineType::String:
            if (i == argc - 1)
            {
                g_Log.setColor(LogColor::Red);
                g_Log.print("command line error: ");
                g_Log.setDefaultColor();
                g_Log.print(format("argument '%s' must be followed by a string", it->first.c_str()));
                result = false;
                continue;
            }

            *(string*) arg->buffer = argv[i + 1];
            i++;
            break;
        }
    }

    return result;
}