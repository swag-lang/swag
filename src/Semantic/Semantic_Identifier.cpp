#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool Semantic::resolveNameAlias(SemanticContext* context)
{
    const auto node = castAst<AstAlias>(context->node, AstNodeKind::NameAlias);
    auto       back = node->lastChild();

    SWAG_ASSERT(node->resolvedSymbolName());
    SWAG_ASSERT(back->resolvedSymbolName());

    SWAG_CHECK(collectAttributes(context, node, nullptr));
    node->addAstFlag(AST_NO_BYTECODE);

    // Constraints with alias on a variable
    if (back->resolvedSymbolName()->is(SymbolKind::Variable))
    {
        // alias x = struct.x is not possible
        if (back->is(AstNodeKind::IdentifierRef))
        {
            int cptVar = 0;
            for (const auto& c : back->children)
            {
                const auto symbolName = c->resolvedSymbolName();
                if (symbolName && symbolName->is(SymbolKind::Variable))
                {
                    SWAG_VERIFY(cptVar == 0, context->report({back, toErr(Err0163)}));
                    cptVar++;
                }
            }
        }
    }

    const auto symbolName = back->resolvedSymbolName();
    if (symbolName->isNot(SymbolKind::Namespace) &&
        symbolName->isNot(SymbolKind::Function) &&
        symbolName->isNot(SymbolKind::Variable))
    {
        Diagnostic err{back, formErr(Err0328, Naming::aKindName(symbolName->kind).c_str())};

        err.addNote(toNte(Nte0013));

        if (symbolName->is(SymbolKind::Enum) ||
            symbolName->is(SymbolKind::Interface) ||
            symbolName->is(SymbolKind::TypeAlias) ||
            symbolName->is(SymbolKind::Struct))
        {
            err.addNote(node, node->kwdLoc, formNte(Nte0025, Naming::aKindName(symbolName->kind).c_str()));
        }

        return context->report(err);
    }

    SWAG_CHECK(node->ownerScope->symTable.registerNameAlias(context, node, node->resolvedSymbolName(), back->resolvedSymbolName(), back->resolvedSymbolOverload()));
    if (node->hasAttribute(ATTRIBUTE_PUBLIC))
        node->ownerScope->addPublicNode(node);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::preResolveIdentifierRef(SemanticContext* context)
{
    const auto node = castAst<AstIdentifierRef>(context->node, AstNodeKind::IdentifierRef);

    // When duplicating an identifier ref, and solve it again, we need to be sure that all that
    // stuff is reset
    if (!node->hasSemFlag(SEMFLAG_TYPE_SOLVED))
    {
        node->typeInfo             = nullptr;
        node->previousResolvedNode = nullptr;
        node->startScope           = nullptr;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveIdentifierRef(SemanticContext* context)
{
    const auto node      = castAst<AstIdentifierRef>(context->node);
    const auto childBack = node->lastChild();

    // Keep resolution
    if (!node->typeInfo || !node->hasSemFlag(SEMFLAG_TYPE_SOLVED))
    {
        node->setResolvedSymbol(childBack->resolvedSymbolName(), childBack->resolvedSymbolOverload());
        node->typeInfo = childBack->typeInfo;
    }

    node->token.text  = childBack->token.text;
    node->byteCodeFct = ByteCodeGen::emitIdentifierRef;

    // Flag inheritance
    node->addAstFlag(AST_CONST_EXPR | AST_FROM_GENERIC_REPLACE);
    for (const auto child : node->children)
    {
        if (!child->hasAstFlag(AST_CONST_EXPR))
            node->removeAstFlag(AST_CONST_EXPR);
        if (!child->hasAstFlag(AST_FROM_GENERIC_REPLACE))
            node->removeAstFlag(AST_FROM_GENERIC_REPLACE);
        if (child->hasAstFlag(AST_GENERIC))
            node->addAstFlag(AST_GENERIC);
        if (child->hasAstFlag(AST_CONST))
            node->addAstFlag(AST_CONST);
    }

    if (childBack->hasFlagComputedValue())
        node->inheritComputedValue(childBack);
    node->inheritAstFlagsOr(childBack, AST_L_VALUE | AST_R_VALUE | AST_TRANSIENT | AST_VALUE_GEN_TYPEINFO | AST_SIDE_EFFECTS);

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload() &&
        node->resolvedSymbolOverload()->hasFlag(OVERLOAD_COMPUTED_VALUE))
    {
        if (!node->hasFlagComputedValue())
        {
            node->setFlagsValueIsComputed();
            *node->computedValue() = node->resolvedSymbolOverload()->computedValue;
        }

        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);

        // If literal is stored in a data segment, then it's still a left value (we can take the address for example)
        if (!node->computedValue()->storageSegment || node->computedValue()->storageOffset == 0xFFFFFFFF)
            node->removeAstFlag(AST_L_VALUE);
    }

    return true;
}

bool Semantic::setupIdentifierRef(SemanticContext*, AstNode* node)
{
    // If type of previous one was const, then we force this node to be const (cannot change it)
    if (node->parent->typeInfo && node->parent->typeInfo->isConst())
        node->addAstFlag(AST_CONST);
    const auto overload = node->resolvedSymbolOverload();
    if (overload && overload->hasFlag(OVERLOAD_CONST_ASSIGN))
        node->addSemFlag(SEMFLAG_CONST_ASSIGN);

    if (node->parent->isNot(AstNodeKind::IdentifierRef))
        return true;

    const auto identifierRef = castAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // If we cannot assign previous, and this was AST_IS_CONST_ASSIGN_INHERIT, then we cannot assign
    // this one either
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->hasSemFlag(SEMFLAG_CONST_ASSIGN_INHERIT))
    {
        node->addSemFlag(SEMFLAG_CONST_ASSIGN);
        node->addSemFlag(SEMFLAG_CONST_ASSIGN_INHERIT);
    }

    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = nullptr;

    const auto typeInfo  = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
    auto       scopeType = typeInfo->getConcreteAlias();
    if (scopeType->isLambdaClosure())
    {
        const auto funcType = castTypeInfo<TypeInfoFuncAttr>(scopeType, TypeInfoKind::LambdaClosure);
        scopeType           = TypeManager::concreteType(funcType->returnType, CONCRETE_FORCE_ALIAS);
    }

    if (!identifierRef->hasSemFlag(SEMFLAG_TYPE_SOLVED))
        identifierRef->typeInfo = typeInfo;

    // Deref
    if (node->typeInfo->isFuncAttr() || node->typeInfo->isLambdaClosure())
    {
        if (scopeType->isPointerRef() && node != identifierRef->lastChild())
        {
            setUnRef(node);
            scopeType = TypeManager::concretePtrRef(scopeType);
        }
    }

    switch (scopeType->kind)
    {
        case TypeInfoKind::Enum:
        {
            identifierRef->startScope = castTypeInfo<TypeInfoEnum>(scopeType, TypeInfoKind::Enum)->scope;
            node->typeInfo            = typeInfo;
            break;
        }
        case TypeInfoKind::Pointer:
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(scopeType, TypeInfoKind::Pointer);
            const auto subType     = TypeManager::concreteType(typePointer->pointedType);
            if (subType->isStruct() || subType->isInterface())
                identifierRef->startScope = castTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
            node->typeInfo = typeInfo;
            break;
        }

        case TypeInfoKind::TypeListArray:
        case TypeInfoKind::TypeListTuple:
            identifierRef->startScope = castTypeInfo<TypeInfoList>(scopeType, scopeType->kind)->scope;
            node->typeInfo            = typeInfo;
            break;

        case TypeInfoKind::Interface:
        case TypeInfoKind::Struct:
            identifierRef->startScope = castTypeInfo<TypeInfoStruct>(scopeType, scopeType->kind)->scope;
            node->typeInfo            = typeInfo;
            break;

        case TypeInfoKind::Array:
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(scopeType, TypeInfoKind::Array);
            const auto subType   = TypeManager::concreteType(typeArray->finalType);
            if (subType->isStruct())
                identifierRef->startScope = castTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
            node->typeInfo = typeInfo;
            break;
        }

        case TypeInfoKind::Slice:
        {
            const auto typeSlice = castTypeInfo<TypeInfoSlice>(scopeType, TypeInfoKind::Slice);
            const auto subType   = TypeManager::concreteType(typeSlice->pointedType);
            if (subType->isStruct())
                identifierRef->startScope = castTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
            node->typeInfo = typeInfo;
            break;
        }
    }

    return true;
}

