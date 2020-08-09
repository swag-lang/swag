#pragma once
#include "Utf8Crc.h"
#include "Backend.h"
#include "BuildParameters.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

enum class PatchType
{
    SymbolTableOffset,
    SymbolTableCount,
    TextSectionOffset,
    TextSectionSize,
    TextSectionRelocTableOffset,
    TextSectionRelocTableCount,
};

enum class CoffSymbolKind
{
    Function,
    Extern,
};

struct CoffSymbol
{
    Utf8Crc        name;
    CoffSymbolKind kind;
    uint32_t       value;
    uint32_t       index;
};

struct CoffRelocation
{
    uint32_t virtualAddress;
    uint32_t symbolIndex;
    uint16_t type;
};

struct CoffRelocationTable
{
    vector<CoffRelocation> table;
};

struct X64PerThread
{
    string                filename;
    Concat                concat;
    map<PatchType, void*> allPatches;

    uint32_t            textSectionOffset = 0;
    vector<const Utf8*> stringTable;
    uint32_t            stringTableOffset = 0;

    CoffRelocationTable relocationTextSection;

    vector<CoffSymbol>     allSymbols;
    map<Utf8Crc, uint32_t> mapSymbols;

    BackendPreCompilePass pass = {BackendPreCompilePass::Init};
};

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
    }

    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);

    void        applyPatch(X64PerThread& pp, PatchType type, uint32_t value);
    void        addPatch(X64PerThread& pp, PatchType type, void* addr);
    CoffSymbol* getSymbol(X64PerThread& pp, const Utf8Crc& name);
    CoffSymbol* getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value = 0);

    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTables(const BuildParameters& buildParameters);
    bool emitHeader(const BuildParameters& buildParameters);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitDataSeg(const BuildParameters& buildParameters);
    bool emitInitConstantSeg(const BuildParameters& buildParameters);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    void emitCall(X64PerThread& pp, const Utf8& name);
    void emitRet(X64PerThread& pp);

    X64PerThread perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};