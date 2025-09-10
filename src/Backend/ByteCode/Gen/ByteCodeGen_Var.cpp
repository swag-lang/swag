#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool ByteCodeGen::emitLocalVarDeclBefore(ByteCodeGenContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // No need to generate a local variable if it is never used
    if (context->sourceFile->module->mustOptimizeSemantic(node) &&
        node->resolvedSymbolOverload() &&
        !node->resolvedSymbolOverload()->hasFlag(OVERLOAD_USED))
    {
        // Keep structs, because of opDrop
        const auto typeInfo = TypeManager::concreteType(node->resolvedSymbolOverload()->typeInfo);
        if (!typeInfo->isStruct() && !typeInfo->isArrayOfStruct())
        {
            if (!node->assignment)
            {
                node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
                return true;
            }

            if (node->assignment->hasFlagComputedValue() || !node->assignment->hasAstFlag(AST_SIDE_EFFECTS))
            {
                SWAG_CHECK(skipNodes(context, node));
                return true;
            }
        }
    }

    return true;
}

bool ByteCodeGen::emitLocalVarDecl(ByteCodeGenContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Debug
    context->bc->localVars.push_back(context->node);

    // If this variable comes from a tuple unpacking, then we have nothing to generate
    if (node->assignment && node->assignment->hasAstFlag(AST_TUPLE_UNPACK))
        return true;

    const auto resolved = node->resolvedSymbolOverload();
    resolved->flags.add(OVERLOAD_EMITTED);

    const auto typeInfo = resolved->typeInfo->getConcreteAlias();
    const bool retVal   = resolved->hasFlag(OVERLOAD_RETVAL);

    Semantic::waitStructGenerated(context->baseJob, typeInfo);
    YIELD();

    // Struct initialization
    bool mustDropLeft = false;
    if (typeInfo->isStruct())
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

        // Generate initialization
        // Do not generate if we have a user define affectation, and the operator is marked as 'complete'
        const auto userOp = node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
        if (!userOp || userOp->symbol->isNot(SymbolKind::Function) || !userOp->node->hasAttribute(ATTRIBUTE_COMPLETE))
        {
            if (!node->hasSemFlag(SEMFLAG_VAR_DECL_STRUCT_PARAMETERS))
            {
                mustDropLeft = true;
                if (!node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED) && !node->hasAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS))
                {
                    // No need to initialize the variable if we are doing a struct to struct copy
                    // No need to drop the left side either
                    if (node->assignment && TypeManager::concretePtrRefType(node->assignment->typeInfo) == typeStruct)
                        mustDropLeft = false;
                    else
                        emitStructInit(context, typeStruct, UINT32_MAX, retVal);
                }

                emitStructParameters(context, UINT32_MAX, retVal);
                node->addSemFlag(SEMFLAG_VAR_DECL_STRUCT_PARAMETERS);
            }
        }

        // User special function
        if (node->hasSpecialFuncCall())
        {
            if (!node->hasSemFlag(SEMFLAG_VAR_DECL_REF_CALL))
            {
                const RegisterList r0 = reserveRegisterRC(context);
                emitRetValRef(context, resolved, r0, retVal, resolved->computedValue.storageOffset);
                node->type->resultRegisterRc = r0;
                node->addSemFlag(SEMFLAG_VAR_DECL_REF_CALL);
            }

            SWAG_CHECK(emitUserOp(context, nullptr, node));
            YIELD();

            return true;
        }
    }

    // User-specific initialization with a right side
    if (node->assignment && !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        // @DirectInlineLocalVar
        // The local variable is using the storage from the inline call.
        // No need to make a copy
        if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_INLINE_STORAGE) && !node->assignment->hasComputedValue())
        {
            freeRegisterRC(context, node->assignment);
        }
        // @ForceNoAffect
        else if (!node->assignment->resultRegisterRc.empty())
        {
            if (!node->hasSemFlag(SEMFLAG_PRE_CAST))
            {
                node->allocateExtension(ExtensionKind::Misc);
                node->extMisc()->additionalRegisterRC = reserveRegisterRC(context);
                emitRetValRef(context, resolved, node->extMisc()->additionalRegisterRC, retVal, resolved->computedValue.storageOffset);
                node->resultRegisterRc = node->assignment->resultRegisterRc;
                node->addSemFlag(SEMFLAG_PRE_CAST);
            }

            SWAG_CHECK(emitCast(context, node->assignment, node->assignment->typeInfo, node->assignment->typeInfoCast));
            YIELD();

            if (!mustDropLeft)
                node->assignment->addAstFlag(AST_NO_LEFT_DROP);

            bool isLet = node->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET);
            if (!node->typeInfo->isNativeIntegerOrRune() &&
                !node->typeInfo->isNativeFloat() &&
                !node->typeInfo->isPointer() &&
                !node->typeInfo->isBool())
            {
                isLet = false;
            }

            // Keep the value in a persistent register, as it cannot be changed
            if (isLet && !resolved->hasFlag(OVERLOAD_REG_PERSISTENT))
            {
                context->bc->staticRegs += node->resultRegisterRc.size();
                node->resultRegisterRc.cannotFree = true;
                resolved->setRegisters(node->resultRegisterRc, OVERLOAD_REG_PERSISTENT);

                switch (resolved->typeInfo->sizeOf)
                {
                    case 1:
                        EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRc)->b.u64 = 0x000000FF;
                        break;
                    case 2:
                        EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRc)->b.u64 = 0x0000FFFF;
                        break;
                    case 4:
                        if (!resolved->typeInfo->isNativeFloat())
                            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, node->resultRegisterRc)->b.u64 = 0xFFFFFFFF;
                        break;
                    default:
                        break;
                }
            }

            // Store value to stack
            if (!isLet || resolved->hasFlag(OVERLOAD_HAS_MAKE_POINTER) || context->sourceFile->module->buildCfg.byteCodeOptimizeLevel <= BuildCfgByteCodeOptim::O2)
            {
                node->allocateExtension(ExtensionKind::Misc);
                emitAffectEqual(context, node->extMisc()->additionalRegisterRC, node->resultRegisterRc, node->typeInfo, node->assignment);
                YIELD();
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
        const auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        const auto finalType = typeArray->finalType;
        if (finalType->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES) || node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
        {
            if (!node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED) || node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
            {
                if (finalType->sizeOf)
                {
                    if (typeArray->totalCount == 1)
                    {
                        if (!node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED) && !node->hasAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS))
                            emitStructInit(context, castTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct), UINT32_MAX, retVal);
                        emitStructParameters(context, UINT32_MAX, retVal);
                    }
                    else
                    {
                        // Need to loop on every element of the array to initialize them
                        RegisterList r0;
                        reserveRegisterRC(context, r0, 2);

                        EMIT_INST1(context, ByteCodeOp::ClearRA, r0[0]);
                        const auto seekJump = context->bc->numInstructions;

                        if (!node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED) && !node->hasAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS))
                            emitStructInit(context, castTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct), r0[0], retVal);
                        emitStructParameters(context, r0[0], retVal);

                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[0])->b.u64 = finalType->sizeOf;

                        auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], 0, r0[1]);
                        inst->b.u64 = static_cast<uint64_t>(typeArray->totalCount) * finalType->sizeOf;
                        inst->addFlag(BCI_IMM_B);

                        inst        = EMIT_INST1(context, ByteCodeOp::JumpIfFalse, r0[1]);
                        inst->b.s32 = static_cast<int32_t>(seekJump - context->bc->numInstructions);

                        freeRegisterRC(context, r0);
                    }
                }

                freeStructParametersRegisters(context);
            }

            return true;
        }
    }

    if (!node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
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
            emitSetZeroStack(context, resolved->computedValue.storageOffset, typeInfo->sizeOf);
        }
    }

    return true;
}
