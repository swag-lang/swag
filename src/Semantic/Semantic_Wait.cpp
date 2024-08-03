#include "pch.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void Semantic::waitSymbolNoLock(Job* job, SymbolName* symbol)
{
    job->setPending(JobWaitKind::WaitSymbol, symbol, nullptr, nullptr);
    symbol->addDependentJobNoLock(job);
}

void Semantic::waitAllStructInterfacesReg(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
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
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
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
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
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
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
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
        typeInfo = castTypeInfo<TypeInfoArray>(typeInfo)->finalType;
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    if (!typeInfo->isStruct())
        return;
    if (typeInfo->isGeneric())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->hasFlag(TYPEINFO_GHOST_TUPLE))
        return;
    if (typeInfoStruct->declNode->is(AstNodeKind::InterfaceDecl))
        return;

    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    ScopedLock lk(structNode->mutex);
    if (!structNode->hasSemFlag(SEMFLAG_STRUCT_OP_ALLOCATED))
    {
        SWAG_ASSERT(!typeInfoStruct->hasFlag(TYPEINFO_SPEC_OP_GENERATED));
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemByteCodeGenerated3, structNode->resolvedSymbolName(), structNode, nullptr);
    }
}

void Semantic::waitStructGenerated(Job* job, TypeInfo* typeInfo)
{
    if (typeInfo->isArrayOfStruct())
        typeInfo = castTypeInfo<TypeInfoArray>(typeInfo)->finalType;
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = castTypeInfo<TypeInfoPointer>(typeInfo)->pointedType;
    typeInfo = typeInfo->getConcreteAlias();

    if (!typeInfo->isStruct())
        return;
    if (typeInfo->isGeneric())
        return;

    const auto typeInfoStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->hasFlag(TYPEINFO_GHOST_TUPLE))
        return;
    if (typeInfoStruct->declNode->is(AstNodeKind::InterfaceDecl))
        return;

    const auto structNode = castAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);

    ScopedLock lk(structNode->mutex);
    if (!structNode->hasSemFlag(SEMFLAG_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemByteCodeGenerated4, structNode->resolvedSymbolName(), structNode, nullptr);
    }
}

void Semantic::waitForOverloads(Job* job, SymbolName* symbol)
{
    {
        SharedLock lk(symbol->mutex);
        if (!symbol->cptOverloads)
            return;
    }

    {
        ScopedLock lk(symbol->mutex);
        if (!symbol->cptOverloads)
            return;
        waitSymbolNoLock(job, symbol);
    }
}

void Semantic::waitOverloadCompleted(Job* job, const SymbolOverload* overload)
{
    if (!overload)
        return;

    {
        // Note sure that we can have a problematic race condition here.
        // And this lock creates contention. So remove it from now (26/02/2024).

        // SharedLock lk(overload->symbol->mutex);
        if (!overload->hasFlag(OVERLOAD_INCOMPLETE))
            return;
    }

    {
        ScopedLock lk(overload->symbol->mutex);
        if (overload->hasFlag(OVERLOAD_INCOMPLETE))
        {
            waitSymbolNoLock(job, overload->symbol);
            return;
        }
    }
}

void Semantic::waitFuncDeclFullResolve(Job* job, AstFuncDecl* funcDecl)
{
    ScopedLock lk(funcDecl->funcMutex);
    if (!funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_FULL_RESOLVE))
    {
        funcDecl->dependentJobs.add(job);
        job->setPending(JobWaitKind::SemFullResolve, nullptr, funcDecl, nullptr);
    }
}

void Semantic::waitStructOverloadDefined(Job* job, const TypeInfo* typeInfo)
{
    if (!typeInfo->isStruct() && !typeInfo->isInterface())
        return;

    //  :BecauseOfThat
    const auto structNode = castAst<AstStruct>(typeInfo->declNode, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
    if (structNode->hasSpecFlag(AstStruct::SPEC_FLAG_NO_OVERLOAD))
        return;

    ScopedLock lk(structNode->mutex);
    if (!structNode->resolvedSymbolOverload())
    {
        structNode->dependentJobs.add(job);
        job->setPending(JobWaitKind::WaitStructSymbol, structNode->resolvedSymbolName(), structNode, nullptr);
        return;
    }
}

void Semantic::waitTypeCompleted(Job* job, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return;
    auto orgTypeInfo = typeInfo;
    typeInfo         = TypeManager::concreteType(typeInfo);
    if (typeInfo->isSlice())
        typeInfo = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice)->pointedType;
    if (typeInfo->isArray())
        typeInfo = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array)->finalType;
    typeInfo = TypeManager::concreteType(typeInfo);
    if (!typeInfo->isStruct() && !typeInfo->isInterface())
        return;
    if (!typeInfo->declNode)
        return;
    if (typeInfo->hasFlag(TYPEINFO_GHOST_TUPLE))
        return;

    waitStructOverloadDefined(job, typeInfo);
    if (job->baseContext->result == ContextResult::Pending)
        return;

    waitOverloadCompleted(job, typeInfo->declNode->resolvedSymbolOverload());
    if (job->baseContext->result == ContextResult::Pending)
        return;

    // Be sure type sizeof is correct, because it could have been created before the
    // raw type has been completed because of //

    orgTypeInfo = TypeManager::concreteType(orgTypeInfo);
    if (orgTypeInfo->isArray())
        orgTypeInfo->sizeOf = castTypeInfo<TypeInfoArray>(orgTypeInfo)->finalType->sizeOf * castTypeInfo<TypeInfoArray>(orgTypeInfo)->totalCount;
    if (orgTypeInfo->isConstAlias())
        orgTypeInfo->sizeOf = orgTypeInfo->getConstAlias()->sizeOf;
    if (typeInfo->isConstAlias())
        typeInfo->sizeOf = typeInfo->getConstAlias()->sizeOf;
}

