#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"
#include "SemanticJob.h"
#include "Report.h"
#include "ModuleManager.h"

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto         funcDecl = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    PushLocation pl(context, &funcDecl->content->token.endLocation);
    PushNode     pn(context, funcDecl->content);

    // No need to do the scope leave stuff if the function does return something, because
    // it has been covered by the mandatory return
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    if (!typeInfo->returnType->isVoid())
        return true;

    SWAG_CHECK(computeLeaveScope(context, funcDecl->scope));
    if (context->result != ContextResult::Done)
        return true;

    emitDebugLine(context);
    EMIT_INST0(context, ByteCodeOp::Ret)->a.u32 = funcDecl->stackSize;

    return true;
}

bool ByteCodeGenJob::emitFuncCallParam(ByteCodeGenContext* context)
{
    auto node = CastAst<AstFuncCallParam>(context->node, AstNodeKind::FuncCallParam);

    // Special case when the parameter comes from an ufcs call that returns an interface.
    // In that case 'specUfcsNode' is the node that makes the call. The register will be
    // the object pointer of the returned interface.
    // :SpecUfcsNode
    if (node->specUfcsNode)
    {
        node->typeInfo         = node->specUfcsNode->typeInfo;
        node->resultRegisterRC = node->specUfcsNode->resultRegisterRC;
        return true;
    }

    auto front = node->childs.front();

    // If we have a cast to an interface, be sure interface has been fully solved
    // Semantic will pass only if the interface has been registered in the struct, and not solved.
    // So we can reach that point and the interface is not done yet.
    // :WaitInterfaceReg
    if (node->castedTypeInfo)
    {
        context->job->waitAllStructInterfaces(node->castedTypeInfo);
        if (context->result != ContextResult::Done)
            return true;
    }

    node->resultRegisterRC = front->resultRegisterRC;
    SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
    return true;
}

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    AstReturn* node       = CastAst<AstReturn>(context->node, AstNodeKind::Return);
    auto       funcNode   = node->ownerFct;
    TypeInfo*  returnType = nullptr;

    // Get the function return type. In case of an emmbedded return, this is the type of the original function to inline
    if (node->ownerInline && (node->semFlags & SEMFLAG_EMBEDDED_RETURN))
        returnType = TypeManager::concreteType(node->ownerInline->func->returnType->typeInfo, CONCRETE_FORCEALIAS);
    else
        returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FORCEALIAS);

    // Copy result to RR0... registers
    if (!(node->semFlags & SEMFLAG_EMIT_DEFERRED) && !node->childs.empty() && !returnType->isVoid())
    {
        auto returnExpression = node->childs.front();
        auto backExpression   = node->childs.back();
        if (backExpression->kind == AstNodeKind::Try || backExpression->kind == AstNodeKind::Catch || backExpression->kind == AstNodeKind::TryCatch)
            backExpression = backExpression->childs.back();
        auto exprType = TypeManager::concretePtrRef(returnExpression->typeInfo);

        // Implicit cast
        if (!(returnExpression->semFlags & SEMFLAG_CAST1))
        {
            SWAG_CHECK(emitCast(context, returnExpression, TypeManager::concreteType(returnExpression->typeInfo), returnExpression->castedTypeInfo));
            if (context->result != ContextResult::Done)
                return true;
            returnExpression->semFlags |= SEMFLAG_CAST1;
        }

        context->job->waitStructGenerated(exprType);
        if (context->result != ContextResult::Done)
            return true;

        //
        // RETVAL
        //
        if ((node->semFlags & SEMFLAG_RETVAL) ||
            (backExpression->resolvedSymbolOverload && backExpression->resolvedSymbolOverload->flags & OVERLOAD_RETVAL))
        {
            auto child = node->childs.front();
            freeRegisterRC(context, child->resultRegisterRC);
        }

        //
        // INLINE
        //
        else if (node->ownerInline && (node->semFlags & SEMFLAG_EMBEDDED_RETURN))
        {
            if (returnType->isStruct())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                SWAG_CHECK(emitCopyStruct(context, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC, exprType, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else if (returnType->isArray())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                SWAG_CHECK(emitCopyArray(context, returnType, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else if (returnType->isClosure())
            {
                // :ConvertToClosure
                if (returnExpression->kind == AstNodeKind::MakePointerLambda)
                {
                    // Copy closure capture buffer
                    // Do it first, because source stack can be shared with node->ownerInline->resultRegisterRC
                    auto nodeCapture = CastAst<AstMakePointer>(returnExpression, AstNodeKind::MakePointerLambda);
                    SWAG_ASSERT(nodeCapture->lambda->captureParameters);
                    auto typeBlock = CastTypeInfo<TypeInfoStruct>(nodeCapture->childs.back()->typeInfo, TypeInfoKind::Struct);
                    if (typeBlock->fields.size())
                    {
                        auto r1 = reserveRegisterRC(context);
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r1, node->ownerInline->resultRegisterRC);
                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = 2 * sizeof(void*);
                        emitMemCpy(context, r1, returnExpression->resultRegisterRC[1], typeBlock->sizeOf);
                        freeRegisterRC(context, r1);
                    }

                    EMIT_INST2(context, ByteCodeOp::SetAtPointer64, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC[0]);
                    auto inst = EMIT_INST1(context, ByteCodeOp::SetAtPointer64, node->ownerInline->resultRegisterRC);
                    inst->flags |= BCI_IMM_B;
                    inst->b.u32 = 1; // <> 0 for closure, 0 for lambda
                    inst->c.u32 = sizeof(void*);
                }
                else
                {
                    emitMemCpy(context, node->ownerInline->resultRegisterRC, returnExpression->resultRegisterRC, SWAG_LIMIT_CLOSURE_SIZEOF);
                }

                freeRegisterRC(context, returnExpression);
            }
            else
            {
                for (auto child : node->childs)
                {
                    auto sizeChilds = child->resultRegisterRC.size();
                    for (int r = 0; r < sizeChilds; r++)
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->ownerInline->resultRegisterRC[r], child->resultRegisterRC[r]);
                    freeRegisterRC(context, child);
                }
            }
        }

        //
        // NORMAL
        //
        else
        {
            if (returnType->isStruct())
            {
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

                // :ReturnStructByValue
                if (CallConv::returnStructByValue(typeFunc))
                {
                    if (!(typeFunc->returnType->flags & TYPEINFO_STRUCT_EMPTY))
                    {
                        switch (typeFunc->returnType->sizeOf)
                        {
                        case 1:
                            EMIT_INST2(context, ByteCodeOp::DeRef8, returnExpression->resultRegisterRC, returnExpression->resultRegisterRC);
                            break;
                        case 2:
                            EMIT_INST2(context, ByteCodeOp::DeRef16, returnExpression->resultRegisterRC, returnExpression->resultRegisterRC);
                            break;
                        case 3:
                        case 4:
                            EMIT_INST2(context, ByteCodeOp::DeRef32, returnExpression->resultRegisterRC, returnExpression->resultRegisterRC);
                            break;
                        default:
                            EMIT_INST2(context, ByteCodeOp::DeRef64, returnExpression->resultRegisterRC, returnExpression->resultRegisterRC);
                            break;
                        }

                        EMIT_INST1(context, ByteCodeOp::CopyRAtoRR, returnExpression->resultRegisterRC);
                    }

                    freeRegisterRC(context, returnExpression->resultRegisterRC);
                }
                else
                {
                    // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                    returnExpression->flags |= AST_NO_LEFT_DROP;
                    RegisterList r0 = reserveRegisterRC(context);
                    EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
                    SWAG_CHECK(emitCopyStruct(context, r0, returnExpression->resultRegisterRC, returnType, returnExpression));
                    freeRegisterRC(context, r0);
                    freeRegisterRC(context, returnExpression->resultRegisterRC);
                }
            }
            else if (returnType->isArray())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->flags |= AST_NO_LEFT_DROP;
                RegisterList r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
                SWAG_CHECK(emitCopyArray(context, returnType, r0, returnExpression->resultRegisterRC, returnExpression));
                freeRegisterRC(context, r0);
                freeRegisterRC(context, returnExpression->resultRegisterRC);
            }
            else if (returnType->isString())
            {
                auto child = node->childs.front();
                if (funcNode->attributeFlags & ATTRIBUTE_AST_FUNC)
                    EMIT_INST2(context, ByteCodeOp::CloneString, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                EMIT_INST2(context, ByteCodeOp::CopyRARBtoRR2, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                freeRegisterRC(context, child->resultRegisterRC);
            }
            else if (returnType->isClosure())
            {
                auto child = node->childs.front();

                RegisterList r1 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r1);

                // :ConvertToClosure
                if (child->kind == AstNodeKind::MakePointerLambda)
                {
                    EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r1, child->resultRegisterRC[0]);
                    auto inst = EMIT_INST1(context, ByteCodeOp::SetAtPointer64, r1);
                    inst->flags |= BCI_IMM_B;
                    inst->b.u32 = 1; // <> 0 for closure, 0 for lambda
                    inst->c.u32 = sizeof(void*);

                    // Copy closure capture buffer
                    auto nodeCapture = CastAst<AstMakePointer>(child, AstNodeKind::MakePointerLambda);
                    SWAG_ASSERT(nodeCapture->lambda->captureParameters);
                    auto typeBlock = CastTypeInfo<TypeInfoStruct>(nodeCapture->childs.back()->typeInfo, TypeInfoKind::Struct);
                    if (typeBlock->fields.size())
                    {
                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = 2 * sizeof(void*);
                        emitMemCpy(context, r1, child->resultRegisterRC[1], typeBlock->sizeOf);
                    }
                }
                else
                {
                    emitMemCpy(context, r1, child->resultRegisterRC, SWAG_LIMIT_CLOSURE_SIZEOF);
                }

                freeRegisterRC(context, r1);
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
                    EMIT_INST1(context, ByteCodeOp::CopyRAtoRR, child->resultRegisterRC[0]);
                else if (numRetReg == 2)
                    EMIT_INST2(context, ByteCodeOp::CopyRARBtoRR2, child->resultRegisterRC[0], child->resultRegisterRC[1]);
                freeRegisterRC(context, child->resultRegisterRC);
            }
        }
    }

    node->semFlags |= SEMFLAG_EMIT_DEFERRED;

    // Leave all scopes
    SWAG_CHECK(emitLeaveScopeReturn(context, &node->forceNoDrop, false));
    if (context->result != ContextResult::Done)
        return true;

    // A return inside an inline function is just a jump to the end of the block
    if (node->ownerInline && (node->semFlags & SEMFLAG_EMBEDDED_RETURN))
    {
        node->seekJump = context->bc->numInstructions;
        EMIT_INST0(context, ByteCodeOp::Jump);
        node->ownerInline->returnList.push_back(node);
    }
    else
    {
        EMIT_INST0(context, ByteCodeOp::Ret)->a.u32 = funcNode->stackSize;
    }

    return true;
}

