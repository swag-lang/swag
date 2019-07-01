#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitVarDecl(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    auto resolved = node->resolvedSymbolOverload;

    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        // User initialization
        if (node->astAssignment)
        {
            RegisterList r0;
            r0 = reserveRegisterRC(context);

            emitInstruction(context, ByteCodeOp::RARefFromStack, r0)->b.s32 = resolved->storageOffset;
            emitAffectEqual(context, r0, node->astAssignment->resultRegisterRC);
            freeRegisterRC(context, r0);
            freeRegisterRC(context, node->astAssignment->resultRegisterRC);
        }
        else if (!(node->flags & AST_DISABLED_INIT))
        {
            // Default initialization
            if (resolved->typeInfo->isNative(NativeType::String))
            {
				emitInstruction(context, ByteCodeOp::ClearRefFromStackPointer)->a.u32 = resolved->storageOffset;
            }
            else if (resolved->typeInfo->kind == TypeInfoKind::Native)
            {
                switch (resolved->typeInfo->sizeOf)
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
                    return internalError(context, "emitVarDecl, invalid size of type for default initialization");
                }
            }
            else if (resolved->typeInfo->kind == TypeInfoKind::Pointer)
            {
                emitInstruction(context, ByteCodeOp::ClearRefFromStackPointer)->a.u32 = resolved->storageOffset;
            }
        }
    }

    return true;
}