bool Semantic::isFunctionButNotACall(SemanticContext*, AstNode* node, const SymbolName* symbol)
{
    const AstIdentifier* id = nullptr;
    if (node && node->is(AstNodeKind::Identifier))
    {
        id = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (id != id->identifierRef()->lastChild())
            return false;
    }

    if (node && node->parent && node->parent->parent)
    {
        const auto grandParent = node->parent->parent;
        if (symbol->is(SymbolKind::Attribute))
        {
            if (grandParent->isNot(AstNodeKind::AttrUse))
                return true;
        }
        else if (symbol->is(SymbolKind::Function))
        {
            if (symbol->is(SymbolKind::Function))
            {
                if (grandParent->is(AstNodeKind::MakePointer) && node == node->parent->lastChild())
                {
                    if (id && id->callParameters)
                        return false;
                    return true;
                }

                if (grandParent->is(AstNodeKind::MakePointerLambda) && node == node->parent->lastChild())
                    return true;
            }

            if (grandParent->hasAstFlag(AST_DEBUG_NODE) ||
                grandParent->is(AstNodeKind::TypeAlias) ||
                grandParent->is(AstNodeKind::NameAlias) ||
                grandParent->is(AstNodeKind::IntrinsicDefined) ||
                grandParent->is(AstNodeKind::IntrinsicLocation) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicStringOf)) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicNameOf)) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicRunes)) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicTypeOf)) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicDeclType)) ||
                (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicKindOf)))
            {
                return true;
            }
        }
    }

    return false;
}

TypeInfoEnum* Semantic::findEnumTypeInContext(SemanticContext*, TypeInfo* typeInfo)
{
    while (true)
    {
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FUNC | CONCRETE_FORCE_ALIAS);
        if (!typeInfo)
            return nullptr;

        if (typeInfo->isArray())
        {
            const auto typeArr = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo           = typeArr->finalType;
            continue;
        }

        if (typeInfo->isSlice())
        {
            const auto typeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
            typeInfo             = typeSlice->pointedType;
            continue;
        }

        if (typeInfo->isPointer())
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            typeInfo               = typePointer->pointedType;
            continue;
        }

        if (typeInfo->isEnum())
        {
            return castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        }

        return nullptr;
    }
}

