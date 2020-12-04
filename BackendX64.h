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

struct DbgLine
{
    uint32_t line;
    uint32_t byteOffset;
};

using DbgTypeIndex = uint32_t;
struct DbgTypeRecordArgList
{
    vector<DbgTypeIndex> args;
    uint32_t             count = 0;
};

struct DbgTypeRecordProcedure
{
    DbgTypeIndex returnType = 0;
    DbgTypeIndex argsType   = 0;
    uint16_t     numArgs    = 0;
};

struct DbgTypeRecordFuncId
{
    DbgTypeIndex type = 0;
};

struct DbgTypeField
{
    const char*  name            = nullptr;
    DbgTypeIndex type            = 0;
    uint32_t     offset          = 0;
    uint16_t     accessSpecifier = 0;
};

struct DbgTypeRecordFieldList
{
    vector<DbgTypeField> fields;
    uint16_t             kind = 0;
};

struct DbgTypeRecordStructure
{
    DbgTypeIndex fieldList   = 0;
    uint32_t     sizeOf      = 0;
    uint16_t     memberCount = 0;
    bool         forward     = false;
};

struct DbgTypeRecordEnum
{
    DbgTypeIndex fieldList      = 0;
    DbgTypeIndex underlyingType = 0;
    uint16_t     count          = 0;
};

struct DbgTypeRecordArray
{
    DbgTypeIndex elementType = 0;
    DbgTypeIndex indexType   = 0;
    uint32_t     sizeOf      = 0;
};

struct DbgTypeRecordPointer
{
    DbgTypeIndex pointeeType;
};

struct DbgTypeRecord
{
    AstNode*               node = nullptr;
    const char*            name = nullptr;
    DbgTypeRecordArgList   LF_ArgList;
    DbgTypeRecordProcedure LF_Procedure;
    DbgTypeRecordFuncId    LF_FuncId;
    DbgTypeRecordFieldList LF_FieldList;
    DbgTypeRecordStructure LF_Structure;
    DbgTypeRecordArray     LF_Array;
    DbgTypeRecordPointer   LF_Pointer;
    DbgTypeRecordEnum      LF_Enum;
    DbgTypeIndex           index = 0;
    uint16_t               kind  = 0;
};

struct CoffFunction
{
    VectorNative<uint16_t> unwind;
    VectorNative<DbgLine>  dbgLines;
    AstNode*               node         = nullptr;
    uint32_t               symbolIndex  = 0;
    uint32_t               startAddress = 0;
    uint32_t               endAddress   = 0;
    uint32_t               xdataOffset  = 0;
    uint32_t               sizeProlog   = 0;
    uint32_t               offsetStack  = 0;
    uint32_t               frameSize    = 0;
    bool                   wrapper      = false;
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
    CoffRelocationTable        relocTableDBGSSection;
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

    uint32_t* patchDBGSSectionRelocTableOffset = nullptr;
    uint16_t* patchDBGSSectionRelocTableCount  = nullptr;
    uint32_t* patchDBGSSectionFlags            = nullptr;

    uint32_t* patchCSOffset   = nullptr;
    uint32_t* patchCSCount    = nullptr;
    uint32_t* patchSSOffset   = nullptr;
    uint32_t* patchSSCount    = nullptr;
    uint32_t* patchGSOffset   = nullptr;
    uint32_t* patchGSCount    = nullptr;
    uint32_t* patchMSOffset   = nullptr;
    uint32_t* patchMSCount    = nullptr;
    uint32_t* patchTSOffset   = nullptr;
    uint32_t* patchTSCount    = nullptr;
    uint32_t* patchDRCount    = nullptr;
    uint32_t* patchDROffset   = nullptr;
    uint32_t* patchPDCount    = nullptr;
    uint32_t* patchPDOffset   = nullptr;
    uint32_t* patchXDCount    = nullptr;
    uint32_t* patchXDOffset   = nullptr;
    uint32_t* patchDBGSCount  = nullptr;
    uint32_t* patchDBGSOffset = nullptr;
    uint32_t* patchDBGTCount  = nullptr;
    uint32_t* patchDBGTOffset = nullptr;

