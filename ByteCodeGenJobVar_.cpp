#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitVarDecl(ByteCodeGenContext* context)
{
    auto node     = static_cast<AstVarDecl*>(context->node);
    auto resolved = node->resolvedSymbolOverload;
    auto typeInfo = resolved->typeInfo;

    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        // User initialization
        if (node->astAssignment)
        {
            RegisterList r0;
            r0          = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
            inst->b.s32 = resolved->storageOffset;
            emitAffectEqual(context, r0, node->astAssignment->resultRegisterRC, node->typeInfo, node->astAssignment->typeInfo);
            freeRegisterRC(context, r0);
            freeRegisterRC(context, node->astAssignment->resultRegisterRC);
			return true;
        }

        // No default initialization
        if (node->flags & AST_DISABLED_INIT)
            return true;

        if (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            if (typeArray->pointedType->kind == TypeInfoKind::Struct)
            {
                return true;
            }
        }

        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            emitStructInit(context);
            return true;
        }

        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack8)->a.u32 = resolved->storageOffset;
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack16)->a.u32 = resolved->storageOffset;
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack32)->a.u32 = resolved->storageOffset;
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::ClearRefFromStack64)->a.u32 = resolved->storageOffset;
            break;
        default:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
            inst->a.u32 = resolved->storageOffset;
            inst->b.u32 = typeInfo->sizeOf;
            break;
        }
        }
    }

    return true;
}
