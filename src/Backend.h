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

static constexpr int MAX_PRECOMPILE_BUFFERS = 1024;

enum class BackendPreCompilePass
{
    Init,
    FunctionBodies,
    End,
    GenerateObj,
    Release,
};

struct BackendEncoder
{
    Utf8                  filename;
    Module*               module = nullptr;
    BackendPreCompilePass pass   = {BackendPreCompilePass::Init};
};

struct Backend
{
    explicit Backend(Module* mdl);
    virtual  ~Backend() = default;

    virtual JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob);
    virtual bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc);

    void        setMustCompile();
    bool        isUpToDate(uint64_t moreRecentSourceFile, bool invert = false);
    static void addFunctionsToJob(Module* moduleToGen, BackendFunctionBodyJob* job, int start, int end);
    void        getRangeFunctionIndexForJob(const BuildParameters& buildParameters, int& start, int& end) const;
    bool        emitAllFunctionBodies(const BuildParameters& buildParameters, Job* ownerJob);
    JobResult   generateExportFile(Job* ownerJob);
    bool        saveExportFile();
    bool        setupExportFile(bool force = false);
    bool        generateOutput(const BuildParameters& buildParameters) const;

    static void           setup();
    static Path           getCacheFolder();
    static Path           getOutputFileName(const BackendTarget& target, const Utf8& name, BuildCfgOutputKind type);
    static Utf8           getObjectFileExtension(const BackendTarget& target);
    static Utf8           getOutputFileExtension(const BackendTarget& target, BuildCfgOutputKind type);
    static BackendObjType getObjType(const BackendTarget& target);
    static const char*    getArchName(const BackendTarget& target);
    static const char*    getOsName(const BackendTarget& target);
    static uint64_t       getRuntimeFlags();

    template<typename T>
    void allocatePerObj(const BuildParameters& buildParameters)
    {
        const auto ct              = buildParameters.compileType;
        const auto precompileIndex = buildParameters.precompileIndex;

        if (!perThread[ct][precompileIndex])
        {
            auto n                         = new T;
            n->module                      = buildParameters.module;
            perThread[ct][precompileIndex] = n;
        }
    }

    template<typename T>
    T& encoder(BackendCompileType ct, uint32_t th) const
    {
        return *static_cast<T*>(perThread[ct][th]);
    }

    BackendEncoder* perThread[Count][MAX_PRECOMPILE_BUFFERS];

    Concat bufferSwg;
    Path   exportFileName;
    Path   exportFilePath;

    Module*               module               = nullptr;
    uint64_t              timeExportFile       = 0;
    uint32_t              numPreCompileBuffers = 0;
    BackendPreCompilePass passExport           = BackendPreCompilePass::Init;

    AstOutput::OutputContext outputContext;
    bool                     mustCompile = true;
};
