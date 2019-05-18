#pragma once
struct CommandLine
{
	bool verbose = true;
    int  tabSize = 4;

	void process(int argc, const char *argv[]);
};