bool Semantic::findEnumTypeInContext(SemanticContext*                                  context,
                                     const AstNode*                                    node,
                                     VectorNative<TypeInfoEnum*>&                      result,
                                     VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has,
                                     VectorNative<SymbolOverload*>&                    testedOver)
{
    result.clear();
    has.clear();

    const auto findParent = node->findParent(AstNodeKind::Return, AstNodeKind::FuncCallParam);

    // If this is a parameter of a function call, we will try to deduce the type with a function signature
    if (findParent &&
        findParent->is(AstNodeKind::FuncCallParam) &&
        findParent->parent->is(AstNodeKind::FuncCallParams) &&
        findParent->parent->parent->is(AstNodeKind::Identifier))
    {
        const auto fctCallParam = castAst<AstFuncCallParam>(findParent);

        VectorNative<OneSymbolMatch> symbolMatch;

        const auto idref = castAst<AstIdentifierRef>(fctCallParam->parent->parent->parent, AstNodeKind::IdentifierRef);
        const auto id    = castAst<AstIdentifier>(fctCallParam->parent->parent, AstNodeKind::Identifier);

        g_SilentError++;
        const auto found = findIdentifierInScopes(context, symbolMatch, idref, id);
        g_SilentError--;
        YIELD();

        if (!found || symbolMatch.empty())
            return true;

        // Be sure symbols have been solved, because we need the types to be deduced
        for (const auto& sm : symbolMatch)
        {
            const auto symbol = sm.symbol;
            if (symbol->isNot(SymbolKind::Function) && symbol->isNot(SymbolKind::Variable) && symbol->isNot(SymbolKind::Attribute))
                continue;

            ScopedLock ls(symbol->mutex);
            if (symbol->cptOverloads)
            {
                waitSymbolNoLock(context->baseJob, symbol);
                return true;
            }
        }

        for (const auto& sm : symbolMatch)
        {
            const auto symbol = sm.symbol;
            if (symbol->isNot(SymbolKind::Function) && symbol->isNot(SymbolKind::Variable) && symbol->isNot(SymbolKind::Attribute))
                continue;

            SharedLock lk(symbol->mutex);
            for (auto& overload : symbol->overloads)
            {
                const auto concrete = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCE_ALIAS);
                if (!concrete->isFuncAttr() && !concrete->isLambdaClosure())
                    continue;
                if (concrete->isGeneric() || concrete->isFromGeneric())
                    continue;

                testedOver.push_back(overload);
                const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(concrete, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

                // If there's only one corresponding type in the function, then take it
                // If it's not the correct parameter, the match will not be done, so we do not really care here
                VectorNative<TypeInfoEnum*> subResult;
                for (const auto param : typeFunc->parameters)
                {
                    auto typeEnum = findEnumTypeInContext(context, param->typeInfo);
                    if (!typeEnum)
                        continue;
                    has.push_back_once({param->declNode, typeEnum});
                    if (typeEnum->contains(node->token.text))
                        subResult.push_back_once(typeEnum);
                }

                if (subResult.size() == 1)
                {
                    result.push_back_once(subResult.front());
                }

                // More that one possible type (at least two different enums with the same identical requested name in the function signature)
                // We are not lucky...
                else if (subResult.size() > 1)
                {
                    int enumIdx = 0;
                    for (uint32_t i = 0; i < fctCallParam->parent->childCount(); i++)
                    {
                        const auto child = fctCallParam->parent->children[i];
                        if (child == fctCallParam)
                            break;
                        if (child->typeInfo && child->typeInfo->getConcreteAlias()->isEnum())
                            enumIdx++;
                        else if (child->is(AstNodeKind::IdentifierRef) && child->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE))
                            enumIdx++;
                    }

                    for (const auto p : typeFunc->parameters)
                    {
                        const auto concreteP = TypeManager::concreteType(p->typeInfo, CONCRETE_FORCE_ALIAS);
                        if (concreteP->isEnum())
                        {
                            if (!enumIdx)
                            {
                                auto typeEnum = findEnumTypeInContext(context, concreteP);
                                if (typeEnum)
                                    has.push_back_once({p->declNode, typeEnum});
                                if (typeEnum && typeEnum->contains(node->token.text))
                                    result.push_back_once(typeEnum);
                                break;
                            }

                            enumIdx--;
                        }
                    }
                }
            }
        }
    }
    else if (findParent && findParent->is(AstNodeKind::Return))
    {
        const auto fctReturn = castAst<AstReturn>(findParent);
        const auto funcNode  = getFunctionForReturn(fctReturn);
        if (funcNode->returnType)
        {
            const auto typeInfo = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FUNC | CONCRETE_FORCE_ALIAS);
            if (typeInfo->isEnum())
            {
                auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
                has.push_back_once({nullptr, typeEnum});
                if (typeEnum->contains(node->token.text))
                {
                    result.push_back(typeEnum);
                    return true;
                }
            }
        }
    }

    if (!result.empty())
        return true;

    // We go up in the hierarchy until we find a statement, or a contextual type to return
    bool canScanChilds = true;
    auto parent        = node->parent;
    while (parent)
    {
        if (canScanChilds && (parent->is(AstNodeKind::BinaryOp) ||
                              parent->is(AstNodeKind::FactorOp) ||
                              parent->is(AstNodeKind::AffectOp) ||
                              parent->is(AstNodeKind::VarDecl) ||
                              parent->is(AstNodeKind::ConstDecl) ||
                              parent->is(AstNodeKind::FuncDeclParam) ||
                              parent->is(AstNodeKind::Switch)))
        {
            if (parent->isNot(AstNodeKind::FactorOp))
                canScanChilds = false;

            SharedLock lk(parent->mutex);
            for (auto c : parent->children)
            {
                auto typeInfoChild = c->typeInfo;

                // Take first child of an expression list
                if (c->is(AstNodeKind::ExpressionList))
                {
                    const auto expr = castAst<AstExpressionList>(c, AstNodeKind::ExpressionList);
                    typeInfoChild   = expr->firstChild()->typeInfo;
                }

                auto typeEnum = findEnumTypeInContext(context, typeInfoChild);
                if (typeEnum)
                {
                    has.push_back_once({c, typeEnum});
                    if (typeEnum->contains(node->token.text))
                        result.push_back_once(typeEnum);
                }
            }
        }
        else
        {
            SharedLock lk(parent->mutex);
            auto       typeEnum = findEnumTypeInContext(context, parent->typeInfo);
            if (typeEnum)
            {
                has.push_back_once({parent, typeEnum});
                if (typeEnum->contains(node->token.text))
                    result.push_back_once(typeEnum);
            }
        }

        if (!result.empty())
            return true;

        if (parent->is(AstNodeKind::Statement) ||
            parent->is(AstNodeKind::SwitchCaseBlock) ||
            parent->is(AstNodeKind::FuncDecl) ||
            parent->is(AstNodeKind::File) ||
            parent->is(AstNodeKind::Module) ||
            parent->is(AstNodeKind::FuncDeclParam) ||
            parent->is(AstNodeKind::VarDecl) ||
            parent->is(AstNodeKind::ConstDecl))
            break;

        parent = parent->parent;
    }

    return true;
}

