// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/Backend.h"
#include "Backend/CallConv.h"
#include "Backend/SCBE/Debug/SCBEDebug.h"
#include "Core/Concat.h"
#include "Semantic/DataSegment.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;

#define REG_OFFSET(__r) ((__r) * sizeof(Register))

enum class CPUBits : uint32_t
{
    INVALID = 0,
    B8      = 8,
    B16     = 16,
    B32     = 32,
    B64     = 64,
    F32,
    F64,
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
    AND    = 0x21,
    BSF    = 0xBC,
    BSR    = 0xBD,
    BT     = 0xBA,
    CDQ    = 0x99,
    CMOVB  = 0x42,
    CMOVBE = 0x46,
    CMOVE  = 0x44,
    CMOVG  = 0x4F,
    CMOVGE = 0x4D,
    CMOVL  = 0x4C,
    CMP    = 0x39,
    CVTF2F = 0x5A,
    CVTF2I = 0x2C,
    CVTI2F = 0x2A,
    DIV    = 0xF1,
    FADD   = 0x58,
    FAND   = 0x54,
    FDIV   = 0x5E,
    FMAX   = 0x5F,
    FMIN   = 0x5D,
    FMUL   = 0x59,
    FSQRT  = 0x51,
    FSUB   = 0x5C,
    FXOR   = 0x57,
    IDIV   = 0xF9,
    IMOD   = IDIV | 2,
    IMUL   = 0xC1,
    MOD    = DIV | 2,
    MUL    = 0xC0,
    OR     = 0x09,
    POPCNT = 0xB8,
    ROL    = 0xC0,
    ROR    = 0xC8,
    SAL    = 0xF0,
    SAR    = 0xF8,
    SHL    = 0xE0,
    SHR    = 0xE8,
    SUB    = 0x29,
    TEST   = 0x85,
    UCOMIF = 0x2E,
    XCHG   = 0x87,
    XOR    = 0x31,
};

enum class CPUSet
{
    SetA,
    SetAE,
    SetG,
    SetB,
    SetBE,
    SetE,
    SetEP,
    SetGE,
    SetL,
    SetLE,
    SetNA,
    SetNE,
    SetNEP,
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

struct CPULabelToSolve
{
    uint32_t ipDest;
    int32_t  currentOffset;
    uint8_t* patch;
};

enum class CPUSymbolKind
{
    Function,
    Extern,
    Custom,
    GlobalString,
};

struct CPUSymbol
{
    Utf8          name;
    CPUSymbolKind kind;
    uint32_t      value;
    uint32_t      index;
    uint16_t      sectionIdx;
};

struct CPURelocation
{
    uint32_t virtualAddress;
    uint32_t symbolIndex;
    uint16_t type;
};

struct CPURelocationTable
{
    Vector<CPURelocation> table;
};

struct CPUFunction
{
    VectorNative<uint16_t> unwind;
    Vector<SCBEDebugLines> dbgLines;
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

struct SCBE_CPU : BackendEncoder
{
    void             clearInstructionCache();
    CPUSymbol*       getSymbol(const Utf8& name);
    CPUSymbol*       getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);
    uint32_t         getOrCreateLabel(uint32_t ip);
    CPUSymbol*       getOrCreateGlobalString(const Utf8& str);
    void             addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type);
    CPUFunction*     registerFunction(AstNode* node, uint32_t symbolIndex);
    static uint32_t  getParamStackOffset(const CPUFunction* cpuFct, uint32_t paramIdx);
    static uint32_t  countBits(CPUBits numBits);
    static CPUBits   getCPUBits(ByteCodeOp op);
    static TypeInfo* getCPUType(ByteCodeOp op);

    static bool isInt(CPUBits numBits) { return numBits == CPUBits::B8 || numBits == CPUBits::B16 || numBits == CPUBits::B32 || numBits == CPUBits::B64; }
    static bool isFloat(CPUBits numBits) { return numBits == CPUBits::F32 || numBits == CPUBits::F64; }

    Concat concat;
    Concat postConcat;

    VectorNative<const Utf8*>     stringTable;
    VectorNative<CPUPushParam>    pushParams;
    VectorNative<CPUPushParam>    pushParams2;
    VectorNative<CPUPushParam>    pushParams3;
    VectorNative<TypeInfo*>       pushParamsTypes;
    CPURelocationTable            relocTableTextSection;
    CPURelocationTable            relocTableCSSection;
    CPURelocationTable            relocTableMSSection;
    CPURelocationTable            relocTableTSSection;
    CPURelocationTable            relocTableTLSSection;
    CPURelocationTable            relocTablePDSection;
    CPURelocationTable            relocTableDBGSSection;
    Vector<CPUSymbol>             allSymbols;
    MapUtf8<uint32_t>             mapSymbols;
    MapUtf8<uint32_t>             globalStrings;
    Map<uint32_t, int32_t>        labels;
    DataSegment                   globalSegment;
    DataSegment                   stringSegment;
    VectorNative<CPULabelToSolve> labelsToSolve;
    Utf8                          directives;
    Vector<CPUFunction>           functions;

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

    // Debug infos
    static constexpr int               MAX_RECORD   = 4;
    uint16_t                           dbgRecordIdx = 0;
    uint16_t*                          dbgStartRecordPtr[MAX_RECORD];
    uint32_t                           dbgStartRecordOffset[MAX_RECORD];
    uint32_t                           dbgTypeRecordsCount = 0;
    Concat                             dbgTypeRecords;
    Map<TypeInfo*, SCBEDebugTypeIndex> dbgMapTypes;
    MapUtf8<SCBEDebugTypeIndex>        dbgMapTypesNames;

    uint32_t storageRegCount = UINT32_MAX;
    uint32_t storageRegStack = 0;
    uint32_t storageRegBits  = 0;
    CPUReg   storageReg      = CPUReg::RAX;
    CPUReg   storageMemReg   = CPUReg::RAX;
};
