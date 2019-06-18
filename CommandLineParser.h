#pragma once

enum CommandLineType
{
    Bool,
    Int,
	Enum,
	StringList,
};

struct CommandLineArgument
{
    void*           buffer;
    CommandLineType type;
    const char*     param;

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

    map<string, CommandLineArgument*> longNameArgs;
    map<string, CommandLineArgument*> shortNameArgs;
};
