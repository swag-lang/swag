#include "pch.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::BinOpPlusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::BinOpPlusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::BinOpPlusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::BinOpPlusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::BinOpPlusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::BinOpPlusF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitBinaryOpPlus, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        int  sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::MulRAVB, r1)->b.u32 = sizeOf;
        emitInstruction(context, ByteCodeOp::IncPointer, r0, r1, r2);
        return true;
    }

    return internalError(context, "emitBinaryOpPlus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);

    // This is the substract of two pointers if we have a s64 on the left, and a pointer on the right
    if (typeInfo->isNative(NativeTypeKind::S64))
    {
        auto rightTypeInfo = TypeManager::concreteType(node->childs[1]->typeInfo);
        if (rightTypeInfo->kind == TypeInfoKind::Pointer)
        {
            auto rightTypePointer = CastTypeInfo<TypeInfoPointer>(rightTypeInfo, TypeInfoKind::Pointer);
            emitInstruction(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            auto sizeOf = rightTypePointer->pointedType->sizeOf;
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::DivRAVB, r2)->b.u32 = sizeOf;
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::BinOpMinusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::BinOpMinusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::BinOpMinusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::BinOpMinusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::BinOpMinusF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitBinaryOpMinus, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        int  sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::MulRAVB, r1)->b.s32 = sizeOf;
        emitInstruction(context, ByteCodeOp::DecPointer, r0, r1, r2);
        return true;
    }

    return internalError(context, "emitBinaryOpMinus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpMul, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpMulS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::BinOpMulS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::BinOpMulU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::BinOpMulU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::BinOpMulF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::BinOpMulF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpMul, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpDiv, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpDivS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::BinOpDivS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::BinOpDivU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::BinOpDivU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::BinOpDivF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::BinOpDivF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpDiv, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpModulo(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpModulo, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpModuloS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::BinOpModuloS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::BinOpModuloU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::BinOpModuloU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpModulo, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskAnd, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BitmaskAndS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::BitmaskAndS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::BitmaskAndU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::BitmaskAndU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskAnd, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskOr, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BitmaskOrS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::BitmaskOrS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::BitmaskOrU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::BitmaskOrU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskOr, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftLeft(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftLeft, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::ShiftLeftS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::ShiftLeftS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::ShiftLeftU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::ShiftLeftU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitShiftLeft, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftRight(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftRight, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::ShiftRightS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::ShiftRightS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::ShiftRightU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::ShiftRightU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitShiftRight, type not supported");
    }
}

bool ByteCodeGenJob::emitXor(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitXor, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::XorS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::XorS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::XorU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::XorU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitXor, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->doneFlags & AST_DONE_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST2;
    }

    if (!(node->doneFlags & AST_DONE_EMIT_BINOP))
    {
        auto r0 = node->childs[0]->resultRegisterRC;
        auto r1 = node->childs[1]->resultRegisterRC;

        if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result == ContextResult::Pending)
                return true;
            node->doneFlags |= AST_DONE_EMIT_BINOP;
        }
        else
        {
            auto r2                = reserveRegisterRC(context);
            node->resultRegisterRC = r2;

            switch (node->token.id)
            {
            case TokenId::SymPlus:
                SWAG_CHECK(emitBinaryOpPlus(context, r0, r1, r2));
                break;
            case TokenId::SymMinus:
                SWAG_CHECK(emitBinaryOpMinus(context, r0, r1, r2));
                break;
            case TokenId::SymAsterisk:
                SWAG_CHECK(emitBinaryOpMul(context, r0, r1, r2));
                break;
            case TokenId::SymSlash:
                SWAG_CHECK(emitBinaryOpDiv(context, r0, r1, r2));
                break;
            case TokenId::SymPercent:
                SWAG_CHECK(emitBinaryOpModulo(context, r0, r1, r2));
                break;
            case TokenId::SymAmpersandAmpersand:
                emitInstruction(context, ByteCodeOp::BinOpAnd, r0, r1, r2);
                break;
            case TokenId::SymVerticalVertical:
                emitInstruction(context, ByteCodeOp::BinOpOr, r0, r1, r2);
                break;
            case TokenId::SymVertical:
                SWAG_CHECK(emitBitmaskOr(context, r0, r1, r2));
                break;
            case TokenId::SymAmpersand:
                SWAG_CHECK(emitBitmaskAnd(context, r0, r1, r2));
                break;
            case TokenId::SymLowerLower:
                SWAG_CHECK(emitShiftLeft(context, r0, r1, r2));
                break;
            case TokenId::SymGreaterGreater:
                SWAG_CHECK(emitShiftRight(context, r0, r1, r2));
                break;
            case TokenId::SymCircumflex:
                SWAG_CHECK(emitXor(context, r0, r1, r2));
                break;
            default:
                return internalError(context, "emitBinaryOp, invalid token op");
            }

            freeRegisterRC(context, r0);
            freeRegisterRC(context, r1);
            node->doneFlags |= AST_DONE_EMIT_BINOP;
        }
    }

    if (!(node->doneFlags & AST_DONE_CAST3))
    {
        SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST3;
    }

    return true;
}

bool ByteCodeGenJob::makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(SemanticJob::makeInline((JobContext*) context, funcDecl, identifier));

    // Create a semantic job to resolve the inline part, and wait for that to be finished
    context->job->setPending(nullptr);
    auto inlineNode = identifier->childs.back();
    auto job        = SemanticJob::newJob(context->job->dependentJob, context->sourceFile, inlineNode, false);
    job->addDependentJob(context->job);
    g_ThreadMgr.addJob(job);
    return true;
}

bool ByteCodeGenJob::emitUserOp(ByteCodeGenContext* context, AstNode* allParams, AstNode* forNode)
{
    AstNode* node           = forNode ? forNode : context->node;
    auto     symbolOverload = node->resolvedUserOpSymbolOverload;
    SWAG_ASSERT(symbolOverload);
    auto funcDecl = CastAst<AstFuncDecl>(symbolOverload->node, AstNodeKind::FuncDecl);

    if (symbolOverload->node->attributeFlags & ATTRIBUTE_INLINE)
    {
        // Need to wait for function full semantic resolve
        {
            scoped_lock lk(funcDecl->mutex);
            if (!(funcDecl->flags & AST_FULL_RESOLVE))
            {
                funcDecl->dependentJobs.add(context->job);
                context->job->setPending(funcDecl->resolvedSymbolName);
                return true;
            }
        }

        if (!(node->doneFlags & AST_DONE_INLINED))
        {
            node->doneFlags |= AST_DONE_INLINED;
            SWAG_CHECK(makeInline(context, funcDecl, node));
            return true;
        }

        if (!(node->doneFlags & AST_DONE_RESOLVE_INLINED))
        {
            node->doneFlags |= AST_DONE_RESOLVE_INLINED;
            context->job->nodes.pop_back();
            context->job->nodes.push_back(node->childs.back());
            context->job->nodes.push_back(node);
        }

        return true;
    }

    bool foreign = symbolOverload->node->attributeFlags & ATTRIBUTE_FOREIGN;
    return emitCall(context, allParams ? allParams : node, funcDecl, nullptr, funcDecl->resultRegisterRC, foreign);
}
