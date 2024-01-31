#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "SemanticError.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool Semantic::resolveNameAlias(SemanticContext* context)
{
    auto node = CastAst<AstAlias>(context->node, AstNodeKind::NameAlias);
    auto back = node->childs.back();

    SWAG_ASSERT(node->resolvedSymbolName);
    SWAG_ASSERT(back->resolvedSymbolName);

    SWAG_CHECK(collectAttributes(context, node, nullptr));
    node->flags |= AST_NO_BYTECODE;

    // Constraints with alias on a variable
    if (back->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        // alias x = struct.x is not possible
        if (back->kind == AstNodeKind::IdentifierRef)
        {
            int cptVar = 0;
            for (auto& c : back->childs)
            {
                if (c->resolvedSymbolName && c->resolvedSymbolName->kind == SymbolKind::Variable)
                {
                    SWAG_VERIFY(cptVar == 0, context->report({back, Err(Err0163)}));
                    cptVar++;
                }
            }
        }
    }

    if (back->resolvedSymbolName->kind != SymbolKind::Namespace &&
        back->resolvedSymbolName->kind != SymbolKind::Function &&
        back->resolvedSymbolName->kind != SymbolKind::Variable)
    {
        Diagnostic                diag{back, Fmt(Err(Err0328), Naming::aKindName(back->resolvedSymbolName->kind).c_str())};
        Vector<const Diagnostic*> notes;

        notes.push_back(Diagnostic::note(Nte(Nte0013)));

        if (back->resolvedSymbolName->kind == SymbolKind::Enum ||
            back->resolvedSymbolName->kind == SymbolKind::Interface ||
            back->resolvedSymbolName->kind == SymbolKind::TypeAlias ||
            back->resolvedSymbolName->kind == SymbolKind::Struct)
        {
            notes.push_back(Diagnostic::note(node, node->kwdLoc, Fmt(Nte(Nte0025), Naming::aKindName(back->resolvedSymbolName->kind).c_str())));
        }

        return context->report(diag, notes);
    }

    SWAG_CHECK(node->ownerScope->symTable.registerNameAlias(context, node, node->resolvedSymbolName, back->resolvedSymbolName, back->resolvedSymbolOverload));
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicNode(node);
    return true;
}

bool Semantic::preResolveIdentifierRef(SemanticContext* context)
{
    auto node = CastAst<AstIdentifierRef>(context->node, AstNodeKind::IdentifierRef);

    // When duplicating an identifier ref, and solve it again, we need to be sure that all that
    // stuff is reset
    if (!(node->semFlags & SEMFLAG_TYPE_SOLVED))
    {
        node->typeInfo             = nullptr;
        node->previousResolvedNode = nullptr;
        node->startScope           = nullptr;
    }

    return true;
}

bool Semantic::resolveIdentifierRef(SemanticContext* context)
{
    auto node      = static_cast<AstIdentifierRef*>(context->node);
    auto childBack = node->childs.back();

    // Keep resolution
    if (!node->typeInfo || !(node->semFlags & SEMFLAG_TYPE_SOLVED))
    {
        node->resolvedSymbolName     = childBack->resolvedSymbolName;
        node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
        node->typeInfo               = childBack->typeInfo;
    }

    node->token.text  = childBack->token.text;
    node->byteCodeFct = ByteCodeGen::emitIdentifierRef;

    // Flag inheritance
    node->flags |= AST_CONST_EXPR | AST_FROM_GENERIC_REPLACE;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_FROM_GENERIC_REPLACE))
            node->flags &= ~AST_FROM_GENERIC_REPLACE;
        if (child->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
        if (child->flags & AST_IS_CONST)
            node->flags |= AST_IS_CONST;
    }

    if (childBack->hasComputedValue())
        node->inheritComputedValue(childBack);
    node->inheritOrFlag(childBack, AST_L_VALUE | AST_R_VALUE | AST_TRANSIENT | AST_VALUE_IS_GEN_TYPEINFO | AST_SIDE_EFFECTS);

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload &&
        (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        if (!node->hasComputedValue())
        {
            node->setFlagsValueIsComputed();
            *node->computedValue = node->resolvedSymbolOverload->computedValue;
        }

        node->flags |= AST_NO_BYTECODE_CHILDS;

        // If literal is stored in a data segment, then it's still a left value (we can take the address for example)
        if (!node->computedValue->storageSegment || node->computedValue->storageOffset == 0xFFFFFFFF)
            node->flags &= ~AST_L_VALUE;
    }

    return true;
}

