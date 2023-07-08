#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Report.h"
#include "Naming.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::sameStackFrame(ByteCodeGenContext* context, SymbolOverload* overload)
{
    if (context->node->isSameStackFrame(overload))
        return true;

    Diagnostic diag{context->node, Fmt(Err(Err0206), Naming::kindName(overload).c_str(), overload->symbol->name.c_str())};

    if (context->node->ownerFct && context->node->ownerFct->attributeFlags & ATTRIBUTE_GENERATED_FUNC)
        diag.hint = Fmt(Hnt(Hnt0095), Naming::kindName(overload).c_str(), context->node->ownerFct->getDisplayName().c_str());

    Diagnostic* note = nullptr;
    if (overload->fromInlineParam)
        note = Diagnostic::note(overload->fromInlineParam, Fmt(Nte(Nte0069), overload->symbol->name.c_str()));

    return context->report(diag, Diagnostic::hereIs(overload, true), note);
}

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!(node->flags & AST_L_VALUE) && !(node->flags & AST_R_VALUE))
        return true;

    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = TypeManager::concreteType(resolved->typeInfo);
    SWAG_VERIFY(!typeInfo->isKindGeneric(), Report::internalError(context->node, "emitIdentifier, type is generic"));

    // If this is a retval, then just copy the return pointer register to a computing register
    if (resolved->flags & OVERLOAD_RETVAL)
    {
        RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, resolved, r0, true, 0);
        identifier->resultRegisterRC                  = r0;
        identifier->identifierRef()->resultRegisterRC = identifier->resultRegisterRC;
        identifier->parent->resultRegisterRC          = node->resultRegisterRC;
        return true;
    }

    // Will be done in the variable declaration
    if (typeInfo->isStruct())
    {
        if (node->flags & AST_IN_TYPE_VAR_DECLARATION)
            return true;
    }

    // A captured variable
    if (resolved->flags & OVERLOAD_VAR_CAPTURE)
    {
        node->resultRegisterRC                        = reserveRegisterRC(context);
        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;

        // Get capture block pointer (first parameter)
        auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
        inst->b.u64 = node->ownerFct->parameters->childs.front()->resolvedSymbolOverload->computedValue.storageOffset;
        inst->c.u64 = 0;

        // :VariadicAndClosure
        // If function is variable, then parameter of the capture context is 2 (after the slice), and not 0.
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
        if (typeFunc->isVariadic())
            inst->c.u64 = 2;

        if (typeInfo->isArray() ||
            typeInfo->isListTuple() ||
            typeInfo->isListArray() ||
            typeInfo->isStruct())
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRC[0])->b.u64 = resolved->computedValue.storageOffset;
            return true;
        }

        if (node->semFlags & SEMFLAG_FROM_REF)
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRC[0])->b.u64 = resolved->computedValue.storageOffset;
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
            return true;
        }

        if (node->forceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            if (resolved->computedValue.storageOffset)
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, node->resultRegisterRC[0])->b.u64 = resolved->computedValue.storageOffset;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
            return true;
        }

        if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->flags & AST_FROM_UFCS)
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = resolved->computedValue.storageOffset + 8;
            // Get the structure pointer
            else
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = resolved->computedValue.storageOffset;

            return true;
        }

        if (typeInfo->numRegisters() == 2)
        {
            transformResultToLinear2(context, node->resultRegisterRC);
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC[1], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset + 8;
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC[0], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset;

            identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
            node->parent->resultRegisterRC                = node->resultRegisterRC;
            return true;
        }

        if (typeInfo->numRegisters() == 1)
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC[0], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset;
            return true;
        }
    }

    // Variable from the tls segment
    bool forStruct = false;
    if (resolved->flags & OVERLOAD_VAR_TLS)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::InternalGetTlsPtr, node->resultRegisterRC);
        forStruct = true;
    }

    // Variable from a struct
    else if (resolved->flags & OVERLOAD_VAR_STRUCT)
    {
        node->resultRegisterRC = identifier->identifierRef()->resultRegisterRC;
        SWAG_VERIFY(node->resultRegisterRC.size() > 0, Report::internalError(context->node, Fmt("emitIdentifier, cannot reference identifier '%s'", identifier->token.ctext()).c_str()));
        forStruct = true;
    }

    if (forStruct)
    {
        SWAG_ASSERT(!(resolved->flags & OVERLOAD_VAR_INLINE));
        if (resolved->computedValue.storageOffset > 0)
        {
            ensureCanBeChangedRC(context, node->resultRegisterRC);

            // :UfcsItfInlined
            // Very specific case where an inlined call returns an interface, and we directly call a lamba of that interface.
            // In that case we want to take the resigter that defined the vtable, not the object.
            if (identifier != identifier->parent->childs.front())
            {
                auto idIdx = identifier->childParentIdx();
                auto prev  = identifier->identifierRef()->childs[idIdx - 1];
                if (prev->childs.size())
                {
                    auto back = prev->childs.back();
                    if (back->kind == AstNodeKind::Inline)
                    {
                        if (back->typeInfo->isInterface())
                        {
                            SWAG_ASSERT(node->resultRegisterRC.countResults == 2);
                            swap(node->resultRegisterRC.oneResult[0], node->resultRegisterRC.oneResult[1]);
                        }
                    }
                }
            }

            auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
            SWAG_ASSERT(resolved->computedValue.storageOffset != UINT32_MAX);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->flags |= BCI_IMM_B;
        }

        auto typeField = node->typeInfo;
        if (node->semFlags & SEMFLAG_FROM_REF)
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
            auto ptrPointer = CastTypeInfo<TypeInfoPointer>(typeField, TypeInfoKind::Pointer);
            SWAG_ASSERT(ptrPointer->flags & TYPEINFO_POINTER_REF);
            typeField = ptrPointer->pointedType;
        }

        if (!node->forceTakeAddress())
            emitStructDeRef(context, typeField);
        else if (node->parent->flags & AST_ARRAY_POINTER_REF)
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);

        // :SilentCall
        if ((resolved->flags & OVERLOAD_VAR_TLS) && node->isSilentCall())
            freeRegisterRC(context, node->parent);

        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;
        return true;
    }

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);

        // Get a parameter from a #validif block... this is special
        if (node->isValidIfParam(resolved))
        {
            ByteCodeInstruction* inst;
            if (typeInfo->numRegisters() == 2)
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRC);
                inst = EMIT_INST2(context, ByteCodeOp::GetParam64SI, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            }
            else
            {
                inst = EMIT_INST2(context, ByteCodeOp::GetParam64SI, node->resultRegisterRC[0], node->resultRegisterRC[0]);
            }

            auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(resolved->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            inst->c.u64     = typeFunc->registerIdxToParamIdx(resolved->storageIndex);
            inst->d.pointer = (uint8_t*) resolved;

            identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
            node->parent->resultRegisterRC                = node->resultRegisterRC;
            return true;
        }

        SWAG_CHECK(sameStackFrame(context, resolved));

        if (node->isSilentCall())
        {
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
            freeRegisterRC(context, node->parent);
        }
        else if (node->semFlags & SEMFLAG_FROM_REF)
        {
            auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            if (!node->forceTakeAddress())
            {
                auto ptrPointer = CastTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
                SWAG_ASSERT(ptrPointer->flags & TYPEINFO_POINTER_REF);
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, ptrPointer->pointedType));
            }
        }
        else if (node->forceTakeAddress() && !typeInfo->isLambdaClosure() && !typeInfo->isArray())
        {
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
            {
                auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
            else if (typeInfo->isSlice())
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRC);

                auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC[0]);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;

                inst        = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC[1]);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex + 1;
            }
            else
            {
                Diagnostic diag{node, node->token, Fmt(Err(Err0462), typeInfo->getDisplayNameC())};
                auto       note = Diagnostic::help(Hlp(Hlp0041));
                return context->report(diag, note);
            }
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = 8;
            else // Get the structure pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->flags & AST_FROM_UFCS)
            {
                auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset + 8;
                inst->c.u64 = resolved->storageIndex + 1;
            }
            // Get the structure pointer
            else
            {
                auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
        }
        else if (typeInfo->isClosure())
        {
            auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC[0]);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            inst        = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC[1]);
            inst->b.u64 = resolved->computedValue.storageOffset + 8;
            inst->c.u64 = resolved->storageIndex + 1;
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            auto inst   = EMIT_INST1(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRC, node->typeInfo));
        }

        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;
        return true;
    }

    // Variable from the data segment
    if (resolved->flags & OVERLOAD_VAR_GLOBAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);

        if (node->isSilentCall())
        {
            auto typeArr   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            auto finalType = TypeManager::concreteType(typeArr->finalType, CONCRETE_ALL);

            // A closure is the pointer to the data, not a pointer to the function.
            // So we must not dereference in that case
            if (finalType->isClosure())
            {
                freeRegisterRC(context, node);
                node->resultRegisterRC = node->parent->resultRegisterRC;
            }
            else
            {
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
                freeRegisterRC(context, node->parent);
            }
        }
        else if (node->semFlags & SEMFLAG_FROM_REF)
        {
            return Report::internalError(context->node, "unsupported identifier reference type");
        }
        else if (typeInfo->isArray() ||
                 typeInfo->isListTuple() ||
                 typeInfo->isListArray() ||
                 typeInfo->isStruct())
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->forceTakeAddress())
                inst->c.pointer = (uint8_t*) resolved;
        }
        else if (node->forceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            inst->c.pointer           = (uint8_t*) resolved;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isClosure())
        {
            emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC[0]);
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset + 8, node->resultRegisterRC[1]);
        }
        else
        {
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRC, node->typeInfo));
        }

        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;
        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));

        if (node->isSilentCall())
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
            freeRegisterRC(context, node->parent);
        }
        else if (node->semFlags & SEMFLAG_FROM_REF)
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            auto inst              = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
            inst->b.u64            = resolved->computedValue.storageOffset;
            if (!node->forceTakeAddress())
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, node->typeInfo));
        }
        else if (typeInfo->isArray() ||
                 typeInfo->isListTuple() ||
                 typeInfo->isListArray() ||
                 typeInfo->isStruct())
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            auto inst              = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64            = resolved->computedValue.storageOffset;
            inst->c.pointer        = (uint8_t*) resolved;
        }
        else if (node->forceTakeAddress() && (!typeInfo->isString() || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            auto inst              = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64            = resolved->computedValue.storageOffset;
            inst->c.pointer        = (uint8_t*) resolved;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            node->resultRegisterRC = reserveRegisterRC(context);

            if (resolved->flags & OVERLOAD_HINT_AS_REG)
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC, resolved->registers[0]);
            else
                EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC)->b.u64 = resolved->computedValue.storageOffset;

            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isInterface() && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            if (resolved->flags & OVERLOAD_HINT_AS_REG)
            {
                node->resultRegisterRC = reserveRegisterRC(context);
                if (node->flags & AST_FROM_UFCS)
                    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC, resolved->registers[1]);
                else if (node->flags & AST_TO_UFCS)
                    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC, resolved->registers[0]);
            }
            else
            {
                node->resultRegisterRC = reserveRegisterRC(context);
                auto inst              = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
                inst->b.u64            = resolved->computedValue.storageOffset;
                inst->c.pointer        = (uint8_t*) resolved;
                if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
                else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                    EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
            }
        }
        else if (typeInfo->isClosure())
        {
            node->resultRegisterRC = reserveRegisterRC(context);
            auto inst              = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64            = resolved->computedValue.storageOffset;
            inst->c.pointer        = (uint8_t*) resolved;
        }
        else
        {
            if (resolved->flags & OVERLOAD_HINT_AS_REG)
            {
                node->resultRegisterRC            = resolved->registers;
                node->resultRegisterRC.cannotFree = true;
            }
            else
            {
                if (typeInfo->numRegisters() == 2)
                {
                    reserveLinearRegisterRC2(context, node->resultRegisterRC);
                    auto inst0   = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[0]);
                    inst0->b.u64 = resolved->computedValue.storageOffset;
                    auto inst1   = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[1]);
                    inst1->b.u64 = resolved->computedValue.storageOffset + 8;
                }
                else
                {
                    node->resultRegisterRC = reserveRegisterRC(context);
                    SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
                    ByteCodeInstruction* inst = nullptr;
                    switch (typeInfo->sizeOf)
                    {
                    case 1:
                        inst = EMIT_INST1(context, ByteCodeOp::GetFromStack8, node->resultRegisterRC);
                        break;
                    case 2:
                        inst = EMIT_INST1(context, ByteCodeOp::GetFromStack16, node->resultRegisterRC);
                        break;
                    case 4:
                        inst = EMIT_INST1(context, ByteCodeOp::GetFromStack32, node->resultRegisterRC);
                        break;
                    case 8:
                        inst = EMIT_INST1(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
                        break;
                    }

                    inst->b.u64 = resolved->computedValue.storageOffset;
                    SWAG_CHECK(emitSafetyValue(context, node->resultRegisterRC, node->typeInfo));
                }
            }
        }

        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;
        return true;
    }

    // Reference to an inline parameter : the registers are directly stored in the overload symbol
    if (resolved->flags & OVERLOAD_VAR_INLINE)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));

        // We need to copy register, and not use it directly, because the register can be changed by
        // some code after (like when dereferencing something)
        SWAG_VERIFY(resolved->registers.size() > 0, Report::internalError(context->node, Fmt("emitIdentifier, identifier not generated '%s'", identifier->token.ctext()).c_str()));
        reserveRegisterRC(context, node->resultRegisterRC, resolved->registers.size());

        for (int i = 0; i < node->resultRegisterRC.size(); i++)
            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[i], resolved->registers[i]);

        // :UfcsItfInlined
        // if we have something of the form vitf.call() where call is inlined.
        // Need to take the vtable register.
        if (node->flags & AST_FROM_UFCS && node->typeInfo->isInterface())
        {
            SWAG_ASSERT(node->resultRegisterRC.size() == 2);
            swap(node->resultRegisterRC.oneResult[0], node->resultRegisterRC.oneResult[1]);
        }

        identifier->identifierRef()->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC                = node->resultRegisterRC;
        return true;
    }

    return Report::internalError(context->node, "emitIdentifier");
}
