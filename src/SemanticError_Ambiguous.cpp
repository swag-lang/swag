#include "pch.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, VectorNative<OneMatch*>& genericMatches)
{
    const auto symbol = tryMatches[0]->overload->symbol;
    if (!node)
        node = context->node;

    Diagnostic err{node, node->token, FMT(Err(Err0019), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    for (const auto match : genericMatches)
    {
        const auto overload = match->symbolOverload;
        const auto note     = Diagnostic::note(overload->node, overload->node->getTokenName(), Nte(Nte0051));
        note->canBeMerged   = false;
        err.addNote(note);
    }

    return context->report(err);
}

bool SemanticError::ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, VectorNative<OneMatch*>& matches, MatchIdParamsFlags flags)
{
    const auto symbol = tryMatches[0]->overload->symbol;
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

    Diagnostic err{node, node->token, FMT(Err(Err0017), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    for (const auto match : matches)
    {
        const auto  overload = match->symbolOverload;
        Diagnostic* note;

        if (overload->typeInfo->isFuncAttr() && overload->typeInfo->isFromGeneric())
        {
            auto couldBe = FMT(Nte(Nte0049), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isFuncAttr())
        {
            auto couldBe = FMT(Nte(Nte0048), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isStruct())
        {
            auto couldBe = FMT(Nte(Nte0050), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else
        {
            const auto concreteType = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
            auto       couldBe      = FMT(Nte(Nte0046), Naming::aKindName(match->symbolOverload).c_str(), concreteType->getDisplayNameC());
            note                    = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }

        note->canBeMerged = false;
        err.addNote(note);
    }

    return context->report(err);
}

bool SemanticError::ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, const SymbolName* symbol, VectorNative<OneSymbolMatch>& matches)
{
    Diagnostic err{identifier, FMT(Err(Err0017), Naming::kindName(symbol->kind).c_str(), identifier->token.c_str())};

    for (const auto& p1 : matches)
    {
        auto       couldBe = FMT(Nte(Nte0047), Naming::aKindName(p1.symbol->kind).c_str());
        const auto note    = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->getTokenName(), couldBe);
        note->canBeMerged  = false;
        err.addNote(note);
    }

    return context->report(err);
}
