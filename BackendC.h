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
struct Utf8;

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    bool preCompile() override;
    bool mustCompile();
    bool compile(const BuildParameters& backendParameters) override;

    bool emitRuntime();
    bool emitDataSegment(DataSegment* dataSegment);
    bool emitStrings();
    void emitArgcArgv();
    bool emitMain();
    bool emitAllFunctionBody();
    bool emitAllFunctionBody(Module* moduleToGen);
    bool emitAllFuncSignatureInternalC();
    bool emitAllFuncSignatureInternalC(Module* moduleToGen);
    bool emitGlobalInit();
    bool emitGlobalDrop();

    bool emitPublic(Module* moduleToGen, Scope* scope);
    bool swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType);
    bool emitForeignCall(ByteCodeInstruction* ip, vector<uint32_t>& pushParams);
    void emitFuncSignatureSwg(Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitFuncWrapperPublic(Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);
    bool emitForeignFuncSignature(Module* moduleToGen, Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forExport);
    void emitFuncSignatureInternalC(ByteCode* bc);
    bool emitFunctionBody(Module* moduleToGen, ByteCode* bc);

    OutputFile bufferC;
};