bool Semantic::getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, const AstIdentifier* /*node*/, const SymbolOverload* overload, AstNode** result, AstNode** resultLeaf)
{
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;

    if (scopeHierarchyVars.empty())
        return true;

    // Not for a global symbol
    if (overload->hasFlag(OVERLOAD_VAR_GLOBAL))
        return true;

    const auto kind = overload->symbol->kind;
    switch (kind)
    {
        case SymbolKind::Enum:
        case SymbolKind::Struct:
        case SymbolKind::TypeAlias:
        case SymbolKind::Interface:
            return true;
    }

    const auto                symbol   = overload->symbol;
    const auto                symScope = symbol->ownerTable->scope;
    Vector<CollectedScopeVar> toCheck;

    // Collect all matches
    bool hasWith = false;
    bool hasUFCS = false;
    for (auto& dep : scopeHierarchyVars)
    {
        bool getIt = false;

        // This is a function, and first parameter matches the using var
        bool okForUFCS = false;
        if (symbol->is(SymbolKind::Function))
        {
            const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->parameters.empty())
            {
                const auto firstParam = typeInfo->parameters.front()->typeInfo;
                if (firstParam->isSame(dep.node->typeInfo, CAST_FLAG_EXACT))
                    okForUFCS = true;
            }
        }

        // Exact same scope
        if (dep.scope == symScope || dep.scope->getFullName() == symScope->getFullName())
            getIt = true;

        // The symbol scope is an 'impl' inside a struct (impl for)
        else if (symScope->is(ScopeKind::Impl) && symScope->parentScope == dep.scope)
            getIt = true;

        // For mtd sub functions and UFCS
        else if (okForUFCS)
        {
            hasUFCS = true;
            getIt   = true;
        }

        if (getIt)
        {
            if (dep.node->parent->is(AstNodeKind::With))
                hasWith = true;
            dep.flags.add(okForUFCS ? COLLECTED_SCOPE_UFCS : 0);
            toCheck.push_back(dep);
        }
    }

    // With has priority over using
    if (hasWith)
    {
        for (auto& dep : toCheck)
        {
            SWAG_ASSERT(dep.node->parent);
            const bool isWith = dep.node->parent->is(AstNodeKind::With);
            if (!isWith)
                dep.node = nullptr;
        }
    }

    // If something has an UFCS match, then remove all those that don't
    if (hasUFCS)
    {
        for (auto& dep : toCheck)
        {
            if (!dep.flags.has(COLLECTED_SCOPE_UFCS))
                dep.node = nullptr;
        }
    }

    // Get one
    AstNode* dependentVar     = nullptr;
    AstNode* dependentVarLeaf = nullptr;
    for (const auto& dep : toCheck)
    {
        if (!dep.node)
            continue;
        if (dep.node == dependentVar)
            continue;

        if (dependentVar)
        {
            if (dep.node->isGeneratedSelf())
            {
                Diagnostic err{dependentVar, formErr(Err0013, dependentVar->typeInfo->getDisplayNameC())};
                err.addNote(dep.node->ownerFct, dep.node->ownerFct->token, toNte(Nte0115));
                err.addNote(toNte(Nte0034));
                return context->report(err);
            }

            Diagnostic err{dep.node, formErr(Err0013, dependentVar->typeInfo->getDisplayNameC())};
            err.addNote(dependentVar, toNte(Nte0060));
            err.addNote(toNte(Nte0034));
            return context->report(err);
        }

        dependentVar     = dep.node;
        dependentVarLeaf = dep.leafNode;

        // This way the UFCS can trigger even with an implicit 'using' var (typically for a 'using self')
        if (!identifierRef->previousResolvedNode)
        {
            if (symbol->is(SymbolKind::Function))
            {
                // Be sure we have a missing parameter in order to try UFCS
                const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
                const bool canTry   = canTryUFCS(context, typeFunc, dependentVar, false);
                YIELD();
                if (canTry)
                {
                    identifierRef->setResolvedSymbol(dependentVar->resolvedSymbolOverload()->symbol, dependentVar->resolvedSymbolOverload());
                    identifierRef->previousResolvedNode = dependentVar;
                }
            }
        }
    }

    *result     = dependentVar;
    *resultLeaf = dependentVarLeaf;
    return true;
}