bool Semantic::setupIdentifierRef(SemanticContext* context, AstNode* node)
{
    // If type of previous one was const, then we force this node to be const (cannot change it)
    if (node->parent->typeInfo && node->parent->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    auto overload = node->resolvedSymbolOverload;
    if (overload && overload->flags & OVERLOAD_CONST_ASSIGN)
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN;

    if (node->parent->kind != AstNodeKind::IdentifierRef)
        return true;

    auto identifierRef = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // If we cannot assign previous, and this was AST_IS_CONST_ASSIGN_INHERIT, then we cannot assign
    // this one either
    if (identifierRef->previousResolvedNode && (identifierRef->previousResolvedNode->semFlags & SEMFLAG_IS_CONST_ASSIGN_INHERIT))
    {
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN;
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN_INHERIT;
    }

    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = nullptr;

    auto typeInfo  = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
    auto scopeType = typeInfo->getConcreteAlias();
    if (scopeType->isLambdaClosure())
    {
        auto funcType = CastTypeInfo<TypeInfoFuncAttr>(scopeType, TypeInfoKind::LambdaClosure);
        scopeType     = TypeManager::concreteType(funcType->returnType, CONCRETE_FORCEALIAS);
    }

    if (!(identifierRef->semFlags & SEMFLAG_TYPE_SOLVED))
        identifierRef->typeInfo = typeInfo;

    switch (scopeType->kind)
    {
    case TypeInfoKind::Enum:
    {
        identifierRef->startScope = CastTypeInfo<TypeInfoEnum>(scopeType, TypeInfoKind::Enum)->scope;
        node->typeInfo            = typeInfo;
        break;
    }
    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(scopeType, TypeInfoKind::Pointer);
        auto subType     = TypeManager::concreteType(typePointer->pointedType);
        if (subType->isStruct() || subType->isInterface())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeListArray:
    case TypeInfoKind::TypeListTuple:
        identifierRef->startScope = CastTypeInfo<TypeInfoList>(scopeType, scopeType->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Interface:
    case TypeInfoKind::Struct:
        identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(scopeType, scopeType->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(scopeType, TypeInfoKind::Array);
        auto subType   = TypeManager::concreteType(typeArray->finalType);
        if (subType->isStruct())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(scopeType, TypeInfoKind::Slice);
        auto subType   = TypeManager::concreteType(typeSlice->pointedType);
        if (subType->isStruct())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    default:
        break;
    }

    return true;
}

bool Semantic::isFunctionButNotACall(SemanticContext* context, AstNode* node, SymbolName* symbol)
{
    AstIdentifier* id = nullptr;
    if (node && node->kind == AstNodeKind::Identifier)
    {
        id = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (id != id->identifierRef()->childs.back())
            return false;
    }

    if (node && node->parent && node->parent->parent)
    {
        auto grandParent = node->parent->parent;
        if (symbol->kind == SymbolKind::Attribute)
        {
            if (grandParent->kind != AstNodeKind::AttrUse)
                return true;
        }
        else if (symbol->kind == SymbolKind::Function)
        {
            if (symbol->kind == SymbolKind::Function)
            {
                if (grandParent->kind == AstNodeKind::MakePointer && node == node->parent->childs.back())
                {
                    if (id && id->callParameters)
                        return false;
                    return true;
                }

                if (grandParent->kind == AstNodeKind::MakePointerLambda && node == node->parent->childs.back())
                    return true;
            }

            if (grandParent->kind == AstNodeKind::TypeAlias ||
                grandParent->kind == AstNodeKind::NameAlias ||
                (grandParent->kind == AstNodeKind::IntrinsicDefined) ||
                (grandParent->kind == AstNodeKind::IntrinsicLocation) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicStringOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicNameOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicRunes) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicTypeOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicDeclType) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicKindOf))
            {
                return true;
            }
        }
    }

    return false;
}

TypeInfoEnum* Semantic::findEnumTypeInContext(SemanticContext* context, TypeInfo* typeInfo)
{
    while (true)
    {
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FUNC | CONCRETE_FORCEALIAS);
        if (!typeInfo)
            return nullptr;

        if (typeInfo->isArray())
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo     = typeArr->finalType;
            continue;
        }

        if (typeInfo->isSlice())
        {
            auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
            typeInfo       = typeSlice->pointedType;
            continue;
        }

        if (typeInfo->isPointer())
        {
            auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            typeInfo         = typePointer->pointedType;
            continue;
        }

        if (typeInfo->isEnum())
        {
            return CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        }

        return nullptr;
    }

    return nullptr;
}

