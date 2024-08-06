#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

namespace
{
    Diagnostic* unknownIdentifierInScope(const AstIdentifierRef* identifierRef, AstIdentifier* identifier, Vector<const Diagnostic*>& notes)
    {
        auto typeRef = TypeManager::concreteType(identifierRef->typeInfo);
        if (typeRef)
            typeRef = typeRef->getFinalType();

        // Error inside a tuple
        if (typeRef && typeRef->isTuple())
        {
            const auto err        = new Diagnostic{identifier, formErr(Err0709, identifier->token.c_str())};
            const auto structNode = castAst<AstStruct>(identifierRef->startScope->owner, AstNodeKind::StructDecl);
            const auto errNode    = structNode->originalParent ? structNode->originalParent : identifierRef->startScope->owner;
            const auto note       = Diagnostic::note(errNode, toNte(Nte0083));
            notes.push_back(note);
            return err;
        }

        AstIdentifier* prevIdentifier = nullptr;
        if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->is(AstNodeKind::Identifier))
            prevIdentifier = castAst<AstIdentifier>(identifierRef->previousResolvedNode, AstNodeKind::Identifier);

        const Utf8 whereScopeName = Naming::kindName(identifierRef->startScope->kind);

        Utf8 displayName;
        if (!identifierRef->startScope->flags.has(SCOPE_FILE))
            displayName = identifierRef->startScope->getDisplayFullName();
        if (displayName.empty() && !identifierRef->startScope->name.empty())
            displayName = identifierRef->startScope->name;
        if (displayName.empty() && typeRef)
            displayName = typeRef->name;
        if (displayName.empty())
            return nullptr;

        Diagnostic* err;
        const auto  typeWhere = identifierRef->startScope->owner->typeInfo;

        if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
        {
            const auto altEnum = prevIdentifier->identifierExtension->alternateEnum;
            const auto msg     = formErr(Err0707, identifier->token.c_str(), altEnum->getDisplayNameC(), whereScopeName.c_str(), displayName.c_str());
            err                = new Diagnostic{identifier, identifier->token, msg};
            const auto note    = Diagnostic::hereIs(altEnum->declNode);
            if (note)
                notes.push_back(note);
        }
        else if (typeWhere->isEnum())
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0702, identifier->token.c_str(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->token.is(g_LangSpec->name_opVisit))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0719, typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->token.text.startsWith(g_LangSpec->name_opVisit))
        {
            const auto visitNode = castAst<AstVisit>(identifier->findParent(AstNodeKind::Visit), AstNodeKind::Visit);
            const Utf8 variant{identifier->token.text.buffer + g_LangSpec->name_opVisit.length()};
            err = new Diagnostic{visitNode, visitNode->extraNameToken, formErr(Err0720, variant.c_str(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->callParameters)
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0714, identifier->token.c_str(), typeWhere->getDisplayNameC())};
        }
        else if (identifierRef->parent && identifierRef->parent->is(AstNodeKind::AttrUse))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0700, identifier->token.c_str(), typeWhere->getDisplayNameC())};
        }
        else if (identifier->callParameters)
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0706, identifier->token.c_str(), typeWhere->getDisplayNameC())};
        }
        else
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0708, identifier->token.c_str(), typeWhere->getDisplayNameC())};
        }

        // Variable before
        if (prevIdentifier &&
            prevIdentifier->resolvedSymbolName() &&
            prevIdentifier->resolvedSymbolName()->is(SymbolKind::Variable) &&
            !prevIdentifier->hasAstFlag(AST_GENERATED))
        {
            err->addNote(prevIdentifier, Diagnostic::isType(prevIdentifier));
        }

        switch (identifierRef->startScope->owner->kind)
        {
            case AstNodeKind::StructDecl:
            case AstNodeKind::InterfaceDecl:
            case AstNodeKind::EnumDecl:
            {
                const auto note = Diagnostic::hereIs(identifierRef->startScope->owner);
                if (note)
                    notes.push_back(note);
                break;
            }
            default:
                break;
        }

        return err;
    }

    bool badParentScope(const AstIdentifier* identifier, Vector<const Diagnostic*>& notes)
    {
        if (identifier->identifierRef()->startScope || identifier == identifier->parent->firstChild())
            return false;

        const auto prev = identifier->identifierRef()->children[identifier->childParentIdx() - 1];
        if (!prev->resolvedSymbolName())
            return false;

        if (prev->hasExtraPointer(ExtraPointerKind::UserOp))
        {
            const auto userOp   = prev->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
            const auto typeInfo = TypeManager::concreteType(userOp->typeInfo);
            const auto msg      = formNte(Nte0124, userOp->symbol->name.c_str(), typeInfo->getDisplayNameC());
            const auto note     = Diagnostic::note(prev, msg);
            notes.push_back(note);
        }
        else if (prev->is(AstNodeKind::ArrayPointerIndex))
        {
            const auto api       = castAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
            const auto typeArray = castTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
            const auto note      = Diagnostic::note(api->array, formNte(Nte0008, api->array->token.c_str(), typeArray->finalType->getDisplayNameC()));
            notes.push_back(note);
        }
        else
        {
            const Diagnostic* note;
            const auto        kindName = Naming::aKindName(prev->resolvedSymbolName()->kind);
            const auto        prevType = TypeManager::concreteType(prev->typeInfo, CONCRETE_FORCE_ALIAS);
            if (prevType)
                note = Diagnostic::note(prev, formNte(Nte0005, prev->token.c_str(), kindName.c_str(), prevType->getDisplayNameC()));
            else
                note = Diagnostic::note(prev, formNte(Nte0004, prev->token.c_str(), kindName.c_str()));
            notes.push_back(note);

            if (prev->typeInfo && prev->typeInfo->declNode)
                notes.push_back(Diagnostic::hereIs(prev->typeInfo->declNode));
        }

        return true;
    }
}

