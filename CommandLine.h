#pragma once
struct CommandLine
{
    bool test       = true;
    bool verbose    = false;
    bool stats      = true;
    bool syntaxOnly = false;

    int    tabSize = 4;
    string fileFilter; // = "30.swg";

    bool process(int argc, const char* argv[]);
};