bool Semantic::findEnumTypeInContext(SemanticContext*                                  context,
                                     AstNode*                                          node,
                                     VectorNative<TypeInfoEnum*>&                      result,
                                     VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has,
                                     VectorNative<SymbolOverload*>&                    testedOver)
{
    result.clear();
    has.clear();

    // If this is a parameter of a function call, we will try to deduce the type with a function signature
    auto fctCallParam = node->findParent(AstNodeKind::FuncCallParam);
    if (fctCallParam &&
        fctCallParam->parent->kind == AstNodeKind::FuncCallParams &&
        fctCallParam->parent->parent->kind == AstNodeKind::Identifier)
    {
        VectorNative<OneSymbolMatch> symbolMatch;

        auto idref = CastAst<AstIdentifierRef>(fctCallParam->parent->parent->parent, AstNodeKind::IdentifierRef);
        auto id    = CastAst<AstIdentifier>(fctCallParam->parent->parent, AstNodeKind::Identifier);

        context->silentError++;
        auto found = findIdentifierInScopes(context, symbolMatch, idref, id);
        context->silentError--;
        YIELD();

        if (!found || symbolMatch.empty())
            return true;

        // Be sure symbols have been solved, because we need the types to be deduced
        for (const auto& sm : symbolMatch)
        {
            auto symbol = sm.symbol;
            if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Variable)
                continue;

            ScopedLock ls(symbol->mutex);
            if (symbol->cptOverloads)
            {
                Semantic::waitSymbolNoLock(context->baseJob, symbol);
                return true;
            }
        }

        for (const auto& sm : symbolMatch)
        {
            auto symbol = sm.symbol;
            if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Variable)
                continue;

            for (auto& overload : symbol->overloads)
            {
                auto concrete = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCEALIAS);
                if (!concrete->isFuncAttr() && !concrete->isLambdaClosure())
                    continue;
                if (concrete->isGeneric() || concrete->isFromGeneric())
                    continue;

                testedOver.push_back(overload);
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(concrete, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

                // If there's only one corresponding type in the function, then take it
                // If it's not the correct parameter, the match will not be done, so we do not really care here
                VectorNative<TypeInfoEnum*> subResult;
                for (auto param : typeFunc->parameters)
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
                    for (size_t i = 0; i < fctCallParam->parent->childs.size(); i++)
                    {
                        auto child = fctCallParam->parent->childs[i];
                        if (child == fctCallParam)
                            break;
                        if (child->typeInfo && child->typeInfo->getConcreteAlias()->isEnum())
                            enumIdx++;
                        else if (child->kind == AstNodeKind::IdentifierRef && child->specFlags & AstIdentifierRef::SPECFLAG_AUTO_SCOPE)
                            enumIdx++;
                    }

                    for (auto p : typeFunc->parameters)
                    {
                        auto concreteP = TypeManager::concreteType(p->typeInfo, CONCRETE_FORCEALIAS);
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
    else
    {
        auto fctReturn = node->findParent(AstNodeKind::Return);
        if (fctReturn)
        {
            auto funcNode = getFunctionForReturn(fctReturn);
            if (funcNode->returnType)
            {
                auto typeInfo = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FUNC | CONCRETE_FORCEALIAS);
                if (typeInfo->isEnum())
                {
                    auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
                    has.push_back_once({nullptr, typeEnum});
                    if (typeEnum->contains(node->token.text))
                    {
                        result.push_back(typeEnum);
                        return true;
                    }
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
        if (canScanChilds && (parent->kind == AstNodeKind::BinaryOp ||
                              parent->kind == AstNodeKind::FactorOp ||
                              parent->kind == AstNodeKind::AffectOp ||
                              parent->kind == AstNodeKind::VarDecl ||
                              parent->kind == AstNodeKind::ConstDecl ||
                              parent->kind == AstNodeKind::FuncDeclParam ||
                              parent->kind == AstNodeKind::Switch))
        {
            if (parent->kind != AstNodeKind::FactorOp)
                canScanChilds = false;

            SharedLock lk(parent->mutex);
            for (auto c : parent->childs)
            {
                auto typeInfoChild = c->typeInfo;

                // Take first child of an expression list
                if (c->kind == AstNodeKind::ExpressionList)
                {
                    auto expr     = CastAst<AstExpressionList>(c, AstNodeKind::ExpressionList);
                    typeInfoChild = expr->childs[0]->typeInfo;
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

        if (parent->kind == AstNodeKind::Statement ||
            parent->kind == AstNodeKind::SwitchCaseBlock ||
            parent->kind == AstNodeKind::FuncDecl ||
            parent->kind == AstNodeKind::File ||
            parent->kind == AstNodeKind::Module ||
            parent->kind == AstNodeKind::FuncDeclParam ||
            parent->kind == AstNodeKind::VarDecl ||
            parent->kind == AstNodeKind::ConstDecl)
            break;

        parent = parent->parent;
    }

    return true;
}

bool Semantic::getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result, AstNode** resultLeaf)
{
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;

    if (scopeHierarchyVars.empty())
        return true;

    // Not for a global symbol
    if (overload->flags & OVERLOAD_VAR_GLOBAL)
        return true;

    auto kind = overload->symbol->kind;
    switch (kind)
    {
    case SymbolKind::Enum:
    case SymbolKind::Struct:
    case SymbolKind::TypeAlias:
    case SymbolKind::Interface:
        return true;
    default:
        break;
    }

    auto                        symbol   = overload->symbol;
    auto                        symScope = symbol->ownerTable->scope;
    Vector<AlternativeScopeVar> toCheck;

    // Collect all matches
    bool hasWith = false;
    bool hasUfcs = false;
    for (auto& dep : scopeHierarchyVars)
    {
        bool getIt = false;

        // This is a function, and first parameter matches the using var
        bool okForUfcs = false;
        if (symbol->kind == SymbolKind::Function)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
            if (typeInfo->parameters.size())
            {
                auto firstParam = typeInfo->parameters.front()->typeInfo;
                if (firstParam->isSame(dep.node->typeInfo, CASTFLAG_EXACT))
                    okForUfcs = true;
            }
        }

        // Exact same scope
        if (dep.scope == symScope || dep.scope->getFullName() == symScope->getFullName())
            getIt = true;

        // The symbol scope is an 'impl' inside a struct (impl for)
        else if (symScope->kind == ScopeKind::Impl && symScope->parentScope == dep.scope)
            getIt = true;

        // For mtd sub functions and ufcs
        else if (okForUfcs)
        {
            hasUfcs = true;
            getIt   = true;
        }

        if (getIt)
        {
            if (dep.node->parent->kind == AstNodeKind::With)
                hasWith = true;
            dep.flags |= okForUfcs ? ALTSCOPE_UFCS : 0;
            toCheck.push_back(dep);
        }
    }

    // With has priority over using
    if (hasWith)
    {
        for (auto& dep : toCheck)
        {
            SWAG_ASSERT(dep.node->parent);
            bool isWith = dep.node->parent->kind == AstNodeKind::With;
            if (!isWith)
                dep.node = nullptr;
        }
    }

    // If something has an ufcs match, then remove all those that don't
    if (hasUfcs)
    {
        for (auto& dep : toCheck)
        {
            if (!(dep.flags & ALTSCOPE_UFCS))
                dep.node = nullptr;
        }
    }

    // Get one
    AstNode* dependentVar     = nullptr;
    AstNode* dependentVarLeaf = nullptr;
    for (auto& dep : toCheck)
    {
        if (!dep.node)
            continue;
        if (dep.node == dependentVar)
            continue;

        if (dependentVar)
        {
            if (dep.node->isGeneratedSelf())
            {
                Diagnostic diag{dependentVar, Fmt(Err(Err0013), dependentVar->typeInfo->getDisplayNameC())};
                auto       note  = Diagnostic::note(dep.node->ownerFct, dep.node->ownerFct->token, Nte(Nte0115));
                auto       note1 = Diagnostic::note(Nte(Nte0034));
                return context->report(diag, note, note1);
            }
            else
            {
                Diagnostic diag{dep.node, Fmt(Err(Err0013), dependentVar->typeInfo->getDisplayNameC())};
                auto       note  = Diagnostic::note(dependentVar, Nte(Nte0060));
                auto       note1 = Diagnostic::note(Nte(Nte0034));
                return context->report(diag, note, note1);
            }
        }

        dependentVar     = dep.node;
        dependentVarLeaf = dep.leafNode;

        // This way the ufcs can trigger even with an implicit 'using' var (typically for a 'using self')
        if (!identifierRef->previousResolvedNode)
        {
            if (symbol->kind == SymbolKind::Function)
            {
                // Be sure we have a missing parameter in order to try ufcs
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
                bool canTry   = canTryUfcs(context, typeFunc, node->callParameters, dependentVar, false);
                YIELD();
                if (canTry)
                {
                    identifierRef->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
                    identifierRef->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
                    identifierRef->previousResolvedNode   = dependentVar;
                }
            }
        }
    }

    *result     = dependentVar;
    *resultLeaf = dependentVarLeaf;
    return true;
}

bool Semantic::appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload)
{
    if (!(node->semFlags & SEMFLAG_LAST_PARAM_CODE) && (overload->symbol->kind == SymbolKind::Function))
    {
        node->semFlags |= SEMFLAG_LAST_PARAM_CODE;

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->isCode())
        {
            if (node->callParameters && node->callParameters->childs.size() < typeFunc->parameters.size())
            {
                auto parent = node->parent;
                if (parent->parent->kind == AstNodeKind::Catch ||
                    parent->parent->kind == AstNodeKind::Try ||
                    parent->parent->kind == AstNodeKind::Assume ||
                    parent->parent->kind == AstNodeKind::TryCatch)
                    parent = parent->parent;

                auto parentIdx = parent->childParentIdx();
                if (parentIdx != parent->parent->childs.size() - 1)
                {
                    auto brother = parent->parent->childs[parentIdx + 1];
                    if (brother->kind == AstNodeKind::Statement)
                    {
                        auto brotherParent = brother->parent;
                        switch (brotherParent->kind)
                        {
                        case AstNodeKind::Loop:
                        case AstNodeKind::If:
                        case AstNodeKind::CompilerIf:
                        case AstNodeKind::While:
                        {
                            Diagnostic diag{node, node->token, Fmt(Err(Err0156), Naming::kindName(overload).c_str(), overload->node->token.ctext(), brotherParent->token.ctext())};
                            return context->report(diag, Diagnostic::hereIs(overload->node));
                        }
                        default:
                            break;
                        }

                        auto fctCallParam = Ast::newFuncCallParam(context->sourceFile, node->callParameters);
                        auto codeNode     = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, node->sourceFile, fctCallParam);
                        codeNode->flags |= AST_NO_BYTECODE;

                        Ast::removeFromParent(brother);
                        Ast::addChildBack(codeNode, brother);
                        auto typeCode     = makeType<TypeInfoCode>();
                        typeCode->content = brother;
                        brother->flags |= AST_NO_SEMANTIC;
                        fctCallParam->semFlags |= SEMFLAG_AUTO_CODE_PARAM;
                        fctCallParam->typeInfo = typeCode;
                        codeNode->typeInfo     = typeCode;
                    }
                }
            }
        }
    }

    return true;
}

bool Semantic::fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* identifier, SymbolOverload* overload, AstNode* ufcsFirstParam)
{
    auto symbol         = overload->symbol;
    auto symbolKind     = symbol->kind;
    auto callParameters = identifier->callParameters;

    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCEALIAS);

    if (identifier->isSilentCall())
    {
        SWAG_ASSERT(typeRef->isArray());
        auto typeArr = CastTypeInfo<TypeInfoArray>(overload->typeInfo, TypeInfoKind::Array);
        typeRef      = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCEALIAS);
    }

    // :ClosureForceFirstParam
    // A closure has always a first parameter of type *void
    if (typeRef->isClosure() && identifier->callParameters)
    {
        if (!(identifier->specFlags & AstIdentifier::SPECFLAG_CLOSURE_FIRST_PARAM))
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
            !TypeManager::concretePtrRefType(symbol->overloads[0]->typeInfo, CONCRETE_FORCEALIAS)->isLambdaClosure())
        {
            if (symbolKind == SymbolKind::Variable)
            {
                Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0290), identifier->token.ctext(), symbol->overloads[0]->typeInfo->getDisplayNameC())};
                return context->report(diag, Diagnostic::hereIs(symbol->overloads[0]));
            }
            else
            {
                Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0289), identifier->token.ctext(), Naming::aKindName(symbol->kind).c_str())};
                return context->report(diag, Diagnostic::hereIs(symbol->overloads[0]));
            }
        }
    }

    if (callParameters)
    {
        auto childCount = callParameters->childs.size();
        for (size_t i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.parameters.push_back(oneParam);

            // Be sure all interfaces of the structure have been solved, in case a cast to an interface is necessary to match
            // a function
            // :WaitInterfaceReg
            TypeInfoStruct* typeStruct = nullptr;
            if (oneParam->typeInfo->isStruct())
                typeStruct = CastTypeInfo<TypeInfoStruct>(oneParam->typeInfo, TypeInfoKind::Struct);
            else if (oneParam->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                auto typePtr = CastTypeInfo<TypeInfoPointer>(oneParam->typeInfo, TypeInfoKind::Pointer);
                typeStruct   = CastTypeInfo<TypeInfoStruct>(typePtr->pointedType, TypeInfoKind::Struct);
            }

            if (typeStruct)
            {
                Semantic::waitAllStructInterfacesReg(context->baseJob, oneParam->typeInfo);
                YIELD();
            }

            // Variadic parameter must be the last one
            if (i != childCount - 1)
            {
                if (oneParam->typeInfo->isVariadic() ||
                    oneParam->typeInfo->isTypedVariadic() ||
                    oneParam->typeInfo->isCVariadic())
                {
                    Diagnostic diag{oneParam, Err(Err0514)};
                    diag.hint = Diagnostic::isType(oneParam);
                    return context->report(diag);
                }
            }
        }
    }

    return true;
}

