#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Os/Os.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"

bool ByteCodeGen::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    const auto funcDecl = castAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);

    // Stack size could have been increased because of last minute inline operators.
    // So we must be sure the DecSPBP value is updated.
    context->bc->stackSize    = funcDecl->stackSize;
    context->bc->dynStackSize = funcDecl->stackSize;

    PushLocation pl(context, &funcDecl->content->token.endLocation);
    PushNode     pn(context, funcDecl->content);

    // No need to do the scope leave stuff if the function does return something, because
    // it has been covered by the mandatory return
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    if (!typeInfo->returnType->isVoid())
        return true;

    SWAG_CHECK(computeLeaveScope(context, funcDecl->scope));
    YIELD();

    emitDebugLine(context);
    EMIT_INST0(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGen::emitFuncCallParam(ByteCodeGenContext* context)
{
    const auto node = castAst<AstFuncCallParam>(context->node, AstNodeKind::FuncCallParam);

    // Special case when the parameter comes from an UFCS call that returns an interface.
    // In that case 'specUFCSNode' is the node that makes the call. The register will be
    // the object pointer of the returned interface.
    // :SpecUFCSNode
    if (node->specUFCSNode)
    {
        node->typeInfo         = node->specUFCSNode->typeInfo;
        node->resultRegisterRc = node->specUFCSNode->resultRegisterRc;
        return true;
    }

    const auto front = node->firstChild();

    // If we have a cast to an interface, be sure interface has been fully solved
    // Semantic will pass only if the interface has been registered in the struct, and not solved.
    // So we can reach that point and the interface is not done yet.
    // :WaitInterfaceReg
    if (node->typeInfoCast)
    {
        Semantic::waitAllStructInterfaces(context->baseJob, node->typeInfoCast);
        YIELD();
    }

    node->resultRegisterRc = front->resultRegisterRc;
    SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->typeInfoCast));
    return true;
}

