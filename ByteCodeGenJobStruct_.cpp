#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    for (auto typeParam : typeInfoStruct->childs)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
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
    auto structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    ByteCode* opInit     = g_Pool_byteCode.alloc();
    opInit->sourceFile   = context->sourceFile;
    opInit->typeInfoFunc = g_TypeMgr.typeInfoOpCall;
    opInit->name         = structNode->ownerScope->fullname + "_" + structNode->name + "_opInit";
    replaceAll(opInit->name, '.', '_');
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
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        SWAG_CHECK(emitClearRefConstantSize(&cxt, typeInfoStruct->sizeOf, 0));
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    for (auto param : typeInfoStruct->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(param->node, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(param->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (param->offset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = param->offset;

        if (varDecl->assignment)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto exprList = CastAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                auto typeList = CastTypeInfo<TypeInfoList>(varDecl->assignment->typeInfo, TypeInfoKind::TypeList);

                auto inst = emitInstruction(&cxt, ByteCodeOp::RARefFromConstantSeg, 1, 2);
                SWAG_ASSERT(exprList->storageOffsetSegment != UINT32_MAX);
                inst->c.u64 = ((uint64_t) exprList->storageOffsetSegment << 32) | (uint32_t) typeList->childs.size();

                emitInstruction(&cxt, ByteCodeOp::CopyVC, 0, 1)->c.u32 = typeVar->sizeOf;
            }
            else if (typeVar->isNative(NativeTypeKind::String))
            {
                auto module      = sourceFile->module;
                auto stringIndex = module->reserveString(varDecl->assignment->computedValue.text);

                emitInstruction(&cxt, ByteCodeOp::CopyRARBStr, 1, 2)->c.u32 = stringIndex;
                emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 1, 0);
                emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 2, 8);
            }
            else if (typeVar->kind == TypeInfoKind::Native)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB64, 1)->b.u64 = varDecl->assignment->computedValue.reg.u64;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 1);
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
        bool done = false;

        // Whatever, generated opInit function, as the user can call it
        if (typeVar->kind == TypeInfoKind::Struct)
        {
            // Function call if necessary
            auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
            if (typeVarStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            {
                emitInstruction(&cxt, ByteCodeOp::PushRAParam);
                auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
                SWAG_ASSERT(typeVarStruct->opInit);
                inst->a.pointer = (uint8_t*) typeVarStruct->opInit;
                inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
                emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
                done = true;
            }
        }

        if (!done)
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

    SWAG_ASSERT(typeInfoStruct->structNode);
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    // Do we need a drop ?
    bool needDrop = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable->mutex);
        auto        symbol = typeInfoStruct->scope->symTable->findNoLock("opDrop");
        if (symbol && symbol->cptOverloads)
        {
            symbol->dependentJobs.add(context->job);
            context->job->setPending();
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserDropFct)
    {
        needDrop = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    if (!needDrop)
    {
        for (auto typeParam : typeInfoStruct->childs)
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

    auto opDrop            = g_Pool_byteCode.alloc();
    opDrop->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opDrop = opDrop;
    opDrop->sourceFile     = sourceFile;
    opDrop->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opDropGenerated";
    replaceAll(opDrop->name, '.', '_');
    opDrop->maxReservedRegisterRC = 3;
    opDrop->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opDrop);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;

    for (auto typeParam : typeInfoStruct->childs)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (typeParam->offset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB)->b.u32 = typeParam->offset;

        // Call drop
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeStructVar->opDrop);
        inst->a.pointer = (uint8_t*) typeStructVar->opDrop;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user drop if defined
    if (typeInfoStruct->opUserDropFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeInfoStruct->opUserDropFct->bc);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserDropFct->bc;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

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
    auto structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    // Do we need a postmove ?
    bool needPostMove = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable->mutex);
        auto        symbol = typeInfoStruct->scope->symTable->findNoLock("opPostMove");
        if (symbol && symbol->cptOverloads)
        {
            symbol->dependentJobs.add(context->job);
            context->job->setPending();
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostMoveFct)
    {
        needPostMove = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    if (!needPostMove)
    {
        for (auto typeParam : typeInfoStruct->childs)
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

    auto opPostMove            = g_Pool_byteCode.alloc();
    opPostMove->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opPostMove = opPostMove;
    opPostMove->sourceFile     = sourceFile;
    opPostMove->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opPostMoveGenerated";
    replaceAll(opPostMove->name, '.', '_');
    opPostMove->maxReservedRegisterRC = 3;
    opPostMove->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opPostMove);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;

    for (auto typeParam : typeInfoStruct->childs)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (typeParam->offset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = typeParam->offset;

        // Call postmove
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeStructVar->opPostMove);
        inst->a.pointer = (uint8_t*) typeStructVar->opPostMove;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user postmove if defined
    if (typeInfoStruct->opUserPostMoveFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeInfoStruct->opUserPostMoveFct->bc);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserPostMoveFct->bc;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

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
    auto structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    // Do we need a postcopy ?
    bool needPostCopy = false;

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable->mutex);
        auto        symbol = typeInfoStruct->scope->symTable->findNoLock("opPostCopy");
        if (symbol && symbol->cptOverloads)
        {
            symbol->dependentJobs.add(context->job);
            context->job->setPending();
            return true;
        }
    }

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostCopyFct)
    {
        needPostCopy = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    if (!needPostCopy)
    {
        for (auto typeParam : typeInfoStruct->childs)
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

    auto opPostCopy            = g_Pool_byteCode.alloc();
    opPostCopy->typeInfoFunc   = g_TypeMgr.typeInfoOpCall;
    typeInfoStruct->opPostCopy = opPostCopy;
    opPostCopy->sourceFile     = sourceFile;
    opPostCopy->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opPostCopyGenerated";
    replaceAll(opPostCopy->name, '.', '_');
    opPostCopy->maxReservedRegisterRC = 3;
    opPostCopy->compilerGenerated     = true;
    sourceFile->module->addByteCodeFunc(opPostCopy);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;

    for (auto typeParam : typeInfoStruct->childs)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (typeParam->offset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = typeParam->offset;

        // Call postcopy
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeStructVar->opPostCopy);
        inst->a.pointer = (uint8_t*) typeStructVar->opPostCopy;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user postcopy if defined
    if (typeInfoStruct->opUserPostCopyFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam);
        auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeInfoStruct->opUserPostCopyFct->bc);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserPostCopyFct->bc;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    return true;
}

