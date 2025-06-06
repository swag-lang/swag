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
            const auto err        = new Diagnostic{identifier, formErr(Err0684, identifier->token.cstr())};
            const auto structNode = castAst<AstStruct>(identifierRef->previousScope->owner, AstNodeKind::StructDecl);
            const auto errNode    = structNode->originalParent ? structNode->originalParent : identifierRef->previousScope->owner;
            const auto note       = Diagnostic::note(errNode, toNte(Nte0202));
            notes.push_back(note);
            return err;
        }

        const AstIdentifier* prevIdentifier = nullptr;
        if (identifierRef->previousNode && identifierRef->previousNode->is(AstNodeKind::Identifier))
            prevIdentifier = castAst<AstIdentifier>(identifierRef->previousNode, AstNodeKind::Identifier);

        const Utf8 whereScopeName = Naming::kindName(identifierRef->previousScope->kind);

        Utf8 displayName;
        if (!identifierRef->previousScope->flags.has(SCOPE_FILE))
            displayName = identifierRef->previousScope->getDisplayFullName();
        if (displayName.empty() && !identifierRef->previousScope->name.empty())
            displayName = identifierRef->previousScope->name;
        if (displayName.empty() && typeRef)
            displayName = typeRef->name;
        if (displayName.empty())
            return nullptr;

        Diagnostic* err;
        const auto  typeWhere = identifierRef->previousScope->owner->typeInfo;

        if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
        {
            const auto altEnum = prevIdentifier->identifierExtension->alternateEnum;
            const auto msg     = formErr(Err0683, identifier->token.cstr(), altEnum->getDisplayNameC(), whereScopeName.cstr(), displayName.cstr());
            err                = new Diagnostic{identifier, identifier->token, msg};
            const auto note    = Diagnostic::hereIs(altEnum->declNode);
            if (note)
                notes.push_back(note);
        }
        else if (!typeWhere)
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0682, identifier->token.cstr())};
        }
        else if (typeWhere->isEnum())
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0675, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->token.is(g_LangSpec->name_opVisit))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0669, typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->token.text.startsWith(g_LangSpec->name_opVisit))
        {
            const auto visitNode = castAst<AstVisit>(identifier->findParent(AstNodeKind::Visit), AstNodeKind::Visit);
            const Utf8 variant{identifier->token.text.buffer + g_LangSpec->name_opVisit.length()};
            err = new Diagnostic{visitNode, visitNode->extraNameToken, formErr(Err0670, variant.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct() && identifier->callParameters && !identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0688, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isInterface() && identifier->callParameters && !identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0685, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (identifierRef->parent && identifierRef->parent->is(AstNodeKind::AttrUse))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0672, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (identifier->callParameters && !identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0680, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (identifier->callParameters)
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0693, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else if (typeWhere->isStruct())
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0700, identifier->token.cstr(), typeWhere->getDisplayNameC())};
        }
        else
        {
            err = new Diagnostic{identifier, identifier->token, formErr(Err0682, identifier->token.cstr())};
        }

        // Variable before
        if (prevIdentifier &&
            prevIdentifier->resolvedSymbolName() &&
            prevIdentifier->resolvedSymbolName()->is(SymbolKind::Variable) &&
            !prevIdentifier->hasAstFlag(AST_GENERATED))
        {
            err->addNote(prevIdentifier, Diagnostic::isType(prevIdentifier));
        }

        switch (identifierRef->previousScope->owner->kind)
        {
            case AstNodeKind::StructDecl:
            case AstNodeKind::InterfaceDecl:
            case AstNodeKind::EnumDecl:
            {
                const auto note = Diagnostic::hereIs(identifierRef->previousScope->owner);
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
        if (identifier->identifierRef()->previousScope || identifier == identifier->parent->firstChild())
            return false;

        const auto prev = identifier->identifierRef()->children[identifier->childParentIdx() - 1];
        if (!prev->resolvedSymbolName())
            return false;

        if (prev->hasExtraPointer(ExtraPointerKind::UserOp))
        {
            const auto userOp   = prev->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
            const auto typeInfo = TypeManager::concreteType(userOp->typeInfo);
            const auto msg      = formNte(Nte0118, userOp->symbol->name.cstr(), typeInfo->getDisplayNameC());
            const auto note     = Diagnostic::note(prev, msg);
            notes.push_back(note);
        }
        else if (prev->is(AstNodeKind::ArrayPointerIndex))
        {
            const auto api       = castAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
            const auto typeArray = castTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
            const auto note      = Diagnostic::note(api->array, formNte(Nte0006, api->array->token.cstr(), typeArray->finalType->getDisplayNameC()));
            notes.push_back(note);
        }
        else
        {
            const Diagnostic* note;
            const auto        kindName = Naming::aKindName(prev->resolvedSymbolName()->kind);
            const auto        prevType = prev->typeInfo->getConcreteAlias();
            if (prevType)
                note = Diagnostic::note(prev, formNte(Nte0003, prev->token.cstr(), kindName.cstr(), prevType->getDisplayNameC()));
            else
                note = Diagnostic::note(prev, formNte(Nte0004, prev->token.cstr(), kindName.cstr()));
            notes.push_back(note);

            if (prev->typeInfo && prev->typeInfo->declNode)
                notes.push_back(Diagnostic::hereIs(prev->typeInfo->declNode));
        }

        return true;
    }
}

bool SemanticError::unknownIdentifierError(SemanticContext* context, const AstIdentifierRef* identifierRef, AstIdentifier* identifier)
{
    // What kind of thing to we search for?
    const auto pr1       = identifier->getParent(1);
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
    else if (pr2->is(AstNodeKind::AffectOp) || pr2->is(AstNodeKind::BinaryOp) || pr1->is(AstNodeKind::ArrayPointerIndex))
        searchFor = IdentifierSearchFor::Variable;
    else if (pr2->is(AstNodeKind::Impl) && pr2->hasSpecFlag(AstImpl::SPEC_FLAG_ENUM))
        searchFor = IdentifierSearchFor::Enum;
    else if (pr2->is(AstNodeKind::Impl))
        searchFor = IdentifierSearchFor::Struct;

    // Default message
    Diagnostic* err;
    switch (searchFor)
    {
        case IdentifierSearchFor::Function:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0679, identifier->token.cstr())};
            break;
        case IdentifierSearchFor::Attribute:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0671, identifier->token.cstr())};
            break;
        case IdentifierSearchFor::Type:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0694, identifier->token.cstr())};
            break;
        case IdentifierSearchFor::Struct:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0692, identifier->token.cstr())};
            break;
        case IdentifierSearchFor::Enum:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0674, identifier->token.cstr())};
            break;
        case IdentifierSearchFor::Variable:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0699, identifier->token.cstr())};
            break;
        default:
            err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0682, identifier->token.cstr())};
            break;
    }

    Vector<const Diagnostic*> notes;

    // Special case with an intrinsic or a compiler function inside a scope
    if (identifierRef->previousScope && identifier->hasIntrinsicName())
    {
        err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0646, identifier->token.cstr())};
        if (identifier->childParentIdx())
            notes.push_back(Diagnostic::note(identifier->parent->children[identifier->childParentIdx() - 1], toNte(Nte0207)));
        return context->report(*err, notes);
    }

    if (identifierRef->previousScope && identifier->token.text[0] == '#')
    {
        err = new Diagnostic{identifier->token.sourceFile, identifier->token, formErr(Err0628, identifier->token.cstr())};
        if (identifier->childParentIdx())
            notes.push_back(Diagnostic::note(identifier->parent->children[identifier->childParentIdx() - 1], toNte(Nte0207)));
        return context->report(*err, notes);
    }

    // Find best matches
    if (!badParentScope(identifier, notes))
    {
        auto& scopeHierarchy     = context->cacheScopeHierarchy;
        auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
        scopeHierarchy.clear();
        if (identifierRef->previousScope)
            Semantic::addCollectedScopeOnce(scopeHierarchy, identifierRef->previousScope);
        else
            Semantic::collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, identifier, COLLECT_ALL);
        const Utf8 appendMsg = findClosestMatchesMsg(identifier->token.text, scopeHierarchy, searchFor);
        notes.push_back(Diagnostic::note(appendMsg));
    }

    // Additional notes
    switch (searchFor)
    {
        case IdentifierSearchFor::Type:
            if (identifier->token.text == "int")
                notes.push_back(Diagnostic::note(formNte(Nte0079, "s32")));
            break;

        case IdentifierSearchFor::Variable:
            if (pr2->is(AstNodeKind::AffectOp) && !identifierRef->previousScope)
                notes.push_back(Diagnostic::note(toNte(Nte0077)));
            break;
    }

    // Error in scope context
    if (identifierRef->previousScope)
    {
        const auto specDiag = unknownIdentifierInScope(identifierRef, identifier, notes);
        if (specDiag)
            err = specDiag;
    }

    commonErrorNotes(context, {}, identifier, err, notes);

    return context->report(*err, notes);
}
