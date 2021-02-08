#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitLocalVarDecl(ByteCodeGenContext* context)
{
    auto node = static_cast<AstVarDecl*>(context->node);

    // Debug
    context->bc->localVars.push_back(context->node);

    // If this variable comes from a tuple unpacking, then we have nothing to generate
    if (node->assignment && node->assignment->flags & AST_TUPLE_UNPACK)
        return true;

    auto resolved = node->resolvedSymbolOverload;

    resolved->flags |= OVERLOAD_EMITTED;

    auto typeInfo = TypeManager::concreteType(resolved->typeInfo, CONCRETE_ALIAS);
    bool retVal   = resolved->flags & OVERLOAD_RETVAL;

    // Initialize the struct, whatever, before the assignment
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        context->job->waitStructGenerated(typeStruct);
        if (context->result == ContextResult::Pending)
            return true;

        // Generate initialization
        // Do not generate if we have a user define affectation, and the operator is marked as 'complete'
        if (!node->extension ||
            !node->extension->resolvedUserOpSymbolOverload ||
            node->extension->resolvedUserOpSymbolOverload->symbol->kind != SymbolKind::Function ||
            !(node->extension->resolvedUserOpSymbolOverload->node->attributeFlags & ATTRIBUTE_COMPLETE))
        {
            if (!(node->doneFlags & AST_DONE_VARDECL_STRUCT_PARAMETERS))
            {
                if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                {
                    // No need to initialize the variable if we are doing a struct to struct copy
                    if (node->assignment && node->assignment->typeInfo == typeStruct)
                        node->assignment->flags |= AST_NO_LEFT_DROP;
                    else
                        emitStructInit(context, typeStruct, UINT32_MAX, retVal);
                }

                emitStructParameters(context, UINT32_MAX, retVal);
                node->doneFlags |= AST_DONE_VARDECL_STRUCT_PARAMETERS;
            }
        }

        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!(node->doneFlags & AST_DONE_VARDECL_REF_CALL))
            {
                RegisterList r0 = reserveRegisterRC(context);

                if (retVal)
                {
                    emitRetValRef(context, r0);
                }
                else
                {
                    auto inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
                    SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
                    inst->b.s32 = resolved->storageOffset;
                }

                node->type->resultRegisterRC = r0;
                node->doneFlags |= AST_DONE_VARDECL_REF_CALL;
            }

            SWAG_CHECK(emitUserOp(context, nullptr, node));
            if (context->result != ContextResult::Done)
                return true;

            return true;
        }
    }

    // User initialization
    if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (!(node->doneFlags & AST_DONE_PRE_CAST))
        {
            node->additionalRegisterRC = reserveRegisterRC(context);
            node->doneFlags |= AST_DONE_PRE_CAST;

            if (retVal)
            {
                emitRetValRef(context, node->additionalRegisterRC);
            }
            else
            {
                auto inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->additionalRegisterRC);
                SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
                inst->b.s32 = resolved->storageOffset;
            }

            node->resultRegisterRC = node->assignment->resultRegisterRC;
        }

        SWAG_CHECK(emitCast(context, node->assignment, node->assignment->typeInfo, node->assignment->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;

        emitAffectEqual(context, node->additionalRegisterRC, node->resultRegisterRC, node->typeInfo, node->assignment);
        freeRegisterRC(context, node);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArray->finalType->kind == TypeInfoKind::Struct)
        {
            context->job->waitStructGenerated(typeArray->finalType);
            if (context->result == ContextResult::Pending)
                return true;

            if ((typeArray->finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
            {
                if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
                {
                    if (typeArray->totalCount == 1)
                    {
                        if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                            emitStructInit(context, CastTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct), UINT32_MAX, retVal);
                        emitStructParameters(context, UINT32_MAX, retVal);
                    }
                    else
                    {
                        // Need to loop on every element of the array in order to initialize them
                        RegisterList r0;
                        reserveRegisterRC(context, r0, 2);
                        emitInstruction(context, ByteCodeOp::SetImmediate64, r0[0])->b.u64 = typeArray->totalCount;
                        emitInstruction(context, ByteCodeOp::ClearRA, r0[1]);
                        auto seekJump = context->bc->numInstructions;

                        if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                            emitStructInit(context, CastTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct), r0[1], retVal);
                        emitStructParameters(context, r0[1], retVal);

                        emitInstruction(context, ByteCodeOp::DecrementRA64, r0[0]);
                        if (typeArray->finalType->sizeOf)
                            emitInstruction(context, ByteCodeOp::Add64byVB64, r0[1])->b.u64 = typeArray->finalType->sizeOf;
                        emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r0[0])->b.s32 = seekJump - context->bc->numInstructions - 1;

                        freeRegisterRC(context, r0);
                    }

                    freeStructParametersRegisters(context);
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
        if (retVal)
        {
            RegisterList r0 = reserveRegisterRC(context);
            emitRetValRef(context, r0);
            emitSetZeroAtPointer(context, typeInfo->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else
        {
            SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
            switch (typeInfo->sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::SetZeroStack8)->a.u32 = resolved->storageOffset;
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::SetZeroStack16)->a.u32 = resolved->storageOffset;
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::SetZeroStack32)->a.u32 = resolved->storageOffset;
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::SetZeroStack64)->a.u32 = resolved->storageOffset;
                break;
            default:
            {
                auto inst   = emitInstruction(context, ByteCodeOp::SetZeroStackX);
                inst->a.u32 = resolved->storageOffset;
                inst->b.u64 = typeInfo->sizeOf;
                break;
            }
            }
        }
    }

    return true;
}
