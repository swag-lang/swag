#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Semantic.h"

void SemanticError::commonErrorNotes(SemanticContext*, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* err, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;

    // Symbol has been found thanks to a using
    if ((node->kind == AstNodeKind::Identifier || node->kind == AstNodeKind::FuncCall) &&
        tryMatches.size() == 1 &&
        tryMatches[0]->dependentVar &&
        !tryMatches[0]->dependentVar->isGeneratedSelf())
    {
        const auto msg  = formNte(Nte0137, tryMatches[0]->overload->symbol->getFullName().c_str());
        const auto note = Diagnostic::note(tryMatches[0]->dependentVar, tryMatches[0]->dependentVar->token, msg);
        notes.push_back(note);
    }

    // If we have an UFCS call, and the match does not come from its symbol table, then that means that we have not found the
    // symbol in the original struct also.
    if ((node->kind == AstNodeKind::Identifier || node->kind == AstNodeKind::FuncCall) &&
        tryMatches.size() == 1)
    {
        const auto identifier    = castAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
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
                const auto msg = formNte(Nte0111, Naming::kindName(overload).c_str(), node->token.c_str(), identifierRef->typeInfo->getDisplayNameC(),
                                         overload->node->ownerStructScope->owner->token.c_str());
                err->remarks.push_back(msg);
            }

            for (const auto s : identifierRef->startScope->childScopes)
            {
                if (s->is(ScopeKind::Impl) && s->symTable.find(node->token.text))
                {
                    auto msg = formNte(Nte0136, node->token.c_str(), s->getFullName().c_str());
                    err->remarks.push_back(msg);
                }
            }
        }
    }
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = formErr(Err0351, node->token.c_str(), typeInfo->getDisplayNameC());
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

    Diagnostic err{sourceFile, token, formErr(Err0626, Naming::kindName(thisKind).c_str(), thisName.c_str(), as.c_str())};
    err.addNote(otherSymbolDecl, otherSymbolDecl->getTokenName(), toNte(Nte0071));
    return context->report(err);
}

bool SemanticError::error(SemanticContext* context, const Utf8& msg)
{
    return context->report({context->node, msg});
}