bool Semantic::appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, const SymbolOverload* overload)
{
    if (!node->hasSemFlag(SEMFLAG_LAST_PARAM_CODE) && overload->symbol->is(SymbolKind::Function))
    {
        node->addSemFlag(SEMFLAG_LAST_PARAM_CODE);

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->isCode())
        {
            if (node->callParameters && node->callParameters->childCount() < typeFunc->parameters.size())
            {
                auto parent = node->parent;
                if (parent->parent->is(AstNodeKind::Catch) ||
                    parent->parent->is(AstNodeKind::Try) ||
                    parent->parent->is(AstNodeKind::Assume) ||
                    parent->parent->is(AstNodeKind::TryCatch))
                    parent = parent->parent;

                const auto parentIdx = parent->childParentIdx();
                if (parentIdx != parent->parent->childCount() - 1)
                {
                    const auto brother = parent->parent->children[parentIdx + 1];
                    if (brother->is(AstNodeKind::Statement))
                    {
                        const auto brotherParent = brother->parent;
                        switch (brotherParent->kind)
                        {
                            case AstNodeKind::Loop:
                            case AstNodeKind::If:
                            case AstNodeKind::CompilerIf:
                            case AstNodeKind::While:
                            {
                                const auto       msg = formErr(Err0156, Naming::kindName(overload).c_str(), overload->node->token.c_str(), brotherParent->token.c_str());
                                const Diagnostic err{node, node->token, msg};
                                return context->report(err, Diagnostic::hereIs(overload->node));
                            }
                            default:
                                break;
                        }

                        const auto fctCallParam = Ast::newFuncCallParam(nullptr, node->callParameters);
                        const auto codeNode     = Ast::newNode<AstCompilerCode>(AstNodeKind::CompilerCode, nullptr, fctCallParam);
                        codeNode->addAstFlag(AST_NO_BYTECODE);
                        codeNode->addSpecFlag(AstCompilerCode::SPEC_FLAG_FROM_NEXT);

                        Ast::removeFromParent(brother);
                        Ast::addChildBack(codeNode, brother);
                        const auto typeCode = makeType<TypeInfoCode>();
                        typeCode->content   = brother;
                        brother->addAstFlag(AST_NO_SEMANTIC);
                        fctCallParam->addSemFlag(SEMFLAG_AUTO_CODE_PARAM);
                        fctCallParam->typeInfo = typeCode;
                        codeNode->typeInfo     = typeCode;
                    }
                }
            }
        }
    }

    return true;
}

