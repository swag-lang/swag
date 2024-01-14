#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

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
        auto msg  = Fmt(Nte(Nte0013), tryMatches[0]->overload->symbol->getFullName().c_str());
        auto note = Diagnostic::note(tryMatches[0]->dependentVar, tryMatches[0]->dependentVar->token, msg);
        notes.push_back(note);
    }
}

void SemanticError::commonErrorRemarks(SemanticContext* context, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag)
{
    if (!node)
        return;
    if (node->kind != AstNodeKind::Identifier && node->kind != AstNodeKind::FuncCall)
        return;
    if (tryMatches.empty())
        return;

    // If we have an UFCS call, and the match does not come from its symtable, then that means that we have not found the
    // symbol in the original struct also.
    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    if (identifier->identifierRef()->startScope && !tryMatches.empty())
    {
        size_t notFound = 0;
        for (auto tryMatch : tryMatches)
        {
            if (tryMatch->ufcs &&
                tryMatch->overload->node->ownerStructScope &&
                identifier->ownerStructScope &&
                tryMatch->overload->node->ownerStructScope->owner != identifier->identifierRef()->startScope->owner)
                notFound++;
        }

        if (notFound == tryMatches.size())
        {
            if (identifier->identifierRef()->typeInfo)
            {
                auto over = tryMatches.front()->overload;
                auto msg  = Fmt(Nte(Nte0043),
                               Naming::kindName(over).c_str(),
                               node->token.ctext(),
                               identifier->identifierRef()->typeInfo->getDisplayNameC(),
                               over->node->ownerStructScope->owner->token.ctext());
                diag->remarks.push_back(msg);
            }

            for (auto s : identifier->identifierRef()->startScope->childScopes)
            {
                if (s->kind == ScopeKind::Impl)
                {
                    if (s->symTable.find(node->token.text))
                    {
                        diag->remarks.push_back(Fmt(Nte(Nte0044), node->token.ctext(), s->getFullName().c_str()));
                    }
                }
            }
        }
    }
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = Fmt(Err(Err0005), node->token.ctext(), typeInfo->getDisplayNameC());
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

    Diagnostic diag{sourceFile, token, Fmt(Err(Err0305), Naming::kindName(thisKind).c_str(), thisName.c_str(), as.c_str())};
    auto       note = Diagnostic::note(otherSymbolDecl, otherSymbolDecl->getTokenName(), Nte(Nte0036));
    return context->report(diag, note);
}

bool SemanticError::error(SemanticContext* context, const Utf8& msg)
{
    return context->report({context->node, msg});
}
