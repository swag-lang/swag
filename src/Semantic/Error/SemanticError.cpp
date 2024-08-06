#include "pch.h"
#include "Semantic/Error/SemanticError.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"

void SemanticError::errorWhereFailed(SemanticContext*, const ErrorParam& errorParam)
{
    AstNode* node;
    AstNode* whereExpr;

    if (errorParam.destFuncDecl)
    {
        node      = errorParam.destFuncDecl;
        whereExpr = errorParam.destFuncDecl->whereExpression;
    }
    else
    {
        node      = errorParam.destStructDecl;
        whereExpr = errorParam.destStructDecl->whereExpression;
    }

    const auto msg = formErr(Err0077, whereExpr->token.c_str(), Naming::kindName(node).c_str(), node->token.c_str(), whereExpr->token.c_str());
    const auto err = new Diagnostic{errorParam.errorNode, errorParam.errorNode->getTokenName(), msg};
    errorParam.addError(err);
    errorParam.addNote(Diagnostic::hereIs(whereExpr, formNte(Nte0071, whereExpr->token.c_str())));
}

void SemanticError::commonErrorNotes(SemanticContext*, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* err, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;

    // Symbol has been found thanks to a using
    if (node->is(AstNodeKind::Identifier) &&
        tryMatches.size() == 1 &&
        tryMatches[0]->dependentVar &&
        !tryMatches[0]->dependentVar->isGeneratedSelf())
    {
        const auto msg  = formNte(Nte0149, tryMatches[0]->overload->symbol->getFullName().c_str());
        const auto note = Diagnostic::note(tryMatches[0]->dependentVar, tryMatches[0]->dependentVar->token, msg);
        notes.push_back(note);
    }

    // If we have an UFCS call, and the match does not come from its symbol table, then that means that we have not found the
    // symbol in the original struct also.
    if (node->is(AstNodeKind::Identifier) && tryMatches.size() == 1)
    {
        const auto identifier    = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        const auto identifierRef = identifier->identifierRef();
        const auto overload      = tryMatches[0]->overload;

        if (identifierRef->startScope &&
            identifier->ownerStructScope &&
            tryMatches[0]->ufcs &&
            overload->node->ownerStructScope &&
            overload->node->ownerStructScope->owner != identifierRef->startScope->owner)
        {
            if (identifierRef->typeInfo)
            {
                const auto msg = formNte(Nte0118,
                                         Naming::kindName(overload).c_str(),
                                         node->token.c_str(),
                                         identifierRef->typeInfo->getDisplayNameC(),
                                         overload->node->ownerStructScope->owner->token.c_str());
                err->remarks.push_back(msg);
            }

            for (const auto s : identifierRef->startScope->childScopes)
            {
                if (s->is(ScopeKind::Impl) && s->symTable.find(node->token.text))
                {
                    auto msg = formNte(Nte0148, node->token.c_str(), s->getFullName().c_str());
                    err->remarks.push_back(msg);
                }
            }
        }
    }
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = formErr(Err0647, node->token.c_str(), typeInfo->getDisplayNameC());
    if (msg)
    {
        text += " ";
        text += msg;
    }

    Diagnostic err{node, node->token, text};
    if (hintType)
        err.addNote(hintType, Diagnostic::isType(typeInfo));
    return context->report(err);
}

bool SemanticError::duplicatedSymbolError(ErrorContext* context,
                                          SourceFile*   sourceFile,
                                          const Token&  token,
                                          SymbolKind    thisKind,
                                          const Utf8&   thisName,
                                          SymbolKind    otherKind,
                                          AstNode*      otherSymbolDecl)
{
    Utf8 as;
    if (thisKind != otherKind)
        as = form("as %s", Naming::aKindName(otherKind).c_str());

    Diagnostic err{sourceFile, token, formErr(Err0023, Naming::kindName(thisKind).c_str(), thisName.c_str(), as.c_str())};
    err.addNote(otherSymbolDecl, otherSymbolDecl->getTokenName(), toNte(Nte0076));
    return context->report(err);
}

bool SemanticError::error(SemanticContext* context, const Utf8& msg)
{
    return context->report({context->node, msg});
}

void ErrorParam::addError(const Diagnostic* note) const
{
    SWAG_ASSERT(note);
    diagError->push_back(note);
}

void ErrorParam::addNote(Diagnostic* note) const
{
    if (!note)
        return;

    diagNote->push_back(note);

    auto remarks = Generic::computeGenericParametersReplacement(bi->genericReplaceTypes);
    if (!remarks.empty())
        note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
}
