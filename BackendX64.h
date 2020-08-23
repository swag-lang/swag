#pragma once
#include "Utf8Crc.h"
#include "Backend.h"
#include "BuildParameters.h"
#include "DataSegment.h"

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
    Custom,
    GlobalString,
};

struct CoffSymbol
{
    Utf8Crc        name;
    CoffSymbolKind kind;
    uint32_t       value;
    uint32_t       index;
    uint16_t       sectionIdx;
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

struct LabelToSolve
{
    uint32_t ipDest;
    int32_t  currentOffset;
    uint8_t* patch;
};

struct X64PerThread
{
    string filename;
    Concat concat;
    Concat postConcat;

    VectorNative<const Utf8*>  stringTable;
    CoffRelocationTable        relocTableTextSection;
    CoffRelocationTable        relocTableCSSection;
    CoffRelocationTable        relocTableMSSection;
    CoffRelocationTable        relocTableTSSection;
    vector<CoffSymbol>         allSymbols;
    map<Utf8Crc, uint32_t>     mapSymbols;
    map<Utf8Crc, uint32_t>     globalStrings;
    map<uint32_t, int32_t>     labels;
    DataSegment                stringSegment;
    VectorNative<LabelToSolve> labelsToSolve;
    Utf8                       directives;

    uint32_t* patchSymbolTableOffset = nullptr;
    uint32_t* patchSymbolTableCount  = nullptr;
    uint32_t* patchTextSectionOffset = nullptr;
    uint32_t* patchTextSectionSize   = nullptr;

    uint32_t* patchTextSectionRelocTableOffset = nullptr;
    uint16_t* patchTextSectionRelocTableCount  = nullptr;
    uint32_t* patchTextSectionFlags            = nullptr;

    uint32_t* patchCSSectionRelocTableOffset = nullptr;
    uint16_t* patchCSSectionRelocTableCount  = nullptr;
    uint32_t* patchCSSectionFlags            = nullptr;

    uint32_t* patchMSSectionRelocTableOffset = nullptr;
    uint16_t* patchMSSectionRelocTableCount  = nullptr;
    uint32_t* patchMSSectionFlags            = nullptr;

    uint32_t* patchTSSectionRelocTableOffset = nullptr;
    uint16_t* patchTSSectionRelocTableCount  = nullptr;
    uint32_t* patchTSSectionFlags            = nullptr;

    uint32_t* patchCSOffset = nullptr;
    uint32_t* patchCSCount  = nullptr;
    uint32_t* patchMSOffset = nullptr;
    uint32_t* patchMSCount  = nullptr;
    uint32_t* patchTSOffset = nullptr;
    uint32_t* patchTSCount  = nullptr;
    uint32_t* patchDRCount  = nullptr;
    uint32_t* patchDROffset = nullptr;

    uint32_t symBSIndex = 0;
    uint32_t symMSIndex = 0;
    uint32_t symCSIndex = 0;
    uint32_t symTSIndex = 0;

    uint32_t symMC_mainContext                  = 0;
    uint32_t symMC_mainContext_allocator_addr   = 0;
    uint32_t symMC_mainContext_allocator_itable = 0;
    uint32_t symDefaultAllocTable               = 0;
    uint32_t symPI_processInfos                 = 0;
    uint32_t symPI_args_addr                    = 0;
    uint32_t symPI_args_count                   = 0;
    uint32_t symPI_contextTlsId                 = 0;
    uint32_t symPI_defaultContext               = 0;
    uint32_t symPI_byteCodeRun                  = 0;

    uint32_t textSectionOffset = 0;
    uint32_t stringTableOffset = 0;

    uint16_t sectionIndexText = 0;
    uint16_t sectionIndexBS   = 0;
    uint16_t sectionIndexMS   = 0;
    uint16_t sectionIndexCS   = 0;
    uint16_t sectionIndexTS   = 0;
    uint16_t sectionIndexDR   = 0;

    BackendPreCompilePass pass = {BackendPreCompilePass::Init};
};

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
    }

    bool                    createRuntime(const BuildParameters& buildParameters);
    JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    uint32_t getOrCreateLabel(X64PerThread& pp, uint32_t ip);
    void     setCalleeParameter(X64PerThread& pp, TypeInfo* typeParam, int calleeIndex, int stackOffset, uint32_t sizeStack);
    bool     emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc);
    bool     emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);

    CoffSymbol* getSymbol(X64PerThread& pp, const Utf8Crc& name);
    CoffSymbol* getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);
    void        emitGlobalString(X64PerThread& pp, int precompileIndex, const Utf8Crc& str, uint8_t reg);

    bool emitDirectives(const BuildParameters& buildParameters);
    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTable(Concat& concat, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    bool emitHeader(const BuildParameters& buildParameters);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool buildRelocConstantSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocTypeSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocMutableSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    uint32_t emitLocalCallParameters(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams);
    void     emitSymbolRelocation(X64PerThread& pp, const Utf8& name);
    void     emitCall(X64PerThread& pp, const Utf8& name);

    void emitForeignCallResult(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT);
    bool emitForeignCall(X64PerThread& pp, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    bool emitForeignCallParameters(X64PerThread& pp, uint32_t& exceededStack, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams);

    X64PerThread perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};