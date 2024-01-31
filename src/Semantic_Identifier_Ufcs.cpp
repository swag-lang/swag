#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "SemanticJob.h"
#include "TypeManager.h"

bool Semantic::canTryUfcs(SemanticContext* context, TypeInfoFuncAttr* typeFunc, AstFuncCallParams* parameters, AstNode* ufcsNode, bool nodeIsExplicit)
{
    if (!ufcsNode || typeFunc->parameters.empty())
        return false;

    // Be sure the identifier we want to use in ufcs emits code, otherwise we cannot use it.
    // This can happen if we have already changed the ast, but the nodes are reavaluated later
    // (because of an inline for example)
    if (ufcsNode->flags & AST_NO_BYTECODE)
        return false;

    // If we have an explicit node, then we can try. Anyway we will also try without...
    if (nodeIsExplicit)
        return true;

    // Compare first function parameter with ufcsNode type.
    bool cmpTypeUfcs = TypeManager::makeCompatibles(context,
                                                    typeFunc->parameters[0]->typeInfo,
                                                    ufcsNode->typeInfo,
                                                    nullptr,
                                                    ufcsNode,
                                                    CASTFLAG_JUST_CHECK | CASTFLAG_UFCS);
    if (context->result != ContextResult::Done)
        return false;

    // In case ufcsNode is not explicit (using var), then be sure that first parameter type matches.
    if (!cmpTypeUfcs)
        return false;

    return true;
}

