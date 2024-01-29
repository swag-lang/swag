#include "pch.h"
#include "Semantic.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Scope.h"
#include "Ast_Flags.h"

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
    if (orgTypeInfo->isConstAlias())
        orgTypeInfo->sizeOf = orgTypeInfo->getConstAlias()->sizeOf;
    if (typeInfo->isConstAlias())
        typeInfo->sizeOf = typeInfo->getConstAlias()->sizeOf;
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

bool Semantic::needToCompleteSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol, bool testOverloads)
{
    if (symbol->kind != SymbolKind::Struct && symbol->kind != SymbolKind::Interface)
        return true;
    if (identifier->callParameters || identifier->genericParameters)
        return true;

    if (testOverloads)
    {
        if (symbol->overloads.size() != 1)
            return true;
        // If this is a generic type, and it's from an instance, we must wait, because we will
        // have to instantiate that symbol too
        if (identifier->ownerStructScope && (identifier->ownerStructScope->owner->flags & AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
            return true;
        if (identifier->ownerFct && (identifier->ownerFct->flags & AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
            return true;
    }

    // If a structure is referencing itself, we will match the incomplete symbol for now
    if (identifier->flags & AST_STRUCT_MEMBER)
        return false;

    // We can also do an incomplete match with the identifier of an Impl block
    if (identifier->flags & AST_CAN_MATCH_INCOMPLETE)
        return false;

    // If identifier is in a pointer type expression, can incomplete resolve
    if (identifier->parent->parent && identifier->parent->parent->kind == AstNodeKind::TypeExpression)
    {
        auto typeExprNode = CastAst<AstTypeExpression>(identifier->parent->parent, AstNodeKind::TypeExpression);
        if (typeExprNode->typeFlags & TYPEFLAG_IS_PTR)
            return false;

        if (typeExprNode->parent && typeExprNode->parent->kind == AstNodeKind::TypeExpression)
        {
            typeExprNode = CastAst<AstTypeExpression>(typeExprNode->parent, AstNodeKind::TypeExpression);
            if (typeExprNode->typeFlags & TYPEFLAG_IS_PTR)
                return false;
        }
    }

    return true;
}

bool Semantic::needToWaitForSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol)
{
    if (!symbol->cptOverloads && !(symbol->flags & SYMBOL_ATTRIBUTE_GEN))
        return false;

    // For a name alias, we wait everything to be done...
    if (identifier->specFlags & AstIdentifier::SPECFLAG_NAME_ALIAS)
        return true;

    // This is enough to resolve, as we just need parameters, and that case means that some functions
    // do not know their return type yet (short lambdas)
    if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
        return false;

    return true;
}