    uint32_t symCOIndex = 0;
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
    uint32_t symPI_threadRun                    = 0;

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
    uint16_t sectionIndexDBGS = 0;
    uint16_t sectionIndexDBGT = 0;

    BackendPreCompilePass pass = {BackendPreCompilePass::Init};

    // Debug infos
    static const int             MAX_RECORD   = 4;
    uint16_t                     dbgRecordIdx = 0;
    uint16_t*                    dbgStartRecordPtr[MAX_RECORD];
    uint32_t                     dbgStartRecordOffset[MAX_RECORD];
    vector<DbgTypeRecord>        dbgTypeRecords;
    map<TypeInfo*, DbgTypeIndex> dbgMapTypes;
    map<Utf8, DbgTypeIndex>      dbgMapTypesNames;
};

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
        memset(perThread, 0, sizeof(perThread));
    }

    bool                    createRuntime(const BuildParameters& buildParameters);
    void                    alignConcat(Concat& concat, uint32_t align);
    JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    uint32_t getOrCreateLabel(X64PerThread& pp, uint32_t ip);
    void     setCalleeParameter(X64PerThread& pp, TypeInfo* typeParam, int calleeIndex, int stackOffset, uint32_t sizeStack);
    uint16_t computeUnwindPushRDI(uint32_t offsetSubRSP);
    void     computeUnwindStack(uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);
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

    void         dbgEmitEmbeddedValue(X64PerThread& pp, ComputedValue& val);
    void         dbgStartRecord(X64PerThread& pp, Concat& concat, uint16_t what);
    void         dbgEndRecord(X64PerThread& pp, Concat& concat, bool align = true);
    void         dbgEmitSecRel(X64PerThread& pp, Concat& concat, uint32_t symbolIndex, uint32_t segIndex);
    void         dbgEmitTruncatedString(Concat& concat, const Utf8& str);
    DbgTypeIndex dbgGetSimpleType(TypeInfo* typeInfo);
    DbgTypeIndex dbgGetOrCreateType(X64PerThread& pp, TypeInfo* typeInfo);
    void         dbgAddTypeRecord(X64PerThread& pp, DbgTypeRecord& tr);
    void         dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset);
    void         dbgEmitCompilerFlagsDebugS(Concat& concat);
    void         dbgEmitGlobalDebugS(X64PerThread& pp, Concat& concat, VectorNative<AstNode*>& gVars, uint32_t segSymIndex);
    bool         dbgEmitDataDebugT(const BuildParameters& buildParameters);
    bool         dbgEmitFctDebugS(const BuildParameters& buildParameters);
    bool         dbgEmit(const BuildParameters& buildParameters);

    bool saveObjFile(const BuildParameters& buildParameters);

    bool buildRelocConstantSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocTypeSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);
    bool buildRelocMutableSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    void emitLocalCallParameters(X64PerThread& pp, uint32_t sizeParamsStack, TypeInfoFuncAttr* typeFuncBC, uint32_t stackRR, const VectorNative<uint32_t>& pushRAParams);
    void emitSymbolRelocation(X64PerThread& pp, const Utf8& name);
    void emitCall(X64PerThread& pp, const Utf8& name);

    void emitForeignCallResult(X64PerThread& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT);
    bool emitForeignCall(X64PerThread& pp, Module* moduleToGen, ByteCodeInstruction* ip, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    bool emitForeignCallParameters(X64PerThread& pp, Module* moduleToGen, uint32_t offsetRT, TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams);

    CoffFunction* registerFunction(X64PerThread& pp, AstNode* node, uint32_t symbolIndex);
    void          registerFunction(CoffFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);

    X64PerThread* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};