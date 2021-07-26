#pragma once
#include "Utf8.h"
#include "Global.h"
struct CommandLine;

enum CommandLineType
{
    Bool,
    Int,
    EnumInt,
    EnumString,
    String,
    StringSet,
};

struct CommandLineArgument
{
    set<Utf8>       cmds;
    string          longName;
    string          shortName;
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
        vector<Utf8> all;
        Utf8::tokenize(commands, ' ', all);

        for (auto& p : all)
        {
            if (p == "all" || p == "bu")
            {
                cmds.insert("build");
                cmds.insert("run");
                cmds.insert("script");
            }

            if (p == "all" || p == "sc")
            {
                cmds.insert("script");
            }

            if (p == "all" || p == "bu" || p == "te")
                cmds.insert("test");

            if (p == "all" || p == "cl")
                cmds.insert("clean");

            if (p == "all" || p == "wa")
                cmds.insert("watch");

            if (p == "all" || p == "ne")
                cmds.insert("new");

            if (p == "all" || p == "li")
                cmds.insert("list");

            if (p == "all" || p == "ge")
                cmds.insert("get");
        }
    }
};

struct CommandLineParser
{
    void   setup(CommandLine* cmdLine);
    bool   isArgValidFor(const string& swagCmd, CommandLineArgument* arg);
    bool   process(const string& swagCmd, int argc, const char* argv[]);
    void   addArg(const char* commands, const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help);
    void   logArguments(const string& cmd);
    string buildString(bool full);

    map<string, CommandLineArgument*> longNameArgs;
    map<string, CommandLineArgument*> shortNameArgs;
};
