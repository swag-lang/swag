#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenContext.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Report.h"
#include "Symbol.h"
#include "TypeManager.h"

bool ByteCodeGen::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRc = node->children.back()->resultRegisterRc;
    return true;
}

bool ByteCodeGen::sameStackFrame(ByteCodeGenContext* context, const SymbolOverload* overload)
{
    if (context->node->isSameStackFrame(overload))
        return true;

    const Diagnostic err{context->node, FMT(Err(Err0081), Naming::kindName(overload).c_str(), overload->symbol->name.c_str())};

    Vector<const Diagnostic*> notes;

    notes.push_back(Diagnostic::hereIs(overload));
    if (context->node->ownerFct && context->node->ownerFct->hasAttribute(ATTRIBUTE_GENERATED_FUNC))
        notes.push_back(Diagnostic::note(FMT(Nte(Nte0194), Naming::kindName(overload).c_str(), context->node->ownerFct->getDisplayName().c_str())));
    if (overload->fromInlineParam)
        notes.push_back(Diagnostic::note(overload->fromInlineParam, FMT(Nte(Nte0077), overload->symbol->name.c_str())));

    return context->report(err, notes);
}

bool ByteCodeGen::emitIdentifier(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (!node->hasAstFlag(AST_L_VALUE) && !node->hasAstFlag(AST_R_VALUE))
        return true;

    const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
    const auto resolved   = node->resolvedSymbolOverload;
    const auto typeInfo   = TypeManager::concreteType(resolved->typeInfo);
    SWAG_VERIFY(!typeInfo->isKindGeneric(), Report::internalError(context->node, "emitIdentifier, type is generic"));

    // If this is a retval, then just copy the return pointer register to a computing register
    if (resolved->hasFlag(OVERLOAD_RETVAL))
    {
        const RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, resolved, r0, true, 0);
        identifier->resultRegisterRc                  = r0;
        identifier->identifierRef()->resultRegisterRc = identifier->resultRegisterRc;
        identifier->parent->resultRegisterRc          = node->resultRegisterRc;
        return true;
    }

    // Will be done in the variable declaration
    if (typeInfo->isStruct())
    {
        if (node->hasAstFlag(AST_IN_TYPE_VAR_DECLARATION))
            return true;
    }

    // A captured variable
    if (resolved->hasFlag(OVERLOAD_VAR_CAPTURE))
    {
        node->resultRegisterRc                        = reserveRegisterRC(context);
        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;

        // Get capture block pointer (first parameter)
        const auto inst    = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
        inst->b.u64u32.low = node->ownerFct->parameters->children.front()->resolvedSymbolOverload->computedValue.storageOffset;

        // :VariadicAndClosure
        // If function is variable, then parameter of the capture context is 2 (after the slice), and not 0.
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
        if (typeFunc->isFctVariadic())
            inst->b.u64u32.high = 2;

        if (typeInfo->isArray() ||
            typeInfo->isListTuple() ||
            typeInfo->isListArray() ||
            typeInfo->isStruct())
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRc[0])->b.u64 = resolved->computedValue.storageOffset;
            return true;
        }

        if (node->hasSemFlag(SEMFLAG_FROM_REF))
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRc[0])->b.u64 = resolved->computedValue.storageOffset;
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            return true;
        }

        if (node->isForceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRc[0])->b.u64 = resolved->computedValue.storageOffset;
            if (node->parent->hasAstFlag(AST_ARRAY_POINTER_REF))
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            return true;
        }

        if (typeInfo->isInterface() && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->hasAstFlag(AST_FROM_UFCS))
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = resolved->computedValue.storageOffset + 8;
                // Get the structure pointer
            else
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = resolved->computedValue.storageOffset;

            return true;
        }

        if (typeInfo->numRegisters() == 2)
        {
            transformResultToLinear2(context, node->resultRegisterRc);
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc[1], node->resultRegisterRc[0])->c.u64 = resolved->computedValue.storageOffset + 8;
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc[0], node->resultRegisterRc[0])->c.u64 = resolved->computedValue.storageOffset;

            identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc                = node->resultRegisterRc;
            return true;
        }

        if (typeInfo->numRegisters() == 1)
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc[0], node->resultRegisterRc[0])->c.u64 = resolved->computedValue.storageOffset;
            return true;
        }
    }

    // Variable from the tls segment
    bool forStruct = false;
    if (resolved->hasFlag(OVERLOAD_VAR_TLS))
    {
        node->resultRegisterRc = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::InternalGetTlsPtr, node->resultRegisterRc);
        forStruct = true;
    }

    // Variable from a struct
    else if (resolved->hasFlag(OVERLOAD_VAR_STRUCT))
    {
        node->resultRegisterRc = identifier->identifierRef()->resultRegisterRc;
        SWAG_VERIFY(node->resultRegisterRc.size() > 0,
                    Report::internalError(context->node, FMT("emitIdentifier, cannot reference identifier [[%s]]", identifier->token.c_str()).c_str()));
        forStruct = true;
    }

    if (forStruct)
    {
        SWAG_ASSERT(!resolved->hasFlag(OVERLOAD_VAR_INLINE));
        if (resolved->computedValue.storageOffset > 0)
        {
            ensureCanBeChangedRC(context, node->resultRegisterRc);

            // :UfcsItfInlined
            // Very specific case where an inlined call returns an interface, and we directly call a lambda of that interface.
            // In that case we want to take the register that defined the vtable, not the object.
            if (identifier != identifier->parent->children.front())
            {
                const auto idIdx = identifier->childParentIdx();
                const auto prev  = identifier->identifierRef()->children[idIdx - 1];
                if (!prev->children.empty())
                {
                    const auto back = prev->children.back();
                    if (back->kind == AstNodeKind::Inline)
                    {
                        if (back->typeInfo->isInterface())
                        {
                            SWAG_ASSERT(node->resultRegisterRc.countResults == 2);
                            swap(node->resultRegisterRc.oneResult[0], node->resultRegisterRc.oneResult[1]);
                        }
                    }
                }
            }

            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRc, 0, node->resultRegisterRc);
            SWAG_ASSERT(resolved->computedValue.storageOffset != UINT32_MAX);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->addFlag(BCI_IMM_B);
        }

        auto typeField = node->typeInfo;
        if (node->hasSemFlag(SEMFLAG_FROM_REF))
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            const auto ptrPointer = castTypeInfo<TypeInfoPointer>(typeField, TypeInfoKind::Pointer);
            SWAG_ASSERT(ptrPointer->isPointerRef());
            typeField = ptrPointer->pointedType;
        }

        if (!node->isForceTakeAddress())
            emitStructDeRef(context, typeField);
        else if (node->parent->hasAstFlag(AST_ARRAY_POINTER_REF))
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);

        // :SilentCall
        if (resolved->hasFlag(OVERLOAD_VAR_TLS) && node->isSilentCall())
            freeRegisterRC(context, node->parent);

        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;
        return true;
    }

    // Function parameter : it's a register on the stack
    if (resolved->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
    {
        node->resultRegisterRc = reserveRegisterRC(context, resolved);
        resolved->setRegisters(node->resultRegisterRc, OVERLOAD_HINT_REG);

        // Get a parameter from a #validif block... this is special
        if (node->isValidIfParam(resolved))
        {
            ByteCodeInstruction* inst;
            if (typeInfo->numRegisters() == 2)
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRc);
                inst = EMIT_INST2(context, ByteCodeOp::GetParam64SI, node->resultRegisterRc[0], node->resultRegisterRc[1]);
            }
            else
            {
                inst = EMIT_INST2(context, ByteCodeOp::GetParam64SI, node->resultRegisterRc[0], node->resultRegisterRc[0]);
            }

            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(resolved->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            inst->c.u64         = typeFunc->registerIdxToParamIdx(resolved->storageIndex);
            inst->d.pointer     = reinterpret_cast<uint8_t*>(resolved);

            identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
            node->parent->resultRegisterRc                = node->resultRegisterRc;
            return true;
        }

        SWAG_CHECK(sameStackFrame(context, resolved));

        if (node->isSilentCall())
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->parent->resultRegisterRc);
            freeRegisterRC(context, node->parent);
        }
        else if (node->hasSemFlag(SEMFLAG_FROM_REF))
        {
            const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset;
            inst->b.u64u32.high = resolved->storageIndex;
            if (!node->isForceTakeAddress())
            {
                const auto ptrPointer = castTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
                SWAG_ASSERT(ptrPointer->isPointerRef());
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRc, ptrPointer->pointedType));
            }
        }
        else if (node->isForceTakeAddress() && !typeInfo->isLambdaClosure() && !typeInfo->isArray())
        {
            if (node->parent->hasAstFlag(AST_ARRAY_POINTER_REF))
            {
                const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
                inst->b.u64u32.low  = resolved->computedValue.storageOffset;
                inst->b.u64u32.high = resolved->storageIndex;
            }
            else if (typeInfo->isSlice())
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRc);

                auto inst           = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc[0]);
                inst->b.u64u32.low  = resolved->computedValue.storageOffset;
                inst->b.u64u32.high = resolved->storageIndex;

                inst                = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc[1]);
                inst->b.u64u32.low  = resolved->computedValue.storageOffset;
                inst->b.u64u32.high = resolved->storageIndex + 1;
            }
            else
            {
                const Diagnostic err{node, node->token, FMT(Err(Err0181), typeInfo->getDisplayNameC())};
                return context->report(err);
            }
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset;
            inst->b.u64u32.high = resolved->storageIndex;
            if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = 8;
            else // Get the structure pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        }
        else if (typeInfo->isInterface() && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->hasAstFlag(AST_FROM_UFCS))
            {
                const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
                inst->b.u64u32.low  = resolved->computedValue.storageOffset + 8;
                inst->b.u64u32.high = resolved->storageIndex + 1;
            }
            // Get the structure pointer
            else
            {
                const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
                inst->b.u64u32.low  = resolved->computedValue.storageOffset;
                inst->b.u64u32.high = resolved->storageIndex;
            }
        }
        else if (typeInfo->isClosure())
        {
            const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset;
            inst->b.u64u32.high = resolved->storageIndex;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            auto inst           = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc[0]);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset;
            inst->b.u64u32.high = resolved->storageIndex;
            inst                = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc[1]);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset + 8;
            inst->b.u64u32.high = resolved->storageIndex + 1;
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            const auto inst     = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRc);
            inst->b.u64u32.low  = resolved->computedValue.storageOffset;
            inst->b.u64u32.high = resolved->storageIndex;
            SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRc, node->typeInfo));
        }

        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;
        return true;
    }

    // Variable from the data segment
    if (resolved->hasFlag(OVERLOAD_VAR_GLOBAL))
    {
        node->resultRegisterRc = reserveRegisterRC(context);

        if (node->isSilentCall())
        {
            const auto typeArr   = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            const auto finalType = TypeManager::concreteType(typeArr->finalType, CONCRETE_ALL);

            // A closure is the pointer to the data, not a pointer to the function.
            // So we must not dereference in that case
            if (finalType->isClosure())
            {
                freeRegisterRC(context, node);
                node->resultRegisterRc = node->parent->resultRegisterRc;
            }
            else
            {
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->parent->resultRegisterRc);
                freeRegisterRC(context, node->parent);
            }
        }
        else if (node->hasSemFlag(SEMFLAG_FROM_REF))
        {
            return Report::internalError(context->node, "unsupported identifier reference type");
        }
        else if (typeInfo->isArray() ||
                 typeInfo->isListTuple() ||
                 typeInfo->isListArray() ||
                 typeInfo->isStruct())
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc);
            if (node->isForceTakeAddress())
                inst->c.pointer = reinterpret_cast<uint8_t*>(resolved);
        }
        else if (node->isForceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc);
            inst->c.pointer           = reinterpret_cast<uint8_t*>(resolved);
            if (node->parent->hasAstFlag(AST_ARRAY_POINTER_REF))
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc, 64);
            if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
            else if (node->hasAstFlag(AST_TO_UFCS)) // Get the struct pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        }
        else if (typeInfo->isInterface() && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc);
            if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
            else if (node->hasAstFlag(AST_TO_UFCS)) // Get the struct pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        }
        else if (typeInfo->isClosure())
        {
            emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc);
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc[0], 64);
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset + 8, node->resultRegisterRc[1], 64);
        }
        else
        {
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRc, typeInfo->sizeOf * 8);
            SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRc, node->typeInfo));
        }

        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;
        return true;
    }

    // Variable from the stack
    if (resolved->hasFlag(OVERLOAD_VAR_LOCAL))
    {
        SWAG_CHECK(sameStackFrame(context, resolved));
        const auto persistentReg = resolved->hasFlag(OVERLOAD_PERSISTENT_REG) && !(context->contextFlags & BCC_FLAG_FOR_DEBUGGER);

        if (node->isSilentCall())
        {
            SWAG_ASSERT(!resolved->hasFlag(OVERLOAD_PERSISTENT_REG));
            node->resultRegisterRc = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->parent->resultRegisterRc);
            freeRegisterRC(context, node->parent);
        }
        else if (node->hasSemFlag(SEMFLAG_FROM_REF))
        {
            node->resultRegisterRc = reserveRegisterRC(context);
            if (persistentReg)
            {
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc, resolved->symRegisters[0]);
            }
            else
            {
                const auto inst = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRc);
                inst->b.u64     = resolved->computedValue.storageOffset;
            }

            if (!node->isForceTakeAddress())
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRc, node->typeInfo));
        }
        else if (typeInfo->isArray() ||
                 typeInfo->isListTuple() ||
                 typeInfo->isListArray() ||
                 typeInfo->isStruct())
        {
            SWAG_ASSERT(!resolved->hasFlag(OVERLOAD_PERSISTENT_REG));
            node->resultRegisterRc = reserveRegisterRC(context);
            const auto inst        = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
            inst->b.u64            = resolved->computedValue.storageOffset;
            inst->c.pointer        = reinterpret_cast<uint8_t*>(resolved);
        }
        else if (node->isForceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            if (persistentReg)
            {
                node->resultRegisterRc            = resolved->symRegisters[0];
                node->resultRegisterRc.cannotFree = true;
            }
            else
            {
                node->resultRegisterRc = reserveRegisterRC(context);
                const auto inst        = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
                inst->b.u64            = resolved->computedValue.storageOffset;
                inst->c.pointer        = reinterpret_cast<uint8_t*>(resolved);
                if (node->parent->hasAstFlag(AST_ARRAY_POINTER_REF))
                    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            }
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            node->resultRegisterRc = reserveRegisterRC(context);

            if (persistentReg)
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc, resolved->symRegisters[0]);
            else
                EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRc)->b.u64 = resolved->computedValue.storageOffset;

            if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
            else if (node->hasAstFlag(AST_TO_UFCS)) // Get the structure pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
        }
        else if (typeInfo->isInterface() && node->hasAstFlag(AST_FROM_UFCS | AST_TO_UFCS) && !node->hasAstFlag(AST_UFCS_FCT))
        {
            if (persistentReg)
            {
                node->resultRegisterRc = reserveRegisterRC(context);
                if (node->hasAstFlag(AST_FROM_UFCS))
                    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc, resolved->symRegisters[1]);
                else if (node->hasAstFlag(AST_TO_UFCS))
                    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc, resolved->symRegisters[0]);
            }
            else
            {
                node->resultRegisterRc = reserveRegisterRC(context);
                const auto inst        = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
                inst->b.u64            = resolved->computedValue.storageOffset;
                inst->c.pointer        = reinterpret_cast<uint8_t*>(resolved);
                if (node->hasAstFlag(AST_FROM_UFCS)) // Get the ITable pointer
                    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc)->c.u64 = sizeof(void*);
                else if (node->hasAstFlag(AST_TO_UFCS)) // Get the structure pointer
                    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);
            }
        }
        else if (typeInfo->isClosure())
        {
            SWAG_ASSERT(!resolved->hasFlag(OVERLOAD_PERSISTENT_REG));
            node->resultRegisterRc = reserveRegisterRC(context);
            const auto inst        = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
            inst->b.u64            = resolved->computedValue.storageOffset;
            inst->c.pointer        = reinterpret_cast<uint8_t*>(resolved);
        }
        else if (persistentReg)
        {
            node->resultRegisterRc            = resolved->symRegisters;
            node->resultRegisterRc.cannotFree = true;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRc);
            const auto inst0 = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRc[0]);
            inst0->b.u64     = resolved->computedValue.storageOffset;
            const auto inst1 = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRc[1]);
            inst1->b.u64     = resolved->computedValue.storageOffset + 8;
        }
        else
        {
            node->resultRegisterRc = reserveRegisterRC(context, resolved);
            resolved->setRegisters(node->resultRegisterRc, OVERLOAD_HINT_REG);

            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            ByteCodeInstruction* inst = nullptr;
            switch (typeInfo->sizeOf)
            {
            case 1:
                inst = EMIT_INST1(context, ByteCodeOp::GetFromStack8, node->resultRegisterRc);
                break;
            case 2:
                inst = EMIT_INST1(context, ByteCodeOp::GetFromStack16, node->resultRegisterRc);
                break;
            case 4:
                inst = EMIT_INST1(context, ByteCodeOp::GetFromStack32, node->resultRegisterRc);
                break;
            case 8:
                inst = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRc);
                break;
            default:
                break;
            }

            inst->b.u64 = resolved->computedValue.storageOffset;
            SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRc, node->typeInfo));
        }

        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;
        return true;
    }

    // Reference to an inline parameter : the registers are directly stored in the overload symbol
    if (resolved->hasFlag(OVERLOAD_VAR_INLINE))
    {
        SWAG_CHECK(sameStackFrame(context, resolved));

        // We need to copy register, and not use it directly, because the register can be changed by
        // some code after (like when dereferencing something)
        SWAG_VERIFY(resolved->symRegisters.size() > 0,
                    Report::internalError(context->node, FMT("emitIdentifier, identifier not generated [[%s]]", identifier->token.c_str()).c_str()));
        SWAG_ASSERT(resolved->hasFlag(OVERLOAD_INLINE_REG));
        reserveRegisterRC(context, node->resultRegisterRc, resolved->symRegisters.size());

        for (uint32_t i = 0; i < node->resultRegisterRc.size(); i++)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[i], resolved->symRegisters[i]);

        // :UfcsItfInlined
        // if we have something of the form vitf.call() where call is inlined.
        // Need to take the vtable register.
        if (node->hasAstFlag(AST_FROM_UFCS) && node->typeInfo->isInterface())
        {
            SWAG_ASSERT(node->resultRegisterRc.size() == 2);
            swap(node->resultRegisterRc.oneResult[0], node->resultRegisterRc.oneResult[1]);
        }

        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        node->parent->resultRegisterRc                = node->resultRegisterRc;
        return true;
    }

    return Report::internalError(context->node, "emitIdentifier");
}
