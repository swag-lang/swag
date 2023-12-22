#include "pch.h"
#include "Job.h"
#include "Diagnostic.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Report.h"

void Job::addDependentJob(Job* job)
{
    ScopedLock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitSymbolNoLock(SymbolName* symbol)
{
    setPending(JobWaitKind::WaitSymbol, symbol, nullptr, nullptr);
    symbol->addDependentJobNoLock(this);
}

void Job::waitAllStructInterfacesReg(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (module->waitImplForToSolve(this, typeInfoStruct))
    {
        setPending(JobWaitKind::WaitInterfacesFor, nullptr, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfacesReg == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(JobWaitKind::WaitInterfacesReg, nullptr, nullptr, typeInfoStruct);
}

void Job::waitAllStructInterfaces(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (module->waitImplForToSolve(this, typeInfoStruct))
    {
        setPending(JobWaitKind::WaitInterfacesFor, nullptr, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfaces == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(JobWaitKind::WaitInterfaces, nullptr, nullptr, typeInfoStruct);
}

void Job::waitAllStructSpecialMethods(TypeInfo* typeInfo)
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
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(JobWaitKind::WaitSpecialMethods, nullptr, nullptr, typeInfoStruct);
}

void Job::waitAllStructMethods(TypeInfo* typeInfo)
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
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(JobWaitKind::WaitMethods, nullptr, nullptr, typeInfoStruct);
}

void Job::waitStructGeneratedAlloc(TypeInfo* typeInfo)
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
        structNode->dependentJobs.add(this);
        setPending(JobWaitKind::SemByteCodeGenerated, structNode->resolvedSymbolName, structNode, nullptr);
    }
}

void Job::waitStructGenerated(TypeInfo* typeInfo)
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
        structNode->dependentJobs.add(this);
        setPending(JobWaitKind::SemByteCodeGenerated, structNode->resolvedSymbolName, structNode, nullptr);
    }
}

void Job::waitOverloadCompleted(SymbolOverload* overload)
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
            waitSymbolNoLock(overload->symbol);
            return;
        }
    }
}

void Job::waitFuncDeclFullResolve(AstFuncDecl* funcDecl)
{
    ScopedLock lk(funcDecl->funcMutex);
    if (!(funcDecl->specFlags & AstFuncDecl::SPECFLAG_FULL_RESOLVE))
    {
        funcDecl->dependentJobs.add(this);
        setPending(JobWaitKind::SemFullResolve, funcDecl->resolvedSymbolName, funcDecl, nullptr);
    }
}

void Job::waitTypeCompleted(TypeInfo* typeInfo)
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
        structNode->dependentJobs.add(this);
        setPending(JobWaitKind::WaitStructSymbol, structNode->resolvedSymbolName, structNode, nullptr);
        return;
    }

    waitOverloadCompleted(typeInfo->declNode->resolvedSymbolOverload);
    if (baseContext->result == ContextResult::Pending)
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

void Job::setPendingInfos(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo)
{
    waitingSymbolSolved = symbolToWait;
    waitingKind         = waitKind;
    waitingNode         = node;
    waitingHintNode     = nullptr;
    waitingType         = typeInfo;
}

void Job::setPending(JobWaitKind waitKind, SymbolName* symbolToWait, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(baseContext);
    if (baseContext->result == ContextResult::Pending)
        return;
    setPendingInfos(waitKind, symbolToWait, node, typeInfo);
    baseContext->result = ContextResult::Pending;
}