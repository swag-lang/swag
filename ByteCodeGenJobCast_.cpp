#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "Global.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"

bool ByteCodeGenJob::emitCastNativeBool(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CastBool64, exprNode->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastBool8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastBool16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastBool32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastBool64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeBool, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeU8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeU16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeU32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeU64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32U64, exprNode->resultRegisterRC, 0x000000FF, 0x00000000);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32U64, exprNode->resultRegisterRC, 0x0000FFFF, 0x00000000);
        break;
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeU64, invalid source type");
        break;
    }

    return true;
}
bool ByteCodeGenJob::emitCastNativeS8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeS16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeS32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeS64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeF32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastU64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeF32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastNativeF64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0xFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0xFFFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastU64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeF64, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastVariadic(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node              = context->node;
    node->typeInfo         = typeInfo;
    node->resultRegisterRC = exprNode->resultRegisterRC;
    emitStructDeRef(context);
    return true;
}

bool ByteCodeGenJob::emitCastSlice(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node        = context->node;
    auto toTypeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

    if (fromTypeInfo->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice     = CastTypeInfo<TypeInfoSlice>(fromTypeInfo, TypeInfoKind::Slice);
        int  diff              = fromTypeSlice->pointedType->sizeOf / toTypeSlice->pointedType->sizeOf;
        auto inst              = emitInstruction(context, ByteCodeOp::MulRAVB, exprNode->resultRegisterRC[1]);
        inst->b.u32            = diff;
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::TypeList)
    {
        auto fromTypeList      = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeList);
        int  diff              = fromTypeList->childs.front()->sizeOf / toTypeSlice->pointedType->sizeOf;
        auto inst              = emitInstruction(context, ByteCodeOp::MulRAVB, exprNode->resultRegisterRC[1]);
        inst->b.u32            = diff;
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Array)
    {
        auto fromTypeArray     = CastTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        node->resultRegisterRC += reserveRegisterRC(context);
        auto inst   = emitInstruction(context, ByteCodeOp::CopyRAVB32, node->resultRegisterRC[1]);
        inst->b.u32 = fromTypeArray->count;
    }
    else
    {
        return internalError(context, "emitCastSlice, invalid expression type", exprNode);
    }

    return true;
}

bool ByteCodeGenJob::emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    if (fromTypeInfo == nullptr)
        return true;
    SWAG_ASSERT(typeInfo);

    auto node = context->node;
    if (typeInfo->kind == TypeInfoKind::Pointer && fromTypeInfo->kind == TypeInfoKind::Pointer)
    {
        node->resultRegisterRC = exprNode->resultRegisterRC;
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        SWAG_CHECK(emitCastSlice(context, exprNode, typeInfo, fromTypeInfo));
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::VariadicValue)
    {
        SWAG_CHECK(emitCastVariadic(context, exprNode, typeInfo, fromTypeInfo));
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, cast type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
        SWAG_CHECK(emitCastNativeBool(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::S8:
        SWAG_CHECK(emitCastNativeS8(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::S16:
        SWAG_CHECK(emitCastNativeS16(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::S32:
        SWAG_CHECK(emitCastNativeS32(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::S64:
        SWAG_CHECK(emitCastNativeS64(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::U8:
        SWAG_CHECK(emitCastNativeU8(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::U16:
        SWAG_CHECK(emitCastNativeU16(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        SWAG_CHECK(emitCastNativeU32(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::U64:
        SWAG_CHECK(emitCastNativeU64(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::F32:
        SWAG_CHECK(emitCastNativeF32(context, exprNode, fromTypeInfo));
        break;
    case NativeTypeKind::F64:
        SWAG_CHECK(emitCastNativeF64(context, exprNode, fromTypeInfo));
        break;
    default:
        internalError(context, "emitCast, invalid cast type");
        break;
    }

    node->resultRegisterRC = exprNode->resultRegisterRC;
    exprNode->typeInfo     = typeInfo;
    return true;
}

bool ByteCodeGenJob::emitExplicitCast(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;

	// Specfic cast, with a type defined
    if (node->childs.size() == 2)
    {
        auto exprNode     = node->childs[1];
        auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
        SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo));
    }

	// Automatic cast
    else
    {
        auto exprNode     = node->childs[0];
        auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
        SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo));
    }

    return true;
}