bool ByteCodeGenJob::emitIntrinsicCVaStart(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto childDest = node->childs[0];

    auto inst = EMIT_INST1(context, ByteCodeOp::IntrinsicCVaStart, childDest->resultRegisterRC);
    SWAG_ASSERT(node->ownerFct);
    SWAG_ASSERT(node->ownerFct->parameters);
    SWAG_ASSERT(!node->ownerFct->parameters->childs.empty());
    auto param = node->ownerFct->parameters->childs.back();
    SWAG_ASSERT(param->typeInfo->isCVariadic());
    auto storageOffset = param->resolvedSymbolOverload->computedValue.storageOffset;
    SWAG_ASSERT(storageOffset != UINT32_MAX);
    inst->b.u64   = storageOffset;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32   = typeInfo->numTotalRegisters();
    freeRegisterRC(context, childDest);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicCVaEnd(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto childDest = node->childs[0];

    EMIT_INST1(context, ByteCodeOp::IntrinsicCVaEnd, childDest->resultRegisterRC);
    freeRegisterRC(context, childDest);
    return true;
}

bool ByteCodeGenJob::emitIntrinsicCVaArg(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto childDest = node->childs[0];

    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = EMIT_INST2(context, ByteCodeOp::IntrinsicCVaArg, childDest->resultRegisterRC, node->resultRegisterRC);
    inst->c.u64            = node->typeInfo->sizeOf;
    freeRegisterRC(context, childDest);
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
        askForByteCode(context->job, node->resolvedSymbolOverload->node, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        if (context->result != ContextResult::Done)
            return true;
    }

    // Some safety checks depending on the intrinsic
    if (mustEmitSafety(context, SAFETY_MATH))
    {
        PushICFlags ic(context, BCI_SAFETY);
        switch (node->tokenId)
        {
        case TokenId::IntrinsicAbs:
        {
            auto t0 = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            emitSafetyNeg(context, callParams->childs[0]->resultRegisterRC, t0, true);
            break;
        }
        case TokenId::IntrinsicSqrt:
        {
            auto t0     = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            auto re     = reserveRegisterRC(context);
            auto op     = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterEqF32 : ByteCodeOp::CompareOpGreaterEqF64;
            auto inst   = EMIT_INST3(context, op, callParams->childs[0]->resultRegisterRC, 0, re);
            inst->b.f64 = 0;
            inst->flags |= BCI_IMM_B;
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicSqrt, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog:
        {
            auto t0     = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            auto re     = reserveRegisterRC(context);
            auto op     = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            auto inst   = EMIT_INST3(context, op, callParams->childs[0]->resultRegisterRC, 0, re);
            inst->b.f64 = 0;
            inst->flags |= BCI_IMM_B;
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog2:
        {
            auto t0     = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            auto re     = reserveRegisterRC(context);
            auto op     = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            auto inst   = EMIT_INST3(context, op, callParams->childs[0]->resultRegisterRC, 0, re);
            inst->b.f64 = 0;
            inst->flags |= BCI_IMM_B;
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog2, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog10:
        {
            auto t0     = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            auto re     = reserveRegisterRC(context);
            auto op     = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            auto inst   = EMIT_INST3(context, op, callParams->childs[0]->resultRegisterRC, 0, re);
            inst->b.f64 = 0;
            inst->flags |= BCI_IMM_B;
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog10, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicASin:
        case TokenId::IntrinsicACos:
        {
            auto t0  = TypeManager::concreteType(callParams->childs[0]->typeInfo);
            auto msg = node->tokenId == TokenId::IntrinsicASin ? safetyMsg(SafetyMsg::IntrinsicASin, t0) : safetyMsg(SafetyMsg::IntrinsicACos, t0);
            auto re  = reserveRegisterRC(context);
            if (t0->nativeType == NativeTypeKind::F32)
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF32, callParams->childs[0]->resultRegisterRC, 0, re);
                inst->b.f32 = -1;
                inst->flags |= BCI_IMM_B;
                emitAssert(context, re, msg);
                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF32, callParams->childs[0]->resultRegisterRC, 0, re);
                inst->b.f32 = 1;
                inst->flags |= BCI_IMM_B;
                emitAssert(context, re, msg);
            }
            else
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF64, callParams->childs[0]->resultRegisterRC, 0, re);
                inst->b.f64 = -1;
                inst->flags |= BCI_IMM_B;
                emitAssert(context, re, msg);
                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF64, callParams->childs[0]->resultRegisterRC, 0, re);
                inst->b.f64 = 1;
                inst->flags |= BCI_IMM_B;
                emitAssert(context, re, msg);
            }
            freeRegisterRC(context, re);
            break;
        }
        default:
            break;
        }
    }

    switch (node->tokenId)
    {
    case TokenId::IntrinsicCompilerError:
    {
        auto child0 = callParams->childs.front();
        auto child1 = callParams->childs.back();
        EMIT_INST3(context, ByteCodeOp::IntrinsicCompilerError, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicCompilerWarning:
    {
        auto child0 = callParams->childs.front();
        auto child1 = callParams->childs.back();
        EMIT_INST3(context, ByteCodeOp::IntrinsicCompilerWarning, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicPanic:
    {
        auto child0 = callParams->childs.front();
        auto child1 = callParams->childs.back();
        EMIT_INST3(context, ByteCodeOp::IntrinsicPanic, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC);
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
    case TokenId::IntrinsicBcBreakpoint:
    {
        EMIT_INST0(context, ByteCodeOp::IntrinsicBcBreakpoint);
        break;
    }
    case TokenId::IntrinsicAlloc:
    {
        auto child0            = callParams->childs.front();
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::IntrinsicAlloc, node->resultRegisterRC, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicFree:
    {
        auto child0 = callParams->childs.front();
        EMIT_INST1(context, ByteCodeOp::IntrinsicFree, child0->resultRegisterRC);
        freeRegisterRC(context, child0);
        break;
    }
    case TokenId::IntrinsicRealloc:
    {
        auto child0            = callParams->childs.front();
        auto child1            = callParams->childs.back();
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::IntrinsicRealloc, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicMemCpy:
    {
        auto childDest = callParams->childs[0];
        auto childSrc  = callParams->childs[1];
        auto childSize = callParams->childs[2];
        EMIT_INST3(context, ByteCodeOp::IntrinsicMemCpy, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
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
        EMIT_INST3(context, ByteCodeOp::IntrinsicMemMove, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
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
        EMIT_INST3(context, ByteCodeOp::IntrinsicMemSet, childDest->resultRegisterRC, childValue->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childValue);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicMemCmp:
    {
        auto childDest         = callParams->childs[0];
        auto childSrc          = callParams->childs[1];
        auto childSize         = callParams->childs[2];
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST4(context, ByteCodeOp::IntrinsicMemCmp, node->resultRegisterRC, childDest->resultRegisterRC, childSrc->resultRegisterRC, childSize->resultRegisterRC);
        freeRegisterRC(context, childDest);
        freeRegisterRC(context, childSrc);
        freeRegisterRC(context, childSize);
        break;
    }
    case TokenId::IntrinsicStrLen:
    {
        auto childSrc          = callParams->childs[0];
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::IntrinsicStrLen, node->resultRegisterRC, childSrc->resultRegisterRC);
        freeRegisterRC(context, childSrc);
        break;
    }
    case TokenId::IntrinsicStrCmp:
    {
        auto childSrc0         = callParams->childs[0];
        auto childSrc1         = callParams->childs[1];
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::IntrinsicStrCmp, node->resultRegisterRC, childSrc0->resultRegisterRC, childSrc1->resultRegisterRC);
        freeRegisterRC(context, childSrc0);
        freeRegisterRC(context, childSrc1);
        break;
    }
    case TokenId::IntrinsicTypeCmp:
    {
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto child2            = callParams->childs[2];
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        freeRegisterRC(context, child2);
        break;
    }
    case TokenId::IntrinsicRtFlags:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::IntrinsicRtFlags, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicStringCmp:
    {
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        node->resultRegisterRC = child1->resultRegisterRC[1];
        EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, child0->resultRegisterRC[0], child0->resultRegisterRC[1], child1->resultRegisterRC[0], child1->resultRegisterRC[1]);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1->resultRegisterRC[0]);
        break;
    }
    case TokenId::IntrinsicItfTableOf:
    {
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::IntrinsicItfTableOf, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }
    case TokenId::IntrinsicDbgAlloc:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST1(context, ByteCodeOp::IntrinsicDbgAlloc, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicSysAlloc:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST1(context, ByteCodeOp::IntrinsicSysAlloc, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicGetContext:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        SWAG_ASSERT(node->identifierRef() == node->parent);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST1(context, ByteCodeOp::IntrinsicGetContext, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicSetContext:
    {
        auto childDest = callParams->childs[0];
        EMIT_INST1(context, ByteCodeOp::IntrinsicSetContext, childDest->resultRegisterRC);
        freeRegisterRC(context, childDest);
        break;
    }
    case TokenId::IntrinsicGetProcessInfos:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        SWAG_ASSERT(node->identifierRef() == node->parent);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST1(context, ByteCodeOp::IntrinsicGetProcessInfos, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicArguments:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        EMIT_INST2(context, ByteCodeOp::IntrinsicArguments, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicModules:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        EMIT_INST2(context, ByteCodeOp::IntrinsicModules, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicGvtd:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        EMIT_INST2(context, ByteCodeOp::IntrinsicGvtd, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicCompiler:
    {
        reserveLinearRegisterRC2(context, node->resultRegisterRC);
        node->parent->resultRegisterRC = node->resultRegisterRC;
        EMIT_INST2(context, ByteCodeOp::IntrinsicCompiler, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        emitPostCallUfcs(context);
        break;
    }
    case TokenId::IntrinsicIsByteCode:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST1(context, ByteCodeOp::IntrinsicIsByteCode, node->resultRegisterRC);
        break;
    }
    case TokenId::IntrinsicGetErr:
    {
        reserveRegisterRC(context, node->resultRegisterRC, 2);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        EMIT_INST2(context, ByteCodeOp::IntrinsicGetErr, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        break;
    }
    case TokenId::IntrinsicAtomicAdd:
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto child0            = callParams->childs[0];
        auto child1            = callParams->childs[1];
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicAdd invalid type");
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
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicAnd invalid type");
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
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicOr invalid type");
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
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicXor invalid type");
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
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS8, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS16, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS32, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS64, child0->resultRegisterRC, child1->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicXchg invalid type");
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
        auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS8, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS16, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS32, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS64, child0->resultRegisterRC, child1->resultRegisterRC, child2->resultRegisterRC, node->resultRegisterRC);
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicAtomicCmpXchg invalid type");
        }
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        freeRegisterRC(context, child2);
        break;
    }

    case TokenId::IntrinsicPow:
    case TokenId::IntrinsicATan2:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto child0                             = callParams->childs[0];
        auto child1                             = callParams->childs[1];
        auto typeInfo                           = TypeManager::concreteType(child0->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
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
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicPow invalid type");
        }

        auto inst   = EMIT_INST3(context, op, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->tokenId;
        freeRegisterRC(context, child0);
        freeRegisterRC(context, child1);
        break;
    }

    case TokenId::IntrinsicBitCountNz:
    case TokenId::IntrinsicBitCountTz:
    case TokenId::IntrinsicBitCountLz:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto child                              = callParams->childs[0];
        auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
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
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicBitCount invalid type");
        }

        auto inst   = EMIT_INST2(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->tokenId;
        freeRegisterRC(context, child);
        break;
    }

    case TokenId::IntrinsicByteSwap:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto child                              = callParams->childs[0];
        auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
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
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicByteSwap invalid type");
        }

        auto inst   = EMIT_INST2(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->tokenId;
        freeRegisterRC(context, child);
        break;
    }

    case TokenId::IntrinsicMulAdd:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto typeInfo                           = TypeManager::concreteType(callParams->childs[0]->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
        ByteCodeOp op = ByteCodeOp::End;
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicMulAddF32;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicMulAddF64;
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicMulAdd invalid type");
        }

        EMIT_INST4(context, op, node->resultRegisterRC, callParams->childs[0]->resultRegisterRC, callParams->childs[1]->resultRegisterRC, callParams->childs[2]->resultRegisterRC);
        freeRegisterRC(context, callParams->childs[0]);
        freeRegisterRC(context, callParams->childs[1]);
        freeRegisterRC(context, callParams->childs[2]);
        break;
    }

    case TokenId::IntrinsicMin:
    case TokenId::IntrinsicMax:
    {
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto child0                             = callParams->childs[0];
        auto child1                             = callParams->childs[1];
        auto typeInfo                           = TypeManager::concreteType(child0->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
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
            op = ByteCodeOp::IntrinsicU64x2;
            break;
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicF32x2;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicF64x2;
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, IntrinsicMin/Max invalid type");
        }

        auto inst   = EMIT_INST3(context, op, node->resultRegisterRC, child0->resultRegisterRC, child1->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->tokenId;
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
        node->resultRegisterRC                  = reserveRegisterRC(context);
        node->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC          = node->resultRegisterRC;
        auto child                              = callParams->childs[0];
        auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
        SWAG_ASSERT(typeInfo->isNative());
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
            op = ByteCodeOp::IntrinsicS64x1;
            break;
        case NativeTypeKind::F32:
            op = ByteCodeOp::IntrinsicF32x1;
            break;
        case NativeTypeKind::F64:
            op = ByteCodeOp::IntrinsicF64x1;
            break;
        default:
            return Report::internalError(context->node, "emitIntrinsic, math intrinsic invalid type");
        }

        auto inst   = EMIT_INST2(context, op, node->resultRegisterRC, child->resultRegisterRC);
        inst->d.u32 = (uint32_t) node->tokenId;
        freeRegisterRC(context, child);
        break;
    }

    default:
        return Report::internalError(context->node, "emitIntrinsic, unknown intrinsic");
    }

    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, node->resultRegisterRC);

    return true;
}

bool ByteCodeGenJob::emitLambdaCall(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto overload = node->resolvedSymbolOverload;

    SWAG_CHECK(emitIdentifier(context));
    node->allocateExtension(ExtensionKind::Misc);
    node->extMisc()->additionalRegisterRC = node->resultRegisterRC;
    auto allParams                        = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_ASSERT(!allParams || allParams->kind == AstNodeKind::FuncCallParams);

    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCEALIAS);

    if (node->isSilentCall())
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(overload->typeInfo, TypeInfoKind::Array);
        typeRef      = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCEALIAS);
    }

    // A closure is the pointer to the variable, not the function address
    // Function address is stored first
    // Then comes 8 or 0 if it's a real closure or a lambda
    if (typeRef->isClosure())
    {
        // Deref capture context. If 0, no context.
        node->extMisc()->additionalRegisterRC += reserveRegisterRC(context);
        auto inst       = EMIT_INST2(context, ByteCodeOp::DeRef64, node->extMisc()->additionalRegisterRC[1], node->extMisc()->additionalRegisterRC[0]);
        inst->c.u64     = 8;
        inst->d.pointer = (uint8_t*) context->node->resolvedSymbolOverload;

        // If 0, keep it 0, otherwhise get the capture context by adding that offset to the address of the closure storage
        inst        = EMIT_INST2(context, ByteCodeOp::MulAddVC64, node->extMisc()->additionalRegisterRC[1], node->extMisc()->additionalRegisterRC[0]);
        inst->c.u64 = 16;

        // Deref function pointer
        inst            = EMIT_INST2(context, ByteCodeOp::DeRef64, node->extMisc()->additionalRegisterRC[0], node->extMisc()->additionalRegisterRC[0]);
        inst->d.pointer = (uint8_t*) context->node->resolvedSymbolOverload;
    }

    emitSafetyNullCheck(context, node->extMisc()->additionalRegisterRC[0]);

    SWAG_CHECK(emitCall(context, allParams, nullptr, (AstVarDecl*) overload->node, node->extMisc()->additionalRegisterRC, false, true, true));
    SWAG_ASSERT(context->result == ContextResult::Done);
    freeRegisterRC(context, node->extMisc()->additionalRegisterRC);
    return true;
}

void ByteCodeGenJob::emitPostCallUfcs(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    // :SpecUfcsNode
    // Specific case. The function returns an interface, so it returns two registers.
    // But we want that interface to be also an ufcs parameter.
    // Ex: var cfg = @compiler().getBuildCfg()
    if (node->typeInfo->isInterface() && node->flags & AST_TO_UFCS)
    {
        auto r = reserveRegisterRC(context);

        // So we need to remember the object pointer to be passed as a parameter.
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r, node->resultRegisterRC[0]);

        // And we need to put the itable pointer in the first register, in order for the lambda value (function pointer)
        // to be dereferenced.
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], node->resultRegisterRC[1]);

        // From now on, the register is the object pointer (not the itable). Used un emitFuncCallParam.
        node->resultRegisterRC = r;
    }
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     funcNode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    auto allParams = node->childs.empty() ? nullptr : node->childs.back();
    SWAG_ASSERT(!allParams || allParams->kind == AstNodeKind::FuncCallParams);
    SWAG_CHECK(emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRC, false, false, true));
    if (context->result != ContextResult::Done)
        return true;
    emitPostCallUfcs(context);
    return true;
}

