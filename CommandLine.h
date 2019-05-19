#pragma once
struct CommandLine
{
    bool   test       = true;
	bool   verbose = false;
    int    tabSize    = 4;
	string fileFilter;// = "UnitTest.cpp";

    void process(int argc, const char* argv[]);
};
