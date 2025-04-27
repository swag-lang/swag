#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#pragma optimize("", off)

bool ByteCodeGen::emitBinaryOpPlus(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

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
                EMIT_INST3(context, ByteCodeOp::BinOpPlusF32_Safe, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::BinOpPlusF64_Safe, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitBinaryOpPlus, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        const auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            const auto rt = reserveRegisterRC(context);

            // Be sure that the pointer is on the left side, because ptr = 1 + ptr is possible
            if (context->node->firstChild()->typeInfo->isPointer())
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

bool ByteCodeGen::emitBinaryOpMinus(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const AstNode* node     = context->node;
    const auto     typeInfo = TypeManager::concreteType(typeInfoExpr);

    // This is the difference of two pointers if we have a s64 on the left, and a pointer on the right
    if (typeInfo->isNative(NativeTypeKind::S64))
    {
        const auto rightTypeInfo = TypeManager::concretePtrRefType(node->secondChild()->typeInfo);
        if (rightTypeInfo->isPointer())
        {
            const auto rightTypePointer = castTypeInfo<TypeInfoPointer>(rightTypeInfo, TypeInfoKind::Pointer);
            EMIT_INST3(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            const auto sizeOf = rightTypePointer->pointedType->sizeOf;
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
                EMIT_INST3(context, ByteCodeOp::BinOpMinusF32_Safe, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::BinOpMinusF64_Safe, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitBinaryOpMinus, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        const auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            const auto rt = reserveRegisterRC(context);
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

bool ByteCodeGen::emitBinaryOpMul(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    // 'mul' will be done by the parent 'add' (mulAdd)
    if (context->node->hasSpecFlag(AstOp::SPEC_FLAG_FMA))
        return true;

    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

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
            EMIT_INST3(context, ByteCodeOp::BinOpMulF32_Safe, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            EMIT_INST3(context, ByteCodeOp::BinOpMulF64_Safe, r0, r1, r2);
            return true;
        default:
            return Report::internalError(context->node, "emitBinaryOpMul, type not supported");
    }
}

bool ByteCodeGen::emitBinaryOpDiv(ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitBinaryOpDiv, type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            emitSafetyDivZero(context, r1, 8);
            emitSafetyDivOverflow(context, r0, r1, 8, false);
            EMIT_INST3(context, ByteCodeOp::BinOpDivS8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
            emitSafetyDivZero(context, r1, 16);
            emitSafetyDivOverflow(context, r0, r1, 16, false);
            EMIT_INST3(context, ByteCodeOp::BinOpDivS16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
            emitSafetyDivZero(context, r1, 32);
            emitSafetyDivOverflow(context, r0, r1, 32, false);
            EMIT_INST3(context, ByteCodeOp::BinOpDivS32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            emitSafetyDivZero(context, r1, 64);
            emitSafetyDivOverflow(context, r0, r1, 64, false);
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

bool ByteCodeGen::emitBinaryOpModulo(ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

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

bool ByteCodeGen::emitBitmaskAnd(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

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

bool ByteCodeGen::emitBitmaskOr(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

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

bool ByteCodeGen::emitShiftLeft(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitShiftLeft, type not native");

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

bool ByteCodeGen::emitShiftRight(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitShiftRight, type not native");

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

bool ByteCodeGen::emitXor(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitXor, type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::BinOpXor8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::BinOpXor16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST3(context, ByteCodeOp::BinOpXor32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::BinOpXor64, r0, r1, r2);
            return true;
        default:
            return Report::internalError(context->node, "emitXor, type not supported");
    }
}

bool ByteCodeGen::emitLogicalAndAfterLeft(ByteCodeGenContext* context)
{
    const auto left    = context->node;
    const auto binNode = castAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->typeInfoCast));
    YIELD();
    binNode->addSemFlag(SEMFLAG_CAST1);

    left->allocateExtension(ExtensionKind::Misc);

    // Can have already been allocated by another 'and' or 'or' above in case of multiple tests
    if (left->extMisc()->additionalRegisterRC.size() == 0)
    {
        // The result register will be stored in additionalRegisterRC of the left expression and retrieved
        // when evaluating the binary expression.
        // :BinOpAndOr
        if (left->resultRegisterRc.cannotFree)
            left->extMisc()->additionalRegisterRC = reserveRegisterRC(context);
        else
        {
            left->extMisc()->additionalRegisterRC = left->resultRegisterRc;
            left->resultRegisterRc.cannotFree     = true;
        }

        // We try to share the result register with other 'and'/'or' to give optimization opportunities when we
        // have more than one test in a row.
        // That way we have multiple jumps with the same register, which can be optimized
        if (binNode->childCount() == 2)
        {
            auto child1 = binNode->secondChild();
            while (child1->is(AstNodeKind::BinaryOp) && (child1->token.is(TokenId::KwdAnd) || child1->token.is(TokenId::KwdOr)))
            {
                const auto child0 = child1->firstChild();
                child0->allocateExtension(ExtensionKind::Misc);
                child0->extMisc()->additionalRegisterRC            = left->extMisc()->additionalRegisterRC;
                child0->extMisc()->additionalRegisterRC.cannotFree = true;
                if (child1->childCount() != 2)
                    break;
                child1 = child1->secondChild();
            }
        }
    }

    if (!left->extMisc()->additionalRegisterRC.isSame(left->resultRegisterRc))
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, left->extMisc()->additionalRegisterRC, left->resultRegisterRc);

    // Short cut. Will just after the right expression in the the left expression is false. A and B => do not evaluate B
    binNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, left->extMisc()->additionalRegisterRC);
    return true;
}

bool ByteCodeGen::emitLogicalAnd(const ByteCodeGenContext* context, uint32_t r1, uint32_t r2)
{
    const auto node = castAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);

    // Because of the shortcuts, there's no need to actually do a 'and' here, as we are sure that the
    // expression on the left is true because of the shortcut
    // Se we just need to propagate the result
    if (r2 != r1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1);

    // And we update the shortcut jump after the 'right' side of the expression
    const auto inst = &context->bc->out[node->seekJumpExpression];
    inst->b.s32     = static_cast<int32_t>(context->bc->numInstructions - node->seekJumpExpression - 1);
    return true;
}

bool ByteCodeGen::emitLogicalOrAfterLeft(ByteCodeGenContext* context)
{
    const auto left    = context->node;
    const auto binNode = castAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->typeInfoCast));
    YIELD();
    binNode->addSemFlag(SEMFLAG_CAST1);

    // See the 'and' version for comments
    left->allocateExtension(ExtensionKind::Misc);
    if (left->extMisc()->additionalRegisterRC.size() == 0)
    {
        // :BinOpAndOr
        if (left->resultRegisterRc.cannotFree)
            left->extMisc()->additionalRegisterRC = reserveRegisterRC(context);
        else
        {
            left->extMisc()->additionalRegisterRC = left->resultRegisterRc;
            left->resultRegisterRc.cannotFree     = true;
        }

        if (binNode->childCount() == 2)
        {
            auto child1 = binNode->secondChild();
            while (child1->is(AstNodeKind::BinaryOp) && (child1->token.is(TokenId::KwdAnd) || child1->token.is(TokenId::KwdOr)))
            {
                const auto child0 = child1->firstChild();
                child0->allocateExtension(ExtensionKind::Misc);
                child0->extMisc()->additionalRegisterRC            = left->extMisc()->additionalRegisterRC;
                child0->extMisc()->additionalRegisterRC.cannotFree = true;
                if (child1->childCount() != 2)
                    break;
                child1 = child1->secondChild();
            }
        }
    }

    if (!left->extMisc()->additionalRegisterRC.isSame(left->resultRegisterRc))
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, left->extMisc()->additionalRegisterRC, left->resultRegisterRc);

    // Short cut. Will just after the right expression in the the left expression is true. A or B => do not evaluate B
    binNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfTrue, left->resultRegisterRc);
    return true;
}

bool ByteCodeGen::emitLogicalOr(const ByteCodeGenContext* context, uint32_t r1, uint32_t r2)
{
    const auto node = castAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);
    if (r2 != r1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1);
    const auto inst = &context->bc->out[node->seekJumpExpression];
    inst->b.s32     = static_cast<int32_t>(context->bc->numInstructions - node->seekJumpExpression - 1);
    return true;
}

bool ByteCodeGen::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!node->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->firstChild(), TypeManager::concreteType(node->firstChild()->typeInfo), node->firstChild()->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST1);
    }

    if (!node->hasSemFlag(SEMFLAG_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->secondChild(), TypeManager::concreteType(node->secondChild()->typeInfo), node->secondChild()->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST2);
    }

    if (!node->hasSemFlag(SEMFLAG_EMIT_OP))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            YIELD();
            node->addSemFlag(SEMFLAG_EMIT_OP);
        }
        else if (node->token.is(TokenId::SymPlus) && node->hasSpecFlag(AstOp::SPEC_FLAG_FMA) && node->firstChild()->secondChild())
        {
            const auto front       = node->firstChild();
            const auto typeInfo    = TypeManager::concreteType(front->typeInfo);
            node->resultRegisterRc = reserveRegisterRC(context);

            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::F32:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicMulAddF32, node->resultRegisterRc, front->firstChild()->resultRegisterRc, front->secondChild()->resultRegisterRc, node->secondChild()->resultRegisterRc);
                    break;
                case NativeTypeKind::F64:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicMulAddF64, node->resultRegisterRc, front->firstChild()->resultRegisterRc, front->secondChild()->resultRegisterRc, node->secondChild()->resultRegisterRc);
                    break;
                default:
                    return Report::internalError(context->node, "emitBinaryOpPlus, muladd, type not supported");
            }

            freeRegisterRC(context, front->firstChild()->resultRegisterRc);
            freeRegisterRC(context, front->secondChild()->resultRegisterRc);
            freeRegisterRC(context, node->secondChild()->resultRegisterRc);
            node->addSemFlag(SEMFLAG_EMIT_OP);
        }
        else if (node->token.is(TokenId::SymAsterisk) && node->hasSpecFlag(AstOp::SPEC_FLAG_FMA))
        {
            node->addSemFlag(SEMFLAG_EMIT_OP);
        }
        else
        {
            auto     r0 = node->firstChild()->resultRegisterRc;
            auto     r1 = node->secondChild()->resultRegisterRc;
            uint32_t r2;

            // Register for the binary operation has already been allocated in 'additionalRegisterRC' by the left expression in case of a logical test
            // So we take it as the result register.
            if (node->token.is(TokenId::KwdAnd) || node->token.is(TokenId::KwdOr))
            {
                // :BinOpAndOr
                const auto front       = node->firstChild();
                r2                     = front->extMisc()->additionalRegisterRC;
                node->resultRegisterRc = front->extMisc()->additionalRegisterRC;
                front->extMisc()->additionalRegisterRC.clear();
            }
            else
            {
                r2                     = reserveRegisterRC(context);
                node->resultRegisterRc = r2;
            }

            auto typeInfoExpr = node->typeInfoCast ? node->typeInfoCast : node->typeInfo;
            typeInfoExpr      = TypeManager::concretePtrRefType(typeInfoExpr);

            switch (node->token.id)
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
                    emitLogicalAnd(context, r1, r2);
                    break;
                case TokenId::KwdOr:
                    emitLogicalOr(context, r1, r2);
                    break;
                default:
                    return Report::internalError(context->node, "emitBinaryOp, invalid token op");
            }

            freeRegisterRC(context, r0);
            freeRegisterRC(context, r1);
            node->addSemFlag(SEMFLAG_EMIT_OP);
        }
    }

    if (!node->hasSemFlag(SEMFLAG_CAST3))
    {
        SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST3);
    }

    return true;
}

