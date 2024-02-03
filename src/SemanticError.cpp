#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"

void SemanticError::commonErrorNotes(SemanticContext* context, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;

    // Symbol has been found thanks to a using
    if ((node->kind == AstNodeKind::Identifier || node->kind == AstNodeKind::FuncCall) &&
        tryMatches.size() == 1 &&
        tryMatches[0]->dependentVar &&
        !tryMatches[0]->dependentVar->isGeneratedSelf())
    {
        const auto msg  = Fmt(Nte(Nte0137), tryMatches[0]->overload->symbol->getFullName().c_str());
        const auto note = Diagnostic::note(tryMatches[0]->dependentVar, tryMatches[0]->dependentVar->token, msg);
        notes.push_back(note);
    }

    // If we have an UFCS call, and the match does not come from its symbol table, then that means that we have not found the
    // symbol in the original struct also.
    if ((node->kind == AstNodeKind::Identifier || node->kind == AstNodeKind::FuncCall) &&
        tryMatches.size() == 1)
    {
        const auto identifier    = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
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
                const auto msg = Fmt(Nte(Nte0111), Naming::kindName(overload).c_str(), node->token.ctext(), identifierRef->typeInfo->getDisplayNameC(), overload->node->ownerStructScope->owner->token.ctext());
                diag->remarks.push_back(msg);
            }

            for (const auto s : identifierRef->startScope->childScopes)
            {
                if (s->kind == ScopeKind::Impl && s->symTable.find(node->token.text))
                {
                    auto msg = Fmt(Nte(Nte0136), node->token.ctext(), s->getFullName().c_str());
                    diag->remarks.push_back(msg);
                }
            }
        }
    }
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = Fmt(Err(Err0351), node->token.ctext(), typeInfo->getDisplayNameC());
    if (msg)
    {
        text += " ";
        text += msg;
    }

    Diagnostic diag{node, node->token, text};
    if (hintType)
        diag.addRange(hintType, Diagnostic::isType(typeInfo));
    return context->report(diag);
}

bool SemanticError::duplicatedSymbolError(ErrorContext* context, SourceFile* sourceFile, Token& token, SymbolKind thisKind, const Utf8& thisName, SymbolKind otherKind, AstNode* otherSymbolDecl)
{
    Utf8 as;
    if (thisKind != otherKind)
        as = Fmt("as %s", Naming::aKindName(otherKind).c_str());

    const Diagnostic diag{sourceFile, token, Fmt(Err(Err0626), Naming::kindName(thisKind).c_str(), thisName.c_str(), as.c_str())};
    const auto       note = Diagnostic::note(otherSymbolDecl, otherSymbolDecl->getTokenName(), Nte(Nte0071));
    return context->report(diag, note);
}

bool SemanticError::error(SemanticContext* context, const Utf8& msg)
{
    return context->report({context->node, msg});
}
