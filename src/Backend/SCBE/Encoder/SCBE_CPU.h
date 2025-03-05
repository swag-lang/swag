// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/Backend.h"
#include "Backend/CallConv.h"
#include "Backend/SCBE/Debug/SCBE_Debug.h"
#include "Core/Concat.h"
#include "Semantic/DataSegment.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;

#define REG_OFFSET(__r) ((__r) * sizeof(Register))

enum class CPUPushParamType
{
    SwagRegister,
    SwagRegisterAdd,
    SwagRegisterMul,
    CPURegister,
    Constant,
    Constant64,
    SymbolRelocationValue,
    SymbolRelocationAddress,
    LoadAddress,
    GlobalString,
    Load,
    CaptureContext,
    SwagParamStructValue,
};

struct CPUPushParam
{
    CPUPushParamType type     = CPUPushParamType::SwagRegister;
    CPUReg           baseReg  = CPUReg::RDI;
    uint64_t         value    = 0;
    uint64_t         value2   = 0;
    TypeInfo*        typeInfo = nullptr;
};

enum class CPUOp : uint8_t
{
    ADD     = 0x01,
    OR      = 0x09,
    AND     = 0x21,
    SUB     = 0x29,
    CVTI2F  = 0x2A,
    CVTF2I  = 0x2C,
    UCOMIF  = 0x2E,
    XOR     = 0x31,
    CMP     = 0x39,
    CMOVB   = 0x42,
    CMOVE   = 0x44,
    CMOVBE  = 0x46,
    CMOVL   = 0x4C,
    CMOVGE  = 0x4D,
    CMOVG   = 0x4F,
    FSQRT   = 0x51,
    FAND    = 0x54,
    FXOR    = 0x57,
    FADD    = 0x58,
    FMUL    = 0x59,
    CVTF2F  = 0x5A,
    FSUB    = 0x5C,
    FMIN    = 0x5D,
    FDIV    = 0x5E,
    FMAX    = 0x5F,
    MOVD    = 0x6E,
    TEST    = 0x85,
    XCHG    = 0x87,
    MOV     = 0x89,
    CDQ     = 0x99,
    NEG     = 0x9F,
    BSWAP   = 0xB0,
    POPCNT  = 0xB8,
    BT      = 0xBA,
    BSF     = 0xBC,
    BSR     = 0xBD,
    MUL     = 0xC0,
    IMUL    = 0xC1,
    ROL     = 0xC7,
    ROR     = 0xC8,
    SHL     = 0xE0,
    SHR     = 0xE8,
    SAL     = 0xF0,
    DIV     = 0xF1,
    MOD     = 0xF3,
    NOT     = 0xF7,
    SAR     = 0xF8,
    IDIV    = 0xF9,
    CMPXCHG = 0xFA,
    IMOD    = 0xFB,
};

enum class CPUCondFlag : uint8_t
{
    A,
    O,
    AE,
    G,
    B,
    BE,
    E,
    EP,
    GE,
    L,
    LE,
    NA,
    NE,
    NEP,
};

enum CPUCondJump : uint8_t
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

using CPUEmitFlags                    = Flags<uint8_t>;
constexpr CPUEmitFlags EMITF_Zero     = 0x00000000;
constexpr CPUEmitFlags EMITF_Overflow = 0x00000001;
constexpr CPUEmitFlags EMITF_Lock     = 0x00000002;
constexpr CPUEmitFlags EMITF_B64      = 0x00000004;

struct CPUJump
{
    void*    addr   = nullptr;
    uint64_t offset = 0;
    OpBits   opBits = OpBits::Zero;
};

struct CPULabelToSolve
{
    uint32_t ipDest;
    CPUJump  jump;
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
    Map<uint32_t, int32_t>        labels;
    VectorNative<CPULabelToSolve> labelsToSolve;
    VectorNative<CPUReg>          unwindRegs;
    VectorNative<uint16_t>        unwind;
    Vector<SCBEDebugLines>        dbgLines;

    ByteCode*         bc                      = nullptr;
    AstFuncDecl*      node                    = nullptr;
    TypeInfoFuncAttr* typeFunc                = nullptr;
    const CallConv*   cc                      = nullptr;
    uint32_t          symbolIndex             = 0;
    uint32_t          startAddress            = 0;
    uint32_t          endAddress              = 0;
    uint32_t          xdataOffset             = 0;
    uint32_t          sizeProlog              = 0;
    uint32_t          offsetByteCodeStack     = 0;
    uint32_t          offsetCallerStackParams = 0;
    uint32_t          offsetParamsAsRegisters = 0;
    uint32_t          sizeStackCallParams     = 0;
    uint32_t          frameSize               = 0;
    uint32_t          offsetFLT               = 0;
    uint32_t          offsetRT                = 0;
    uint32_t          offsetResult            = 0;

    uint32_t getStackOffsetParam(uint32_t paramIdx) const;
    uint32_t getStackOffsetCallerParam(uint32_t paramIdx) const;
    uint32_t getStackOffsetBCStack() const;
};

struct SCBE_CPU : BackendEncoder
{
    void init(const BuildParameters& buildParameters) override;

    uint32_t getStackOffsetReg(uint32_t reg) const;
    uint32_t getStackOffsetRT(uint32_t reg) const;
    uint32_t getStackOffsetResult() const;
    uint32_t getStackOffsetFLT() const;

