#include "pch.h"
#include "Generic.h"
#include "Ast.h"
#include "AstFlags.h"
#include "CommandLine.h"
#include "Scope.h"
#include "SemanticJob.h"

Job* Generic::end(SemanticContext* context, Job* job, SymbolName* symbol, AstNode* newNode, bool waitSymbol, const VectorMap<Utf8, GenericReplaceType>& replaceTypes)
{
    // Need to wait for the struct/function to be semantic resolved
    symbol->cptOverloads++;
    symbol->cptOverloadsInit++;
    symbol->nodes.push_back(newNode);

    if (waitSymbol && context->result == ContextResult::Done)
        Semantic::waitSymbolNoLock(job, symbol);

    // Run semantic on that struct/function
    const auto sourceFile = context->sourceFile;
    const auto newJob     = SemanticJob::newJob(job->dependentJob, sourceFile, newNode, false);

    // Store stack of instantiation contexts
    const auto srcCxt  = context;
    const auto destCxt = &newJob->context;
    destCxt->errCxtSteps.insert(destCxt->errCxtSteps.end(), srcCxt->errCxtSteps.begin(), srcCxt->errCxtSteps.end());

    // New context
    ErrorCxtStep expNode;
    expNode.node          = context->node;
    expNode.replaceTypes  = replaceTypes;
    const auto exportNode = expNode.node->extraPointer<AstNode>(ExtraPointerKind::ExportNode);
    if (exportNode)
        expNode.node = exportNode;
    expNode.type = ErrCxtStepKind::Generic;
    destCxt->errCxtSteps.push_back(expNode);

    return newJob;
}

void Generic::checkCanInstantiateGenericSymbol(SemanticContext* context, OneMatch& firstMatch)
{
    const auto symbol = firstMatch.symbolName;

    // Be sure number of overloads has not changed since then
    if (firstMatch.numOverloadsWhenChecked != symbol->overloads.size())
    {
        context->result = ContextResult::NewChildren;
        return;
    }

    if (firstMatch.numOverloadsInitWhenChecked != symbol->cptOverloadsInit)
    {
        context->result = ContextResult::NewChildren;
        return;
    }

    // Cannot instantiate if the type is incomplete
    Semantic::waitForGenericParameters(context, firstMatch);
}

bool Generic::instantiateGenericSymbol(SemanticContext* context, OneMatch& firstMatch, bool forStruct)
{
    const auto node              = context->node;
    auto&      matches           = context->cacheMatches;
    const auto symbol            = firstMatch.symbolName;
    auto       genericParameters = firstMatch.genericParameters;
    ScopedLock lk(symbol->mutex);

    // If we are inside a generic function (not instantiated), then we are done, we
    // cannot instantiate (can occur when evaluating function body of an incomplete short lambda)
    if (node->ownerFct && node->ownerFct->hasAstFlag(AST_IS_GENERIC))
        return true;

    checkCanInstantiateGenericSymbol(context, firstMatch);
    YIELD();

    if (forStruct)
    {
        // Be sure we have generic parameters if there's an automatic match
        if (!genericParameters && firstMatch.matchFlags & SymbolMatchContext::MATCH_GENERIC_AUTO)
        {
            SWAG_ASSERT(!firstMatch.genericParametersCallTypes.empty());
            const auto identifier         = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
            identifier->genericParameters = Ast::newFuncCallGenParams(nullptr, node);
            genericParameters             = identifier->genericParameters;
            for (int i = 0; i < static_cast<int>(firstMatch.genericParametersCallTypes.size()); i++)
            {
                const auto& param     = firstMatch.genericParametersCallTypes[i];
                const auto  callParam = Ast::newFuncCallParam(nullptr, genericParameters);
                callParam->typeInfo   = param.typeInfoReplace;
                if (param.fromNode)
                    callParam->token = param.fromNode->token;

                if (firstMatch.genericParametersCallValues[i])
                {
                    callParam->allocateComputedValue();
                    *callParam->computedValue() = *firstMatch.genericParametersCallValues[i];
                }
            }
        }

        // We can instantiate the struct because it's no more generic, and we have generic parameters to replace
        if (!node->hasAstFlag(AST_IS_GENERIC) && genericParameters)
        {
            bool alias = false;
            SWAG_CHECK(Generic::instantiateStruct(context, genericParameters, firstMatch, alias));
            if (alias)
            {
                const auto oneMatch      = context->getOneMatch();
                oneMatch->symbolOverload = firstMatch.symbolOverload;
                matches.push_back(oneMatch);
            }
        }

        // The new struct is no more generic without generic parameters.
        // It happens each time we reference a generic struct without generic parameters, like impl 'Array', @typeof(Array), self* Array or even
        // a variable type (in that case we will try later to instantiate it with default generic parameters).
        // So we match a generic struct as a normal match without instantiation (for now).
        else if (!node->hasAstFlag(AST_IS_GENERIC))
        {
            const auto oneMatch      = context->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->addAstFlag(AST_IS_GENERIC);
        }

        // The new struct is still generic and we have generic parameters
        else
        {
            SWAG_ASSERT(genericParameters);
            const auto oneMatch      = context->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->addAstFlag(AST_IS_GENERIC);

            // :DupGen
            // We generate a new struct with the wanted generic parameters to have those names for replacement.
            auto newStructType = castTypeInfo<TypeInfoStruct>(firstMatch.symbolOverload->typeInfo, TypeInfoKind::Struct);
            if (newStructType->genericParameters.size() == genericParameters->children.size() && !genericParameters->children.empty())
            {
                const auto typeWasForced = firstMatch.symbolOverload->typeInfo->clone();
                newStructType            = castTypeInfo<TypeInfoStruct>(typeWasForced, TypeInfoKind::Struct);
                for (size_t i = 0; i < genericParameters->children.size(); i++)
                {
                    newStructType->genericParameters[i]->name     = genericParameters->children[i]->typeInfo->name;
                    newStructType->genericParameters[i]->typeInfo = genericParameters->children[i]->typeInfo;
                }

                typeWasForced->forceComputeName();
                oneMatch->typeWasForced = typeWasForced;
            }
        }
    }
    else
    {
        SWAG_CHECK(Generic::instantiateFunction(context, genericParameters, firstMatch));
    }

    return true;
}

