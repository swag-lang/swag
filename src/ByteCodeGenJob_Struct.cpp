#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "SemanticJob.h"
#include "LanguageSpec.h"
#include "ModuleManager.h"

void ByteCodeGenJob::emitOpCallUser(ByteCodeGenContext* context, AstFuncDecl* funcDecl, ByteCode* bc, bool pushParam, uint32_t offset, uint32_t numParams)
{
    if (!funcDecl && !bc)
        return;

    if (funcDecl)
        askForByteCode(context->job, funcDecl, 0, context->bc);
    else if (bc && bc->node)
        askForByteCode(context->job, bc->node, 0, context->bc);

    if (pushParam)
    {
        SWAG_ASSERT(numParams == 1);
        emitInstruction(context, ByteCodeOp::GetParam64, 0, 24);
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
        context->bc->hasForeignFunctionCallsModules.insert(ModuleManager::getForeignModuleName(funcDecl));
        SWAG_ASSERT(inst->a.pointer);
    }
    else
    {
        auto inst = emitInstruction(context, ByteCodeOp::LocalCall);
        SWAG_ASSERT(bc || (funcDecl && funcDecl->extension && funcDecl->extension->bc));
        auto bcReal     = bc ? bc : funcDecl->extension->bc;
        bcReal->isUsed  = true;
        inst->a.pointer = (uint8_t*) bcReal;
        SWAG_ASSERT(numParams <= 2);
        inst->b.pointer = numParams == 1 ? (uint8_t*) g_TypeMgr->typeInfoOpCall : (uint8_t*) g_TypeMgr->typeInfoOpCall2;
        SWAG_ASSERT(inst->a.pointer);
    }

    emitInstruction(context, ByteCodeOp::IncSPPostCall, numParams * 8);
}

bool ByteCodeGenJob::generateStruct_opInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->opInit)
        return true;

    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto       symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opInit);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, JobWaitKind::EmitInit, structNode, nullptr);
            return true;
        }
    }

    // If user function is foreign, then this is the generated version with everything already done
    if (typeInfoStruct->opUserInitFct && typeInfoStruct->opUserInitFct->attributeFlags & ATTRIBUTE_FOREIGN)
        return true;

    // Need to wait for user function full semantic resolve
    if (typeInfoStruct->opUserInitFct)
    {
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserInitFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
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
    opInit->typeInfoFunc = g_TypeMgr->typeInfoOpCall;
    opInit->name         = structNode->ownerScope->getFullName();
    opInit->name += "_";
    opInit->name += structNode->token.text;
    opInit->name += "_opInit";
    opInit->name.replaceAll('.', '_');
    opInit->maxReservedRegisterRC = 3;
    opInit->isCompilerGenerated   = true;
    typeInfoStruct->opInit        = opInit;

    // Export generated function if necessary
    if (!(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opInit->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = g_LangSpec->name_opInitGenerated;
        if (typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            funcNode->attributeFlags |= structNode->attributeFlags & ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserInitFct)
            typeInfoStruct->opUserInitFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opInit;
        opInit->node            = funcNode;
    }

    sourceFile->module->addByteCodeFunc(opInit);

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;
    if (cxt.bc->node)
        cxt.bc->node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;

    // All fields are explicitly not initialized, so we are done, function is empty
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
    {
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
        {
            ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }
        return true;
    }

    // No special value, so we can just clear the struct
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        emitInstruction(&cxt, ByteCodeOp::GetParam64, 0, 24);
        emitSetZeroAtPointer(&cxt, typeInfoStruct->sizeOf, 0);
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
        {
            ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
            cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
        }
        return true;
    }

    for (auto param : typeInfoStruct->fields)
    {
        auto varDecl = CastAst<AstVarDecl>(param->declNode, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(param->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::GetParam64, 0, 24);
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
            SWAG_ASSERT(varType->computedValue->storageSegment);
            SWAG_ASSERT(varType->computedValue->storageOffset != 0xFFFFFFFF);
            emitMakeSegPointer(&cxt, varType->computedValue->storageSegment, varType->computedValue->storageOffset, 1);
            emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            continue;
        }

        // User initialization
        if (varDecl->assignment && varDecl->assignment->kind != AstNodeKind::ExplicitNoInit)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto exprList = CastAst<AstExpressionList>(varDecl->assignment, AstNodeKind::ExpressionList);
                SWAG_ASSERT(exprList->computedValue->storageSegment);
                SWAG_ASSERT(exprList->computedValue->storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, exprList->computedValue->storageSegment, exprList->computedValue->storageOffset, 1);
                emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            }
            else if (typeVar->isNative(NativeTypeKind::String))
            {
                auto storageSegment = SemanticJob::getConstantSegFromContext(varDecl);
                auto storageOffset  = storageSegment->addString(varDecl->assignment->computedValue->text);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, storageSegment, storageOffset, 1);
                emitInstruction(&cxt, ByteCodeOp::SetImmediate64, 2)->b.u64 = varDecl->assignment->computedValue->text.length();
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 2)->c.u32 = 8;
            }
            // :opAffectConstExpr
            else if (typeVar->kind == TypeInfoKind::Struct &&
                     varDecl->resolvedSymbolOverload &&
                     varDecl->resolvedSymbolOverload->flags & OVERLOAD_STRUCT_AFFECT &&
                     varDecl->computedValue &&
                     varDecl->computedValue->storageSegment)
            {
                auto storageSegment = varDecl->computedValue->storageSegment;
                auto storageOffset  = varDecl->computedValue->storageOffset;
                SWAG_ASSERT(storageSegment);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(&cxt, storageSegment, storageOffset, 1);
                emitMemCpy(&cxt, 0, 1, typeVar->sizeOf);
            }
            else
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue->reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue->reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate32, 1)->b.u64 = varDecl->assignment->computedValue->reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::SetImmediate64, 1)->b.u64 = varDecl->assignment->computedValue->reg.u64;
                    emitInstruction(&cxt, ByteCodeOp::SetAtPointer64, 0, 1);
                    break;
                default:
                    return context->internalError("generateStructInit, invalid native type sizeof", varDecl);
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
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    return true;
}

