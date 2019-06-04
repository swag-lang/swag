#include "pch.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "Module.h"
#include "SourceFile.h"
#include "TypeManager.h"
#include "Global.h"

bool ByteCodeGenJob::emitCastNativeF32(ByteCodeGenContext* context, TypeInfo* typeInfo, uint32_t r0)
{
    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, r0);
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
    case NativeType::F32:
        SWAG_CHECK(emitCastNativeF32(context, fromTypeInfo, exprNode->resultRegisterRC));
        break;
    }

	node->resultRegisterRC = exprNode->resultRegisterRC;
    return true;
}