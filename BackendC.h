#pragma once
#include "Concat.h"
struct Module;
struct AstNode;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct TypeInfo;

struct BackendC
{
    BackendC(Module* mdl)
        : module{mdl}
    {
    }

    bool writeFile(const char* fileName, Concat& concat);
    bool compile();
    bool generate();

    bool emitHeader();
    bool emitFooter();
    bool emitRuntime();
    bool emitDataSegment();
    bool emitMain();
    bool emitFunctions();
    bool emitFuncSignatures();

    const char* swagTypeToCType(TypeInfo* typeInfo);
    void        emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignatureC(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignatureH(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);

    Module* module;

    fs::path destFileH;
    fs::path destFileC;
    fs::path destFileSwg;
    fs::path destFile;

    Concat bufferH;
    Concat bufferC;
    Concat bufferSwg;
};
