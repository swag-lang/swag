#pragma once
#include "Register.h"
#include "ByteCodeOp.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct ByteCode;
struct SourceLocation;

const uint16_t BCI_JUMP_DEST     = 0x0001;
const uint16_t BCI_SAFETY        = 0x0002;
const uint16_t BCI_IMM_A         = 0x0004;
const uint16_t BCI_IMM_B         = 0x0008;
const uint16_t BCI_IMM_C         = 0x0010;
const uint16_t BCI_IMM_D         = 0x0020;
const uint16_t BCI_VARIADIC      = 0x0040;
const uint16_t BCI_POST_COPYMOVE = 0x0080;
const uint16_t BCI_UNPURE        = 0x0100;
const uint16_t BCI_TRYCATCH      = 0x0200;
const uint16_t BCI_CAN_OVERFLOW  = 0x0400;
const uint16_t BCI_START_STMT_N  = 0x0800;
const uint16_t BCI_START_STMT_S  = 0x1000;
const uint16_t BCI_START_STMT    = BCI_START_STMT_N | BCI_START_STMT_S;
const uint16_t BCI_NO_BACKEND    = 0x2000;
const uint16_t BCI_CANT_OVERFLOW = 0x4000;

const uint8_t BCID_OPT_FLAG  = 0x01;
const uint8_t BCID_SAN_PASS  = 0x02;
const uint8_t BCID_SAFETY_OF = 0x04;

struct ByteCodeInstruction
{
    // Keep 'op' first to derefence it in the runner without an offset
    ByteCodeOp op;
    uint16_t   flags;
    uint16_t   padding0;
    uint8_t    numVariadicParams;
    uint8_t    dynFlags;

    Register a;
    Register b;
    Register c;
    Register d;

    AstNode*        node;
    SourceLocation* location;

#ifdef SWAG_DEV_MODE
    const char* sourceFile;
    int         sourceLine;
    int         serial;
    int         treeNode;
    uint32_t    crc;
#endif
};
