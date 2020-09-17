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

struct CoffFunction
{
    uint32_t symbolIndex;
    uint32_t startAddress;
    uint32_t endAddress;
    uint32_t xdataOffset = 0;
    uint32_t sizeProlog  = 0;
    uint16_t unwind0     = 0;
    uint16_t unwind1     = 0;
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
    CoffRelocationTable        relocTablePDSection;
    vector<CoffSymbol>         allSymbols;
    map<Utf8Crc, uint32_t>     mapSymbols;
    map<Utf8Crc, uint32_t>     globalStrings;
    map<uint32_t, int32_t>     labels;
    DataSegment                globalSegment;
    DataSegment                stringSegment;
    VectorNative<LabelToSolve> labelsToSolve;
    Utf8                       directives;
    vector<CoffFunction>       functions;

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

    uint32_t* patchPDSectionRelocTableOffset = nullptr;
    uint16_t* patchPDSectionRelocTableCount  = nullptr;
    uint32_t* patchPDSectionFlags            = nullptr;

    uint32_t* patchCSOffset = nullptr;
    uint32_t* patchCSCount  = nullptr;
    uint32_t* patchSSOffset = nullptr;
    uint32_t* patchSSCount  = nullptr;
    uint32_t* patchGSOffset = nullptr;
    uint32_t* patchGSCount  = nullptr;
    uint32_t* patchMSOffset = nullptr;
    uint32_t* patchMSCount  = nullptr;
    uint32_t* patchTSOffset = nullptr;
    uint32_t* patchTSCount  = nullptr;
    uint32_t* patchDRCount  = nullptr;
    uint32_t* patchDROffset = nullptr;
    uint32_t* patchPDCount  = nullptr;
    uint32_t* patchPDOffset = nullptr;
    uint32_t* patchXDCount  = nullptr;
    uint32_t* patchXDOffset = nullptr;

    uint32_t symBSIndex = 0;
    uint32_t symMSIndex = 0;
    uint32_t symCSIndex = 0;
    uint32_t symTSIndex = 0;
    uint32_t symXDIndex = 0;

    uint32_t symMC_mainContext                  = 0;
    uint32_t symMC_mainContext_allocator_addr   = 0;
    uint32_t symMC_mainContext_allocator_itable = 0;
    uint32_t symMC_mainContext_flags            = 0;
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
    uint16_t sectionIndexGS   = 0;
    uint16_t sectionIndexCS   = 0;
    uint16_t sectionIndexSS   = 0;
    uint16_t sectionIndexTS   = 0;
    uint16_t sectionIndexDR   = 0;
    uint16_t sectionIndexPD   = 0;
    uint16_t sectionIndexXD   = 0;

    BackendPreCompilePass pass = {BackendPreCompilePass::Init};
};

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
    }

    bool                    createRuntime(const BuildParameters& buildParameters);
    void                    alignConcat(Concat& concat, uint32_t align);
    JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    uint32_t getOrCreateLabel(X64PerThread& pp, uint32_t ip);
    void     setCalleeParameter(X64PerThread& pp, TypeInfo* typeParam, int calleeIndex, int stackOffset, uint32_t sizeStack);
    void     computeUnwind(uint32_t sizeStack, uint32_t offsetSubRSP, uint16_t& unwind0, uint16_t& unwind1);
    bool     emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* bc);
    bool     emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);

    CoffSymbol* getSymbol(X64PerThread& pp, const Utf8Crc& name);
    CoffSymbol* getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);
    void        emitGlobalString(X64PerThread& pp, int precompileIndex, const Utf8Crc& str, uint8_t reg);

    bool emitXData(const BuildParameters& buildParameters);
    bool emitPData(const BuildParameters& buildParameters);
    bool emitDirectives(const BuildParameters& buildParameters);
    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTable(Concat& concat, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    bool emitHeader(const BuildParameters& buildParameters);

    bool saveObjFile(const BuildParameters& buildParameters);

    bool buildRelocConstantSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocTypeSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocMutableSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    void emitLocalCallParameters(X64PerThread& pp, uint32_t sizeParamsStack, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams);
    void emitSymbolRelocation(X64PerThread& pp, const Utf8& name);
    void emitCall(X64PerThread& pp, const Utf8& name);

    void emitForeignCallResult(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT);
    bool emitForeignCall(X64PerThread& pp, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    bool emitForeignCallParameters(X64PerThread& pp, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams);

    void registerFunction(X64PerThread& pp, uint32_t symbolIndex, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, uint16_t unwind0 = 0, uint16_t unwind1 = 0);

    X64PerThread perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};