#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitUnaryOpMinus(ByteCodeGenContext* context, uint32_t r0)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::NegS32, r0);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::NegS64, r0);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::NegF32, r0);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::NegF64, r0);
        return true;
    default:
        return internalError(context);
    }
}
bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;
    auto     r0   = node->childs[0]->resultRegisterRC;

    node->resultRegisterRC = r0;
    switch (node->token.id)
    {
    case TokenId::SymExclam:
        emitInstruction(context, ByteCodeOp::NegBool, r0);
        return true;
    case TokenId::SymMinus:
        SWAG_CHECK(emitUnaryOpMinus(context, r0));
        return true;
    default:
        return internalError(context);
    }
}
