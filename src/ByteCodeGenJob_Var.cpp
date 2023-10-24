#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Module.h"
#include "Ast.h"

bool ByteCodeGenJob::emitLocalVarDeclBefore(ByteCodeGenContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // No need to generate a local variable if it is never used
    if (context->sourceFile->module->mustOptimizeBytecode(node))
    {
        if (node->resolvedSymbolOverload && !(node->resolvedSymbolOverload->flags & OVERLOAD_USED))
        {
            // Keep structs, because of opDrop
            auto typeInfo = TypeManager::concreteType(node->resolvedSymbolOverload->typeInfo);
            if (!typeInfo->isStruct() && !typeInfo->isArrayOfStruct())
            {
                if (!node->assignment)
                {
                    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
                    return true;
                }

                if (node->assignment->hasComputedValue() || !(node->assignment->flags & AST_SIDE_EFFECTS))
                {
                    SWAG_CHECK(skipNodes(context, node));
                    return true;
                }
            }
        }
    }

    return true;
}

bool ByteCodeGenJob::emitLocalVarDecl(ByteCodeGenContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Debug
    context->bc->localVars.push_back(context->node);

    // If this variable comes from a tuple unpacking, then we have nothing to generate
    if (node->assignment && node->assignment->flags & AST_TUPLE_UNPACK)
        return true;

    auto resolved = node->resolvedSymbolOverload;
    resolved->flags |= OVERLOAD_EMITTED;

    auto typeInfo = TypeManager::concreteType(resolved->typeInfo, CONCRETE_FORCEALIAS);
    bool retVal   = resolved->flags & OVERLOAD_RETVAL;

    context->job->waitStructGenerated(typeInfo);
    if (context->result == ContextResult::Pending)
        return true;

    // Struct initialization
    bool mustDropLeft = false;
    if (typeInfo->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

        // Generate initialization
        // Do not generate if we have a user define affectation, and the operator is marked as 'complete'
        if (!node->hasExtMisc() ||
            !node->extMisc()->resolvedUserOpSymbolOverload ||
            node->extMisc()->resolvedUserOpSymbolOverload->symbol->kind != SymbolKind::Function ||
            !(node->extMisc()->resolvedUserOpSymbolOverload->node->attributeFlags & ATTRIBUTE_COMPLETE))
        {
            if (!(node->semFlags & SEMFLAG_VARDECL_STRUCT_PARAMETERS))
            {
                mustDropLeft = true;
                if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                {
                    // No need to initialize the variable if we are doing a struct to struct copy
                    // No need to drop the left side either
                    if (node->assignment && TypeManager::concreteType(node->assignment->typeInfo) == typeStruct)
                        mustDropLeft = false;
                    else
                        emitStructInit(context, typeStruct, UINT32_MAX, retVal);
                }

                emitStructParameters(context, UINT32_MAX, retVal);
                node->semFlags |= SEMFLAG_VARDECL_STRUCT_PARAMETERS;
            }
        }

        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!(node->semFlags & SEMFLAG_VARDECL_REF_CALL))
            {
                RegisterList r0 = reserveRegisterRC(context);
                emitRetValRef(context, resolved, r0, retVal, resolved->computedValue.storageOffset);
                node->type->resultRegisterRC = r0;
                node->semFlags |= SEMFLAG_VARDECL_REF_CALL;
            }

            SWAG_CHECK(emitUserOp(context, nullptr, node));
            if (context->result != ContextResult::Done)
                return true;

            return true;
        }
    }

    // User specific initialization with a right side
    if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        // :DirectInlineLocalVar
        // The local variable is using the storage from the inline call.
        // No need to make a copy
        if (node->specFlags & AstVarDecl::SPECFLAG_INLINE_STORAGE)
        {
            freeRegisterRC(context, node->assignment);
        }
        else
        {
            if (!(node->semFlags & SEMFLAG_PRE_CAST))
            {
                node->allocateExtension(ExtensionKind::Misc);
                node->extMisc()->additionalRegisterRC = reserveRegisterRC(context);
                emitRetValRef(context, resolved, node->extMisc()->additionalRegisterRC, retVal, resolved->computedValue.storageOffset);
                node->resultRegisterRC = node->assignment->resultRegisterRC;
                node->semFlags |= SEMFLAG_PRE_CAST;
            }

            SWAG_CHECK(emitCast(context, node->assignment, node->assignment->typeInfo, node->assignment->castedTypeInfo));
            if (context->result != ContextResult::Done)
                return true;

            if (!mustDropLeft)
                node->assignment->flags |= AST_NO_LEFT_DROP;

            bool isLet = node->specFlags & AstVarDecl::SPECFLAG_IS_LET;
            if (!node->typeInfo->isNativeIntegerOrRune() &&
                !node->typeInfo->isNativeFloat() &&
                !node->typeInfo->isPointer() &&
                !node->typeInfo->isBool())
            {
                isLet = false;
            }

            // Keep the value in a persistent register, as it cannot be changed
            if (isLet && !(resolved->flags & OVERLOAD_PERSISTENT_REG))
            {
                context->bc->staticRegs += node->resultRegisterRC.size();
                node->resultRegisterRC.cannotFree = true;
                resolved->setRegisters(node->resultRegisterRC, OVERLOAD_PERSISTENT_REG);

                switch (resolved->typeInfo->sizeOf)
                {
                case 1:
                    EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRC)->b.u64 = 0x000000FF;
                    break;
                case 2:
                    EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRC)->b.u64 = 0x0000FFFF;
                    break;
                case 4:
                    if (!resolved->typeInfo->isNativeFloat())
                        EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRC)->b.u64 = 0xFFFFFFFF;
                    break;
                }
            }

            // Store value to stack
            if (!isLet || (resolved->flags & OVERLOAD_HAS_MAKE_POINTER) || (context->sourceFile->module->buildCfg.byteCodeOptimizeLevel != 2))
            {
                node->allocateExtension(ExtensionKind::Misc);
                emitAffectEqual(context, node->extMisc()->additionalRegisterRC, node->resultRegisterRC, node->typeInfo, node->assignment);
                if (context->result != ContextResult::Done)
                    return true;
            }

            freeRegisterRC(context, node);
        }

        return true;
    }

    // No default init for structures, it has been done before
    if (typeInfo->isStruct())
        return true;

    if (typeInfo->isArrayOfStruct())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto finalType = typeArray->finalType;
        if ((finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES) || (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
        {
            if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) || (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
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
                    EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0[0])->b.u64 = typeArray->totalCount;
                    EMIT_INST1(context, ByteCodeOp::ClearRA, r0[1]);
                    auto seekJump = context->bc->numInstructions;

                    if (!(node->flags & AST_EXPLICITLY_NOT_INITIALIZED) && !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS))
                        emitStructInit(context, CastTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct), r0[1], retVal);
                    emitStructParameters(context, r0[1], retVal);

                    EMIT_INST1(context, ByteCodeOp::DecrementRA64, r0[0]);
                    if (finalType->sizeOf)
                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[1])->b.u64 = finalType->sizeOf;
                    EMIT_INST1(context, ByteCodeOp::JumpIfNotZero64, r0[0])->b.s32 = seekJump - context->bc->numInstructions - 1;

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
            emitRetValRef(context, resolved, r0, true, 0);
            emitSetZeroAtPointer(context, typeInfo->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else if (typeInfo->isClosure())
        {
            EMIT_INST0(context, ByteCodeOp::SetZeroStack64)->a.u32 = resolved->computedValue.storageOffset;
        }
        else
        {
            SWAG_ASSERT(resolved->computedValue.storageOffset != UINT32_MAX);
            switch (typeInfo->sizeOf)
            {
            case 1:
                EMIT_INST0(context, ByteCodeOp::SetZeroStack8)->a.u32 = resolved->computedValue.storageOffset;
                break;
            case 2:
                EMIT_INST0(context, ByteCodeOp::SetZeroStack16)->a.u32 = resolved->computedValue.storageOffset;
                break;
            case 4:
                EMIT_INST0(context, ByteCodeOp::SetZeroStack32)->a.u32 = resolved->computedValue.storageOffset;
                break;
            case 8:
                EMIT_INST0(context, ByteCodeOp::SetZeroStack64)->a.u32 = resolved->computedValue.storageOffset;
                break;
            default:
            {
                auto inst   = EMIT_INST0(context, ByteCodeOp::SetZeroStackX);
                inst->a.u32 = resolved->computedValue.storageOffset;
                inst->b.u64 = typeInfo->sizeOf;
                break;
            }
            }
        }
    }

    return true;
}