bool SemanticError::unknownIdentifierError(SemanticContext* context, const AstIdentifierRef* identifierRef, AstIdentifier* identifier)
{
    // What kind of thing to we search for ?
    const auto pr2       = identifier->getParent(2);
    auto       searchFor = IdentifierSearchFor::Whatever;
    if (pr2->is(AstNodeKind::TypeExpression) && identifier->parent->lastChild() == identifier)
        searchFor = IdentifierSearchFor::Type;
    else if (pr2->is(AstNodeKind::AttrUse) && identifier->parent->lastChild() == identifier)
        searchFor = IdentifierSearchFor::Attribute;
    else if (identifier->callParameters && identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        searchFor = IdentifierSearchFor::Struct;
    else if (identifier->callParameters)
        searchFor = IdentifierSearchFor::Function;

    // Default message
    Diagnostic* err;
    switch (searchFor)
    {
        case IdentifierSearchFor::Function:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0706, identifier->token.c_str())};
            break;
        case IdentifierSearchFor::Attribute:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0700, identifier->token.c_str())};
            break;
        case IdentifierSearchFor::Type:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0718, identifier->token.c_str())};
            break;
        case IdentifierSearchFor::Struct:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0717, identifier->token.c_str())};
            break;
        default:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0710, identifier->token.c_str())};
            break;
    }

    Vector<const Diagnostic*> notes;

    // Special case with an intrinsic or a compiler function inside a scope
    if (identifierRef->startScope && identifier->token.text[0] == '@')
    {
        err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0253, identifier->token.c_str())};
        if (identifier->childParentIdx())
            notes.push_back(Diagnostic::note(identifier->parent->children[identifier->childParentIdx() - 1], toNte(Nte0205)));
        return context->report(*err, notes);
    }

    if (identifierRef->startScope && identifier->token.text[0] == '#')
    {
        err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0194, identifier->token.c_str())};
        if (identifier->childParentIdx())
            notes.push_back(Diagnostic::note(identifier->parent->children[identifier->childParentIdx() - 1], toNte(Nte0205)));
        return context->report(*err, notes);
    }

    // Find best matches
    if (!badParentScope(identifier, notes))
    {
        auto& scopeHierarchy     = context->cacheScopeHierarchy;
        auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
        scopeHierarchy.clear();
        if (identifierRef->startScope)
            Semantic::addCollectedScopeOnce(scopeHierarchy, identifierRef->startScope);
        else
            Semantic::collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, identifier, COLLECT_ALL);
        const Utf8 appendMsg = findClosestMatchesMsg(identifier->token.text, scopeHierarchy, searchFor);
        notes.push_back(Diagnostic::note(appendMsg));
    }

    // Error in scope context
    if (identifierRef->startScope)
    {
        const auto specDiag = unknownIdentifierInScope(identifierRef, identifier, notes);
        if (specDiag)
            err = specDiag;
    }

    commonErrorNotes(context, {}, identifier, err, notes);

    // Additional notes
    if (searchFor == IdentifierSearchFor::Type && identifier->token.text == "int")
        notes.push_back(Diagnostic::note(formNte(Nte0109, "s32")));

    return context->report(*err, notes);
}
