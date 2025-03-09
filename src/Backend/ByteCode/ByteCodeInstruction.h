#pragma once
#include "Backend/ByteCode/ByteCodeOp.h"
#include "Backend/ByteCode/Register.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct ByteCode;
struct SourceLocation;

using InstructionFlags                        = Flags<uint16_t>;
constexpr InstructionFlags BCI_JUMP_DEST      = 0x0001;
constexpr InstructionFlags BCI_SAFETY         = 0x0002;
constexpr InstructionFlags BCI_IMM_A          = 0x0004;
constexpr InstructionFlags BCI_IMM_B          = 0x0008;
constexpr InstructionFlags BCI_IMM_C          = 0x0010;
constexpr InstructionFlags BCI_IMM_D          = 0x0020;
constexpr InstructionFlags BCI_VARIADIC       = 0x0040;
constexpr InstructionFlags BCI_POST_COPY_MOVE = 0x0080;
constexpr InstructionFlags BCI_NOT_NULL       = 0x0100;
constexpr InstructionFlags BCI_TRY_CATCH      = 0x0200;
constexpr InstructionFlags BCI_CAN_OVERFLOW   = 0x0400;
constexpr InstructionFlags BCI_START_STMT_N   = 0x0800;
constexpr InstructionFlags BCI_START_STMT_S   = 0x1000;
constexpr InstructionFlags BCI_START_STMT     = BCI_START_STMT_N | BCI_START_STMT_S;
constexpr InstructionFlags BCI_NO_BACKEND     = 0x2000;
constexpr InstructionFlags BCI_CANT_OVERFLOW  = 0x4000;

using InstructionDynFlags                    = Flags<uint8_t>;
constexpr InstructionDynFlags BCID_OPT_FLAG  = 0x01;
constexpr InstructionDynFlags BCID_SAN_PASS  = 0x02;
constexpr InstructionDynFlags BCID_SAFETY_OF = 0x04;
constexpr InstructionDynFlags BCID_SWAP      = 0x08;

struct ByteCodeInstruction
{
    SWAG_FORCE_INLINE bool hasFlag(InstructionFlags fl) const { return flags.has(fl); }
    void                   addFlag(InstructionFlags fl) { flags.add(fl); }
    void                   removeFlag(InstructionFlags fl) { flags.remove(fl); }
    void                   inheritFlag(const ByteCodeInstruction* ip, InstructionFlags fl) { flags.add(ip->flags.mask(fl)); }

    SWAG_FORCE_INLINE bool hasDynFlag(InstructionDynFlags fl) const { return dynFlags.has(fl); }
    SWAG_FORCE_INLINE void addDynFlag(InstructionDynFlags fl) { dynFlags.add(fl); }

    SWAG_FORCE_INLINE bool hasOpFlag(OpFlags fl) const { return g_ByteCodeOpDesc[static_cast<int>(op)].flags.has(fl); }

    bool isMemCpy() const { return hasOpFlag(OPF_MEMCPY); }
    bool isPushParam() const { return hasOpFlag(OPF_PUSH_PARAM); }
    bool isCall() const { return hasOpFlag(OPF_CALL); }
    bool isLocalCall() const { return hasOpFlag(OPF_LOCAL_CALL); }
    bool isLambdaCall() const { return hasOpFlag(OPF_LAMBDA_CALL); }
    bool isForeignCall() const { return hasOpFlag(OPF_FOREIGN_CALL); }
    bool isJump() const { return hasOpFlag(OPF_JUMP); }
    bool isJumpDyn() const { return hasOpFlag(OPF_JUMP_DYN); }
    bool isJumpOrDyn() const { return hasOpFlag(OPF_JUMP | OPF_JUMP_DYN); }
    bool isRet() const { return op == ByteCodeOp::Ret || op == ByteCodeOp::CopyRAtoRRRet || op == ByteCodeOp::InternalUnreachable || op == ByteCodeOp::Unreachable; }

    bool hasRefToRegA(uint32_t reg) const { return a.u32 == reg && !hasFlag(BCI_IMM_A) && hasOpFlag(OPF_READ_A | OPF_WRITE_A); }
    bool hasRefToRegB(uint32_t reg) const { return b.u32 == reg && !hasFlag(BCI_IMM_B) && hasOpFlag(OPF_READ_B | OPF_WRITE_B); }
    bool hasRefToRegC(uint32_t reg) const { return c.u32 == reg && !hasFlag(BCI_IMM_C) && hasOpFlag(OPF_READ_C | OPF_WRITE_C); }
    bool hasRefToRegD(uint32_t reg) const { return d.u32 == reg && !hasFlag(BCI_IMM_D) && hasOpFlag(OPF_READ_D | OPF_WRITE_D); }
    bool hasRefToReg(uint32_t reg) const { return hasRefToRegA(reg) || hasRefToRegB(reg) || hasRefToRegC(reg) || hasRefToRegD(reg); }

