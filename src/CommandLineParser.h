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
    vector<Utf8>    cmds;
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
        tokenize(commands, ',', all);

        for (auto& p : all)
        {
            if (p == "all" || p == "build")
            {
                cmds.push_back("build");
                cmds.push_back("run");
            }

            if (p == "all" || p == "build" || p == "test")
                cmds.push_back("test");

            if (p == "all" || p == "clean")
                cmds.push_back("clean");

            if (p == "all" || p == "watch")
                cmds.push_back("watch");

            if (p == "all" || p == "new")
                cmds.push_back("new");
        }
    }
};

struct CommandLineParser
{
    void   setup(CommandLine* cmdLine);
    bool   isArgValidFor(const string& swagCmd, CommandLineArgument* arg);
    bool   process(const string& swagCmd, int argc, const char* argv[]);
    void   addArg(const char* commands, const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help);
    void   logArguments();
    string buildString(bool full);

    map<string, CommandLineArgument*> longNameArgs;
    map<string, CommandLineArgument*> shortNameArgs;
};