bool Semantic::getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam)
{
    auto symbol = overload->symbol;

    bool canDoUfcs = false;
    if (symbol->kind == SymbolKind::Function)
        canDoUfcs = true;
    if (symbol->kind == SymbolKind::Variable && overload->typeInfo->isLambdaClosure())
        canDoUfcs = node->callParameters;
    if (isFunctionButNotACall(context, node, symbol))
        canDoUfcs = false;

    if (!canDoUfcs)
        return true;

    // If a variable is defined just before a function call, then this can be an UFCS (uniform function call syntax)
    if (identifierRef->resolvedSymbolName && identifierRef->previousResolvedNode)
    {
        bool canTry = false;

        // Before was a variable
        if (identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
            canTry = true;
        // Before was an enum value
        else if (identifierRef->resolvedSymbolName->kind == SymbolKind::EnumValue)
            canTry = true;
        // Before was a function call
        else if (identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
                 identifierRef->previousResolvedNode &&
                 identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
            canTry = true;
        // Before was an inlined function call
        else if (identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
                 identifierRef->previousResolvedNode &&
                 identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier &&
                 identifierRef->previousResolvedNode->childs.size() &&
                 identifierRef->previousResolvedNode->childs.front()->kind == AstNodeKind::FuncCallParams &&
                 identifierRef->previousResolvedNode->childs.back()->kind == AstNodeKind::Inline)
            canTry = true;

        if (canTry)
        {
            SWAG_ASSERT(identifierRef->previousResolvedNode);
            if (!node->callParameters)
            {
                Diagnostic diag{node, Fmt(Err(Err0540), Naming::kindName(overload).c_str())};
                auto       note = Diagnostic::hereIs(overload);
                return context->report(diag, note);
            }

            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
            canTry        = canTryUfcs(context, typeFunc, node->callParameters, identifierRef->previousResolvedNode, true);
            YIELD();
            if (canTry)
                *ufcsFirstParam = identifierRef->previousResolvedNode;
        }
    }

    if (symbol->kind == SymbolKind::Variable)
    {
        bool fine = false;

        if (identifierRef->resolvedSymbolName &&
            identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
            identifierRef->previousResolvedNode &&
            identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier &&
            identifierRef->previousResolvedNode->flags & AST_INLINED)
        {
            fine = true;
        }

        if (identifierRef->resolvedSymbolName &&
            identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
            identifierRef->previousResolvedNode &&
            identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
        {
            fine = true;
        }

        if (!fine)
        {
            if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind != SymbolKind::Variable)
            {
                auto       subNode = identifierRef->previousResolvedNode ? identifierRef->previousResolvedNode : node;
                Diagnostic diag{subNode, subNode->token, Fmt(Err(Err0317), identifierRef->resolvedSymbolName->name.c_str(), Naming::aKindName(identifierRef->resolvedSymbolName->kind).c_str())};
                diag.addRange(node->token, Nte(Nte0159));
                return context->report(diag);
            }
        }
    }

    return true;
}

bool Semantic::ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match)
{
    auto symbol       = match.symbolOverload->symbol;
    auto dependentVar = match.dependentVar;
    auto node         = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    auto fctCallParam = Ast::newNode<AstFuncCallParam>(nullptr, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(context->sourceFile, node);

    SWAG_CHECK(checkIsConcrete(context, identifierRef->previousResolvedNode));

    // Insert variable in first position. Need to update child
    // rank of all brothers.
    node->callParameters->childs.push_front(fctCallParam);

    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = identifierRef->previousResolvedNode->typeInfo;
    fctCallParam->token    = identifierRef->previousResolvedNode->token;
    fctCallParam->inheritTokenLocation(node);
    fctCallParam->byteCodeFct = ByteCodeGen::emitFuncCallParam;
    fctCallParam->inheritOwners(node->callParameters);
    fctCallParam->flags |= AST_TO_UFCS | AST_GENERATED;
    fctCallParam->inheritOrFlag(node, AST_IN_MIXIN);

    // If this is a closure, then parameter index 0 is for the embedded struct.
    // So ufcs will have index 1.
    if (match.symbolOverload->typeInfo->isClosure())
        fctCallParam->indexParam = 1;

    SWAG_ASSERT(match.solvedParameters.size() > 0);
    SWAG_ASSERT(match.solvedParameters[0]->index == 0);
    fctCallParam->resolvedParameter = match.solvedParameters[0];

    auto idRef = Ast::newIdentifierRef(node->sourceFile, fctCallParam);
    if (symbol->kind == SymbolKind::Variable)
    {
        if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
        {
            // Function that returns an interface, used as an ufcs.
            // Ex: var cfg = @compiler().getBuildCfg()
            // :SpecUfcsNode
            identifierRef->previousResolvedNode->flags |= AST_TO_UFCS;
            fctCallParam->specUfcsNode = identifierRef->previousResolvedNode;
            auto id                    = Ast::newIdentifier(node->sourceFile, Fmt("__8tmp_%d", g_UniqueID.fetch_add(1)), idRef, idRef);
            id->flags |= AST_NO_BYTECODE;
        }
        else
        {
            // Call from a lambda, on a variable : we need to keep the original variable, and put the UFCS one in its own identifierref
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            for (auto child : identifierRef->childs)
            {
                auto copyChild = Ast::cloneRaw(child, idRef);

                // We want to generate bytecode for the expression on the left only if the lambda is dereferenced from a struct/itf
                // Otherwise the left expression is only used for scoping
                if (!identifierRef->startScope ||
                    identifierRef->startScope != match.symbolOverload->node->ownerStructScope)
                {
                    child->flags |= AST_NO_BYTECODE;
                    copyChild->flags |= AST_UFCS_FCT;
                }

                if (child == identifierRef->previousResolvedNode)
                {
                    copyChild->flags |= AST_TO_UFCS;
                    break;
                }
            }
        }
    }
    else
    {
        identifierRef->previousResolvedNode->flags |= AST_UFCS_FCT;

        // If ufcs comes from a using var, then we must make a reference to the using var in
        // the first call parameter
        if (dependentVar == identifierRef->previousResolvedNode)
        {
            for (auto child : dependentVar->childs)
            {
                auto copyChild = Ast::newIdentifier(node->sourceFile, child->token.text.empty() ? dependentVar->token.text : child->token.text, idRef, idRef);
                copyChild->inheritOwners(fctCallParam);
                copyChild->inheritOrFlag(idRef, AST_IN_MIXIN);
                if (!child->resolvedSymbolOverload)
                {
                    copyChild->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
                    copyChild->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
                    copyChild->typeInfo               = dependentVar->typeInfo;

                    // In case of a parameter of an inlined function, we will have to find the real OVERLOAD_VAR_INLINE variable
                    // :InlineUsingParam
                    if (dependentVar->kind == AstNodeKind::FuncDeclParam && copyChild->ownerInline && copyChild->ownerInline->parametersScope)
                    {
                        // Really, but REALLY not sure about that fix !! Seems really like a hack...
                        if (!copyChild->isSameStackFrame(copyChild->resolvedSymbolOverload))
                        {
                            auto sym = copyChild->ownerInline->parametersScope->symTable.find(dependentVar->resolvedSymbolOverload->symbol->name);
                            if (sym)
                            {
                                ScopedLock lk(sym->mutex);
                                SWAG_ASSERT(sym->overloads.size() == 1);
                                copyChild->resolvedSymbolOverload = sym->overloads[0];
                                copyChild->resolvedSymbolName     = copyChild->resolvedSymbolOverload->symbol;
                                copyChild->typeInfo               = copyChild->resolvedSymbolOverload->typeInfo;
                            }
                        }
                    }
                }
                else
                {
                    copyChild->resolvedSymbolName     = child->resolvedSymbolOverload->symbol;
                    copyChild->resolvedSymbolOverload = child->resolvedSymbolOverload;
                    copyChild->typeInfo               = child->typeInfo;
                }

                copyChild->byteCodeFct = ByteCodeGen::emitIdentifier;
                copyChild->flags |= AST_TO_UFCS | AST_L_VALUE;
                copyChild->flags |= AST_UFCS_FCT;
            }
        }
        else
        {
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            // We copy instead of moving in case this will be evaluated another time (inline)
            for (auto child : identifierRef->childs)
            {
                auto copyChild = Ast::cloneRaw(child, idRef);
                child->flags |= AST_NO_BYTECODE;
                if (child == identifierRef->previousResolvedNode)
                {
                    copyChild->flags |= AST_TO_UFCS;
                    copyChild->flags |= AST_UFCS_FCT;
                    break;
                }
            }
        }
    }

    idRef->inheritAndFlag1(AST_CONST_EXPR);
    fctCallParam->inheritAndFlag1(AST_CONST_EXPR);

    identifierRef->previousResolvedNode->flags |= AST_FROM_UFCS;
    return true;
}
