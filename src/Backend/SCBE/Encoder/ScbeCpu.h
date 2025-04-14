// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/Backend.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/CallConv.h"
#include "Backend/RegisterSet.h"
#include "Backend/SCBE/Debug/ScbeDebug.h"
#include "Core/Concat.h"
#include "Semantic/DataSegment.h"

enum class ByteCodeOp : uint16_t;
struct ScbeMicroInstruction;
struct AstNode;

enum class CpuPushParamType
{
    SwagRegister,
    SwagRegisterAdd,
    SwagRegisterMul,
    CpuRegister,
    Constant,
    Constant64,
    SymbolRelocValue,
    SymbolRelocAddr,
    LoadAddress,
    GlobalString,
    Load,
    CaptureContext,
    SwagParamStructValue,
};

struct CpuPushParam
{
    CpuPushParamType type     = CpuPushParamType::SwagRegister;
    CpuReg           baseReg  = CpuReg::Rsp;
    uint64_t         value    = 0;
    uint64_t         value2   = 0;
    TypeInfo*        typeInfo = nullptr;
};

enum class CpuOp : uint8_t
{
    ADD      = 0x01,
    OR       = 0x09,
    AND      = 0x21,
    SUB      = 0x29,
    CVTI2F   = 0x2A,
    CVTU2F64 = 0x2B,
    CVTF2I   = 0x2C,
    XOR      = 0x31,
    FSQRT    = 0x51,
    FAND     = 0x54,
    FXOR     = 0x57,
    FADD     = 0x58,
    FMUL     = 0x59,
    CVTF2F   = 0x5A,
    FSUB     = 0x5C,
    FMIN     = 0x5D,
    FDIV     = 0x5E,
    FMAX     = 0x5F,
    MOVSXD   = 0x63,
    XCHG     = 0x87,
    MOV      = 0x8B,
    LEA      = 0x8D,
    NEG      = 0x9F,
    BSWAP    = 0xB0,
    POPCNT   = 0xB8,
    BSF      = 0xBC,
    BSR      = 0xBD,
    MUL      = 0xC0,
    IMUL     = 0xC1,
    ROL      = 0xC7,
    ROR      = 0xC8,
    SHL      = 0xE0,
    SHR      = 0xE8,
    SAL      = 0xF0,
    DIV      = 0xF1,
    MOD      = 0xF3,
    NOT      = 0xF7,
    SAR      = 0xF8,
    IDIV     = 0xF9,
    CMPXCHG  = 0xFA,
    IMOD     = 0xFB,
    MULADD   = 0xFC,
};

enum class CpuCond : uint8_t
{
    A,
    O,
    AE,
    G,
    B,
    BE,
    E,
    GE,
    L,
    LE,
    NA,
    NE,
    P,
    NP,
    EP,
    NEP,
};

enum class CpuCondJump : uint8_t
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
    JS,
    JNP,
    JUMP,
};

using CpuEmitFlags                    = Flags<uint8_t>;
constexpr CpuEmitFlags EMIT_Zero      = 0x00000000;
constexpr CpuEmitFlags EMIT_Overflow  = 0x00000001;
constexpr CpuEmitFlags EMIT_Lock      = 0x00000002;
constexpr CpuEmitFlags EMIT_B64       = 0x00000004;
constexpr CpuEmitFlags EMIT_CanEncode = 0x00000008;

enum class CpuEncodeResult : uint32_t
{
    Zero,
    Left2Reg,
    Left2Rax,
    Right2Reg,
    Right2Rcx,
    Simplify,
    NotSupported,
};

struct CpuJump
{
    void*    patchOffsetAddr = nullptr;
    uint64_t offsetStart     = 0;
    OpBits   opBits          = OpBits::Zero;
};

struct CpuLabelToSolve
{
    uint32_t ipDest;
    CpuJump  jump;
};

enum class CpuSymbolKind
{
    Function,
    Extern,
    Custom,
    GlobalString,
};

struct CpuSymbol
{
    Utf8          name;
    CpuSymbolKind kind;
    uint32_t      value;
    uint32_t      index;
    uint16_t      sectionIdx;
};

struct CpuRelocation
{
    uint32_t virtualAddress;
    uint32_t symbolIndex;
    uint16_t type;
};

struct CpuRelocationTable
{
    Vector<CpuRelocation> table;
};

