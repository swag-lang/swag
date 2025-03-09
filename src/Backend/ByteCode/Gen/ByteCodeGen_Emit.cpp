#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool ByteCodeGen::emitPassThrough(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (node->children.empty())
        return true;

    const auto child = node->lastChild();
    if (node->hasAstFlag(AST_DISCARD))
        freeRegisterRC(context, child);
    else
    {
        SWAG_CHECK(emitCast(context, child, TypeManager::concreteType(child->typeInfo), child->typeInfoCast));
        node->resultRegisterRc = child->resultRegisterRc;
    }

    if (child->hasExtMisc())
        freeRegisterRC(context, child->extMisc()->additionalRegisterRC);

    return true;
}

bool ByteCodeGen::emitUnreachable(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitSafetyUnreachable(context));
    return true;
}

void ByteCodeGen::emitDebugLine(ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    PushLocation lk(context, &node->token.endLocation);
    emitDebugLine(context);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ByteCodeGen::emitDebugLine(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.backendDebugInfos &&
        context->sourceFile->module->buildCfg.byteCodeOptimizeLevel <= BuildCfgByteCodeOptim::O1 &&
        !context->sourceFile->hasFlag(FILE_SCRIPT))
    {
        EMIT_INST0(context, ByteCodeOp::DebugNop);
    }
}

bool ByteCodeGen::emitDebugNop(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (!node->children.empty())
        node->resultRegisterRc = node->lastChild()->resultRegisterRc;
    emitDebugLine(context, node);
    return true;
}

ByteCodeInstruction* ByteCodeGen::emitMakeSegPointer(const ByteCodeGenContext* context, const DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0)
{
    SWAG_ASSERT(storageSegment);
    switch (storageSegment->kind)
    {
        case SegmentKind::Data:
            return EMIT_INST2(context, ByteCodeOp::MakeMutableSegPointer, r0, storageOffset);
        case SegmentKind::Bss:
            return EMIT_INST2(context, ByteCodeOp::MakeBssSegPointer, r0, storageOffset);
        case SegmentKind::Compiler:
            return EMIT_INST2(context, ByteCodeOp::MakeCompilerSegPointer, r0, storageOffset);
        case SegmentKind::Constant:
            return EMIT_INST2(context, ByteCodeOp::MakeConstantSegPointer, r0, storageOffset);
        default:
            SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGen::emitGetFromSeg(const ByteCodeGenContext* context, const DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0, uint32_t opBits)
{
    SWAG_ASSERT(storageSegment);

    if (opBits == 8)
    {
        switch (storageSegment->kind)
        {
            case SegmentKind::Data:
                return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg8, r0, storageOffset);
            case SegmentKind::Bss:
                return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg8, r0, storageOffset);
            case SegmentKind::Compiler:
                return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg8, r0, storageOffset);
            default:
                SWAG_ASSERT(false);
        }
    }
    else if (opBits == 16)
    {
        switch (storageSegment->kind)
        {
            case SegmentKind::Data:
                return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg16, r0, storageOffset);
            case SegmentKind::Bss:
                return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg16, r0, storageOffset);
            case SegmentKind::Compiler:
                return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg16, r0, storageOffset);
            default:
                SWAG_ASSERT(false);
        }
    }
    else if (opBits == 32)
    {
        switch (storageSegment->kind)
        {
            case SegmentKind::Data:
                return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg32, r0, storageOffset);
            case SegmentKind::Bss:
                return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg32, r0, storageOffset);
            case SegmentKind::Compiler:
                return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg32, r0, storageOffset);
            default:
                SWAG_ASSERT(false);
        }
    }
    else if (opBits == 64)
    {
        switch (storageSegment->kind)
        {
            case SegmentKind::Data:
                return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg64, r0, storageOffset);
            case SegmentKind::Bss:
                return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg64, r0, storageOffset);
            case SegmentKind::Compiler:
                return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg64, r0, storageOffset);
            default:
                SWAG_ASSERT(false);
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGen::emitInstruction(const ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const char* file, uint32_t line)
{
    AstNode*   node = context->node;
    const auto bc   = context->bc;

    context->bc->makeRoomForInstructions();

    SWAG_RACE_CONDITION_WRITE(bc->raceCond);
    SWAG_ASSERT(bc->out);
    ByteCodeInstruction& ins = bc->out[bc->numInstructions++];

    ins.op = op;

    ins.a.u64 = r0;
    ins.b.u64 = r1;
    ins.c.u64 = r2;
    ins.d.u64 = r3;

    ins.flags    = context->instructionsFlags;
    ins.dynFlags = 0;

    ins.node = context->forceNode ? context->forceNode : node;

    if (context->tryCatchScope)
        ins.addFlag(BCI_TRY_CATCH);
    if (ins.node && ins.node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON))
        ins.addFlag(BCI_CAN_OVERFLOW);

#if defined SWAG_DEV_MODE
    ins.sourceFile            = file;
    ins.sourceLine            = line;
    static std::atomic serial = 0;
    ins.serial                = serial++;
    ins.treeNode              = 0;
    ins.crc                   = 0;
#endif

    if (context->noLocation)
        ins.location = nullptr;
    else if (context->forceLocation)
        ins.location = context->forceLocation;
    else
        ins.location = &node->token.startLocation;

    if (ins.isJumpOrDyn())
        bc->numJumps++;
    if (op == ByteCodeOp::DebugNop)
        bc->numDebugNops++;

    // Some operations, like IntrinsicTypeCmp for example, are in fact call to runtime functions.
    // We must be sure that we have enough room on the stack to store the parameters (x64 backend).
    // 5 is the maximum registers needed for a given op call (4 registers + 1 return).
    context->bc->maxCallParams = std::max(context->bc->maxCallParams, static_cast<uint32_t>(5));
    return &ins;
}