bool ByteCodeGen::emitUserOp(ByteCodeGenContext* context, AstNode* allParams, AstNode* forNode, bool freeRegisterParams)
{
    AstNode*   node           = forNode ? forNode : context->node;
    const auto symbolOverload = node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
    SWAG_ASSERT(symbolOverload);
    const auto funcDecl = castAst<AstFuncDecl>(symbolOverload->node, AstNodeKind::FuncDecl);

    // Note: Do not inline a call when evaluation compile-time affectation (SEMFLAG_EXEC_RET_STACK)
    if (!node->hasSemFlag(SEMFLAG_EXEC_RET_STACK))
    {
        if (Semantic::mustInline(funcDecl))
        {
            SWAG_CHECK(Semantic::makeInline(context, funcDecl, node, false));
            YIELD();

            if (!node->hasSemFlag(SEMFLAG_RESOLVE_INLINED))
            {
                // Deal with inline mixin and macros added by the makeInline semantic pass
                if (node->ownerFct)
                {
                    while (!node->ownerFct->pendingInline.empty())
                    {
                        const auto identifier = node->ownerFct->pendingInline.back();
                        Semantic::makeInline(context, identifier, false);
                        YIELD();
                        node->ownerFct->pendingInline.pop_back();
                    }
                }

                node->addSemFlag(SEMFLAG_RESOLVE_INLINED);
                const auto back = node->lastChild();
                SWAG_ASSERT(back->is(AstNodeKind::Inline));
                context->baseJob->nodes.push_back(back);
                context->result = ContextResult::NewChildren;
            }

            return true;
        }
    }

    const bool foreign = funcDecl->isForeign();

    // We are less restrictive on type parameters for user op, as types are more in control.
    // Se we could have a necessary cast now.
    if (allParams)
    {
        for (const auto c : allParams->children)
        {
            SWAG_CHECK(emitCast(context, c, c->typeInfo, c->typeInfoCast));
            SWAG_ASSERT(context->result == ContextResult::Done);
        }
    }

    return emitCall(context, allParams ? allParams : node, funcDecl, nullptr, funcDecl->resultRegisterRc, foreign, freeRegisterParams);
}