struct CpuFunction
{
    uint32_t getStackOffsetParam(uint32_t paramIdx) const { return sizeStackCallParams + (paramIdx < cc->paramsRegistersInteger.size() ? offsetParamsAsRegisters : offsetCallerStackParams) + (paramIdx * sizeof(Register)); }
    uint32_t getStackOffsetCallerParam(uint32_t paramIdx) const { return sizeStackCallParams + offsetCallerStackParams + (paramIdx * sizeof(Register)); }
    uint32_t getStackOffsetBC() const { return sizeStackCallParams + offsetByteCodeStack; }
    uint32_t getStackOffsetReg(uint32_t reg) const { return sizeStackCallParams + (reg * sizeof(Register)); }
    uint32_t getStackOffsetRT(uint32_t reg) const { return sizeStackCallParams + offsetRT + (reg * sizeof(Register)); }
    uint32_t getStackOffsetResult() const { return sizeStackCallParams + offsetResult; }
    uint32_t getStackOffsetFLT() const { return sizeStackCallParams + offsetFLT; }
    bool     isStackOffsetBC(uint32_t offset) const { return offset >= getStackOffsetBC() && offset < getStackOffsetBC() + bc->stackSize; }
    bool     isStackOffsetLocalParam(uint32_t offset) const { return !cc->paramsRegistersInteger.empty() && offset >= getStackOffsetParam(0) && offset <= getStackOffsetParam(cc->paramsRegistersInteger.size() - 1); }
    bool     isStackOffsetRT(uint32_t offset) const { return offset >= getStackOffsetRT(0) && offset <= getStackOffsetRT(1); }
    bool     isStackOffsetResult(uint32_t offset) const { return offset == getStackOffsetResult(); }
    bool     isStackOffsetReg(uint32_t offset) const { return offset >= getStackOffsetReg(0) && offset < getStackOffsetReg(bc->maxReservedRegisterRC); }
    bool     isStackOffsetTransient(uint32_t offset) const { return isStackOffsetLocalParam(offset) || isStackOffsetReg(offset) || isStackOffsetRT(offset) || isStackOffsetResult(offset); }

    Map<uint32_t, int32_t>        labels;
    VectorNative<CpuLabelToSolve> labelsToSolve;
    RegisterSet                   usedRegs;
    VectorNative<CpuReg>          unwindRegs;
    VectorNative<uint32_t>        unwindOffsetRegs;
    VectorNative<uint16_t>        unwind;
    Vector<ScbeDebugLines>        dbgLines;

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
    bool              noStackFrame            = false;
};

struct ScbeCpu : BackendEncoder
{
    void init(const BuildParameters& buildParameters) override;

    CpuSymbol*   getOrAddSymbol(const Utf8& name, CpuSymbolKind kind, uint32_t value = 0, uint16_t sectionIdx = 0);
    CpuSymbol*   getOrCreateGlobalString(const Utf8& str);
    void         addSymbolRelocation(uint32_t virtualAddr, uint32_t symbolIndex, uint16_t type);
    CpuFunction* addFunction(const Utf8& funcName, const CallConv* ccFunc, ByteCode* bc);
    void         endFunction() const;

    bool acceptsRegA(const ScbeMicroInstruction* inst, CpuReg reg);
    bool acceptsRegB(const ScbeMicroInstruction* inst, CpuReg reg);
    bool acceptsRegC(const ScbeMicroInstruction* inst, CpuReg reg);

    virtual RegisterSet getReadRegisters(ScbeMicroInstruction* inst);
    virtual RegisterSet getWriteRegisters(ScbeMicroInstruction* inst);
    RegisterSet         getReadWriteRegisters(ScbeMicroInstruction* inst);

