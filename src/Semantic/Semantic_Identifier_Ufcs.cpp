#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"

bool Semantic::canTryUFCS(SemanticContext* context, TypeInfoFuncAttr* typeFunc, AstNode* nodeUFCS, bool nodeIsExplicit)
{
    if (!nodeUFCS || typeFunc->parameters.empty())
        return false;

    // Be sure the identifier we want to use in UFCS emits code, otherwise we cannot use it.
    // This can happen if we have already changed the AST, but the nodes are reevaluated later
    // (because of an inline for example)
    if (nodeUFCS->hasAstFlag(AST_NO_BYTECODE))
        return false;

    // If we have an explicit node, then we can try. Anyway we will also try without...
    if (nodeIsExplicit)
        return true;

    // Compare first function parameter with nodeUFCS type.
    const bool cmpTypeUFCS = TypeManager::makeCompatibles(context,
                                                          typeFunc->parameters[0]->typeInfo,
                                                          nodeUFCS->typeInfo,
                                                          nullptr,
                                                          nodeUFCS,
                                                          CAST_FLAG_JUST_CHECK | CAST_FLAG_UFCS | CAST_FLAG_ACCEPT_PENDING);
    if (context->result != ContextResult::Done)
        return false;

    // In case nodeUFCS is not explicit (using var), then be sure that first parameter type matches.
    if (!cmpTypeUFCS)
        return false;

    return true;
}

bool Semantic::getUFCS(SemanticContext* context, const AstIdentifierRef* identifierRef, AstIdentifier* node, const SymbolOverload* overload, AstNode** firstParamUFCS)
{
    const auto symbol = overload->symbol;

    bool canDoUFCS = false;
    if (symbol->is(SymbolKind::Function))
        canDoUFCS = true;
    if (symbol->is(SymbolKind::Variable) && overload->typeInfo->isLambdaClosure())
        canDoUFCS = node->callParameters;
    if (isFunctionButNotACall(context, node, symbol))
        canDoUFCS = false;

    if (!canDoUFCS)
        return true;

    // If a variable is defined just before a function call, then this can be an UFCS (uniform function call syntax)
    const auto idRefSymbolName = identifierRef->resolvedSymbolName();
    if (idRefSymbolName && identifierRef->previousNode)
    {
        bool canTry = false;

        // Before was a variable
        if (idRefSymbolName->is(SymbolKind::Variable))
            canTry = true;
            // Before was an enum value
        else if (idRefSymbolName->is(SymbolKind::EnumValue))
            canTry = true;
            // Before was a function call
        else if (idRefSymbolName->is(SymbolKind::Function) &&
                 identifierRef->previousNode &&
                 identifierRef->previousNode->hasAstFlag(AST_FUNC_CALL))
            canTry = true;
            // Before was an inlined function call
        else if (idRefSymbolName->is(SymbolKind::Function) &&
                 identifierRef->previousNode &&
                 identifierRef->previousNode->is(AstNodeKind::Identifier) &&
                 !identifierRef->previousNode->children.empty() &&
                 identifierRef->previousNode->firstChild()->is(AstNodeKind::FuncCallParams) &&
                 identifierRef->previousNode->lastChild()->is(AstNodeKind::Inline))
            canTry = true;

        if (canTry)
        {
            SWAG_ASSERT(identifierRef->previousNode);
            if (!node->callParameters)
            {
                Diagnostic err{node, formErr(Err0394, Naming::kindName(overload).cstr(), node->token.cstr())};
                err.addNote(Diagnostic::hereIs(overload));
                return context->report(err);
            }

            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
            canTry              = canTryUFCS(context, typeFunc, identifierRef->previousNode, true);
            YIELD();
            if (canTry)
                *firstParamUFCS = identifierRef->previousNode;
        }
    }

    if (symbol->is(SymbolKind::Variable))
    {
        bool fine = false;

        if (idRefSymbolName &&
            idRefSymbolName->is(SymbolKind::Function) &&
            identifierRef->previousNode &&
            identifierRef->previousNode->is(AstNodeKind::Identifier) &&
            identifierRef->previousNode->hasAstFlag(AST_INLINED))
        {
            fine = true;
        }

        if (idRefSymbolName &&
            idRefSymbolName->is(SymbolKind::Function) &&
            identifierRef->previousNode &&
            identifierRef->previousNode->hasAstFlag(AST_FUNC_CALL))
        {
            fine = true;
        }

        if (!fine)
        {
            if (idRefSymbolName && idRefSymbolName->isNot(SymbolKind::Variable))
            {
                const auto subNode = identifierRef->previousNode ? identifierRef->previousNode : node;
                Diagnostic err{subNode, subNode->token, formErr(Err0650, idRefSymbolName->name.cstr(), Naming::aKindName(idRefSymbolName->kind).cstr())};
                err.addNote(node->token, "this is a lambda expression");
                return context->report(err);
            }
        }
    }

    return true;
}

