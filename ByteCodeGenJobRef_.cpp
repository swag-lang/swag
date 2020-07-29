#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC, node->array->resultRegisterRC);
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC         = node->array->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitStringRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);

    if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
        emitInstruction(context, ByteCodeOp::BoundCheckString, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);
    emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitArrayRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    // Boundcheck
    if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        auto r0       = reserveRegisterRC(context);

        emitInstruction(context, ByteCodeOp::CopyVBtoRA32, r0)->b.u32 = typeInfo->count;
        emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);

        freeRegisterRC(context, r0);
    }

    // Pointer increment
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitSliceRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    node->array->resultRegisterRC += reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);

    // Boundcheck
    if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
        emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    // Pointer increment
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access);
    return true;
}

bool ByteCodeGenJob::emitStructDeRef(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto typeInfo = node->typeInfo;

    if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->isPointerTo(TypeInfoKind::Interface))
    {
        if (typeInfo->isPointerTo(TypeInfoKind::Interface))
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        if (node->flags & AST_FROM_UFCS)
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u32 = sizeof(void*);
        else if (node->flags & AST_TO_UFCS)
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Array)
    {
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRC, typeInfo));
    return true;
}

bool ByteCodeGenJob::emitTypeDeRef(ByteCodeGenContext* context, RegisterList& r0, TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Reference)
    {
        emitInstruction(context, ByteCodeOp::DeRef64, r0);
        return true;
    }

    typeInfo = TypeManager::concreteReference(typeInfo);
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);

    if (typeInfo->numRegisters() == 2)
    {
        r0 += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, r0[0], r0[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeList ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Array)
    {
        return true;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::DeRef8, r0);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::DeRef16, r0);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::DeRef32, r0);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::DeRef64, r0);
        break;
    default:
        return internalError(context, "emitTypeDeRef, size not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto node      = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    auto typeArray = TypeManager::concreteType(node->array->typeInfo);

    // Dereference of a string constant
    if (typeArray->isNative(NativeTypeKind::String))
    {
        if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
            emitInstruction(context, ByteCodeOp::BoundCheckString, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);
        emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of a slice
    else if (typeArray->kind == TypeInfoKind::Slice)
    {
        auto typeInfo = CastTypeInfo<TypeInfoSlice>(typeArray, TypeInfoKind::Slice);
        int  sizeOf   = typeInfo->pointedType->sizeOf;

        if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->pointedType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->pointedType));
        else if (!(node->flags & AST_TAKE_ADDRESS) || typeInfo->pointedType->kind == TypeInfoKind::Pointer)
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->pointedType));

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of a pointer
    else if (typeArray->kind == TypeInfoKind::Pointer)
    {
        auto typeInfo = CastTypeInfo<TypeInfoPointer>(typeArray, TypeInfoKind::Pointer);
        int  sizeOf   = typeInfo->pointedType->sizeOf;

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->finalType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->finalType));
        else if (!(node->flags & AST_TAKE_ADDRESS))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->pointedType));

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference of an array
    else if (typeArray->kind == TypeInfoKind::Array)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(typeArray, TypeInfoKind::Array);
        int  sizeOf   = typeInfo->pointedType->sizeOf;

        if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
        {
            auto r0                                                     = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, r0)->b.u32 = typeInfo->count;
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);
            freeRegisterRC(context, r0);
        }

        truncRegisterRC(context, node->array->resultRegisterRC, 1);

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->pointedType->isNative(NativeTypeKind::String))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->pointedType));
        else if ((!(node->flags & AST_TAKE_ADDRESS) && typeInfo->pointedType->kind != TypeInfoKind::Array) || (typeInfo->pointedType->kind == TypeInfoKind::Pointer))
            SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, typeInfo->pointedType));

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access);
    }

    // Dereference a variadic parameter
    else if (typeArray->kind == TypeInfoKind::Variadic)
    {
        RegisterList r0;
        reserveRegisterRC(context, r0, 2);

        if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::DeRef64, r0);
            emitInstruction(context, ByteCodeOp::ClearMaskU64, r0)->b.u64 = 0x00000000'FFFFFFFF;
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);
        }

        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef64, r0);
        // Get total number of pushed arguments
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU64VB, r0)->b.u32 = 32;
        // Offset from variadic on top of stack to the list of 'any' (number of total pushed arguments * register)
        emitInstruction(context, ByteCodeOp::Mul64byVB32, r0)->b.u32 = sizeof(Register);
        // r0[1] now points to the list of any
        emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, r0[0], r0[1]);

        // Now we point to the first 'any' of the argument list
        emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = 2 * sizeof(Register);
        emitInstruction(context, ByteCodeOp::IncPointer32, r0[1], node->access->resultRegisterRC, r0[0]);

        // Deref the any
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, r0[0], r0[1]);

        node->resultRegisterRC = r0;
        freeRegisterRC(context, node->array);
        freeRegisterRC(context, node->access);
    }

    // Dereference a typed variadic parameter
    else if (typeArray->kind == TypeInfoKind::TypedVariadic)
    {
        auto r0 = reserveRegisterRC(context);

        if (context->sourceFile->module->buildParameters.buildCfg->debugBoundCheck || g_CommandLine.debug)
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0, node->array->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::DeRef64, r0);
            emitInstruction(context, ByteCodeOp::ClearMaskU64, r0)->b.u64 = 0x00000000'FFFFFFFF;
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);
        }

        auto rawType = ((TypeInfoVariadic*) typeArray)->rawType;

        // Offset from variadic named parameter to the first parameter on the stack
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef64, r0);
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU64VB, r0)->b.u32 = 32;
        emitInstruction(context, ByteCodeOp::Mul64byVB32, r0)->b.u32         = sizeof(Register);
        emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, r0, node->array->resultRegisterRC);

        // Offset pointer to the parameter
        emitInstruction(context, ByteCodeOp::Mul64byVB32, node->access->resultRegisterRC)->b.u32 = sizeof(Register) * rawType->numRegisters();
        emitInstruction(context, ByteCodeOp::IncPointer32, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        SWAG_CHECK(emitTypeDeRef(context, node->array->resultRegisterRC, rawType));
        node->resultRegisterRC = node->array->resultRegisterRC;

        freeRegisterRC(context, node->access);
        freeRegisterRC(context, r0);
    }

    // Dereference a struct
    else if (typeArray->kind == TypeInfoKind::Struct)
    {
        if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
        {
            AstNode allParams;
            allParams.childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, &allParams));
            if (context->result == ContextResult::Pending)
                return true;
        }
    }
    else
    {
        return internalError(context, "emitPointerDeRef, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitMakeLambda(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto front    = node->childs.front();
    auto funcNode = CastAst<AstFuncDecl>(front->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);

    // Construct a lambda to a foreign function
    if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
    {
        freeRegisterRC(context, front);
        node->resultRegisterRC = reserveRegisterRC(context);

        auto inst       = emitInstruction(context, ByteCodeOp::MakeLambdaForeign, node->resultRegisterRC);
        inst->b.pointer = (uint8_t*) funcNode;

        funcNode->flags |= AST_USED_FOREIGN;
        context->job->module->registerForeign(funcNode);
        return true;
    }

    // Need to generate bytecode, if not already done or running
    askForByteCode(context->job->dependentJob, context->job, funcNode, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
    if (context->result == ContextResult::Pending)
        return true;

    freeRegisterRC(context, front);
    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = emitInstruction(context, ByteCodeOp::MakeLambda, node->resultRegisterRC);
    inst->b.pointer        = (uint8_t*) funcNode->bc;
    return true;
}

bool ByteCodeGenJob::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitInit(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    // Determine if we just need to clear the memory
    bool justClear = true;
    if (node->parameters)
    {
        for (auto child : node->parameters->childs)
        {
            if (!(child->flags & AST_VALUE_COMPUTED))
            {
                justClear = false;
                break;
            }

            if (child->computedValue.reg.u64)
            {
                justClear = false;
                break;
            }
        }
    }

    TypeInfoStruct* typeStruct = nullptr;
    if (typeExpression->pointedType->kind == TypeInfoKind::Struct)
    {
        typeStruct = CastTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
        if (typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            justClear = false;
    }

    // Number of elements to init. If 0, then this is dynamic
    uint32_t numToInit = 0;
    if (!node->count)
        numToInit = 1;
    else if (node->count->flags & AST_VALUE_COMPUTED)
        numToInit = node->count->computedValue.reg.u32;

    if (justClear)
    {
        uint32_t sizeToClear = typeExpression->pointedType->sizeOf;
        if (sizeToClear)
        {
            if (numToInit)
            {
                sizeToClear *= numToInit;
                SWAG_CHECK(emitClearRefConstantSize(context, sizeToClear, node->expression->resultRegisterRC));
            }
            else
            {
                SWAG_ASSERT(node->count);
                emitInstruction(context, ByteCodeOp::SetZeroAtPointerXRB, node->expression->resultRegisterRC, node->count->resultRegisterRC)->c.u32 = sizeToClear;
            }
        }
    }
    else if (!node->parameters || node->parameters->childs.empty())
    {
        SWAG_ASSERT(typeStruct);
        if (!(typeStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            waitStructGenerated(context, typeStruct);
            if (context->result != ContextResult::Done)
                return true;

            SWAG_ASSERT(typeStruct->opInit);
            if (!generateStruct_opInit(context, typeStruct))
                return false;

            auto startLoop = context->bc->numInstructions;
            emitInstruction(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRC);
            SWAG_ASSERT(typeStruct->opInit);
            emitOpCallUser(context, nullptr, typeStruct->opInit, false);

            if (numToInit != 1)
            {
                emitInstruction(context, ByteCodeOp::IncPointerVB32, node->expression->resultRegisterRC)->b.u32 = typeStruct->sizeOf;
                emitInstruction(context, ByteCodeOp::DecrementRA32, node->count->resultRegisterRC);
                auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero32, node->count->resultRegisterRC);
                instJump->b.s32 = startLoop - context->bc->numInstructions;
            }
        }
    }
    else if (!typeStruct)
    {
        auto child     = node->parameters->childs.front();
        auto startLoop = context->bc->numInstructions;
        SWAG_CHECK(emitAffectEqual(context, node->expression->resultRegisterRC, child->resultRegisterRC, child->typeInfo, child));
        if (numToInit != 1)
        {
            emitInstruction(context, ByteCodeOp::IncPointerVB32, node->expression->resultRegisterRC)->b.u32 = typeExpression->pointedType->sizeOf;
            emitInstruction(context, ByteCodeOp::DecrementRA32, node->count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero32, node->count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }
    }
    else
    {
        RegisterList r1;
        reserveRegisterRC(context, r1, 1);

        auto startLoop = context->bc->numInstructions;
        for (auto child : node->parameters->childs)
        {
            auto param     = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r1, node->expression->resultRegisterRC);
            if (typeParam->offset)
                emitInstruction(context, ByteCodeOp::AddVBtoRA32, r1)->b.u32 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
            freeRegisterRC(context, child);
        }

        if (numToInit != 1)
        {
            emitInstruction(context, ByteCodeOp::IncPointerVB32, node->expression->resultRegisterRC)->b.u32 = typeExpression->pointedType->sizeOf;
            emitInstruction(context, ByteCodeOp::DecrementRA32, node->count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero32, node->count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }

        freeRegisterRC(context, r1);
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    freeRegisterRC(context, node->parameters);
    return true;
}

bool ByteCodeGenJob::emitDrop(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstDrop>(context->node, AstNodeKind::Drop);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    if (typeExpression->pointedType->kind == TypeInfoKind::Struct)
    {
        // Number of elements to init. If 0, then this is dynamic
        uint32_t numToInit = 0;
        if (!node->count)
            numToInit = 1;
        else if (node->count->flags & AST_VALUE_COMPUTED)
            numToInit = node->count->computedValue.reg.u32;

        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
        generateStruct_opDrop(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;

        if (typeStruct->opDrop)
        {
            auto startLoop = context->bc->numInstructions;
            emitInstruction(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRC);
            auto inst       = emitInstruction(context, ByteCodeOp::LocalCall);
            inst->a.pointer = (uint8_t*) typeStruct->opDrop;
            inst->b.pointer = (uint8_t*) typeStruct->opDrop->typeInfoFunc;
            emitInstruction(context, ByteCodeOp::IncSP, 8);

            if (numToInit != 1)
            {
                emitInstruction(context, ByteCodeOp::IncPointerVB32, node->expression->resultRegisterRC)->b.u32 = typeExpression->pointedType->sizeOf;
                emitInstruction(context, ByteCodeOp::DecrementRA32, node->count->resultRegisterRC);
                auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero32, node->count->resultRegisterRC);
                instJump->b.s32 = startLoop - context->bc->numInstructions;
            }
        }
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    return true;
}

bool ByteCodeGenJob::emitClearRefConstantSize(ByteCodeGenContext* context, uint32_t sizeOf, uint32_t registerIndex)
{
    switch (sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer8, registerIndex);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer16, registerIndex);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer32, registerIndex);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, registerIndex);
        break;
    default:
        emitInstruction(context, ByteCodeOp::SetZeroAtPointerX, registerIndex)->b.u32 = sizeOf;
        break;
    }

    return true;
}
