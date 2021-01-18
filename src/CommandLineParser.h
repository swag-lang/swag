#pragma once
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
    string          longName;
    string          shortName;
    void*           buffer;
    const char*     param;
    const char*     help;
    CommandLineType type;

    CommandLineArgument(CommandLineType type, void* buffer, const char* param, const char* help)
        : type{type}
        , buffer{buffer}
        , param{param}
        , help{help}
    {
    }
};

struct CommandLineParser
{
    void   setup(CommandLine* cmdLine);
    bool   process(int argc, const char* argv[]);
    void   addArg(const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help);
    void   logArguments();
    string buildString(bool full);

    map<string, CommandLineArgument*> longNameArgs;
    map<string, CommandLineArgument*> shortNameArgs;
};
