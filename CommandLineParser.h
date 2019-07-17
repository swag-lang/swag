#pragma once

enum CommandLineType
{
    Bool,
    Int,
    Enum,
    String,
    StringList,
};

struct CommandLineArgument
{
    void*           buffer;
    CommandLineType type;
    const char*     param;
    string          longName;
    string          shortName;

    CommandLineArgument(CommandLineType type, void* buffer, const char* param)
        : type{type}
        , buffer{buffer}
        , param{param}
    {
    }
};

struct CommandLineParser
{
    void setup(CommandLine* cmdLine);
    bool process(int argc, const char* argv[]);
    void addArg(const char* longName, const char* shortName, CommandLineType type, void* address, const char* param = nullptr);
    void logArguments();

    map<string, CommandLineArgument*> longNameArgs;
    map<string, CommandLineArgument*> shortNameArgs;
};