void ByteCodeGenJob::computeSourceLocation(JobContext* context, AstNode* node, uint32_t* storageOffset, DataSegment** storageSegment, bool forceCompiler)
{
    auto seg        = SemanticJob::getConstantSegFromContext(context->node, forceCompiler);
    *storageSegment = seg;

    auto sourceFile = node->sourceFile;
    auto module     = sourceFile->module;

    auto     str = sourceFile->path;
    uint8_t* addrName;
    auto     offsetName = seg->addString(str.string(), &addrName);

    SourceLocationCache tmpLoc;
    tmpLoc.loc.fileName.buffer = addrName;
    tmpLoc.loc.fileName.count  = str.string().length();
    tmpLoc.loc.lineStart       = node->token.startLocation.line;
    tmpLoc.loc.colStart        = node->token.startLocation.column;
    tmpLoc.loc.lineEnd         = node->token.endLocation.line;
    tmpLoc.loc.colEnd          = node->token.endLocation.column;

    ScopedLock lock(module->mutexSourceLoc);

    // Is the same location is in the cache ?
    uint32_t crc = Crc32::compute(addrName, (uint32_t) str.string().length());
    crc          = Crc32::compute((uint8_t*) &node->token.startLocation.line, sizeof(uint32_t), crc);
    crc          = Crc32::compute((uint8_t*) &node->token.startLocation.column, sizeof(uint32_t), crc);
    crc          = Crc32::compute((uint8_t*) &node->token.endLocation.line, sizeof(uint32_t), crc);
    crc          = Crc32::compute((uint8_t*) &node->token.endLocation.column, sizeof(uint32_t), crc);
    auto it      = module->cacheSourceLoc.find(crc);
    if (it != module->cacheSourceLoc.end())
    {
        for (auto& l : it->second)
        {
            if (l.loc.lineStart == node->token.startLocation.line &&
                l.loc.colStart == node->token.startLocation.column &&
                l.loc.lineEnd == node->token.endLocation.line &&
                l.loc.colEnd == node->token.endLocation.column &&
                l.storageSegment == seg &&
                !strncmp((const char*) l.loc.fileName.buffer, (const char*) addrName, l.loc.fileName.count))
            {
                *storageOffset = l.storageOffset;
                return;
            }
        }
    }

    SwagSourceCodeLocation* loc;
    auto                    offset = seg->reserve(sizeof(SwagSourceCodeLocation), (uint8_t**) &loc, sizeof(void*));
    seg->addInitPtr(offset, offsetName);
    memcpy(loc, &tmpLoc.loc, sizeof(tmpLoc.loc));

    // Store in the cache
    tmpLoc.storageSegment = seg;
    tmpLoc.storageOffset  = offset;
    if (it != module->cacheSourceLoc.end())
        it->second.push_back(tmpLoc);
    else
        module->cacheSourceLoc[crc] = VectorNative<SourceLocationCache>{tmpLoc};

    *storageOffset = offset;
}