bool Semantic::fillMatchContextCallParameters(SemanticContext*      context,
                                              SymbolMatchContext&   symMatchContext,
                                              AstIdentifier*        identifier,
                                              const SymbolOverload* overload,
                                              AstNode*              ufcsFirstParam)
{
    const auto symbol         = overload->symbol;
    const auto symbolKind     = symbol->kind;
    const auto callParameters = identifier->callParameters;

    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCE_ALIAS);

    if (identifier->isSilentCall())
    {
        SWAG_ASSERT(typeRef->isArray());
        const auto typeArr = castTypeInfo<TypeInfoArray>(overload->typeInfo, TypeInfoKind::Array);
        typeRef            = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCE_ALIAS);
    }

    // :ClosureForceFirstParam
    // A closure has always a first parameter of type *void
    if (typeRef->isClosure() && identifier->callParameters)
    {
        if (!identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_CLOSURE_FIRST_PARAM))
        {
            Ast::constructNode(&context->closureFirstParam);
            context->closureFirstParam.kind     = AstNodeKind::FuncCallParam;
            context->closureFirstParam.typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
            symMatchContext.parameters.push_back(&context->closureFirstParam);
            symMatchContext.matchFlags |= SymbolMatchContext::MATCH_CLOSURE_PARAM;
        }
    }

    if (ufcsFirstParam)
        symMatchContext.parameters.push_back(ufcsFirstParam);

    if (callParameters || ufcsFirstParam)
    {
        if (symbolKind != SymbolKind::Attribute &&
            symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias &&
            !identifier->isSilentCall() &&
            !symbol->overloads[0]->typeInfo->isKindGeneric() &&
            !TypeManager::concretePtrRefType(symbol->overloads[0]->typeInfo, CONCRETE_FORCE_ALIAS)->isLambdaClosure())
        {
            if (symbolKind == SymbolKind::Variable)
            {
                const Diagnostic err{identifier, identifier->token, formErr(Err0290, identifier->token.c_str(), symbol->overloads[0]->typeInfo->getDisplayNameC())};
                return context->report(err, Diagnostic::hereIs(symbol->overloads[0]));
            }

            const Diagnostic err{identifier, identifier->token, formErr(Err0289, identifier->token.c_str(), Naming::aKindName(symbol->kind).c_str())};
            return context->report(err, Diagnostic::hereIs(symbol->overloads[0]));
        }
    }

    if (callParameters)
    {
        const auto childCount = callParameters->childCount();
        for (uint32_t i = 0; i < childCount; i++)
        {
            const auto oneParam = castAst<AstFuncCallParam>(callParameters->children[i], AstNodeKind::FuncCallParam);
            symMatchContext.parameters.push_back(oneParam);

            // Be sure all interfaces of the structure have been solved, in case a cast to an interface is necessary to match
            // a function
            // :WaitInterfaceReg
            const TypeInfoStruct* typeStruct = nullptr;
            if (oneParam->typeInfo->isStruct())
                typeStruct = castTypeInfo<TypeInfoStruct>(oneParam->typeInfo, TypeInfoKind::Struct);
            else if (oneParam->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                const auto typePtr = castTypeInfo<TypeInfoPointer>(oneParam->typeInfo, TypeInfoKind::Pointer);
                typeStruct         = castTypeInfo<TypeInfoStruct>(typePtr->pointedType, TypeInfoKind::Struct);
            }

            if (typeStruct)
            {
                waitAllStructInterfacesReg(context->baseJob, oneParam->typeInfo);
                YIELD();
            }

            // Variadic parameter must be the last one
            if (i != childCount - 1)
            {
                if (oneParam->typeInfo->isVariadic() ||
                    oneParam->typeInfo->isTypedVariadic() ||
                    oneParam->typeInfo->isCVariadic())
                {
                    Diagnostic err{oneParam, toErr(Err0514)};
                    err.hint = Diagnostic::isType(oneParam);
                    return context->report(err);
                }
            }
        }
    }

    return true;
}

bool Semantic::fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, const SymbolOverload* overload)
{
    const auto genericParameters = node->genericParameters;

    // User generic parameters
    if (genericParameters)
    {
        const auto symbol     = overload->symbol;
        const auto symbolKind = symbol->kind;

        node->inheritAstFlagsOr(genericParameters, AST_GENERIC);
        if (symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias)
        {
            const auto firstNode = symbol->nodes.front();
            Diagnostic err{genericParameters, formErr(Err0683, Naming::aKindName(symbol->kind).c_str())};
            err.addNote(node, node->token, formNte(Nte0199, node->token.c_str(), Naming::aKindName(symbol->kind).c_str()));
            err.addNote(Diagnostic::hereIs(firstNode));
            return context->report(err);
        }

        const auto childCount = genericParameters->childCount();
        for (uint32_t i = 0; i < childCount; i++)
        {
            const auto oneParam = castAst<AstFuncCallParam>(genericParameters->children[i], AstNodeKind::FuncCallParam);
            symMatchContext.genericParameters.push_back(oneParam);
            GenericReplaceType st;
            st.typeInfoReplace = oneParam->typeInfo;
            st.fromNode        = oneParam;
            symMatchContext.genericParametersCallTypes.push_back(st);
            symMatchContext.genericParametersCallValues.push_back(oneParam->safeComputedValue());
        }
    }

    return true;
}

