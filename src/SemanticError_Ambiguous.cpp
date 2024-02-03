#include "pch.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& genericMatches)
{
    const auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    const Diagnostic diag{node, node->token, Fmt(Err(Err0019), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (const auto match : genericMatches)
    {
        const auto overload = match->symbolOverload;
        const auto note     = Diagnostic::note(overload->node, overload->node->getTokenName(), Nte(Nte0051));
        note->canBeMerged   = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}

bool SemanticError::ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& matches, uint32_t flags)
{
    const auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    if (flags & MIP_FOR_GHOSTING)
    {
        AstNode*   otherNode = nullptr;
        SymbolKind otherKind = SymbolKind::Invalid;
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
        return duplicatedSymbolError(context, node->sourceFile, node->token, symbol->kind, symbol->name, otherKind, otherNode);
    }

    const Diagnostic diag{node, node->token, Fmt(Err(Err0017), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (const auto match : matches)
    {
        const auto        overload = match->symbolOverload;
        Diagnostic* note     = nullptr;

        if (overload->typeInfo->isFuncAttr() && overload->typeInfo->flags & TYPEINFO_FROM_GENERIC)
        {
            auto couldBe = Fmt(Nte(Nte0049), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isFuncAttr())
        {
            auto couldBe = Fmt(Nte(Nte0048), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isStruct())
        {
            auto couldBe = Fmt(Nte(Nte0050), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else
        {
            const auto concreteType = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
            auto couldBe      = Fmt(Nte(Nte0046), Naming::aKindName(match->symbolOverload).c_str(), concreteType->getDisplayNameC());
            note              = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }

        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}

bool SemanticError::ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol, VectorNative<OneSymbolMatch>& dependentSymbols)
{
    const Diagnostic diag{identifier, Fmt(Err(Err0017), Naming::kindName(symbol->kind).c_str(), identifier->token.ctext())};

    Vector<const Diagnostic*> notes;
    for (const auto& p1 : dependentSymbols)
    {
        auto       couldBe = Fmt(Nte(Nte0047), Naming::aKindName(p1.symbol->kind).c_str());
        const auto note    = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->getTokenName(), couldBe);
        note->canBeMerged  = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}