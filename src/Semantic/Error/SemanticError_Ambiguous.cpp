#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Naming.h"

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, VectorNative<OneMatch*>& genericMatches)
{
    const auto symbol = tryMatches[0]->overload->symbol;
    if (!node)
        node = context->node;

    Diagnostic err{node, node->token, formErr(Err0019, Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    for (const auto match : genericMatches)
    {
        const auto overload = match->symbolOverload;
        const auto note     = Diagnostic::note(overload->node, overload->node->getTokenName(), toNte(Nte0051));
        note->canBeMerged   = false;
        err.addNote(note);
    }

    return context->report(err);
}

bool SemanticError::ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneMatch*>& matches, MatchIdParamsFlags flags)
{
    const auto symbol = matches[0]->symbolOverload->symbol;
    if (!node)
        node = context->node;

    if (flags.has(MIP_FOR_GHOSTING))
    {
        AstNode* otherNode = nullptr;
        auto     otherKind = SymbolKind::Invalid;
        for (const auto match : matches)
        {
            if (match->symbolOverload->node != node && !match->symbolOverload->node->isParentOf(node))
            {
                otherKind = match->symbolOverload->symbol->kind;
                otherNode = match->symbolOverload->node;
                break;
            }
        }

        SWAG_ASSERT(otherNode);
        return duplicatedSymbolError(context, node->token.sourceFile, node->token, symbol->kind, symbol->name, otherKind, otherNode);
    }

    Diagnostic err{node, node->token, formErr(Err0017, Naming::kindName(matches[0]->symbolOverload).c_str(), symbol->name.c_str())};

    SetUtf8 here;
    bool    first = true;
    for (const auto match : matches)
    {
        const auto overload = match->symbolOverload;
        Utf8       couldBe;

        if (overload->typeInfo->isFuncAttr() && overload->typeInfo->isFromGeneric())
        {
            couldBe = formNte(Nte0049, overload->typeInfo->getDisplayNameC());
        }
        else
        {
            const auto kindName = Naming::kindName(match->symbolOverload);
            if (here.contains(kindName))
                couldBe = formNte(Nte0048, kindName.c_str());
            else
                couldBe = formNte(Nte0046, kindName.c_str());
            here.insert(kindName);
        }

        if (!first)
            couldBe = "or " + couldBe;

        const auto note   = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        note->canBeMerged = false;
        err.addNote(note);

        first = false;
    }

    return context->report(err);
}

bool SemanticError::ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, const SymbolName* symbol, VectorNative<OneSymbolMatch>& matches)
{
    Diagnostic err{identifier, formErr(Err0017, Naming::kindName(symbol->kind).c_str(), identifier->token.c_str())};

    for (const auto& p1 : matches)
    {
        auto       couldBe = formNte(Nte0047, Naming::aKindName(p1.symbol->kind).c_str());
        const auto note    = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->getTokenName(), couldBe);
        note->canBeMerged  = false;
        err.addNote(note);
    }

    return context->report(err);
}
