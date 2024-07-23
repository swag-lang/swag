#include "pch.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

namespace
{
    void cleanMatches(VectorNative<OneMatch*>& matches)
    {
        for (uint32_t i = 0; i < matches.size(); i++)
        {
            if (matches[i]->remove)
            {
                matches[i] = matches.back();
                matches.pop_back();
                i--;
            }
        }
    }

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

bool Semantic::filterMatchesDirect(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    const auto node         = context->node;
    const auto countMatches = matches.size();

    // Sometimes we don't care about multiple symbols with the same name
    if (countMatches > 1 && node->parent && node->parent->parent)
    {
        const auto grandParent = node->parent->parent;
        if (grandParent->is(AstNodeKind::IntrinsicDefined) ||
            (grandParent->is(AstNodeKind::IntrinsicProp) && grandParent->token.is(TokenId::IntrinsicNameOf)))
        {
            matches.count = 1;
            return true;
        }
    }

    for (uint32_t i = 0; i < countMatches; i++)
    {
        const auto curMatch = matches[i];
        const auto over     = curMatch->symbolOverload;
        const auto overSym  = over->symbol;

        // Take care of where/check
        if (overSym->is(SymbolKind::Function) &&
            !context->node->hasAstFlag(AST_IN_VALIDIF) &&
            !context->node->hasAttribute(ATTRIBUTE_MATCH_WHERE_OFF))
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
        if (overSym->is(SymbolKind::Function) &&
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
    for (uint32_t i = 0; i < countMatches; i++)
    {
        const auto curMatch = matches[i];
        const auto over     = curMatch->symbolOverload;
        const auto overSym  = over->symbol;

        // In case of an alias, we take the first one, which should be the 'closest' one.
        // Not sure this is true, perhaps one day will have to change the way we find it.
        if (overSym->name.find(g_LangSpec->name_atalias) == 0)
        {
            for (uint32_t j = 0; j < countMatches; j++)
            {
                if (i != j)
                    matches[j]->remove = true;
            }

            break;
        }

        // Priority to UFCS
        if (!curMatch->ufcs)
        {
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->symbolOverload->hasFlag(OVERLOAD_IMPL_IN_STRUCT))
                {
                    // If interface name is alone, then we take in priority the interface definition, not the impl block
                    if (node == node->parent->firstChild())
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
            for (uint32_t j = 0; j < countMatches; j++)
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
            for (uint32_t j = 0; j < countMatches; j++)
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
                for (uint32_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->symbol->is(SymbolKind::Function))
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
                for (uint32_t j = 0; j < countMatches; j++)
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
        if (curMatch->symbolOverload->symbol->is(SymbolKind::Namespace))
        {
            for (uint32_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->symbolOverload->symbol->isNot(SymbolKind::Namespace))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Closure variable has a priority over a "out of scope" one
        if (curMatch->symbolOverload->symbol->is(SymbolKind::Variable))
        {
            for (uint32_t j = 0; j < countMatches; j++)
            {
                if (i == j)
                    continue;
                if (matches[j]->symbolOverload->symbol->is(SymbolKind::Variable) && matches[j]->symbolOverload->hasFlag(OVERLOAD_VAR_CAPTURE))
                {
                    if (curMatch->symbolOverload->node->ownerScope->isParentOf(matches[j]->symbolOverload->node->ownerScope))
                    {
                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // If we didn't match with UFCS, then priority to a match that do not start with 'self'
        if (!curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc0->parameters.empty() && typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (uint32_t j = 0; j < countMatches; j++)
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

        // If we did match with UFCS, then priority to a match that starts with 'self'
        if (curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters.empty() || !typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (uint32_t j = 0; j < countMatches; j++)
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

        // 2 UFCS : priority to the first parameter that is not const
        if (curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            const auto typeFunc0 = castTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters[0]->typeInfo->isConst())
            {
                for (uint32_t j = 0; j < countMatches; j++)
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
        if (m->symbolOverload->symbol->isNot(SymbolKind::Function))
            continue;

        m->coerceCast = 0;
        for (const auto flags : m->solvedCastFlags)
        {
            if (flags.has(CAST_RESULT_COERCE) && !flags.has(CAST_RESULT_UNTYPED_CONVERT))
                m->coerceCast += m->solvedCastFlags.size();
            else if (flags.has(CAST_RESULT_UNTYPED_CONVERT))
                m->coerceCast += 1;
            else if (flags.has(CAST_RESULT_CONST_COERCE))
                m->coerceCast += 1;
        }
    }
}

bool Semantic::filterMatchesCoerceCast(SemanticContext*, VectorNative<OneMatch*>& matches)
{
    if (matches.size() <= 1)
        return true;

    std::ranges::sort(matches, [](const OneMatch* x, const OneMatch* y) { return x->coerceCast < y->coerceCast; });

    const auto prio = matches[0]->coerceCast;
    for (const auto m : matches)
    {
        if (m->symbolOverload->symbol->isNot(SymbolKind::Function))
            continue;

        if (m->coerceCast > prio)
            m->remove = true;
    }

    cleanMatches(matches);
    return true;
}

bool Semantic::filterGenericMatches(const SemanticContext* context, VectorNative<OneMatch*>& matches, VectorNative<OneMatch*>& genMatches)
{
    // Remove all matches with more conversions than the generic match
    if (!matches.empty() && genMatches.size() == 1)
    {
        for (uint32_t i = 0; i < matches.size(); i++)
        {
            if (!matches[i]->castFlagsResult.has(CAST_RESULT_AUTO_OP_CAST))
            {
                if (matches[i]->coerceCast > genMatches[0]->coerceCast)
                {
                    matches[i] = matches.back();
                    matches.pop_back();
                    i--;
                }
            }
        }
    }

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
        for (uint32_t i = 0; i < genMatches.size(); i++)
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
        for (uint32_t i = 0; i < genMatches.size(); i++)
        {
            if (genMatches[i]->genericReplaceTypes.size() > bestS)
            {
                bestS = genMatches[i]->genericReplaceTypes.size();
                bestI = i;
            }
        }

        for (uint32_t i = 0; i < genMatches.size(); i++)
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

    // priority to UFCS
    for (uint32_t i = 0; i < genMatches.size(); i++)
    {
        if (!genMatches[i]->ufcs)
        {
            for (uint32_t j = 0; j < genMatches.size(); j++)
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
    for (uint32_t i = 0; i < genMatches.size(); i++)
    {
        if (genMatches[i]->castFlagsResult.has(CAST_RESULT_STRUCT_CONVERT))
        {
            for (uint32_t j = 0; j < genMatches.size(); j++)
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
        newGenericMatches.reserve(genMatches.size());
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

    for (uint32_t i = 0; i < matches.size(); i++)
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
        if (over->symbol->is(SymbolKind::Function))
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
    auto&      dependentSymbols = context->cacheSymbolsMatch;

    if (dependentSymbols.size() == 1)
        return true;

    for (auto& p : dependentSymbols)
    {
        const auto oneSymbol = p.symbol;
        if (p.remove)
            continue;

        // A variable which is name as a function...
        if (!node->callParameters &&
            oneSymbol->is(SymbolKind::Function) &&
            !isFunctionButNotACall(context, node, oneSymbol))
        {
            p.remove = true;
            continue;
        }

        // A function call which is named as a variable, and the variable is not a lambda
        if (node->callParameters &&
            oneSymbol->is(SymbolKind::Variable))
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
        if (!p.altFlags.has(COLLECTED_SCOPE_STRUCT_USING) && p.symbol->is(SymbolKind::GenericType))
        {
            for (auto& p1 : dependentSymbols)
            {
                if (p1.altFlags.has(COLLECTED_SCOPE_STRUCT_USING) && p1.symbol->is(SymbolKind::GenericType))
                    p1.remove = true;
            }
        }

        // Reference to a variable inside a struct, without a direct explicit reference
        if (oneSymbol->isNot(SymbolKind::Function) &&
            oneSymbol->isNot(SymbolKind::GenericType) &&
            oneSymbol->isNot(SymbolKind::Struct) &&
            oneSymbol->isNot(SymbolKind::Enum) &&
            (oneSymbol->overloads.size() != 1 || !oneSymbol->overloads[0]->hasFlag(OVERLOAD_COMPUTED_VALUE)) &&
            oneSymbol->ownerTable->scope->is(ScopeKind::Struct) &&
            !identifierRef->startScope)
        {
            p.remove = true;

            auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
            for (const auto& dep : scopeHierarchyVars)
            {
                if (dep.scope->getFullName() == oneSymbol->ownerTable->scope->getFullName())
                {
                    p.remove = false;
                    break;
                }
            }
        }
    }

    // Eliminate all matches tag as 'remove'
    for (uint32_t i = 0; i < dependentSymbols.size(); i++)
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

bool Semantic::filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches, VectorNative<OneMatch*>& genericMatches, VectorNative<OneMatch*>& genericMatchesSI)
{
    if (matches.size() + genericMatches.size() + genericMatchesSI.size() > 1)
    {
        computeMatchesCoerceCast(matches);
        computeMatchesCoerceCast(genericMatches);
        computeMatchesCoerceCast(genericMatchesSI);
    }

    SWAG_CHECK(filterMatchesDirect(context, matches));
    YIELD();

    SWAG_CHECK(filterMatchesCompare(context, matches));
    SWAG_CHECK(filterMatchesInContext(context, matches));
    SWAG_CHECK(filterMatchesCoerceCast(context, matches));

    SWAG_CHECK(filterGenericMatches(context, matches, genericMatches));
    SWAG_CHECK(filterGenericMatches(context, matches, genericMatchesSI));
    SWAG_CHECK(filterMatchesCoerceCast(context, genericMatches));
    SWAG_CHECK(filterMatchesCoerceCast(context, genericMatchesSI));

    SWAG_ASSERT(context->result == ContextResult::Done);
    return true;
}
