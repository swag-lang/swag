#pragma once
#include "ByteCodeOp.h"
#include "Register.h"

enum class ByteCodeOp : uint16_t;
struct AstNode;
struct ByteCode;
struct SourceLocation;

constexpr uint16_t BCI_JUMP_DEST      = 0x0001;
constexpr uint16_t BCI_SAFETY         = 0x0002;
constexpr uint16_t BCI_IMM_A          = 0x0004;
constexpr uint16_t BCI_IMM_B          = 0x0008;
constexpr uint16_t BCI_IMM_C          = 0x0010;
constexpr uint16_t BCI_IMM_D          = 0x0020;
constexpr uint16_t BCI_VARIADIC       = 0x0040;
constexpr uint16_t BCI_POST_COPY_MOVE = 0x0080;
constexpr uint16_t BCI_NOT_PURE       = 0x0100;
constexpr uint16_t BCI_TRY_CATCH      = 0x0200;
constexpr uint16_t BCI_CAN_OVERFLOW   = 0x0400;
constexpr uint16_t BCI_START_STMT_N   = 0x0800;
constexpr uint16_t BCI_START_STMT_S   = 0x1000;
constexpr uint16_t BCI_START_STMT     = BCI_START_STMT_N | BCI_START_STMT_S;
constexpr uint16_t BCI_NO_BACKEND     = 0x2000;
constexpr uint16_t BCI_CANT_OVERFLOW  = 0x4000;

constexpr uint8_t BCID_OPT_FLAG  = 0x01;
constexpr uint8_t BCID_SAN_PASS  = 0x02;
constexpr uint8_t BCID_SAFETY_OF = 0x04;

struct ByteCodeInstruction
{
	// clang-format off
	bool hasFlag(uint16_t fl) const { return flags & fl; }
	void addFlag(uint16_t fl) { flags |= fl; }
	void removeFlag(uint16_t fl) { flags &= ~fl; }
	void inheritFlag(const ByteCodeInstruction* ip, uint16_t fl) { flags |= ip->flags & fl; }
	// clang-format on

	// Keep 'op' first to dereference it in the runner without an offset
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
