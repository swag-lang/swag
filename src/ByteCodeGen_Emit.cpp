#include "pch.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Module.h"
#include "TypeManager.h"

bool ByteCodeGen::emitPassThrough(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (node->childs.empty())
        return true;

    const auto child = node->childs.back();
    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, child);
    else
    {
        SWAG_CHECK(emitCast(context, child, TypeManager::concreteType(child->typeInfo), child->castedTypeInfo));
        node->resultRegisterRC = child->resultRegisterRC;
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

void ByteCodeGen::emitDebugLine(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.backendDebugInformations &&
        context->sourceFile->module->buildCfg.byteCodeOptimizeLevel == 0 &&
        !context->sourceFile->isScriptFile)
    {
        EMIT_INST0(context, ByteCodeOp::DebugNop);
    }
}

bool ByteCodeGen::emitDebugNop(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (!node->childs.empty())
        node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    emitDebugLine(context, node);
    return true;
}

ByteCodeInstruction* ByteCodeGen::emitMakeSegPointer(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0)
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

ByteCodeInstruction* ByteCodeGen::emitGetFromSeg(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0, uint32_t numBits)
{
    SWAG_ASSERT(storageSegment);

    if (numBits == 8)
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
    else if (numBits == 16)
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
    else if (numBits == 32)
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
    else if (numBits == 64)
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

ByteCodeInstruction* ByteCodeGen::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const char* file, uint32_t line)
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
        ins.flags |= BCI_TRYCATCH;
    if (ins.node && ins.node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON)
        ins.flags |= BCI_CAN_OVERFLOW;

#if defined SWAG_DEV_MODE
    ins.sourceFile            = file;
    ins.sourceLine            = line;
    static atomic<int> serial = 0;
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

    if (ByteCode::isJumpOrDyn(&ins))
        bc->numJumps++;

    // Some operations, like IntrinsicTypeCmp for example, are in fact call to runtime functions.
    // We must be sure that we have enough room on the stack to store the parameters (x64 backend).
    // 5 is the maximum registers needed for a given op call (4 registers + 1 return).
    context->bc->maxCallParams = max(context->bc->maxCallParams, 5);
    return &ins;
}
