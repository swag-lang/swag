#pragma once
#include "Concat.h"
#include "Ast.h"
#include "AstOutput.h"
#include "BackendParameters.h"
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
struct BackendFunctionBodyJobBase;
enum class JobResult;
enum class BuildCfgBackendKind;

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

    virtual JobResult                   prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob);
    virtual bool                        generateOutput(const BuildParameters& backendParameters);
    virtual BackendFunctionBodyJobBase* newFunctionJob();

    void setMustCompile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    void        addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJobBase* job, int start, int end);
    void        getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end);
    bool        emitAllFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob);

    JobResult generateExportFile(Job* ownerJob);
    bool      saveExportFile();
    bool      setupExportFile(bool force = false);

    Concat bufferSwg;
    string exportFileName;
    string exportFilePath;

    Module*               module               = nullptr;
    uint64_t              timeExportFile       = 0;
    int                   numPreCompileBuffers = 0;
    BackendPreCompilePass passExport           = BackendPreCompilePass::Init;

    AstOutput::OutputContext outputContext;
    bool                     mustCompile = true;

    static void   setup();
    static string getCacheFolder(const BuildParameters& buildParameters);
    static Utf8   getOutputFileName(const BuildParameters& buildParameters);
    static bool   passByValue(TypeInfo* typeInfo);

    static string         getObjectFileExtension(const BackendTarget& target);
    static string         getOutputFileExtension(const BackendTarget& target, BuildCfgBackendKind type);
    static BackendObjType getObjType(const BackendTarget& target);
    static const char*    GetArchName(const BackendTarget& target);
    static const char*    GetOsName(const BackendTarget& target);
};