bool Semantic::setFirstParamUFCS(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match)
{
    const auto symbol       = match.symbolOverload->symbol;
    const auto dependentVar = match.dependentVar;
    const auto node         = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);

    const auto fctCallParam        = Ast::newNode<AstFuncCallParam>(AstNodeKind::FuncCallParam, nullptr, nullptr);
    fctCallParam->token.sourceFile = node->token.sourceFile;
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(nullptr, node);

    SWAG_CHECK(checkIsConcrete(context, identifierRef->previousNode));

    // Insert variable in first position. Need to update child
    // rank of all brothers.
    node->callParameters->children.push_front(fctCallParam);

    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = identifierRef->previousNode->typeInfo;
    fctCallParam->token    = identifierRef->previousNode->token;
    fctCallParam->inheritTokenLocation(node->token);
    fctCallParam->byteCodeFct = ByteCodeGen::emitFuncCallParam;
    fctCallParam->inheritOwners(node->callParameters);
    fctCallParam->addAstFlag(AST_TO_UFCS | AST_GENERATED);
    fctCallParam->inheritAstFlagsOr(node, AST_IN_MIXIN);

    // If this is a closure, then parameter index 0 is for the embedded struct.
    // So UFCS will have index 1.
    if (match.symbolOverload->typeInfo->isClosure())
        fctCallParam->indexParam = 1;

    SWAG_ASSERT(!match.solvedParameters.empty());
    SWAG_ASSERT(match.solvedParameters[0]->index == 0);
    fctCallParam->resolvedParameter = match.solvedParameters[0];

    const auto idRef = Ast::newIdentifierRef(nullptr, fctCallParam);
    if (symbol->is(SymbolKind::Variable))
    {
        if (identifierRef->previousNode && identifierRef->previousNode->hasAstFlag(AST_FUNC_CALL))
        {
            // Function that returns an interface, used as an UFCS.
            // Ex: var cfg = @compiler().getBuildCfg()
            // @SpecUFCSNode
            identifierRef->previousNode->addAstFlag(AST_TO_UFCS);
            fctCallParam->specUFCSNode = identifierRef->previousNode;
            const auto id              = Ast::newIdentifier(idRef, form("__8tmp_%d", g_UniqueID.fetch_add(1)), nullptr, idRef);
            id->addAstFlag(AST_NO_BYTECODE);
        }
        else
        {
            // Call from a lambda, on a variable: we need to keep the original variable, and put the UFCS one in its own identifier-ref
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            for (const auto child : identifierRef->children)
            {
                const auto copyChild = Ast::cloneRaw(child, idRef);

                // We want to generate bytecode for the expression on the left only if the lambda is dereferenced from a struct/itf
                // Otherwise the left expression is only used for scoping
                if (!identifierRef->previousScope ||
                    identifierRef->previousScope != match.symbolOverload->node->ownerStructScope)
                {
                    child->addAstFlag(AST_NO_BYTECODE);
                    copyChild->addAstFlag(AST_UFCS_FCT);
                }

                if (child == identifierRef->previousNode)
                {
                    copyChild->addAstFlag(AST_TO_UFCS);
                    break;
                }
            }
        }
    }

    // UFCS on a function call
    else
    {
        identifierRef->previousNode->addAstFlag(AST_UFCS_FCT);

        // If the UFCS comes from a using var, then we must make a reference to that using var as
        // the first call parameter of the following function
        if (dependentVar == identifierRef->previousNode)
        {
            for (const auto child : dependentVar->children)
            {
                const auto copyChild = Ast::newIdentifier(idRef, child->token.text.empty() ? dependentVar->token.text : child->token.text, nullptr, idRef);
                copyChild->inheritOwners(fctCallParam);
                copyChild->inheritAstFlagsOr(idRef, AST_IN_MIXIN);
                if (!child->resolvedSymbolOverload() || dependentVar->hasAstFlag(AST_IN_CAPTURE_BLOCK))
                {
                    copyChild->setResolvedSymbol(dependentVar->resolvedSymbolOverload()->symbol, dependentVar->resolvedSymbolOverload());
                    copyChild->typeInfo = dependentVar->typeInfo;

                    // In case of a parameter of an inlined function, we will have to find the real OVERLOAD_VAR_INLINE variable
                    // @InlineUsingParam
                    if (dependentVar->is(AstNodeKind::FuncDeclParam) && copyChild->hasOwnerInline() && copyChild->ownerInline()->parametersScope)
                    {
                        // Really, but REALLY not sure about that fix!! Seems really like a hack...
                        if (!copyChild->isSameStackFrame(copyChild->resolvedSymbolOverload()))
                        {
                            const auto sym = copyChild->ownerInline()->parametersScope->symTable.find(dependentVar->resolvedSymbolOverload()->symbol->name);
                            if (sym)
                            {
                                ScopedLock lk(sym->mutex);
                                SWAG_ASSERT(sym->overloads.size() == 1);
                                copyChild->setResolvedSymbol(sym->overloads[0]->symbol, sym->overloads[0]);
                                copyChild->typeInfo = copyChild->resolvedSymbolOverload()->typeInfo;
                            }
                        }
                    }
                }
                else
                {
                    copyChild->setResolvedSymbol(child->resolvedSymbolOverload()->symbol, child->resolvedSymbolOverload());
                    copyChild->typeInfo = child->typeInfo;
                }

                copyChild->byteCodeFct = ByteCodeGen::emitIdentifier;
                copyChild->addAstFlag(AST_TO_UFCS | AST_L_VALUE | AST_UFCS_FCT);
            }
        }
        else
        {
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            // We copy instead of moving in case this will be evaluated another time (inline)
            for (const auto child : identifierRef->children)
            {
                const auto copyChild = Ast::cloneRaw(child, idRef);
                child->addAstFlag(AST_NO_BYTECODE);
                if (child == identifierRef->previousNode)
                {
                    copyChild->addAstFlag(AST_TO_UFCS);
                    copyChild->addAstFlag(AST_UFCS_FCT);
                    break;
                }
            }
        }
    }

    idRef->inheritAstFlagsAnd(AST_CONST_EXPR);
    fctCallParam->inheritAstFlagsAnd(AST_CONST_EXPR);

    identifierRef->previousNode->addAstFlag(AST_FROM_UFCS);
    return true;
}
