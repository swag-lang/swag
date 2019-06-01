#pragma once

enum CommandLineType
{
	Bool,
	Int,
};

struct CommandLineArgument
{
	void* buffer;
	CommandLineType type;

	CommandLineArgument(CommandLineType type, void* buffer) :
		type{ type },
		buffer{ buffer }
	{

	}
};

struct CommandLineParser
{
	void setup(CommandLine* cmdLine);
	bool process(int argc, const char* argv[]);
	void addArg(const char* longName, const char* shortName, CommandLineType type, void* address);

	map<string, CommandLineArgument*> longNameArgs;
	map<string, CommandLineArgument*> shortNameArgs;
};
