#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

void SemanticError::symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;
    if (node->kind != AstNodeKind::Identifier && node->kind != AstNodeKind::FuncCall)
        return;
    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    // Symbol has been found with a using : display it
    if (tryMatches.size() == 1 && tryMatches[0]->dependentVar)
    {
        // Do not generate a note if this is a generated 'using' in case of methods
        if (!tryMatches[0]->dependentVar->isGeneratedSelf())
        {
            auto note = Diagnostic::note(tryMatches[0]->dependentVar, Fmt(Nte(Nte0013), tryMatches[0]->overload->symbol->name.c_str()));
            notes.push_back(note);
        }
    }

    // Additional error if the first parameter does not match, or if nothing matches
    bool badUfcs = tryMatches.empty();
    for (auto over : tryMatches)
    {
        if (over->symMatchContext.result == MatchResult::BadSignature && over->symMatchContext.badSignatureInfos.badSignatureParameterIdx == 0)
        {
            badUfcs = true;
            break;
        }
    }

    if (badUfcs && !identifier->identifierRef()->startScope)
    {
        // There's something before (identifier is not the only one in the identifierRef).
        if (identifier != identifier->parent->childs.front())
        {
            auto idIdx = identifier->childParentIdx();
            auto prev  = identifier->identifierRef()->childs[idIdx - 1];
            if (prev->resolvedSymbolName)
            {
                if (prev->hasExtMisc() && prev->extMisc()->resolvedUserOpSymbolOverload)
                {
                    auto typeInfo = TypeManager::concreteType(prev->extMisc()->resolvedUserOpSymbolOverload->typeInfo);
                    auto note     = Diagnostic::note(prev, Fmt(Nte(Nte0018), prev->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str(), typeInfo->getDisplayNameC()));
                    note->hint    = Diagnostic::isType(typeInfo);
                    notes.push_back(note);
                    return;
                }

                if (prev->kind == AstNodeKind::ArrayPointerIndex)
                {
                    auto api = CastAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
                    if (api->array->typeInfo)
                    {
                        prev           = api->array;
                        auto typeArray = CastTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
                        auto note      = Diagnostic::note(prev, Fmt(Nte(Nte0000), prev->token.ctext(), typeArray->finalType->getDisplayNameC()));
                        notes.push_back(note);
                    }
                }
                else
                {
                    Diagnostic* note = nullptr;
                    if (prev->typeInfo)
                        note = Diagnostic::note(prev, Fmt(Nte(Nte0001), prev->token.ctext(), Naming::aKindName(prev->resolvedSymbolName->kind).c_str(), prev->typeInfo->getDisplayNameC()));
                    else
                        note = Diagnostic::note(prev, Fmt(Nte(Nte0010), prev->token.ctext(), Naming::aKindName(prev->resolvedSymbolName->kind).c_str()));
                    notes.push_back(note);
                    if (prev->resolvedSymbolOverload)
                        notes.push_back(Diagnostic::hereIs(prev));
                }
            }
        }
    }
}

void SemanticError::symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag)
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