void Semantic::waitForGenericParameters(const SemanticContext* context, OneMatch& match)
{
    for (const auto& val : match.genericReplaceTypes | std::views::values)
    {
        const auto typeInfo = val.typeInfoReplace;
        const auto declNode = typeInfo->declNode;

        if (!declNode)
            continue;

        waitStructOverloadDefined(context->baseJob, typeInfo);
        if (context->result == ContextResult::Pending)
            return;

        const auto overload = declNode->resolvedSymbolOverload();
        if (!overload)
            continue;
        if (overload->symbol == match.symbolName)
            continue;

        if (typeInfo->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            if (typeStruct->fromGeneric)
                continue;

            // If a field in a struct has generic parameters with the struct itself, we authorize to wait
            if (context->node->hasAstFlag(AST_STRUCT_MEMBER) && typeInfo == context->node->ownerStructScope->owner->typeInfo)
                continue;
        }

        waitOverloadCompleted(context->baseJob, overload);
        if (context->result == ContextResult::Pending)
            return;
    }
}

bool Semantic::waitForStructUserOps(SemanticContext* context, const AstNode* node)
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

bool Semantic::needToCompleteSymbolNoLock(SemanticContext*, const AstIdentifier* identifier, SymbolName* symbol, bool testOverloads)
{
    if (symbol->isNot(SymbolKind::Struct) && symbol->isNot(SymbolKind::Interface))
        return true;
    if (identifier->callParameters || identifier->genericParameters)
        return true;

    if (testOverloads)
    {
        if (symbol->overloads.size() != 1)
            return true;
        // If this is a generic type, and it's from an instance, we must wait, because we will
        // have to instantiate that symbol too
        if (identifier->ownerStructScope && identifier->ownerStructScope->owner->hasAstFlag(AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
            return true;
        if (identifier->ownerFct && identifier->ownerFct->hasAstFlag(AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
            return true;
    }

    // If a structure is referencing itself, we will match the incomplete symbol for now
    if (identifier->hasAstFlag(AST_STRUCT_MEMBER))
        return false;

    // We can also do an incomplete match with the identifier of an Impl block
    if (identifier->hasAstFlag(AST_CAN_MATCH_INCOMPLETE))
        return false;

    // If identifier is in a pointer type expression, can incomplete resolve
    if (const auto pr2 = identifier->getParent(2); pr2->is(AstNodeKind::TypeExpression))
    {
        auto typeExprNode = castAst<AstTypeExpression>(pr2, AstNodeKind::TypeExpression);
        if (typeExprNode->typeFlags.has(TYPEFLAG_IS_PTR))
            return false;

        if (const auto pr1 = typeExprNode->getParent(); pr1->is(AstNodeKind::TypeExpression))
        {
            typeExprNode = castAst<AstTypeExpression>(pr1, AstNodeKind::TypeExpression);
            if (typeExprNode->typeFlags.has(TYPEFLAG_IS_PTR))
                return false;
        }
    }

    return true;
}

bool Semantic::needToWaitForSymbolNoLock(SemanticContext* /*context*/, const AstIdentifier* identifier, const SymbolName* symbol)
{
    if (!symbol->cptOverloads && !symbol->hasFlag(SYMBOL_ATTRIBUTE_GEN))
        return false;

    // For a name alias, we wait everything to be done...
    if (identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS))
        return true;

    // This is enough to resolve, as we just need parameters, and that case means that some functions
    // do not know their return type yet (short lambdas)
    if (symbol->is(SymbolKind::Function) && symbol->overloads.size() == symbol->cptOverloadsInit)
        return false;

    return true;
}
