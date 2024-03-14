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
constexpr InstructionFlags BCI_NOT_PURE       = 0x0100;
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

struct ByteCodeInstruction
{
    bool hasFlag(InstructionFlags fl) const { return flags.has(fl); }
    void addFlag(InstructionFlags fl) { flags.add(fl); }
    void removeFlag(InstructionFlags fl) { flags.remove(fl); }
    void inheritFlag(const ByteCodeInstruction* ip, InstructionFlags fl) { flags.add(ip->flags.mask(fl)); }

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