void Generic::setUserGenericTypeReplacement(SymbolMatchContext& context, VectorNative<TypeInfoParam*>& genericParameters)
{
    const int wantedNumGenericParams = static_cast<int>(genericParameters.size());
    const int numGenericParams       = static_cast<int>(context.genericParameters.size());
    if (!numGenericParams && !wantedNumGenericParams)
        return;

    context.genericParametersCallTypes.expand_clear(wantedNumGenericParams);
    context.genericParametersCallValues.expand_clear(wantedNumGenericParams);

    if (numGenericParams > wantedNumGenericParams)
    {
        context.badSignatureInfos.badSignatureNum1 = numGenericParams;
        context.badSignatureInfos.badSignatureNum2 = wantedNumGenericParams;
        context.result                             = MatchResult::TooManyGenericParameters;
        return;
    }

    VectorMap<Utf8, GenericReplaceType> m;
    for (int i = 0; i < numGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto  genType     = genericParameters[i]->typeInfo;
        const auto& genTypeName = genType->name;
        const auto  genNode     = context.genericParameters[i];

        if (!context.genericParametersCallTypes[i].typeInfoReplace)
        {
            GenericReplaceType st;
            st.typeInfoGeneric = genType;
            st.typeInfoReplace = genNode->typeInfo;
            st.fromNode        = genNode;

            context.genericReplaceTypes[genTypeName] = st;
            context.genericParametersCallTypes[i]    = st;
            context.genericReplaceValues[genName]    = genNode->safeComputedValue();
            context.genericParametersCallValues[i]   = genNode->safeComputedValue();
        }
        else
        {
            context.genericParametersCallTypes[i].typeInfoGeneric = genType;
            context.genericReplaceTypes[genTypeName]              = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]                 = context.genericParametersCallValues[i];

            // We have a new type replacement, so we must be sure that every other types registered in the
            // context.genericReplaceTypes are up to date too.
            // For example if type T is now s32, we must be sure that a potential *T or [..] T in the map will
            // be updated correspondingly.
            if (context.genericReplaceTypes.size() > 1)
            {
                m.clear();
                m[genTypeName] = context.genericParametersCallTypes[i];

                for (auto& v : context.genericReplaceTypes)
                {
                    if (!v.second.typeInfoGeneric)
                        continue;
                    if (v.first == genTypeName)
                        continue;
                    const auto typeInfoReplace = replaceGenericTypes(m, v.second.typeInfoGeneric);
                    if (typeInfoReplace != v.second.typeInfoGeneric)
                    {
                        v.second.typeInfoReplace = typeInfoReplace;
                    }
                }
            }
        }
    }

    for (auto i = numGenericParams; i < wantedNumGenericParams; i++)
    {
        const auto& genName     = genericParameters[i]->name;
        const auto  genType     = genericParameters[i]->typeInfo;
        const auto& genTypeName = genType->name;

        if (context.genericParametersCallTypes[i].typeInfoReplace)
        {
            context.genericReplaceTypes[genTypeName] = context.genericParametersCallTypes[i];
            context.genericReplaceValues[genName]    = context.genericParametersCallValues[i];
        }
        else
        {
            auto it = context.genericReplaceTypes.find(genTypeName);
            if (it != context.genericReplaceTypes.end())
                context.genericParametersCallTypes[i] = it->second;

            auto it1 = context.genericReplaceValues.find(genName);
            if (it1 != context.genericReplaceValues.end())
                context.genericParametersCallValues[i] = it1->second;
        }
    }

    for (int i = 0; i < wantedNumGenericParams; i++)
    {
        const auto genType                                    = genericParameters[i]->typeInfo;
        context.mapGenericTypeToIndex[genType->name]          = i;
        context.genericParametersCallTypes[i].typeInfoGeneric = genType;
    }
}

