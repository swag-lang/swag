#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "SemanticJob.h"
#include "TypeManager.h"

namespace
{
    void cleanMatches(VectorNative<OneMatch*>& matches)
    {
        for (size_t i = 0; i < matches.size(); i++)
        {
            if (matches[i]->remove)
            {
                matches[i] = matches.back();
                matches.pop_back();
                i--;
            }
        }
    }
}

bool Semantic::filterMatchesDirect(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    const auto node         = context->node;
    const auto countMatches = matches.size();

    // Sometimes we don't care about multiple symbols with the same name
    if (countMatches > 1 && node->parent && node->parent->parent)
    {
        const auto grandParent = node->parent->parent;
        if (grandParent->kind == AstNodeKind::IntrinsicDefined ||
            (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicNameOf))
        {
            matches.count = 1;
            return true;
        }
    }

    for (size_t i = 0; i < countMatches; i++)
    {
        const auto curMatch = matches[i];
        const auto over     = curMatch->symbolOverload;
        const auto overSym  = over->symbol;

        // Take care of #validif/#validifx
        if (overSym->kind == SymbolKind::Function &&
            !context->node->hasAstFlag(AST_IN_VALIDIF) &&
            !context->node->hasAttribute(ATTRIBUTE_MATCH_VALIDIF_OFF))
        {
            const auto funcDecl = castAst<AstFuncDecl>(over->node, AstNodeKind::FuncDecl);
            if (funcDecl->validIf)
            {
                SWAG_CHECK(solveValidIf(context, curMatch, funcDecl));
                YIELD();
                if (curMatch->remove)
                    continue;
            }
        }

        // Do not match "self"
        if (overSym->kind == SymbolKind::Function &&
            context->node->hasAttribute(ATTRIBUTE_MATCH_SELF_OFF))
        {
            if (node->hasOwnerInline())
            {
                if (node->ownerInline()->func->resolvedSymbolOverload() == over)
                {
                    matches[i]->remove = true;
                    continue;
                }
            }
            else if (node->ownerFct && node->ownerFct->resolvedSymbolOverload() == over)
            {
                matches[i]->remove = true;
                continue;
            }
        }
    }

    cleanMatches(matches);
    return true;
}