bool Semantic::fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload)
{
    auto genericParameters = node->genericParameters;

    // User generic parameters
    if (genericParameters)
    {
        auto symbol     = overload->symbol;
        auto symbolKind = symbol->kind;

        node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
        if (symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias)
        {
            auto       firstNode = symbol->nodes.front();
            Diagnostic diag{genericParameters, Fmt(Err(Err0683), Naming::aKindName(symbol->kind).c_str())};
            auto       note = Diagnostic::note(node, node->token, Fmt(Nte(Nte0199), node->token.ctext(), Naming::aKindName(symbol->kind).c_str()));
            return context->report(diag, note, Diagnostic::hereIs(firstNode));
        }

        auto childCount = genericParameters->childs.size();
        for (size_t i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.genericParameters.push_back(oneParam);
            GenericReplaceType st;
            st.typeInfoReplace = oneParam->typeInfo;
            st.fromNode        = oneParam;
            symMatchContext.genericParametersCallTypes.push_back(st);
            symMatchContext.genericParametersCallValues.push_back(oneParam->computedValue);
        }
    }

    return true;
}

bool Semantic::solveValidIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl)
{
    ScopedLock lk0(funcDecl->funcMutex);
    ScopedLock lk1(funcDecl->mutex);

    // Be sure block has been solved
    if (!(funcDecl->specFlags & AstFuncDecl::SPECFLAG_PARTIAL_RESOLVE))
    {
        funcDecl->dependentJobs.add(context->baseJob);
        context->baseJob->setPending(JobWaitKind::SemPartialResolve, funcDecl->resolvedSymbolName, funcDecl, nullptr);
        return true;
    }

    // Execute #validif/#validifx block
    auto expr = funcDecl->validif->childs.back();

    // #validifx is evaluated for each call, so we remove the AST_VALUE_COMPUTED computed flag.
    // #validif is evaluated once, so keep it.
    if (funcDecl->validif->kind == AstNodeKind::CompilerValidIfx)
        expr->flags &= ~AST_VALUE_COMPUTED;

    if (!expr->hasComputedValue())
    {
        auto node                  = context->node;
        context->validIfParameters = oneMatch->oneOverload->callParameters;

        ErrCxtStepKind type;
        if (funcDecl->validif->kind == AstNodeKind::CompilerValidIfx)
            type = ErrCxtStepKind::ValidIfx;
        else
            type = ErrCxtStepKind::ValidIf;

        PushErrCxtStep ec(context, node, type, nullptr);
        auto           result      = executeCompilerNode(context, expr, false);
        context->validIfParameters = nullptr;
        if (!result)
            return false;
        YIELD();
    }

    // Result
    if (!expr->computedValue->reg.b)
    {
        oneMatch->remove                              = true;
        oneMatch->oneOverload->symMatchContext.result = MatchResult::ValidIfFailed;
        return true;
    }

    if (funcDecl->content && funcDecl->content->flags & AST_NO_SEMANTIC)
    {
        funcDecl->content->flags &= ~AST_NO_SEMANTIC;

        // Need to restart semantic on instantiated function and on its content,
        // because the #validif has passed
        // It's safe to create a job with the content as it has been fully evaluated.
        // It's NOT safe for the function itself as the job that deals with it can be
        // still running
        auto job     = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, funcDecl->content, false);
        auto baseJob = (SemanticJob*) context->baseJob;
        job->context.errCxtSteps.insert(job->context.errCxtSteps.begin(),
                                        baseJob->context.errCxtSteps.begin(),
                                        baseJob->context.errCxtSteps.end());

        // This comes from a generic instantiation. Add context
        if (oneMatch->oneOverload->overload->typeInfo->isFromGeneric())
        {
            ErrorCxtStep expNode;

            auto typeFunc        = CastTypeInfo<TypeInfoFuncAttr>(oneMatch->oneOverload->overload->typeInfo, TypeInfoKind::FuncAttr);
            expNode.node         = context->node;
            expNode.replaceTypes = typeFunc->replaceTypes;
            if (expNode.node->hasExtMisc() && expNode.node->extMisc()->exportNode)
                expNode.node = expNode.node->extMisc()->exportNode;
            expNode.type = ErrCxtStepKind::Generic;
            job->context.errCxtSteps.push_back(expNode);
        }

        // To avoid a race condition with the job that is currently dealing with the funcDecl,
        // we will reevaluate it with a semanticAfterFct trick
        funcDecl->content->allocateExtension(ExtensionKind::Semantic);
        auto sem = funcDecl->content->extSemantic();
        SWAG_ASSERT(!sem->semanticAfterFct ||
                    sem->semanticAfterFct == Semantic::resolveFuncDeclAfterSI ||
                    sem->semanticAfterFct == Semantic::resolveScopedStmtAfter);
        sem->semanticAfterFct = Semantic::resolveFuncDeclAfterSI;

        g_ThreadMgr.addJob(job);
    }

    return true;
}

