#include "pch.h"
#include "Semantic/Error/SemanticError.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"

void SemanticError::errorConstraintFailed(SemanticContext*, const ErrorParam& errorParam, AstNode* constraintExpr)
{
    AstNode* node;
    ErrorID  errID;

    if (errorParam.destFuncDecl)
    {
        errID = Err0078;
        node  = errorParam.destFuncDecl;
    }
    else
    {
        errID = Err0079;
        node  = errorParam.destStructDecl;
    }

    const auto msg = formErr(errID, constraintExpr->token.cstr(), Naming::kindName(node).cstr(), node->token.cstr(), constraintExpr->token.cstr());
    const auto err = new Diagnostic{errorParam.errorNode, errorParam.errorNode->getTokenName(), msg};
    errorParam.addError(err);
    errorParam.addNote(Diagnostic::hereIs(constraintExpr, form("this is the failed [[%s]] constraint", constraintExpr->token.cstr())));
}

void SemanticError::commonErrorNotes(SemanticContext*, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* err, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;

    // Symbol has been found thanks to a using
    if (node->is(AstNodeKind::Identifier) &&
        tryMatches.size() == 1 &&
        tryMatches[0]->dependentVar &&
        !tryMatches[0]->dependentVar->isGeneratedMe())
    {
        const auto msg  = form("the symbol [[%s]] was found through a [[using]] statement", tryMatches[0]->overload->symbol->getFullName().cstr());
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

        if (identifierRef->previousScope &&
            identifier->ownerStructScope &&
            tryMatches[0]->ufcs &&
            overload->node->ownerStructScope &&
            overload->node->ownerStructScope->owner != identifierRef->previousScope->owner)
        {
            if (identifierRef->typeInfo)
            {
                const auto msg = form("the %s [[%s]] wasn't found in [[%s]]. The alternative from [[%s]] was selected",
                                         Naming::kindName(overload).cstr(),
                                         node->token.cstr(),
                                         identifierRef->typeInfo->getDisplayNameC(),
                                         overload->node->ownerStructScope->owner->token.cstr());
                err->remarks.push_back(msg);
            }

            for (const auto s : identifierRef->previousScope->childrenScopes)
            {
                if (s->is(ScopeKind::Impl) && s->symTable.find(node->token.text))
                {
                    auto msg = form("the symbol [[%s]] is already present in the interface scope [[%s]]", node->token.cstr(), s->getFullName().cstr());
                    err->remarks.push_back(msg);
                }
            }
        }
    }
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, const AstNode* hintType)
{
    Utf8 text = formErr(Err0596, node->token.cstr(), typeInfo->getDisplayNameC());
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
    Utf8 what;
    if (thisKind != otherKind)
    {
        what = "symbol";
        as   = form("as [[%s]]", Naming::aKindName(otherKind).cstr());
    }
    else
        what = Naming::kindName(thisKind);

    Diagnostic err{sourceFile, token, formErr(Err0004, what.cstr(), Naming::kindName(thisKind).cstr(), thisName.cstr(), as.cstr())};

    if (otherSymbolDecl->isGeneratedMe())
        err.addNote(otherSymbolDecl->ownerFct, otherSymbolDecl->ownerFct->token, "there is already an implied first parameter named [[me]] because of [[mtd]]");
    else
        err.addNote(otherSymbolDecl, otherSymbolDecl->getTokenName(), "this is the other definition");

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

void ErrorParam::addNote(Diagnostic* note, bool addGeneric) const
{
    if (!note)
        return;

    diagNote->push_back(note);

    if (bi && addGeneric)
    {
        auto remarks = Generic::computeGenericParametersReplacement(bi->genericReplaceTypes, bi->genericReplaceValues);
        if (!remarks.empty())
            note->autoRemarks.insert(note->autoRemarks.end(), remarks.begin(), remarks.end());
    }
}
