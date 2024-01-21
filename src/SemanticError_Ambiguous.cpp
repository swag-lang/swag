#include "pch.h"
#include "SemanticError.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneGenericMatch*>& genericMatches)
{
    auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    Diagnostic diag{node, node->token, Fmt(Err(Err0019), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (auto match : genericMatches)
    {
        auto overload     = match->symbolOverload;
        auto note         = Diagnostic::note(overload->node, overload->node->getTokenName(), Nte(Nte0051));
        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}

bool SemanticError::ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& matches, uint32_t flags)
{
    auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    if (flags & MIP_FOR_GHOSTING)
    {
        AstNode*   otherNode = nullptr;
        SymbolKind otherKind = SymbolKind::Invalid;
        for (auto match : matches)
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

    Diagnostic diag{node, node->token, Fmt(Err(Err0017), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (auto match : matches)
    {
        auto        overload = match->symbolOverload;
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
            auto concreteType = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
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
    Diagnostic diag{identifier, Fmt(Err(Err0017), Naming::kindName(symbol->kind).c_str(), identifier->token.ctext())};

    Vector<const Diagnostic*> notes;
    for (auto& p1 : dependentSymbols)
    {
        auto couldBe      = Fmt(Nte(Nte0047), Naming::aKindName(p1.symbol->kind).c_str());
        auto note         = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->getTokenName(), couldBe);
        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}