bool ByteCodeGenJob::generateStruct_opDrop(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_DROP)
        return true;
    if (typeInfoStruct->opDrop)
        return true;

    SWAG_ASSERT(typeInfoStruct->declNode);
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto       symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opDrop);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, JobWaitKind::EmitDrop, structNode, nullptr);
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
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserDropFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
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
        if (typeStructVar->opDrop || typeStructVar->opUserDropFct)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report(typeParam->declNode, Fmt(Err(Err0911), typeStructVar->getDisplayNameC())));
    }

    if (!needDrop)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_DROP;
        return true;
    }

    auto opDrop            = g_Allocator.alloc<ByteCode>();
    opDrop->typeInfoFunc   = g_TypeMgr->typeInfoOpCall;
    typeInfoStruct->opDrop = opDrop;
    opDrop->sourceFile     = sourceFile;
    opDrop->name           = structNode->ownerScope->getFullName();
    opDrop->name += "_";
    opDrop->name += structNode->token.text;
    opDrop->name += "_opDropGenerated";
    opDrop->name.replaceAll('.', '_');
    opDrop->maxReservedRegisterRC = 3;
    opDrop->isCompilerGenerated   = true;

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opDrop->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = g_LangSpec->name_opDropGenerated;
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserDropFct)
            typeInfoStruct->opUserDropFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opDrop;
        opDrop->node            = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opDrop;
    if (cxt.bc->node)
        cxt.bc->node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;

    // Call user function if defined
    emitOpCallUser(&cxt, typeInfoStruct->opUserDropFct);

    for (auto typeParam : typeInfoStruct->fields)
    {
        auto typeVar = TypeManager::concreteType(typeParam->typeInfo);
        if (typeVar->kind != TypeInfoKind::Struct)
            continue;
        auto typeStructVar = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
        emitOpCallUser(&cxt, typeStructVar->opUserDropFct, typeStructVar->opDrop, true, typeParam->offset);
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    sourceFile->module->addByteCodeFunc(opDrop);
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostMove(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_NO_POST_MOVE)
        return true;
    if (typeInfoStruct->opPostMove)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto       symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostMove);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, JobWaitKind::EmitPostMove, structNode, nullptr);
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
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostMoveFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
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
        if (typeStructVar->opPostMove || typeStructVar->opUserPostMoveFct)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report(typeParam->declNode, Fmt(Err(Err0910), typeStructVar->getDisplayNameC())));
    }

    if (!needPostMove)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_MOVE;
        return true;
    }

    auto opPostMove            = g_Allocator.alloc<ByteCode>();
    opPostMove->typeInfoFunc   = g_TypeMgr->typeInfoOpCall;
    typeInfoStruct->opPostMove = opPostMove;
    opPostMove->sourceFile     = sourceFile;
    opPostMove->name           = structNode->ownerScope->getFullName();
    opPostMove->name += "_";
    opPostMove->name += structNode->token.text;
    opPostMove->name += "_opPostMoveGenerated";
    opPostMove->name.replaceAll('.', '_');
    opPostMove->maxReservedRegisterRC = 3;
    opPostMove->isCompilerGenerated   = true;

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opPostMove->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = g_LangSpec->name_opPostMoveGenerated;
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserPostMoveFct)
            typeInfoStruct->opUserPostMoveFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opPostMove;
        opPostMove->node        = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostMove;
    if (cxt.bc->node)
        cxt.bc->node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;

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

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
    }

    sourceFile->module->addByteCodeFunc(opPostMove);
    return true;
}

