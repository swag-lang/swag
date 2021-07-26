#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"
#include "Runtime.h"
#include "SemanticJob.h"
#include "TypeTable.h"
#include "ErrorIds.h"

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto         funcDecl = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    PushLocation pl(context, &funcDecl->content->token.endLocation);
    PushNode     pn(context, funcDecl->content);

    // No need to do the scope leave stuff if the function does return something, because
    // it has been covered by the mandatory return
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        return true;

    SWAG_CHECK(emitLeaveScope(context, funcDecl->scope));
    if (context->result != ContextResult::Done)
        return true;

    emitInstruction(context, ByteCodeOp::Ret)->a.u32 = funcDecl->stackSize;
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
    TypeInfo*  returnType = nullptr;

    // Get the function return type. In case of an emmbedded return, this is the type of the original function to inline
    if (node->ownerInline && (node->semFlags & AST_SEM_EMBEDDED_RETURN))
        returnType = TypeManager::concreteType(node->ownerInline->func->returnType->typeInfo, CONCRETE_ALIAS);
    else
        returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_ALIAS);

    // Copy result to RR0... registers
    if (!(node->doneFlags & AST_DONE_EMIT_DEFERRED) && !node->childs.empty() && !returnType->isNative(NativeTypeKind::Void))
    {
        auto returnExpression = node->childs.front();
        auto backExpression   = node->childs.back();
        if (backExpression->kind == AstNodeKind::Try || backExpression->kind == AstNodeKind::Catch)
            backExpression = backExpression->childs.back();
        auto exprType = TypeManager::concreteReference(returnExpression->typeInfo);

        // Implicit cast
        if (!(returnExpression->doneFlags & AST_DONE_CAST1))
        {
            SWAG_CHECK(emitCast(context, returnExpression, TypeManager::concreteType(returnExpression->typeInfo), returnExpression->castedTypeInfo));
            if (context->result != ContextResult::Done)
                return true;
            returnExpression->doneFlags |= AST_DONE_CAST1;
        }

        context->job->waitStructGenerated(exprType);
        if (context->result != ContextResult::Done)
            return true;

        //
        // RETVAL
        //
        if ((node->doneFlags & AST_DONE_RETVAL) ||
            (backExpression->resolvedSymbolOverload && backExpression->resolvedSymbolOverload->flags & OVERLOAD_RETVAL))
        {
            auto child = node->childs.front();
            freeRegisterRC(context, child->resultRegisterRC);
        }

        //
        // INLINE
        //
        else if (node->ownerInline && (node->semFlags & AST_SEM_EMBEDDED_RETURN))
        {
            if (returnType->kind == TypeInfoKind::Struct)
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                SWAG_CHECK(emitCopyStruct(context, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC, exprType, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else if (returnType->kind == TypeInfoKind::Array)
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                SWAG_CHECK(emitCopyArray(context, returnType, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else
            {
                for (auto child : node->childs)
                {
                    auto sizeChilds = child->resultRegisterRC.size();
                    for (int r = 0; r < sizeChilds; r++)
                        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->ownerInline->resultRegisterRC[r], child->resultRegisterRC[r]);
                    freeRegisterRC(context, child);
                }
            }
        }

        //
        // NORMAL
        //
        else
        {
            if (returnType->kind == TypeInfoKind::Struct)
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                RegisterList r0 = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, r0);
                SWAG_CHECK(emitCopyStruct(context, r0, returnExpression->resultRegisterRC, exprType, returnExpression));
                freeRegisterRC(context, r0);
                freeRegisterRC(context, returnExpression->resultRegisterRC);
            }
            else if (returnType->kind == TypeInfoKind::Array)
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                RegisterList r1 = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, r1);
                SWAG_CHECK(emitCopyArray(context, returnType, r1, returnExpression->resultRegisterRC, returnExpression));
                freeRegisterRC(context, r1);
                freeRegisterRC(context, returnExpression->resultRegisterRC);
            }
            else if (returnType->isNative(NativeTypeKind::String))
            {
                auto child = node->childs.front();
                if (funcNode->attributeFlags & ATTRIBUTE_AST_FUNC)
                    emitInstruction(context, ByteCodeOp::CloneString, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                emitInstruction(context, ByteCodeOp::CopyRCtoRR2, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                freeRegisterRC(context, child->resultRegisterRC);
            }
            else
            {
                SWAG_ASSERT(node->childs.size() == 1);
                auto child = node->childs.front();
                SWAG_ASSERT(child->resultRegisterRC.size() >= returnType->numRegisters());
                auto numRetReg = returnType->numRegisters();
                SWAG_ASSERT(numRetReg <= 2);
                if (numRetReg == 1)
                    emitInstruction(context, ByteCodeOp::CopyRCtoRR, child->resultRegisterRC[0]);
                else if (numRetReg == 2)
                    emitInstruction(context, ByteCodeOp::CopyRCtoRR2, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                freeRegisterRC(context, child->resultRegisterRC);
            }
        }
    }

    node->doneFlags |= AST_DONE_EMIT_DEFERRED;

    // Leave all scopes
    SWAG_CHECK(emitLeaveScopeReturn(context, &node->forceNoDrop, false));
    if (context->result != ContextResult::Done)
        return true;

    // A return inside an inline function is just a jump to the end of the block
    if (node->ownerInline && (node->semFlags & AST_SEM_EMBEDDED_RETURN))
    {
        node->seekJump = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::Jump);
        node->ownerInline->returnList.push_back(node);
    }
    else
    {
        emitInstruction(context, ByteCodeOp::Ret)->a.u32 = funcNode->stackSize;
    }

    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstIdentifier>(context->node, AstNodeKind::FuncCall);
    auto callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParams);

    // If the intrinsic is defined in runtime, then need to wait for the function bytecode
    // to be generated
    if (node->resolvedSymbolOverload->node->flags & AST_DEFINED_INTRINSIC)
    {
        askForByteCode(context->job, node->resolvedSymbolOverload->node, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Some safety checks depending on the intrinsic
    switch (node->token.id)
    {
    case TokenId::IntrinsicAbs:
        emitSafetyNeg(context, callParams->childs[0]->resultRegisterRC, TypeManager::concreteReferenceType(callParams->childs[0]->typeInfo), true);
        break;
    }

    switch (node->token.id)
    {
    case TokenId::IntrinsicErrorMsg:
    {
        auto child0 = callParams->childs.front();
        auto child1 = callParams->childs.back();
        emitInstruction(context, ByteCodeOp::IntrinsicErrorMsg, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicPanic:
    {
        auto child0 = callParams->childs.front();
        auto child1 = callParams->childs.back();
        emitInstruction(context, ByteCodeOp::IntrinsicPanic, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicAssert:
    {
        auto child0 = callParams->childs.front();
        emitAssert(context, child0->resultRegisterRC, "assertion failed");
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicBcDbg:
    {
        emitInstruction(context, ByteCodeOp::IntrinsicBcDbg);
        break;
    }
    case TokenId::IntrinsicAlloc:
    {
        auto child0            = callParams->childs.front();
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicAlloc, node->resultRegisterRC, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicFree:
    {
        auto child0 = callParams->childs.front();
        emitInstruction(context, ByteCodeOp::IntrinsicFree, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicRealloc:
    {
        auto child0            = callParams->childs.front();
        auto child1            = callParams->childs.back();
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicRealloc, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicMemCpy:
    {
        auto childDest = callParams->childs[0];
        auto childSrc  = callParams->childs[1];
        auto childSize = callParams->childs[2];
        emitSafetyNullPointer(context, childDest->resultRegisterRC, Msg0526);
        emitSafetyNullPointer(context, childSrc->resultRegisterRC, Msg0527);
        emitInstruction(context, ByteCodeOp::IntrinsicMemCpy, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childSrc);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicMemMove:
    {
        auto childDest = callParams->childs[0];
        auto childSrc  = callParams->childs[1];
        auto childSize = callParams->childs[2];
        emitSafetyNullPointer(context, childDest->resultRegisterRC, Msg0528);
        emitSafetyNullPointer(context, childSrc->resultRegisterRC, Msg0529);
        emitInstruction(context, ByteCodeOp::IntrinsicMemMove, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childSrc);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicMemSet:
    {
        auto childDest  = callParams->childs[0];
        auto childValue = callParams->childs[1];
        auto childSize  = callParams->childs[2];
        emitSafetyNullPointer(context, childDest->resultRegisterRC, Msg0530);
        emitInstruction(context, ByteCodeOp::IntrinsicMemSet, childDest->resultRegisterRC, childValue->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childValue);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicMemCmp:
    {
        auto childDest = callParams->childs[0];
        auto childSrc  = callParams->childs[1];
        auto childSize = callParams->childs[2];
        emitSafetyNullPointer(context, childDest->resultRegisterRC, Msg0531);
        emitSafetyNullPointer(context, childSrc->resultRegisterRC, Msg0532);
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicMemCmp, node->resultRegisterRC, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childSrc);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicCStrLen:
    {
        auto childSrc = callParams->childs[0];
        emitSafetyNullPointer(context, childSrc->resultRegisterRC, Msg0533);
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicCStrLen, node->resultRegisterRC, childSrc->resultRegisterRC);
        freeRegisterRC(context, childSrc);
        break;
    }
    case TokenId::IntrinsicTypeCmp:
    {
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto child2            = callParams->childs[2];
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        freeRegisterRC(context, child2);
        break;
    }
    case TokenId::IntrinsicStrCmp:
    {
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        node->resultRegisterRC = child1->resultRegisterRC[1];
        emitInstruction(context, ByteCodeOp::IntrinsicStrCmp, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC[0], child1->resultRegisterRC[1]);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1->resultRegisterRC[0]);
        break;
    }
    case TokenId::IntrinsicSetErr:
    {
        auto child0 = callParams->childs[0];
        emitInstruction(context, ByteCodeOp::IntrinsicSetErr, child0->resultRegisterRC[0], child0->resultRegisterRC[1]);
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicGetContext:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        SWAG_ASSERT(node->identifierRef == node->parent);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicGetContext, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicSetContext:
    {
        auto childDest = callParams->childs[0];
        emitInstruction(context, ByteCodeOp::IntrinsicSetContext, childDest->resultRegisterRC);
        freeRegisterRC(context, childDest);
        break;
    }
    case TokenId::IntrinsicArguments:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicArguments, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicCompiler:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicCompiler, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicIsByteCode:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::IntrinsicIsByteCode, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicAtomicAdd:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAddS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAddS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAddS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAddS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicAdd invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicAtomicAnd:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAndS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAndS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAndS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicAndS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicAnd invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicAtomicOr:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicOrS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicOrS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicOrS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicOrS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicOr invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicAtomicXor:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXorS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXorS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXorS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXorS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicXor invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }

    case TokenId::IntrinsicAtomicXchg:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXchgS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXchgS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXchgS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicXchgS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicXchg invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }

    case TokenId::IntrinsicAtomicCmpXchg:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto child2            = callParams->childs[2];
        auto typeInfo          = TypeManager::concreteReferenceType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicCmpXchgS8, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicCmpXchgS16, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicCmpXchgS32, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::IntrinsicAtomicCmpXchgS64, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicAtomicCmpXchg invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        freeRegisterRC(context, child2);
        break;
    }

    case TokenId::IntrinsicPow:
    case TokenId::IntrinsicATan2:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        auto child0                           = callParams->childs[0];
        auto child1                           = callParams->childs[1];
        auto typeInfo                         = TypeManager::concreteReferenceType(child0->typeInfo);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicF32x2;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicF64x2;
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicPow invalid type");
        }

        auto inst   = emitInstruction(context, op, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->token.id;
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }

    case TokenId::IntrinsicBitCountNz:
    case TokenId::IntrinsicBitCountTz:
    case TokenId::IntrinsicBitCountLz:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        auto child                            = callParams->childs[0];
        auto typeInfo                         = TypeManager::concreteReferenceType(child->typeInfo);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            op = ByteCodeOp::IntrinsicS8x1;
            break;
        case NativeTypeKind::U16:
            op = ByteCodeOp::IntrinsicS16x1;
            break;
        case NativeTypeKind::U32:
            op = ByteCodeOp::IntrinsicS32x1;
            break;
        case NativeTypeKind::U64:
            op = ByteCodeOp::IntrinsicS64x1;
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicBitCount invalid type");
        }

        auto inst   = emitInstruction(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->token.id;
        freeRegisterRC(context, child);
        break;
    }

    case TokenId::IntrinsicByteSwap:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        auto child                            = callParams->childs[0];
        auto typeInfo                         = TypeManager::concreteReferenceType(child->typeInfo);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U16:
            op = ByteCodeOp::IntrinsicS16x1;
            break;
        case NativeTypeKind::U32:
            op = ByteCodeOp::IntrinsicS32x1;
            break;
        case NativeTypeKind::U64:
            op = ByteCodeOp::IntrinsicS64x1;
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicByteSwap invalid type");
        }

        auto inst   = emitInstruction(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->token.id;
        freeRegisterRC(context, child);
        break;
    }

    case TokenId::IntrinsicMin:
    case TokenId::IntrinsicMax:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        auto child0                           = callParams->childs[0];
        auto child1                           = callParams->childs[1];
        auto typeInfo                         = TypeManager::concreteReferenceType(child0->typeInfo);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            op = ByteCodeOp::IntrinsicS8x2;
            break;
        case NativeTypeKind::S16:
            op = ByteCodeOp::IntrinsicS16x2;
            break;
        case NativeTypeKind::S32:
            op = ByteCodeOp::IntrinsicS32x2;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            op = ByteCodeOp::IntrinsicS64x2;
            break;
        case NativeTypeKind::U8:
            op = ByteCodeOp::IntrinsicU8x2;
            break;
        case NativeTypeKind::U16:
            op = ByteCodeOp::IntrinsicU16x2;
            break;
        case NativeTypeKind::U32:
            op = ByteCodeOp::IntrinsicU32x2;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            op = ByteCodeOp::IntrinsicU64x2;
            break;
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicF32x2;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicF64x2;
            break;
        default:
            return internalError(context, "emitIntrinsic, IntrinsicMin/Max invalid type");
        }

        auto inst   = emitInstruction(context, op, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->token.id;
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }

    case TokenId::IntrinsicAbs:
    case TokenId::IntrinsicSqrt:
    case TokenId::IntrinsicSin:
    case TokenId::IntrinsicCos:
    case TokenId::IntrinsicTan:
    case TokenId::IntrinsicSinh:
    case TokenId::IntrinsicCosh:
    case TokenId::IntrinsicTanh:
    case TokenId::IntrinsicASin:
    case TokenId::IntrinsicACos:
    case TokenId::IntrinsicATan:
    case TokenId::IntrinsicLog:
    case TokenId::IntrinsicLog2:
    case TokenId::IntrinsicLog10:
    case TokenId::IntrinsicFloor:
    case TokenId::IntrinsicCeil:
    case TokenId::IntrinsicTrunc:
    case TokenId::IntrinsicRound:
    case TokenId::IntrinsicExp:
    case TokenId::IntrinsicExp2:
    {
        node->resultRegisterRC                = reserveRegisterRC(context);
        node->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC        = node->resultRegisterRC;
        auto child                            = callParams->childs[0];
        auto typeInfo                         = TypeManager::concreteReferenceType(child->typeInfo);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            op = ByteCodeOp::IntrinsicS8x1;
            break;
        case NativeTypeKind::S16:
            op = ByteCodeOp::IntrinsicS16x1;
            break;
        case NativeTypeKind::S32:
            op = ByteCodeOp::IntrinsicS32x1;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            op = ByteCodeOp::IntrinsicS64x1;
            break;
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicF32x1;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicF64x1;
            break;
        default:
            return internalError(context, "emitIntrinsic, math intrinsic invalid type");
        }

        auto inst   = emitInstruction(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->token.id;
        freeRegisterRC(context, child);
        break;
    }

    default:
        return internalError(context, "emitIntrinsic, unknown intrinsic");
    }

    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, node->resultRegisterRC);

    return true;
}

bool ByteCodeGenJob::emitLambdaCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;

    SWAG_CHECK(emitIdentifier(context));
    node->additionalRegisterRC = node->resultRegisterRC;
    auto allParams             = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_ASSERT(!allParams || allParams->kind == AstNodeKind::FuncCallParams);

    SWAG_CHECK(emitCall(context, allParams, nullptr, (AstVarDecl*) overload->node, node->additionalRegisterRC, false, true, true));
    SWAG_ASSERT(context->result == ContextResult::Done);
    freeRegisterRC(context, node->additionalRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     funcNode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    auto allParams = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_ASSERT(!allParams || allParams->kind == AstNodeKind::FuncCallParams);
    SWAG_CHECK(emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRC, false, false, true));
    return true;
}

void ByteCodeGenJob::computeSourceLocation(JobContext* context, AstNode* node, uint32_t* storageOffset, DataSegment** storageSegment)
{
    auto seg        = SemanticJob::getConstantSegFromContext(context->node);
    *storageSegment = seg;

    auto sourceFile = node->sourceFile;
    auto str        = Utf8(Utf8::normalizePath(sourceFile->path));
    auto offset     = seg->reserve(sizeof(SwagCompilerSourceLocation), sizeof(void*));
    auto loc        = (SwagCompilerSourceLocation*) seg->address(offset);
    auto offsetName = seg->addString(str);
    auto addrName   = seg->address(offsetName);
    seg->addInitPtr(offset, offsetName);
    loc->fileName.buffer = addrName;
    loc->fileName.count  = str.length();
    loc->lineStart       = node->token.startLocation.line;
    loc->colStart        = node->token.startLocation.column;
    loc->lineEnd         = node->token.endLocation.line;
    loc->colEnd          = node->token.endLocation.column;
    *storageOffset       = offset;
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
        case TokenId::CompilerCallerLocation:
        {
            regList = reserveRegisterRC(context);
            uint32_t     storageOffset;
            DataSegment* storageSegment;
            computeSourceLocation(context, node, &storageOffset, &storageSegment);
            emitMakeSegPointer(context, storageSegment, regList[0], storageOffset);
            break;
        }
        case TokenId::CompilerCallerFunction:
        {
            reserveLinearRegisterRC2(context, regList);
            const auto& str            = node->ownerFct->getNameForUserCompiler();
            auto        storageSegment = SemanticJob::getConstantSegFromContext(context->node);
            auto        storageOffset  = storageSegment->addString(str);
            SWAG_ASSERT(storageOffset != UINT32_MAX);
            emitMakeSegPointer(context, storageSegment, regList[0], storageOffset);
            emitInstruction(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = str.length();
            break;
        }
        default:
            return internalError(context, "emitDefaultParamValue, invalid compiler function", defaultParam->assignment);
        }

        return true;
    }

    SWAG_ASSERT(defaultParam->assignment->flags & AST_VALUE_COMPUTED);
    SWAG_CHECK(emitLiteral(context, defaultParam->assignment, defaultParam->typeInfo, regList));
    return true;
}

void ByteCodeGenJob::emitPushRAParams(ByteCodeGenContext* context, VectorNative<uint32_t>& accParams)
{
    int cpt = (int) accParams.size();
    int i   = 0;
    while (i < cpt)
    {
        if (cpt - i >= 4)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam4, accParams[i], accParams[i + 1], accParams[i + 2], accParams[i + 3]);
            i += 4;
        }
        else if (cpt - i >= 3)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam3, accParams[i], accParams[i + 1], accParams[i + 2]);
            i += 3;
        }
        else if (cpt - i >= 2)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam2, accParams[i], accParams[i + 1]);
            i += 2;
        }
        else
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, accParams[i]);
            i += 1;
        }
    }

    accParams.clear();
}

