#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "SemanticJob.h"

bool ByteCodeGenJob::canEmitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc)
{
    if (!funcDecl && !bc)
        return false;

    auto node        = context->node;
    auto module      = node->sourceFile->module;
    bool foreignCall = funcDecl && !bc && (funcDecl->attributeFlags & ATTRIBUTE_FOREIGN);
    if (foreignCall)
        return true;

    SWAG_ASSERT(bc || (funcDecl && funcDecl->extension && funcDecl->extension->bc));
    bc = bc ? bc : funcDecl->extension->bc;
    if (bc->isDoingNothing() && module->mustOptimizeBC(node))
        return false;
    return true;
}

void ByteCodeGenJob::emitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc, bool pushParam, uint32_t offset, uint32_t numParams)
{
    if (!funcDecl && !bc)
        return;

    if (pushParam)
    {
        SWAG_ASSERT(numParams == 1);
        emitInstruction(context, ByteCodeOp::GetFromStackParam64, 0, 24);
        if (offset)
        {
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, 0, 0, 0);
            SWAG_ASSERT(offset != 0xFFFFFFFF);
            inst->b.u64 = offset;
            inst->flags |= BCI_IMM_B;
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
        SWAG_ASSERT(bc || (funcDecl && funcDecl->extension && funcDecl->extension->bc));
        inst->a.pointer = (uint8_t*) (bc ? bc : funcDecl->extension->bc);
        SWAG_ASSERT(numParams <= 2);
        inst->b.pointer = numParams == 1 ? (uint8_t*) g_TypeMgr.typeInfoOpCall : (uint8_t*) g_TypeMgr.typeInfoOpCall2;
        SWAG_ASSERT(inst->a.pointer);
    }

    emitInstruction(context, ByteCodeOp::IncSPPostCall, numParams * 8);
}

bool ByteCodeGenJob::generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->opInit)
        return true;

    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Need to be sure that function has been solved
    {
        scoped_lock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto        symbol = typeInfoStruct->scope->symTable.findNoLock("opInit");
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, "EMIT_INIT", structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserInitFct && typeInfoStruct->opUserInitFct->attributeFlags & ATTRIBUTE_FOREIGN)
        return true;

    // Need to wait for user function full semantic resolve
    if (typeInfoStruct->opUserInitFct)
    {
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserInitFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Be sure sub structs are generated too
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

    ByteCode* opInit     = g_Allocator.alloc<ByteCode>();
    opInit->sourceFile   = context->sourceFile;
    opInit->typeInfoFunc = g_TypeMgr.typeInfoOpCall;
    opInit->name         = structNode->ownerScope->getFullName() + "_" + structNode->token.text.c_str() + "_opInit";
    opInit->name.replaceAll('.', '_');
    opInit->maxReservedRegisterRC = 3;
    opInit->compilerGenerated     = true;
    typeInfoStruct->opInit        = opInit;

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opInit->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = "opInitGenerated";
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserInitFct)
            typeInfoStruct->opUserInitFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opInit;
        opInit->node            = funcNode;
    }

    sourceFile->module->addByteCodeFunc(opInit);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;

    // All fields are explicitly not initialized, so we are done, function is empty
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
    {
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
        {
            unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }
        return true;
    }

    // No special value, so we can just clear the struct
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        emitInstruction(&cxt, ByteCodeOp::GetFromStackParam64, 0, 24);
        emitSetZeroAtPointer(&cxt, typeInfoStruct->sizeOf, 0);
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
        {
            unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }
        return true;
    }

    for (auto param : typeInfoStruct->fields)
    {
        auto varDecl = CastAst<AstVarDecl>(param->declNode, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(param->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::GetFromStackParam64, 0, 24);
        if (param->offset)
        {
            auto inst = emitInstruction(&cxt, ByteCodeOp::IncPointer64, 0, 0, 0);
            SWAG_ASSERT(param->offset != 0xFFFFFFFF);
            inst->b.u64 = param->offset;
            inst->flags |= BCI_IMM_B;
        }

        // A structure initialized with a literal
        if (varDecl->type && varDecl->type->flags & AST_HAS_STRUCT_PARAMETERS)
        {
            auto varType = varDecl->type;
            SWAG_ASSERT(varType->computedValue.storageSegment);
            SWAG_ASSERT(varType->computedValue.storageOffset != 0xFFFFFFFF);
            emitMakeSegPointer(&cxt, varType->computedValue.storageSegment, 1, varType->computedValue.storageOffset);
            emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            continue;
        }

        // User initialization
        if (varDecl->assignment)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto exprList = CastAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                SWAG_ASSERT(exprList->computedValue.storageSegment);
                SWAG_ASSERT(exprList->computedValue.storageOffset != 0xFFFFFFFF);
                emitMakeSegPointer(&cxt, exprList->computedValue.storageSegment, 1, exprList->computedValue.storageOffset);
                emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            }
            else if (typeVar->isNative(NativeTypeKind::String))
            {
                auto storageSegment = SemanticJob::getConstantSegFromContext(varDecl);
                auto storageOffset  = storageSegment->addString(varDecl->assignment->computedValue.text);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, storageSegment, 1, storageOffset);
                emitInstruction(&cxt, ByteCodeOp::SetImmediate64, 2)->b.u64 = varDecl->assignment->computedValue.text.length();
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 2)->c.u32 = 8;
            }
            else
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue.reg.u32;
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
                    emitOpCallUser(&cxt, typeInVarStruct->opUserInitFct, typeInVarStruct->opInit, false);
                }
                else
                {
                    // Need to loop on every element of the array in order to initialize them
                    RegisterList r0 = reserveRegisterRC(&cxt);

                    auto inst     = emitInstruction(&cxt, ByteCodeOp::SetImmediate32, r0);
                    inst->b.u64   = typeArray->totalCount;
                    auto seekJump = cxt.bc->numInstructions;

                    emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                    emitOpCallUser(&cxt, typeInVarStruct->opUserInitFct, typeInVarStruct->opInit, false);

                    inst = emitInstruction(&cxt, ByteCodeOp::IncPointer64, 0, 0, 0);
                    inst->flags |= BCI_IMM_B;
                    inst->b.u64 = typeInVarStruct->sizeOf;

                    emitInstruction(&cxt, ByteCodeOp::DecrementRA32, r0);
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
            SWAG_ASSERT(typeVarStruct->opInit || typeVarStruct->opUserInitFct);
            emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
            emitOpCallUser(&cxt, typeVarStruct->opUserInitFct, typeVarStruct->opInit, false);
        }
        else
        {
            emitSetZeroAtPointer(&cxt, typeVar->sizeOf, 0);
        }
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    return true;
}