bool ByteCodeGenJob::generateStruct_opPostCopy(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutexGen);
    if (typeInfoStruct->flags & (TYPEINFO_STRUCT_NO_POST_COPY | TYPEINFO_STRUCT_NO_COPY))
        return true;
    if (typeInfoStruct->opPostCopy)
        return true;

    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    // Need to be sure that function has been solved
    {
        ScopedLock lockTable(typeInfoStruct->scope->symTable.mutex);
        auto       symbol = typeInfoStruct->scope->symTable.findNoLock(g_LangSpec->name_opPostCopy);
        if (symbol && symbol->cptOverloads)
        {
            symbol->addDependentJob(context->job);
            context->job->setPending(symbol, JobWaitKind::EmitPostMove, structNode, nullptr);
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
        askForByteCode(context->job, (AstFuncDecl*) typeInfoStruct->opUserPostCopyFct, ASKBC_WAIT_SEMANTIC_RESOLVED, context->bc);
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
        if (typeStructVar->opPostCopy || typeStructVar->opUserPostCopyFct)
            SWAG_VERIFY(!(structNode->structFlags & STRUCTFLAG_UNION), context->report(typeParam->declNode, Fmt(Err(Err0909), typeStructVar->getDisplayNameC())));
    }

    if (!needPostCopy)
    {
        typeInfoStruct->flags |= TYPEINFO_STRUCT_NO_POST_COPY;
        return true;
    }

    auto opPostCopy            = g_Allocator.alloc<ByteCode>();
    opPostCopy->typeInfoFunc   = g_TypeMgr->typeInfoOpCall;
    typeInfoStruct->opPostCopy = opPostCopy;
    opPostCopy->sourceFile     = sourceFile;
    opPostCopy->name           = structNode->ownerScope->getFullName();
    opPostCopy->name += "_";
    opPostCopy->name += structNode->token.text;
    opPostCopy->name += "_opPostCopyGenerated";
    opPostCopy->name.replaceAll('.', '_');
    opPostCopy->maxReservedRegisterRC = 3;
    opPostCopy->isCompilerGenerated   = true;

    // Export generated function if necessary
    if (structNode->attributeFlags & ATTRIBUTE_PUBLIC && !(structNode->flags & AST_FROM_GENERIC))
    {
        auto funcNode        = Ast::newNode<AstFuncDecl>(nullptr, AstNodeKind::FuncDecl, sourceFile, structNode);
        funcNode->typeInfo   = opPostCopy->typeInfoFunc;
        funcNode->ownerScope = structNode->scope;
        funcNode->token.text = g_LangSpec->name_opPostCopyGenerated;
        funcNode->attributeFlags |= ATTRIBUTE_PUBLIC;
        if (typeInfoStruct->opUserPostCopyFct)
            typeInfoStruct->opUserPostCopyFct->attributeFlags &= ~ATTRIBUTE_PUBLIC;
        funcNode->allocateExtension();
        funcNode->extension->bc = opPostCopy;
        opPostCopy->node        = funcNode;
    }

    ByteCodeGenContext cxt{*context};
    cxt.bc = opPostCopy;
    if (cxt.bc->node)
        cxt.bc->node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;

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

    if (structNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        ScopedLock lk1(cxt.bc->sourceFile->module->mutexByteCode);
        cxt.bc->sourceFile->module->byteCodePrintBC.push_back(cxt.bc);
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

    auto       structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    ScopedLock lk(structNode->mutex);
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
        {
            return context->report({from, Fmt(Err(Err0231), typeInfo->getDisplayNameC())});
        }

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
                if (toDrop.overload == from->resolvedSymbolOverload && toDrop.storageOffset == from->childs.back()->computedValue->storageOffset)
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
        auto overload = node->resolvedSymbolOverload;
        SWAG_ASSERT(overload);
        if (overload->node->ownerInline && overload->node->ownerInline->resultRegisterRC.countResults)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0, overload->node->ownerInline->resultRegisterRC);
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
                // Already set by something else, as a direct reference, so no need to copy
                if (child->doneFlags & AST_DONE_FIELD_STRUCT)
                {
                    freeRegisterRC(context, child);
                    continue;
                }

                auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                SWAG_ASSERT(param->resolvedParameter);
                auto typeParam = param->resolvedParameter;

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

bool ByteCodeGenJob::emitInit(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstInit>(context->node, AstNodeKind::Init);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    // Number of values to initialize. 0 is dynamic (comes from a register)
    uint64_t numToInit = 0;
    if (!node->count)
        numToInit = 1;
    else if (node->count->flags & AST_VALUE_COMPUTED)
        numToInit = node->count->computedValue->reg.u64;
    else if (!(node->count->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->count->doneFlags |= AST_DONE_CAST1;
    }

    SWAG_CHECK(emitInit(context, typeExpression, node->expression->resultRegisterRC, numToInit, node->count, node->parameters));
    if (context->result != ContextResult::Done)
        return true;

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    if (node->parameters)
    {
        for (auto c : node->parameters->childs)
            freeRegisterRC(context, c);
    }

    return true;
}

bool ByteCodeGenJob::emitInit(ByteCodeGenContext* context, TypeInfoPointer* typeExpression, RegisterList& rExpr, uint64_t numToInit, AstNode* count, AstNode* parameters)
{
    // Determine if we just need to clear the memory
    bool justClear = true;
    if (parameters)
    {
        for (auto child : parameters->childs)
        {
            if (!(child->flags & AST_VALUE_COMPUTED))
            {
                justClear = false;
                break;
            }

            if (child->computedValue->reg.u64)
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
        context->job->waitStructGenerated(typeStruct);
        if (context->result != ContextResult::Done)
            return true;
        if (typeStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            justClear = false;
    }

    if (justClear)
    {
        uint64_t sizeToClear = typeExpression->pointedType->sizeOf;
        if (sizeToClear)
        {
            if (numToInit)
            {
                sizeToClear *= numToInit;
                emitSetZeroAtPointer(context, sizeToClear, rExpr);
            }
            else
            {
                SWAG_ASSERT(count);
                emitInstruction(context, ByteCodeOp::SetZeroAtPointerXRB, rExpr, count->resultRegisterRC)->c.u64 = sizeToClear;
            }
        }
    }
    else if (!parameters || parameters->childs.empty())
    {
        SWAG_ASSERT(typeStruct);
        if (!(typeStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            SWAG_ASSERT(typeStruct->opInit || typeStruct->opUserInitFct);
            if (!generateStruct_opInit(context, typeStruct))
                return false;

            // Constant loop
            uint32_t regCount = 0;
            if (numToInit > 1)
            {
                regCount    = reserveRegisterRC(context);
                auto inst   = emitInstruction(context, ByteCodeOp::SetImmediate64, regCount);
                inst->b.u64 = numToInit;
            }

            ensureCanBeChangedRC(context, rExpr);
            auto startLoop = context->bc->numInstructions;
            emitInstruction(context, ByteCodeOp::PushRAParam, rExpr);
            SWAG_ASSERT(typeStruct->opInit || typeStruct->opUserInitFct);
            emitOpCallUser(context, typeStruct->opUserInitFct, typeStruct->opInit, false);

            // Dynamic loop
            if (numToInit == 0)
            {
                auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
                inst->flags |= BCI_IMM_B;
                inst->b.u64 = typeStruct->sizeOf;
                emitInstruction(context, ByteCodeOp::DecrementRA64, count->resultRegisterRC);
                auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, count->resultRegisterRC);
                instJump->b.s32 = startLoop - context->bc->numInstructions;
            }

            // Constant loop
            else if (numToInit > 1)
            {
                auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
                inst->flags |= BCI_IMM_B;
                inst->b.u64 = typeStruct->sizeOf;
                emitInstruction(context, ByteCodeOp::DecrementRA64, regCount);
                auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, regCount);
                instJump->b.s32 = startLoop - context->bc->numInstructions;
                freeRegisterRC(context, regCount);
            }
        }
    }
    else if (!typeStruct)
    {
        auto child = parameters->childs.front();
        ensureCanBeChangedRC(context, rExpr);
        auto startLoop = context->bc->numInstructions;
        SWAG_CHECK(emitAffectEqual(context, rExpr, child->resultRegisterRC, child->typeInfo, child));
        SWAG_ASSERT(context->result == ContextResult::Done);
        if (numToInit != 1)
        {
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            emitInstruction(context, ByteCodeOp::DecrementRA64, count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }
    }
    else
    {
        RegisterList r1;
        reserveRegisterRC(context, r1, 1);
        ensureCanBeChangedRC(context, rExpr);

        auto startLoop = context->bc->numInstructions;

        for (auto child : parameters->childs)
        {
            auto param     = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            auto typeParam = param->resolvedParameter;
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, rExpr);
            if (typeParam->offset)
                emitInstruction(context, ByteCodeOp::Add64byVB64, r1)->b.u64 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child);
            SWAG_ASSERT(context->result == ContextResult::Done);
        }

        if (numToInit != 1)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointer64, rExpr, 0, rExpr);
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            inst->flags |= BCI_IMM_B;

            emitInstruction(context, ByteCodeOp::DecrementRA64, count->resultRegisterRC);
            auto instJump   = emitInstruction(context, ByteCodeOp::JumpIfNotZero64, count->resultRegisterRC);
            instJump->b.s32 = startLoop - context->bc->numInstructions;
        }

        for (auto child : parameters->childs)
            freeRegisterRC(context, child);
        freeRegisterRC(context, r1);
    }

    return true;
}

