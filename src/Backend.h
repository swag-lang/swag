#pragma once
#include "Ast.h"
#include "AstOutput.h"
#include "BackendParameters.h"
#include "Concat.h"

struct AstFuncDecl;
struct AstNode;
struct AstStruct;
struct AstVarDecl;
struct BackendFunctionBodyJob;
struct BackendTarget;
struct BuildParameters;
struct Job;
struct Module;
struct Scope;
struct TypeInfo;
struct TypeInfoEnum;
struct TypeInfoFuncAttr;
struct TypeInfoParam;
struct TypeInfoStruct;
enum class JobResult;
enum class BuildCfgBackendKind;
enum class BuildCfgOutputKind;

static constexpr int MAX_PRECOMPILE_BUFFERS = 16;

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

    virtual JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob);
    virtual bool      generateOutput(const BuildParameters& backendParameters);
    virtual bool      emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);

    void setMustCompile();
    bool isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);

    static void addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end);
    void        getRangeFunctionIndexForJob(const BuildParameters& buildParameters, const Module* moduleToGen, int& start, int& end) const;
    bool        emitAllFunctionBodies(const BuildParameters& buildParameters, Module* moduleToGen, Job* ownerJob);

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
    static Path getOutputFileName(const BackendTarget& target, const Utf8& name, BuildCfgOutputKind type);

    static Utf8           getObjectFileExtension(const BackendTarget& target);
    static Utf8           getOutputFileExtension(const BackendTarget& target, BuildCfgOutputKind type);
    static BackendObjType getObjType(const BackendTarget& target);
    static const char*    getArchName(const BackendTarget& target);
    static const char*    getOsName(const BackendTarget& target);
    static uint64_t       getRuntimeFlags(Module* module);
};
