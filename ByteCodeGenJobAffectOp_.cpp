#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"

bool ByteCodeGenJob::emitAffectEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
    case NativeType::S8:
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOp8, r0, r1);
        return true;
	case NativeType::S16:
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOp16, r0, r1);
        return true;
	case NativeType::S32:
    case NativeType::U32:
    case NativeType::F32:
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::AffectOp32, r0, r1);
        return true;
    case NativeType::S64:
    case NativeType::U64:
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOp64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffect(ByteCodeGenContext* context)
{
    AstNode* node   = context->node;
    auto     module = context->sourceFile->module;

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;
    module->freeRegisterRC(r0);
    module->freeRegisterRC(r1);

    emitCast(context, node->childs[1]->castedTypeInfo, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo));
    switch (node->token.id)
    {
    case TokenId::SymEqual:
        SWAG_CHECK(emitAffectEqual(context, r0, r1));
        return true;
    default:
        return internalError(context, "emitAffect, invalid token op");
    }

    return true;
}