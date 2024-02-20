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
    SetUtf8         cmdSet;
    Utf8            longName;
    Utf8            shortName;
    void*           buffer;
    const char*     param;
    const char*     help;
    CommandLineType type;

    CommandLineArgument(const char* commands, CommandLineType type, void* buffer, const char* param, const char* help) :
        buffer{buffer},
        param{param},
        help{help},
        type{type}
    {
        Vector<Utf8> all;
        Utf8::tokenize(commands, ' ', all);

        for (auto& p : all)
        {
            if (p == "all" || p == "bu")
            {
                cmdSet.insert("build");
                cmdSet.insert("run");
            }

            if (p == "all" || p == "sc")
            {
                cmdSet.insert("script");
            }

            if (p == "all" || p == "bu" || p == "te")
                cmdSet.insert("test");

            if (p == "all" || p == "cl")
                cmdSet.insert("clean");

            if (p == "all" || p == "ne")
                cmdSet.insert("new");

            if (p == "all" || p == "li")
                cmdSet.insert("list");

            if (p == "all" || p == "ge")
                cmdSet.insert("get");

            if (p == "all" || p == "ru")
                cmdSet.insert("run");

            if (p == "all" || p == "doc")
                cmdSet.insert("doc");
        }
    }
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