bool Semantic::solveValidIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl)
{
    ScopedLock lk0(funcDecl->funcMutex);
    ScopedLock lk1(funcDecl->mutex);

    // Be sure block has been solved
    if (!funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_PARTIAL_RESOLVE))
    {
        funcDecl->dependentJobs.add(context->baseJob);
        context->baseJob->setPending(JobWaitKind::SemPartialResolve, funcDecl->resolvedSymbolName(), funcDecl, nullptr);
        return true;
    }

    // Execute #validif/#validifx block
    const auto expr = funcDecl->validIf->lastChild();

    // #validifx is evaluated for each call, so we remove the AST_VALUE_COMPUTED computed flag.
    // #validif is evaluated once, so keep it.
    if (funcDecl->validIf->is(AstNodeKind::CompilerValidIfx))
        expr->removeAstFlag(AST_COMPUTED_VALUE);

    if (!expr->hasFlagComputedValue())
    {
        const auto node            = context->node;
        context->validIfParameters = oneMatch->oneOverload->callParameters;

        ErrCxtStepKind type;
        if (funcDecl->validIf->is(AstNodeKind::CompilerValidIfx))
            type = ErrCxtStepKind::ValidIfx;
        else
            type = ErrCxtStepKind::ValidIf;

        PushErrCxtStep ec(context, node, type, nullptr);
        const auto     result      = executeCompilerNode(context, expr, false);
        context->validIfParameters = nullptr;
        if (!result)
            return false;
        YIELD();
    }

    // Result
    if (!expr->computedValue()->reg.b)
    {
        oneMatch->remove                              = true;
        oneMatch->oneOverload->symMatchContext.result = MatchResult::ValidIfFailed;
        return true;
    }

    if (funcDecl->content && funcDecl->content->hasAstFlag(AST_NO_SEMANTIC))
    {
        funcDecl->content->removeAstFlag(AST_NO_SEMANTIC);

        // Need to restart semantic on instantiated function and on its content,
        // because the #validif has passed
        // It's safe to create a job with the content as it has been fully evaluated.
        // It's NOT safe for the function itself as the job that deals with it can be
        // still running
        const auto job     = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, funcDecl->content, false);
        const auto baseJob = reinterpret_cast<SemanticJob*>(context->baseJob);
        job->context.errCxtSteps.insert(job->context.errCxtSteps.begin(), baseJob->context.errCxtSteps.begin(), baseJob->context.errCxtSteps.end());

        // This comes from a generic instantiation. Add context
        if (oneMatch->oneOverload->overload->typeInfo->isFromGeneric())
        {
            ErrorCxtStep expNode;

            const auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(oneMatch->oneOverload->overload->typeInfo, TypeInfoKind::FuncAttr);
            expNode.node          = context->node;
            expNode.replaceTypes  = typeFunc->replaceTypes;
            const auto exportNode = expNode.node->extraPointer<AstNode>(ExtraPointerKind::ExportNode);
            if (exportNode)
                expNode.node = exportNode;
            expNode.type = ErrCxtStepKind::Generic;
            job->context.errCxtSteps.push_back(expNode);
        }

        // To avoid a race condition with the job that is currently dealing with the funcDecl,
        // we will reevaluate it with a semanticAfterFct trick
        funcDecl->content->allocateExtension(ExtensionKind::Semantic);
        const auto sem = funcDecl->content->extSemantic();
        SWAG_ASSERT(!sem->semanticAfterFct || sem->semanticAfterFct == Semantic::resolveFuncDeclAfterSI || sem->semanticAfterFct == Semantic::resolveScopedStmtAfter);
        sem->semanticAfterFct = resolveFuncDeclAfterSI;

        g_ThreadMgr.addJob(job);
    }

    return true;
}

bool Semantic::waitForSymbols(SemanticContext* context, AstIdentifier* identifier, Job* job)
{
    auto& symbolsMatch = context->cacheSymbolsMatch;
    for (const auto& p : symbolsMatch)
    {
        const auto symbol = p.symbol;

        // First test, with just a SharedLock for contention
        {
            SharedLock lkn(symbol->mutex);
            if (!needToWaitForSymbolNoLock(context, identifier, symbol))
                continue;
        }

        {
            // Do the test again, this time with a lock
            ScopedLock lkn(symbol->mutex);
            if (!needToWaitForSymbolNoLock(context, identifier, symbol))
                continue;

            // Can we make a partial match ?
            if (needToCompleteSymbolNoLock(context, identifier, symbol, true))
            {
                waitSymbolNoLock(job, symbol);
                return true;
            }

            // Be sure that we have at least a registered incomplete symbol
            SWAG_ASSERT(symbol->overloads.size() == 1);
            if (!symbol->overloads[0]->hasFlag(OVERLOAD_INCOMPLETE))
            {
                waitSymbolNoLock(job, symbol);
                return true;
            }
        }

        // Partial resolution
        identifier->setResolvedSymbol(symbol, symbol->overloads[0]);
        identifier->typeInfo = identifier->resolvedSymbolOverload()->typeInfo;

        // In case identifier is part of a reference, need to initialize it
        if (identifier != identifier->identifierRef()->lastChild())
            SWAG_CHECK(setupIdentifierRef(context, identifier));

        symbolsMatch.clear();
        return true;
    }

    return true;
}

bool Semantic::resolveIdentifier(SemanticContext* context)
{
    const auto node = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    return resolveIdentifier(context, node, RI_ZERO);
}