void ByteCodeGenJob::waitStructGenerated(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    if (typeInfoStruct->structNode->kind == AstNodeKind::InterfaceDecl)
        return;

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    if (!(structNode->flags & AST_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(context->job);
        context->job->setPending();
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

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    structNode->flags |= AST_BYTECODE_GENERATED;
    for (auto job : node->dependentJobs.list)
        g_ThreadMgr.addJob(job);
    node->dependentJobs.clear();

    return true;
}

bool ByteCodeGenJob::emitStructCopyMoveCall(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* typeInfo, AstNode* from)
{
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    bool mustCopy = (from->flags & (AST_TRANSIENT | AST_FORCE_MOVE)) ? false : true;

    // Shallow copy
    emitInstruction(context, ByteCodeOp::CopyVC, r0, r1)->c.u32 = typeInfoStruct->sizeOf;

    // A copy
    if (mustCopy)
    {
        if (typeInfoStruct->opPostCopy)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
            SWAG_ASSERT(typeInfoStruct->opPostCopy);
            inst->a.pointer = (uint8_t*) typeInfoStruct->opPostCopy;
            inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
            emitInstruction(context, ByteCodeOp::IncSP, 8);
        }
    }

    // A move
    else
    {
        if (typeInfoStruct->opPostMove)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r1);
            auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
            SWAG_ASSERT(typeInfoStruct->opPostMove);
            inst->a.pointer = (uint8_t*) typeInfoStruct->opPostMove;
            inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
            emitInstruction(context, ByteCodeOp::IncSP, 8);
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
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfoStruct->sizeOf;
    }
    else
    {
        SWAG_ASSERT(node->ownerFct);

        // Push self
        RegisterList r0   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32       = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer, r0, regOffset, r0);

        // Then call
        emitInstruction(context, ByteCodeOp::PushRAParam, r0);
        inst = emitInstruction(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(typeInfoStruct->opInit);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opInit;
        inst->b.pointer = (uint8_t*) g_TypeMgr.typeInfoOpCall;
        emitInstruction(context, ByteCodeOp::IncSP, 8);
        freeRegisterRC(context, r0);
    }

    return true;
}

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset)
{
    auto node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::LetDecl);
    auto resolved = node->resolvedSymbolOverload;

    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        RegisterList r0   = reserveRegisterRC(context);
        RegisterList r1   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32       = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer, r0, regOffset, r0);

        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        if (identifier->callParameters)
        {
            for (auto child : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                SWAG_ASSERT(param->resolvedParameter);
                auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
                emitInstruction(context, ByteCodeOp::CopyRARB, r1, r0);
                if (typeParam->offset)
                    emitInstruction(context, ByteCodeOp::IncRAVB, r1)->b.u32 = typeParam->offset;
                emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
                freeRegisterRC(context, child);
            }
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }
}
