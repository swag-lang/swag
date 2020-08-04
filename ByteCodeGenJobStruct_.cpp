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
        waitStructGenerated(context, typeStructVar);
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
    opInit->name         = structNode->ownerScope->fullname + "_" + structNode->name.c_str() + "_opInit";
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
        SWAG_CHECK(emitClearRefConstantSize(&cxt, typeInfoStruct->sizeOf, 0));
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
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB32, 0)->b.u32 = param->offset;

        if (varDecl->assignment)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto exprList = CastAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                auto typeList = CastTypeInfo<TypeInfoList>(varDecl->assignment->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

                auto inst = emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointerOC, 1, 2);
                SWAG_ASSERT(exprList->storageOffsetSegment != UINT32_MAX);
                inst->c.u64 = ((uint64_t) exprList->storageOffsetSegment << 32) | (uint32_t) typeList->subTypes.size();

                emitInstruction(&cxt, ByteCodeOp::MemCpyVC32, 0, 1)->c.u32 = typeVar->sizeOf;
            }
            else if (typeVar->isNative(NativeTypeKind::String))
            {
                auto offset = sourceFile->module->constantSegment.addString(varDecl->assignment->computedValue.text);
                SWAG_ASSERT(offset != UINT32_MAX);
                emitInstruction(&cxt, ByteCodeOp::MakeConstantSegPointer, 1, offset);
                emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA32, 2, (uint32_t) varDecl->assignment->computedValue.text.length());
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1, 0);
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 2, 8);
            }
            else if (typeVar->kind == TypeInfoKind::Native)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA64, 1)->b.u64 = varDecl->assignment->computedValue.reg.u64;
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

                // Need to loop on every element of the array in order to initialize them
                RegisterList r0 = reserveRegisterRC(&cxt);

                emitInstruction(&cxt, ByteCodeOp::CopyVBtoRA32, r0)->b.u32 = typeArray->totalCount;
                auto seekJump                                              = cxt.bc->numInstructions;

                emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                emitOpCallUser(&cxt, nullptr, typeInVarStruct->opInit, false);

                emitInstruction(&cxt, ByteCodeOp::DecrementRA32, r0);
                emitInstruction(&cxt, ByteCodeOp::AddVBtoRA32, 0)->b.u32      = typeInVarStruct->sizeOf;
                emitInstruction(&cxt, ByteCodeOp::JumpIfNotZero32, r0)->b.s32 = seekJump - cxt.bc->numInstructions - 1;

                freeRegisterRC(&cxt, r0);
            }
            else
            {
                SWAG_CHECK(emitClearRefConstantSize(&cxt, typeVar->sizeOf, 0));
            }
        }
        else if (typeVar->kind == TypeInfoKind::Struct && (typeVar->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
            SWAG_ASSERT(typeVarStruct->opInit);
            emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
            emitOpCallUser(&cxt, nullptr, typeVarStruct->opInit, false);
        }
        else
        {
            SWAG_CHECK(emitClearRefConstantSize(&cxt, typeVar->sizeOf, 0));
        }
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    //opInit->print();
    return true;
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
            context->job->setPending(symbol);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserDropFct)
    {
        needDrop = true;
        if (!(typeInfoStruct->opUserDropFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job->dependentJob, context->job, (AstFuncDecl*) typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
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
            waitStructGenerated(context, typeStructVar);
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
    opDrop->name           = structNode->ownerScope->fullname + "_" + structNode->name.c_str() + "_opDropGenerated";
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

void ByteCodeGenJob::emitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc, bool pushParam, uint32_t offset)
{
    if (!funcDecl && !bc)
        return;

    if (pushParam)
    {
        emitInstruction(context, ByteCodeOp::GetFromStackParam64, 0, 24);
        if (offset)
            emitInstruction(context, ByteCodeOp::IncPointerVB32, 0)->b.u32 = offset;
        emitInstruction(context, ByteCodeOp::PushRAParam, 0);
    }

    if (funcDecl && funcDecl->attributeFlags & ATTRIBUTE_FOREIGN)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::ForeignCall);
        inst->a.pointer = (uint8_t*) funcDecl;
        inst->b.pointer = (uint8_t*) funcDecl->typeInfo;
        funcDecl->flags |= AST_USED_FOREIGN;
        SWAG_ASSERT(inst->a.pointer);
    }
    else
    {

        auto inst       = emitInstruction(context, ByteCodeOp::LocalCall);
        inst->a.pointer = (uint8_t*) (funcDecl ? funcDecl->bc : bc);
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        SWAG_ASSERT(inst->a.pointer);
    }

    emitInstruction(context, ByteCodeOp::IncSP, 8);
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
            context->job->setPending(symbol);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostMoveFct)
    {
        needPostMove = true;
        if (!(typeInfoStruct->opUserPostMoveFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job->dependentJob, context->job, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
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
            waitStructGenerated(context, typeStructVar);
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
    opPostMove->name           = structNode->ownerScope->fullname + "_" + structNode->name.c_str() + "_opPostMoveGenerated";
    opPostMove->name.replaceAll('.', '_');
    opPostMove->maxReservedRegisterRC = 3;
    opPostMove->compilerGenerated     = true;
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
            context->job->setPending(symbol);
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostCopyFct)
    {
        needPostCopy = true;
        if (!(typeInfoStruct->opUserPostCopyFct->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            askForByteCode(context->job->dependentJob, context->job, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
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
            waitStructGenerated(context, typeStructVar);
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
    opPostCopy->name           = structNode->ownerScope->fullname + "_" + structNode->name.c_str() + "_opPostCopyGenerated";
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

void ByteCodeGenJob::waitStructGenerated(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    if (typeInfoStruct->declNode->kind == AstNodeKind::InterfaceDecl)
        return;

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    if (!(structNode->flags & AST_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(context->job);
        context->job->setPending(structNode->resolvedSymbolName);
    }
}

bool ByteCodeGenJob::emitStruct(ByteCodeGenContext* context)
{
    AstStruct*      node           = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_IS_TUPLE))
    {
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
    }

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
    emitInstruction(context, ByteCodeOp::MemCpyVC32, r0, r1)->c.u32 = typeInfoStruct->sizeOf;

    // A copy
    bool mustCopy = (from->flags & (AST_TRANSIENT | AST_FORCE_MOVE)) ? false : true;
    if (mustCopy)
    {
        if (typeInfoStruct->opPostCopy)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, nullptr, typeInfoStruct->opPostCopy, false);
        }
    }

    // A move
    else
    {
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
                emitInstruction(context, ByteCodeOp::SetZeroAtPointerX, r1)->b.u32 = typeInfoStruct->sizeOf;
            }
        }
    }

    return true;
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, uint32_t regOffset)
{
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;

    // All fields are explicitly not initialized, so we are done
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
        return true;

    // Just clear the content of the structure
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::SetZeroStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfoStruct->sizeOf;
    }
    else
    {
        // Push self
        RegisterList r0   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::MakePointerToStack, r0);
        inst->b.s32       = resolved->storageOffset;

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

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset)
{
    PushContextFlags cf(context, BCC_FLAG_NOSAFETY);
    auto             node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::LetDecl);
    auto             resolved = node->resolvedSymbolOverload;

    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        RegisterList r0   = reserveRegisterRC(context);
        RegisterList r1   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::MakePointerToStack, r0);
        inst->b.s32       = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer32, r0, regOffset, r0);

        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        if (identifier->callParameters)
        {
            for (auto child : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                SWAG_ASSERT(param->resolvedParameter);
                auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
                emitInstruction(context, ByteCodeOp::CopyRBtoRA, r1, r0);
                if (typeParam->offset)
                    emitInstruction(context, ByteCodeOp::AddVBtoRA32, r1)->b.u32 = typeParam->offset;
                emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
            }
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }
}

void ByteCodeGenJob::freeStructParametersRegisters(ByteCodeGenContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::LetDecl);
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