bool Semantic::resolveIdentifier(SemanticContext* context, AstIdentifier* identifier, ResolveIdFlags riFlags)
{
    identifier->byteCodeFct = ByteCodeGen::emitIdentifier;

    // Current file scope
    const auto identifierRef = identifier->identifierRef();
    if (context->sourceFile && context->sourceFile->scopeFile && identifier->token.text == context->sourceFile->scopeFile->name)
    {
        SWAG_VERIFY(identifier == identifierRef->firstChild(), context->report({identifier, toErr(Err0366)}));
        identifierRef->startScope = context->sourceFile->scopeFile;
        return true;
    }

    // If previous identifier was generic, then stop evaluation
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->typeInfo->isKindGeneric())
    {
        // Just take the generic type for now
        identifier->typeInfo    = g_TypeMgr->typeInfoUndefined;
        identifierRef->typeInfo = identifierRef->previousResolvedNode->typeInfo;
        return true;
    }

    // We have updated one of the call parameters.
    // So we must update the types.
    if (identifier->callParameters && identifier->hasSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING))
    {
        for (const auto c : identifier->callParameters->children)
        {
            if (c->resolvedSymbolOverload() &&
                c->typeInfo &&
                c->typeInfo->isLambdaClosure() &&
                c->typeInfo != c->resolvedSymbolOverload()->typeInfo)
            {
                const auto newTypeInfo = c->resolvedSymbolOverload()->typeInfo->clone();
                newTypeInfo->kind      = TypeInfoKind::LambdaClosure;
                newTypeInfo->sizeOf    = c->typeInfo->sizeOf;
                c->typeInfo            = newTypeInfo;
            }
        }
    }

    // Already solved
    if (identifier->hasAstFlag(AST_FROM_GENERIC) &&
        identifier->typeInfo &&
        !identifier->typeInfo->isUndefined())
    {
        if (identifier->resolvedSymbolOverload())
        {
            OneMatch oneMatch;
            oneMatch.symbolOverload = identifier->resolvedSymbolOverload();
            oneMatch.scope          = identifier->resolvedSymbolOverload()->node->ownerScope;
            SWAG_CHECK(setMatchResult(context, identifierRef, identifier, oneMatch));
        }

        return true;
    }

    auto& scopeHierarchy     = context->cacheScopeHierarchy;
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
    auto& symbolsMatch       = context->cacheSymbolsMatch;

    // Compute dependencies if not already done
    if (identifier->semanticState == AstNodeResolveState::ProcessingChildren || riFlags.has(RI_FOR_GHOSTING) || riFlags.has(RI_FOR_ZERO_GHOSTING))
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        symbolsMatch.clear();
    }

    if (symbolsMatch.empty())
    {
        if (identifier->isSilentCall())
        {
            OneSymbolMatch sm;
            sm.symbol = identifierRef->resolvedSymbolName();
            symbolsMatch.push_back(sm);
        }
        else
        {
            SWAG_CHECK(findIdentifierInScopes(context, identifierRef, identifier));
            if (context->result != ContextResult::Done)
                symbolsMatch.clear();
            YIELD();
        }

        // Because of #self
        if (identifier->hasSemFlag(SEMFLAG_FORCE_SCOPE))
            return true;

        if (symbolsMatch.empty())
        {
            SWAG_ASSERT(identifierRef->hasAstFlag(AST_SILENT_CHECK));
            return true;
        }
    }

    // Filter symbols
    SWAG_CHECK(filterSymbols(context, identifier));
    if (symbolsMatch.empty())
        return context->report({identifier, formErr(Err0730, identifier->token.c_str())});

    // If we have multiple symbols, we need to check that no one can be solved as incomplete, otherwise it
    // can lead to ambiguities, or even worse, take the wrong one.
    if (symbolsMatch.size() > 1)
    {
        // A struct and an interface, this is legit
        bool fine = false;
        if (symbolsMatch.size() == 2 && symbolsMatch[0].symbol->is(SymbolKind::Struct) && symbolsMatch[1].symbol->is(SymbolKind::Interface))
            fine = true;
        else if (symbolsMatch.size() == 2 && symbolsMatch[0].symbol->is(SymbolKind::Interface) && symbolsMatch[1].symbol->is(SymbolKind::Struct))
            fine = true;

        if (!fine)
        {
            for (const auto& p : symbolsMatch)
            {
                SharedLock lkn(p.symbol->mutex);
                if (!needToCompleteSymbolNoLock(context, identifier, p.symbol, false))
                    return SemanticError::ambiguousSymbolError(context, identifier, p.symbol, symbolsMatch);
            }
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    SWAG_CHECK(waitForSymbols(context, identifier, context->baseJob));
    if (symbolsMatch.empty())
        return true;
    YIELD();

    // Do the actual match
    SWAG_CHECK(computeMatch(context, identifier, riFlags, symbolsMatch, identifierRef));
    YIELD();

    // No match !
    auto& matches = context->cacheMatches;
    if (matches.empty())
    {
        if (identifierRef->hasAstFlag(AST_SILENT_CHECK))
            return true;

        if (identifier->hasSemFlag(SEMFLAG_FORCE_UFCS))
            return SemanticError::unknownIdentifierError(context, identifierRef, castAst<AstIdentifier>(identifier, AstNodeKind::Identifier));

        return false;
    }

    // We have one or more match, and we were just asking to check ghosting. So we are done.
    if (riFlags.has(RI_FOR_GHOSTING | RI_FOR_ZERO_GHOSTING))
        return true;

    // Name alias with overloads (more than one match)
    if (matches.size() > 1 && identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS))
    {
        identifier->setResolvedSymbol(matches[0]->symbolOverload->symbol, nullptr);
        return true;
    }

    // Deal with UFCS. Now that the match is done, we will change the AST in order to
    // add the UFCS parameters to the function call parameters
    const auto& match = matches[0];
    if (match->ufcs && !identifier->isForcedUFCS())
    {
        // Do not change AST if this is code inside a generic function
        if (!identifier->ownerFct || !identifier->ownerFct->hasAstFlag(AST_GENERIC))
        {
            if (match->dependentVar && !identifierRef->previousResolvedNode)
            {
                identifierRef->setResolvedSymbol(match->dependentVar->resolvedSymbolOverload()->symbol, match->dependentVar->resolvedSymbolOverload());
                identifierRef->previousResolvedNode = match->dependentVar;
            }

            SWAG_CHECK(setFirstParamUFCS(context, identifierRef, *match));
        }
    }

    SWAG_CHECK(setMatchResultAndType(context, identifierRef, identifier, *match));
    YIELD();

    return true;
}
