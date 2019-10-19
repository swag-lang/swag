#pragma once
#include "OutputFile.h"
struct Module;
struct BuildParameters;
struct TypeInfoFuncAttr;
struct AstFuncDecl;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    void generateSwg();
    void emitSeparator(Concat& buffer, const char* title);

    virtual bool preCompile();
    virtual bool compile(const BuildParameters& backendParameters) = 0;

    Module*    module;
    OutputFile bufferSwg;

    void emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitFuncSignaturesSwg(Module* moduleToGen);
	bool emitFuncSignaturesSwg();
};