bool Semantic::resolveIdentifier(SemanticContext* context)
{
    auto node = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    return resolveIdentifier(context, node, RI_ZERO);
}

bool Semantic::resolveIdentifier(SemanticContext* context, AstIdentifier* identifier, uint32_t riFlags)
{
    auto  job                = context->baseJob;
    auto& scopeHierarchy     = context->cacheScopeHierarchy;
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
    auto& dependentSymbols   = context->cacheDependentSymbols;
    auto  identifierRef      = identifier->identifierRef();

    identifier->byteCodeFct = ByteCodeGen::emitIdentifier;

    // Current file scope
    if (context->sourceFile && context->sourceFile->scopeFile && identifier->token.text == context->sourceFile->scopeFile->name)
    {
        SWAG_VERIFY(identifier == identifierRef->childs.front(), context->report({identifier, Err(Err0366)}));
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
    if (identifier->callParameters && identifier->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING)
    {
        for (auto c : identifier->callParameters->childs)
        {
            if (c->resolvedSymbolOverload &&
                c->typeInfo &&
                c->typeInfo->isLambdaClosure() &&
                c->typeInfo != c->resolvedSymbolOverload->typeInfo)
            {
                auto newTypeInfo    = c->resolvedSymbolOverload->typeInfo->clone();
                newTypeInfo->kind   = TypeInfoKind::LambdaClosure;
                newTypeInfo->sizeOf = c->typeInfo->sizeOf;
                c->typeInfo         = newTypeInfo;
            }
        }
    }

    // Already solved
    if ((identifier->flags & AST_FROM_GENERIC) &&
        identifier->typeInfo &&
        !identifier->typeInfo->isUndefined())
    {
        if (identifier->resolvedSymbolOverload)
        {
            OneMatch oneMatch;
            oneMatch.symbolOverload = identifier->resolvedSymbolOverload;
            oneMatch.scope          = identifier->resolvedSymbolOverload->node->ownerScope;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, identifier, oneMatch));
        }

        return true;
    }

    // Compute dependencies if not already done
    if (identifier->semanticState == AstNodeResolveState::ProcessingChilds || (riFlags & RI_FOR_GHOSTING) || (riFlags & RI_FOR_ZERO_GHOSTING))
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        dependentSymbols.clear();
    }

    if (dependentSymbols.empty())
    {
        if (identifier->isSilentCall())
        {
            OneSymbolMatch sm;
            sm.symbol = identifierRef->resolvedSymbolName;
            dependentSymbols.push_back(sm);
        }
        else
        {
            SWAG_CHECK(findIdentifierInScopes(context, identifierRef, identifier));
            if (context->result != ContextResult::Done)
            {
                dependentSymbols.clear();
                return true;
            }
        }

        // Because of #self
        if (identifier->semFlags & SEMFLAG_FORCE_SCOPE)
            return true;

        if (dependentSymbols.empty())
        {
            SWAG_ASSERT(identifierRef->flags & AST_SILENT_CHECK);
            return true;
        }
    }

    // Filter symbols
    SWAG_CHECK(filterSymbols(context, identifier));
    if (dependentSymbols.empty())
        return context->report({identifier, Fmt(Err(Err0730), identifier->token.ctext())});

    // If we have multiple symbols, we need to check that no one can be solved as incomplete, otherwhise it
    // can lead to ambiguities, or even worse, take the wrong one.
    if (dependentSymbols.size() > 1)
    {
        // A struct and an interface, this is legit
        bool fine = false;
        if (dependentSymbols.size() == 2 && dependentSymbols[0].symbol->kind == SymbolKind::Struct && dependentSymbols[1].symbol->kind == SymbolKind::Interface)
            fine = true;
        else if (dependentSymbols.size() == 2 && dependentSymbols[0].symbol->kind == SymbolKind::Interface && dependentSymbols[1].symbol->kind == SymbolKind::Struct)
            fine = true;

        if (!fine)
        {
            for (auto& p : dependentSymbols)
            {
                SharedLock lkn(p.symbol->mutex);
                if (!needToCompleteSymbol(context, identifier, p.symbol, false))
                    return SemanticError::ambiguousSymbolError(context, identifier, p.symbol, dependentSymbols);
            }
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto& p : dependentSymbols)
    {
        auto symbol = p.symbol;

        // First test, with just a SharedLock for contention
        {
            SharedLock lkn(symbol->mutex);
            if (!needToWaitForSymbol(context, identifier, symbol))
                continue;
        }

        {
            // Do the test again, this time with a lock
            ScopedLock lkn(symbol->mutex);
            if (!needToWaitForSymbol(context, identifier, symbol))
                continue;

            // Can we make a partial match ?
            if (needToCompleteSymbol(context, identifier, symbol, true))
            {
                Semantic::waitSymbolNoLock(job, symbol);
                return true;
            }

            // Be sure that we have at least a registered incomplete symbol
            SWAG_ASSERT(symbol->overloads.size() == 1);
            if (!(symbol->overloads[0]->flags & OVERLOAD_INCOMPLETE))
            {
                Semantic::waitSymbolNoLock(job, symbol);
                return true;
            }
        }

        // Partial resolution
        identifier->resolvedSymbolName     = symbol;
        identifier->resolvedSymbolOverload = symbol->overloads[0];
        identifier->typeInfo               = identifier->resolvedSymbolOverload->typeInfo;

        // In case identifier is part of a reference, need to initialize it
        if (identifier != identifier->identifierRef()->childs.back())
            SWAG_CHECK(setupIdentifierRef(context, identifier));

        return true;
    }

    auto orgResolvedSymbolOverload = identifierRef->resolvedSymbolOverload;
    auto orgResolvedSymbolName     = identifierRef->resolvedSymbolName;
    auto orgPreviousResolvedNode   = identifierRef->previousResolvedNode;

    // In case of a reevaluation, ufcsFirstParam will be null, but we still want to cast for ufcs
    bool hasForcedUfcs = false;
    if (identifier->callParameters && !identifier->callParameters->childs.empty() && identifier->callParameters->childs.front()->flags & AST_TO_UFCS)
        hasForcedUfcs = true;

    while (true)
    {
        // Collect all overloads to solve. We collect also the number of overloads when the collect is done, in
        // case that number changes (other thread) during the resolution. Because if the number of overloads differs
        // at one point in the process (for a given symbol), then this will invalidate the resolution
        // (number of overloads can change when instantiating a generic)
        auto& toSolveOverload = context->cacheToSolveOverload;
        toSolveOverload.clear();
        for (auto& p : dependentSymbols)
        {
            auto       symbol = p.symbol;
            SharedLock lk(symbol->mutex);
            for (auto over : symbol->overloads)
            {
                OneOverload t;
                t.overload         = over;
                t.scope            = p.scope;
                t.cptOverloads     = (uint32_t) symbol->overloads.size();
                t.cptOverloadsInit = (uint32_t) symbol->cptOverloadsInit;
                toSolveOverload.push_back(t);
            }
        }

        // Can happen if a symbol is inside a disabled #if for example
        if (toSolveOverload.empty())
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;
            return context->report({identifier, Fmt(Err(Err0730), identifier->token.ctext())});
        }

        auto& listTryMatch = context->cacheListTryMatch;
        context->clearTryMatch();

        for (auto& oneOver : toSolveOverload)
        {
            auto symbolOverload                   = oneOver.overload;
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;

            // Is there a using variable associated with the symbol to solve ?
            AstNode* dependentVar     = nullptr;
            AstNode* dependentVarLeaf = nullptr;
            SWAG_CHECK(getUsingVar(context, identifierRef, identifier, symbolOverload, &dependentVar, &dependentVarLeaf));
            YIELD();

            // Get the ufcs first parameter if we can
            AstNode* ufcsFirstParam = nullptr;

            // The ufcs parameter has already been set in we are evaluating an identifier for the second time
            // (when we inline a function call)
            if (!identifier->callParameters || identifier->callParameters->childs.empty() || !(identifier->callParameters->childs.front()->flags & AST_TO_UFCS))
            {
                SWAG_CHECK(getUfcs(context, identifierRef, identifier, symbolOverload, &ufcsFirstParam));
                YIELD();
                if ((identifier->semFlags & SEMFLAG_FORCE_UFCS) && !ufcsFirstParam)
                    continue;
            }

            // If the last parameter of a function is of type 'code', and the last call parameter is not,
            // then we take the next statement, after the function, and put it as the last parameter
            SWAG_CHECK(appendLastCodeStatement(context, identifier, symbolOverload));

            // This if for a lambda
            bool forLambda = false;
            if (identifier->forceTakeAddress() && isFunctionButNotACall(context, identifier, symbolOverload->symbol))
                forLambda = true;

            // Will try with ufcs, and will try without
            for (int tryUfcs = 0; tryUfcs < 2; tryUfcs++)
            {
                auto  tryMatch        = context->getTryMatch();
                auto& symMatchContext = tryMatch->symMatchContext;

                tryMatch->symMatchContext.matchFlags |= forLambda ? SymbolMatchContext::MATCH_FOR_LAMBDA : 0;

                tryMatch->genericParameters = identifier->genericParameters;
                tryMatch->callParameters    = identifier->callParameters;
                tryMatch->dependentVar      = dependentVar;
                tryMatch->dependentVarLeaf  = dependentVarLeaf;
                tryMatch->overload          = symbolOverload;
                tryMatch->scope             = oneOver.scope;
                tryMatch->ufcs              = ufcsFirstParam || hasForcedUfcs;
                tryMatch->cptOverloads      = oneOver.cptOverloads;
                tryMatch->cptOverloadsInit  = oneOver.cptOverloadsInit;

                SWAG_CHECK(fillMatchContextCallParameters(context, symMatchContext, identifier, symbolOverload, ufcsFirstParam));
                YIELD();
                SWAG_CHECK(fillMatchContextGenericParameters(context, symMatchContext, identifier, symbolOverload));

                listTryMatch.push_back(tryMatch);

                if (!ufcsFirstParam)
                    break;
                if (identifier->semFlags & SEMFLAG_FORCE_UFCS)
                    break;
                ufcsFirstParam = nullptr;
            }
        }

        if (listTryMatch.empty())
        {
            context->cacheMatches.clear();
            break;
        }

        uint32_t mipFlags = 0;
        if (riFlags & RI_FOR_GHOSTING)
            mipFlags |= MIP_FOR_GHOSTING;
        if (riFlags & RI_FOR_ZERO_GHOSTING)
            mipFlags |= MIP_FOR_ZERO_GHOSTING;
        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, identifier, mipFlags));

        if (context->result == ContextResult::Pending)
        {
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;
            return true;
        }

        if (context->result == ContextResult::NewChilds1)
        {
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;
            context->result                       = ContextResult::NewChilds;
            return true;
        }

        // The number of overloads for a given symbol has been changed by another thread, which
        // means that we need to restart everything...
        if (context->result != ContextResult::NewChilds)
            break;

        context->result = ContextResult::Done;
    }

    if (context->cacheMatches.empty())
    {
        // We want to force the ufcs
        if (identifier->semFlags & SEMFLAG_FORCE_UFCS)
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;
            SemanticError::unknownIdentifierError(context, identifierRef, CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier));
            return false;
        }

        return false;
    }

    if (riFlags & (RI_FOR_GHOSTING | RI_FOR_ZERO_GHOSTING))
        return true;

    // Name alias with overloads (more than one match)
    if (identifier->specFlags & AstIdentifier::SPECFLAG_NAME_ALIAS && context->cacheMatches.size() > 1)
    {
        identifier->resolvedSymbolName     = context->cacheMatches[0]->symbolOverload->symbol;
        identifier->resolvedSymbolOverload = nullptr;
        return true;
    }

    // Deal with ufcs. Now that the match is done, we will change the ast in order to
    // add the ufcs parameters to the function call parameters
    auto& match = context->cacheMatches[0];
    if (match->ufcs && !hasForcedUfcs)
    {
        // Do not change AST if this is code inside a generic function
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            if (match->dependentVar && !identifierRef->previousResolvedNode)
            {
                identifierRef->resolvedSymbolOverload = match->dependentVar->resolvedSymbolOverload;
                identifierRef->resolvedSymbolName     = match->dependentVar->resolvedSymbolOverload->symbol;
                identifierRef->previousResolvedNode   = match->dependentVar;
            }

            SWAG_CHECK(ufcsSetFirstParam(context, identifierRef, *match));
        }
    }

    if (identifier->isSilentCall())
        identifier->typeInfo = identifierRef->typeInfo;
    else if (match->typeWasForced)
        identifier->typeInfo = match->typeWasForced;
    else
        identifier->typeInfo = match->symbolOverload->typeInfo;

    SWAG_CHECK(setSymbolMatch(context, identifierRef, identifier, *match));
    return true;
}
