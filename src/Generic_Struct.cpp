#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Generic.h"
#include "SemanticJob.h"
#include "Symbol.h"
#include "TypeManager.h"

bool Generic::instantiateDefaultGenericVar(SemanticContext* context, AstVarDecl* node)
{
    if (node->typeInfo->isStruct() && node->type && node->type->kind == AstNodeKind::TypeExpression)
    {
        const auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            const auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            const auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
            if (nodeStruct->genericParameters)
            {
                const auto idRef      = CastAst<AstIdentifierRef>(typeExpr->identifier, AstNodeKind::IdentifierRef);
                const auto identifier = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
                if (!identifier->genericParameters)
                {
                    identifier->genericParameters = Ast::newFuncCallGenParams(context->sourceFile, identifier);
                    identifier->genericParameters->flags |= AST_NO_BYTECODE;

                    CloneContext cloneContext;
                    for (const auto p : nodeStruct->genericParameters->childs)
                    {
                        const auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                        if (!param->assignment)
                        {
                            const Diagnostic diag{node->sourceFile, node->type->token, FMT(Err(Err0557), typeExpr->identifier->resolvedSymbolName->name.c_str())};
                            return context->report(diag, Diagnostic::hereIs(typeExpr->identifier->resolvedSymbolOverload));
                        }

                        const auto child    = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->flags &= ~AST_IS_GENERIC;
                    identifier->flags &= ~AST_IS_GENERIC;
                    node->flags &= ~AST_IS_GENERIC;
                    node->type->flags &= ~AST_IS_GENERIC;

                    // Force the reevaluation of the variable and its childs
                    context->result     = ContextResult::NewChilds;
                    node->semanticState = AstNodeResolveState::Enter;
                    Ast::visit(node, [](AstNode* n)
                    {
                        n->semFlags &= ~SEMFLAG_ONCE;
                    });
                    return true;
                }
            }
        }
    }

    return context->report({node, FMT(Err(Err0086), node->typeInfo->getDisplayNameC())});
}

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneMatch& match, bool& alias)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report({node, FMT(Err(Err0301), node->token.ctext())}));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    const auto typeStruct = CastTypeInfo<TypeInfoStruct>(match.symbolOverload->typeInfo, match.symbolOverload->typeInfo->kind);
    Semantic::waitAllStructSpecialMethods(context->baseJob, typeStruct);
    YIELD();
    Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
    YIELD();

    // Clone original node
    const auto overload   = match.symbolOverload;
    const auto sourceNode = overload->node;

    // Can be a type alias
    // In that case, we need to retrieve the real struct
    const auto genericStructType = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, overload->typeInfo->kind);
    const auto sourceSymbol      = match.symbolName;
    SWAG_VERIFY(sourceNode->kind == AstNodeKind::StructDecl, context->report({node, node->token, FMT(Err(Err0297), node->token.ctext())}));

    // Make a new type
    const auto newType = CastTypeInfo<TypeInfoStruct>(genericStructType->clone(), genericStructType->kind);
    newType->removeGenericFlag();
    newType->fromGeneric = genericStructType;

    // Replace generic types in the struct generic parameters
    const auto sourceNodeStruct = CastAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
    SWAG_CHECK(replaceGenericParameters(context, true, false, newType->genericParameters, sourceNodeStruct->genericParameters->childs, genericParameters, match));

    // For a tuple, replace inside types with real ones
    if (newType->isTuple())
    {
        for (const auto param : newType->fields)
            param->typeInfo = Generic::replaceGenericTypes(match.genericReplaceTypes, param->typeInfo);
    }

    newType->forceComputeName();

    // Be sure that after the generic instantiation, we will not have a match.
    // If this is the case, then the corresponding instantiation is used instead of creating a new exact same one.
    // This can happen when deducing generic parameters.
    for (const auto& v : sourceSymbol->overloads)
    {
        if (v->typeInfo->name == newType->name)
        {
            TypeInfoStruct* t0 = (TypeInfoStruct*) v->typeInfo;
            TypeInfoStruct* t1 = (TypeInfoStruct*) newType;

            bool same = true;
            for (size_t i = 0; i < t0->genericParameters.size(); i++)
            {
                if (!t0->genericParameters[i]->typeInfo->isSame(t1->genericParameters[i]->typeInfo, CASTFLAG_EXACT))
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

    // :GenericConcreteAlias
    // Make all types concrete in case of simple aliases
    for (auto& val : cloneContext.replaceTypes | views::values)
    {
        val.typeInfoReplace = TypeManager::concreteType(val.typeInfoReplace, CONCRETE_ALIAS);
    }

    const auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    newType->scope              = structNode->scope;
    newType->declNode           = structNode;
    newType->replaceTypes       = cloneContext.replaceTypes;
    newType->replaceValues      = cloneContext.replaceValues;
    structNode->typeInfo        = newType;
    structNode->originalGeneric = sourceNode;

    // Replace generic values in the struct generic parameters
    SWAG_CHECK(replaceGenericParameters(context, false, true, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));

    const auto structJob = end(context, context->baseJob, sourceSymbol, structNode, true, cloneContext.replaceTypes);

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (const auto method : newType->methods)
    {
        if (method->declNode->isSpecialFunctionName())
        {
            auto specFunc = CastAst<AstFuncDecl>(method->declNode, AstNodeKind::FuncDecl);
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
    newType->cptRemainingInterfaces = (uint32_t) genericStructType->interfaces.size();
    for (const auto itf : genericStructType->interfaces)
    {
        auto typeItf = (TypeInfoParam*) itf->clone();
        newType->interfaces.push_back(typeItf);

        SWAG_ASSERT(itf->declNode);
        auto newItf       = itf->declNode->clone(cloneContext);
        typeItf->declNode = newItf;
        newItf->flags |= AST_FROM_GENERIC;
        structNode->extOwner()->nodesToFree.push_back(newItf);

        const auto implJob = SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, newItf, false);
        structJob->addDependentJob(implJob);
    }

    context->baseJob->jobsToAdd.push_back(structJob);
    return true;
}
