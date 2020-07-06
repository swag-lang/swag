#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto node = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);

    // No need to do the scope leave stuff if the function does return something, because
    // it has been covered by the mandatory return
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        return true;

    SWAG_CHECK(emitLeaveScope(context, node->scope));
    if (context->result != ContextResult::Done)
        return true;

    if (node->stackSize)
        emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = node->stackSize;

    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitFuncCallParam(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    auto     back          = node->childs.front();
    node->resultRegisterRC = back->resultRegisterRC;
    SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
    return true;
}

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    AstReturn* node       = CastAst<AstReturn>(context->node, AstNodeKind::Return);
    auto       funcNode   = node->ownerFct;
    auto       returnType = funcNode->returnType->typeInfo;

    // Copy result to RR0... registers
    if (!(node->doneFlags & AST_DONE_EMIT_DEFERRED))
    {
        if (!node->childs.empty())
        {
            auto returnExpression = node->childs.front();
            if (node->ownerInline && (node->flags & AST_EMBEDDED_RETURN))
            {
                auto inlineReturnType = node->ownerInline->func->returnType->typeInfo;
                if (inlineReturnType->kind == TypeInfoKind::Struct)
                    return internalError(context, "emitReturn, invalid inline return");
                else if (inlineReturnType->flags & TYPEINFO_RETURN_BY_COPY)
                    return internalError(context, "emitReturn, invalid inline return");
                else
                {
                    for (auto child : node->childs)
                    {
                        auto sizeChilds = child->resultRegisterRC.size();
                        for (int r = 0; r < sizeChilds; r++)
                        {
                            emitInstruction(context, ByteCodeOp::CopyRARB, node->ownerInline->resultRegisterRC[r], child->resultRegisterRC[r]);
                        }
                    }
                }
            }
            else
            {
                if (returnType->kind == TypeInfoKind::Struct)
                {
                    waitStructGenerated(context, CastTypeInfo<TypeInfoStruct>(returnExpression->typeInfo, TypeInfoKind::Struct));
                    if (context->result == ContextResult::Pending)
                        return true;
                    RegisterList r0 = reserveRegisterRC(context);
                    emitInstruction(context, ByteCodeOp::CopyRCxRRx, r0, 0);
                    // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                    returnExpression->flags |= AST_NO_LEFT_DROP;
                    SWAG_CHECK(emitStructCopyMoveCall(context, r0, returnExpression->resultRegisterRC, returnExpression->typeInfo, returnExpression));
                    freeRegisterRC(context, r0);
                }
                else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
                {
                    auto r0 = reserveRegisterRC(context);
                    emitInstruction(context, ByteCodeOp::CopyRCxRRx, r0, 0);
                    emitInstruction(context, ByteCodeOp::CopyVC, r0, returnExpression->resultRegisterRC)->c.u32 = returnExpression->typeInfo->sizeOf;
                    freeRegisterRC(context, r0);
                }
                else if (returnType->isNative(NativeTypeKind::String))
                {
                    auto child = node->childs.front();
                    if (funcNode->attributeFlags & ATTRIBUTE_AST_FUNC)
                    {
                        emitInstruction(context, ByteCodeOp::CloneString, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                    }

                    emitInstruction(context, ByteCodeOp::CopyRRxRCx, 0, child->resultRegisterRC[0]);
                    emitInstruction(context, ByteCodeOp::CopyRRxRCx, 1, child->resultRegisterRC[1]);
                }
                else
                {
                    for (auto child : node->childs)
                    {
                        auto sizeChilds = child->resultRegisterRC.size();
                        for (int r = 0; r < sizeChilds; r++)
                        {
                            emitInstruction(context, ByteCodeOp::CopyRRxRCx, r, child->resultRegisterRC[r]);
                        }
                    }
                }
            }
        }

        node->doneFlags |= AST_DONE_EMIT_DEFERRED;
    }

    // Leave all scopes
    Scope* topScope = nullptr;
    if (node->ownerInline && (node->flags & AST_EMBEDDED_RETURN))
        topScope = node->ownerInline->scope;
    else
        topScope = funcNode->scope;

    Scope::collectScopeFrom(node->ownerScope, topScope, context->job->collectScopes);
    for (auto scope : context->job->collectScopes)
    {
        SWAG_CHECK(emitLeaveScope(context, scope, node->forceNoDrop));
        if (context->result != ContextResult::Done)
            return true;
    }

    // A return inside an inline function is just a jump to the end of the block
    if (node->ownerInline && (node->flags & AST_EMBEDDED_RETURN))
    {
        node->seekJump = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::Jump);
        node->ownerInline->returnList.push_back(node);
    }
    else
    {
        if (funcNode->stackSize)
            emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = funcNode->stackSize;
        emitInstruction(context, ByteCodeOp::Ret);
    }

    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstIdentifier>(context->node, AstNodeKind::FuncCall);
    auto callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParams);

    SWAG_ASSERT(!node->name.empty());
    SWAG_ASSERT(g_LangSpec.intrinsics.find(node->name) != g_LangSpec.intrinsics.end());

    auto intrinsic = g_LangSpec.intrinsics[node->name];
    switch (intrinsic)
    {
    case Intrinsic::IntrinsicPrint:
    {
        auto child0   = callParams->childs[0];
        auto typeInfo = TypeManager::concreteType(child0->typeInfo);
        if (typeInfo->kind == TypeInfoKind::Native)
        {
            switch (typeInfo->nativeType)
            {
            case NativeTypeKind::S64:
                emitInstruction(context, ByteCodeOp::IntrinsicPrintS64, child0->resultRegisterRC);
                break;
            case NativeTypeKind::F64:
                emitInstruction(context, ByteCodeOp::IntrinsicPrintF64, child0->resultRegisterRC);
                break;
            case NativeTypeKind::String:
                emitInstruction(context, ByteCodeOp::IntrinsicPrintString, child0->resultRegisterRC[0], child0->resultRegisterRC[1]);
                break;
            default:
                return internalError(context, "emitIntrinsic, @print invalid type");
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Slice)
        {
            emitInstruction(context, ByteCodeOp::IntrinsicPrintString, child0->resultRegisterRC[0], child0->resultRegisterRC[1]);
        }
        else
        {
            return internalError(context, "emitIntrinsic, @print invalid type");
        }

        freeRegisterRC(context, child0);
        break;
    }
    case Intrinsic::IntrinsicAssert:
    {
        auto child0 = callParams->childs.front();
        if (!child0->isConstantTrue())
            emitInstruction(context, ByteCodeOp::IntrinsicAssert, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case Intrinsic::IntrinsicAlloc:
    {
        auto child0            = callParams->childs.front();
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicAlloc, node->resultRegisterRC, child0->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicFree:
    {
        auto child0 = callParams->childs.front();
        emitInstruction(context, ByteCodeOp::IntrinsicFree, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case Intrinsic::IntrinsicRealloc:
    {
        auto child0            = callParams->childs.front();
        auto child1            = callParams->childs.back();
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicRealloc, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicMemCpy:
    {
        auto childDest = callParams->childs[0];
        auto childSrc  = callParams->childs[1];
        auto childSize = callParams->childs[2];
        emitInstruction(context, ByteCodeOp::MemCpy, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childSrc);
        freeRegisterRC(context, childSize);
        break;
    }
    case Intrinsic::IntrinsicMemSet:
    {
        auto childDest  = callParams->childs[0];
        auto childValue = callParams->childs[1];
        auto childSize  = callParams->childs[2];
        emitInstruction(context, ByteCodeOp::MemSet, childDest->resultRegisterRC, childValue->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childValue);
        freeRegisterRC(context, childSize);
        break;
    }
    case Intrinsic::IntrinsicMemCmp:
    {
        auto childDest         = callParams->childs[0];
        auto childValue        = callParams->childs[1];
        auto childSize         = callParams->childs[2];
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::MemCmp, node->resultRegisterRC, childDest->resultRegisterRC, childValue->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childValue);
        freeRegisterRC(context, childSize);
        break;
    }
    case Intrinsic::IntrinsicGetContext:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        SWAG_ASSERT(node->identifierRef == node->parent);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicGetContext, node->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicSetContext:
    {
        auto childDest = callParams->childs[0];
        emitInstruction(context, ByteCodeOp::IntrinsicSetContext, childDest->resultRegisterRC);
        freeRegisterRC(context, childDest);
    }
    case Intrinsic::IntrinsicArguments:
    {
        reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicArguments, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case Intrinsic::IntrinsicTarget:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicTarget, node->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicIsByteCode:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicIsByteCode, node->resultRegisterRC);
        break;
    }

    default:
        return internalError(context, "emitIntrinsic, unknown intrinsic");
    }

    return true;
}

bool ByteCodeGenJob::emitLambdaCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;

    SWAG_CHECK(emitIdentifier(context));
    node->additionalRegisterRC = node->resultRegisterRC;
    auto allParams             = node->childs.empty() ? nullptr : node->childs.front();
    SWAG_CHECK(emitCall(context, allParams, nullptr, (AstVarDecl*) overload->node, node->additionalRegisterRC, false));
    SWAG_ASSERT(context->result == ContextResult::Done);
    return true;
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     funcNode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    auto allParams = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_CHECK(emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRC, false));
    return true;
}

bool ByteCodeGenJob::emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList)
{
    auto node         = context->node;
    auto defaultParam = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
    SWAG_ASSERT(defaultParam->assignment);

    if (defaultParam->assignment->kind == AstNodeKind::CompilerSpecialFunction)
    {
        switch (defaultParam->assignment->token.id)
        {
        case TokenId::CompilerCallerLine:
        {
            reserveRegisterRC(context, regList, 1);
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList)->b.u32 = node->token.startLocation.line + 1;
            break;
        }
        case TokenId::CompilerCallerFile:
        {
            reserveLinearRegisterRC(context, regList, 2);
            auto str    = Utf8(node->sourceFile->path);
            auto offset = context->sourceFile->module->constantSegment.addString(str);
            SWAG_ASSERT(offset != UINT32_MAX);
            emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, regList[0], offset);
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList[1], (uint32_t) str.length());
            break;
        }
        case TokenId::CompilerCallerFunction:
        {
            reserveLinearRegisterRC(context, regList, 2);
            const auto& str    = node->ownerFct->scopedName;
            auto        offset = context->sourceFile->module->constantSegment.addString(str);
            SWAG_ASSERT(offset != UINT32_MAX);
            emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, regList[0], offset);
            emitInstruction(context, ByteCodeOp::CopyRAVB32, regList[1], (uint32_t) str.length());
            break;
        }
        default:
            return internalError(context, "emitLocalCall, invalid compiler function", defaultParam->assignment);
        }
    }
    else
    {
        SWAG_ASSERT(defaultParam->assignment->flags & AST_VALUE_COMPUTED);
        SWAG_CHECK(emitLiteral(context, defaultParam->assignment, defaultParam->typeInfo, regList));
    }

    return true;
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, RegisterList& varNodeRegisters, bool foreign)
{
    AstNode*          node         = context->node;
    TypeInfoFuncAttr* typeInfoFunc = nullptr;
    if (funcNode)
    {
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    }
    else
    {
        auto typeVar = TypeManager::concreteType(varNode->typeInfo, CONCRETE_ALIAS);
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::Lambda);
    }

    // Be sure referenced function has bytecode
    askForByteCode(context->job->dependentJob, context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
    if (context->result == ContextResult::Pending)
        return true;

    int precallStack  = 0;
    int numCallParams = allParams ? (int) allParams->childs.size() : 0;

    // If we are in a function that need to keep the RR0 register alive, we need to save it
    bool rr0Saved = false;
    if (node->ownerFct &&
        node->ownerFct->returnType &&
        node->ownerFct->returnType->typeInfo &&
        (node->ownerFct->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY))
    {
        emitInstruction(context, ByteCodeOp::PushRRSaved, 0);
        rr0Saved = true;
    }

    // Store in RR0 the address of the stack to store the result
    if (typeInfoFunc->returnType && (typeInfoFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY))
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto inst              = emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC);
        inst->b.u32            = node->fctCallStorageOffset;
        emitInstruction(context, ByteCodeOp::CopyRRxRCxCall, 0, node->resultRegisterRC);
        context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
    }

    // Sort childs by parameter index
    if (allParams && (allParams->flags & AST_MUST_SORT_CHILDS))
    {
        sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2) {
            AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
            AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
            return p1->index < p2->index;
        });
    }

    // For a untyped variadic, we need to store all parameters as 'any'
    RegisterList toFree;
    if (allParams && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
    {
        auto numFuncParams = (int) typeInfoFunc->parameters.size();
        auto numVariadic   = (uint32_t)(numCallParams - numFuncParams) + 1;
        int  offset        = numVariadic * 2 * sizeof(Register);
        for (int i = (int) numCallParams - 1; i >= numFuncParams - 1; i--)
        {
            auto child     = allParams->childs[i];
            auto typeParam = child->typeInfo;

            // Be sure to point to the first register of the type, if it has many
            offset += (typeParam->numRegisters() - 1) * sizeof(Register);

            ByteCodeInstruction* inst;

            // Store concrete type info
            auto r0 = reserveRegisterRC(context);
            toFree += r0;
            SWAG_ASSERT(child->concreteTypeInfoStorage != UINT32_MAX);
            emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, r0)->b.u64 = child->concreteTypeInfoStorage;
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);

            if (typeParam->kind == TypeInfoKind::Struct)
            {
                // For a struct (and not a pointer to struct), we directly set the data pointer in the 'any' instead
                // of pushing it to the stack.
                emitInstruction(context, ByteCodeOp::PushRAParam, child->resultRegisterRC[0]);
            }
            else
            {
                auto r1 = reserveRegisterRC(context);
                toFree += r1;

                // The value will be stored on the stack (1 or 2 registers max). So we push now the address
                // of that value on that stack. This is the data part of the 'any'
                // Store address of value on the stack
                inst        = emitInstruction(context, ByteCodeOp::MovRASP, r1);
                inst->b.u32 = offset;
                inst->c.u32 = child->resultRegisterRC[0];
                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
            }

            precallStack += 2 * sizeof(Register);
            offset -= 2 * sizeof(Register);
            offset += sizeof(Register);
        }
    }

    // Push missing default parameters
    uint64_t numPushParams = 0;
    int      numTypeParams = (int) typeInfoFunc->parameters.size();
    if (numCallParams < numTypeParams)
    {
        // Push all parameters, from end to start
        for (int i = numTypeParams - 1; i >= 0; i--)
        {
            // Determine if this parameter has been covered by the call
            bool covered = false;
            for (int j = 0; j < numCallParams; j++)
            {
                auto param = static_cast<AstFuncCallParam*>(allParams->childs[j]);
                if (param->index == i)
                {
                    toFree += param->resultRegisterRC;
                    for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
                    {
                        emitInstruction(context, ByteCodeOp::PushRAParam, param->resultRegisterRC[r]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                    }

                    covered = true;
                    break;
                }
            }

            // If not covered, then this is a default value
            if (!covered)
            {
                // funcnode can be null in case of a lambda, so we need to retreive the function description from the type
                auto funcDescription = funcNode;
                if (!funcDescription)
                {
                    SWAG_ASSERT(typeInfoFunc->declNode);
                    funcDescription = CastAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                }

                auto defaultParam = CastAst<AstVarDecl>(funcDescription->parameters->childs[i], AstNodeKind::FuncDeclParam);

                // Empty variadic parameter
                if (defaultParam->typeInfo->kind != TypeInfoKind::Variadic && defaultParam->typeInfo->kind != TypeInfoKind::TypedVariadic)
                {
                    SWAG_ASSERT(defaultParam->assignment);

                    RegisterList regList;
                    SWAG_CHECK(emitDefaultParamValue(context, defaultParam, regList));

                    toFree += regList;
                    for (int r = regList.size() - 1; r >= 0; r--)
                    {
                        emitInstruction(context, ByteCodeOp::PushRAParam, regList[r]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                    }
                }
            }
        }
    }

    // Fast call. No need to do fancy things, all the parameters are covered by the call
    else if (numCallParams)
    {
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = allParams->childs[i];
            if (param->typeInfo && (param->typeInfo->kind == TypeInfoKind::Variadic || param->typeInfo->kind == TypeInfoKind::TypedVariadic))
            {
                SWAG_ASSERT(i == numCallParams - 1);
            }
            else
            {
                toFree += param->resultRegisterRC;
                for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
                {
                    emitInstruction(context, ByteCodeOp::PushRAParam, param->resultRegisterRC[r]);
                    precallStack += sizeof(Register);
                    numPushParams++;
                }
            }
        }
    }

    // Pass a variadic parameter to another function
    auto lastParam = allParams && !allParams->childs.empty() ? allParams->childs.back() : nullptr;
    if (lastParam && lastParam->typeInfo && ((lastParam->typeInfo->kind == TypeInfoKind::TypedVariadic) || (lastParam->typeInfo->kind == TypeInfoKind::Variadic)))
    {
        RegisterList r0;
        reserveRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], lastParam->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef64, r0[0]);
        emitInstruction(context, ByteCodeOp::IncPointerVB, lastParam->resultRegisterRC)->b.s32 = -8;
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[1], lastParam->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef64, r0[1]);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0[0]);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0[1]);
        toFree += r0;
        precallStack += 2 * sizeof(Register);
        numPushParams += 2;
    }

    // Variadic parameter is on top of stack
    else
    {
        if (typeInfoFunc->flags & TYPEINFO_VARIADIC)
        {
            auto numVariadic = (uint32_t)(numCallParams - numTypeParams) + 1;

            // Store number of extra parameters
            auto r0 = reserveRegisterRC(context);
            toFree += r0;
            emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.u64 = numVariadic | ((numPushParams + 1) << 32);
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);

            // Store address on the stack of those parameters. This must be the last push
            auto r1 = reserveRegisterRC(context);
            toFree += r1;
            emitInstruction(context, ByteCodeOp::MovRASPVaargs, r1);
            emitInstruction(context, ByteCodeOp::PushRAParam, r1);

            precallStack += 2 * sizeof(Register);
        }
        else if (typeInfoFunc->flags & TYPEINFO_TYPED_VARIADIC)
        {
            auto numVariadic  = (uint32_t)(numCallParams - numTypeParams) + 1;
            auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
            auto offset       = (numPushParams - numVariadic * typeVariadic->rawType->numRegisters()) + 1;

            // Store number of extra parameters
            auto r0 = reserveRegisterRC(context);
            toFree += r0;
            emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.u64 = numVariadic | (offset << 32);
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);

            // Store address on the stack of those parameters. This must be the last push
            auto r1 = reserveRegisterRC(context);
            toFree += r1;
            emitInstruction(context, ByteCodeOp::MovRASPVaargs, r1);
            emitInstruction(context, ByteCodeOp::PushRAParam, r1);

            precallStack += 2 * sizeof(Register);
        }
    }

    // Free all registers now that the call can really be done
    freeRegisterRC(context, toFree);

    if (foreign)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::ForeignCall);
        inst->a.pointer = (uint8_t*) funcNode;
        inst->b.pointer = (uint8_t*) typeInfoFunc;
        funcNode->flags |= AST_USED_FOREIGN;
    }
    else if (funcNode)
    {
        auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(funcNode->bc);
        inst->a.pointer = (uint8_t*) funcNode->bc;
        inst->b.pointer = (uint8_t*) typeInfoFunc;
    }
    else
    {
        SWAG_ASSERT(varNodeRegisters.size() > 0);
        auto inst       = emitInstruction(context, ByteCodeOp::LambdaCall, varNodeRegisters);
        inst->b.pointer = (uint8_t*) typeInfoFunc;
    }

    // Copy result in a computing register
    if (typeInfoFunc->returnType && typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        if (!(typeInfoFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            auto numRegs = typeInfoFunc->returnType->numRegisters();
            reserveRegisterRC(context, node->resultRegisterRC, numRegs);
            context->bc->maxCallResults = max(context->bc->maxCallResults, numRegs);
            for (int idx = 0; idx < numRegs; idx++)
                emitInstruction(context, ByteCodeOp::CopyRCxRRxCall, node->resultRegisterRC[idx], idx);
        }
    }

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        emitInstruction(context, ByteCodeOp::IncSP, precallStack);
    }

    // If we are in a function that need to keep the RR0 register alive, we need to restore it
    if (rr0Saved)
    {
        emitInstruction(context, ByteCodeOp::PopRRSaved, 0);
    }

    // This is usefull when function call is inside an expression like func().something
    // The emitIdentifier will have to know the register where the result of func() is stored
    node->parent->resultRegisterRC = node->resultRegisterRC;

    return true;
}

