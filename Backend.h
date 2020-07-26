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
struct BackendFunctionBodyJob;
enum class JobResult;

static const int MAX_PRECOMPILE_BUFFERS = 128;

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual void                    setup()                                                           = 0;
    virtual JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) = 0;
    virtual bool                    compile(const BuildParameters& backendParameters)                 = 0;
    virtual BackendFunctionBodyJob* newFunctionJob()                                                  = 0;

    void setMustCompile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    void addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end);
    void getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end);
    bool emitAllFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob);

    bool generateExportFile();
    void setupExportFile();
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
