#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitNullConditionalOp(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto child0 = node->childs[0];
    auto child1 = node->childs[1];

    if (!(child0->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, child0, child0->typeInfo, child0->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        child0->doneFlags |= AST_DONE_CAST1;
    }

    if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context, child0, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        emitInstruction(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRC)->b.s32 = child0->resultRegisterRC.size() + 1; // After the "if not null"
        freeRegisterRC(context, node->resultRegisterRC);
        reserveRegisterRC(context, node->resultRegisterRC, child0->resultRegisterRC.size());
    }
    else
    {
        reserveRegisterRC(context, node->resultRegisterRC, child0->resultRegisterRC.size());
        emitInstruction(context, ByteCodeOp::JumpIfZero64, child0->resultRegisterRC)->b.s32 = child0->resultRegisterRC.size() + 1; // After the "if not null"
    }

    // If not null
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[r], child0->resultRegisterRC[r]);
    emitInstruction(context, ByteCodeOp::Jump)->a.s32 = node->resultRegisterRC.size(); // After the if null

    // If null
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[r], child1->resultRegisterRC[r]);

    freeRegisterRC(context, child0);
    freeRegisterRC(context, child1);

    return true;
}

bool ByteCodeGenJob::emitConditionalOp(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto child0 = node->childs[0];
    auto child1 = node->childs[1];
    auto child2 = node->childs[2];

    if (!(child0->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, child0, child0->typeInfo, child0->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        child0->doneFlags |= AST_DONE_CAST1;
    }

    reserveRegisterRC(context, node->resultRegisterRC, child1->resultRegisterRC.size());
    emitInstruction(context, ByteCodeOp::JumpIfNotTrue, child0->resultRegisterRC)->b.s32 = node->resultRegisterRC.size() + 1;

    // If true
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[r], child1->resultRegisterRC[r]);
    emitInstruction(context, ByteCodeOp::Jump)->a.s32 = node->resultRegisterRC.size();

    // If false
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[r], child2->resultRegisterRC[r]);

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
        node->flags |= AST_NO_BYTECODE_CHILDS;

    return true;
}

void ByteCodeGenJob::collectLiteralsChilds(AstNode* node, VectorNative<AstNode*>* orderedChilds)
{
    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
            collectLiteralsChilds(child, orderedChilds);
        else
            orderedChilds->push_back(child);
    }
}

void ByteCodeGenJob::transformResultToLinear2(ByteCodeGenContext* context, AstNode* node)
{
    if (node->resultRegisterRC[1] != node->resultRegisterRC[0] + 1)
    {
        RegisterList r0;
        reserveLinearRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[0], node->resultRegisterRC[0]);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[1], node->resultRegisterRC[1]);
        freeRegisterRC(context, node->resultRegisterRC);
        node->resultRegisterRC = r0;
    }
}

