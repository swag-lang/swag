#pragma once
struct CommandLine
{
    bool test           = true;
    bool verbose        = false;
    bool silent         = false;
    bool stats          = false;
    bool syntaxOnly     = false;
    bool output         = true;
    bool errorSourceOut = true; // Errors will output source code
    bool errorNoteOut   = true; // Errors will output notes, if defined

    int    numCores = 0;
    int    tabSize  = 4;
    string fileFilter;

    fs::path exePath;
};
