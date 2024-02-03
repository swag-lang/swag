#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

static Diagnostic* unknownIdentifierInScope(AstIdentifierRef* identifierRef, AstIdentifier* node, Vector<const Diagnostic*>& notes)
{
    auto typeRef = TypeManager::concreteType(identifierRef->typeInfo);
    if (typeRef)
        typeRef = typeRef->getFinalType();

    // Error inside a tuple
    if (typeRef && typeRef->isTuple())
    {
        const auto diag       = new Diagnostic{node, Fmt(Err(Err0716), node->token.ctext())};
        const auto structNode = CastAst<AstStruct>(identifierRef->startScope->owner, AstNodeKind::StructDecl);
        const auto errNode    = structNode->originalParent ? structNode->originalParent : identifierRef->startScope->owner;
        const auto note       = Diagnostic::note(errNode, Nte(Nte0078));
        notes.push_back(note);
        return diag;
    }

    AstIdentifier* prevIdentifier = nullptr;
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier)
        prevIdentifier = CastAst<AstIdentifier>(identifierRef->previousResolvedNode, AstNodeKind::Identifier);

    const Utf8 whereScopeName = Naming::kindName(identifierRef->startScope->kind).c_str();

    Utf8 displayName;
    if (!(identifierRef->startScope->flags & SCOPE_FILE))
        displayName = identifierRef->startScope->getDisplayFullName();
    if (displayName.empty() && !identifierRef->startScope->name.empty())
        displayName = identifierRef->startScope->name;
    if (displayName.empty() && typeRef)
        displayName = typeRef->name;
    if (displayName.empty())
        return nullptr;

    Diagnostic* diag      = nullptr;
    const auto  typeWhere = identifierRef->startScope->owner->typeInfo;

    const auto varDecl = node->findParent(AstNodeKind::VarDecl);
    const auto idRef   = node->identifierRef();
    if (idRef && (idRef->flags & AST_TUPLE_UNPACK) && varDecl)
    {
        diag = new Diagnostic{node, Fmt(Err(Err0719), varDecl->token.ctext(), displayName.c_str())};
    }
    else if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
    {
        const auto altEnum = prevIdentifier->identifierExtension->alternateEnum;
        const auto msg     = Fmt(Err(Err0714), node->token.ctext(), altEnum->getDisplayNameC(), whereScopeName.c_str(), displayName.c_str());
        diag               = new Diagnostic{node, node->token, msg};
        notes.push_back(Diagnostic::hereIs(altEnum->declNode));
    }
    else if (!typeWhere)
    {
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0713), node->token.ctext(), whereScopeName.c_str(), displayName.c_str())};
    }
    else if (typeWhere->kind == TypeInfoKind::Enum)
    {
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0708), node->token.ctext(), typeWhere->getDisplayNameC())};
    }
    else if (typeWhere->kind == TypeInfoKind::Struct && node->token.text == g_LangSpec->name_opVisit)
    {
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0420), typeWhere->getDisplayNameC())};
    }
    else if (typeWhere->kind == TypeInfoKind::Struct && node->token.text.startsWith(g_LangSpec->name_opVisit))
    {
        const auto visitNode = CastAst<AstVisit>(node->findParent(AstNodeKind::Visit), AstNodeKind::Visit);
        const Utf8 variant{node->token.text.buffer + g_LangSpec->name_opVisit.length()};
        diag = new Diagnostic{visitNode, visitNode->extraNameToken, Fmt(Err(Err0419), variant.c_str(), typeWhere->getDisplayNameC())};
    }
    else if (typeWhere->kind == TypeInfoKind::Struct && node->callParameters)
    {
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0723), node->token.ctext(), typeWhere->getDisplayNameC())};
    }
    else
    {
        diag = new Diagnostic{node, node->token, Fmt(Err(Err0715), node->token.ctext(), typeWhere->getDisplayNameC())};
    }

    // Variable before
    if (prevIdentifier &&
        prevIdentifier->resolvedSymbolName &&
        prevIdentifier->resolvedSymbolName->kind == SymbolKind::Variable &&
        !(prevIdentifier->flags & AST_GENERATED))
    {
        diag->addRange(prevIdentifier, Diagnostic::isType(prevIdentifier));
    }

    switch (identifierRef->startScope->owner->kind)
    {
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::EnumDecl:
    {
        const auto note = Diagnostic::hereIs(identifierRef->startScope->owner);
        notes.push_back(note);
        break;
    }
    default:
        break;
    }

    return diag;
}

