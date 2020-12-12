#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"

bool ByteCodeGenJob::generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->opInit)
        return true;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind == TypeInfoKind::Array)
            typeVar = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array)->pointedType;
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        context->job->waitStructGenerated(typeStructVar);
        if (context->result == ContextResult::Pending)
            return true;
        generateStruct_opInit(context, typeStructVar);
        if (context->result == ContextResult::Pending)
            return true;
    }

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    ByteCode* opInit     = g_Allocator.alloc<ByteCode>();
    opInit->sourceFile   = context->sourceFile;
    opInit->typeInfoFunc = g_TypeMgr.typeInfoOpCall;
    opInit->name         = structNode->ownerScope->getFullName() + "_" + structNode->token.text.c_str() + "_opInit";
    opInit->name.replaceAll('.', '_');
    opInit->maxReservedRegisterRC = 3;
    opInit->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opInit);
    typeInfoStruct->opInit = opInit;

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;

    // All fields are explicitly not initialized, so we are done, function is empty
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
    {
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    // No special value, so we can just clear the struct
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        emitInstruction(&cxt, ByteCodeOp::GetFromStackParam64, 0, 24);
        emitSetZeroAtPointer(&cxt, typeInfoStruct->sizeOf, 0);
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    for (auto param : typeInfoStruct->fields)
    {
        auto varDecl = CastAst<AstVarDecl>(param->node, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(param->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::GetFromStackParam64, 0, 24);
        if (param->offset)
        {
            auto inst = emitInstruction(&cxt, ByteCodeOp::IncPointer32, 0, 0, 0);
            inst->flags |= BCI_IMM_B;
            inst->b.u32 = param->offset;
        }

        // A structure initialized with a literal
        if (varDecl->type && varDecl->type->flags & AST_HAS_STRUCT_PARAMETERS)
        {
            emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointer, 1)->b.u32 = varDecl->type->computedValue.reg.offset;

            auto inst = emitInstruction(&cxt, ByteCodeOp::IntrinsicMemCpy, 0, 1);
            inst->flags |= BCI_IMM_C;
            inst->c.u32 = typeVar->sizeOf;
            continue;
        }

        // User initialization
        if (varDecl->assignment)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto exprList = CastAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                auto typeList = CastTypeInfo<TypeInfoList>(varDecl->assignment->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

                emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointer, 1)->b.u32 = exprList->computedValue.reg.offset;
                emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointer, 2)->b.u32 = (uint32_t) typeList->subTypes.size();

                auto inst = emitInstruction(&cxt, ByteCodeOp::IntrinsicMemCpy, 0, 1);
                inst->flags |= BCI_IMM_C;
                inst->c.u32 = typeVar->sizeOf;
            }
            else if (typeVar->isNative(NativeTypeKind::String))
            {
                auto offset = sourceFile->module->constantSegment.addString(varDecl->assignment->computedValue.text);
                SWAG_ASSERT(offset != UINT32_MAX);
                emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointer, 1, offset);
                emitInstruction(&cxt, ByteCodeOp::SetImmediate64, 2)->b.u64 = varDecl->assignment->computedValue.text.length();
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                emitInstruction(&cxt, ByteCodeOp::IncPointer32, 0, 8, 0)->flags |= BCI_IMM_B;
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 2);
            }
            else if (typeVar->kind == TypeInfoKind::Native)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate64, 1)->b.u64 = varDecl->assignment->computedValue.reg.u64;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                    break;
                default:
                    return internalError(context, "generateStructInit, invalid native type sizeof", varDecl);
                }
            }
            else
            {
                return internalError(context, "generateStructInit, invalid assignment type", varDecl);
            }

            continue;
        }

        // Default initialization
        if (typeVar->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
            auto typeInVar = typeArray->pointedType;
            if (typeInVar->kind == TypeInfoKind::Struct && (typeInVar->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
            {
                auto typeInVarStruct = CastTypeInfo<TypeInfoStruct>(typeInVar, TypeInfoKind::Struct);
                if (typeArray->totalCount == 1)
                {
                    emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                    emitOpCallUser(&cxt, nullptr, typeInVarStruct->opInit, false);
                }
                else
                {
                    // Need to loop on every element of the array in order to initialize them
                    RegisterList r0 = reserveRegisterRC(&cxt);

                    emitInstruction(&cxt, ByteCodeOp::SetImmediate64, r0)->b.u64 = typeArray->totalCount;
                    auto seekJump                                                = cxt.bc->numInstructions;

                    emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                    emitOpCallUser(&cxt, nullptr, typeInVarStruct->opInit, false);

                    emitInstruction(&cxt, ByteCodeOp::DecrementRA32, r0);
                    emitInstruction(&cxt, ByteCodeOp::Add32byVB32, 0)->b.u32      = typeInVarStruct->sizeOf;
                    emitInstruction(&cxt, ByteCodeOp::JumpIfNotZero32, r0)->b.s32 = seekJump - cxt.bc->numInstructions - 1;

                    freeRegisterRC(&cxt, r0);
                }
            }
            else
            {
                emitSetZeroAtPointer(&cxt, typeVar->sizeOf, 0);
            }
        }
        else if (typeVar->kind == TypeInfoKind::Struct && (typeVar->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            auto typeVarStruct = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            SWAG_ASSERT(typeVarStruct->opInit);
            emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
            emitOpCallUser(&cxt, nullptr, typeVarStruct->opInit, false);
        }
        else
        {
            emitSetZeroAtPointer(&cxt, typeVar->sizeOf, 0);
        }
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    //opInit->print();
    return true;
}

void ByteCodeGenJob::emitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc, bool pushParam, uint32_t offset)
{
    if (!funcDecl && !bc)
        return;

    if (pushParam)
    {
        emitInstruction(context, ByteCodeOp::GetFromStackParam64, 0, 24);
        if (offset)
        {
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer32, 0, 0, 0);
            inst->flags |= BCI_IMM_B;
            inst->b.u32 = offset;
        }

        emitInstruction(context, ByteCodeOp::PushRAParam, 0);
    }

    if (funcDecl && !bc && funcDecl->attributeFlags & ATTRIBUTE_FOREIGN)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::ForeignCall);
        inst->a.pointer = (uint8_t*) funcDecl;
        inst->b.pointer = (uint8_t*) funcDecl->typeInfo;
        SWAG_ASSERT(inst->a.pointer);
    }
    else
    {

        auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(bc || (funcDecl && funcDecl->bc));
        inst->a.pointer = (uint8_t*) (bc ? bc : funcDecl->bc);
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        SWAG_ASSERT(inst->a.pointer);
    }

    emitInstruction(context, ByteCodeOp::IncSPPostCall, 8);
}

