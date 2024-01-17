#pragma once
#include "Backend.h"
#include "ComputedValue.h"
#include "DataSegment.h"
#include "CallConv.h"
#include "Concat.h"

#define REG_OFFSET(__r) __r * sizeof(Register)

#define MK_ALIGN16(__s) \
    if (__s % 16)       \
        __s += 16 - (__s % 16);

enum class CPUBits : uint32_t
{
    B8  = 8,
    B16 = 16,
    B32 = 32,
    B64 = 64,
};

enum class CPUPushParamType
{
    Reg,
    RegAdd,
    RegMul,
    RAX,
    Imm,
    Imm64,
    RelocV,
    RelocAddr,
    Addr,
    GlobalString,
};

struct CPUPushParam
{
    CPUPushParamType type = CPUPushParamType::Reg;
    uint64_t         reg  = 0;
    uint64_t         val  = 0;
};

enum class CPUOp : uint8_t
{
    ADD    = 0x01,
    OR     = 0x09,
    AND    = 0x21,
    SUB    = 0x29,
    CVTI2F = 0x2A,
    CVTF2I = 0x2C,
    XOR    = 0x31,
    CMOVB  = 0x42,
    CMOVE  = 0x44,
    CMOVBE = 0x46,
    CMOVL  = 0x4C,
    CMOVGE = 0x4D,
    CMOVG  = 0x4F,
    FADD   = 0x58,
    FMUL   = 0x59,
    CVTF2F  = 0x5A,
    FSUB   = 0x5C,
    FDIV   = 0x5E,
    XCHG   = 0x87,
    MUL    = 0xC0,
    IMUL   = 0xC1,
    SHL    = 0xE0,
    SHR    = 0xE8,
    SAL    = 0xF0,
    DIV    = 0xF1,
    MOD    = DIV | 2,
    SAR    = 0xF8,
    IDIV   = 0xF9,
    IMOD   = IDIV | 2,
};

enum CPUJumpType
{
    JNO,
    JNZ,
    JZ,
    JL,
    JLE,
    JB,
    JBE,
    JGE,
    JAE,
    JG,
    JA,
    JP,
    JNP,
    JUMP,
};

struct LabelToSolve
{
    uint32_t ipDest;
    int32_t  currentOffset;
    uint8_t* patch;
};

using DbgTypeIndex = uint32_t;
struct DbgTypeRecordArgList
{
    Vector<DbgTypeIndex> args;
    uint32_t             count = 0;
};

struct DbgTypeRecordProcedure
{
    DbgTypeIndex returnType = 0;
    DbgTypeIndex argsType   = 0;
    uint16_t     numArgs    = 0;
};

struct DbgTypeRecordMFunction
{
    DbgTypeIndex returnType = 0;
    DbgTypeIndex structType = 0;
    DbgTypeIndex thisType   = 0;
    DbgTypeIndex argsType   = 0;
    uint16_t     numArgs    = 0;
};

struct DbgTypeRecordFuncId
{
    DbgTypeIndex type = 0;
};

struct DbgTypeRecordMFuncId
{
    DbgTypeIndex parentType = 0;
    DbgTypeIndex type       = 0;
};

struct DbgTypeField
{
    Utf8          name;
    ComputedValue value;
    TypeInfo*     valueType       = nullptr;
    DbgTypeIndex  type            = 0;
    uint16_t      accessSpecifier = 0;
    uint16_t      kind            = 0;
};

struct DbgTypeRecordFieldList
{
    Vector<DbgTypeField> fields;
};

struct DbgTypeRecordDerivedList
{
    Vector<DbgTypeIndex> derived;
};

struct DbgTypeRecordStructure
{
    DbgTypeIndex fieldList   = 0;
    DbgTypeIndex derivedList = 0;
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
    DbgTypeIndex pointeeType = 0;
    bool         asRef       = false;
};

struct DbgTypeRecord
{
    Utf8                     name;
    AstNode*                 node = nullptr;
    DbgTypeRecordArgList     LF_ArgList;
    DbgTypeRecordProcedure   LF_Procedure;
    DbgTypeRecordMFunction   LF_MFunction;
    DbgTypeRecordFuncId      LF_FuncId;
    DbgTypeRecordMFuncId     LF_MFuncId;
    DbgTypeRecordFieldList   LF_FieldList;
    DbgTypeRecordDerivedList LF_DerivedList;
    DbgTypeRecordStructure   LF_Structure;
    DbgTypeRecordArray       LF_Array;
    DbgTypeRecordPointer     LF_Pointer;
    DbgTypeRecordEnum        LF_Enum;
    DbgTypeIndex             index = 0;
    uint16_t                 kind  = 0;
};

struct DbgLine
{
    uint32_t line;
    uint32_t byteOffset;
};

