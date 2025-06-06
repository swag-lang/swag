#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Generic/Generic.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool Generic::instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node)
{
    if (node->typeInfo->isStruct() && node->type && node->type->is(AstNodeKind::TypeExpression))
    {
        const auto typeExpr = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            const auto nodeStruct = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
            if (nodeStruct->genericParameters)
            {
                const auto idRef      = castAst<AstIdentifierRef>(typeExpr->identifier, AstNodeKind::IdentifierRef);
                const auto identifier = castAst<AstIdentifier>(idRef->lastChild(), AstNodeKind::Identifier);
                if (!identifier->genericParameters)
                {
                    identifier->genericParameters = Ast::newFuncCallGenParams(nullptr, identifier);
                    identifier->genericParameters->addAstFlag(AST_NO_BYTECODE);

                    CloneContext cloneContext;
                    for (const auto p : nodeStruct->genericParameters->children)
                    {
                        const auto param = castAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                        if (!param->assignment)
                        {
                            const Diagnostic err{node->token.sourceFile, node->type->token, formErr(Err0082, typeExpr->identifier->resolvedSymbolName()->name.cstr())};
                            return context->report(err, Diagnostic::hereIs(typeExpr->identifier->resolvedSymbolOverload()));
                        }

                        const auto child    = Ast::newFuncCallParam(nullptr, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->removeAstFlag(AST_GENERIC);
                    identifier->removeAstFlag(AST_GENERIC);
                    node->removeAstFlag(AST_GENERIC);
                    node->type->removeAstFlag(AST_GENERIC);

                    // Force the reevaluation of the variable and its children
                    context->result     = ContextResult::NewChildren;
                    node->semanticState = AstNodeResolveState::Enter;
                    Ast::visit(node, [](AstNode* n) { n->removeSemFlag(SEMFLAG_ONCE); });
                    return true;
                }
            }
        }
    }

    return context->report({node, formErr(Err0719, node->typeInfo->getDisplayNameC())});
}

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneMatch& match, bool& alias)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report({node, formErr(Err0723, node->token.cstr())}));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(match.symbolOverload->typeInfo, match.symbolOverload->typeInfo->kind);
    Semantic::waitAllStructSpecialMethods(context->baseJob, typeStruct);
    YIELD();
    Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
    YIELD();

    // Clone original node
    const auto overload   = match.symbolOverload;
    const auto sourceNode = overload->node;

    // Can be a type alias
    // In that case, we need to retrieve the real struct
    const auto genericStructType = castTypeInfo<TypeInfoStruct>(overload->typeInfo, overload->typeInfo->kind);
    const auto sourceSymbol      = match.symbolName;
    SWAG_VERIFY(sourceNode->is(AstNodeKind::StructDecl), context->report({node, node->token, formErr(Err0080, node->token.cstr())}));

    // Make a new type
    const auto newType = castTypeInfo<TypeInfoStruct>(genericStructType->clone(), genericStructType->kind);
    newType->removeGenericFlag();
    newType->fromGeneric = genericStructType;

    // Replace generic types in the struct generic parameters
    const auto sourceNodeStruct = castAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
    SWAG_CHECK(replaceGenericParameters(match, newType->genericParameters, sourceNodeStruct->genericParameters->children, genericParameters, true, false));

    // For a tuple, replace inside types with real ones
    if (newType->isTuple())
    {
        for (const auto param : newType->fields)
            param->typeInfo = replaceGenericTypes(match.genericReplaceTypes, param->typeInfo);
    }

    newType->forceComputeName();

    // Be sure that after the generic instantiation, we will not have a match.
    // If this is the case, then the corresponding instantiation is used instead of creating a new exact same one.
    // This can happen when deducing generic parameters.
    for (const auto& v : sourceSymbol->overloads)
    {
        if (v->typeInfo->name == newType->name)
        {
            const auto t0 = castTypeInfo<TypeInfoStruct>(v->typeInfo);
            const auto t1 = castTypeInfo<TypeInfoStruct>(newType);

            bool same = true;
            for (uint32_t i = 0; i < t0->genericParameters.size(); i++)
            {
                if (!t0->genericParameters[i]->typeInfo->isSame(t1->genericParameters[i]->typeInfo, CAST_FLAG_EXACT))
                {
                    same = false;
                    break;
                }
            }

            if (same)
            {
                alias                = true;
                match.symbolOverload = v;
                return true;
            }
        }
    }

    CloneContext cloneContext;
    cloneContext.replaceTypes  = match.genericReplaceTypes;
    cloneContext.replaceValues = match.genericReplaceValues;

    // Add the struct type replacement now, in case the struct has a field to replace
    if (!genericStructType->isTuple())
    {
        GenericReplaceType st;
        st.typeInfoGeneric                                  = genericStructType;
        st.typeInfoReplace                                  = newType;
        st.fromNode                                         = overload->node;
        cloneContext.replaceTypes[overload->typeInfo->name] = st;
    }

    // @GenericConcreteAlias
    // Make all types concrete in case of simple aliases
    for (auto& val : cloneContext.replaceTypes | std::views::values)
    {
        val.typeInfoReplace = TypeManager::concreteType(val.typeInfoReplace, CONCRETE_ALIAS);
    }

    const auto structNode = castAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->addAstFlag(AST_FROM_GENERIC);
    structNode->content->removeAstFlag(AST_NO_SEMANTIC);
    Ast::addChildBack(sourceNode->parent, structNode);

    newType->scope               = structNode->scope;
    newType->declNode            = structNode;
    newType->replaceTypes        = cloneContext.replaceTypes;
    newType->replaceValues       = cloneContext.replaceValues;
    structNode->typeInfo         = newType;
    structNode->originalGeneric  = sourceNode;
    structNode->instantiatedFrom = node;

    // Replace generic values in the struct generic parameters
    SWAG_CHECK(replaceGenericParameters(match, newType->genericParameters, structNode->genericParameters->children, genericParameters, false, true));

    const auto structJob = end(context, context->baseJob, sourceSymbol, structNode, true, cloneContext.replaceTypes);

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (const auto method : newType->methods)
    {
        if (method->declNode->isSpecialFunctionName())
        {
            auto specFunc = castAst<AstFuncDecl>(method->declNode, AstNodeKind::FuncDecl);
            if (specFunc != genericStructType->opUserDropFct &&
                specFunc != genericStructType->opUserPostCopyFct &&
                specFunc != genericStructType->opUserPostMoveFct &&
                !specFunc->genericParameters)
            {
                instantiateSpecialFunc(context, structJob, cloneContext, &specFunc);
            }
        }
    }

    // Instantiate generic interfaces
    structNode->allocateExtension(ExtensionKind::Owner);
    cloneContext.ownerStructScope   = structNode->scope;
    newType->cptRemainingInterfaces = genericStructType->interfaces.size();
    for (const auto itf : genericStructType->interfaces)
    {
        auto typeItf = itf->clone();
        newType->interfaces.push_back(typeItf);

        SWAG_ASSERT(itf->declNode);
        auto newItf       = itf->declNode->clone(cloneContext);
        typeItf->declNode = newItf;
        newItf->addAstFlag(AST_FROM_GENERIC);
        structNode->extOwner()->nodesToFree.push_back(newItf);

        const auto implJob = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, newItf, false);
        structJob->addDependentJob(implJob);
    }

    context->baseJob->jobsToAdd.push_back(structJob);
    return true;
}
