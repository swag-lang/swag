#pragma once
#include "Backend.h"
#include "OutputFile.h"
struct Module;
struct AstNode;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct TypeInfo;
struct BackendParameters;

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    bool generate() override;
    bool compile(const BackendParameters& backendParameters) override;

    bool writeFile(OutputFile& concat);

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
    void        emitFuncSignatureInternalC(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool        emitInternalFunction(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);

    string destFile;

    OutputFile bufferH;
    OutputFile bufferC;
    OutputFile bufferSwg;
};