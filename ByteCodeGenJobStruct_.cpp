#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "SemanticJob.h"
#include "TypeManager.h"

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

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserDropFct)
    {
        needDrop = true;
        askForByteCode(context, (AstFuncDecl*) typeInfoStruct->opUserDropFct);
        if (context->result == ByteCodeResult::Pending)
            return true;
    }

    if (!needDrop)
    {
        for (auto child : structNode->content->childs)
        {
            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
            auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            generateStruct_opDrop(context, typeStructVar);
            if (context->result == ByteCodeResult::Pending)
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
    typeInfoStruct->opDrop = opDrop;
    opDrop->sourceFile     = sourceFile;
    opDrop->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opDropGenerated";
    replaceAll(opDrop->name, '.', '_');
    opDrop->typeInfoFunc          = CastTypeInfo<TypeInfoFuncAttr>(typeInfoStruct->opInitFct->typeInfo, TypeInfoKind::FuncAttr);
    opDrop->maxReservedRegisterRC = 3;
    sourceFile->module->addByteCodeFunc(opDrop);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;

    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (varDecl->resolvedSymbolOverload->storageOffset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

        // Call drop
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeStructVar->opDrop;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opDrop->typeInfoFunc;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user drop if defined
    if (typeInfoStruct->opUserDropFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserDropFct->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opDrop->typeInfoFunc;
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

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostMoveFct)
    {
        needPostMove = true;
        askForByteCode(context, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct);
        if (context->result == ByteCodeResult::Pending)
            return true;
    }

    if (!needPostMove)
    {
        for (auto child : structNode->content->childs)
        {
            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
            auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            generateStruct_opPostMove(context, typeStructVar);
            if (context->result == ByteCodeResult::Pending)
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
    typeInfoStruct->opPostMove = opPostMove;
    opPostMove->sourceFile     = sourceFile;
    opPostMove->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opPostMoveGenerated";
    replaceAll(opPostMove->name, '.', '_');
    opPostMove->typeInfoFunc          = CastTypeInfo<TypeInfoFuncAttr>(typeInfoStruct->opInitFct->typeInfo, TypeInfoKind::FuncAttr);
    opPostMove->maxReservedRegisterRC = 3;
    sourceFile->module->addByteCodeFunc(opPostMove);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;

    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (varDecl->resolvedSymbolOverload->storageOffset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

        // Call postmove
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeStructVar->opPostMove;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opPostMove->typeInfoFunc;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user postmove if defined
    if (typeInfoStruct->opUserPostMoveFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserPostMoveFct->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opPostMove->typeInfoFunc;
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

    // Need to wait for function full semantic resolve
    if (typeInfoStruct->opUserPostCopyFct)
    {
        needPostCopy = true;
        askForByteCode(context, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct);
        if (context->result == ByteCodeResult::Pending)
            return true;
    }

    if (!needPostCopy)
    {
        for (auto child : structNode->content->childs)
        {
            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
            auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
            if (typeVar->kind != TypeInfoKind::Struct)
                continue;
            auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
            generateStruct_opPostCopy(context, typeStructVar);
            if (context->result == ByteCodeResult::Pending)
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
    typeInfoStruct->opPostCopy = opPostCopy;
    opPostCopy->sourceFile     = sourceFile;
    opPostCopy->name           = structNode->ownerScope->fullname + "_" + structNode->name + "_opPostCopyGenerated";
    replaceAll(opPostCopy->name, '.', '_');
    opPostCopy->typeInfoFunc          = CastTypeInfo<TypeInfoFuncAttr>(typeInfoStruct->opInitFct->typeInfo, TypeInfoKind::FuncAttr);
    opPostCopy->maxReservedRegisterRC = 3;
    sourceFile->module->addByteCodeFunc(opPostCopy);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;

    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (varDecl->resolvedSymbolOverload->storageOffset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

        // Call postcopy
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeStructVar->opPostCopy;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opPostCopy->typeInfoFunc;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    // Then call user postcopy if defined
    if (typeInfoStruct->opUserPostCopyFct)
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opUserPostCopyFct->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) opPostCopy->typeInfoFunc;
        emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    return true;
}

bool ByteCodeGenJob::prepareEmitStructDrop(ByteCodeGenContext* context, TypeInfo* typeInfo)
{
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(generateStruct_opDrop(context, typeInfoStruct));
    if (context->result == ByteCodeResult::Pending)
        return true;
    return true;
}

bool ByteCodeGenJob::prepareEmitStructCopyMove(ByteCodeGenContext* context, TypeInfo* typeInfo)
{
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(generateStruct_opPostCopy(context, typeInfoStruct));
    if (context->result == ByteCodeResult::Pending)
        return true;
    SWAG_CHECK(generateStruct_opPostMove(context, typeInfoStruct));
    if (context->result == ByteCodeResult::Pending)
        return true;
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
            auto inst       = emitInstruction(context, ByteCodeOp::LocalCall);
            inst->a.pointer = (uint8_t*) typeInfoStruct->opPostCopy;
            inst->b.u64     = 1;
            inst->c.pointer = (uint8_t*) typeInfoStruct->opInitFct->typeInfo;
            emitInstruction(context, ByteCodeOp::IncSP, 8);
        }
    }

    // A move
    else
    {
        if (typeInfoStruct->opPostMove)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r1);
            auto inst       = emitInstruction(context, ByteCodeOp::LocalCall);
            inst->a.pointer = (uint8_t*) typeInfoStruct->opPostMove;
            inst->b.u64     = 1;
            inst->c.pointer = (uint8_t*) typeInfoStruct->opInitFct->typeInfo;
            emitInstruction(context, ByteCodeOp::IncSP, 8);
        }
    }

    return true;
}

bool ByteCodeGenJob::generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    auto sourceFile   = context->sourceFile;
    auto opInitFct    = CastAst<AstFuncDecl>(typeInfoStruct->opInitFct, AstNodeKind::FuncDecl);
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(opInitFct->typeInfo, TypeInfoKind::FuncAttr);
    auto structNode   = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    scoped_lock lk(typeInfoStruct->opInitFct->mutex);
    if (typeInfoStruct->opInitFct->bc && typeInfoStruct->opInitFct->bc->out)
        return true;

    ByteCode* opInit;
    if (typeInfoStruct->opInitFct->bc)
        opInit = typeInfoStruct->opInitFct->bc;
    else
        opInit = g_Pool_byteCode.alloc();
    opInit->sourceFile = context->sourceFile;
    opInit->name       = structNode->ownerScope->fullname + "_" + structNode->name + "_opInit";
    replaceAll(opInit->name, '.', '_');
    opInit->typeInfoFunc          = typeInfoFunc;
    opInit->maxReservedRegisterRC = 3;

    if (!typeInfoStruct->opInitFct->bc)
        sourceFile->module->addByteCodeFunc(opInit);
    typeInfoStruct->opInitFct->bc = opInit;

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
        switch (typeInfoStruct->sizeOf)
        {
        case 1:
            emitInstruction(&cxt, ByteCodeOp::Clear8, 0);
            break;
        case 2:
            emitInstruction(&cxt, ByteCodeOp::Clear16, 0);
            break;
        case 4:
            emitInstruction(&cxt, ByteCodeOp::Clear32, 0);
            break;
        case 8:
            emitInstruction(&cxt, ByteCodeOp::Clear64, 0);
            break;
        default:
            emitInstruction(&cxt, ByteCodeOp::ClearX, 0)->b.u32 = typeInfoStruct->sizeOf;
            break;
        }

        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(varDecl->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (varDecl->resolvedSymbolOverload->storageOffset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

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
        }
        else
        {
            bool done = false;

            // Whatever, generated opInit function, as the user can call it
            if (typeVar->kind == TypeInfoKind::Struct)
            {
                auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
                SWAG_ASSERT(typeVarStruct->opInitFct);
                if (!generateStruct_opInit(context, typeVarStruct))
                    return false;

                // Function call if necessary
                if (typeVar->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
                {
                    emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                    auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
                    SWAG_ASSERT(typeInfoStruct->opInitFct->bc->out);
                    inst->a.pointer = (uint8_t*) typeVarStruct->opInitFct->bc;
                    inst->b.u64     = 1;
                    inst->c.pointer = (uint8_t*) typeInfoFunc;
                    emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
                    done = true;
                }
            }

            if (!done)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::Clear8, 0);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::Clear16, 0);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::Clear32, 0);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::Clear64, 0);
                    break;
                default:
                    emitInstruction(&cxt, ByteCodeOp::ClearX, 0)->b.u32 = typeVar->sizeOf;
                    break;
                }
            }
        }
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    //opInit->print();
    return true;
}

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset)
{
    auto node     = static_cast<AstVarDecl*>(context->node);
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
        for (auto child : identifier->callParameters->childs)
        {
            auto param     = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
            emitInstruction(context, ByteCodeOp::CopyRARB, r1, r0);
            emitInstruction(context, ByteCodeOp::IncRAVB, r1)->b.u32 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
            freeRegisterRC(context, child);
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, uint32_t regOffset)
{
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;

    // Be sure referenced function has bytecode
    if (!generateStruct_opInit(context, typeInfoStruct))
        return false;

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
        emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
        inst            = emitInstruction(context, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opInitFct->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) typeInfoStruct->opInitFct->typeInfo;
        emitInstruction(context, ByteCodeOp::IncSP, 8);

        freeRegisterRC(context, r0);
    }

    return true;
}
