#pragma once
#include "SCBE_CPU.h"

enum X64DispMode
{
    DISP8  = 0b01,
    DISP32 = 0b10,
    REGREG = 0b11,
};

struct SCBE_X64 : SCBE_CPU
{
    void emit_Symbol_RelocationRef(const Utf8& name);
    void emit_Symbol_RelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emit_Symbol_RelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emit_Symbol_GlobalString(CPURegister reg, const Utf8& str);

    void emit_Push(CPURegister reg);
    void emit_Pop(CPURegister reg);

    void emit_Call(const Utf8& symbolName);
    void emit_Call_Far(const Utf8& symbolName);
    void emit_Call_Indirect(CPURegister reg);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr);
    void emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);
    void emit_Ret();

    void emit_ClearN(CPURegister reg, CPUBits numBits);
    void emit_ClearX(uint32_t count, uint32_t offset, CPURegister reg);

    void emit_CmpN(CPURegister regSrc, CPURegister regDst, CPUBits numBits);
    void emit_CmpF32(CPURegister regSrc, CPURegister regDst);
    void emit_CmpF64(CPURegister regSrc, CPURegister regDst);

    void emit_CmpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUBits numBits);
    void emit_CmpF32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emit_CmpF64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);

    void emit_CmpN_Immediate(CPURegister reg, uint64_t value, CPUBits numBits);

    void emit_CmpN_IndirectDst(uint32_t offsetStack, uint32_t value, CPUBits numBits);

    void emit_CopyN(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emit_CopyF32(CPURegister regDst, CPURegister regSrc);
    void emit_CopyF64(CPURegister regDst, CPURegister regSrc);
    void emit_CopyX(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset);
    void emit_CopyDownUp(CPURegister reg, CPUBits numBits);

    void emit_JumpTable(CPURegister table, CPURegister offset);
    void emit_LongJumpOp(CPUJumpType jumpType);
    void emit_NearJumpOp(CPUJumpType jumpType);
    void emit_Jump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset);
    void emit_Jump(CPURegister reg);

    void emit_Extend_U16U64(CPURegister regDst, CPURegister regSrc);
    void emit_Extend_U8U64(CPURegister regDst, CPURegister regSrc);

    void emit_Load8_Immediate(CPURegister reg, uint8_t value);
    void emit_Load16_Immediate(CPURegister reg, uint16_t value);
    void emit_Load32_Immediate(CPURegister reg, uint32_t value);
    void emit_Load64_Immediate(CPURegister reg, uint64_t value, bool force64bits = false);
    void emit_LoadN_Immediate(CPURegister reg, uint64_t value, CPUBits numBits);

    void emit_Load8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Load64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_LoadN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);
    void emit_LoadF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_LoadF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);

    void emit_Store8_Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg);
    void emit_Store16_Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg);
    void emit_Store32_Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg);
    void emit_Store64_Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg);

    void emit_Store8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_Store64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emit_StoreN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);

    void emit_LoadAddress_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);

    void emit_LoadS16S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS16S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS32S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadS8S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU16U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU16U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU8U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emit_LoadU8U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);

    void emit_OpN(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits);
    void emit_OpF32(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits = CPUBits::B32);
    void emit_OpF64(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits = CPUBits::B32);

    void emit_OpN(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits);
    void emit_OpF32(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op);
    void emit_OpF64(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op);

    void emit_OpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits, bool lock = false);
    void emit_OpF32_Indirect(CPURegister reg, CPURegister memReg, CPUOp op);
    void emit_OpF64_Indirect(CPURegister reg, CPURegister memReg, CPUOp op);

    void emit_OpN_Immediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emit_OpN_IndirectDst(uint32_t offsetStack, uint64_t value, CPURegister memReg, CPUOp op, CPUBits numBits);
    void emit_OpN_IndirectDst(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits);
    void emit_OpN_IndirectDst(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);

    void emit_SetA(CPURegister reg = RAX);
    void emit_SetAE(CPURegister reg = RAX);
    void emit_SetG(CPURegister reg = RAX);
    void emit_SetB();
    void emit_SetBE();
    void emit_SetE();
    void emit_SetEP();
    void emit_SetGE();
    void emit_SetL();
    void emit_SetLE();
    void emit_SetNA();
    void emit_SetNE();
    void emit_SetNEP();

    void emit_Cwd();
    void emit_Cdq();
    void emit_Cqo();

    void emit_TestN(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emit_NotN(CPURegister reg, CPUBits numBits);
    void emit_NotN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emit_IncN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emit_DecN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emit_NegN(CPURegister reg, CPUBits numBits);
    void emit_NegN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emit_CMovN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emit_CmpXChg(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emit_BSwapN(CPURegister reg, CPUBits numBits);
    void emit_RotateN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emit_Nop();
    void emit_CastU64F64(CPURegister regDst, CPURegister regSrc);
};