bool ByteCodeGenJob::emitExpressionList(ByteCodeGenContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    // This is a special expression list which initialize the pointer and the count of a slice
    if ((node->flags & AST_SLICE_INIT_EXPRESSION) ||
        (node->parent && node->parent->kind == AstNodeKind::FuncCallParam && (node->parent->flags & AST_SLICE_INIT_EXPRESSION)))
    {
        node->resultRegisterRC = node->childs.front()->resultRegisterRC;
        node->resultRegisterRC += node->childs.back()->resultRegisterRC;
        transformResultToLinear2(context, node);
        return true;
    }

    auto job      = context->job;
    auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    reserveRegisterRC(context, node->resultRegisterRC, 2);

    if (!(node->flags & AST_CONST_EXPR))
    {
        job->collectChilds.clear();
        collectLiteralsChilds(node, &job->collectChilds);

        // Must be called only on the real node !
        auto listNode = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // Emit one affectation per child
        int      offsetIdx = listNode->storageOffset;
        uint32_t oneOffset = typeList->subTypes.front()->typeInfo->sizeOf;
        for (auto child : job->collectChilds)
        {
            emitInstruction(context, ByteCodeOp::MakePointerToStack, node->resultRegisterRC)->b.u32 = offsetIdx;
            emitAffectEqual(context, node->resultRegisterRC, child->resultRegisterRC);
            offsetIdx += oneOffset;
            freeRegisterRC(context, child);
        }

        // Reference to the stack, and store the number of element in a register
        emitInstruction(context, ByteCodeOp::MakePointerToStack, node->resultRegisterRC[0])->b.u32 = listNode->storageOffset;
        emitInstruction(context, ByteCodeOp::CopyVBtoRA32, node->resultRegisterRC[1])->b.u32       = (uint32_t) listNode->childs.size();
    }
    else
    {
        // Emit a reference to the buffer
        auto inst = emitInstruction(context, ByteCodeOp::MakeConstantSegPointerOC, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        SWAG_ASSERT(node->storageOffsetSegment != UINT32_MAX); // Be sure it has been reserved
        inst->c.u64 = ((uint64_t) node->storageOffsetSegment << 32) | (uint32_t) typeList->subTypes.size();
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

    // We have null
    if (node->castedTypeInfo && node->castedTypeInfo == g_TypeMgr.typeInfoNull)
    {
        // We want a string or a slice
        if (node->typeInfo->nativeType == NativeTypeKind::String || node->typeInfo->kind == TypeInfoKind::Slice)
        {
            reserveLinearRegisterRC(context, regList, 2);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[0]);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
            node->castedTypeInfo = nullptr;
            return true;
        }

        // We want an interface
        if (node->typeInfo->kind == TypeInfoKind::Interface)
        {
            reserveLinearRegisterRC(context, regList, 3);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[2]);
            emitInstruction(context, ByteCodeOp::CopyRBAddrToRA, regList[0], regList[1]);
            node->castedTypeInfo = nullptr;
            return true;
        }
    }

    reserveRegisterRC(context, regList, 1);

    if (node->flags & AST_VALUE_IS_TYPEINFO)
    {
        SWAG_ASSERT(node->computedValue.reg.u32 != UINT32_MAX);
        emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, regList[0])->b.u32 = node->computedValue.reg.u32;
        node->parent->resultRegisterRC                                                  = node->resultRegisterRC;
    }
    else if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.b = node->computedValue.reg.b;
            return true;
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.u8 = node->computedValue.reg.u8;
            return true;
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.u16 = node->computedValue.reg.u16;
            return true;
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA64, regList)->b.u64 = node->computedValue.reg.u64;
            return true;
        case NativeTypeKind::S8:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.s8 = node->computedValue.reg.s8;
            return true;
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.s16 = node->computedValue.reg.s16;
            return true;
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.s32 = node->computedValue.reg.s32;
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA64, regList)->b.s64 = node->computedValue.reg.s64;
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.f32 = node->computedValue.reg.f32;
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA64, regList)->b.f64 = node->computedValue.reg.f64;
            return true;
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeTypeKind::String:
        {
            reserveLinearRegisterRC(context, regList, 2);
            auto offset = context->sourceFile->module->constantSegment.addString(node->computedValue.text);
            SWAG_ASSERT(offset != UINT32_MAX);
            emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, regList[0], offset);
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, regList[1], (uint32_t) node->computedValue.text.length());
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
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        reserveLinearRegisterRC(context, regList, 2);

        auto inst = emitInstruction(context, ByteCodeOp::MakeConstantSegPointerOC, regList[0], regList[1]);
        SWAG_ASSERT(node->resolvedSymbolOverload);
        SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
        uint32_t storageOffset = node->resolvedSymbolOverload->storageOffset;
        inst->c.u64            = ((uint64_t) storageOffset << 32) | (uint32_t) typeArray->count;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto inst = emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, regList[0]);
        SWAG_ASSERT(node->resolvedSymbolOverload);
        SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
        inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer && node->castedTypeInfo && node->castedTypeInfo->isNative(NativeTypeKind::String))
    {
        auto offset = context->sourceFile->module->constantSegment.addString(node->computedValue.text);
        SWAG_ASSERT(offset != UINT32_MAX);
        emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, regList[0], offset);
    }
    else
    {
        return internalError(context, format("emitLiteral, unsupported type '%s'", typeInfo->name.c_str()).c_str());
    }

    return true;
}
