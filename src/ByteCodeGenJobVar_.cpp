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

    context->job->waitStructGenerated(typeInfo);
    if (context->result == ContextResult::Pending)
        return true;

    // Struct initialization
    bool mustDropLeft = false;
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

        // Generate initialization
        // Do not generate if we have a user define affectation, and the operator is marked as 'complete'
        if (!node->extension ||
            !node->extension->resolvedUserOpSymbolOverload ||
            node->extension->resolvedUserOpSymbolOverload->symbol->kind != SymbolKind::Function ||
            !(node->extension->resolvedUserOpSymbolOverload->node->attributeFlags & ATTRIBUTE_COMPLETE))
        {
            if (!(node->doneFlags & AST_DONE_VARDECL_STRUCT_PARAMETERS))
            {
                mustDropLeft = true;
                if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                {
                    // No need to initialize the variable if we are doing a struct to struct copy
                    // No need to drop the left side either
                    if (node->assignment && node->assignment->typeInfo == typeStruct)
                        mustDropLeft = false;
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
                emitRetValRef(context, r0, retVal, resolved->storageOffset);
                node->type->resultRegisterRC = r0;
                node->doneFlags |= AST_DONE_VARDECL_REF_CALL;
            }

            SWAG_CHECK(emitUserOp(context, nullptr, node));
            if (context->result != ContextResult::Done)
                return true;

            return true;
        }
    }

    // Allocate a scoped register to the variable
    /*if (node->ownerFct && node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (!(resolved->flags & OVERLOAD_CAN_CHANGE) && resolved->registers.size() == 0)
        {
            if (resolved->typeInfo->numRegisters() == 1)
            {
                //if (node->ownerFct->token.text == "convertArgcArgv1")
                {
                    resolved->flags |= OVERLOAD_REGISTER;
                    resolved->registers         = reserveRegisterRC(context);
                    resolved->registers.canFree = false;
                    node->ownerScope->owner->allocateExtension();
                    for (int i = 0; i < resolved->registers.size(); i++)
                        node->ownerScope->owner->extension->registersToRelease.push_back(resolved->registers[i]);
                }
            }
        }
    }*/

    // User specific initialization with a right side
    if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (!(node->doneFlags & AST_DONE_PRE_CAST))
        {
            if (resolved->flags & OVERLOAD_REGISTER)
            {
                node->additionalRegisterRC = resolved->registers;
                emitInstruction(context, ByteCodeOp::CopyRBAddrToRA, node->additionalRegisterRC, resolved->registers);
            }
            else
            {
                node->additionalRegisterRC = reserveRegisterRC(context);
                emitRetValRef(context, node->additionalRegisterRC, retVal, resolved->storageOffset);
            }

            node->resultRegisterRC = node->assignment->resultRegisterRC;
            node->doneFlags |= AST_DONE_PRE_CAST;
        }

        SWAG_CHECK(emitCast(context, node->assignment, node->assignment->typeInfo, node->assignment->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;

        if (!mustDropLeft)
            node->assignment->flags |= AST_NO_LEFT_DROP;
        emitAffectEqual(context, node->additionalRegisterRC, node->resultRegisterRC, node->typeInfo, node->assignment);
        if (context->result == ContextResult::Pending)
            return true;

        freeRegisterRC(context, node);
        return true;
    }

    // No default init for structures, it has been done before
    if (typeInfo->kind == TypeInfoKind::Struct)
        return true;

    if (typeInfo->isArrayOfStruct())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto finalType = typeArray->finalType;
        if ((finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
        {
            if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) || (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
            {
                if (typeArray->totalCount == 1)
                {
                    if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                        emitStructInit(context, CastTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct), UINT32_MAX, retVal);
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
                        emitStructInit(context, CastTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct), r0[1], retVal);
                    emitStructParameters(context, r0[1], retVal);

                    emitInstruction(context, ByteCodeOp::DecrementRA64, r0[0]);
                    if (finalType->sizeOf)
                        emitInstruction(context, ByteCodeOp::Add64byVB64, r0[1])->b.u64 = finalType->sizeOf;
                    emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r0[0])->b.s32 = seekJump - context->bc->numInstructions - 1;

                    freeRegisterRC(context, r0);
                }

                freeStructParametersRegisters(context);
            }

            return true;
        }
    }

    if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (retVal)
        {
            RegisterList r0 = reserveRegisterRC(context);
            emitRetValRef(context, r0, true, 0);
            emitSetZeroAtPointer(context, typeInfo->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else if (resolved->flags & OVERLOAD_REGISTER)
        {
            SWAG_ASSERT(resolved->registers.size());
            for (int i = 0; i < resolved->registers.size(); i++)
                emitInstruction(context, ByteCodeOp::ClearRA)->a.u32 = resolved->registers[i];
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
