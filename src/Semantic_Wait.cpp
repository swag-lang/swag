#include "pch.h"
#include "Semantic.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "LanguageSpec.h"

void Semantic::waitSymbolNoLock(Job* job, SymbolName* symbol)
{
    job->setPending(JobWaitKind::WaitSymbol, symbol, nullptr, nullptr);
    symbol->addDependentJobNoLock(job);
}

void Semantic::waitAllStructInterfacesReg(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (job->module->waitImplForToSolve(job, typeInfoStruct))
    {
        job->setPending(JobWaitKind::WaitInterfacesFor, nullptr, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfacesReg == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(job);
    job->setPending(JobWaitKind::WaitInterfacesReg, nullptr, nullptr, typeInfoStruct);
}

void Semantic::waitAllStructInterfaces(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (job->module->waitImplForToSolve(job, typeInfoStruct))
    {
        job->setPending(JobWaitKind::WaitInterfacesFor, nullptr, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfaces == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(job);
    job->setPending(JobWaitKind::WaitInterfaces, nullptr, nullptr, typeInfoStruct);
}

void Semantic::waitAllStructSpecialMethods(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingSpecialMethods == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(job);
    job->setPending(JobWaitKind::WaitSpecialMethods, nullptr, nullptr, typeInfoStruct);
}

void Semantic::waitAllStructMethods(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingMethods == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(job);
    job->setPending(JobWaitKind::WaitMethods, nullptr, nullptr, typeInfoStruct);
}

void Semantic::waitStructGeneratedAlloc(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isArrayOfStruct())
        typeInfo = ((TypeInfoArray*) typeInfo)->finalType;
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;
    if (typeInfo->isGeneric())
        return;

    auto typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->flags & TYPEINFO_GHOST_TUPLE)
        return;
    if (typeInfoStruct->declNode->kind == AstNodeKind::InterfaceDecl)
        return;

    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    ScopedLock lk(structNode->mutex);
    if (!(structNode->semFlags & SEMFLAG_STRUCT_OP_ALLOCATED))
    {
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemByteCodeGenerated, structNode->resolvedSymbolName, structNode, nullptr);
    }
}

void Semantic::waitStructGenerated(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isArrayOfStruct())
        typeInfo = ((TypeInfoArray*) typeInfo)->finalType;
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    typeInfo = typeInfo->getConcreteAlias();

    if (!typeInfo->isStruct())
        return;
    if (typeInfo->isGeneric())
        return;

    auto typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->flags & TYPEINFO_GHOST_TUPLE)
        return;
    if (typeInfoStruct->declNode->kind == AstNodeKind::InterfaceDecl)
        return;

    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    ScopedLock lk(structNode->mutex);
    if (!(structNode->semFlags & SEMFLAG_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemByteCodeGenerated, structNode->resolvedSymbolName, structNode, nullptr);
    }
}

void Semantic::waitOverloadCompleted(Job* job, SymbolOverload* overload)
{
    SWAG_ASSERT(overload);

    {
        SharedLock lk(overload->symbol->mutex);
        if (!(overload->flags & OVERLOAD_INCOMPLETE))
            return;
    }

    {
        ScopedLock lk(overload->symbol->mutex);
        if (overload->flags & OVERLOAD_INCOMPLETE)
        {
            waitSymbolNoLock(job, overload->symbol);
            return;
        }
    }
}

void Semantic::waitFuncDeclFullResolve(Job* job, AstFuncDecl* funcDecl)
{
    ScopedLock lk(funcDecl->funcMutex);
    if (!(funcDecl->specFlags & AstFuncDecl::SPECFLAG_FULL_RESOLVE))
    {
        funcDecl->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemFullResolve, funcDecl->resolvedSymbolName, funcDecl, nullptr);
    }
}

void Semantic::waitTypeCompleted(Job* job, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return;
    auto orgTypeInfo = typeInfo;
    typeInfo         = TypeManager::concreteType(typeInfo);
    if (typeInfo->isSlice())
        typeInfo = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice)->pointedType;
    if (typeInfo->isArray())
        typeInfo = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array)->finalType;
    typeInfo = TypeManager::concreteType(typeInfo);
    if (!typeInfo->isStruct() && !typeInfo->isInterface())
        return;
    if (!typeInfo->declNode)
        return;
    if (typeInfo->flags & TYPEINFO_GHOST_TUPLE)
        return;

    //  :BecauseOfThat
    auto       structNode = CastAst<AstStruct>(typeInfo->declNode, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
    ScopedLock lk(structNode->mutex);
    if (!structNode->resolvedSymbolOverload)
    {
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::WaitStructSymbol, structNode->resolvedSymbolName, structNode, nullptr);
        return;
    }

    waitOverloadCompleted(job, typeInfo->declNode->resolvedSymbolOverload);
    if (job->baseContext->result == ContextResult::Pending)
        return;

    // Be sure type sizeof is correct, because it could have been created before the
    // raw type has been completed because of //

    orgTypeInfo = TypeManager::concreteType(orgTypeInfo);
    if (orgTypeInfo->isArray())
        orgTypeInfo->sizeOf = ((TypeInfoArray*) orgTypeInfo)->finalType->sizeOf * ((TypeInfoArray*) orgTypeInfo)->totalCount;
    if (orgTypeInfo->isFakeAlias())
        orgTypeInfo->sizeOf = orgTypeInfo->getFakeAlias()->sizeOf;
    if (typeInfo->isFakeAlias())
        typeInfo->sizeOf = typeInfo->getFakeAlias()->sizeOf;
}

bool Semantic::waitForStructUserOps(SemanticContext* context, AstNode* node)
{
    SymbolName* symbol = nullptr;
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opPostCopy, node, &symbol));
    YIELD();
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opPostMove, node, &symbol));
    YIELD();
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opDrop, node, &symbol));
    YIELD();
    return true;
}

void Semantic::waitForGenericParameters(SemanticContext* context, OneGenericMatch& match)
{
    for (auto it : match.genericReplaceTypes)
    {
        auto typeInfo = it.second.typeInfoReplace;
        auto declNode = typeInfo->declNode;
        if (!declNode)
            continue;
        if (!declNode->resolvedSymbolOverload)
            continue;
        if (declNode->resolvedSymbolOverload->symbol == match.symbolName)
            continue;
        if (typeInfo->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            if (typeStruct->fromGeneric)
                continue;

            // If a field in a struct has generic parameters with the struct itself, we authorize to wait
            if (context->node->flags & AST_STRUCT_MEMBER && typeInfo == context->node->ownerStructScope->owner->typeInfo)
                continue;
        }

        Semantic::waitOverloadCompleted(context->baseJob, declNode->resolvedSymbolOverload);
        if (context->result == ContextResult::Pending)
            return;

        SWAG_ASSERT(typeInfo->sizeOf > 0);
    }
}