static bool badParentScope(AstIdentifier* identifier, Vector<const Diagnostic*>& notes)
{
    if (identifier->identifierRef()->startScope || identifier == identifier->parent->childs.front())
        return false;

    const auto prev = identifier->identifierRef()->childs[identifier->childParentIdx() - 1];
    if (!prev->resolvedSymbolName)
        return false;

    if (prev->hasExtMisc() && prev->extMisc()->resolvedUserOpSymbolOverload)
    {
        const auto typeInfo = TypeManager::concreteType(prev->extMisc()->resolvedUserOpSymbolOverload->typeInfo);
        const auto msg      = Fmt(Nte(Nte0114), prev->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str(), typeInfo->getDisplayNameC());
        const auto note     = Diagnostic::note(prev, msg);
        notes.push_back(note);
    }
    else if (prev->kind == AstNodeKind::ArrayPointerIndex)
    {
        const auto api       = CastAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
        const auto typeArray = CastTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
        const auto note      = Diagnostic::note(api->array, Fmt(Nte(Nte0006), api->array->token.ctext(), typeArray->finalType->getDisplayNameC()));
        notes.push_back(note);
    }
    else
    {
        const Diagnostic* note     = nullptr;
        const auto  kindName = Naming::aKindName(prev->resolvedSymbolName->kind);
        if (prev->typeInfo)
            note = Diagnostic::note(prev, Fmt(Nte(Nte0004), prev->token.ctext(), kindName.c_str(), prev->typeInfo->getDisplayNameC()));
        else
            note = Diagnostic::note(prev, Fmt(Nte(Nte0003), prev->token.ctext(), kindName.c_str()));
        notes.push_back(note);
        if (prev->resolvedSymbolOverload)
            notes.push_back(Diagnostic::hereIs(prev));
    }

    return true;
}

void SemanticError::unknownIdentifierError(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    // What kind of thing to we search for ?
    auto searchFor = IdentifierSearchFor::Whatever;
    if (node->parent->parent && node->parent->parent->kind == AstNodeKind::TypeExpression && node->parent->childs.back() == node)
        searchFor = IdentifierSearchFor::Type;
    else if (node->parent->parent && node->parent->parent->kind == AstNodeKind::AttrUse && node->parent->childs.back() == node)
        searchFor = IdentifierSearchFor::Attribute;
    else if (node->callParameters && (node->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
        searchFor = IdentifierSearchFor::Struct;
    else if (node->callParameters)
        searchFor = IdentifierSearchFor::Function;

    Vector<const Diagnostic*> notes;
    Diagnostic*               diag = nullptr;

    // Default message
    switch (searchFor)
    {
    case IdentifierSearchFor::Function:
        if (node->token.text[0] == '#')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0706), node->token.ctext())};
        else if (node->token.text[0] == '@')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0721), node->token.ctext())};
        else
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0712), node->token.ctext())};
        break;
    case IdentifierSearchFor::Attribute:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0705), node->token.ctext())};
        break;
    case IdentifierSearchFor::Type:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0727), node->token.ctext())};
        break;
    case IdentifierSearchFor::Struct:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0726), node->token.ctext())};
        break;
    default:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0717), node->token.ctext())};
        break;
    }

    // Find best matches
    if (!badParentScope(node, notes))
    {
        auto& scopeHierarchy     = context->cacheScopeHierarchy;
        auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;
        scopeHierarchy.clear();
        if (identifierRef->startScope)
            Semantic::addAlternativeScopeOnce(scopeHierarchy, identifierRef->startScope);
        else
            Semantic::collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, COLLECT_ALL);
        const Utf8 appendMsg = findClosestMatchesMsg(node->token.text, scopeHierarchy, searchFor);
        notes.push_back(Diagnostic::note(appendMsg));
    }

    // Error in scope context
    if (identifierRef->startScope)
    {
        const auto specDiag = unknownIdentifierInScope(identifierRef, node, notes);
        if (specDiag)
            diag = specDiag;
    }

    commonErrorNotes(context, {}, node, diag, notes);
    context->report(*diag, notes);
}
