#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "Global.h"
#include "ByteCodeOp.h"

bool ByteCodeGenJob::emitCastNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        break;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        break;
    case NativeType::U32:
    case NativeType::U64:
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeType::S32:
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S64, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        break;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        break;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        break;
    case NativeType::U32:
    case NativeType::U64:
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeType::S32:
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC, 0x00000000);
        break;
    case NativeType::U64:
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeType::S64:
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x000000FF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0x0000FFFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CastU64F32, exprNode->resultRegisterRC);
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64F32, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        break;
    case NativeType::F64:
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
    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0xFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC, 0xFFFF);
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CastU64F64, exprNode->resultRegisterRC);
        break;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CastS64F64, exprNode->resultRegisterRC);
        break;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeType::F64:
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastNativeF64, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCast(ByteCodeGenContext* context)
{
    AstNode* node         = context->node;
    auto     typeInfo     = node->typeInfo;
    auto     exprNode     = node->childs[1];
    auto     fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, cast type not native");
    if (fromTypeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        SWAG_CHECK(emitCastNativeS8(context, exprNode, fromTypeInfo));
        break;
    case NativeType::S16:
        SWAG_CHECK(emitCastNativeS16(context, exprNode, fromTypeInfo));
        break;
    case NativeType::S32:
        SWAG_CHECK(emitCastNativeS32(context, exprNode, fromTypeInfo));
        break;
    case NativeType::S64:
        SWAG_CHECK(emitCastNativeS64(context, exprNode, fromTypeInfo));
        break;
    case NativeType::U8:
        SWAG_CHECK(emitCastNativeU8(context, exprNode, fromTypeInfo));
        break;
    case NativeType::U16:
        SWAG_CHECK(emitCastNativeU16(context, exprNode, fromTypeInfo));
        break;
    case NativeType::U32:
        SWAG_CHECK(emitCastNativeU32(context, exprNode, fromTypeInfo));
        break;
    case NativeType::U64:
        SWAG_CHECK(emitCastNativeU64(context, exprNode, fromTypeInfo));
        break;
    case NativeType::F32:
        SWAG_CHECK(emitCastNativeF32(context, exprNode, fromTypeInfo));
        break;
    case NativeType::F64:
        SWAG_CHECK(emitCastNativeF64(context, exprNode, fromTypeInfo));
        break;
    default:
        internalError(context, "emitCast, invalid cast type");
        break;
    }

    node->resultRegisterRC = exprNode->resultRegisterRC;
    return true;
}