// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/ScbeCpu.h"

enum class ScbeMicroOp : uint8_t
{
#define SCBE_MICRO_OP(__op, ...) __op,
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOpList.h"
};

struct ScbeMicro final : ScbeCpu
{
    void init(const BuildParameters& buildParameters) override;

    void emitEnter(uint32_t sizeStack) override;
    void emitLeave() override;
    void emitDebug(ByteCodeInstruction* ipAddr) override;
    void emitLabel(uint32_t instructionIndex) override;
    void emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits) override;
    void emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc) override;
    void emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx) override;
    void emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits) override;
    void emitSymbolRelocationPtr(CpuReg reg, const Utf8& name) override;
    void emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest) override;

    CpuEncodeResult encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodePush(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodePop(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeNop(CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeRet(CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCallLocal(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCallExtern(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCallReg(CpuReg reg, const CallConv* callConv, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeLoadAddMulCstRegMem(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, uint64_t addValue, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuEncodeResult encodeOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;

    static ScbeMicroInstruction* getNextInstruction(ScbeMicroInstruction* inst);
    static ScbeMicroInstruction* getNextFlagSensitive(ScbeMicroInstruction* inst);
    ScbeMicroInstruction*        getFirstInstruction() const;
    ScbeMicroInstruction*        addInstruction(ScbeMicroOp op, CpuEmitFlags emitFlags);
    void                         postProcess() const;
    void                         solveLabels();
    void                         process(ScbeCpu& encoder);
    void                         encode(ScbeCpu& encoder) const;
    void                         print() const;

    Map<uint32_t, uint32_t> labels;
    bool                    nextIsJumpDest = false;
};
