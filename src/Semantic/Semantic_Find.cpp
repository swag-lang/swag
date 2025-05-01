#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"

TypeInfoEnum* Semantic::findEnumType(TypeInfo* typeInfo)
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
        findParent->getParent(1)->is(AstNodeKind::FuncCallParams) &&
        findParent->getParent(2)->is(AstNodeKind::Identifier))
    {
        const auto fctCallParam = castAst<AstFuncCallParam>(findParent);
        const auto idref        = castAst<AstIdentifierRef>(fctCallParam->getParent(3), AstNodeKind::IdentifierRef);
        const auto id           = castAst<AstIdentifier>(fctCallParam->getParent(2), AstNodeKind::Identifier);

        VectorNative<OneSymbolMatch> symbolMatch;
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
            if (symbol->isNot(SymbolKind::Function) &&
                symbol->isNot(SymbolKind::Variable) &&
                symbol->isNot(SymbolKind::Attribute) &&
                symbol->isNot(SymbolKind::Struct))
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
            if (symbol->isNot(SymbolKind::Function) &&
                symbol->isNot(SymbolKind::Variable) &&
                symbol->isNot(SymbolKind::Attribute) &&
                symbol->isNot(SymbolKind::Struct))
                continue;

            SharedLock lk(symbol->mutex);
            for (auto& overload : symbol->overloads)
            {
                const auto concrete = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCE_ALIAS);
                if (concrete->isGeneric() || concrete->isFromGeneric())
                    continue;

                if (concrete->isStruct())
                {
                    testedOver.push_back(overload);
                    const auto typeStruct = castTypeInfo<TypeInfoStruct>(concrete, TypeInfoKind::Struct);

                    const auto foundName = typeStruct->scope->symTable.find(fctCallParam->token.text);
                    if (!foundName || foundName->overloads.empty())
                        continue;
                    auto typeEnum = findEnumType(foundName->overloads[0]->typeInfo);
                    if (!typeEnum)
                        continue;
                    has.push_back_once({foundName->overloads[0]->node, typeEnum});
                    if (typeEnum->contains(node->token.text))
                        result.push_back_once(typeEnum);
                }

                else if (concrete->isFuncAttr() || concrete->isLambdaClosure())
                {
                    testedOver.push_back(overload);
                    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(concrete, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

                    // If there's only one corresponding type in the function, then take it
                    // If it's not the correct parameter, the match will not be done, so we do not really care here
                    VectorNative<TypeInfoEnum*> subResult;
                    for (const auto param : typeFunc->parameters)
                    {
                        auto typeEnum = findEnumType(param->typeInfo);
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

                    // More than one possible type (at least two different enums with the same identical requested name in the function signature)
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
                                    auto typeEnum = findEnumType(concreteP);
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

    // We go up in the hierarchy until we find a statement or a contextual type to return
    bool canScanChildren = true;
    auto parent          = node->parent;
    while (parent)
    {
        if (canScanChildren && (parent->is(AstNodeKind::BinaryOp) ||
                                parent->is(AstNodeKind::FactorOp) ||
                                parent->is(AstNodeKind::AffectOp) ||
                                parent->is(AstNodeKind::VarDecl) ||
                                parent->is(AstNodeKind::ConstDecl) ||
                                parent->is(AstNodeKind::FuncDeclParam) ||
                                parent->is(AstNodeKind::Switch)))
        {
            if (parent->isNot(AstNodeKind::FactorOp))
                canScanChildren = false;

            SharedLock lk(parent->mutex);
            for (auto c : parent->children)
            {
                auto typeInfoChild = c->typeInfo;

                // Take the first child of an expression list
                if (c->is(AstNodeKind::ExpressionList))
                {
                    const auto expr = castAst<AstExpressionList>(c, AstNodeKind::ExpressionList);
                    typeInfoChild   = expr->firstChild()->typeInfo;
                }

                auto typeEnum = findEnumType(typeInfoChild);
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
            auto       typeEnum = findEnumType(parent->typeInfo);
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
