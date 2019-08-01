#pragma once
#include "Backend.h"
#include "OutputFile.h"
struct Module;
struct AstNode;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct TypeInfo;
struct BackendParameters;
struct ByteCode;

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    bool generate() override;
    bool compile(const BackendParameters& backendParameters) override;

    void emitSeparator(Concat& buffer, const char* title);
    bool emitHeader();
    bool emitFooter();
    bool emitRuntime();
    bool emitDataSegment();
    bool emitConstantSegment();
    bool emitStrings();
    bool emitMain();
    bool emitFunctions();
    bool emitFuncSignatures();
    bool emitGlobalInit();

    const char* swagTypeToCType(TypeInfo* typeInfo);
    void        emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignatureInternalC(TypeInfoFuncAttr* typeFunc, const string& name);
    void        emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool        emitInternalFunction(TypeInfoFuncAttr* typeFunc, ByteCode* bc, const string& name);

    string destFile;

    OutputFile bufferH;
    OutputFile bufferC;
    OutputFile bufferSwg;
};