#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Diagnostic.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::sameStackFrame(ByteCodeGenContext* context, SymbolOverload* overload)
{
    if (!context->node->isSameStackFrame(overload))
        return context->report(context->node, Fmt(Err(Err0206), overload->symbol->name.c_str()));
    return true;
}

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!(node->flags & AST_L_VALUE) && !(node->flags & AST_R_VALUE))
        return true;

    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = TypeManager::concreteType(resolved->typeInfo);
    SWAG_VERIFY(typeInfo->kind != TypeInfoKind::Generic, Report::internalError(context->node, "emitIdentifier, type is generic"));

    // If this is a retval, then just copy the return pointer register to a computing register
    if (resolved->flags & OVERLOAD_RETVAL)
    {
        RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, r0, true, 0);
        identifier->resultRegisterRC                = r0;
        identifier->identifierRef->resultRegisterRC = identifier->resultRegisterRC;
        identifier->parent->resultRegisterRC        = node->resultRegisterRC;
        return true;
    }

    // Will be done in the variable declaration
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->flags & AST_IN_TYPE_VAR_DECLARATION)
            return true;
    }

    // A captured variable
    if (resolved->flags & OVERLOAD_VAR_CAPTURE)
    {
        node->resultRegisterRC                      = reserveRegisterRC(context);
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;

        // Get capture block pointer (first parameter)
        auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
        inst->b.u64 = node->ownerFct->parameters->childs.front()->resolvedSymbolOverload->computedValue.storageOffset;
        inst->c.u64 = 0;

        if (typeInfo->kind == TypeInfoKind::Array ||
            typeInfo->kind == TypeInfoKind::TypeListTuple ||
            typeInfo->kind == TypeInfoKind::TypeListArray ||
            typeInfo->kind == TypeInfoKind::Struct)
        {
            emitInstruction(context, ByteCodeOp::Add64byVB64, node->resultRegisterRC[0])->b.u64 = resolved->computedValue.storageOffset;
            return true;
        }

        if (node->forceTakeAddress() && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            emitInstruction(context, ByteCodeOp::Add64byVB64, node->resultRegisterRC[0])->b.u64 = resolved->computedValue.storageOffset;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->flags & AST_FROM_UFCS)
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = resolved->computedValue.storageOffset + 8;
            // Get the structure pointer
            else
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = resolved->computedValue.storageOffset;

            return true;
        }

        if (typeInfo->numRegisters() == 2)
        {
            transformResultToLinear2(context, node->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC[1], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset + 8;
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC[0], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset;

            identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
            node->parent->resultRegisterRC              = node->resultRegisterRC;
            return true;
        }

        if (typeInfo->numRegisters() == 1)
        {
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC[0], node->resultRegisterRC[0])->c.u64 = resolved->computedValue.storageOffset;
            return true;
        }
    }

    // Variable from the tls segment
    if (resolved->flags & OVERLOAD_VAR_TLS)
    {
        if (node->semFlags & AST_SEM_FROM_REF)
            return Report::internalError(context->node, "unsupported identifier reference type");

        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::InternalGetTlsPtr, node->resultRegisterRC);

        SWAG_ASSERT(!(resolved->flags & OVERLOAD_VAR_INLINE));
        if (node->resolvedSymbolOverload->computedValue.storageOffset > 0)
        {
            ensureCanBeChangedRC(context, node->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
            SWAG_ASSERT(node->resolvedSymbolOverload->computedValue.storageOffset != UINT32_MAX);
            inst->b.u64 = node->resolvedSymbolOverload->computedValue.storageOffset;
            inst->flags |= BCI_IMM_B;
        }

        if (!node->forceTakeAddress())
            emitStructDeRef(context);
        else if (node->parent->flags & AST_ARRAY_POINTER_REF)
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);

        // :SilentCall
        if (node->token.text.empty())
            freeRegisterRC(context, node->parent);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Reference inside a struct
    if (resolved->flags & OVERLOAD_VAR_STRUCT)
    {
        if (node->semFlags & AST_SEM_FROM_REF)
            return Report::internalError(context->node, "unsupported identifier reference type");

        SWAG_ASSERT(!(resolved->flags & OVERLOAD_VAR_INLINE));
        node->resultRegisterRC = identifier->identifierRef->resultRegisterRC;
        SWAG_VERIFY(node->resultRegisterRC.size() > 0, Report::internalError(context->node, Fmt("emitIdentifier, cannot reference identifier `%s`", identifier->token.ctext()).c_str()));

        if (node->resolvedSymbolOverload->computedValue.storageOffset > 0)
        {
            ensureCanBeChangedRC(context, node->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
            SWAG_ASSERT(node->resolvedSymbolOverload->computedValue.storageOffset != UINT32_MAX);
            inst->b.u64 = node->resolvedSymbolOverload->computedValue.storageOffset;
            inst->flags |= BCI_IMM_B;
        }

        if (!node->forceTakeAddress())
            emitStructDeRef(context);
        else if (node->parent->flags & AST_ARRAY_POINTER_REF)
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);

        // Get a parameter from a #selectif block... this is special
        if (node->isSelectIfParam(resolved))
        {
            ByteCodeInstruction* inst;
            if (typeInfo->numRegisters() == 2)
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRC);
                inst = emitInstruction(context, ByteCodeOp::GetParam64SI, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            }
            else
            {
                inst = emitInstruction(context, ByteCodeOp::GetParam64SI, node->resultRegisterRC[0], node->resultRegisterRC[0]);
            }

            auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(resolved->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            inst->c.u64     = typeFunc->registerIdxToParamIdx(resolved->storageIndex);
            inst->d.pointer = (uint8_t*) resolved;

            identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
            node->parent->resultRegisterRC              = node->resultRegisterRC;
            return true;
        }

        SWAG_CHECK(sameStackFrame(context, resolved));

        // :SilentCall
        if (node->token.text.empty())
        {
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
            freeRegisterRC(context, node->parent);
        }
        else if (node->semFlags & AST_SEM_FROM_REF)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            if (!node->forceTakeAddress())
            {
                auto ptrPointer = CastTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
                SWAG_ASSERT(ptrPointer->flags & TYPEINFO_POINTER_REF);
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, ptrPointer->pointedType));
            }
        }
        else if (node->forceTakeAddress() && typeInfo->kind != TypeInfoKind::Lambda && typeInfo->kind != TypeInfoKind::Array)
        {
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
            else if (typeInfo->kind == TypeInfoKind::Slice)
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRC);

                auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC[0]);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;

                inst        = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC[1]);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex + 1;
            }
            else
            {
                return context->report(node, Fmt(Err(Err0462), typeInfo->getDisplayNameC()));
            }
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = 8;
            else // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            // Get the ITable pointer
            if (node->flags & AST_FROM_UFCS)
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset + 8;
                inst->c.u64 = resolved->storageIndex + 1;
            }
            // Get the structure pointer
            else
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->computedValue.storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
        }
        else if (typeInfo->isClosure())
        {
            auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC[0]);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
            inst        = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC[1]);
            inst->b.u64 = resolved->computedValue.storageOffset + 8;
            inst->c.u64 = resolved->storageIndex + 1;
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            auto inst   = emitInstruction(context, ByteCodeOp::GetParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            inst->c.u64 = resolved->storageIndex;
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Variable from the data segment
    if (resolved->flags & OVERLOAD_VAR_GLOBAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);

        // :SilentCall
        if (node->token.text.empty())
        {
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
            freeRegisterRC(context, node->parent);
        }
        else if (node->semFlags & AST_SEM_FROM_REF)
        {
            return Report::internalError(context->node, "unsupported identifier reference type");
        }
        else if (typeInfo->kind == TypeInfoKind::Array ||
                 typeInfo->kind == TypeInfoKind::TypeListTuple ||
                 typeInfo->kind == TypeInfoKind::TypeListArray ||
                 typeInfo->kind == TypeInfoKind::Struct)
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->forceTakeAddress())
                inst->c.pointer = (uint8_t*) resolved;
        }
        else if (node->forceTakeAddress() && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            ByteCodeInstruction* inst = emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            inst->c.pointer           = (uint8_t*) resolved;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitGetFromSeg(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitMakeSegPointer(context, resolved->computedValue.storageSegment, resolved->computedValue.storageOffset, node->resultRegisterRC);
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
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
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));
        node->resultRegisterRC = reserveRegisterRC(context);

        // :SilentCall
        if (node->token.text.empty())
        {
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->parent->resultRegisterRC);
            freeRegisterRC(context, node->parent);
        }
        else if (node->semFlags & AST_SEM_FROM_REF)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            if (!node->forceTakeAddress())
                SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, node->typeInfo));
        }
        else if (typeInfo->kind == TypeInfoKind::Array ||
                 typeInfo->kind == TypeInfoKind::TypeListTuple ||
                 typeInfo->kind == TypeInfoKind::TypeListArray ||
                 typeInfo->kind == TypeInfoKind::Struct)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
        }
        else if (node->forceTakeAddress() && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC)->b.u64 = resolved->computedValue.storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)) && !(node->flags & AST_UFCS_FCT))
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u64 = resolved->computedValue.storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isClosure())
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->computedValue.storageOffset;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            auto inst0   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[0]);
            inst0->b.u64 = resolved->computedValue.storageOffset;
            auto inst1   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[1]);
            inst1->b.u64 = resolved->computedValue.storageOffset + 8;
        }
        else
        {
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            ByteCodeInstruction* inst = nullptr;
            switch (typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
                break;
            }

            inst->b.u64 = resolved->computedValue.storageOffset;
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Reference to an inline parameter : the registers are directly stored in the overload symbol
    if (resolved->flags & OVERLOAD_VAR_INLINE)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));

        // We need to copy register, and not use it directly, because the register can be changed by
        // some code after (like when dereferencing something)
        SWAG_VERIFY(resolved->registers.size() > 0, Report::internalError(context->node, Fmt("emitIdentifier, identifier not generated `%s`", identifier->token.ctext()).c_str()));

        reserveRegisterRC(context, node->resultRegisterRC, resolved->registers.size());
        for (int i = 0; i < node->resultRegisterRC.size(); i++)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[i], resolved->registers[i]);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    return Report::internalError(context->node, "emitIdentifier");
}
