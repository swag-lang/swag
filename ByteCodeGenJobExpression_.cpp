#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitTrinaryOp(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto child0 = node->childs[0];
    auto child1 = node->childs[1];
    auto child2 = node->childs[2];

    reserveRegisterRC(context, node->resultRegisterRC, child1->resultRegisterRC.size());

    emitInstruction(context, ByteCodeOp::JumpNotTrue, child0->resultRegisterRC)->b.s32 = node->resultRegisterRC.size() + 1;

    // If true
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRARB, node->resultRegisterRC[r], child1->resultRegisterRC[r]);
    emitInstruction(context, ByteCodeOp::Jump)->a.s32 = node->resultRegisterRC.size();

    // If false
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRARB, node->resultRegisterRC[r], child2->resultRegisterRC[r]);

    freeRegisterRC(context, child0);
    freeRegisterRC(context, child1);
    freeRegisterRC(context, child2);

    return true;
}

bool ByteCodeGenJob::emitExpressionListBefore(ByteCodeGenContext* context)
{
    auto node = context->node;

    // Do not generate bytecode for childs in case of a constant expression, because
    // the full content of the expression is in the constant segment
    if (node->flags & AST_CONST_EXPR)
    {
        node->flags |= AST_NO_BYTECODE_CHILDS;
    }

    return true;
}

void ByteCodeGenJob::collectLiteralsChilds(AstNode* node, vector<AstNode*>* orderedChilds)
{
    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
            collectLiteralsChilds(child, orderedChilds);
        else
            orderedChilds->push_back(child);
    }
}

bool ByteCodeGenJob::emitExpressionList(ByteCodeGenContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    // This is a special expression list which initialize the pointer and the count of a slice
    if (node->flags & AST_SLICE_INIT_EXPRESSION)
    {
        node->resultRegisterRC = node->childs.front()->resultRegisterRC;
        node->resultRegisterRC += node->childs.back()->resultRegisterRC;
        return true;
    }

    auto job      = context->job;
    auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeList);

    reserveRegisterRC(context, node->resultRegisterRC, 2);

    if (!(node->flags & AST_CONST_EXPR))
    {
        job->collectChilds.clear();
        collectLiteralsChilds(node, &job->collectChilds);

        // Must be called only on the real node !
        auto listNode = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // Emit one affectation per child
        int      offsetIdx = listNode->storageOffset;
        uint32_t oneOffset = typeList->childs.front()->sizeOf;
        for (auto child : job->collectChilds)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = offsetIdx;
            emitAffectEqual(context, node->resultRegisterRC, child->resultRegisterRC);
            offsetIdx += oneOffset;
            freeRegisterRC(context, child);
        }

        // Reference to the stack, and store the number of element in a register
        emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC[0])->b.u32 = listNode->storageOffset;
        emitInstruction(context, ByteCodeOp::CopyRAVB32, node->resultRegisterRC[1])->b.u32     = (uint32_t) listNode->childs.size();
    }
    else
    {
        // Emit a reference to the buffer
        auto inst = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        SWAG_ASSERT(node->storageOffsetSegment != UINT32_MAX); // Be sure it has been reserved
        inst->c.u64 = ((uint64_t) node->storageOffsetSegment << 32) | (uint32_t) typeList->childs.size();
    }

    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    auto node = context->node;
    SWAG_CHECK(emitLiteral(context, node, nullptr, node->resultRegisterRC));
    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context, AstNode* node, TypeInfo* toType, RegisterList& regList)
{
    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    // If we need a cast to an any, then first resolve literal with its real type
    if (typeInfo->isNative(NativeTypeKind::Any))
    {
        SWAG_ASSERT(node->castedTypeInfo);
        typeInfo = node->castedTypeInfo;
    }

    // We have null, and we want a string
    if (node->typeInfo->nativeType == NativeTypeKind::String && node->castedTypeInfo && node->castedTypeInfo == g_TypeMgr.typeInfoNull)
    {
        reserveLinearRegisterRC(context, regList, 2);
        emitInstruction(context, ByteCodeOp::ClearRA, regList[0]);
        emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
        return true;
    }

    reserveRegisterRC(context, regList, 1);

    if (node->flags & AST_VALUE_IS_TYPEINFO)
    {
        emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, regList[0])->b.u32 = node->computedValue.reg.u32;
        node->parent->resultRegisterRC = node->resultRegisterRC;
    }
    else if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.b = node->computedValue.reg.b;
            return true;
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.u8 = node->computedValue.reg.u8;
            return true;
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.u16 = node->computedValue.reg.u16;
            return true;
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, regList)->b.u64 = node->computedValue.reg.u64;
            return true;
        case NativeTypeKind::S8:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.s8 = node->computedValue.reg.s8;
            return true;
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.s16 = node->computedValue.reg.s16;
            return true;
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.s32 = node->computedValue.reg.s32;
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, regList)->b.s64 = node->computedValue.reg.s64;
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.f32 = node->computedValue.reg.f32;
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, regList)->b.f64 = node->computedValue.reg.f64;
            return true;
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeTypeKind::String:
        {
            reserveLinearRegisterRC(context, regList, 2);
            auto index  = context->sourceFile->module->reserveString(node->computedValue.text);
            auto inst   = emitInstruction(context, ByteCodeOp::CopyRARBStr, regList[0], regList[1]);
            inst->c.u32 = index;
            return true;
        }
        default:
            return internalError(context, "emitLiteral, type not supported");
        }
    }
    else if (typeInfo == g_TypeMgr.typeInfoNull)
    {
        if (toType && (toType->kind == TypeInfoKind::Slice || toType->isNative(NativeTypeKind::String)))
        {
            reserveLinearRegisterRC(context, regList, 2);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[0]);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::ClearRA, regList);
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto     typeArray     = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        uint32_t storageOffset = node->computedValue.reg.u32;
        reserveLinearRegisterRC(context, regList, 2);
        auto inst   = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, regList[0], regList[1]);
        inst->c.u64 = ((uint64_t) storageOffset << 32) | (uint32_t) typeArray->count;
    }
    else
    {
        return internalError(context, "emitLiteral, type not native");
    }

    return true;
}
