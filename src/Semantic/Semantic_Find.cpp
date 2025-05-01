#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"

bool Semantic::findFuncCallInContext(SemanticContext* context, const AstNode* node, VectorNative<SymbolOverload*>& result, AstFuncCallParam** callParam)
{
    result.clear();

    auto findParent = node->parent;
    while (findParent && findParent->isNot(AstNodeKind::FuncCallParam) && findParent->isNot(AstNodeKind::Statement))
        findParent = findParent->parent;
    if (!findParent || findParent->isNot(AstNodeKind::FuncCallParam))
        return true;

    // If this is a parameter of a function call, we will try to deduce the type with a function signature
    if (findParent->isNot(AstNodeKind::FuncCallParam) ||
        findParent->getParent(1)->isNot(AstNodeKind::FuncCallParams) ||
        findParent->getParent(2)->isNot(AstNodeKind::Identifier))
        return true;

    const auto fctCallParam = castAst<AstFuncCallParam>(findParent);
    const auto idref        = castAst<AstIdentifierRef>(fctCallParam->getParent(3), AstNodeKind::IdentifierRef);
    const auto id           = castAst<AstIdentifier>(fctCallParam->getParent(2), AstNodeKind::Identifier);
    *callParam              = fctCallParam;

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
            if (symbol != node->resolvedSymbolName() || symbol->cptOverloads > 1)
            {
                waitSymbolNoLock(context->baseJob, symbol);
                return true;
            }
        }
    }

    // Be sure symbols have been solved, because we need the types to be deduced
    for (const auto& sm : symbolMatch)
    {
        const auto symbol = sm.symbol;
        if (symbol->isNot(SymbolKind::Function) &&
            symbol->isNot(SymbolKind::Variable) &&
            symbol->isNot(SymbolKind::Attribute) &&
            symbol->isNot(SymbolKind::Struct))
            continue;

        SharedLock lk(symbol->mutex);
        for (const auto overload : symbol->overloads)
        {
            const auto concrete = overload->typeInfo->getConcreteAlias();
            if (concrete->isGeneric() || concrete->isFromGeneric())
                continue;
            if (!concrete->isFuncAttr() && !concrete->isLambdaClosure() && !concrete->isStruct())
                continue;

            result.push_back(overload);
        }
    }

    return true;
}

bool Semantic::findFuncCallParamInContext(SemanticContext*                     context,
                                          const AstFuncCallParam*              callParam,
                                          const VectorNative<SymbolOverload*>& overloads,
                                          VectorNative<TypeInfoParam*>&        result)
{
    result.clear();

    for (const auto overload : overloads)
    {
        const auto concrete = overload->typeInfo->getConcreteAlias();
        if (!concrete->isFuncAttr() && !concrete->isLambdaClosure())
            continue;
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(concrete, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

        // Named param
        const auto isNamed = callParam->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        if (isNamed)
        {
            for (const auto param : typeFunc->parameters)
            {
                if (param->name == isNamed->token.text)
                {
                    result.push_back(param);
                }
            }

            continue;
        }

        auto       paramIdx = callParam->childParentIdx();
        const auto callFct  = castAst<AstIdentifier>(callParam->getParent(2), AstNodeKind::Identifier);

        // Because of UFCS
        if (!overload->symbol->is(SymbolKind::Attribute))
        {
            if (typeFunc->isMethod())
                paramIdx++;

            else if (callFct->childParentIdx() != 0)
            {
                const auto previousNode = callFct->identifierRef()->children[callFct->childParentIdx() - 1];
                if (!previousNode->resolvedSymbolName())
                    paramIdx++;
                else if (previousNode->resolvedSymbolName()->isNot(SymbolKind::Namespace) &&
                         previousNode->resolvedSymbolName()->isNot(SymbolKind::Struct))
                {
                    paramIdx++;
                }
            }
        }

        if (paramIdx >= typeFunc->parameters.size())
            continue;
        result.push_back(typeFunc->parameters[paramIdx]);
    }

    return true;
}

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

bool Semantic::findEnumTypeInCallContext(SemanticContext*                                  context,
                                         const AstNode*                                    node,
                                         VectorNative<TypeInfoEnum*>&                      result,
                                         VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has,
                                         VectorNative<SymbolOverload*>&                    testedOver)
{
    VectorNative<SymbolOverload*> overloads;
    AstFuncCallParam*             fctCallParam;
    SWAG_CHECK(findFuncCallInContext(context, node, overloads, &fctCallParam));
    YIELD();
    if (overloads.empty())
        return true;

    const auto findParent = node->findParent(AstNodeKind::FuncCallParam);
    for (auto& overload : overloads)
    {
        testedOver.push_back(overload);
        const auto concrete = overload->typeInfo->getConcreteAlias();

        if (concrete->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(concrete, TypeInfoKind::Struct);
            const auto foundName  = typeStruct->scope->symTable.find(findParent->token.text);
            if (!foundName || foundName->overloads.empty())
                continue;
            auto typeEnum = findEnumType(foundName->overloads[0]->typeInfo);
            if (!typeEnum)
                continue;
            has.push_back_once({foundName->overloads[0]->node, typeEnum});
            if (typeEnum->contains(node->token.text))
                result.push_back_once(typeEnum);
            continue;
        }
    }

    VectorNative<TypeInfoParam*> resultParams;
    SWAG_CHECK(findFuncCallParamInContext(context, fctCallParam, overloads, resultParams));
    for (const auto it : resultParams)
    {
        auto typeEnum = findEnumType(it->typeInfo);
        if (!typeEnum)
            continue;
        has.push_back_once({it->declNode, typeEnum});
        if (typeEnum->contains(node->token.text))
            result.push_back_once(typeEnum);
    }

    return true;
}

bool Semantic::findEnumTypeInContext(SemanticContext*                                  context,
                                     const AstNode*                                    node,
                                     VectorNative<TypeInfoEnum*>&                      result,
                                     VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has,
                                     VectorNative<SymbolOverload*>&                    testedOver)
{
    result.clear();
    has.clear();

    // Search as a call parameter
    SWAG_CHECK(findEnumTypeInCallContext(context, node, result, has, testedOver));
    YIELD();
    if (!result.empty())
        return true;

    // Search as a return
    const auto findParent = node->findParent(AstNodeKind::Return);
    if (findParent)
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
