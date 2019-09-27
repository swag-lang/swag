#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitVarDecl(ByteCodeGenContext* context)
{
    auto node     = static_cast<AstVarDecl*>(context->node);
    auto resolved = node->resolvedSymbolOverload;
    auto typeInfo = TypeManager::concreteType(resolved->typeInfo, MakeConcrete::FlagAlias);

    // Initialize the struct, whatever, before the assignment
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(prepareEmitStructDrop(context, typeInfo));
        if (context->result == ByteCodeResult::Pending)
            return true;

        if (node->byteCodePass == 0)
        {
            if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
            {
                auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
                SWAG_ASSERT(typeStruct->opInitFct);
                emitStructInit(context, typeStruct, UINT32_MAX);
            }

            emitStructParameters(context, UINT32_MAX);
        }

        if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
        {
            if (node->byteCodePass == 0)
            {
                RegisterList r0                                                 = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::RARefFromStack, r0)->b.s32 = resolved->storageOffset;
                node->type->resultRegisterRC                                    = r0;
            }

            SWAG_CHECK(emitLocalCall(context, node, static_cast<AstFuncDecl*>(node->resolvedUserOpSymbolOverload->node), nullptr));
            if (context->result == ByteCodeResult::Pending)
                node->byteCodePass++;
            return true;
        }
    }

    // User initialization
    if (node->assignment)
    {
        RegisterList r0 = reserveRegisterRC(context);

        emitInstruction(context, ByteCodeOp::RARefFromStack, r0)->b.s32 = resolved->storageOffset;
        node->resultRegisterRC                                          = node->assignment->resultRegisterRC;
        SWAG_CHECK(emitCast(context, node->assignment, node->assignment->typeInfo, node->assignment->castedTypeInfo));
        emitAffectEqual(context, r0, node->resultRegisterRC, node->typeInfo, node->assignment);
        freeRegisterRC(context, r0);
        freeRegisterRC(context, node);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArray->rawType->kind == TypeInfoKind::Struct)
        {
            if ((typeArray->rawType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
            {
                if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
                {
                    // Need to loop on every element of the array in order to initialize them
                    RegisterList r0;
                    reserveRegisterRC(context, r0, 3);
                    emitInstruction(context, ByteCodeOp::CopyRAVB32, r0[0])->b.u32 = typeArray->totalCount;
                    emitInstruction(context, ByteCodeOp::ClearRA, r0[1]);
                    auto seekJump = context->bc->numInstructions;

                    if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                        emitStructInit(context, CastTypeInfo<TypeInfoStruct>(typeArray->rawType, TypeInfoKind::Struct), r0[1]);
                    emitStructParameters(context, r0[1]);

                    emitInstruction(context, ByteCodeOp::DecRA, r0[0]);
                    if (typeArray->rawType->sizeOf)
                        emitInstruction(context, ByteCodeOp::IncRAVB, r0[1])->b.u32 = typeArray->rawType->sizeOf;
                    emitInstruction(context, ByteCodeOp::IsNullU32, r0[0], r0[2]);
                    emitInstruction(context, ByteCodeOp::JumpNotTrue, r0[2])->b.s32 = seekJump - context->bc->numInstructions - 1;

                    freeRegisterRC(context, r0);
                }

                return true;
            }
        }
    }

    // No default init for structures, it has been done before
    if (typeInfo->kind == TypeInfoKind::Struct)
        return true;

    if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
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
