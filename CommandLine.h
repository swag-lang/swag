#pragma once
struct CommandLine
{
    bool test       = true;
    bool verbose    = false;
    bool silent     = false;
    bool stats      = false;
    bool syntaxOnly = false;
    bool output     = true;

    int    numCores = 0;
    int    tabSize  = 4;
	string fileFilter;// = "120";

    fs::path exePath;
};