bool ByteCodeGenJob::emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList)
{
    auto node         = context->node;
    auto defaultParam = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
    SWAG_ASSERT(defaultParam->assignment);

    if (defaultParam->assignment->kind == AstNodeKind::CompilerSpecialValue)
    {
        switch (defaultParam->assignment->tokenId)
        {
        case TokenId::CompilerCallerLocation:
        {
            regList = reserveRegisterRC(context);
            uint32_t     storageOffset;
            DataSegment* storageSegment;
            computeSourceLocation(context, node, &storageOffset, &storageSegment);
            emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
            break;
        }

        case TokenId::CompilerOs:
        {
            regList     = reserveRegisterRC(context);
            auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
            inst->b.u64 = (uint64_t) g_CommandLine.target.os;
            break;
        }
        case TokenId::CompilerArch:
        {
            regList     = reserveRegisterRC(context);
            auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
            inst->b.u64 = (uint64_t) g_CommandLine.target.arch;
            break;
        }

        case TokenId::CompilerSwagOs:
        {
            regList     = reserveRegisterRC(context);
            auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
            inst->b.u64 = (uint64_t) OS::getNativeTarget().os;
            break;
        }

        case TokenId::CompilerBackend:
        {
            regList     = reserveRegisterRC(context);
            auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
            inst->b.u64 = (uint64_t) g_CommandLine.backendGenType;
            break;
        }

        case TokenId::CompilerCallerFunction:
        case TokenId::CompilerBuildCfg:
        case TokenId::CompilerCpu:
        {
            reserveLinearRegisterRC2(context, regList);
            auto str            = SemanticJob::getCompilerFunctionString(node, defaultParam->assignment->tokenId);
            auto storageSegment = SemanticJob::getConstantSegFromContext(context->node);
            auto storageOffset  = storageSegment->addString(str);
            SWAG_ASSERT(storageOffset != UINT32_MAX);
            emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = str.length();
            break;
        }
        default:
            return Report::internalError(defaultParam->assignment, "emitDefaultParamValue, invalid compiler function");
        }

        return true;
    }

    SWAG_ASSERT(defaultParam->assignment->hasComputedValue());
    SWAG_CHECK(emitLiteral(context, defaultParam->assignment, defaultParam->typeInfo, regList));
    return true;
}

