#pragma once
#include "OutputFile.h"
struct Module;
struct BuildParameters;
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct AstFuncDecl;
struct AstStruct;
struct Scope;

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

	void emitStructSignatureSwg(TypeInfoStruct* typeStruct, AstStruct* node);
    void emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void emitPublicSignaturesSwg(Module* moduleToGen, Scope* scope);
};
