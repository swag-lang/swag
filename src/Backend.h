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
struct BackendFunctionBodyJob;
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

    virtual JobResult prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob);
    virtual bool      generateOutput(const BuildParameters& backendParameters);
    virtual bool      emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);

    void setMustCompile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    void addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end);
    void getRangeFunctionIndexForJob(const BuildParameters& buildParameters, Module* moduleToGen, int& start, int& end);
    bool emitAllFunctionBodies(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob);

    JobResult generateExportFile(Job* ownerJob);
    bool      saveExportFile();
    bool      setupExportFile(bool force = false);

    Concat bufferSwg;
    Path   exportFileName;
    Path   exportFilePath;

    Module*               module               = nullptr;
    uint64_t              timeExportFile       = 0;
    int                   numPreCompileBuffers = 0;
    BackendPreCompilePass passExport           = BackendPreCompilePass::Init;

    AstOutput::OutputContext outputContext;
    bool                     mustCompile = true;

    static void setup();
    static Path getCacheFolder(const BuildParameters& buildParameters);
    static Path getOutputFileName(const BuildParameters& buildParameters);

    static Utf8           getObjectFileExtension(const BackendTarget& target);
    static Utf8           getOutputFileExtension(const BackendTarget& target, BuildCfgBackendKind type);
    static BackendObjType getObjType(const BackendTarget& target);
    static const char*    getArchName(const BackendTarget& target);
    static const char*    getOsName(const BackendTarget& target);
    static uint64_t       getRuntimeFlags(Module* module);
};