void ByteCodeGenJob::emitPushRAParams(ByteCodeGenContext* context, VectorNative<uint32_t>& accParams, bool forVariadic)
{
    auto node = context->node;

    int cpt = (int) accParams.size();

    if (node->typeInfo && node->typeInfo->isClosure())
    {
        // The first argument of the function needs to be the capture context, which is stored in
        // node->additionalRegisterRC as the second register.
        SWAG_ASSERT(node->extension);
        accParams[(int) accParams.size() - 1] = node->extMisc()->additionalRegisterRC[1];

        // The last pushParam needs to be treated in a different way in case of closure, because
        // if in fact we call a lambda, it does not exists (the closure context)
        cpt--;
    }

    int i = 0;
    while (i < cpt)
    {
        ByteCodeInstruction* inst;
        if (cpt - i >= 4)
        {
            inst = EMIT_INST4(context, ByteCodeOp::PushRAParam4, accParams[i], accParams[i + 1], accParams[i + 2], accParams[i + 3]);
            i += 4;
        }
        else if (cpt - i >= 3)
        {
            inst = EMIT_INST3(context, ByteCodeOp::PushRAParam3, accParams[i], accParams[i + 1], accParams[i + 2]);
            i += 3;
        }
        else if (cpt - i >= 2)
        {
            inst = EMIT_INST2(context, ByteCodeOp::PushRAParam2, accParams[i], accParams[i + 1]);
            i += 2;
        }
        else
        {
            inst = EMIT_INST1(context, ByteCodeOp::PushRAParam, accParams[i]);
            i += 1;
        }

        inst->flags |= forVariadic ? BCI_VARIADIC : 0;
    }

    // Closure context
    if (node->typeInfo && node->typeInfo->isClosure())
        EMIT_INST2(context, ByteCodeOp::PushRAParamCond, accParams.back(), accParams.back());

    accParams.clear();
}

bool ByteCodeGenJob::checkCatchError(ByteCodeGenContext* context, AstNode* srcNode, AstNode* callNode, AstNode* funcNode, AstNode* parent, TypeInfo* typeInfoFunc)
{
    bool raiseErrors = typeInfoFunc->flags & TYPEINFO_CAN_THROW;
    if (raiseErrors && (!callNode->hasExtOwner() || !callNode->extOwner()->ownerTryCatchAssume))
    {
        if (!srcNode)
            srcNode = typeInfoFunc->declNode;

        if (callNode->kind == AstNodeKind::Inline)
        {
            Diagnostic diag{callNode->sourceFile, callNode->token, Fmt(Err(Err0534), funcNode->token.ctext())};
            return context->report(diag, Diagnostic::hereIs(srcNode));
        }
        else
        {
            Diagnostic diag{callNode, Fmt(Err(Err0534), funcNode->token.ctext())};
            return context->report(diag, Diagnostic::hereIs(srcNode));
        }
    }

    if (!raiseErrors)
    {
        if (parent->kind == AstNodeKind::Try ||
            parent->kind == AstNodeKind::Catch ||
            parent->kind == AstNodeKind::TryCatch ||
            parent->kind == AstNodeKind::Assume)
        {
            if (!srcNode)
                srcNode = typeInfoFunc->declNode;
            Diagnostic diag{parent, parent->token, Fmt(Err(Err0535), parent->token.ctext(), srcNode->token.ctext())};
            return context->report(diag, Diagnostic::hereIs(srcNode));
        }
    }

    return true;
}

bool ByteCodeGenJob::emitReturnByCopyAddress(ByteCodeGenContext* context, AstNode* node, TypeInfoFuncAttr* typeInfoFunc)
{
    node->resultRegisterRC = reserveRegisterRC(context);

    auto testReturn = node;
    if (testReturn->kind == AstNodeKind::Identifier || testReturn->kind == AstNodeKind::FuncCall)
    {
        // We need a copy in #ast functions
        if (!node->ownerFct || !(node->ownerFct->attributeFlags & ATTRIBUTE_AST_FUNC))
            testReturn = testReturn->parent;
    }

    // If in a return expression, just push the caller retval
    AstNode* parentReturn = testReturn ? testReturn->inSimpleReturn() : nullptr;
    if (parentReturn)
    {
        // Must be the last expression in the return expression (no deref !)
        if (node->parent->kind != AstNodeKind::IdentifierRef || node == node->parent->childs.back())
        {
            if (node->ownerInline)
            {
                SWAG_IF_ASSERT(auto parentTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->ownerInline->func->typeInfo, TypeInfoKind::FuncAttr));
                SWAG_ASSERT(CallConv::returnByStackAddress(parentTypeFunc));
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->ownerInline->resultRegisterRC);
            }
            else
            {
                SWAG_IF_ASSERT(auto parentTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr));
                SWAG_ASSERT(CallConv::returnByStackAddress(parentTypeFunc));
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->resultRegisterRC);
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRC);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->semFlags |= SEMFLAG_RETVAL;
            return true;
        }
    }

    // A function call used to initialized the field of a struct
    // No need to put the result on the stack and copy the result later, just make a direct reference to
    // the field address
    testReturn = node;
    if (testReturn->kind == AstNodeKind::Identifier || testReturn->kind == AstNodeKind::FuncCall)
        testReturn = testReturn->parent;

    if (testReturn->parent->kind == AstNodeKind::FuncCallParam &&
        testReturn->parent->parent->parent->kind == AstNodeKind::Identifier &&
        testReturn->parent->parent->parent->parent->parent->kind == AstNodeKind::TypeExpression &&
        testReturn->parent->parent->parent->parent->parent->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS &&
        testReturn->parent->parent->parent->parent->parent->parent->kind == AstNodeKind::VarDecl &&
        testReturn->parent->parent->parent->typeInfo &&
        testReturn->parent->parent->parent->typeInfo->isStruct())
    {
        auto varNode  = CastAst<AstVarDecl>(testReturn->parent->parent->parent->parent->parent->parent, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        auto resolved = varNode->resolvedSymbolOverload;
        auto param    = CastAst<AstFuncCallParam>(testReturn->parent, AstNodeKind::FuncCallParam);
        SWAG_ASSERT(param->resolvedParameter);
        auto typeParam = param->resolvedParameter;

        emitRetValRef(context, resolved, node->resultRegisterRC, false, resolved->computedValue.storageOffset + typeParam->offset);
        EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRC);
        context->bc->maxCallResults = max(context->bc->maxCallResults, 1);

        testReturn->parent->semFlags |= SEMFLAG_FIELD_STRUCT;
        return true;
    }

    // Store in RR0 the address of the stack to store the result
    auto inst   = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
    inst->b.u64 = node->computedValue->storageOffset;
    EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRC);
    context->bc->maxCallResults = max(context->bc->maxCallResults, 1);

    if (node->resolvedSymbolOverload)
        node->resolvedSymbolOverload->flags |= OVERLOAD_EMITTED;

    // Push a var drop, except if we are in an expression (constexpr).
    // So check that the ownerScope will be executed (the bytecode should be a parent of the scope).
    if (context->bc->node->isParentOf(node->ownerScope->owner))
        node->ownerScope->symTable.addVarToDrop(node->resolvedSymbolOverload, typeInfoFunc->returnType, node->computedValue->storageOffset);

    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, node->resultRegisterRC);

    return true;
}

