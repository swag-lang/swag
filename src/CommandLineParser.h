#pragma once

struct CommandLine;

enum class CommandLineType
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
    CommandLineArgument(const char* commands, CommandLineType type, void* buffer, const char* param, const char* help);

    SetUtf8         cmdSet;
    Utf8            longName;
    Utf8            shortName;
    void*           buffer;
    const char*     param;
    const char*     help;
    CommandLineType type;
};

struct CommandLineParser
{
    void        setup(CommandLine* cmdLine);
    static bool isArgValidFor(const Utf8& swagCmd, const CommandLineArgument* arg);
    bool        process(const Utf8& swagCmd, int argc, const char* argv[]);
    void        addArg(const char* commands, const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help);
    void        logArguments(const Utf8& cmd) const;
    Utf8        buildString() const;

    MapUtf8<CommandLineArgument*> longNameArgs;
    MapUtf8<CommandLineArgument*> shortNameArgs;
};