bool ByteCodeGenJob::generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_DROP)
        return true;
    if (typeInfoStruct->opDrop)
        return true;

    SWAG_ASSERT(typeInfoStruct->declNode);
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

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

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserDropFct && typeInfoStruct->opUserDropFct->attributeFlags & ATTRIBUTE_FOREIGN)
        return true;

    // Need to wait for user function full semantic resolve
    bool needDrop = false;
    if (typeInfoStruct->opUserDropFct)
    {
        needDrop = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Be sure sub structs are generated too
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
        generateStruct_opDrop(context, typeStructVar);
        if (context->result == ContextResult::Pending)
            return true;
        if (typeStructVar->opDrop || typeStructVar->opUserDropFct)
            needDrop = true;
        if (typeStructVar->opDrop || typeStructVar->opUserDropFct || typeStructVar->flags & TYPEINFO_HAD_DROP)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report({typeParam->declNode, Utf8::format(Msg0911, typeStructVar->getDisplayName().c_str())}));
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

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opDrop->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = "opDropGenerated";
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserDropFct)
            typeInfoStruct->opUserDropFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opDrop;
        opDrop->node            = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;

    // Call user function if defined
    if (canEmitOpCallUser(&cxt, typeInfoStruct->opUserDropFct))
        emitOpCallUser(&cxt, typeInfoStruct->opUserDropFct);

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        if (!canEmitOpCallUser(&cxt, typeStructVar->opUserDropFct, typeStructVar->opDrop))
            continue;
        emitOpCallUser(&cxt, typeStructVar->opUserDropFct, typeStructVar->opDrop, true, typeParam->offset);
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    // Revert back function because is empty
    if (!canEmitOpCallUser(&cxt, nullptr, cxt.bc))
    {
        typeInfoStruct->opDrop        = nullptr;
        typeInfoStruct->opUserDropFct = nullptr;
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_DROP | TYPEINFO_HAD_DROP;
        return true;
    }

    sourceFile->module->addByteCodeFunc(opDrop);
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_POST_MOVE)
        return true;
    if (typeInfoStruct->opPostMove)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

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

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserPostMoveFct && typeInfoStruct->opUserPostMoveFct->attributeFlags & ATTRIBUTE_FOREIGN)
        return true;

    // Need to wait for function full semantic resolve
    bool needPostMove = false;
    if (typeInfoStruct->opUserPostMoveFct)
    {
        needPostMove = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Be sure sub structs are generated too
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
        generateStruct_opPostMove(context, typeStructVar);
        if (context->result == ContextResult::Pending)
            return true;
        if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct)
            needPostMove = true;
        if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct || typeStructVar->flags & TYPEINFO_HAD_POST_MOVE)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report({typeParam->declNode, Utf8::format(Msg0910, typeStructVar->getDisplayName().c_str())}));
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

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opPostMove->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = "opPostMoveGenerated";
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserPostMoveFct)
            typeInfoStruct->opUserPostMoveFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opPostMove;
        opPostMove->node        = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        if (!canEmitOpCallUser(&cxt, typeStructVar->opUserPostMoveFct, typeStructVar->opPostMove))
            continue;
        emitOpCallUser(&cxt, typeStructVar->opUserPostMoveFct, typeStructVar->opPostMove, true, typeParam->offset);
    }

    // Then call user function if defined
    if (canEmitOpCallUser(&cxt, typeInfoStruct->opUserPostMoveFct))
        emitOpCallUser(&cxt, typeInfoStruct->opUserPostMoveFct);

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    // Revert back function because it's empty
    if (!canEmitOpCallUser(&cxt, nullptr, cxt.bc))
    {
        typeInfoStruct->opPostMove = nullptr;
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_MOVE | TYPEINFO_HAD_POST_MOVE;
        return true;
    }

    sourceFile->module->addByteCodeFunc(opPostMove);
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    scoped_lock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & (TYPEINFO_STRUCT_NO_POST_COPY | TYPEINFO_STRUCT_NO_COPY))
        return true;
    if (typeInfoStruct->opPostCopy)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

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

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserPostCopyFct && typeInfoStruct->opUserPostCopyFct->attributeFlags & ATTRIBUTE_FOREIGN)
        return true;

    // Need to wait for function full semantic resolve
    bool needPostCopy = false;
    if (typeInfoStruct->opUserPostCopyFct)
    {
        needPostCopy = true;
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED | ASKBC_ADD_DEP_NODE);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Be sure sub structs are generated too
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
        generateStruct_opPostCopy(context, typeStructVar);
        if (context->result == ContextResult::Pending)
            return true;
        if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct)
            needPostCopy = true;
        if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct || typeStructVar->flags & TYPEINFO_HAD_POST_COPY)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report({typeParam->declNode, Utf8::format(Msg0909, typeStructVar->getDisplayName().c_str())}));
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

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opPostCopy->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = "opPostCopyGenerated";
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserPostCopyFct)
            typeInfoStruct->opUserPostCopyFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opPostCopy;
        opPostCopy->node        = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        if (!canEmitOpCallUser(&cxt, typeStructVar->opUserPostCopyFct, typeStructVar->opPostCopy))
            continue;
        emitOpCallUser(&cxt, typeStructVar->opUserPostCopyFct, typeStructVar->opPostCopy, true, typeParam->offset);
    }

    // Then call user function if defined
    if (canEmitOpCallUser(&cxt, typeInfoStruct->opUserPostCopyFct))
        emitOpCallUser(&cxt, typeInfoStruct->opUserPostCopyFct);

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        unique_lock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    // Revert back function because it's empty
    if (!canEmitOpCallUser(&cxt, nullptr, cxt.bc))
    {
        typeInfoStruct->opPostCopy = nullptr;
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_COPY | TYPEINFO_HAD_POST_COPY;
        return true;
    }

    sourceFile->module->addByteCodeFunc(opPostCopy);
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
    structNode->semFlags |= AST_SEM_BYTECODE_GENERATED;
    node->dependentJobs.setRunning();
    return true;
}