    virtual bool doesReadFlags(ScbeMicroInstruction* inst) const;
    virtual bool doesWriteFlags(ScbeMicroInstruction* inst) const;

    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CpuPushParam>& cpuParams, const CallConv* callConv);
    void emitComputeCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CpuPushParam>& cpuParams, CpuReg memRegResult, uint32_t memOffsetResult, void* resultAddr);
    void emitStoreCallResult(CpuReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc);
    void emitLabels();

    void emitSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadSymRelocAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadSymbolRelocValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitPush(CpuReg reg, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitPop(CpuReg reg, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitNop(CpuEmitFlags emitFlags = EMIT_Zero);
    void emitRet(CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCallLocal(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCallExtern(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCallReg(CpuReg reg, const CallConv* callConv, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitPatchJump(const CpuJump& jump, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitPatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitJumpReg(CpuReg reg, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadAmcRegMem(CpuReg regDst, OpBits opBitsDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, OpBits opBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadAmcMemReg(CpuReg regDst1, CpuReg regDst2, uint64_t mulValue, uint64_t addValue, OpBits opBitsDst, CpuReg regSrc, OpBits opBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadAddressAmcRegMem(CpuReg regDst, OpBits opBitsDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, OpBits opBitsSrc, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitSetCondReg(CpuReg reg, CpuCond cpuCond, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitLoadCondRegReg(CpuReg regDst, CpuReg regSrc, CpuCond setType, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);
    void emitOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMIT_Zero);

    virtual void emitEnter(uint32_t sizeStack);
    virtual void emitLeave();
    virtual void emitDebug(ByteCodeInstruction* ipAddr);
    virtual void emitLabel(uint32_t instructionIndex);
    virtual void emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    virtual void emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits);
    virtual void emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx);
    virtual void emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits);
    virtual void emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest);

    virtual CpuEncodeResult encodeLoadSymbolRelocAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)                                                                     = 0;
    virtual CpuEncodeResult encodeLoadSymRelocValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)                                                                          = 0;
    virtual CpuEncodeResult encodePush(CpuReg reg, CpuEmitFlags emitFlags)                                                                                                                              = 0;
    virtual CpuEncodeResult encodePop(CpuReg reg, CpuEmitFlags emitFlags)                                                                                                                               = 0;
    virtual CpuEncodeResult encodeNop(CpuEmitFlags emitFlags)                                                                                                                                           = 0;
    virtual CpuEncodeResult encodeRet(CpuEmitFlags emitFlags)                                                                                                                                           = 0;
    virtual CpuEncodeResult encodeCallLocal(const Utf8& symbolName, const CallConv* cc, CpuEmitFlags emitFlags)                                                                                         = 0;
    virtual CpuEncodeResult encodeCallExtern(const Utf8& symbolName, const CallConv* cc, CpuEmitFlags emitFlags)                                                                                        = 0;
    virtual CpuEncodeResult encodeCallReg(CpuReg reg, const CallConv* cc, CpuEmitFlags emitFlags)                                                                                                       = 0;
    virtual CpuEncodeResult encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags)                                          = 0;
    virtual CpuEncodeResult encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags)                                                                                      = 0;
    virtual CpuEncodeResult encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags)                                                                                                                = 0;
    virtual CpuEncodeResult encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags)                                                                                    = 0;
    virtual CpuEncodeResult encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags)                                                                                                                           = 0;
    virtual CpuEncodeResult encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags)                                                                      = 0;
    virtual CpuEncodeResult encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)                                                                                         = 0;
    virtual CpuEncodeResult encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags)                                                                                       = 0;
    virtual CpuEncodeResult encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)                                                    = 0;
    virtual CpuEncodeResult encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)                                   = 0;
    virtual CpuEncodeResult encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)                                                      = 0;
    virtual CpuEncodeResult encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)                                     = 0;
    virtual CpuEncodeResult encodeLoadAmcRegMem(CpuReg regDst, OpBits opBitsDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, OpBits opBitsSrc, CpuEmitFlags emitFlags)        = 0;
    virtual CpuEncodeResult encodeLoadAmcMemReg(CpuReg regDst1, CpuReg regDst2, uint64_t mulValue, uint64_t addValue, OpBits opBitsDst, CpuReg regSrc, OpBits opBitsSrc, CpuEmitFlags emitFlags)        = 0;
    virtual CpuEncodeResult encodeLoadAddressAmcRegMem(CpuReg regDst, OpBits opBitsDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, OpBits opBitsSrc, CpuEmitFlags emitFlags) = 0;
    virtual CpuEncodeResult encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags)                                                                                 = 0;
    virtual CpuEncodeResult encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)                                                                      = 0;
    virtual CpuEncodeResult encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)                                                                  = 0;
    virtual CpuEncodeResult encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags)                                                                                            = 0;
    virtual CpuEncodeResult encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)                                                                       = 0;
    virtual CpuEncodeResult encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)                                                                   = 0;
    virtual CpuEncodeResult encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)                                                                                          = 0;
    virtual CpuEncodeResult encodeSetCondReg(CpuReg reg, CpuCond setType, CpuEmitFlags emitFlags)                                                                                                       = 0;
    virtual CpuEncodeResult encodeLoadCondRegReg(CpuReg regDst, CpuReg regSrc, CpuCond setType, OpBits opBits, CpuEmitFlags emitFlags)                                                                  = 0;
    virtual CpuEncodeResult encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)                                                                                                           = 0;
    virtual CpuEncodeResult encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                                        = 0;
    virtual CpuEncodeResult encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                                                               = 0;
    virtual CpuEncodeResult encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                                         = 0;
    virtual CpuEncodeResult encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                     = 0;
    virtual CpuEncodeResult encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                        = 0;
    virtual CpuEncodeResult encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                                           = 0;
    virtual CpuEncodeResult encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                    = 0;
    virtual CpuEncodeResult encodeOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)                                                            = 0;

    Concat concat;
    Concat postConcat;

    ScbeCpu*                                    cpu = nullptr;
    VectorNative<const Utf8*>                   stringTable;
    VectorNative<uint32_t>                      pushRAParams;
    VectorNative<std::pair<uint32_t, uint32_t>> pushRVParams;
    VectorNative<CpuPushParam>                  pushParams;
    CpuRelocationTable                          relocTableTextSection;
    CpuRelocationTable                          relocTableCSSection;
    CpuRelocationTable                          relocTableMSSection;
    CpuRelocationTable                          relocTableTSSection;
    CpuRelocationTable                          relocTableTLSSection;
    CpuRelocationTable                          relocTablePDSection;
    CpuRelocationTable                          relocTableDBGSSection;
    Vector<CpuSymbol>                           allSymbols;
    MapUtf8<uint32_t>                           mapSymbols;
    MapUtf8<uint32_t>                           globalStrings;
    DataSegment                                 globalSegment;
    DataSegment                                 stringSegment;
    Utf8                                        directives;
    Vector<CpuFunction*>                        functions;

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
    Map<TypeInfo*, ScbeDebugTypeIndex> dbgMapTypes;
    MapUtf8<ScbeDebugTypeIndex>        dbgMapTypesNames;
};