bool ByteCodeGen::emitReturn(ByteCodeGenContext* context)
{
    AstReturn* node       = castAst<AstReturn>(context->node, AstNodeKind::Return);
    const auto funcNode   = node->ownerFct;
    TypeInfo*  returnType = nullptr;

    // Get the function return type. In case of an embedded return, this is the type of the original function to inline
    if (node->hasOwnerInline() && node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
        returnType = TypeManager::concreteType(node->ownerInline()->func->returnType->typeInfo, CONCRETE_FORCE_ALIAS);
    else
        returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FORCE_ALIAS);

    // Copy result to RR0... registers
    if (!node->hasSemFlag(SEMFLAG_EMIT_DEFERRED) && !node->children.empty() && !returnType->isVoid())
    {
        const auto returnExpression = node->firstChild();
        auto       backExpression   = node->lastChild();
        if (backExpression->is(AstNodeKind::Try) || backExpression->is(AstNodeKind::Catch) || backExpression->is(AstNodeKind::TryCatch))
            backExpression = backExpression->lastChild();
        const auto exprType = TypeManager::concretePtrRef(returnExpression->typeInfo);

        // Implicit cast
        if (!returnExpression->hasSemFlag(SEMFLAG_CAST1))
        {
            SWAG_CHECK(emitCast(context, returnExpression, TypeManager::concreteType(returnExpression->typeInfo), returnExpression->typeInfoCast));
            YIELD();
            returnExpression->addSemFlag(SEMFLAG_CAST1);
        }

        Semantic::waitStructGenerated(context->baseJob, exprType);
        YIELD();

        //
        // RETVAL
        //
        if (node->hasSemFlag(SEMFLAG_RETVAL) ||
            (backExpression->resolvedSymbolOverload() && backExpression->resolvedSymbolOverload()->hasFlag(OVERLOAD_RETVAL)))
        {
            const auto child = node->firstChild();
            freeRegisterRC(context, child->resultRegisterRc);
        }

        //
        // INLINE
        //
        else if (node->hasOwnerInline() && node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
        {
            if (returnType->isStruct())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->addAstFlag(AST_NO_LEFT_DROP);
                SWAG_CHECK(emitCopyStruct(context, node->ownerInline()->resultRegisterRc, returnExpression->resultRegisterRc, returnType, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else if (returnType->isArray())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->addAstFlag(AST_NO_LEFT_DROP);
                SWAG_CHECK(emitCopyArray(context, returnType, node->ownerInline()->resultRegisterRc, returnExpression->resultRegisterRc, returnExpression));
                freeRegisterRC(context, returnExpression);
            }
            else if (returnType->isClosure())
            {
                // :ConvertToClosure
                if (returnExpression->is(AstNodeKind::MakePointerLambda))
                {
                    // Copy closure capture buffer
                    // Do it first, because source stack can be shared with node->ownerInline->resultRegisterRC
                    const auto nodeCapture = castAst<AstMakePointer>(returnExpression, AstNodeKind::MakePointerLambda);
                    SWAG_ASSERT(nodeCapture->lambda->captureParameters);
                    const auto typeBlock = castTypeInfo<TypeInfoStruct>(nodeCapture->lastChild()->typeInfo, TypeInfoKind::Struct);
                    if (!typeBlock->fields.empty())
                    {
                        const auto r1 = reserveRegisterRC(context);
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r1, node->ownerInline()->resultRegisterRc);
                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = 2 * sizeof(void*);
                        emitMemCpy(context, r1, returnExpression->resultRegisterRc[1], typeBlock->sizeOf);
                        freeRegisterRC(context, r1);
                    }

                    EMIT_INST2(context, ByteCodeOp::SetAtPointer64, node->ownerInline()->resultRegisterRc, returnExpression->resultRegisterRc[0]);
                    const auto inst = EMIT_INST1(context, ByteCodeOp::SetAtPointer64, node->ownerInline()->resultRegisterRc);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u32 = 1; // <> 0 for closure, 0 for lambda
                    inst->c.u32 = sizeof(void*);
                }
                else
                {
                    emitMemCpy(context, node->ownerInline()->resultRegisterRc, returnExpression->resultRegisterRc, SWAG_LIMIT_CLOSURE_SIZEOF);
                }

                freeRegisterRC(context, returnExpression);
            }
            else
            {
                for (const auto child : node->children)
                {
                    const auto sizeChildren = child->resultRegisterRc.size();
                    for (uint32_t r = 0; r < sizeChildren; r++)
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->ownerInline()->resultRegisterRc[r], child->resultRegisterRc[r]);
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
                const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

                // :ReturnStructByValue
                if (CallConv::returnStructByValue(typeFunc))
                {
                    if (!typeFunc->returnType->hasFlag(TYPEINFO_STRUCT_EMPTY))
                    {
                        switch (typeFunc->returnType->sizeOf)
                        {
                            case 1:
                                EMIT_INST2(context, ByteCodeOp::DeRef8, returnExpression->resultRegisterRc, returnExpression->resultRegisterRc);
                                break;
                            case 2:
                                EMIT_INST2(context, ByteCodeOp::DeRef16, returnExpression->resultRegisterRc, returnExpression->resultRegisterRc);
                                break;
                            case 3:
                            case 4:
                                EMIT_INST2(context, ByteCodeOp::DeRef32, returnExpression->resultRegisterRc, returnExpression->resultRegisterRc);
                                break;
                            default:
                                EMIT_INST2(context, ByteCodeOp::DeRef64, returnExpression->resultRegisterRc, returnExpression->resultRegisterRc);
                                break;
                        }

                        EMIT_INST1(context, ByteCodeOp::CopyRAtoRR, returnExpression->resultRegisterRc);
                    }

                    freeRegisterRC(context, returnExpression->resultRegisterRc);
                }
                else
                {
                    // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                    returnExpression->addAstFlag(AST_NO_LEFT_DROP);
                    RegisterList r0 = reserveRegisterRC(context);
                    EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
                    SWAG_CHECK(emitCopyStruct(context, r0, returnExpression->resultRegisterRc, returnType, returnExpression));
                    freeRegisterRC(context, r0);
                    freeRegisterRC(context, returnExpression->resultRegisterRc);
                }
            }
            else if (returnType->isArray())
            {
                // Force raw copy (no drop on the left, i.e. the argument to return the result) because it has not been initialized
                returnExpression->addAstFlag(AST_NO_LEFT_DROP);
                RegisterList r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
                SWAG_CHECK(emitCopyArray(context, returnType, r0, returnExpression->resultRegisterRc, returnExpression));
                freeRegisterRC(context, r0);
                freeRegisterRC(context, returnExpression->resultRegisterRc);
            }
            else if (returnType->isString())
            {
                const auto child = node->firstChild();
                if (funcNode->hasAttribute(ATTRIBUTE_AST_FUNC))
                    EMIT_INST2(context, ByteCodeOp::CloneString, child->resultRegisterRc[0], child->resultRegisterRc[1]);
                EMIT_INST2(context, ByteCodeOp::CopyRARBtoRR2, child->resultRegisterRc[0], child->resultRegisterRc[1]);
                freeRegisterRC(context, child->resultRegisterRc);
            }
            else if (returnType->isClosure())
            {
                const auto child = node->firstChild();

                RegisterList r1 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r1);

                // :ConvertToClosure
                if (child->is(AstNodeKind::MakePointerLambda))
                {
                    EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r1, child->resultRegisterRc[0]);
                    const auto inst = EMIT_INST1(context, ByteCodeOp::SetAtPointer64, r1);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u32 = 1; // <> 0 for closure, 0 for lambda
                    inst->c.u32 = sizeof(void*);

                    // Copy closure capture buffer
                    const auto nodeCapture = castAst<AstMakePointer>(child, AstNodeKind::MakePointerLambda);
                    SWAG_ASSERT(nodeCapture->lambda->captureParameters);
                    const auto typeBlock = castTypeInfo<TypeInfoStruct>(nodeCapture->lastChild()->typeInfo, TypeInfoKind::Struct);
                    if (!typeBlock->fields.empty())
                    {
                        EMIT_INST1(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = 2 * sizeof(void*);
                        emitMemCpy(context, r1, child->resultRegisterRc[1], typeBlock->sizeOf);
                    }
                }
                else
                {
                    emitMemCpy(context, r1, child->resultRegisterRc, SWAG_LIMIT_CLOSURE_SIZEOF);
                }

                freeRegisterRC(context, r1);
                freeRegisterRC(context, child->resultRegisterRc);
            }
            else
            {
                SWAG_ASSERT(node->childCount() == 1);
                const auto child = node->firstChild();
                SWAG_ASSERT(child->resultRegisterRc.size() >= returnType->numRegisters());
                const auto numRetReg = returnType->numRegisters();
                SWAG_ASSERT(numRetReg <= 2);
                if (numRetReg == 1)
                    EMIT_INST1(context, ByteCodeOp::CopyRAtoRR, child->resultRegisterRc[0]);
                else if (numRetReg == 2)
                    EMIT_INST2(context, ByteCodeOp::CopyRARBtoRR2, child->resultRegisterRc[0], child->resultRegisterRc[1]);
                freeRegisterRC(context, child->resultRegisterRc);
            }
        }
    }

    node->addSemFlag(SEMFLAG_EMIT_DEFERRED);

    // Leave all scopes
    SWAG_CHECK(emitLeaveScopeReturn(context, &node->forceNoDrop, false));
    YIELD();

    // A return inside an inline function is just a jump to the end of the block
    if (node->hasOwnerInline() && node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
    {
        node->seekJump = context->bc->numInstructions;
        EMIT_INST0(context, ByteCodeOp::Jump);
        node->ownerInline()->returnList.push_back(node);
    }
    else
    {
        EMIT_INST0(context, ByteCodeOp::Ret);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicCVaStart(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto childDest = node->firstChild();

    const auto inst = EMIT_INST1(context, ByteCodeOp::IntrinsicCVaStart, childDest->resultRegisterRc);
    SWAG_ASSERT(node->ownerFct);
    SWAG_ASSERT(node->ownerFct->parameters);
    SWAG_ASSERT(!node->ownerFct->parameters->children.empty());
    const auto param = node->ownerFct->parameters->lastChild();
    SWAG_ASSERT(param->typeInfo->isCVariadic());
    const auto storageOffset = param->resolvedSymbolOverload()->computedValue.storageOffset;
    SWAG_ASSERT(storageOffset != UINT32_MAX);
    inst->b.u64         = storageOffset;
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
    inst->c.u32         = typeInfo->numTotalRegisters();
    freeRegisterRC(context, childDest);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicCVaEnd(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto childDest = node->firstChild();

    EMIT_INST1(context, ByteCodeOp::IntrinsicCVaEnd, childDest->resultRegisterRc);
    freeRegisterRC(context, childDest);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIntrinsicCVaArg(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto childDest = node->firstChild();

    node->resultRegisterRc = reserveRegisterRC(context);
    const auto inst        = EMIT_INST2(context, ByteCodeOp::IntrinsicCVaArg, childDest->resultRegisterRc, node->resultRegisterRc);
    inst->c.u64            = node->typeInfo->sizeOf;
    freeRegisterRC(context, childDest);
    return true;
}

bool ByteCodeGen::emitIntrinsic(ByteCodeGenContext* context)
{
    const auto node       = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    const auto callParams = castAst<AstNode>(node->firstChild(), AstNodeKind::FuncCallParams);

    // If the intrinsic is defined in runtime, then need to wait for the function bytecode
    // to be generated
    if (node->resolvedSymbolOverload()->node->hasAstFlag(AST_DEFINED_INTRINSIC))
    {
        askForByteCode(context->baseJob, node->resolvedSymbolOverload()->node, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
        YIELD();
    }

    // Some safety checks depending on the intrinsic
    emitSafetyIntrinsics(context);

    switch (node->token.id)
    {
        case TokenId::IntrinsicCompilerError:
        {
            auto child0 = callParams->firstChild();
            auto child1 = callParams->lastChild();
            EMIT_INST3(context, ByteCodeOp::IntrinsicCompilerError, child0->resultRegisterRc[0], child0->resultRegisterRc[1], child1->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicCompilerWarning:
        {
            auto child0 = callParams->firstChild();
            auto child1 = callParams->lastChild();
            EMIT_INST3(context, ByteCodeOp::IntrinsicCompilerWarning, child0->resultRegisterRc[0], child0->resultRegisterRc[1], child1->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicPanic:
        {
            auto child0 = callParams->firstChild();
            auto child1 = callParams->lastChild();
            EMIT_INST3(context, ByteCodeOp::IntrinsicPanic, child0->resultRegisterRc[0], child0->resultRegisterRc[1], child1->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicAssert:
        {
            auto child0 = callParams->firstChild();
            emitAssert(context, child0->resultRegisterRc, "assertion failed");
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
            auto child0            = callParams->firstChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::IntrinsicAlloc, node->resultRegisterRc, child0->resultRegisterRc);
            freeRegisterRC(context, child0);
            break;
        }
        case TokenId::IntrinsicFree:
        {
            auto child0 = callParams->firstChild();
            EMIT_INST1(context, ByteCodeOp::IntrinsicFree, child0->resultRegisterRc);
            freeRegisterRC(context, child0);
            break;
        }
        case TokenId::IntrinsicRealloc:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->lastChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST3(context, ByteCodeOp::IntrinsicRealloc, node->resultRegisterRc, child0->resultRegisterRc, child1->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicMemCpy:
        {
            auto childDest = callParams->firstChild();
            auto childSrc  = callParams->secondChild();
            auto childSize = callParams->children[2];
            EMIT_INST3(context, ByteCodeOp::IntrinsicMemCpy, childDest->resultRegisterRc, childSrc->resultRegisterRc, childSize->resultRegisterRc);
            freeRegisterRC(context, childDest);
            freeRegisterRC(context, childSrc);
            freeRegisterRC(context, childSize);
            break;
        }
        case TokenId::IntrinsicMemMove:
        {
            auto childDest = callParams->firstChild();
            auto childSrc  = callParams->secondChild();
            auto childSize = callParams->children[2];
            EMIT_INST3(context, ByteCodeOp::IntrinsicMemMove, childDest->resultRegisterRc, childSrc->resultRegisterRc, childSize->resultRegisterRc);
            freeRegisterRC(context, childDest);
            freeRegisterRC(context, childSrc);
            freeRegisterRC(context, childSize);
            break;
        }
        case TokenId::IntrinsicMemSet:
        {
            auto childDest  = callParams->firstChild();
            auto childValue = callParams->secondChild();
            auto childSize  = callParams->children[2];
            EMIT_INST3(context, ByteCodeOp::IntrinsicMemSet, childDest->resultRegisterRc, childValue->resultRegisterRc, childSize->resultRegisterRc);
            freeRegisterRC(context, childDest);
            freeRegisterRC(context, childValue);
            freeRegisterRC(context, childSize);
            break;
        }
        case TokenId::IntrinsicMemCmp:
        {
            auto childDest         = callParams->firstChild();
            auto childSrc          = callParams->secondChild();
            auto childSize         = callParams->children[2];
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST4(context, ByteCodeOp::IntrinsicMemCmp, node->resultRegisterRc, childDest->resultRegisterRc, childSrc->resultRegisterRc, childSize->resultRegisterRc);
            freeRegisterRC(context, childDest);
            freeRegisterRC(context, childSrc);
            freeRegisterRC(context, childSize);
            break;
        }
        case TokenId::IntrinsicStrLen:
        {
            auto childSrc          = callParams->firstChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::IntrinsicStrLen, node->resultRegisterRc, childSrc->resultRegisterRc);
            freeRegisterRC(context, childSrc);
            break;
        }
        case TokenId::IntrinsicStrCmp:
        {
            auto childSrc0         = callParams->firstChild();
            auto childSrc1         = callParams->secondChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST3(context, ByteCodeOp::IntrinsicStrCmp, node->resultRegisterRc, childSrc0->resultRegisterRc, childSrc1->resultRegisterRc);
            freeRegisterRC(context, childSrc0);
            freeRegisterRC(context, childSrc1);
            break;
        }
        case TokenId::IntrinsicTypeCmp:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto child2            = callParams->children[2];
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            freeRegisterRC(context, child2);
            break;
        }
        case TokenId::IntrinsicIs:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST3(context, ByteCodeOp::IntrinsicIs, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicAs:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto child2            = callParams->children[2];
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST4(context, ByteCodeOp::IntrinsicAs, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            freeRegisterRC(context, child2);
            break;
        }
        case TokenId::IntrinsicRtFlags:
        {
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST1(context, ByteCodeOp::IntrinsicRtFlags, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicStringCmp:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            node->resultRegisterRc = child1->resultRegisterRc[1];
            EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, child0->resultRegisterRc[0], child0->resultRegisterRc[1], child1->resultRegisterRc[0], child1->resultRegisterRc[1]);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1->resultRegisterRc[0]);
            break;
        }
        case TokenId::IntrinsicItfTableOf:
        {
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST3(context, ByteCodeOp::IntrinsicItfTableOf, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }
        case TokenId::IntrinsicDbgAlloc:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST1(context, ByteCodeOp::IntrinsicDbgAlloc, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicSysAlloc:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST1(context, ByteCodeOp::IntrinsicSysAlloc, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicGetContext:
        {
            node->resultRegisterRc = reserveRegisterRC(context);
            SWAG_ASSERT(node->identifierRef() == node->parent);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST1(context, ByteCodeOp::IntrinsicGetContext, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicSetContext:
        {
            auto childDest = callParams->firstChild();
            EMIT_INST1(context, ByteCodeOp::IntrinsicSetContext, childDest->resultRegisterRc);
            freeRegisterRC(context, childDest);
            break;
        }
        case TokenId::IntrinsicGetProcessInfos:
        {
            node->resultRegisterRc = reserveRegisterRC(context);
            SWAG_ASSERT(node->identifierRef() == node->parent);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST1(context, ByteCodeOp::IntrinsicGetProcessInfos, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicArguments:
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            node->parent->resultRegisterRc = node->resultRegisterRc;
            EMIT_INST2(context, ByteCodeOp::IntrinsicArguments, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            break;
        }
        case TokenId::IntrinsicModules:
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            node->parent->resultRegisterRc = node->resultRegisterRc;
            EMIT_INST2(context, ByteCodeOp::IntrinsicModules, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            break;
        }
        case TokenId::IntrinsicGvtd:
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            node->parent->resultRegisterRc = node->resultRegisterRc;
            EMIT_INST2(context, ByteCodeOp::IntrinsicGvtd, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            break;
        }
        case TokenId::IntrinsicCompiler:
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            node->parent->resultRegisterRc = node->resultRegisterRc;
            EMIT_INST2(context, ByteCodeOp::IntrinsicCompiler, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            emitPostCallUFCS(context);
            break;
        }
        case TokenId::IntrinsicIsByteCode:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST1(context, ByteCodeOp::IntrinsicIsByteCode, node->resultRegisterRc);
            break;
        }
        case TokenId::IntrinsicGetErr:
        {
            reserveRegisterRC(context, node->resultRegisterRc, 2);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            EMIT_INST2(context, ByteCodeOp::IntrinsicGetErr, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            break;
        }
        case TokenId::IntrinsicAtomicAdd:
        {
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS8, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS16, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS32, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAddS64, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS8, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS16, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS32, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicAndS64, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS8, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS16, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS32, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicOrS64, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS8, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS16, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS32, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXorS64, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS8, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS16, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS32, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST3(context, ByteCodeOp::IntrinsicAtomicXchgS64, child0->resultRegisterRc, child1->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc = reserveRegisterRC(context);
            auto child0            = callParams->firstChild();
            auto child1            = callParams->secondChild();
            auto child2            = callParams->children[2];
            auto typeInfo          = TypeManager::concreteType(child1->typeInfo);
            switch (typeInfo->nativeType)
            {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS8, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS16, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS32, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                    EMIT_INST4(context, ByteCodeOp::IntrinsicAtomicCmpXchgS64, child0->resultRegisterRc, child1->resultRegisterRc, child2->resultRegisterRc, node->resultRegisterRc);
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
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child0                             = callParams->firstChild();
            auto child1                             = callParams->secondChild();
            auto typeInfo                           = TypeManager::concreteType(child0->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            auto inst   = EMIT_INST3(context, op, node->resultRegisterRc, child0->resultRegisterRc, child1->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }

        case TokenId::IntrinsicBitCountNz:
        case TokenId::IntrinsicBitCountTz:
        case TokenId::IntrinsicBitCountLz:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child                              = callParams->firstChild();
            auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            auto inst   = EMIT_INST2(context, op, node->resultRegisterRc, child->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
            freeRegisterRC(context, child);
            break;
        }

        case TokenId::IntrinsicRol:
        case TokenId::IntrinsicRor:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child0                             = callParams->firstChild();
            auto child1                             = callParams->secondChild();
            auto typeInfo                           = TypeManager::concreteType(child0->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
            switch (typeInfo->nativeType)
            {
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
                default:
                    return Report::internalError(context->node, "emitIntrinsic, IntrinsicRol/IntrinsicRor invalid type");
            }

            auto inst   = EMIT_INST3(context, op, node->resultRegisterRc, child0->resultRegisterRc, child1->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
            freeRegisterRC(context, child0);
            freeRegisterRC(context, child1);
            break;
        }

        case TokenId::IntrinsicByteSwap:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child                              = callParams->firstChild();
            auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            auto inst   = EMIT_INST2(context, op, node->resultRegisterRc, child->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
            freeRegisterRC(context, child);
            break;
        }

        case TokenId::IntrinsicMulAdd:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto typeInfo                           = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            EMIT_INST4(context, op, node->resultRegisterRc, callParams->firstChild()->resultRegisterRc, callParams->secondChild()->resultRegisterRc, callParams->children[2]->resultRegisterRc);
            freeRegisterRC(context, callParams->firstChild());
            freeRegisterRC(context, callParams->secondChild());
            freeRegisterRC(context, callParams->children[2]);
            break;
        }

        case TokenId::IntrinsicMin:
        case TokenId::IntrinsicMax:
        {
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child0                             = callParams->firstChild();
            auto child1                             = callParams->secondChild();
            auto typeInfo                           = TypeManager::concreteType(child0->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            auto inst   = EMIT_INST3(context, op, node->resultRegisterRc, child0->resultRegisterRc, child1->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
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
            node->resultRegisterRc                  = reserveRegisterRC(context);
            node->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc          = node->resultRegisterRc;
            auto child                              = callParams->firstChild();
            auto typeInfo                           = TypeManager::concreteType(child->typeInfo);
            SWAG_ASSERT(typeInfo->isNative());
            auto op = ByteCodeOp::End;
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

            auto inst   = EMIT_INST2(context, op, node->resultRegisterRc, child->resultRegisterRc);
            inst->d.u32 = static_cast<uint32_t>(node->token.id);
            freeRegisterRC(context, child);
            break;
        }

        default:
            return Report::internalError(context->node, "emitIntrinsic, unknown intrinsic");
    }

    if (node->hasAstFlag(AST_DISCARD))
        freeRegisterRC(context, node->resultRegisterRc);

    return true;
}

bool ByteCodeGen::emitLambdaCall(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    const auto overload = node->resolvedSymbolOverload();

    SWAG_CHECK(emitIdentifier(context));
    node->allocateExtension(ExtensionKind::Misc);
    node->extMisc()->additionalRegisterRC = node->resultRegisterRc;
    const auto allParams                  = node->children.empty() ? nullptr : node->lastChild();
    SWAG_ASSERT(!allParams || allParams->is(AstNodeKind::FuncCallParams));

    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCE_ALIAS);

    if (node->isSilentCall())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(overload->typeInfo, TypeInfoKind::Array);
        typeRef            = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCE_ALIAS);
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
        inst->d.pointer = reinterpret_cast<uint8_t*>(context->node->resolvedSymbolOverload());

        // If 0, keep it 0, otherwise get the capture context by adding that offset to the address of the closure storage
        inst        = EMIT_INST2(context, ByteCodeOp::MulAddVC64, node->extMisc()->additionalRegisterRC[1], node->extMisc()->additionalRegisterRC[0]);
        inst->c.u64 = 16;

        // Deref function pointer
        inst            = EMIT_INST2(context, ByteCodeOp::DeRef64, node->extMisc()->additionalRegisterRC[0], node->extMisc()->additionalRegisterRC[0]);
        inst->d.pointer = reinterpret_cast<uint8_t*>(context->node->resolvedSymbolOverload());
    }

    emitSafetyNullCheck(context, node->extMisc()->additionalRegisterRC[0]);

    SWAG_CHECK(emitCall(context, allParams, nullptr, castAst<AstVarDecl>(overload->node), node->extMisc()->additionalRegisterRC, false, true));
    SWAG_ASSERT(context->result == ContextResult::Done);
    freeRegisterRC(context, node->extMisc()->additionalRegisterRC);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ByteCodeGen::emitPostCallUFCS(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    // :SpecUFCSNode
    // Specific case. The function returns an interface, so it returns two registers.
    // But we want that interface to be also an UFCS parameter.
    // Ex: var cfg = @compiler().getBuildCfg()
    if (node->typeInfo->isInterface() && node->hasAstFlag(AST_TO_UFCS))
    {
        const auto r = reserveRegisterRC(context);

        // So we need to remember the object pointer to be passed as a parameter.
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r, node->resultRegisterRc[0]);

        // And we need to put the itable pointer in the first register, in order for the lambda value (function pointer)
        // to be dereferenced.
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], node->resultRegisterRc[1]);

        // From now on, the register is the object pointer (not the itable). Used un emitFuncCallParam.
        node->resultRegisterRc = r;
    }
}

bool ByteCodeGen::emitCall(ByteCodeGenContext* context)
{
    const AstNode* node     = context->node;
    const auto     overload = node->resolvedSymbolOverload();
    const auto     funcNode = castAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    const auto allParams = node->children.empty() ? nullptr : node->lastChild();
    SWAG_ASSERT(!allParams || allParams->is(AstNodeKind::FuncCallParams));
    SWAG_CHECK(emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRc, false, true));
    YIELD();
    emitPostCallUFCS(context);
    return true;
}

void ByteCodeGen::computeSourceLocation(const JobContext* context, AstNode* node, uint32_t* storageOffset, DataSegment** storageSegment, bool forceCompiler)
{
    const auto seg  = Semantic::getConstantSegFromContext(context->node, forceCompiler);
    *storageSegment = seg;

    const auto sourceFile = node->token.sourceFile;
    const auto module     = sourceFile->module;

    const auto str = sourceFile->path;
    uint8_t*   addrName;
    const auto offsetName = seg->addString(str, &addrName);

    SourceLocationCache tmpLoc;
    tmpLoc.loc.fileName.buffer = addrName;
    tmpLoc.loc.fileName.count  = str.length();
    tmpLoc.loc.lineStart       = node->token.startLocation.line;
    tmpLoc.loc.colStart        = node->token.startLocation.column;
    tmpLoc.loc.lineEnd         = node->token.endLocation.line;
    tmpLoc.loc.colEnd          = node->token.endLocation.column;

    ScopedLock lock(module->mutexSourceLoc);

    // Is the same location is in the cache ?
    uint32_t crc  = Crc32::compute(addrName, str.length());
    crc           = Crc32::compute(reinterpret_cast<uint8_t*>(&node->token.startLocation.line), sizeof(uint32_t), crc);
    crc           = Crc32::compute(reinterpret_cast<uint8_t*>(&node->token.startLocation.column), sizeof(uint32_t), crc);
    crc           = Crc32::compute(reinterpret_cast<uint8_t*>(&node->token.endLocation.line), sizeof(uint32_t), crc);
    crc           = Crc32::compute(reinterpret_cast<uint8_t*>(&node->token.endLocation.column), sizeof(uint32_t), crc);
    const auto it = module->cacheSourceLoc.find(crc);
    if (it != module->cacheSourceLoc.end())
    {
        for (const auto& l : it->second)
        {
            if (l.loc.lineStart == node->token.startLocation.line &&
                l.loc.colStart == node->token.startLocation.column &&
                l.loc.lineEnd == node->token.endLocation.line &&
                l.loc.colEnd == node->token.endLocation.column &&
                l.storageSegment == seg &&
                !strncmp(static_cast<const char*>(l.loc.fileName.buffer), reinterpret_cast<const char*>(addrName), l.loc.fileName.count))
            {
                *storageOffset = l.storageOffset;
                return;
            }
        }
    }

    SwagSourceCodeLocation* loc;
    const auto              offset = seg->reserve(sizeof(SwagSourceCodeLocation), reinterpret_cast<uint8_t**>(&loc), sizeof(void*));
    seg->addInitPtr(offset, offsetName);
    std::copy_n(&tmpLoc.loc, 1, loc);

    // Store in the cache
    tmpLoc.storageSegment = seg;
    tmpLoc.storageOffset  = offset;
    if (it != module->cacheSourceLoc.end())
        it->second.push_back(tmpLoc);
    else
        module->cacheSourceLoc[crc] = VectorNative{tmpLoc};

    *storageOffset = offset;
}

bool ByteCodeGen::emitDefaultParamValue(ByteCodeGenContext* context, AstNode* param, RegisterList& regList)
{
    const auto node         = context->node;
    const auto defaultParam = castAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
    SWAG_ASSERT(defaultParam->assignment);

    if (defaultParam->assignment->is(AstNodeKind::CompilerSpecialValue))
    {
        switch (defaultParam->assignment->token.id)
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
                regList         = reserveRegisterRC(context);
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
                inst->b.u64     = static_cast<uint64_t>(g_CommandLine.target.os);
                break;
            }
            case TokenId::CompilerArch:
            {
                regList         = reserveRegisterRC(context);
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
                inst->b.u64     = static_cast<uint64_t>(g_CommandLine.target.arch);
                break;
            }

            case TokenId::CompilerSwagOs:
            {
                regList         = reserveRegisterRC(context);
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
                inst->b.u64     = static_cast<uint64_t>(OS::getNativeTarget().os);
                break;
            }

            case TokenId::CompilerBackend:
            {
                regList         = reserveRegisterRC(context);
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[0]);
                inst->b.u64     = static_cast<uint64_t>(g_CommandLine.backendGenType);
                break;
            }

            case TokenId::CompilerCallerFunction:
            case TokenId::CompilerBuildCfg:
            case TokenId::CompilerCpu:
            {
                reserveLinearRegisterRC2(context, regList);
                const auto str            = Semantic::getCompilerFunctionString(node, defaultParam->assignment->token.id);
                const auto storageSegment = Semantic::getConstantSegFromContext(context->node);
                const auto storageOffset  = storageSegment->addString(str);
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

    SWAG_ASSERT(defaultParam->assignment->hasFlagComputedValue());
    SWAG_CHECK(emitLiteral(context, defaultParam->assignment, defaultParam->typeInfo, regList));
    return true;
}

void ByteCodeGen::emitPushRAParams(const ByteCodeGenContext* context, VectorNative<uint32_t>& accParams, bool forVariadic)
{
    const auto node = context->node;

    int cpt = static_cast<int>(accParams.size());

    if (node->typeInfo && node->typeInfo->isClosure())
    {
        // The first argument of the function needs to be the capture context, which is stored in
        // node->additionalRegisterRC as the second register.
        SWAG_ASSERT(node->hasExtension());
        accParams[static_cast<int>(accParams.size()) - 1] = node->extMisc()->additionalRegisterRC[1];

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

        inst->flags.add(forVariadic ? BCI_VARIADIC : 0);
    }

    // Closure context
    if (node->typeInfo && node->typeInfo->isClosure())
        EMIT_INST2(context, ByteCodeOp::PushRAParamCond, accParams.back(), accParams.back());

    accParams.clear();
}

bool ByteCodeGen::checkCatchError(ByteCodeGenContext* context, AstNode* srcNode, const AstNode* callNode, const AstNode* funcNode, AstNode* parent, const TypeInfo* typeInfoFunc)
{
    const bool raiseErrors = typeInfoFunc->hasFlag(TYPEINFO_CAN_THROW);
    if (raiseErrors && !callNode->hasOwnerTryCatchAssume())
    {
        if (!srcNode)
            srcNode = typeInfoFunc->declNode;
        const Diagnostic err{callNode->token.sourceFile, callNode->token, formErr(Err0431, funcNode->token.cstr())};
        return context->report(err, Diagnostic::hereIs(srcNode, formNte(Nte0107, Naming::kindName(srcNode).cstr(), srcNode->token.cstr())));
    }

    if (!raiseErrors)
    {
        if (parent->is(AstNodeKind::Try) ||
            parent->is(AstNodeKind::Catch) ||
            parent->is(AstNodeKind::TryCatch) ||
            parent->is(AstNodeKind::Assume))
        {
            if (!srcNode)
                srcNode = typeInfoFunc->declNode;
            const Diagnostic err{parent, parent->token, formErr(Err0385, parent->token.cstr(), srcNode->token.cstr())};
            return context->report(err, Diagnostic::hereIs(srcNode));
        }
    }

    return true;
}

bool ByteCodeGen::emitReturnByCopyAddress(const ByteCodeGenContext* context, AstNode* node, const TypeInfoFuncAttr* typeInfoFunc)
{
    node->resultRegisterRc = reserveRegisterRC(context);

    auto testReturn = node;
    if (testReturn->is(AstNodeKind::Identifier))
    {
        // We need a copy in #ast functions
        if (!node->ownerFct || !node->ownerFct->hasAttribute(ATTRIBUTE_AST_FUNC))
            testReturn = testReturn->parent;
    }

    // If in a return expression, just push the caller retval
    AstNode* parentReturn = testReturn ? testReturn->inSimpleReturn() : nullptr;
    if (parentReturn)
    {
        // Must be the last expression in the return expression (no deref !)
        if (node->parent->isNot(AstNodeKind::IdentifierRef) || node == node->parent->lastChild())
        {
            if (node->hasOwnerInline())
            {
                SWAG_IF_ASSERT(const auto parentTypeFunc = castTypeInfo<TypeInfoFuncAttr>(node->ownerInline()->func->typeInfo, TypeInfoKind::FuncAttr));
                SWAG_ASSERT(CallConv::returnByStackAddress(parentTypeFunc));
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->ownerInline()->resultRegisterRc);
            }
            else
            {
                SWAG_IF_ASSERT(const auto parentTypeFunc = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr));
                SWAG_ASSERT(CallConv::returnByStackAddress(parentTypeFunc));
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->resultRegisterRc);
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRc);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->addSemFlag(SEMFLAG_RETVAL);
            return true;
        }
    }

    // A function call used to initialize the field of a struct
    // No need to put the result on the stack and copy the result later, just make a direct reference to
    // the field address
    testReturn = node;
    if (testReturn->is(AstNodeKind::Identifier))
        testReturn = testReturn->parent;
    if (testReturn->getParent(1)->is(AstNodeKind::FuncCallParam) &&
        testReturn->getParent(3)->is(AstNodeKind::Identifier) &&
        testReturn->getParent(5)->is(AstNodeKind::TypeExpression) &&
        testReturn->getParent(5)->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS) &&
        testReturn->getParent(6)->is(AstNodeKind::VarDecl) &&
        testReturn->getParent(3)->typeInfo &&
        testReturn->getParent(3)->typeInfo->isStruct())
    {
        const auto varNode  = castAst<AstVarDecl>(testReturn->parent->parent->parent->parent->parent->parent, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        const auto resolved = varNode->resolvedSymbolOverload();
        if (resolved->computedValue.storageOffset != UINT32_MAX)
        {
            const auto param = castAst<AstFuncCallParam>(testReturn->parent, AstNodeKind::FuncCallParam);
            SWAG_ASSERT(param->resolvedParameter);
            const auto typeParam = param->resolvedParameter;

            emitRetValRef(context, resolved, node->resultRegisterRc, false, resolved->computedValue.storageOffset + typeParam->offset);
            EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRc);
            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);

            testReturn->parent->addSemFlag(SEMFLAG_FIELD_STRUCT);
            return true;
        }
    }

    // Store in RR0 the address of the stack to store the result
    const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
    inst->b.u64     = node->computedValue()->storageOffset;
    EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRc);
    context->bc->maxCallResults = max(context->bc->maxCallResults, 1);

    if (node->resolvedSymbolOverload())
        node->resolvedSymbolOverload()->flags.add(OVERLOAD_EMITTED);

    // Push a var drop, except if we are in an expression (constexpr).
    // So check that the ownerScope will be executed (the bytecode should be a parent of the scope).
    if (context->bc->node->isParentOf(node->ownerScope->owner))
        node->ownerScope->symTable.addVarToDrop(node->resolvedSymbolOverload(), typeInfoFunc->returnType, node->computedValue()->storageOffset);

    if (node->hasAstFlag(AST_DISCARD))
        freeRegisterRC(context, node->resultRegisterRc);

    return true;
}

bool ByteCodeGen::emitCall(ByteCodeGenContext* context,
                           AstNode*            allParams,
                           AstFuncDecl*        funcNode,
                           AstVarDecl*         varNode,
                           RegisterList&       varNodeRegisters,
                           bool                foreign,
                           bool                freeRegistersParams)
{
    AstNode* node = context->node;

    TypeInfoFuncAttr* typeInfoFunc = nullptr;
    if (funcNode)
    {
        typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    }
    else if (varNode->typeInfo->isArray())
    {
        // :SilentCall
        auto typeArr = castTypeInfo<TypeInfoArray>(varNode->typeInfo, TypeInfoKind::Array);
        auto typeVar = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCE_ALIAS);
        typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
    }
    else if (varNode->typeInfo->isPointerRef())
    {
        auto typeVar = TypeManager::concretePtrRefType(varNode->typeInfo, CONCRETE_FORCE_ALIAS);
        typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
        EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
    }
    else
    {
        auto typeVar = TypeManager::concreteType(varNode->typeInfo, CONCRETE_FORCE_ALIAS);
        typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(typeVar, TypeInfoKind::LambdaClosure);
    }

    // Be sure referenced function has bytecode
    askForByteCode(context->baseJob, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
    YIELD();

    uint32_t numCallParams = allParams ? allParams->childCount() : 0;

    // For a untyped variadic, we need to store all parameters as 'any'
    // So we must generate one type per parameter
    Vector<uint32_t> storageOffsetsVariadicTypes;
    if (allParams && typeInfoFunc->hasFlag(TYPEINFO_VARIADIC))
    {
        auto  numFuncParams  = typeInfoFunc->parameters.size();
        auto  module         = context->sourceFile->module;
        auto& typeGen        = module->typeGen;
        auto  storageSegment = Semantic::getConstantSegFromContext(allParams);

        // We must export one type per parameter
        for (int i = static_cast<int>(numCallParams - 1); i >= static_cast<int>(numFuncParams - 1); i--)
        {
            auto     child        = allParams->children[i];
            auto     concreteType = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
            uint32_t storageOffset;
            context->baseJob = context->baseJob;
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, concreteType, storageSegment, &storageOffset));
            YIELD();
            storageOffsetsVariadicTypes.push_back(storageOffset);
        }
    }

    // Error, check validity.
    if (node->parent->is(AstNodeKind::IdentifierRef))
        SWAG_CHECK(checkCatchError(context, varNode, node, node, node->getParent(2), typeInfoFunc));
    else
        SWAG_CHECK(checkCatchError(context, varNode, node, node, node->parent, typeInfoFunc));

    // In a defer block, the RR registers (result of the function) are set before the block.
    // So we need to be sure that no defer statement will change them, to keep the return
    // values intact
    bool savedRR = false;
    if (node->ownerFct && node->hasAstFlag(AST_IN_DEFER))
    {
        auto ownerTypeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        auto ownerReturnType   = ownerTypeInfoFunc->concreteReturnType();
        if (!ownerReturnType->isVoid())
        {
            EMIT_INST0(context, ByteCodeOp::PushRR);
            savedRR = true;
        }
    }

    // Return by copy
    if (CallConv::returnByStackAddress(typeInfoFunc))
    {
        SWAG_CHECK(emitReturnByCopyAddress(context, node, typeInfoFunc));
    }

    uint32_t maxCallParams = typeInfoFunc->numReturnRegisters();

    // Sort children by parameter index
    if (allParams && allParams->hasAstFlag(AST_MUST_SORT_CHILDREN))
    {
        std::ranges::sort(allParams->children, [](AstNode* n1, AstNode* n2) {
            const auto p1 = castAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
            const auto p2 = castAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
            return p1->indexParam < p2->indexParam;
        });
    }
    else if (allParams && allParams->hasSemFlag(SEMFLAG_INVERSE_PARAMS))
    {
        SWAG_ASSERT(allParams->childCount() == 2);
        allParams->swap2Children();
    }

    // For a untyped variadic, we need to store all parameters as 'any'
    uint32_t               precallStack = 0;
    VectorNative<uint32_t> toFree;
    if (allParams && typeInfoFunc->hasFlag(TYPEINFO_VARIADIC))
    {
        auto     numFuncParams  = typeInfoFunc->parameters.size();
        auto     numVariadic    = (numCallParams - numFuncParams) + 1;
        uint32_t offset         = numVariadic * (2 * sizeof(Register));
        auto     storageSegment = Semantic::getConstantSegFromContext(allParams);

        for (int i = static_cast<int>(numCallParams - 1); i >= static_cast<int>(numFuncParams - 1); i--)
        {
            auto child     = allParams->children[i];
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
                EMIT_INST1(context, ByteCodeOp::PushRAParam, child->resultRegisterRc[0]);
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
                inst->c.u64 = child->resultRegisterRc[0];
                EMIT_INST1(context, ByteCodeOp::PushRAParam, r1);
                maxCallParams++;
            }

            precallStack += 2 * sizeof(Register);
            offset -= 2 * sizeof(Register);
            offset += sizeof(Register);
        }
    }

    // Push missing default parameters
    uint32_t numPushParams = 0;
    uint32_t numTypeParams = typeInfoFunc->parameters.size();
    if (numCallParams < numTypeParams)
    {
        // Push all parameters, from end to start
        VectorNative<uint32_t> accParams;
        for (uint32_t i = numTypeParams - 1; i != UINT32_MAX; i--)
        {
            // Determine if this parameter has been covered by the call
            bool covered = false;
            for (uint32_t j = 0; j < numCallParams; j++)
            {
                auto param = castAst<AstFuncCallParam>(allParams->children[j], AstNodeKind::FuncCallParam);
                if (param->indexParam == i)
                {
                    if (param->hasExtMisc() && !param->extMisc()->additionalRegisterRC.cannotFree)
                    {
                        for (uint32_t r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                    }

                    if (param->typeInfo->isArray() || param->typeInfo->hasFlag(TYPEINFO_LIST_ARRAY_ARRAY))
                        truncRegisterRC(context, param->resultRegisterRc, 1);

                    for (uint32_t r = param->resultRegisterRc.size() - 1; r != UINT32_MAX; r--)
                    {
                        if (freeRegistersParams && !param->resultRegisterRc.cannotFree)
                            toFree.push_back(param->resultRegisterRc[r]);
                        accParams.push_back(param->resultRegisterRc[r]);
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
                    if (typeInfoFunc->declNode->is(AstNodeKind::FuncDecl))
                    {
                        auto funcDesc = castAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl);
                        parameters    = funcDesc->parameters;
                    }
                    else
                    {
                        auto funcDesc = castAst<AstTypeLambda>(typeInfoFunc->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
                        parameters    = funcDesc->firstChild();
                    }
                }

                // Empty variadic parameter
                auto defaultParam = castAst<AstVarDecl>(parameters->children[i], AstNodeKind::FuncDeclParam);
                if (!defaultParam->typeInfo->isVariadic() &&
                    !defaultParam->typeInfo->isTypedVariadic() &&
                    !defaultParam->typeInfo->isCVariadic())
                {
                    SWAG_ASSERT(defaultParam->assignment);

                    RegisterList regList;
                    SWAG_CHECK(emitDefaultParamValue(context, defaultParam, regList));

                    for (uint32_t r = regList.size() - 1; r != UINT32_MAX;)
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
            auto typeVariadic = castTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
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
        for (uint32_t i = numCallParams - 1; i != UINT32_MAX; i--)
        {
            auto param = allParams->children[i];
            if (param->resultRegisterRc.size() == 0)
                continue;
            if (!param->typeInfo)
                continue;

            if (param->typeInfo->isArray() || param->typeInfo->hasFlag(TYPEINFO_LIST_ARRAY_ARRAY))
                truncRegisterRC(context, param->resultRegisterRc, 1);

            if (!param->typeInfo->isVariadic() &&
                !param->typeInfo->isTypedVariadic() &&
                !param->typeInfo->hasFlag(TYPEINFO_SPREAD))
            {
                bool done = false;

                // If this is a variadic parameter of a cvarargs, we need to promote the value
                if (typeInfoFunc->isFctCVariadic() &&
                    i >= numTypeParams - 1 &&
                    param->typeInfo->isNative() &&
                    param->typeInfo->sizeOf < sizeof(Register))
                {
                    switch (param->typeInfo->nativeType)
                    {
                        case NativeTypeKind::S8:
                            EMIT_INST2(context, ByteCodeOp::CastS8S32, param->resultRegisterRc, param->resultRegisterRc);
                            break;
                        case NativeTypeKind::S16:
                            EMIT_INST2(context, ByteCodeOp::CastS16S32, param->resultRegisterRc, param->resultRegisterRc);
                            break;
                        case NativeTypeKind::U8:
                        case NativeTypeKind::Bool:
                            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, param->resultRegisterRc)->b.u64 = 0x000000FF;
                            break;
                        case NativeTypeKind::U16:
                            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, param->resultRegisterRc)->b.u64 = 0x0000FFFF;
                            break;
                        case NativeTypeKind::F32:
                            EMIT_INST2(context, ByteCodeOp::CastF32F64, param->resultRegisterRc, param->resultRegisterRc);
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
                            for (uint32_t r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                                toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                        }

                        for (uint32_t r = param->resultRegisterRc.size() - 1; r != UINT32_MAX;)
                        {
                            if (freeRegistersParams && !param->resultRegisterRc.cannotFree)
                                toFree.push_back(param->resultRegisterRc[r]);
                            auto inst   = EMIT_INST1(context, ByteCodeOp::PushRVParam, param->resultRegisterRc[r--]);
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
                        for (uint32_t r = 0; freeRegistersParams && r < param->extMisc()->additionalRegisterRC.size(); r++)
                            toFree.push_back(param->extMisc()->additionalRegisterRC[r]);
                    }

                    for (uint32_t r = param->resultRegisterRc.size() - 1; r != UINT32_MAX;)
                    {
                        if (freeRegistersParams && !param->resultRegisterRc.cannotFree)
                            toFree.push_back(param->resultRegisterRc[r]);
                        accParams.push_back(param->resultRegisterRc[r--]);
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
    auto numVariadic = (numCallParams - numTypeParams) + 1;
    if (typeInfoFunc->hasFlag(TYPEINFO_VARIADIC))
    {
        if (numVariadic > SWAG_LIMIT_MAX_VARIADIC_PARAMS)
            return context->report({allParams, formErr(Err0494, SWAG_LIMIT_MAX_VARIADIC_PARAMS, numVariadic)});
    }

    auto lastParam = allParams && !allParams->children.empty() ? allParams->lastChild() : nullptr;

    if (lastParam && lastParam->typeInfo && lastParam->typeInfo->isTypedVariadic())
    {
        precallStack += 2 * sizeof(Register);
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRc[1], lastParam->resultRegisterRc[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // Pass a variadic parameter to another function
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->isVariadic())
    {
        precallStack += 2 * sizeof(Register);
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRc[1], lastParam->resultRegisterRc[0]);
        numPushParams += 2;
        maxCallParams += 2;
        freeRegisterRC(context, lastParam);
    }

    // If last parameter is a spread, then no need to deal with variadic slice : already done
    else if (lastParam && lastParam->typeInfo && lastParam->typeInfo->hasFlag(TYPEINFO_SPREAD))
    {
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, lastParam->resultRegisterRc[1], lastParam->resultRegisterRc[0]);
        maxCallParams += 2;
        precallStack += 2 * sizeof(Register);
        freeRegisterRC(context, lastParam);
    }

    // Variadic parameter is on top of stack
    else if (typeInfoFunc->hasFlag(TYPEINFO_VARIADIC))
    {
        // The array of any has been pushed first, so we need to offset all pushed parameters to point to it
        // This is the main difference with typed variadic, which directly point to the pushed variadic parameters
        auto offset = numPushParams;

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst                = EMIT_INST1(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32              = offset * sizeof(Register);
        context->bc->maxSpVaargs = max(context->bc->maxSpVaargs, maxCallParams + 2);

        // If this is a closure, the first parameter is optional, depending on node->additionalRegisterRC[1] content
        // So we remove the first parameter by default, and will add it below is necessary
        if (node->typeInfo && node->typeInfo->isClosure())
            inst->b.u32 -= sizeof(Register);

        inst->d.pointer = reinterpret_cast<uint8_t*>(typeInfoFunc);

        // In case of a real closure not affected to a lambda, we must count the first parameter
        // So we add sizeof(Register) to the CopySP pointer
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->hasExtension());
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->extMisc()->additionalRegisterRC[1]);
            inst->addFlag(BCI_NO_BACKEND);
            inst->b.s64 = 1;
            inst        = EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[0]);
            inst->b.s64 = sizeof(Register);
            inst->addFlag(BCI_NO_BACKEND);
        }

        EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0[1])->b.u64 = numVariadic;
        EMIT_INST2(context, ByteCodeOp::PushRAParam2, r0[1], r0[0]);
        maxCallParams += 2;

        precallStack += 2 * sizeof(Register);
    }
    else if (typeInfoFunc->hasFlag(TYPEINFO_TYPED_VARIADIC))
    {
        auto typeVariadic = castTypeInfo<TypeInfoVariadic>(typeInfoFunc->parameters.back()->typeInfo, TypeInfoKind::TypedVariadic);
        auto offset       = numPushParams - numVariadic * typeVariadic->rawType->numRegisters();

        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        toFree.push_back(r0[0]);
        toFree.push_back(r0[1]);

        auto inst                = EMIT_INST1(context, ByteCodeOp::CopySPVaargs, r0[0]);
        inst->b.u32              = offset * sizeof(Register);
        context->bc->maxSpVaargs = max(context->bc->maxSpVaargs, maxCallParams + 2);

        // If this is a closure, the first parameter is optional, depending on node->additionalRegisterRC[1] content
        // So we remove the first parameter by default, and will add it below is necessary
        if (node->typeInfo && node->typeInfo->isClosure())
            inst->b.u32 -= sizeof(Register);

        inst->d.pointer = reinterpret_cast<uint8_t*>(typeInfoFunc);

        // In case of a real closure not affected to a lambda, we must count the first parameter
        // So we add sizeof(Register) to the CopySP pointer
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->hasExtension());
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->extMisc()->additionalRegisterRC[1]);
            inst->addFlag(BCI_NO_BACKEND);
            inst->b.s64 = 1;
            inst        = EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0[0]);
            inst->addFlag(BCI_NO_BACKEND);
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
        inst->a.pointer         = reinterpret_cast<uint8_t*>(funcNode);
        inst->b.pointer         = reinterpret_cast<uint8_t*>(typeInfoFunc);
        inst->numVariadicParams = static_cast<uint8_t>(numVariadic);
        context->bc->hasForeignFunctionCallsModules.insert(ModuleManager::getForeignModuleName(funcNode));
    }
    else if (funcNode)
    {
        auto inst = EMIT_INST0(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(funcNode->hasExtSemantic() && funcNode->extByteCode()->bc);
        inst->a.pointer                     = reinterpret_cast<uint8_t*>(funcNode->extByteCode()->bc);
        inst->b.pointer                     = reinterpret_cast<uint8_t*>(typeInfoFunc);
        inst->numVariadicParams             = static_cast<uint8_t>(numVariadic);
        funcNode->extByteCode()->bc->isUsed = true;
    }
    else
    {
        SWAG_ASSERT(varNodeRegisters.size() > 0);
        auto inst                            = EMIT_INST1(context, ByteCodeOp::LambdaCall, varNodeRegisters);
        inst->b.pointer                      = reinterpret_cast<uint8_t*>(typeInfoFunc);
        inst->numVariadicParams              = static_cast<uint8_t>(numVariadic);
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

        if (!node->hasAstFlag(AST_DISCARD))
        {
            reserveRegisterRC(context, node->resultRegisterRc, numRegs);
            if (numRegs == 1)
            {
                auto returnType = typeInfoFunc->concreteReturnType();
                EMIT_INST1(context, ByteCodeOp::CopyRTtoRA, node->resultRegisterRc[0]);

                if (node->hasSemFlag(SEMFLAG_FROM_REF) && !node->isForceTakeAddress())
                {
                    auto ptrPointer = castTypeInfo<TypeInfoPointer>(typeInfoFunc->returnType, TypeInfoKind::Pointer);
                    SWAG_ASSERT(ptrPointer->isPointerRef());
                    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRc, ptrPointer->pointedType));
                }

                // :ReturnStructByValue
                else if (returnType->isStruct())
                {
                    SWAG_ASSERT(returnType->sizeOf <= sizeof(void*));
                    SWAG_ASSERT(node->computedValue());
                    EMIT_INST2(context, ByteCodeOp::SetAtStackPointer64, node->computedValue()->storageOffset, node->resultRegisterRc);
                    EMIT_INST2(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc, node->computedValue()->storageOffset);
                }
            }
            else
            {
                SWAG_ASSERT(numRegs == 2);
                EMIT_INST2(context, ByteCodeOp::CopyRT2toRARB, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            }
        }
    }

    // Save the maximum number of push params in that bytecode
    context->bc->maxCallParams = max(context->bc->maxCallParams, maxCallParams);

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        if (node->typeInfo && node->typeInfo->isClosure())
        {
            SWAG_ASSERT(node->hasExtension());
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
    if (savedRR)
    {
        EMIT_INST0(context, ByteCodeOp::PopRR);
    }

    // This is useful when function call is inside an expression like func().something
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
            node->parent->resultRegisterRc = node->resultRegisterRc;
            break;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitFuncDeclParams(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    const auto funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);

    // 3 pointers are already on that stack after BP : saved BP, BC and IP.
    uint32_t offset = 3 * sizeof(void*);

    // Variadic parameter is the last one pushed on the stack
    SWAG_IF_ASSERT(uint32_t storageIndex = 0);
    if (funcNode->typeInfo->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        const auto param                      = node->lastChild();
        const auto resolved                   = param->resolvedSymbolOverload();
        resolved->computedValue.storageOffset = offset;
        offset += g_TypeMgr->typeInfoVariadic->sizeOf;
        SWAG_ASSERT(resolved->storageIndex == 0);
        SWAG_IF_ASSERT(storageIndex += 2);
    }

    // Preallocate 32 registers, so that we can assign registers to the function parameters which
    // have more chanced to be reused.
    for (uint32_t i = 0; i < 32; i++)
        freeRegisterRC(context, context->bc->maxReservedRegisterRC++);

    const auto childCount = node->childCount();
    for (uint32_t i = 0; i < childCount; i++)
    {
        if (i == childCount - 1 && funcNode->typeInfo->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
            break;

        const auto param                      = node->children[i];
        const auto resolved                   = param->resolvedSymbolOverload();
        resolved->computedValue.storageOffset = offset;
        SWAG_ASSERT(resolved->storageIndex == storageIndex);

        RegisterList rc;
        rc += context->bc->maxReservedRegisterRC++;
        if (resolved->typeInfo->numRegisters() == 2)
            rc += context->bc->maxReservedRegisterRC++;
        resolved->setRegisters(rc, OVERLOAD_HINT_REG);
        freeRegisterRC(context, rc);

        const auto     typeInfo     = TypeManager::concreteType(resolved->typeInfo);
        const uint32_t numRegisters = typeInfo->numRegisters();
        offset += numRegisters * sizeof(Register);
        SWAG_IF_ASSERT(storageIndex += numRegisters);
    }

    return true;
}

bool ByteCodeGen::emitBeforeFuncDeclContent(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    auto       funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);
    PushNode pn(context, funcNode->content);

    // Store the context in a persistent register
    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT) ||
        funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW) ||
        funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME) ||
        funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC))
    {
        SWAG_ASSERT(funcNode->registerGetContext == UINT32_MAX);
        funcNode->registerGetContext = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::IntrinsicGetContext, funcNode->registerGetContext);
    }

    // Store RR if input value is defined (return by address)
    // If we are in a function that need to keep the RR0 register alive, we need to save it
    const auto ownerTypeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
    const auto ownerReturnType   = ownerTypeInfoFunc->concreteReturnType();
    if (!ownerReturnType->isVoid())
    {
        if (CallConv::returnByStackAddress(ownerTypeInfoFunc))
        {
            SWAG_ASSERT(funcNode->registerStoreRR == UINT32_MAX);
            funcNode->registerStoreRR = reserveRegisterRC(context);
            const auto inst           = EMIT_INST1(context, ByteCodeOp::SaveRRtoRA, funcNode->registerStoreRR);
            inst->addFlag(BCI_NOT_PURE);
        }
    }

    // Clear stack trace when entering a #<function>
    if (funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC) &&
        context->sourceFile->module->buildCfg.errorStackTrace)
    {
        SWAG_ASSERT(funcNode->registerGetContext != UINT32_MAX);
        EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
    }

    // Should be aligned !
    SWAG_ASSERT(!(funcNode->stackSize & 7));

    if (funcNode->stackSize > g_CommandLine.limitStackRT)
        Report::report({funcNode, formErr(Err0026, Utf8::toNiceSize(g_CommandLine.limitStackRT).cstr())});

    context->bc->stackSize    = funcNode->stackSize;
    context->bc->dynStackSize = funcNode->stackSize;
    context->bc->emitSPSize   = funcNode->stackSize;
    EMIT_INST0(context, ByteCodeOp::DecSPBP);

    return true;
}

bool ByteCodeGen::emitForeignCall(ByteCodeGenContext* context)
{
    const AstNode* node      = context->node;
    const auto     overload  = node->resolvedSymbolOverload();
    const auto     funcNode  = castAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    const auto     allParams = node->children.empty() ? nullptr : node->lastChild();
    SWAG_ASSERT(!allParams || allParams->is(AstNodeKind::FuncCallParams));
    emitCall(context, allParams, funcNode, nullptr, funcNode->resultRegisterRc, true, true);
    return true;
}

bool ByteCodeGen::makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(Semantic::makeInline(context, funcDecl, identifier));

    // Create a semantic job to resolve the inline part, and wait for that to be finished
    context->baseJob->setPending(JobWaitKind::MakeInline, nullptr, funcDecl, nullptr);
    const auto inlineNode = identifier->lastChild();
    SWAG_ASSERT(inlineNode->is(AstNodeKind::Inline));
    const auto job = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, inlineNode, false);
    job->addDependentJob(context->baseJob);
    context->baseJob->jobsToAdd.push_back(job);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitCastAs(ByteCodeGenContext* context)
{
    const auto node        = context->node;
    const auto child0      = node->firstChild();
    const auto child1      = node->secondChild();
    node->resultRegisterRc = reserveRegisterRC(context);

    const auto r0 = reserveRegisterRC(context);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0, child0->resultRegisterRc[0]);
    SWAG_CHECK(emitKindOfInterface(context, child0));
    
    EMIT_INST4(context, ByteCodeOp::IntrinsicAs, child1->resultRegisterRc, child0->resultRegisterRc, r0, node->resultRegisterRc);
    freeRegisterRC(context, child0);
    freeRegisterRC(context, child1);
    freeRegisterRC(context, r0);
    
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitCastIs(ByteCodeGenContext* context)
{
    const auto node        = context->node;
    const auto child0      = node->firstChild();
    const auto child1      = node->secondChild();
    node->resultRegisterRc = reserveRegisterRC(context);
   
    SWAG_CHECK(emitKindOfInterface(context, child0));
    EMIT_INST3(context, ByteCodeOp::IntrinsicIs, child1->resultRegisterRc, child0->resultRegisterRc, node->resultRegisterRc);

    freeRegisterRC(context, child0);
    freeRegisterRC(context, child1);
    return true;
}