bool ByteCodeGenJob::generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_DROP)
        return true;
    if (typeInfoStruct->opDrop)
        return true;

    SWAG_ASSERT(typeInfoStruct->declNode);
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Do we need a drop ?
    bool needDrop = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto        symbol = typeInfoStruct->scope->symTable.findNoLock("opDrop");
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, "EMIT_DROP", structNode, nullptr);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserDropFct)
    {
        needDrop = true;
        if (!(typeInfoStruct->opUserDropFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
            if (context->result == ContextResult::Pending)
                return true;
        }
    }

    if (!needDrop)
    {
        for (auto typeParam : typeInfoStruct->fields)
        {
            auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            context->job->waitStructGenerated(typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            generateStruct_opDrop(context, typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            if (typeStructVar->opDrop)
                needDrop = true;
        }
    }

    if (!needDrop)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_DROP;
        return true;
    }

    auto opDrop            = g_Allocator.alloc<ByteCode>();
    opDrop->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opDrop = opDrop;
    opDrop->sourceFile     = sourceFile;
    opDrop->name           = structNode->ownerScope->getFullName() + "_" + structNode->token.text.c_str() + "_opDropGenerated";
    opDrop->name.replaceAll('.', '_');
    opDrop->maxReservedRegisterRC = 3;
    opDrop->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opDrop);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitOpCallUser(&cxt, typeStructVar->opUserDropFct, typeStructVar->opDrop, true, typeParam->offset);
    }

    // Then call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserDropFct);

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    //opDrop->print();
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_POST_MOVE)
        return true;
    if (typeInfoStruct->opPostMove)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Do we need a postmove ?
    bool needPostMove = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto        symbol = typeInfoStruct->scope->symTable.findNoLock("opPostMove");
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, "EMIT_POST_MOVE", structNode, nullptr);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostMoveFct)
    {
        needPostMove = true;
        if (!(typeInfoStruct->opUserPostMoveFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
            if (context->result == ContextResult::Pending)
                return true;
        }
    }

    if (!needPostMove)
    {
        for (auto typeParam : typeInfoStruct->fields)
        {
            auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            context->job->waitStructGenerated(typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            generateStruct_opPostMove(context, typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            if (typeStructVar->opPostMove)
                needPostMove = true;
        }
    }

    if (!needPostMove)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_MOVE;
        return true;
    }

    auto opPostMove            = g_Allocator.alloc<ByteCode>();
    opPostMove->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opPostMove = opPostMove;
    opPostMove->sourceFile     = sourceFile;
    opPostMove->name           = structNode->ownerScope->getFullName() + "_" + structNode->token.text.c_str() + "_opPostMoveGenerated";
    opPostMove->name.replaceAll('.', '_');
    opPostMove->maxReservedRegisterRC = 3;
    opPostMove->compilerGenerated     = true;
    opPostMove->isPostMove            = true;
    sourceFile->module->addByteCodeFunc(opPostMove);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitOpCallUser(&cxt, typeStructVar->opUserPostMoveFct, typeStructVar->opPostMove, true, typeParam->offset);
    }

    // Then call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserPostMoveFct);

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_POST_COPY)
        return true;
    if (typeInfoStruct->opPostCopy)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Do we need a postcopy ?
    bool needPostCopy = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto        symbol = typeInfoStruct->scope->symTable.findNoLock("opPostCopy");
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, "EMIT_POST_COPY", structNode, nullptr);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostCopyFct)
    {
        needPostCopy = true;
        if (!(typeInfoStruct->opUserPostCopyFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
            if (context->result == ContextResult::Pending)
                return true;
        }
    }

    if (!needPostCopy)
    {
        for (auto typeParam : typeInfoStruct->fields)
        {
            auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            context->job->waitStructGenerated(typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            generateStruct_opPostCopy(context, typeStructVar);
            if (context->result == ContextResult::Pending)
                return true;
            if (typeStructVar->opPostCopy)
                needPostCopy = true;
        }
    }

    if (!needPostCopy)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_COPY;
        return true;
    }

    auto opPostCopy            = g_Allocator.alloc<ByteCode>();
    opPostCopy->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opPostCopy = opPostCopy;
    opPostCopy->sourceFile     = sourceFile;
    opPostCopy->name           = structNode->ownerScope->getFullName() + "_" + structNode->token.text.c_str() + "_opPostCopyGenerated";
    opPostCopy->name.replaceAll('.', '_');
    opPostCopy->maxReservedRegisterRC = 3;
    opPostCopy->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opPostCopy);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitOpCallUser(&cxt, typeStructVar->opUserPostCopyFct, typeStructVar->opPostCopy, true, typeParam->offset);
    }

    // Then call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserPostCopyFct);

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    return true;
}