void Generic::setContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, const SymbolOverload* symOverload, MatchIdParamsFlags flags)
{
    const auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypeToIndex.clear();

    auto& toCheck = context->tmpNodes;
    toCheck.clear();

    // If we are inside a struct, then we can inherit the generic concrete types of that struct
    if (node->ownerStructScope)
        toCheck.push_back(node->ownerStructScope->owner);

    // If function A in a struct calls function B in the same struct, then we can inherit the match types of function A
    // when instantiating function B
    if (node->ownerFct && node->ownerStructScope && node->ownerFct->ownerStructScope == symOverload->node->ownerStructScope)
        toCheck.push_back(node->ownerFct);

    // We do not want function to deduce their generic type from context, as the generic type can be deduced from the
    // parameters
    if (node->ownerFct && !symOverload->typeInfo->isFuncAttr())
        toCheck.push_back(node->ownerFct);

    // Except for a second try
    if (node->ownerFct && symOverload->typeInfo->isFuncAttr() && flags.has(MIP_SECOND_GENERIC_TRY))
        toCheck.push_back(node->ownerFct);

    // With A.B form, we try to get generic parameters from A if they exist
    if (node->kind == AstNodeKind::Identifier)
    {
        const auto identifier = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef()->startScope)
            toCheck.push_back(identifier->identifierRef()->startScope->owner);
    }

    // Except that with using, B could be in fact in another struct than A.
    // In that case we have a dependentVar, so replace what needs to be replaced.
    // What a mess...
    if (oneTryMatch.dependentVarLeaf)
    {
        if (oneTryMatch.dependentVarLeaf->typeInfo && oneTryMatch.dependentVarLeaf->typeInfo->isStruct())
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(oneTryMatch.dependentVarLeaf->typeInfo, TypeInfoKind::Struct);
            toCheck.push_back(typeStruct->declNode);
        }
    }

    // Collect all
    for (const auto one : toCheck)
    {
        if (one->kind == AstNodeKind::FuncDecl)
        {
            const auto nodeFunc = castAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(nodeFunc->typeInfo, TypeInfoKind::FuncAttr);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeFunc->replaceTypes.size());
            for (const auto& oneReplace : typeFunc->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeFunc->replaceValues.size());
            for (const auto& oneReplace : typeFunc->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;
        }
        else if (one->kind == AstNodeKind::StructDecl)
        {
            const auto nodeStruct = castAst<AstStruct>(one, AstNodeKind::StructDecl);
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(nodeStruct->typeInfo, TypeInfoKind::Struct);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeStruct->replaceTypes.size());
            for (const auto& oneReplace : typeStruct->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeStruct->replaceValues.size());
            for (const auto& oneReplace : typeStruct->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;
        }
    }
}

Vector<Utf8> Generic::computeGenericParametersReplacement(const VectorMap<Utf8, GenericReplaceType>& replace)
{
    if (replace.empty())
        return {};

    Vector<Utf8> result;
    Utf8         remark;
    for (auto p : replace)
    {
        // Can occur in case of constants (like string for example)
        if (p.first == p.second.typeInfoReplace->getDisplayName())
            continue;
        if (p.first.length() > 2 && p.first[0] == '_' && p.first[1] == '_') // Generated name
            continue;

        Utf8 rem = "with ";
        rem += FMT("%s = %s", p.first.c_str(), p.second.typeInfoReplace->getDisplayNameC());
        rem.replace("struct ", "");

        if (remark.empty() || remark.length() + rem.length() + 2 < g_CommandLine.errorRightColumn)
        {
            if (!remark.empty())
                remark += ", ";
            remark += rem;
        }
        else
        {
            result.push_back(remark);
            remark.clear();
        }
    }

    if (!remark.empty())
        result.push_back(remark);

    return result;
}
