#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Naming.h"

void SemanticError::ambiguousArguments(SemanticContext*, Diagnostic& err, VectorNative<OneMatch*>& matches)
{
    for (uint32_t i = 0; i < matches[0]->solvedCastFlags.size(); i++)
    {
        uint32_t cptUntyped = 0;
        for (const auto match : matches)
        {
            if (match->solvedParameters.size() <= i)
                break;
            if (match->parameters.size() <= i && match->paramParameters.size() <= i)
                break;
            if (match->solvedCastFlags.size() <= i)
                break;

            if (match->solvedCastFlags[i].has(CAST_RESULT_UNTYPED_CONVERT | CAST_RESULT_COERCE))
                cptUntyped++;
        }

        if (cptUntyped == matches.size())
        {
            Utf8 toWhat;
            if (matches.size() == 2)
            {
                toWhat = form("either [[%s]] or [[%s]]",
                              matches[0]->solvedParameters[i]->typeInfo->getDisplayNameC(),
                              matches[1]->solvedParameters[i]->typeInfo->getDisplayNameC());
            }
            else if (matches.size() == 3)
            {
                toWhat = form("either [[%s]], [[%s]] or [[%s]]",
                              matches[0]->solvedParameters[i]->typeInfo->getDisplayNameC(),
                              matches[1]->solvedParameters[i]->typeInfo->getDisplayNameC(),
                              matches[2]->solvedParameters[i]->typeInfo->getDisplayNameC());
            }
            else
            {
                toWhat = "multiple types";
            }

            const auto callParam = i < matches[0]->paramParameters.size() ? matches[0]->paramParameters[i].param : matches[0]->parameters[i];
            const auto note      = Diagnostic::note(callParam, formNte(Nte0118, matches[0]->solvedParameters[i]->declNode->token.cstr(), toWhat.cstr()));
            err.addNote(note);
            break;
        }
    }
}

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, VectorNative<OneMatch*>& matches)
{
    const auto symbol = tryMatches[0]->overload->symbol;
    if (!node)
        node = context->node;

    Diagnostic err{node, node->token, formErr(Err0010, Naming::kindName(symbol->kind).cstr(), symbol->name.cstr())};

    bool first = true;
    for (const auto match : matches)
    {
        auto couldBe = toNte(Nte0078);
        if (!first)
            couldBe = "or " + couldBe;

        const auto note   = Diagnostic::note(match->symbolOverload->node, match->symbolOverload->node->getTokenName(), couldBe);
        note->canBeMerged = false;
        err.addNote(note);

        first = false;
    }

    ambiguousArguments(context, err, matches);
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

    Diagnostic err{node, node->token, formErr(Err0009, Naming::kindName(matches[0]->symbolOverload).cstr(), symbol->name.cstr())};

    SetUtf8 here;
    bool    first = true;
    for (const auto match : matches)
    {
        const auto overload = match->symbolOverload;
        Utf8       couldBe;

        if (overload->typeInfo->isFuncAttr() && overload->typeInfo->isFromGeneric())
        {
            couldBe = formNte(Nte0075, overload->typeInfo->getDisplayNameC());
        }
        else
        {
            const auto kindName = Naming::kindName(match->symbolOverload);
            if (here.contains(kindName))
                couldBe = formNte(Nte0077, kindName.cstr());
            else
                couldBe = formNte(Nte0076, kindName.cstr());
            here.insert(kindName);
        }

        if (!first)
            couldBe = "or " + couldBe;

        const auto note   = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        note->canBeMerged = false;
        err.addNote(note);

        first = false;
    }

    ambiguousArguments(context, err, matches);
    return context->report(err);
}

bool SemanticError::ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, const SymbolName* symbol, VectorNative<OneSymbolMatch>& matches)
{
    Diagnostic err{identifier, formErr(Err0009, Naming::kindName(symbol->kind).cstr(), identifier->token.cstr())};

    bool    first = true;
    SetUtf8 here;
    for (const auto& p1 : matches)
    {
        Utf8 couldBe;

        const auto kindName = Naming::kindName(p1.symbol->kind);
        if (here.contains(kindName))
            couldBe = formNte(Nte0077, kindName.cstr());
        else
            couldBe = formNte(Nte0076, kindName.cstr());
        here.insert(kindName);

        if (!first)
            couldBe = "or " + couldBe;
        const auto note   = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->getTokenName(), couldBe);
        note->canBeMerged = false;
        err.addNote(note);

        first = false;
    }

    return context->report(err);
}