bool ByteCodeGenJob::emitStruct(ByteCodeGenContext* context)
{
    AstStruct*      node           = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    SWAG_CHECK(generateStruct_opInit(context, typeInfoStruct));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(generateStruct_opDrop(context, typeInfoStruct));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(generateStruct_opPostCopy(context, typeInfoStruct));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(generateStruct_opPostMove(context, typeInfoStruct));
    if (context->result == ContextResult::Pending)
        return true;

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    structNode->flags |= AST_BYTECODE_GENERATED;
    node->dependentJobs.setRunning();
    return true;
}

bool ByteCodeGenJob::emitStructCopyMoveCall(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* typeInfo, AstNode* from)
{
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    // Need to drop first
    if (typeInfoStruct->opDrop)
    {
        bool mustDrop = (from->flags & AST_NO_LEFT_DROP) ? false : true;
        if (mustDrop)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, nullptr, typeInfoStruct->opDrop, false);
        }
    }

    // Shallow copy
    auto inst = emitInstruction(context, ByteCodeOp::IntrinsicMemCpy, r0, r1);
    inst->flags |= BCI_IMM_C;
    inst->c.u32 = typeInfoStruct->sizeOf;

    // A copy
    bool mustCopy = (from->flags & (AST_TRANSIENT | AST_FORCE_MOVE)) ? false : true;
    if (mustCopy)
    {
        PushICFlags sf(context, BCI_POST_COPYMOVE);
        if (typeInfoStruct->opPostCopy)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, nullptr, typeInfoStruct->opPostCopy, false);
        }
    }

    // A move
    else
    {
        PushICFlags sf(context, BCI_POST_COPYMOVE);
        if (typeInfoStruct->opPostMove)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, nullptr, typeInfoStruct->opPostMove, false);
        }

        // Reinit source struct, except if AST_NO_RIGHT_DROP, because if we do not drop the
        // right expression, then this is not necessary to reinitialize it
        if (typeInfoStruct->opDrop && !(from->flags & AST_NO_RIGHT_DROP))
        {
            if (typeInfoStruct->opInit && (typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
            {
                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, nullptr, typeInfoStruct->opInit, false);
            }
            else
            {
                emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r1);
            }

            // If the current scope contains a drop for that variable, then we remove it, because we have
            // just reset the content
            for (auto& toDrop : from->ownerScope->symTable.structVarsToDrop)
            {
                if (toDrop.overload == from->resolvedSymbolOverload)
                {
                    toDrop.typeStruct = nullptr;
                    break;
                }
            }
        }
    }

    return true;
}

