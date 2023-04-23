#include "pch.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Report.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusS8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusS16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusU8, r0, r1, r2);
            return true;
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusU16, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            EMIT_INST3(context, ByteCodeOp::BinOpPlusF64, r0, r1, r2);
            return true;
        default:
            return Report::internalError(context->node, "emitBinaryOpPlus, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            auto rt = reserveRegisterRC(context);

            // Be sure that the pointer is on the left side, because ptr = 1 + ptr is possible
            if (context->node->childs[0]->typeInfo->isPointer())
            {
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, rt, r1);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
                EMIT_INST3(context, ByteCodeOp::IncPointer64, r0, rt, r2);
            }
            else
            {
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, rt, r0);
                EMIT_INST1(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
                EMIT_INST3(context, ByteCodeOp::IncPointer64, r1, rt, r2);
            }

            freeRegisterRC(context, rt);
        }
        else
        {
            EMIT_INST3(context, ByteCodeOp::IncPointer64, r0, r1, r2);
        }

        return true;
    }

    return Report::internalError(context->node, "emitBinaryOpPlus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(typeInfoExpr);

    // This is the substract of two pointers if we have a s64 on the left, and a pointer on the right
    if (typeInfo->isNative(NativeTypeKind::S64))
    {
        auto rightTypeInfo = TypeManager::concreteType(node->childs[1]->typeInfo);
        if (rightTypeInfo->isPointer())
        {
            auto rightTypePointer = CastTypeInfo<TypeInfoPointer>(rightTypeInfo, TypeInfoKind::Pointer);
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            auto sizeOf = rightTypePointer->pointedType->sizeOf;
            if (sizeOf > 1)
                EMIT_INST1(context, ByteCodeOp::Div64byVB64, r2)->b.u64 = sizeOf;
            return true;
        }
    }

    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusU8, r0, r1, r2);
            return true;
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusU16, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            EMIT_INST3(context, ByteCodeOp::BinOpMinusF64, r0, r1, r2);
            return true;
        default:
            return Report::internalError(context->node, "emitBinaryOpMinus, type not supported");
        }
    }
    else if (typeInfo->isPointer())
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            auto rt = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, rt, r1);
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
            EMIT_INST3(context, ByteCodeOp::DecPointer64, r0, rt, r2);
            freeRegisterRC(context, rt);
        }
        else
        {
            EMIT_INST3(context, ByteCodeOp::DecPointer64, r0, r1, r2);
        }

        return true;
    }

    return Report::internalError(context->node, "emitBinaryOpMinus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    // 'mul' will be done by the parent 'add' (mulAdd)
    if (context->node->specFlags & AstOp::SPECFLAG_FMA)
        return true;

    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBinaryOpMul, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        EMIT_INST3(context, ByteCodeOp::BinOpMulS8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
        EMIT_INST3(context, ByteCodeOp::BinOpMulS16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
        EMIT_INST3(context, ByteCodeOp::BinOpMulS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        EMIT_INST3(context, ByteCodeOp::BinOpMulS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U8:
        EMIT_INST3(context, ByteCodeOp::BinOpMulU8, r0, r1, r2);
        return true;
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpMulU16, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpMulU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpMulU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        EMIT_INST3(context, ByteCodeOp::BinOpMulF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        EMIT_INST3(context, ByteCodeOp::BinOpMulF64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitBinaryOpMul, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBinaryOpDiv, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitSafetyDivZero(context, r1, 8);
        EMIT_INST3(context, ByteCodeOp::BinOpDivS8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
        emitSafetyDivZero(context, r1, 16);
        EMIT_INST3(context, ByteCodeOp::BinOpDivS16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        EMIT_INST3(context, ByteCodeOp::BinOpDivS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitSafetyDivZero(context, r1, 64);
        EMIT_INST3(context, ByteCodeOp::BinOpDivS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U8:
        emitSafetyDivZero(context, r1, 8);
        EMIT_INST3(context, ByteCodeOp::BinOpDivU8, r0, r1, r2);
        return true;
    case NativeTypeKind::U16:
        emitSafetyDivZero(context, r1, 16);
        EMIT_INST3(context, ByteCodeOp::BinOpDivU16, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitSafetyDivZero(context, r1, 32);
        EMIT_INST3(context, ByteCodeOp::BinOpDivU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitSafetyDivZero(context, r1, 64);
        EMIT_INST3(context, ByteCodeOp::BinOpDivU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        emitSafetyDivZero(context, r1, 32);
        EMIT_INST3(context, ByteCodeOp::BinOpDivF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        emitSafetyDivZero(context, r1, 64);
        EMIT_INST3(context, ByteCodeOp::BinOpDivF64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitBinaryOpDiv, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpModulo(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBinaryOpModulo, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitSafetyDivZero(context, r1, 8);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloS8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
        emitSafetyDivZero(context, r1, 16);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloS16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        emitSafetyDivZero(context, r1, 64);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U8:
        emitSafetyDivZero(context, r1, 8);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloU8, r0, r1, r2);
        return true;
    case NativeTypeKind::U16:
        emitSafetyDivZero(context, r1, 16);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloU16, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitSafetyDivZero(context, r1, 32);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        emitSafetyDivZero(context, r1, 64);
        EMIT_INST3(context, ByteCodeOp::BinOpModuloU64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitBinaryOpModulo, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskAnd(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBitmaskAnd, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitBitmaskAnd, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskOr(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBitmaskOr, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskOr8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskOr16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskOr32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpBitmaskOr64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitBitmaskOr, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftLeft(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitShiftLeft, type not native");

    emitSafetyLeftShift(context, r0, r1, typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftS8, r0, r1, r2);
        return true;
    case NativeTypeKind::U8:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, r2);
        return true;

    case NativeTypeKind::S16:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftS16, r0, r1, r2);
        return true;
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, r2);
        return true;

    case NativeTypeKind::S32:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftS32, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, r2);
        return true;

    case NativeTypeKind::S64:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitShiftLeft, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftRight(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitShiftRight, type not native");

    emitSafetyRightShift(context, r0, r1, typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightS8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightS16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightS64, r0, r1, r2);
        return true;

    case NativeTypeKind::U8:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightU8, r0, r1, r2);
        return true;
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightU16, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpShiftRightU64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitShiftRight, type not supported");
    }
}

bool ByteCodeGenJob::emitXor(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitXor, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        EMIT_INST3(context, ByteCodeOp::BinOpXorU8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        EMIT_INST3(context, ByteCodeOp::BinOpXorU16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        EMIT_INST3(context, ByteCodeOp::BinOpXorU32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        EMIT_INST3(context, ByteCodeOp::BinOpXorU64, r0, r1, r2);
        return true;
    default:
        return Report::internalError(context->node, "emitXor, type not supported");
    }
}

bool ByteCodeGenJob::emitLogicalAndAfterLeft(ByteCodeGenContext* context)
{
    auto left    = context->node;
    auto binNode = CastAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->castedTypeInfo));
    if (context->result != ContextResult::Done)
        return true;
    binNode->semFlags |= SEMFLAG_CAST1;

    left->allocateExtension(ExtensionKind::Misc);

    // Can have already been allocated by another 'and' or 'or' above in case of multiple tests
    if (left->extMisc()->additionalRegisterRC.size() == 0)
    {
        // The result register will be stored in additionalRegisterRC of the left expresion and retreived
        // when evaluating the binary expression
        // :BinOpAndOr
        left->extMisc()->additionalRegisterRC = left->resultRegisterRC;
        left->resultRegisterRC.cannotFree     = true;

        // We try to share the result register with other 'and'/'or' to give optimization opportunities when we
        // have more than one test in a row.
        // That way we have multiple jumps with the same register, which can be optimized
        if (binNode->childs.size() == 2)
        {
            auto child1 = binNode->childs[1];
            while (child1->kind == AstNodeKind::BinaryOp && (child1->tokenId == TokenId::KwdAnd || child1->tokenId == TokenId::KwdOr))
            {
                auto child0 = child1->childs[0];
                child0->allocateExtension(ExtensionKind::Misc);
                child0->extMisc()->additionalRegisterRC            = left->extMisc()->additionalRegisterRC;
                child0->extMisc()->additionalRegisterRC.cannotFree = true;
                if (child1->childs.size() != 2)
                    break;
                child1 = child1->childs[1];
            }
        }
    }

    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, left->extMisc()->additionalRegisterRC, left->resultRegisterRC);

    // Short cut. Will just after the right expression in the the left expression is false. A and B => do not evaluate B
    binNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, left->extMisc()->additionalRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLogicalAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node = CastAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);

    // Because of the shortcuts, there's no need to actually do a 'and' here, as we are sure that the
    // expression on the left is true because of the shortcut
    // Se we just need to propagate the result
    if (r2 != r1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1);

    // And we update the shortcut jump after the 'right' side of the expression
    auto inst   = &context->bc->out[node->seekJumpExpression];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpExpression - 1;
    return true;
}

bool ByteCodeGenJob::emitLogicalOrAfterLeft(ByteCodeGenContext* context)
{
    auto left    = context->node;
    auto binNode = CastAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->castedTypeInfo));
    if (context->result != ContextResult::Done)
        return true;
    binNode->semFlags |= SEMFLAG_CAST1;

    // See the 'and' version for comments
    left->allocateExtension(ExtensionKind::Misc);
    if (left->extMisc()->additionalRegisterRC.size() == 0)
    {
        left->extMisc()->additionalRegisterRC = left->resultRegisterRC;
        left->resultRegisterRC.cannotFree     = true;

        if (binNode->childs.size() == 2)
        {
            auto child1 = binNode->childs[1];
            while (child1->kind == AstNodeKind::BinaryOp && (child1->tokenId == TokenId::KwdAnd || child1->tokenId == TokenId::KwdOr))
            {
                auto child0 = child1->childs[0];
                child0->allocateExtension(ExtensionKind::Misc);
                child0->extMisc()->additionalRegisterRC            = left->extMisc()->additionalRegisterRC;
                child0->extMisc()->additionalRegisterRC.cannotFree = true;
                if (child1->childs.size() != 2)
                    break;
                child1 = child1->childs[1];
            }
        }
    }

    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, left->extMisc()->additionalRegisterRC, left->resultRegisterRC);

    // Short cut. Will just after the right expression in the the left expression is true. A or B => do not evaluate B
    binNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfTrue, left->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLogicalOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node = CastAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);
    if (r2 != r1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1);
    auto inst   = &context->bc->out[node->seekJumpExpression];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpExpression - 1;
    return true;
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!(node->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->semFlags |= SEMFLAG_CAST1;
    }

    if (!(node->semFlags & SEMFLAG_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->semFlags |= SEMFLAG_CAST2;
    }

    if (!(node->semFlags & SEMFLAG_EMIT_OP))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result != ContextResult::Done)
                return true;
            node->semFlags |= SEMFLAG_EMIT_OP;
        }
        else if (node->tokenId == TokenId::SymPlus && node->specFlags & AstOp::SPECFLAG_FMA)
        {
            auto front             = node->childs[0];
            auto typeInfo          = TypeManager::concreteType(front->typeInfo);
            node->resultRegisterRC = reserveRegisterRC(context);

            switch (typeInfo->nativeType)
            {
            case NativeTypeKind::F32:
                EMIT_INST4(context, ByteCodeOp::IntrinsicMulAddF32, node->resultRegisterRC, front->childs[0]->resultRegisterRC, front->childs[1]->resultRegisterRC, node->childs[1]->resultRegisterRC);
                break;
            case NativeTypeKind::F64:
                EMIT_INST4(context, ByteCodeOp::IntrinsicMulAddF64, node->resultRegisterRC, front->childs[0]->resultRegisterRC, front->childs[1]->resultRegisterRC, node->childs[1]->resultRegisterRC);
                break;
            default:
                return Report::internalError(context->node, "emitBinaryOpPlus, muladd, type not supported");
            }

            freeRegisterRC(context, front->childs[0]->resultRegisterRC);
            freeRegisterRC(context, front->childs[1]->resultRegisterRC);
            freeRegisterRC(context, node->childs[1]->resultRegisterRC);
            node->semFlags |= SEMFLAG_EMIT_OP;
        }
        else if (node->tokenId == TokenId::SymAsterisk && node->specFlags & AstOp::SPECFLAG_FMA)
        {
            node->semFlags |= SEMFLAG_EMIT_OP;
        }
        else
        {
            auto     r0 = node->childs[0]->resultRegisterRC;
            auto     r1 = node->childs[1]->resultRegisterRC;
            uint32_t r2 = 0;

            // Register for the binary operation has already been allocated in 'additionalRegisterRC' by the left expression in case of a logical test
            // So we take it as the result register.
            if (node->tokenId == TokenId::KwdAnd || node->tokenId == TokenId::KwdOr)
            {
                // :BinOpAndOr
                auto front             = node->childs[0];
                r2                     = front->extMisc()->additionalRegisterRC;
                node->resultRegisterRC = front->extMisc()->additionalRegisterRC;
                front->extMisc()->additionalRegisterRC.clear();
            }
            else
            {
                r2                     = reserveRegisterRC(context);
                node->resultRegisterRC = r2;
            }

            auto typeInfoExpr = node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo;

            switch (node->tokenId)
            {
            case TokenId::SymPlus:
                SWAG_CHECK(emitBinaryOpPlus(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymMinus:
                SWAG_CHECK(emitBinaryOpMinus(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymAsterisk:
                SWAG_CHECK(emitBinaryOpMul(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymSlash:
                SWAG_CHECK(emitBinaryOpDiv(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymPercent:
                SWAG_CHECK(emitBinaryOpModulo(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymVertical:
                SWAG_CHECK(emitBitmaskOr(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymAmpersand:
                SWAG_CHECK(emitBitmaskAnd(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymLowerLower:
                SWAG_CHECK(emitShiftLeft(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymGreaterGreater:
                SWAG_CHECK(emitShiftRight(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymCircumflex:
                SWAG_CHECK(emitXor(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::KwdAnd:
                emitLogicalAnd(context, r0, r1, r2);
                break;
            case TokenId::KwdOr:
                emitLogicalOr(context, r0, r1, r2);
                break;
            default:
                return Report::internalError(context->node, "emitBinaryOp, invalid token op");
            }

            freeRegisterRC(context, r0);
            freeRegisterRC(context, r1);
            node->semFlags |= SEMFLAG_EMIT_OP;
        }
    }

    if (!(node->semFlags & SEMFLAG_CAST3))
    {
        SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->semFlags |= SEMFLAG_CAST3;
    }

    return true;
}

bool ByteCodeGenJob::emitUserOp(ByteCodeGenContext* context, AstNode* allParams, AstNode* forNode, bool freeRegisterParams)
{
    AstNode* node = forNode ? forNode : context->node;
    SWAG_ASSERT(node->extension);
    auto symbolOverload = node->extMisc()->resolvedUserOpSymbolOverload;
    SWAG_ASSERT(symbolOverload);
    auto funcDecl = CastAst<AstFuncDecl>(symbolOverload->node, AstNodeKind::FuncDecl);

    // Note: Do not inline a call when evaluation compile time affectation (SEMFLAG_EXEC_RET_STACK)
    if (funcDecl->mustInline() && !(node->semFlags & SEMFLAG_EXEC_RET_STACK))
    {
        // Expand inline function. Do not expand an inline call inside a function marked as inline.
        // The expansion will be done at the lowest level possible
        // Remember: inline functions are also compiled as non inlined (mostly for compile time execution
        // of calls), and we do not want the call to be inlined twice (one in the normal owner function, and one
        // again after the owner function has been duplicated).
        if (!node->ownerFct || !node->ownerFct->mustInline())
        {
            // Need to wait for function full semantic resolve
            context->job->waitFuncDeclFullResolve(funcDecl);
            if (context->result != ContextResult::Done)
                return true;

            if (!(node->flags & AST_INLINED))
            {
                node->flags |= AST_INLINED;
                SWAG_CHECK(makeInline(context, funcDecl, node));
                return true;
            }

            if (!(node->semFlags & SEMFLAG_RESOLVE_INLINED))
            {
                node->semFlags |= SEMFLAG_RESOLVE_INLINED;
                auto back = node->childs.back();
                SWAG_ASSERT(back->kind == AstNodeKind::Inline);
                context->job->nodes.push_back(back);
                context->result = ContextResult::NewChilds;
            }

            return true;
        }
    }

    bool foreign = funcDecl->isForeign();

    // We are less restrictive on type parameters for useop, as type are more in control.
    // Se we could have a needed cast now.
    if (allParams)
    {
        for (auto c : allParams->childs)
        {
            SWAG_CHECK(emitCast(context, c, c->typeInfo, c->castedTypeInfo));
            SWAG_ASSERT(context->result == ContextResult::Done);
        }
    }

    return emitCall(context, allParams ? allParams : node, funcDecl, nullptr, funcDecl->resultRegisterRC, foreign, false, freeRegisterParams);
}