bool ByteCodeGenJob::emitFuncDeclParams(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);

    // 3 pointers are already on that stack after BP : saved BP, BC and IP.
    int offset = 3 * sizeof(void*);

    // Then add the full stack size of the function
    offset += funcNode->stackSize;

    // Variadic parameter is the last one pushed on the stack
    int storageIndex = 0;
    if (funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto param              = node->childs.back();
        auto resolved           = param->resolvedSymbolOverload;
        resolved->storageOffset = offset;
        offset += g_TypeMgr.typeInfoVariadic->sizeOf;
        resolved->storageIndex = 0; // Always the first one
        storageIndex += 2;
    }

    auto childSize = node->childs.size();
    for (int i = 0; i < childSize; i++)
    {
        if ((i == childSize - 1) && funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
            break;
        auto param              = node->childs[i];
        auto resolved           = param->resolvedSymbolOverload;
        resolved->storageOffset = offset;
        resolved->storageIndex  = storageIndex;

        auto typeInfo     = TypeManager::concreteType(resolved->typeInfo);
        int  numRegisters = typeInfo->numRegisters();
        offset += numRegisters * sizeof(Register);
        storageIndex += numRegisters;
    }

    return true;
}

bool ByteCodeGenJob::emitBeforeFuncDeclContent(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);

    if (funcNode->stackSize)
    {
        auto inst   = emitInstruction(context, ByteCodeOp::DecSP);
        inst->a.u32 = funcNode->stackSize;
        emitInstruction(context, ByteCodeOp::MovSPBP);
    }

    return true;
}

bool ByteCodeGenJob::emitForeignCall(ByteCodeGenContext* context)
{
    AstNode* node      = context->node;
    auto     overload  = node->resolvedSymbolOverload;
    auto     funcNode  = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    auto     allParams = node->childs.empty() ? nullptr : node->childs.front();

    emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRC, true);
    return true;
}
