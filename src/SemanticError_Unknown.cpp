#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

void SemanticError::unknownIdentifierError(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    auto& scopeHierarchy     = context->cacheScopeHierarchy;
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;

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

    // Find best matches
    if (identifierRef->startScope)
    {
        scopeHierarchy.clear();
        Semantic::addAlternativeScopeOnce(scopeHierarchy, identifierRef->startScope);
    }
    else
    {
        Semantic::collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, COLLECT_ALL);
    }

    Vector<const Diagnostic*> notes;
    Diagnostic*               diag = nullptr;

    // Message
    switch (searchFor)
    {
    case IdentifierSearchFor::Function:
        if (node->token.text[0] == '#')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0140), node->token.ctext())};
        else if (node->token.text[0] == '@')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0129), node->token.ctext())};
        else
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0228), node->token.ctext())};
        break;
    case IdentifierSearchFor::Attribute:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0251), node->token.ctext())};
        break;
    case IdentifierSearchFor::Type:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0165), node->token.ctext())};
        break;
    case IdentifierSearchFor::Struct:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0393), node->token.ctext())};
        break;
    default:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0122), node->token.ctext())};
        break;
    }

    // Error in scope context
    if (identifierRef->startScope)
    {
        auto typeWhere = identifierRef->startScope->owner->typeInfo;
        auto typeRef   = TypeManager::concreteType(identifierRef->typeInfo);
        if (typeRef && typeRef->isPointer())
            typeRef = CastTypeInfo<TypeInfoPointer>(typeRef, TypeInfoKind::Pointer)->pointedType;

        if (typeRef && typeRef->isTuple())
        {
            diag            = new Diagnostic{node, Fmt(Err(Err0093), node->token.ctext())};
            auto structNode = CastAst<AstStruct>(identifierRef->startScope->owner, AstNodeKind::StructDecl);
            auto note       = Diagnostic::note(structNode->originalParent ? structNode->originalParent : identifierRef->startScope->owner, Nte(Nte0030));
            notes.push_back(note);
        }
        else
        {
            Utf8           displayName;
            AstIdentifier* prevIdentifier = nullptr;
            if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier)
                prevIdentifier = CastAst<AstIdentifier>(identifierRef->previousResolvedNode, AstNodeKind::Identifier);
            if (!(identifierRef->startScope->flags & SCOPE_FILE))
                displayName = identifierRef->startScope->getDisplayFullName();
            if (displayName.empty() && !identifierRef->startScope->name.empty())
                displayName = identifierRef->startScope->name;
            if (displayName.empty() && typeRef)
                displayName = typeRef->name;
            if (!displayName.empty())
            {
                auto varDecl = node->findParent(AstNodeKind::VarDecl);
                auto idRef   = node->identifierRef();
                if (idRef && idRef->flags & AST_TUPLE_UNPACK && varDecl)
                {
                    diag = new Diagnostic{node, Fmt(Err(Err0821), varDecl->token.ctext(), displayName.c_str())};
                }
                else if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
                {
                    auto altEnum    = prevIdentifier->identifierExtension->alternateEnum;
                    diag            = new Diagnostic{node, node->token, Fmt(Err(Err0492), node->token.ctext(), altEnum->getDisplayNameC(), Naming::kindName(identifierRef->startScope->kind).c_str(), displayName.c_str())};
                    auto note       = Diagnostic::hereIs(altEnum->declNode, false, true);
                    note->showRange = false;
                    notes.push_back(note);
                }
                else if (typeWhere)
                {
                    if (typeWhere->kind == TypeInfoKind::Struct && node->callParameters)
                    {
                        if (node->token.text.startsWith(g_LangSpec->name_opVisit) && node->token.text != g_LangSpec->name_opVisit)
                        {
                            Utf8 variant{node->token.text.buffer + g_LangSpec->name_opVisit.length()};
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err1139), variant.c_str(), typeWhere->getDisplayNameC())};
                        }
                        else if (node->token.text == g_LangSpec->name_opVisit)
                        {
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err0557), typeWhere->getDisplayNameC())};
                        }
                        else
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err1107), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }
                    else if (typeWhere->kind == TypeInfoKind::Enum)
                    {
                        diag = new Diagnostic{node, node->token, Fmt(Err(Err0144), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }
                    else
                    {
                        diag = new Diagnostic{node, node->token, Fmt(Err(Err0112), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }

                    if (prevIdentifier && prevIdentifier->resolvedSymbolName && prevIdentifier->resolvedSymbolName->kind == SymbolKind::Variable)
                        diag->addRange(prevIdentifier, Diagnostic::isType(prevIdentifier));
                }
                else
                {
                    Utf8 where = Naming::kindName(identifierRef->startScope->kind).c_str();
                    diag       = new Diagnostic{node, node->token, Fmt(Err(Err0110), node->token.ctext(), where.c_str(), displayName.c_str())};
                }

                switch (identifierRef->startScope->owner->kind)
                {
                case AstNodeKind::StructDecl:
                case AstNodeKind::InterfaceDecl:
                case AstNodeKind::EnumDecl:
                {
                    auto note = Diagnostic::hereIs(identifierRef->startScope->owner, false, true);
                    notes.push_back(note);
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    VectorNative<OneTryMatch*> v;
    symbolErrorRemarks(context, v, node, diag);
    symbolErrorNotes(context, v, node, diag, notes);

    Utf8 appendMsg = findClosestMatchesMsg(node->token.text, scopeHierarchy, searchFor);
    if (!appendMsg.empty())
        notes.push_back(Diagnostic::note(appendMsg));

    context->report(*diag, notes);
}
