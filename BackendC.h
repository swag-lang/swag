#pragma once
#include "Concat.h"
struct Module;
struct AstNode;

struct BackendC
{
    BackendC(Module* mdl)
        : module{mdl}
    {
    }

    bool writeFile(const char* fileName, Concat& concat);
    bool compile();
    bool generate();

    bool emitRuntime();
    bool emitDataSegment();
    bool emitMain();
    bool emitFunctions();

    Module* module;

    fs::path destHFile;
    fs::path destCFile;
    fs::path outputFile;

    Concat outputH;
    Concat outputC;

    vector<AstNode*> testFuncs;
};