    bool hasWriteRefToRegA(uint32_t reg) const { return a.u32 == reg && !hasFlag(BCI_IMM_A) && hasOpFlag(OPF_WRITE_A); }
    bool hasWriteRefToRegB(uint32_t reg) const { return b.u32 == reg && !hasFlag(BCI_IMM_B) && hasOpFlag(OPF_WRITE_B); }
    bool hasWriteRefToRegC(uint32_t reg) const { return c.u32 == reg && !hasFlag(BCI_IMM_C) && hasOpFlag(OPF_WRITE_C); }
    bool hasWriteRefToRegD(uint32_t reg) const { return d.u32 == reg && !hasFlag(BCI_IMM_D) && hasOpFlag(OPF_WRITE_D); }
    bool hasWriteRefToReg(uint32_t reg) const { return hasWriteRefToRegA(reg) || hasWriteRefToRegB(reg) || hasWriteRefToRegC(reg) || hasWriteRefToRegD(reg); }

    bool hasReadRefToRegA(uint32_t reg) const { return a.u32 == reg && !hasFlag(BCI_IMM_A) && hasOpFlag(OPF_READ_A); }
    bool hasReadRefToRegB(uint32_t reg) const { return b.u32 == reg && !hasFlag(BCI_IMM_B) && hasOpFlag(OPF_READ_B); }
    bool hasReadRefToRegC(uint32_t reg) const { return c.u32 == reg && !hasFlag(BCI_IMM_C) && hasOpFlag(OPF_READ_C); }
    bool hasReadRefToRegD(uint32_t reg) const { return d.u32 == reg && !hasFlag(BCI_IMM_D) && hasOpFlag(OPF_READ_D); }
    bool hasReadRefToReg(uint32_t reg) const { return hasReadRefToRegA(reg) || hasReadRefToRegB(reg) || hasReadRefToRegC(reg) || hasReadRefToRegD(reg); }

    bool hasWriteRegInA() const { return !hasFlag(BCI_IMM_A) && hasOpFlag(OPF_WRITE_A); }
    bool hasWriteRegInB() const { return !hasFlag(BCI_IMM_B) && hasOpFlag(OPF_WRITE_B); }
    bool hasWriteRegInC() const { return !hasFlag(BCI_IMM_C) && hasOpFlag(OPF_WRITE_C); }
    bool hasWriteRegInD() const { return !hasFlag(BCI_IMM_D) && hasOpFlag(OPF_WRITE_D); }

    bool hasReadRegInA() const { return !hasFlag(BCI_IMM_A) && hasOpFlag(OPF_READ_A); }
    bool hasReadRegInB() const { return !hasFlag(BCI_IMM_B) && hasOpFlag(OPF_READ_B); }
    bool hasReadRegInC() const { return !hasFlag(BCI_IMM_C) && hasOpFlag(OPF_READ_C); }
    bool hasReadRegInD() const { return !hasFlag(BCI_IMM_D) && hasOpFlag(OPF_READ_D); }

    bool hasSomethingInA() const { return hasOpFlag(OPF_READ_A | OPF_WRITE_A | OPF_READ_VAL32_A | OPF_READ_VAL64_A); }
    bool hasSomethingInB() const { return hasOpFlag(OPF_READ_B | OPF_WRITE_B | OPF_READ_VAL32_B | OPF_READ_VAL64_B); }
    bool hasSomethingInC() const { return hasOpFlag(OPF_READ_C | OPF_WRITE_C | OPF_READ_VAL32_C | OPF_READ_VAL64_C); }
    bool hasSomethingInD() const { return hasOpFlag(OPF_READ_D | OPF_WRITE_D | OPF_READ_VAL32_D | OPF_READ_VAL64_D); }

    uint32_t countWriteRegs() const { return hasWriteRegInA() + hasWriteRegInB() + hasWriteRegInC() + hasWriteRegInD(); }
    uint32_t countReadRegs() const { return hasReadRegInA() + hasReadRegInB() + hasReadRegInC() + hasReadRegInD(); }

    // Keep 'op' first to dereference it in the runner without an offset
    ByteCodeOp          op;
    InstructionFlags    flags;
    InstructionDynFlags dynFlags;
    uint8_t             numVariadicParams;
    uint16_t            padding0;

    Register a;
    Register b;
    Register c;
    Register d;

    AstNode*        node;
    SourceLocation* location;

#ifdef SWAG_DEV_MODE
    const char* sourceFile;
    uint32_t    sourceLine;
    uint32_t    serial;
    uint32_t    treeNode;
    uint32_t    crc;
#endif
};