bool ByteCodeGenJob::checkCatchError(ByteCodeGenContext* context, AstNode* callNode, AstNode* funcNode, AstNode* parent, TypeInfo* typeInfoFunc)
{
    bool raiseErrors = typeInfoFunc->flags & TYPEINFO_CAN_THROW;
    if (raiseErrors && !callNode->ownerTryCatchAssume)
        return context->report({callNode, callNode->token, Utf8::format(Msg0534, funcNode->token.text.c_str())});

    if (!raiseErrors)
    {
        if (parent->kind == AstNodeKind::Try ||
            parent->kind == AstNodeKind::Catch ||
            parent->kind == AstNodeKind::Assume)
        {
            return context->report({parent, Utf8::format(Msg0535, parent->token.text.c_str())});
        }
    }

    return true;
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, RegisterList& varNodeRegisters, bool foreign, bool lambda, bool freeRegistersParams)
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
    askForByteCode(context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
    if (context->result == ContextResult::Pending)
        return true;

    // Error, check validity.
    SWAG_CHECK(checkCatchError(context, node, node, node->parent->parent, typeInfoFunc));

    int precallStack  = 0;
    int numCallParams = allParams ? (int) allParams->childs.size() : 0;

    // If we are in a function that need to keep the RR0 register alive, we need to save it
    bool rr0Saved = false;
    if (node->ownerFct &&
        node->ownerFct->returnType &&
        node->ownerFct->returnType->typeInfo &&
        (node->ownerFct->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY))
    {
        emitInstruction(context, ByteCodeOp::PushRR);
        rr0Saved = true;
    }

    auto returnType = typeInfoFunc->returnType;
    if (returnType && (returnType->flags & TYPEINFO_RETURN_BY_COPY))
    {
        auto testReturn = node;
        if (testReturn->kind == AstNodeKind::Identifier)
            testReturn = testReturn->parent;

        // If in a return expression, just push the caller retval
        AstNode* parentReturn = testReturn ? testReturn->inSimpleReturn() : nullptr;
        if (parentReturn)
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            if (node->ownerInline)
                emitInstruction(context, ByteCodeOp::CopyRCtoRT, node->ownerInline->resultRegisterRC);
            else
            {
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, node->resultRegisterRC);
                emitInstruction(context, ByteCodeOp::CopyRCtoRT, node->resultRegisterRC);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->doneFlags |= AST_DONE_RETVAL;
        }
        else
        {
            // Store in RR0 the address of the stack to store the result
            node->resultRegisterRC = reserveRegisterRC(context);
            auto inst              = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64            = node->computedValue.storageOffset;
            emitInstruction(context, ByteCodeOp::CopyRCtoRT, node->resultRegisterRC);
            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);

            if (node->resolvedSymbolOverload)
                node->resolvedSymbolOverload->flags |= OVERLOAD_EMITTED;

            node->ownerScope->symTable.addVarToDrop(node->resolvedSymbolOverload, typeInfoFunc->returnType, node->computedValue.storageOffset);

            if (node->flags & AST_DISCARD)
                freeRegisterRC(context, node->resultRegisterRC);
        }
    }

    uint32_t maxCallParams = typeInfoFunc->numReturnRegisters();

    // Sort childs by parameter index
    if (allParams && (allParams->flags & AST_MUST_SORT_CHILDS))
    {
        sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2) {
            AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
            AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
            return p1->indexParam < p2->indexParam;
        });
    }
    else if (allParams && (allParams->semFlags & AST_SEM_INVERSE_PARAMS))
    {
        SWAG_ASSERT(allParams->childs.size() == 2);
        allParams->swap2Childs();
    }

    // For a untyped variadic, we need to store all parameters as 'any'
    VectorNative<uint32_t> toFree;
    if (allParams && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
    {
        auto numFuncParams = (int) typeInfoFunc->parameters.size();
        auto numVariadic   = (uint32_t)(numCallParams - numFuncParams) + 1;
        int  offset        = numVariadic * 2 * sizeof(Register);
        for (int i = (int) numCallParams - 1; i >= numFuncParams - 1; i--)
        {
            auto child     = allParams->childs[i];
            auto typeParam = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);

            // Be sure to point to the first register of the type, if it has many
            offset += (typeParam->numRegisters() - 1) * sizeof(Register);

            // Store concrete type info
            auto r0 = reserveRegisterRC(context);
            toFree.push_back(r0);

            // If this is a reference, then we push the type pointed by it, so that the user will receive a real type,
            // and not a reference to a type
            if (typeParam->kind == TypeInfoKind::Reference)
            {
                auto segType = child->concreteTypeInfoSegment;
                SWAG_ASSERT(segType);
                auto typeRef   = (ConcreteTypeInfoReference*) segType->address(child->concreteTypeInfoStorage);
                auto offsetRef = segType->offset((uint8_t*) typeRef->pointedType);
                emitMakeSegPointer(context, segType, r0, offsetRef);
            }
            else
            {
                emitMakeSegPointer(context, child->concreteTypeInfoSegment, r0, child->concreteTypeInfoStorage);
            }

            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            maxCallParams++;

            // For a big data, or a reference, we directly set the data pointer in the 'any' instead
            // of pushing it to the stack.
            if ((typeParam->flags & TYPEINFO_RETURN_BY_COPY) || typeParam->kind == TypeInfoKind::Reference)
            {
                emitInstruction(context, ByteCodeOp::PushRAParam, child->resultRegisterRC[0]);
                maxCallParams++;
            }
            else
            {
                auto r1 = reserveRegisterRC(context);
                toFree.push_back(r1);

                // The value will be stored on the stack (1 or 2 registers max). So we push now the address
                // of that value on that stack. This is the data part of the 'any'
                // Store address of value on the stack
                auto inst   = emitInstruction(context, ByteCodeOp::CopySP, r1);
                inst->b.u64 = offset;
                inst->c.u64 = child->resultRegisterRC[0];
                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
                maxCallParams++;
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
        VectorNative<uint32_t> accParams;
        for (int i = numTypeParams - 1; i >= 0; i--)
        {
            // Determine if this parameter has been covered by the call
            bool covered = false;
            for (int j = 0; j < numCallParams; j++)
            {
                auto param = static_cast<AstFuncCallParam*>(allParams->childs[j]);
                if (param->indexParam == i)
                {
                    if (param->additionalRegisterRC.canFree)
                    {
                        for (int r = 0; freeRegistersParams && r < param->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->additionalRegisterRC[r]);
                    }

                    for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
                    {
                        if (freeRegistersParams && param->resultRegisterRC.canFree)
                            toFree.push_back(param->resultRegisterRC[r]);
                        accParams.push_back(param->resultRegisterRC[r]);
                        maxCallParams++;
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
                // funcnode can be null in case of a lambda, so we need to retrieve the function description from the type
                auto funcDescription = funcNode;
                if (!funcDescription)
                {
                    SWAG_ASSERT(typeInfoFunc->declNode);
                    funcDescription = CastAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                }

                // Empty variadic parameter
                auto defaultParam = CastAst<AstVarDecl>(funcDescription->parameters->childs[i], AstNodeKind::FuncDeclParam);
                if (defaultParam->typeInfo->kind != TypeInfoKind::Variadic && defaultParam->typeInfo->kind != TypeInfoKind::TypedVariadic)
                {
                    SWAG_ASSERT(defaultParam->assignment);

                    RegisterList regList;
                    SWAG_CHECK(emitDefaultParamValue(context, defaultParam, regList));

                    for (int r = regList.size() - 1; r >= 0;)
                    {
                        if (regList.canFree)
                            toFree.push_back(regList[r]);
                        accParams.push_back(regList[r--]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                        maxCallParams++;
                    }
                }
            }
        }

        emitPushRAParams(context, accParams);
    }

    // Fast call. No need to do fancy things, all the parameters are covered by the call
    else if (numCallParams)
    {
        // Get the last variadic real type
        TypeInfo* typeRawVariadic = nullptr;
        if (typeInfoFunc->parameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
            typeRawVariadic   = typeVariadic->rawType;
        }

        VectorNative<uint32_t> accParams;
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = allParams->childs[i];
            if (param->resultRegisterRC.size() == 0)
                continue;
            if (!param->typeInfo)
                continue;

            if (param->typeInfo->kind != TypeInfoKind::Variadic && param->typeInfo->kind != TypeInfoKind::TypedVariadic && !(param->typeInfo->flags & TYPEINFO_SPREAD))
            {
                // If we push a something to a typed variadic, we need to push PushRVParam and not PushRAParam if the size
                // is less than a register, because we want the typed variadic to be a real slice (not always a slice of registers)
                bool done = false;
                if (typeRawVariadic && i >= numTypeParams - 1)
                {
                    if (typeRawVariadic->kind == TypeInfoKind::Native && typeRawVariadic->sizeOf < sizeof(Register))
                    {
                        done = true;
                        if (param->additionalRegisterRC.canFree)
                        {
                            for (int r = 0; freeRegistersParams && r < param->additionalRegisterRC.size(); r++)
                                toFree.push_back(param->additionalRegisterRC[r]);
                        }

                        for (int r = param->resultRegisterRC.size() - 1; r >= 0;)
                        {
                            if (freeRegistersParams && param->resultRegisterRC.canFree)
                                toFree.push_back(param->resultRegisterRC[r]);
                            auto inst   = emitInstruction(context, ByteCodeOp::PushRVParam, param->resultRegisterRC[r--]);
                            inst->b.u64 = typeRawVariadic->sizeOf;
                            precallStack += typeRawVariadic->sizeOf;
                            numPushParams++;
                            maxCallParams++;
                        }
                    }
                }

                if (!done)
                {
                    if (param->additionalRegisterRC.canFree)
                    {
                        for (int r = 0; freeRegistersParams && r < param->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->additionalRegisterRC[r]);
                    }

                    for (int r = param->resultRegisterRC.size() - 1; r >= 0;)
                    {
                        if (freeRegistersParams && param->resultRegisterRC.canFree)
                            toFree.push_back(param->resultRegisterRC[r]);
                        accParams.push_back(param->resultRegisterRC[r--]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                        maxCallParams++;
                    }
                }
            }
        }

        emitPushRAParams(context, accParams);
    }

    // Pass a variadic parameter to another function
    auto lastParam = allParams && !allParams->childs.empty() ? allParams->childs.back() : nullptr;
    if (lastParam && lastParam->typeInfo && lastParam->typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        precallStack += 2 * sizeof(Register);
        emitInstruction(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // Pass a variadic parameter to another function
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->kind == TypeInfoKind::Variadic)
    {
        precallStack += 2 * sizeof(Register);
        emitInstruction(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // If last parameter is a spread, then no need to deal with variadic slice : already done
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->flags & TYPEINFO_SPREAD)
    {
        emitInstruction(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        maxCallParams += 2;
        precallStack += 2 * sizeof(Register);
        freeRegisterRC(context, lastParam);
    }

    // Variadic parameter is on top of stack
    else if (typeInfoFunc->flags & TYPEINFO_VARIADIC)
    {
        auto numVariadic = (uint32_t)(numCallParams - numTypeParams) + 1;

        // The array of any has been pushed first, so we need to offset all pushed parameters to point to it
        // This is the main difference with typedvariadic, which directly point to the pushed variadic parameters
        auto offset = numPushParams;

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst       = emitInstruction(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32     = (uint32_t) offset * sizeof(Register);
        inst->c.b       = foreign || lambda;
        inst->d.pointer = (uint8_t*) typeInfoFunc;

        emitInstruction(context, ByteCodeOp::SetImmediate64, r0[1])->b.u64 = numVariadic;
        emitInstruction(context, ByteCodeOp::PushRAParam2, r0[1], r0[0]);
        maxCallParams += 2;

        precallStack += 2 * sizeof(Register);
    }
    else if (typeInfoFunc->flags & TYPEINFO_TYPED_VARIADIC)
    {
        auto numVariadic  = (uint32_t)(numCallParams - numTypeParams) + 1;
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
        auto offset       = (numPushParams - numVariadic * typeVariadic->rawType->numRegisters());

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst       = emitInstruction(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32     = (uint32_t) offset * sizeof(Register);
        inst->c.b       = foreign || lambda;
        inst->d.pointer = (uint8_t*) typeInfoFunc;

        emitInstruction(context, ByteCodeOp::SetImmediate64, r0[1])->b.u64 = numVariadic;
        emitInstruction(context, ByteCodeOp::PushRAParam2, r0[1], r0[0]);
        maxCallParams += 2;

        precallStack += 2 * sizeof(Register);
    }

    if (foreign)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::ForeignCall);
        inst->a.pointer = (uint8_t*) funcNode;
        inst->b.pointer = (uint8_t*) typeInfoFunc;
    }
    else if (funcNode)
    {
        auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(funcNode->extension && funcNode->extension->bc);
        inst->a.pointer = (uint8_t*) funcNode->extension->bc;
        inst->b.pointer = (uint8_t*) typeInfoFunc;
    }
    else
    {
        SWAG_ASSERT(varNodeRegisters.size() > 0);
        emitSafetyNullLambda(context, varNodeRegisters, Msg0859);
        auto inst       = emitInstruction(context, ByteCodeOp::LambdaCall, varNodeRegisters);
        inst->b.pointer = (uint8_t*) typeInfoFunc;
    }

    // Free all registers now that the call can really be done
    for (auto r : toFree)
        freeRegisterRC(context, r);

    // Copy result in a computing register
    if (typeInfoFunc->returnType && typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        if (!(typeInfoFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            auto numRegs = typeInfoFunc->returnType->numRegisters();

            // Need to do that even if discard, not sure why
            context->bc->maxCallResults = max(context->bc->maxCallResults, numRegs);

            if (!(node->flags & AST_DISCARD))
            {
                reserveRegisterRC(context, node->resultRegisterRC, numRegs);
                if (numRegs == 1)
                {
                    emitInstruction(context, ByteCodeOp::CopyRTtoRC, node->resultRegisterRC[0]);
                }
                else
                {
                    SWAG_ASSERT(numRegs == 2);
                    emitInstruction(context, ByteCodeOp::CopyRTtoRC2, node->resultRegisterRC[0], node->resultRegisterRC[1]);
                }
            }
        }
    }

    // Save the maximum number of pushraparams in that bytecode
    context->bc->maxCallParams = max(context->bc->maxCallParams, maxCallParams);

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        emitInstruction(context, ByteCodeOp::IncSPPostCall, precallStack);
    }

    // If we are in a function that need to keep the RR0 register alive, we need to restore it
    if (rr0Saved)
    {
        emitInstruction(context, ByteCodeOp::PopRR);
    }

    // This is usefull when function call is inside an expression like func().something
    // The emitIdentifier will have to know the register where the result of func() is stored
    switch (node->parent->kind)
    {
    // To avoid a double release of the register in emitLoop... sight...
    case AstNodeKind::While:
        break;
    default:
        node->parent->resultRegisterRC = node->resultRegisterRC;
        break;
    }

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
    uint32_t storageIndex = 0;
    if (funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto param                            = node->childs.back();
        auto resolved                         = param->resolvedSymbolOverload;
        resolved->computedValue.storageOffset = offset;
        offset += g_TypeMgr.typeInfoVariadic->sizeOf;
        SWAG_ASSERT(resolved->storageIndex == 0);
        storageIndex += 2;
    }

    auto childSize = node->childs.size();
    for (int i = 0; i < childSize; i++)
    {
        if ((i == childSize - 1) && funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
            break;
        auto param                            = node->childs[i];
        auto resolved                         = param->resolvedSymbolOverload;
        resolved->computedValue.storageOffset = offset;
        SWAG_ASSERT(resolved->storageIndex == storageIndex);

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
    PushNode pn(context, funcNode->content);

    // Clear stack trace when entering a #<function>
    if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
    {
        emitInstruction(context, ByteCodeOp::InternalInitStackTrace);
    }

    // Clear error when entering a #<function> or a function than can raise en error
    if ((funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC) || (funcNode->typeInfo->flags & TYPEINFO_CAN_THROW))
        emitInstruction(context, ByteCodeOp::InternalClearErr);

    if (funcNode->stackSize)
    {
        if (funcNode->stackSize > g_CommandLine.stackSize)
            context->sourceFile->report({funcNode, funcNode->token, Utf8::format(Msg0536, Utf8::toNiceSize(g_CommandLine.stackSize).c_str())});
        emitInstruction(context, ByteCodeOp::DecSPBP)->a.u32 = funcNode->stackSize;
    }

    return true;
}

bool ByteCodeGenJob::emitForeignCall(ByteCodeGenContext* context)
{
    AstNode* node      = context->node;
    auto     overload  = node->resolvedSymbolOverload;
    auto     funcNode  = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    auto     allParams = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_ASSERT(!allParams || allParams->kind == AstNodeKind::FuncCallParams);
    emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRC, true, false, true);
    return true;
}