void ByteCodeGenJob::emitRetValRef(ByteCodeGenContext* context, RegisterList& r0)
{
    auto node = context->node;
    if (node->ownerInline)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0, node->ownerInline->resultRegisterRC);
    else
        emitInstruction(context, ByteCodeOp::CopyRRtoRC, r0, 0);
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, uint32_t regOffset, bool retVal)
{
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;

    // All fields are explicitly not initialized, so we are done
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
        return true;

    // Just clear the content of the structure
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        if (retVal)
        {
            RegisterList r0 = reserveRegisterRC(context);
            emitRetValRef(context, r0);
            emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else
        {
            auto inst = emitInstruction(context, ByteCodeOp::SetZeroStackX);
            SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
            inst->a.u32 = resolved->storageOffset;
            inst->b.u32 = typeInfoStruct->sizeOf;
        }
    }
    else
    {
        // Push self
        RegisterList r0 = reserveRegisterRC(context);

        if (retVal)
        {
            emitRetValRef(context, r0);
        }
        else
        {
            auto inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
            SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
            inst->b.s32 = resolved->storageOffset;
        }

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer32, r0, regOffset, r0);

        // Then call
        SWAG_ASSERT(typeInfoStruct->opInit);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0);
        emitOpCallUser(context, nullptr, typeInfoStruct->opInit, false);
        freeRegisterRC(context, r0);
    }

    return true;
}

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset, bool retVal)
{
    PushContextFlags cf(context, BCC_FLAG_NOSAFETY);
    auto             node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    auto             resolved = node->resolvedSymbolOverload;

    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        RegisterList r0 = reserveRegisterRC(context);

        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        if (identifier->callParameters)
        {
            for (auto child : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                SWAG_ASSERT(param->resolvedParameter);
                auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);

                if (retVal)
                {
                    emitRetValRef(context, r0);
                    if (typeParam->offset)
                    {
                        auto inst = emitInstruction(context, ByteCodeOp::IncPointer32, r0, typeParam->offset, r0);
                        inst->flags |= BCI_IMM_B;
                    }
                }
                else
                {
                    auto inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
                    SWAG_ASSERT(resolved->storageOffset != UINT32_MAX);
                    inst->b.s32 = resolved->storageOffset + typeParam->offset;
                }

                if (regOffset != UINT32_MAX)
                    emitInstruction(context, ByteCodeOp::IncPointer32, r0, regOffset, r0);

                child->flags |= AST_NO_LEFT_DROP;
                emitAffectEqual(context, r0, child->resultRegisterRC, child->typeInfo, child);
                freeRegisterRC(context, child);
            }
        }

        freeRegisterRC(context, r0);
    }
}

void ByteCodeGenJob::freeStructParametersRegisters(ByteCodeGenContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
        if (identifier->callParameters)
        {
            for (auto child : identifier->callParameters->childs)
            {
                freeRegisterRC(context, child);
            }
        }
    }
}