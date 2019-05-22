#pragma once
struct CommandLine
{
    bool process(int argc, const char* argv[]);

    bool test       = true;
    bool verbose    = false;
    bool silent     = false;
    bool stats      = true;
    bool syntaxOnly = false;

    int    tabSize = 4;
	string fileFilter;// = "51.swg";
};
