#pragma once
#include "Utf8.h"
#include "Map.h"
#include "Set.h"
struct CommandLine;

enum CommandLineType
{
    Bool,
    Int,
    EnumInt,
    EnumString,
    String,
    StringSet,
    StringPath,
};

struct CommandLineArgument
{
    SetUtf8         cmds;
    Utf8            longName;
    Utf8            shortName;
    void*           buffer;
    const char*     param;
    const char*     help;
    CommandLineType type;

    CommandLineArgument(const char* commands, CommandLineType type, void* buffer, const char* param, const char* help)
        : type{type}
        , buffer{buffer}
        , param{param}
        , help{help}
    {
        Vector<Utf8> all;
        Utf8::tokenize(commands, ' ', all);

        for (auto& p : all)
        {
            if (p == "all" || p == "bu")
            {
                cmds.insert("build");
                cmds.insert("run");
            }

            if (p == "all" || p == "sc")
            {
                cmds.insert("script");
            }

            if (p == "all" || p == "bu" || p == "te")
                cmds.insert("test");

            if (p == "all" || p == "cl")
                cmds.insert("clean");

            if (p == "all" || p == "ne")
                cmds.insert("new");

            if (p == "all" || p == "li")
                cmds.insert("list");

            if (p == "all" || p == "ge")
                cmds.insert("get");

            if (p == "all" || p == "ru")
                cmds.insert("run");
        }
    }
};

struct CommandLineParser
{
    void setup(CommandLine* cmdLine);
    bool isArgValidFor(const Utf8& swagCmd, CommandLineArgument* arg);
    bool process(const Utf8& swagCmd, int argc, const char* argv[]);
    void addArg(const char* commands, const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help);
    void logArguments(const Utf8& cmd);
    Utf8 buildString(bool full);

    MapUtf8<CommandLineArgument*> longNameArgs;
    MapUtf8<CommandLineArgument*> shortNameArgs;
};