bool Semantic::filterMatchesCompare(const SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    const auto countMatches = matches.size();
    if (countMatches <= 1)
        return true;

    const auto node = context->node;
    for (size_t i = 0; i < countMatches; i++)
    {
        const auto curMatch = matches[i];
        const auto over     = curMatch->symbolOverload;
        const auto overSym  = over->symbol;

        // In case of an alias, we take the first one, which should be the 'closest' one.
        // Not sure this is true, perhaps one day will have to change the way we find it.
        if (overSym->name.find(g_LangSpec->name_atalias) == 0)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (i != j)
                    matches[j]->remove = true;
            }

            break;
        }

        // Priority to 'ufcs'
        if (!curMatch->ufcs)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->ufcs)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a 'moveref' call
        if (curMatch->castFlagsResult.has(CAST_RESULT_AUTO_MOVE_OP_AFFECT))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->castFlagsResult.has(CAST_RESULT_AUTO_MOVE_OP_AFFECT))
                {
                    matches[j]->remove = true;
                }
            }
        }

        // Priority to a guess 'moveref' call
        if (curMatch->castFlagsResult.has(CAST_RESULT_GUESS_MOVE))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->castFlagsResult.has(CAST_RESULT_GUESS_MOVE))
                {
                    matches[j]->remove = true;
                }
            }
        }

        // Priority if no CAST_RESULT_STRUCT_CONVERT
        if (curMatch->castFlagsResult.has(CAST_RESULT_STRUCT_CONVERT))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->castFlagsResult.has(CAST_RESULT_STRUCT_CONVERT))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a match without an auto cast
        if (curMatch->castFlagsResult.has(CAST_RESULT_GEN_AUTO_OP_CAST))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->castFlagsResult.has(CAST_RESULT_GEN_AUTO_OP_CAST))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to no coerce
        if (curMatch->castFlagsResult.has(CAST_RESULT_COERCE))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->castFlagsResult.has(CAST_RESULT_COERCE))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a non empty function
        if (over->node->isEmptyFct())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->symbolOverload->node->isEmptyFct() &&
                    matches[j]->symbolOverload->symbol == curMatch->symbolOverload->symbol)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a local var/parameter versus a function
        if (over->typeInfo->isFuncAttr())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->symbolOverload->hasFlag(OVERLOAD_VAR_LOCAL | OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_VAR_INLINE))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a concrete type versus a generic one
        const auto lastOverloadType = overSym->ownerTable->scope->owner->typeInfo;
        if (lastOverloadType && lastOverloadType->isGeneric())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                const auto newOverloadType = matches[j]->symbolOverload->symbol->ownerTable->scope->owner->typeInfo;
                if (newOverloadType && !newOverloadType->isGeneric())
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // If we are referencing an interface name, then interface can come from the interface definition and
        // from the sub scope inside a struct (OVERLOAD_IMPL_IN_STRUCT).
        if (over->hasFlag(OVERLOAD_IMPL_IN_STRUCT))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->symbolOverload->hasFlag(OVERLOAD_IMPL_IN_STRUCT))
                {
                    // If interface name is alone, then we take in priority the interface definition, not the impl block
                    if (node == node->parent->children.front())
                        curMatch->remove = true;
                    // If interface name is not alone (like X.ITruc), then we take in priority the sub scope
                    else
                        matches[j]->remove = true;
                }
            }
        }

        // Priority to a user generic parameters, instead of a copy one
        if (over->node->hasAstFlag(AST_GENERATED_GENERIC_PARAM))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->symbolOverload->node->hasAstFlag(AST_GENERATED_GENERIC_PARAM))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to the same stack frame
        if (!node->isSameStackFrame(over))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (node->isSameStackFrame(matches[j]->symbolOverload))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to lambda call in a parameter over a function outside the actual function
        if (over->typeInfo->isLambdaClosure())
        {
            const auto callParams = over->node->findParent(AstNodeKind::FuncCallParams);
            if (callParams)
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->symbol->kind == SymbolKind::Function)
                    {
                        const auto nodeFct = matches[j]->symbolOverload->node;
                        if (!callParams->ownerFct->isParentOf(nodeFct))
                        {
                            matches[j]->remove = true;
                        }
                    }
                }
            }
        }

        // Priority to the same inline scope
        if (node->hasOwnerInline())
        {
            if (!node->ownerInline()->scope->isParentOf(over->node->ownerScope))
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    const auto nodeToKeep = matches[j]->symbolOverload->node;
                    if (node->ownerInline()->scope->isParentOf(nodeToKeep->ownerScope))
                    {
                        const auto inMixin = nodeToKeep->findParent(AstNodeKind::CompilerMixin);
                        if (inMixin)
                        {
                            const auto inMacro = inMixin->findParent(AstNodeKind::CompilerMacro);
                            if (inMacro)
                            {
                                break;
                            }
                        }

                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // Priority to not a namespace (??)
        if (curMatch->symbolOverload->symbol->kind == SymbolKind::Namespace)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->symbolOverload->symbol->kind != SymbolKind::Namespace)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Closure variable has a priority over a "out of scope" one
        if (curMatch->symbolOverload->symbol->kind == SymbolKind::Variable)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (i == j)
                    continue;
                if (matches[j]->symbolOverload->symbol->kind == SymbolKind::Variable && matches[j]->symbolOverload->hasFlag(OVERLOAD_VAR_CAPTURE))
                {
                    if (curMatch->symbolOverload->node->ownerScope->isParentOf(matches[j]->symbolOverload->node->ownerScope))
                    {
                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // If we didn't match with ufcs, then priority to a match that do not start with 'self'
        if (!curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc0->parameters.empty() && typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->isFuncAttr())
                    {
                        const auto typeFunc1 = castTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (typeFunc1->parameters.empty() || !typeFunc1->parameters[0]->typeInfo->isSelf())
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }

        // If we did match with ufcs, then priority to a match that starts with 'self'
        if (curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters.empty() || !typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->isFuncAttr())
                    {
                        const auto typeFunc1 = castTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (!typeFunc1->parameters.empty() && typeFunc1->parameters[0]->typeInfo->isSelf())
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }

        // 2 ufcs : priority to the first parameter that is not const
        if (curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters[0]->typeInfo->isConst())
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->ufcs && matches[j]->symbolOverload->typeInfo->isFuncAttr())
                    {
                        const auto typeFunc1 = castTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (!typeFunc1->parameters[0]->typeInfo->isConst())
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    cleanMatches(matches);
    return true;
}

void Semantic::computeMatchesCoerceCast(VectorNative<OneMatch*>& matches)
{
    for (const auto m : matches)
    {
        if (m->symbolOverload->symbol->kind != SymbolKind::Function)
            continue;

        m->coerceCast = 0;
        for (const auto flags : m->solvedCastFlags)
        {
            if (flags.has(CAST_RESULT_COERCE) && !flags.has(CAST_RESULT_UNTYPED_CONVERT))
                m->coerceCast++;
            else if (flags.has(CAST_RESULT_CONST_COERCE))
                m->coerceCast++;
        }
    }
}

bool Semantic::filterMatchesCoerceCast(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    if (matches.size() <= 1)
        return true;

    ranges::sort(matches, [](const OneMatch* x, const OneMatch* y)
                 {
                     return x->coerceCast < y->coerceCast;
                 });

    const auto prio = matches[0]->coerceCast;
    for (const auto m : matches)
    {
        if (m->symbolOverload->symbol->kind != SymbolKind::Function)
            continue;

        if (m->coerceCast > prio)
            m->remove = true;
    }

    cleanMatches(matches);
    return true;
}

namespace
{
    bool areGenericReplaceTypesIdentical(TypeInfo* typeInfo, const OneMatch& match)
    {
        if (typeInfo->kind != TypeInfoKind::FuncAttr)
            return false;

        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
        if (match.genericReplaceTypes.size() != typeFunc->replaceTypes.size())
            return false;

        for (const auto& rt : match.genericReplaceTypes)
        {
            auto it = typeFunc->replaceTypes.find(rt.first);
            if (it == typeFunc->replaceTypes.end())
                return false;
            if (rt.second.typeInfoReplace != it->second.typeInfoReplace)
                return false;
        }

        return true;
    }

    int scopeCost(const Scope* from, const Scope* to)
    {
        // Not sure if this is really correct, because we do not take care of 'using'
        int cost = 0;
        while (from && from != to)
        {
            cost += 1;
            from = from->parentScope;
        }

        return cost;
    }
}

bool Semantic::filterGenericMatches(const SemanticContext* context, VectorNative<OneMatch*>& matches, VectorNative<OneMatch*>& genMatches)
{
    // We have a match and more than one generic match
    // We need to be sure than instantiated another generic match will not be better than keeping
    // the already instantiated one
    if (genMatches.size() > 1 && matches.size() == 1)
    {
        for (const auto& m : genMatches)
        {
            if (m->coerceCast < matches[0]->coerceCast)
            {
                matches.clear();
                break;
            }
        }
    }

    if (genMatches.size() > 1 && matches.size() == 1)
    {
        auto   bestIsIdCost = true;
        size_t bestGenId    = 0;

        auto idCost = scopeCost(context->node->ownerScope, matches[0]->symbolOverload->node->ownerScope);
        for (size_t i = 0; i < genMatches.size(); i++)
        {
            const auto& p    = genMatches[i];
            const auto  cost = scopeCost(context->node->ownerScope, p->symbolOverload->node->ownerScope);
            if (cost < idCost)
            {
                bestIsIdCost = false;
                bestGenId    = i;
                idCost       = cost;
            }
        }

        if (!bestIsIdCost)
        {
            matches.clear();
            const auto temp = genMatches[bestGenId];
            genMatches.clear();
            genMatches.push_back(temp);
        }
    }

    // Take the most "specialized" generic match, i.e. the one with the more 'genericReplaceTypes'
    if (genMatches.size() > 1)
    {
        size_t bestS = 0;
        size_t bestI = 0;
        for (size_t i = 0; i < genMatches.size(); i++)
        {
            if (genMatches[i]->genericReplaceTypes.size() > bestS)
            {
                bestS = genMatches[i]->genericReplaceTypes.size();
                bestI = i;
            }
        }

        for (size_t i = 0; i < genMatches.size(); i++)
        {
            if (genMatches[i]->genericReplaceTypes.size() < bestS)
            {
                if (genMatches[i]->matchFlags == genMatches[bestI]->matchFlags &&
                    genMatches[i]->castFlagsResult == genMatches[bestI]->castFlagsResult)
                {
                    genMatches[i] = genMatches.back();
                    genMatches.pop_back();
                    i--;
                }
            }
        }
    }

    // priority to 'ufcs'
    for (size_t i = 0; i < genMatches.size(); i++)
    {
        if (!genMatches[i]->ufcs)
        {
            for (size_t j = 0; j < genMatches.size(); j++)
            {
                if (genMatches[j]->ufcs)
                {
                    genMatches[i] = genMatches.back();
                    genMatches.pop_back();
                    i--;
                    break;
                }
            }
        }
    }

    // A match with a struct conversion has less priority
    for (size_t i = 0; i < genMatches.size(); i++)
    {
        if (genMatches[i]->castFlagsResult.has(CAST_RESULT_STRUCT_CONVERT))
        {
            for (size_t j = 0; j < genMatches.size(); j++)
            {
                if (!genMatches[j]->castFlagsResult.has(CAST_RESULT_STRUCT_CONVERT))
                {
                    genMatches[i] = genMatches.back();
                    genMatches.pop_back();
                    i--;
                    break;
                }
            }
        }
    }

    // If there's one match, but we match with a untyped int conversion, and there's more than one
    // generic match, we must remove the match in order to raise an error on multiple overloads, otherwise
    // we can match or not with an untyped integer depending on instantiation order.
    if (matches.size() == 1 &&
        genMatches.size() > 1 &&
        matches[0]->castFlagsResult.has(CAST_RESULT_UNTYPED_CONVERT) &&
        !matches[0]->oneOverload->overload->node->isSpecialFunctionName())
    {
        matches.clear();
    }

    // We eliminate all generic matches that will create an already existing match.
    // This way we can keep some matches with autoOpCast if we have no choice (we will not try to
    // instantiate 'without' autoOpCast, because it would result in a autoOpCast too).
    if (!matches.empty() && !genMatches.empty())
    {
        VectorNative<OneMatch*> newGenericMatches;
        newGenericMatches.reserve(static_cast<int>(genMatches.size()));
        for (const auto& match : matches)
        {
            for (auto& genMatch : genMatches)
            {
                const auto same = areGenericReplaceTypesIdentical(match->oneOverload->overload->typeInfo, *genMatch);
                if (!same)
                    newGenericMatches.push_back_once(genMatch);
            }
        }

        genMatches = std::move(newGenericMatches);
    }

    return true;
}

bool Semantic::filterMatchesInContext(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    if (matches.size() <= 1)
        return true;

    for (size_t i = 0; i < matches.size(); i++)
    {
        const auto                                       oneMatch = matches[i];
        const auto                                       over     = oneMatch->symbolOverload;
        VectorNative<TypeInfoEnum*>                      typeEnum;
        VectorNative<std::pair<AstNode*, TypeInfoEnum*>> hasEnum;
        VectorNative<SymbolOverload*>                    testedOver;
        SWAG_CHECK(findEnumTypeInContext(context, over->node, typeEnum, hasEnum, testedOver));
        YIELD();

        if (typeEnum.size() == 1 && typeEnum[0]->scope == oneMatch->scope)
        {
            matches.clear();
            matches.push_back(oneMatch);
            return true;
        }

        // We try to eliminate a function if it does not match a contextual generic match
        // Because if we call a generic function without generic parameters, we can have multiple matches
        // if the generic type has not been deduced from parameters (if any).
        if (over->symbol->kind == SymbolKind::Function)
        {
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc->replaceTypes.empty())
            {
                const auto             node = context->node;
                VectorNative<AstNode*> toCheck;

                // Pick contextual generic type replacements
                if (node->ownerFct)
                    toCheck.push_back(node->ownerFct);
                if (node->hasOwnerInline())
                    toCheck.push_back(node->ownerInline()->func);

                for (const auto c : toCheck)
                {
                    const auto typeFuncCheck = castTypeInfo<TypeInfoFuncAttr>(c->typeInfo, TypeInfoKind::FuncAttr);
                    if (typeFuncCheck->replaceTypes.size() != typeFunc->replaceTypes.size())
                        continue;
                    for (auto& it : typeFunc->replaceTypes)
                    {
                        auto it1 = typeFuncCheck->replaceTypes.find(it.first);
                        if (it1 == typeFuncCheck->replaceTypes.end())
                            continue;
                        if (it.second.typeInfoReplace != it1->second.typeInfoReplace)
                            oneMatch->remove = true;
                    }
                }

                if (oneMatch->remove)
                {
                    matches[i] = matches.back();
                    matches.pop_back();
                    i--;
                }
            }
        }
    }

    return true;
}

bool Semantic::filterSymbols(SemanticContext* context, AstIdentifier* node)
{
    const auto identifierRef    = node->identifierRef();
    auto&      dependentSymbols = context->cacheDependentSymbols;

    if (dependentSymbols.size() == 1)
        return true;

    for (auto& p : dependentSymbols)
    {
        const auto oneSymbol = p.symbol;
        if (p.remove)
            continue;

        // A variable inside a scope file has priority
        if (p.asFlags.has(ALT_SCOPE_FILE_PRIVATE))
        {
            for (auto& p1 : dependentSymbols)
            {
                if (!p1.asFlags.has(ALT_SCOPE_FILE_PRIVATE))
                    p1.remove = true;
            }
        }

        // A variable which is name as a function...
        if (!node->callParameters &&
            oneSymbol->kind == SymbolKind::Function &&
            !isFunctionButNotACall(context, node, oneSymbol))
        {
            p.remove = true;
            continue;
        }

        // A function call which is named as a variable, and the variable is not a lambda
        if (node->callParameters &&
            oneSymbol->kind == SymbolKind::Variable)
        {
            bool ok = false;
            for (const auto& o : oneSymbol->overloads)
            {
                if (o->typeInfo->isLambdaClosure())
                {
                    ok = true;
                    break;
                }
            }

            if (!ok)
            {
                p.remove = true;
                continue;
            }
        }

        // If a generic type does not come from a 'using', it has priority
        if (!p.asFlags.has(ALT_SCOPE_STRUCT_USING) && p.symbol->kind == SymbolKind::GenericType)
        {
            for (auto& p1 : dependentSymbols)
            {
                if (p1.asFlags.has(ALT_SCOPE_STRUCT_USING) && p1.symbol->kind == SymbolKind::GenericType)
                    p1.remove = true;
            }
        }

        // Reference to a variable inside a struct, without a direct explicit reference
        bool isValid = true;
        if (oneSymbol->kind != SymbolKind::Function &&
            oneSymbol->kind != SymbolKind::GenericType &&
            oneSymbol->kind != SymbolKind::Struct &&
            oneSymbol->kind != SymbolKind::Enum &&
            (oneSymbol->overloads.size() != 1 || !oneSymbol->overloads[0]->hasFlag(OVERLOAD_COMPUTED_VALUE)) &&
            oneSymbol->ownerTable->scope->kind == ScopeKind::Struct &&
            !identifierRef->startScope)
        {
            isValid                  = false;
            auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
            for (const auto& dep : scopeHierarchyVars)
            {
                if (dep.scope->getFullName() == oneSymbol->ownerTable->scope->getFullName())
                {
                    isValid = true;
                    break;
                }
            }
        }

        p.remove = !isValid;
    }

    // Eliminate all matches tag as 'remove'
    for (size_t i = 0; i < dependentSymbols.size(); i++)
    {
        if (dependentSymbols[i].remove)
        {
            dependentSymbols[i] = dependentSymbols.back();
            dependentSymbols.pop_back();
            i--;
        }
    }

    return true;
}