struct DbgLines
{
    SourceFile*     sourceFile;
    Vector<DbgLine> lines;
    AstFuncDecl*    inlined = nullptr;
};

enum class CoffSymbolKind
{
    Function,
    Extern,
    Custom,
    GlobalString,
};

struct CoffSymbol
{
    Utf8           name;
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
    Vector<CoffRelocation> table;
};

struct CoffFunction
{
    VectorNative<uint16_t> unwind;
    Vector<DbgLines>       dbgLines;
    AstNode*               node                    = nullptr;
    TypeInfoFuncAttr*      typeFunc                = nullptr;
    uint32_t               symbolIndex             = 0;
    uint32_t               startAddress            = 0;
    uint32_t               endAddress              = 0;
    uint32_t               xdataOffset             = 0;
    uint32_t               sizeProlog              = 0;
    uint32_t               offsetStack             = 0;
    uint32_t               offsetCallerStackParams = 0;
    uint32_t               offsetLocalStackParams  = 0;
    uint32_t               frameSize               = 0;
    uint32_t               numScratchRegs          = 0;
};

struct EncoderCPU
{
    void clearInstructionCache();

    Utf8   filename;
    Concat concat;
    Concat postConcat;

    VectorNative<const Utf8*>  stringTable;
    VectorNative<CPUPushParam> pushParams;
    VectorNative<CPUPushParam> pushParams2;
    VectorNative<CPUPushParam> pushParams3;
    VectorNative<TypeInfo*>    pushParamsTypes;
    CoffRelocationTable        relocTableTextSection;
    CoffRelocationTable        relocTableCSSection;
    CoffRelocationTable        relocTableMSSection;
    CoffRelocationTable        relocTableTSSection;
    CoffRelocationTable        relocTableTLSSection;
    CoffRelocationTable        relocTablePDSection;
    CoffRelocationTable        relocTableDBGSSection;
    Vector<CoffSymbol>         allSymbols;
    MapUtf8<uint32_t>          mapSymbols;
    MapUtf8<uint32_t>          globalStrings;
    Map<uint32_t, int32_t>     labels;
    DataSegment                globalSegment;
    DataSegment                stringSegment;
    VectorNative<LabelToSolve> labelsToSolve;
    Utf8                       directives;
    Vector<CoffFunction>       functions;

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

    uint32_t* patchTLSSectionRelocTableOffset = nullptr;
    uint16_t* patchTLSSectionRelocTableCount  = nullptr;
    uint32_t* patchTLSSectionFlags            = nullptr;

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
    uint32_t* patchTLSOffset  = nullptr;
    uint32_t* patchTLSCount   = nullptr;

    uint32_t symCOIndex  = 0;
    uint32_t symBSIndex  = 0;
    uint32_t symMSIndex  = 0;
    uint32_t symCSIndex  = 0;
    uint32_t symTLSIndex = 0;
    uint32_t symXDIndex  = 0;

    uint32_t symMC_mainContext                  = 0;
    uint32_t symMC_mainContext_allocator_addr   = 0;
    uint32_t symMC_mainContext_allocator_itable = 0;
    uint32_t symMC_mainContext_flags            = 0;
    uint32_t symDefaultAllocTable               = 0;
    uint32_t symTls_threadLocalId               = 0;
    uint32_t symPI_processInfos                 = 0;
    uint32_t symPI_modulesAddr                  = 0;
    uint32_t symPI_modulesCount                 = 0;
    uint32_t symPI_argsAddr                     = 0;
    uint32_t symPI_argsCount                    = 0;
    uint32_t symPI_contextTlsId                 = 0;
    uint32_t symPI_defaultContext               = 0;
    uint32_t symPI_byteCodeRun                  = 0;
    uint32_t symPI_makeCallback                 = 0;
    uint32_t symPI_backendKind                  = 0;
    uint32_t symCst_U64F64                      = 0;

    uint32_t textSectionOffset = 0;
    uint32_t stringTableOffset = 0;

    uint16_t sectionIndexText = 0;
    uint16_t sectionIndexBS   = 0;
    uint16_t sectionIndexMS   = 0;
    uint16_t sectionIndexGS   = 0;
    uint16_t sectionIndexCS   = 0;
    uint16_t sectionIndexSS   = 0;
    uint16_t sectionIndexTS   = 0;
    uint16_t sectionIndexTLS  = 0;
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
    uint32_t                     dbgTypeRecordsCount = 0;
    Concat                       dbgTypeRecords;
    Map<TypeInfo*, DbgTypeIndex> dbgMapTypes;
    MapUtf8<DbgTypeIndex>        dbgMapTypesNames;

    uint32_t    storageRegCount = UINT32_MAX;
    uint32_t    storageRegStack = 0;
    uint32_t    storageRegBits  = 0;
    CPURegister storageReg      = RAX;
    CPURegister storageMemReg   = RAX;
};
