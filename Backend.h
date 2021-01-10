#pragma once
#include "OutputFile.h"
#include "Ast.h"
struct Module;
struct BuildParameters;
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct TypeInfoEnum;
struct TypeInfo;
struct AstFuncDecl;
struct AstStruct;
struct AstNode;
struct Scope;
struct TypeInfoParam;
struct AstVarDecl;
struct Job;
struct BackendFunctionBodyJob;
enum class JobResult;

static const int MAX_PRECOMPILE_BUFFERS = 16;
enum class BackendPreCompilePass
{
    Init,
    FunctionBodies,
    End,
    GenerateObj,
    Release,
};

struct Backend
{
    Backend(Module* mdl)
        : module{mdl}
    {
    }

    virtual JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob);
    virtual bool                    generateOutput(const BuildParameters& backendParameters);
    virtual BackendFunctionBodyJob* newFunctionJob();

    void setMustCompile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    void addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end);
    void getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end);
    bool emitAllFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob);

    JobResult generateExportFile(Job* ownerJob);
    bool      saveExportFile();
    bool      setupExportFile(bool force = false);
    bool      emitAttributesUsage(TypeInfoFuncAttr* typeFunc, int indent);
    bool      emitAttributes(TypeInfo* typeInfo, int indent);
    bool      emitTypeTuple(TypeInfo* typeInfo, int indent);
    void      emitType(TypeInfo* typeInfo, int indent);
    bool      emitGenericParameters(AstNode* node, int indent);
    bool      emitPublicEnumSwg(TypeInfoEnum* typeEnum, AstNode* node, int indent);
    bool      emitPublicStructSwg(TypeInfoStruct* typeStruct, AstStruct* node, int indent);
    bool      emitVarSwg(const char* kindName, AstVarDecl* node, int indent);
    bool      emitPublicFuncSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent);
    bool      emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, int indent);
    bool      emitPublicScopeContentSwg(Module* moduleToGen, Scope* scope, int indent);
    bool      emitPublicScopeSwg(Module* moduleToGen, Scope* scope, int indent);

    OutputFile bufferSwg;

    Module*               module               = nullptr;
    uint64_t              timeExportFile       = 0;
    int                   numPreCompileBuffers = 0;
    BackendPreCompilePass passExport           = BackendPreCompilePass::Init;

    Ast::OutputContext outputContext;
    bool               mustCompile = true;

    static string    compilerExe;
    static string    compilerPath;
    static string    linkerExe;
    static string    linkerPath;
    static void      setup();
    static string    getCacheFolder(const BuildParameters& buildParameters);
    static string    getOutputFileName(const BuildParameters& buildParameters);
    static bool      passByValue(TypeInfo* typeInfo);
    static TypeInfo* registerIdxToType(TypeInfoFuncAttr* typeFunc, int argIdx);
};