bool ByteCodeGenJob::emitDropCopyMove(ByteCodeGenContext* context)
{
    auto node           = CastAst<AstDropCopyMove>(context->node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    auto typeExpression = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->expression->typeInfo), TypeInfoKind::Pointer);

    if (typeExpression->pointedType->kind != TypeInfoKind::Struct)
    {
        freeRegisterRC(context, node->expression);
        freeRegisterRC(context, node->count);
        return true;
    }

    // Number of elements to deal with. If 0, then this is dynamic
    uint64_t numToDo = 0;
    if (!node->count)
        numToDo = 1;
    else if (node->count->flags & AST_VALUE_COMPUTED)
        numToDo = node->count->computedValue->reg.u64;
    else if (!(node->count->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->count, node->count->typeInfo, node->count->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->count->doneFlags |= AST_DONE_CAST1;
    }

    auto typeStruct    = CastTypeInfo<TypeInfoStruct>(typeExpression->pointedType, TypeInfoKind::Struct);
    bool somethingToDo = false;
    switch (node->kind)
    {
    case AstNodeKind::Drop:
        generateStruct_opDrop(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opDrop || typeStruct->opUserDropFct;
        break;
    case AstNodeKind::PostCopy:
        generateStruct_opPostCopy(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opPostCopy || typeStruct->opUserPostCopyFct;
        break;
    case AstNodeKind::PostMove:
        generateStruct_opPostMove(context, typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
        somethingToDo = typeStruct->opPostMove || typeStruct->opUserPostMoveFct;
        break;
    }

    if (somethingToDo)
    {
        ensureCanBeChangedRC(context, node->expression->resultRegisterRC);

        auto startLoop = context->bc->numInstructions;

        uint32_t jumpAfter = 0;
        if (numToDo != 1)
        {
            jumpAfter = context->bc->numInstructions;
            emitInstruction(context, ByteCodeOp::JumpIfZero64, node->count->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::DecrementRA64, node->count->resultRegisterRC);
        }

        emitInstruction(context, ByteCodeOp::PushRAParam, node->expression->resultRegisterRC);

        switch (node->kind)
        {
        case AstNodeKind::Drop:
            emitOpCallUser(context, typeStruct->opUserDropFct, typeStruct->opDrop, false);
            break;
        case AstNodeKind::PostCopy:
            emitOpCallUser(context, typeStruct->opUserPostCopyFct, typeStruct->opPostCopy, false);
            break;
        case AstNodeKind::PostMove:
            emitOpCallUser(context, typeStruct->opUserPostMoveFct, typeStruct->opPostMove, false);
            break;
        }

        if (numToDo != 1)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointer64, node->expression->resultRegisterRC, 0, node->expression->resultRegisterRC);
            inst->b.u64 = typeExpression->pointedType->sizeOf;
            inst->flags |= BCI_IMM_B;
            auto instJump                     = emitInstruction(context, ByteCodeOp::Jump);
            instJump->b.s32                   = startLoop - context->bc->numInstructions;
            context->bc->out[jumpAfter].b.s32 = context->bc->numInstructions - jumpAfter - 1;
        }
    }

    freeRegisterRC(context, node->expression);
    freeRegisterRC(context, node->count);
    return true;
}