#pragma once
struct CommandLine
{
    bool   test    = true;
    bool   verbose = false;
    bool   stats   = true;
    int    tabSize = 4;
	string fileFilter;// = "AMFImporter_Geometry.cpp";

    void process(int argc, const char* argv[]);
};