bool ByteCodeGenJob::emitCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode, RegisterList& varNodeRegisters, bool foreign, bool lambda, bool freeRegistersParams)
{
    AstNode* node = context->node;

    TypeInfoFuncAttr* typeInfoFunc = nullptr;
    if (funcNode)
    {
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    }
    else if (varNode->typeInfo->isArray())
    {
        // :SilentCall
        auto typeArr = CastTypeInfo<TypeInfoArray>(varNode->typeInfo, TypeInfoKind::Array);
        auto typeVar = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCEALIAS);
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
    }
    else if (varNode->typeInfo->isPointerRef())
    {
        auto typeVar = TypeManager::concretePtrRefType(varNode->typeInfo, CONCRETE_FORCEALIAS);
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
    }
    else
    {
        auto typeVar = TypeManager::concreteType(varNode->typeInfo, CONCRETE_FORCEALIAS);
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
    }

    // Be sure referenced function has bytecode
    askForByteCode(context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
    if (context->result != ContextResult::Done)
        return true;

    int numCallParams = allParams ? (int) allParams->childs.size() : 0;

    // For a untyped variadic, we need to store all parameters as 'any'
    // So we must generate one type per parameter
    Vector<uint32_t> storageOffsetsVariadicTypes;
    if (allParams && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
    {
        auto  numFuncParams  = (int) typeInfoFunc->parameters.size();
        auto  module         = context->sourceFile->module;
        auto& typeGen        = module->typeGen;
        auto  storageSegment = SemanticJob::getConstantSegFromContext(allParams);

        // We must export one type per parameter
        for (int i = (int) numCallParams - 1; i >= numFuncParams - 1; i--)
        {
            auto     child        = allParams->childs[i];
            auto     concreteType = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
            uint32_t storageOffset;
            context->baseJob = context->job;
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, concreteType, storageSegment, &storageOffset));
            if (context->result != ContextResult::Done)
                return true;
            storageOffsetsVariadicTypes.push_back(storageOffset);
        }
    }

    // Error, check validity.
    if (node->parent->kind == AstNodeKind::IdentifierRef)
        SWAG_CHECK(checkCatchError(context, varNode, node, node, node->parent->parent, typeInfoFunc));
    else
        SWAG_CHECK(checkCatchError(context, varNode, node, node, node->parent, typeInfoFunc));

    // If we are in a function that need to keep the RR0 register alive, we need to save it
    bool rr0Saved = false;
    if (node->ownerFct)
    {
        auto ownerTypeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        auto ownerReturnType   = ownerTypeInfoFunc->concreteReturnType();
        if (!ownerReturnType->isVoid())
        {
            if (CallConv::returnByStackAddress(ownerTypeInfoFunc))
            {
                EMIT_INST0(context, ByteCodeOp::PushRR);
                rr0Saved = true;
            }
            else if (node->flags & AST_IN_DEFER)
            {
                EMIT_INST0(context, ByteCodeOp::PushRR);
                rr0Saved = true;
            }
        }
    }

    // Return by copy
    if (CallConv::returnByStackAddress(typeInfoFunc))
    {
        SWAG_CHECK(emitReturnByCopyAddress(context, node, typeInfoFunc));
    }

    uint32_t maxCallParams = typeInfoFunc->numReturnRegisters();

    // Sort childs by parameter index
    if (allParams && (allParams->flags & AST_MUST_SORT_CHILDS))
    {
        sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2)
             {
                 AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
                 AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
                 return p1->indexParam < p2->indexParam; });
    }
    else if (allParams && (allParams->semFlags & SEMFLAG_INVERSE_PARAMS))
    {
        SWAG_ASSERT(allParams->childs.size() == 2);
        allParams->swap2Childs();
    }

    // For a untyped variadic, we need to store all parameters as 'any'
    int                    precallStack = 0;
    VectorNative<uint32_t> toFree;
    if (allParams && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
    {
        auto numFuncParams  = (int) typeInfoFunc->parameters.size();
        auto numVariadic    = (uint32_t) (numCallParams - numFuncParams) + 1;
        int  offset         = numVariadic * 2 * sizeof(Register);
        auto storageSegment = SemanticJob::getConstantSegFromContext(allParams);

        for (int i = (int) numCallParams - 1; i >= numFuncParams - 1; i--)
        {
            auto child     = allParams->childs[i];
            auto typeParam = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);

            // Be sure to point to the first register of the type, if it has many
            offset += (typeParam->numRegisters() - 1) * sizeof(Register);

            // Store concrete type info
            auto r0 = reserveRegisterRC(context);
            toFree.push_back(r0);
            emitMakeSegPointer(context, storageSegment, storageOffsetsVariadicTypes.front(), r0);
            storageOffsetsVariadicTypes.erase(storageOffsetsVariadicTypes.begin());
            EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
            maxCallParams++;

            // For a big data we directly set the data pointer in the 'any' instead of pushing it to the stack.
            if (typeParam->isStruct() ||
                typeParam->isArray() ||
                typeParam->isListArray() ||
                typeParam->isListTuple())
            {
                EMIT_INST1(context, ByteCodeOp::PushRAParam, child->resultRegisterRC[0]);
                maxCallParams++;
            }
            else
            {
                auto r1 = reserveRegisterRC(context);
                toFree.push_back(r1);

                // The value will be stored on the stack (1 or 2 registers max). So we push now the address
                // of that value on that stack. This is the data part of the 'any'
                // Store address of value on the stack
                auto inst   = EMIT_INST1(context, ByteCodeOp::CopySP, r1);
                inst->b.u64 = offset;
                inst->c.u64 = child->resultRegisterRC[0];
                EMIT_INST1(context, ByteCodeOp::PushRAParam, r1);
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
                auto param = CastAst<AstFuncCallParam>(allParams->childs[j], AstNodeKind::FuncCallParam);
                if (param->indexParam == i)
                {
                    if (param->hasExtMisc() && !param->extMisc()->additionalRegisterRC.cannotFree)
                    {
                        for (int r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                    }

                    if (param->typeInfo->isArray() || param->typeInfo->flags & TYPEINFO_LISTARRAY_ARRAY)
                        truncRegisterRC(context, param->resultRegisterRC, 1);

                    for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
                    {
                        if (freeRegistersParams && !param->resultRegisterRC.cannotFree)
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

            // If not covered, then this is a default value, or a variadic parameter
            if (!covered)
            {
                // funcNode can be null in case of a lambda, so we need to retrieve the function description from the type
                AstNode* parameters = nullptr;
                if (funcNode)
                    parameters = funcNode->parameters;
                else
                {
                    SWAG_ASSERT(typeInfoFunc->declNode);
                    if (typeInfoFunc->declNode->kind == AstNodeKind::FuncDecl)
                    {
                        auto funcDesc = CastAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl);
                        parameters    = funcDesc->parameters;
                    }
                    else
                    {
                        auto funcDesc = CastAst<AstTypeLambda>(typeInfoFunc->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
                        parameters    = funcDesc->childs.front();
                    }
                }

                // Empty variadic parameter
                auto defaultParam = CastAst<AstVarDecl>(parameters->childs[i], AstNodeKind::FuncDeclParam);
                if (!defaultParam->typeInfo->isVariadic() &&
                    !defaultParam->typeInfo->isTypedVariadic() &&
                    !defaultParam->typeInfo->isCVariadic())
                {
                    SWAG_ASSERT(defaultParam->assignment);

                    RegisterList regList;
                    SWAG_CHECK(emitDefaultParamValue(context, defaultParam, regList));

                    for (int r = regList.size() - 1; r >= 0;)
                    {
                        if (!regList.cannotFree)
                            toFree.push_back(regList[r]);
                        accParams.push_back(regList[r--]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                        maxCallParams++;
                    }
                }
            }
        }

        emitPushRAParams(context, accParams, false);
    }

    // Fast call. No need to do fancy things, all the parameters are covered by the call
    else if (numCallParams)
    {
        // Get the last variadic real type
        TypeInfo* typeRawVariadic = nullptr;
        bool      forVariadic     = false;
        if (typeInfoFunc->parameters.back()->typeInfo->isTypedVariadic())
        {
            auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
            typeRawVariadic   = typeVariadic->rawType;
            forVariadic       = true;
        }
        else if (typeInfoFunc->parameters.back()->typeInfo->isVariadic())
        {
            forVariadic = true;
        }
        else if (typeInfoFunc->parameters.back()->typeInfo->isCVariadic())
        {
            forVariadic = true;
        }

        VectorNative<uint32_t> accParams;
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = allParams->childs[i];
            if (param->resultRegisterRC.size() == 0)
                continue;
            if (!param->typeInfo)
                continue;

            if (param->typeInfo->isArray() || param->typeInfo->flags & TYPEINFO_LISTARRAY_ARRAY)
                truncRegisterRC(context, param->resultRegisterRC, 1);

            if (!param->typeInfo->isVariadic() &&
                !param->typeInfo->isTypedVariadic() &&
                !(param->typeInfo->flags & TYPEINFO_SPREAD))
            {
                bool done = false;

                // If this is a variadic parameter of a cvarargs, we need to promote the value
                if ((typeInfoFunc->isCVariadic()) &&
                    i >= numTypeParams - 1 &&
                    param->typeInfo->isNative() &&
                    param->typeInfo->sizeOf < sizeof(Register))
                {
                    switch (param->typeInfo->nativeType)
                    {
                    case NativeTypeKind::S8:
                        EMIT_INST2(context, ByteCodeOp::CastS8S32, param->resultRegisterRC, param->resultRegisterRC);
                        break;
                    case NativeTypeKind::S16:
                        EMIT_INST2(context, ByteCodeOp::CastS16S32, param->resultRegisterRC, param->resultRegisterRC);
                        break;
                    case NativeTypeKind::U8:
                    case NativeTypeKind::Bool:
                        EMIT_INST1(context, ByteCodeOp::ClearMaskU32, param->resultRegisterRC)->b.u64 = 0x000000FF;
                        break;
                    case NativeTypeKind::U16:
                        EMIT_INST1(context, ByteCodeOp::ClearMaskU32, param->resultRegisterRC)->b.u64 = 0x0000FFFF;
                        break;
                    case NativeTypeKind::F32:
                        EMIT_INST2(context, ByteCodeOp::CastF32F64, param->resultRegisterRC, param->resultRegisterRC);
                        break;
                    default:
                        break;
                    }
                }

                // If we push a something to a typed variadic, we need to push PushRVParam and not PushRAParam if the size
                // is less than a register, because we want the typed variadic to be a real slice (not always a slice of registers)
                if (typeRawVariadic && i >= numTypeParams - 1)
                {
                    if (typeRawVariadic->isNative() && typeRawVariadic->sizeOf < sizeof(Register))
                    {
                        done = true;
                        if (param->hasExtMisc() && !param->extMisc()->additionalRegisterRC.cannotFree)
                        {
                            for (int r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                                toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                        }

                        for (int r = param->resultRegisterRC.size() - 1; r >= 0;)
                        {
                            if (freeRegistersParams && !param->resultRegisterRC.cannotFree)
                                toFree.push_back(param->resultRegisterRC[r]);
                            auto inst   = EMIT_INST1(context, ByteCodeOp::PushRVParam, param->resultRegisterRC[r--]);
                            inst->b.u64 = typeRawVariadic->sizeOf;
                            precallStack += typeRawVariadic->sizeOf;
                            numPushParams++;
                            maxCallParams++;
                        }
                    }
                }

                if (!done)
                {
                    if (param->hasExtMisc() && !param->extMisc()->additionalRegisterRC.cannotFree)
                    {
                        for (int r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                    }

                    for (int r = param->resultRegisterRC.size() - 1; r >= 0;)
                    {
                        if (freeRegistersParams && !param->resultRegisterRC.cannotFree)
                            toFree.push_back(param->resultRegisterRC[r]);
                        accParams.push_back(param->resultRegisterRC[r--]);
                        precallStack += sizeof(Register);
                        numPushParams++;
                        maxCallParams++;
                    }
                }
            }
        }

        emitPushRAParams(context, accParams, forVariadic);
    }

    // Pass a variadic parameter to another function
    auto numVariadic = (uint32_t) (numCallParams - numTypeParams) + 1;
    if (typeInfoFunc->flags & TYPEINFO_VARIADIC)
        SWAG_VERIFY(numVariadic <= SWAG_LIMIT_MAX_VARIADIC_PARAMS, context->report({allParams, Fmt(Err(Err0578), SWAG_LIMIT_MAX_VARIADIC_PARAMS, numVariadic)}));

    auto lastParam = allParams && !allParams->childs.empty() ? allParams->childs.back() : nullptr;

    if (lastParam && lastParam->typeInfo && lastParam->typeInfo->isTypedVariadic())
    {
        precallStack += 2 * sizeof(Register);
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // Pass a variadic parameter to another function
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->isVariadic())
    {
        precallStack += 2 * sizeof(Register);
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // If last parameter is a spread, then no need to deal with variadic slice : already done
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->flags & TYPEINFO_SPREAD)
    {
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRC[1], lastParam->resultRegisterRC[0]);
        maxCallParams += 2;
        precallStack += 2 * sizeof(Register);
        freeRegisterRC(context, lastParam);
    }

    // Variadic parameter is on top of stack
    else if (typeInfoFunc->flags & TYPEINFO_VARIADIC)
    {
        // The array of any has been pushed first, so we need to offset all pushed parameters to point to it
        // This is the main difference with typedvariadic, which directly point to the pushed variadic parameters
        auto offset = numPushParams;

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst                = EMIT_INST1(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32              = (uint32_t) offset * sizeof(Register);
        context->bc->maxSPVaargs = max(context->bc->maxSPVaargs, maxCallParams + 2);

        // If this is a closure, the first parameter is optionnal, depending on node->additionalRegisterRC[1] content
        // So we remove the first parameter by default, and will add it below is necessary
        if (node->typeInfo && node->typeInfo->isClosure())
            inst->b.u32 -= sizeof(Register);

        inst->d.pointer = (uint8_t*) typeInfoFunc;

        // In case of a real closure not affected to a lambda, we must count the first parameter
        // So we add sizeof(Register) to the CopySP pointer
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->extension);
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->extMisc()->additionalRegisterRC[1]);
            inst->flags |= BCI_NO_BACKEND;
            inst->b.s64 = 1;
            inst        = EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[0]);
            inst->b.s64 = sizeof(Register);
            inst->flags |= BCI_NO_BACKEND;
        }

        EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0[1])->b.u64 = numVariadic;
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, r0[1], r0[0]);
        maxCallParams += 2;

        precallStack += 2 * sizeof(Register);
    }
    else if (typeInfoFunc->flags & TYPEINFO_TYPED_VARIADIC)
    {
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
        auto offset       = (numPushParams - numVariadic * typeVariadic->rawType->numRegisters());

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst                = EMIT_INST1(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32              = (uint32_t) offset * sizeof(Register);
        context->bc->maxSPVaargs = max(context->bc->maxSPVaargs, maxCallParams + 2);

        // If this is a closure, the first parameter is optionnal, depending on node->additionalRegisterRC[1] content
        // So we remove the first parameter by default, and will add it below is necessary
        if (node->typeInfo && node->typeInfo->isClosure())
            inst->b.u32 -= sizeof(Register);

        inst->d.pointer = (uint8_t*) typeInfoFunc;

        // In case of a real closure not affected to a lambda, we must count the first parameter
        // So we add sizeof(Register) to the CopySP pointer
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->extension);
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->extMisc()->additionalRegisterRC[1]);
            inst->flags |= BCI_NO_BACKEND;
            inst->b.s64 = 1;
            inst        = EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[0]);
            inst->flags |= BCI_NO_BACKEND;
            inst->b.s64 = sizeof(Register);
        }

        EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0[1])->b.u64 = numVariadic;
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, r0[1], r0[0]);
        maxCallParams += 2;

        precallStack += 2 * sizeof(Register);
    }

    if (foreign)
    {
        auto inst               = EMIT_INST0(context, ByteCodeOp::ForeignCall);
        inst->a.pointer         = (uint8_t*) funcNode;
        inst->b.pointer         = (uint8_t*) typeInfoFunc;
        inst->numVariadicParams = (uint8_t) numVariadic;
        context->bc->hasForeignFunctionCallsModules.insert(ModuleManager::getForeignModuleName(funcNode));
    }
    else if (funcNode)
    {
        auto inst = EMIT_INST0(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(funcNode->extension && funcNode->extension->bytecode && funcNode->extByteCode()->bc);
        inst->a.pointer                     = (uint8_t*) funcNode->extByteCode()->bc;
        inst->b.pointer                     = (uint8_t*) typeInfoFunc;
        inst->numVariadicParams             = (uint8_t) numVariadic;
        funcNode->extByteCode()->bc->isUsed = true;
    }
    else
    {
        SWAG_ASSERT(varNodeRegisters.size() > 0);
        auto inst                            = EMIT_INST1(context, ByteCodeOp::LambdaCall, varNodeRegisters);
        inst->b.pointer                      = (uint8_t*) typeInfoFunc;
        inst->numVariadicParams              = (uint8_t) numVariadic;
        context->bc->hasForeignFunctionCalls = true;
    }

    // Free all registers now that the call can really be done
    for (auto r : toFree)
        freeRegisterRC(context, r);

    // Copy result in a computing register
    if (typeInfoFunc->returnType &&
        !typeInfoFunc->returnType->isVoid() &&
        !CallConv::returnByStackAddress(typeInfoFunc))
    {
        auto numRegs = typeInfoFunc->returnType->numRegisters();

        // Need to do that even if discard, not sure why
        context->bc->maxCallResults = max(context->bc->maxCallResults, numRegs);

        if (!(node->flags & AST_DISCARD))
        {
            reserveRegisterRC(context, node->resultRegisterRC, numRegs);
            if (numRegs == 1)
            {
                auto returnType = typeInfoFunc->concreteReturnType();
                EMIT_INST1(context, ByteCodeOp::CopyRTtoRA, node->resultRegisterRC[0]);

                if (node->semFlags & SEMFLAG_FROM_REF && !node->forceTakeAddress())
                {
                    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(typeInfoFunc->returnType, TypeInfoKind::Pointer);
                    SWAG_ASSERT(ptrPointer->flags & TYPEINFO_POINTER_REF);
                    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, ptrPointer->pointedType));
                }

                // :ReturnStructByValue
                else if (returnType->isStruct())
                {
                    SWAG_ASSERT(returnType->sizeOf <= sizeof(void*));
                    SWAG_ASSERT(node->computedValue);
                    EMIT_INST2(context, ByteCodeOp::SetAtStackPointer64, node->computedValue->storageOffset, node->resultRegisterRC);
                    EMIT_INST2(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC, node->computedValue->storageOffset);
                }
            }
            else
            {
                SWAG_ASSERT(numRegs == 2);
                SWAG_ASSERT(!(node->semFlags & SEMFLAG_FROM_REF));
                EMIT_INST2(context, ByteCodeOp::CopyRT2toRARB, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            }
        }
    }

    // Save the maximum number of pushraparams in that bytecode
    context->bc->maxCallParams = max(context->bc->maxCallParams, maxCallParams);

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->extension);
            EMIT_INST2(context, ByteCodeOp::IncSPPostCallCond, node->extMisc()->additionalRegisterRC[1], sizeof(void*));
            if (precallStack - sizeof(void*))
                EMIT_INST1(context, ByteCodeOp::IncSPPostCall, precallStack - sizeof(void*));
        }
        else
        {
            EMIT_INST1(context, ByteCodeOp::IncSPPostCall, precallStack);
        }
    }

    // If we are in a function that need to keep the RR0 register alive, we need to restore it
    if (rr0Saved)
    {
        EMIT_INST0(context, ByteCodeOp::PopRR);
    }

    // This is usefull when function call is inside an expression like func().something
    // The emitIdentifier will have to know the register where the result of func() is stored
    switch (node->parent->kind)
    {
    // To avoid a double release of the register in emitLoop... sight...
    case AstNodeKind::While:
        break;
    // Because resultRegisterRC is used to store the result
    case AstNodeKind::ConditionalExpression:
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
    SWAG_IF_ASSERT(uint32_t storageIndex = 0);
    if (funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto param                            = node->childs.back();
        auto resolved                         = param->resolvedSymbolOverload;
        resolved->computedValue.storageOffset = offset;
        offset += g_TypeMgr->typeInfoVariadic->sizeOf;
        SWAG_ASSERT(resolved->storageIndex == 0);
        SWAG_IF_ASSERT(storageIndex += 2);
    }

    auto childSize = node->childs.size();
    for (size_t i = 0; i < childSize; i++)
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
        SWAG_IF_ASSERT(storageIndex += numRegisters);
    }

    return true;
}