    CPUSymbol*   getOrAddSymbol(const Utf8& name, CPUSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);
    CPUSymbol*   getOrCreateGlobalString(const Utf8& str);
    void         addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type);
    CPUFunction* addFunction(const Utf8& funcName, const CallConv* cc, ByteCode* bc);
    void         endFunction() const;
    bool         isNoOp(uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) const;

    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, const CallConv* callConv);
    void emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& cpuParams, CPUReg memRegResult, uint32_t memOffsetResult, void* resultAddr);
    void emitStoreCallResult(CPUReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc);

    virtual void emitDebug(ByteCodeInstruction* ipAddr);
    virtual void emitEndProlog();
    virtual void emitLoadParam(CPUReg reg, uint32_t paramIdx, OpBits opBits);
    virtual void emitLoadSignedExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    virtual void emitLoadZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    virtual void emitLoadAddressParam(CPUReg reg, uint32_t paramIdx);
    virtual void emitStoreParam(uint32_t paramIdx, CPUReg reg, OpBits opBits);
    virtual void emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx);
    virtual void emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits);
    virtual void emitSymbolRelocationPtr(CPUReg reg, const Utf8& name);
    virtual void emitLabel(uint32_t instructionIndex);
    virtual void emitLabels();
    virtual void emitJump(CPUCondJump jumpType, uint32_t ipDest);

    virtual void    emitSymbolRelocationRef(const Utf8& name)                                                                                     = 0;
    virtual void    emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset)                                                = 0;
    virtual void    emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)                                                  = 0;
    virtual void    emitSymbolGlobalString(CPUReg reg, const Utf8& str)                                                                           = 0;
    virtual void    emitPush(CPUReg reg)                                                                                                          = 0;
    virtual void    emitPop(CPUReg reg)                                                                                                           = 0;
    virtual void    emitNop()                                                                                                                     = 0;
    virtual void    emitRet()                                                                                                                     = 0;
    virtual void    emitCallLocal(const Utf8& symbolName)                                                                                         = 0;
    virtual void    emitCallExtern(const Utf8& symbolName)                                                                                        = 0;
    virtual void    emitCallIndirect(CPUReg reg)                                                                                                  = 0;
    virtual void    emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)                      = 0;
    virtual CPUJump emitJump(CPUCondJump jumpType, OpBits opBits)                                                                                 = 0;
    virtual void    emitPatchJump(const CPUJump& jump)                                                                                            = 0;
    virtual void    emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)                                                                = 0;
    virtual void    emitJump(CPUReg reg)                                                                                                          = 0;
    virtual void    emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)                                                        = 0;
    virtual void    emitLoad(CPUReg reg, uint64_t value, OpBits opBits)                                                                           = 0;
    virtual void    emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits)                                                                         = 0;
    virtual void    emitLoad(CPUReg regDstSrc, OpBits opBits)                                                                                     = 0;
    virtual void    emitLoad(CPUReg reg, uint64_t value)                                                                                          = 0;
    virtual void    emitLoadSignedExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)                                      = 0;
    virtual void    emitLoadSignedExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)                     = 0;
    virtual void    emitLoadZeroExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)                                        = 0;
    virtual void    emitLoadZeroExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)                       = 0;
    virtual void    emitLoadAddress(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)                              = 0;
    virtual void    emitLoadAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset)                                                                = 0;
    virtual void    emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)                                                       = 0;
    virtual void    emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)                                                   = 0;
    virtual void    emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits)                                                                              = 0;
    virtual void    emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)                                                         = 0;
    virtual void    emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)                                                     = 0;
    virtual void    emitCmp(CPUReg reg, uint64_t value, OpBits opBits)                                                                            = 0;
    virtual void    emitSet(CPUReg reg, CPUCondFlag setType)                                                                                      = 0;
    virtual void    emitClear(CPUReg reg, OpBits opBits)                                                                                          = 0;
    virtual void    emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count)                                                                  = 0;
    virtual void    emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count)                                                                        = 0;
    virtual void    emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)                                                       = 0;
    virtual void    emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits)                                                                              = 0;
    virtual void    emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero)                      = 0;
    virtual void    emitOpBinary(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero)  = 0;
    virtual void    emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero)     = 0;
    virtual void    emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero)                        = 0;
    virtual void    emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) = 0;
    virtual void    emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)                                                        = 0;

    Concat concat;
    Concat postConcat;

    VectorNative<const Utf8*>                   stringTable;
    VectorNative<uint32_t>                      pushRAParams;
    VectorNative<std::pair<uint32_t, uint32_t>> pushRVParams;
    VectorNative<CPUPushParam>                  pushParams;
    CPURelocationTable                          relocTableTextSection;
    CPURelocationTable                          relocTableCSSection;
    CPURelocationTable                          relocTableMSSection;
    CPURelocationTable                          relocTableTSSection;
    CPURelocationTable                          relocTableTLSSection;
    CPURelocationTable                          relocTablePDSection;
    CPURelocationTable                          relocTableDBGSSection;
    Vector<CPUSymbol>                           allSymbols;
    MapUtf8<uint32_t>                           mapSymbols;
    MapUtf8<uint32_t>                           globalStrings;
    DataSegment                                 globalSegment;
    DataSegment                                 stringSegment;
    Utf8                                        directives;
    Vector<CPUFunction*>                        functions;

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

    BuildCfgBackendOptim optLevel = BuildCfgBackendOptim::O0;

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
};
