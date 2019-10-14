#pragma once
#include "Backend.h"
#include "OutputFile.h"
struct Module;
struct AstNode;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct TypeInfo;
struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct DataSegment;

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    bool preCompile() override;
    bool compile(const BuildParameters& backendParameters) override;

    void emitSeparator(Concat& buffer, const char* title);
    bool emitHeader();
    bool emitFooter();
    bool emitRuntime();
    bool emitDataSegment(DataSegment* dataSegment);
    bool emitStrings();
    void emitArgcArgv();
    bool emitMain();
    bool emitFunctions();
    bool emitFunctions(Module* moduleToGen);
    bool emitFuncSignatures();
    bool emitFuncSignatures(Module* moduleToGen);
    bool emitGlobalInit();
    bool emitGlobalDrop();

    const char* swagTypeToCType(TypeInfo* typeInfo);
    bool        emitForeignCall(ByteCodeInstruction* ip, vector<uint32_t>& pushParams);
    void        emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    void        emitFuncSignatureInternalC(ByteCode* bc);
    bool        emitInternalFunction(Module* moduleToGen, ByteCode* bc);

    OutputFile bufferH;
    OutputFile bufferC;
    OutputFile bufferSwg;
};