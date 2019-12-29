#pragma once
#include "OutputFile.h"
struct Module;
struct BuildParameters;
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct TypeInfoEnum;
struct AstFuncDecl;
struct AstStruct;
struct AstNode;
struct Scope;
struct TypeInfoParam;
struct AstVarDecl;
struct Job;
enum class JobResult;

static const int MAX_PRECOMPILE_BUFFERS = 128;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual bool      check()                                           = 0;
    virtual JobResult preCompile(Job* ownerJob, int preCompileIndex)    = 0;
    virtual bool      compile(const BuildParameters& backendParameters) = 0;

    void emitSeparator(Concat& buffer, const char* title);
    bool generateExportFile();
    void setMustCompile();
    void setupExportFile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    bool emitAttributes(AstNode* node);
    bool emitAttributes(TypeInfoParam* param);
    bool emitGenericParameters(AstNode* node);
    bool emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node);
    bool emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node);
    bool emitPublicConstSwg(AstVarDecl* node);
    bool emitPublicTypeAliasSwg(AstNode* node);
    bool emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node);
    bool emitPublicSwg(Module* moduleToGen, Scope* scope);

    OutputFile bufferSwg;

    Module*  module               = nullptr;
    uint64_t timeExportFile       = 0;
    int      numPreCompileBuffers = 0;

    bool mustCompile         = true;
    bool exportFileGenerated = false;
};
