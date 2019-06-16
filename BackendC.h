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

    void emitSeparator(Concat& buffer, const char* title);
    bool emitHeader();
    bool emitFooter();
    bool emitRuntime();
    bool emitDataSegment();
    bool emitMain();
    bool emitFunctions();
    bool emitFuncSignatures();

    const char* swagTypeToCType(TypeInfo* typeInfo);
    void        emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignatureInternalC(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool        emitInternalFunction(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);

    Module* module;

    string destFileH;
    string destFileC;
    string destFileSwg;
    string destFile;

    Concat bufferH;
    Concat bufferC;
    Concat bufferSwg;
};