bool ByteCodeGenJob::emitBeforeFuncDeclContent(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);
    PushNode pn(context, funcNode->content);

    // Store the context in a persistent register
    if (funcNode->specFlags & AstFuncDecl::SPECFLAG_REG_GET_CONTEXT ||
        funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC ||
        funcNode->typeInfo->flags & TYPEINFO_CAN_THROW)
    {
        SWAG_ASSERT(funcNode->registerGetContext == UINT32_MAX);
        funcNode->registerGetContext = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::IntrinsicGetContext, funcNode->registerGetContext);
    }

    // Clear stack trace when entering a #<function>
    if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC &&
        context->sourceFile->module->buildCfg.errorStackTrace)
    {
        SWAG_ASSERT(funcNode->registerGetContext != UINT32_MAX);
        EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
    }

    // Should be aligned !
    SWAG_ASSERT(!(funcNode->stackSize & 7));

    if (funcNode->stackSize > g_CommandLine.stackSizeRT)
        Report::report({funcNode, Fmt(Err(Err0536), Utf8::toNiceSize(g_CommandLine.stackSizeRT).c_str())});

    context->bc->stackSize    = funcNode->stackSize;
    context->bc->dynStackSize = funcNode->stackSize;

    if (funcNode->stackSize == 0)
        EMIT_INST0(context, ByteCodeOp::SetBP);
    else
        EMIT_INST0(context, ByteCodeOp::DecSPBP)->a.u32 = funcNode->stackSize;

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

bool ByteCodeGenJob::makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(SemanticJob::makeInline((JobContext*) context, funcDecl, identifier));

    // Create a semantic job to resolve the inline part, and wait for that to be finished
    context->job->setPending(JobWaitKind::MakeInline, nullptr, funcDecl, nullptr);
    auto inlineNode = identifier->childs.back();
    SWAG_ASSERT(inlineNode->kind == AstNodeKind::Inline);
    auto job = SemanticJob::newJob(context->job->dependentJob, context->sourceFile, inlineNode, false);
    job->addDependentJob(context->job);
    context->job->jobsToAdd.push_back(job);
    return true;
}