bool ByteCodeGenJob::emitCopyStruct(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* typeInfo, AstNode* from)
{
    TypeInfoStruct* typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    // Need to drop first
    if (typeInfoStruct->opDrop || typeInfoStruct->opUserDropFct)
    {
        bool mustDrop = (from->flags & AST_NO_LEFT_DROP) ? false : true;
        if (mustDrop)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserDropFct, typeInfoStruct->opDrop, false);
        }
    }

    // Shallow copy
    emitMemCpy(context, r0, r1, typeInfoStruct->sizeOf);

    // A copy
    bool mustCopy = (from->flags & (AST_TRANSIENT | AST_FORCE_MOVE)) ? false : true;
    if (mustCopy)
    {
        if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_COPY)
            return context->report({context->node, Utf8::format(Msg0231, typeInfo->getDisplayName().c_str())});

        PushICFlags sf(context, BCI_POST_COPYMOVE);
        if (typeInfoStruct->opPostCopy || typeInfoStruct->opUserPostCopyFct)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserPostCopyFct, typeInfoStruct->opPostCopy, false);
        }
    }

    // A move
    else
    {
        PushICFlags sf(context, BCI_POST_COPYMOVE);
        if (typeInfoStruct->opPostMove || typeInfoStruct->opUserPostMoveFct)
        {
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, typeInfoStruct->opUserPostMoveFct, typeInfoStruct->opPostMove, false);
        }

        // If the current scope contains a drop for that variable, then we remove it, because we have
        // just reset the content
        bool mustReinit = true;
        for (auto& toDrop : from->ownerScope->symTable.structVarsToDrop)
        {
            if (toDrop.overload && toDrop.overload->symbol->kind == SymbolKind::Function && from->kind == AstNodeKind::IdentifierRef)
            {
                if (toDrop.overload == from->resolvedSymbolOverload && toDrop.storageOffset == from->childs.back()->computedValue.storageOffset)
                {
                    mustReinit        = false;
                    toDrop.typeStruct = nullptr;
                    break;
                }
            }
        }

        // Reinit source struct, except if AST_NO_RIGHT_DROP, because if we do not drop the
        // right expression, then this is not necessary to reinitialize it
        // Note that if we have remove the opDrop in the code above, no need to reinitialize the variable.
        if (mustReinit && (typeInfoStruct->opDrop || typeInfoStruct->opUserDropFct) && !(from->flags & AST_NO_RIGHT_DROP))
        {
            if ((typeInfoStruct->opInit || typeInfoStruct->opUserInitFct) && (typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
            {
                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, typeInfoStruct->opUserInitFct, typeInfoStruct->opInit, false);
            }
            else
            {
                emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r1);
            }
        }
    }

    return true;
}

