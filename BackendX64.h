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
    string filename;
    Concat concat;

    vector<const Utf8*>    stringTable;
    CoffRelocationTable    relocTableTextSection;
    vector<CoffSymbol>     allSymbols;
    map<Utf8Crc, uint32_t> mapSymbols;

    uint32_t* patchSymbolTableOffset           = nullptr;
    uint32_t* patchSymbolTableCount            = nullptr;
    uint32_t* patchTextSectionOffset           = nullptr;
    uint32_t* patchTextSectionSize             = nullptr;
    uint32_t* patchTextSectionRelocTableOffset = nullptr;
    uint16_t* patchTextSectionRelocTableCount  = nullptr;
    uint32_t* patchTextSectionFlags            = nullptr;
    uint32_t* patchCSOffset                    = nullptr;
    uint32_t* patchDSOffset                    = nullptr;

    uint32_t textSectionOffset = 0;
    uint32_t stringTableOffset = 0;

    uint16_t sectionIndexText = 0;
    uint16_t sectionIndexBS   = 0;
    uint16_t sectionIndexCS   = 0;
    uint16_t sectionIndexDS   = 0;

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

    CoffSymbol* getSymbol(X64PerThread& pp, const Utf8Crc& name);
    CoffSymbol* getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value = 0);

    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTable(X64PerThread& pp, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint32_t* offset, uint16_t* count);
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