void ByteCodeGenJob::emitRetValRef(ByteCodeGenContext* context, RegisterList& r0, bool retVal, uint32_t stackOffset)
{
    auto node = context->node;
    if (retVal)
    {
        if (node->resolvedSymbolOverload->node->ownerInline)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0, node->resolvedSymbolOverload->node->ownerInline->resultRegisterRC);
        else
            emitInstruction(context, ByteCodeOp::CopyRRtoRC, r0);
    }
    else
    {
        auto inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
        SWAG_ASSERT(stackOffset != UINT32_MAX);
        inst->b.s32 = stackOffset;
    }
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
            emitRetValRef(context, r0, true, 0);
            emitSetZeroAtPointer(context, typeInfoStruct->sizeOf, r0);
            freeRegisterRC(context, r0);
        }
        else
        {
            auto inst = emitInstruction(context, ByteCodeOp::SetZeroStackX);
            SWAG_ASSERT(resolved->computedValue.storageOffset != UINT32_MAX);
            inst->a.u32 = resolved->computedValue.storageOffset;
            inst->b.u64 = typeInfoStruct->sizeOf;
        }
    }
    else
    {
        // Push self
        RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, r0, retVal, resolved->computedValue.storageOffset);

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, regOffset, r0);

        // Then call
        SWAG_ASSERT(typeInfoStruct->opInit || typeInfoStruct->opUserInitFct);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0);
        emitOpCallUser(context, typeInfoStruct->opUserInitFct, typeInfoStruct->opInit, false);
        freeRegisterRC(context, r0);
    }

    return true;
}

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset, bool retVal)
{
    PushContextFlags cf(context, BCC_FLAG_NOSAFETY);
    auto             node     = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
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

                emitRetValRef(context, r0, retVal, resolved->computedValue.storageOffset + typeParam->offset);
                if (retVal && typeParam->offset)
                {
                    auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, r0, 0, r0);
                    SWAG_ASSERT(typeParam->offset != 0xFFFFFFFF);
                    inst->b.u64 = typeParam->offset;
                    inst->flags |= BCI_IMM_B;
                }

                if (regOffset != UINT32_MAX)
                    emitInstruction(context, ByteCodeOp::IncPointer64, r0, regOffset, r0);

                child->flags |= AST_NO_LEFT_DROP;
                emitAffectEqual(context, r0, child->resultRegisterRC, TypeManager::concreteReference(child->typeInfo), child);
                SWAG_ASSERT(context->result == ContextResult::Done);
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