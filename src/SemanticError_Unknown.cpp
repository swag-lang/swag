#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

namespace
{
	Diagnostic* unknownIdentifierInScope(const AstIdentifierRef* identifierRef, AstIdentifier* node, Vector<const Diagnostic*>& notes)
	{
		auto typeRef = TypeManager::concreteType(identifierRef->typeInfo);
		if (typeRef)
			typeRef = typeRef->getFinalType();

		// Error inside a tuple
		if (typeRef && typeRef->isTuple())
		{
			const auto err        = new Diagnostic{node, FMT(Err(Err0716), node->token.c_str())};
			const auto structNode = castAst<AstStruct>(identifierRef->startScope->owner, AstNodeKind::StructDecl);
			const auto errNode    = structNode->originalParent ? structNode->originalParent : identifierRef->startScope->owner;
			const auto note       = Diagnostic::note(errNode, Nte(Nte0078));
			notes.push_back(note);
			return err;
		}

		AstIdentifier* prevIdentifier = nullptr;
		if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier)
			prevIdentifier = castAst<AstIdentifier>(identifierRef->previousResolvedNode, AstNodeKind::Identifier);

		const Utf8 whereScopeName = Naming::kindName(identifierRef->startScope->kind).c_str();

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

		const auto varDecl = node->findParent(AstNodeKind::VarDecl);
		const auto idRef   = node->identifierRef();
		if (idRef && varDecl && idRef->hasAstFlag(AST_TUPLE_UNPACK))
		{
			err = new Diagnostic{node, FMT(Err(Err0719), varDecl->token.c_str(), displayName.c_str())};
		}
		else if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
		{
			const auto altEnum = prevIdentifier->identifierExtension->alternateEnum;
			const auto msg     = FMT(Err(Err0714), node->token.c_str(), altEnum->getDisplayNameC(), whereScopeName.c_str(), displayName.c_str());
			err                = new Diagnostic{node, node->token, msg};
			notes.push_back(Diagnostic::hereIs(altEnum->declNode));
		}
		else if (!typeWhere)
		{
			err = new Diagnostic{node, node->token, FMT(Err(Err0713), node->token.c_str(), whereScopeName.c_str(), displayName.c_str())};
		}
		else if (typeWhere->kind == TypeInfoKind::Enum)
		{
			err = new Diagnostic{node, node->token, FMT(Err(Err0708), node->token.c_str(), typeWhere->getDisplayNameC())};
		}
		else if (typeWhere->kind == TypeInfoKind::Struct && node->token.text == g_LangSpec->name_opVisit)
		{
			err = new Diagnostic{node, node->token, FMT(Err(Err0420), typeWhere->getDisplayNameC())};
		}
		else if (typeWhere->kind == TypeInfoKind::Struct && node->token.text.startsWith(g_LangSpec->name_opVisit))
		{
			const auto visitNode = castAst<AstVisit>(node->findParent(AstNodeKind::Visit), AstNodeKind::Visit);
			const Utf8 variant{node->token.text.buffer + g_LangSpec->name_opVisit.length()};
			err = new Diagnostic{visitNode, visitNode->extraNameToken, FMT(Err(Err0419), variant.c_str(), typeWhere->getDisplayNameC())};
		}
		else if (typeWhere->kind == TypeInfoKind::Struct && node->callParameters)
		{
			err = new Diagnostic{node, node->token, FMT(Err(Err0723), node->token.c_str(), typeWhere->getDisplayNameC())};
		}
		else
		{
			err = new Diagnostic{node, node->token, FMT(Err(Err0715), node->token.c_str(), typeWhere->getDisplayNameC())};
		}

		// Variable before
		if (prevIdentifier &&
			prevIdentifier->resolvedSymbolName &&
			prevIdentifier->resolvedSymbolName->kind == SymbolKind::Variable &&
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
		if (identifier->identifierRef()->startScope || identifier == identifier->parent->children.front())
			return false;

		const auto prev = identifier->identifierRef()->children[identifier->childParentIdx() - 1];
		if (!prev->resolvedSymbolName)
			return false;

		if (prev->hasExtMisc() && prev->extMisc()->resolvedUserOpSymbolOverload)
		{
			const auto typeInfo = TypeManager::concreteType(prev->extMisc()->resolvedUserOpSymbolOverload->typeInfo);
			const auto msg      = FMT(Nte(Nte0114), prev->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str(), typeInfo->getDisplayNameC());
			const auto note     = Diagnostic::note(prev, msg);
			notes.push_back(note);
		}
		else if (prev->kind == AstNodeKind::ArrayPointerIndex)
		{
			const auto api       = castAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
			const auto typeArray = castTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
			const auto note      = Diagnostic::note(api->array, FMT(Nte(Nte0006), api->array->token.c_str(), typeArray->finalType->getDisplayNameC()));
			notes.push_back(note);
		}
		else
		{
			const Diagnostic* note;
			const auto        kindName = Naming::aKindName(prev->resolvedSymbolName->kind);
			if (prev->typeInfo)
				note = Diagnostic::note(prev, FMT(Nte(Nte0004), prev->token.c_str(), kindName.c_str(), prev->typeInfo->getDisplayNameC()));
			else
				note = Diagnostic::note(prev, FMT(Nte(Nte0003), prev->token.c_str(), kindName.c_str()));
			notes.push_back(note);
		}

		return true;
	}
}

void SemanticError::unknownIdentifierError(SemanticContext* context, const AstIdentifierRef* identifierRef, AstIdentifier* node)
{
	// What kind of thing to we search for ?
	auto searchFor = IdentifierSearchFor::Whatever;
	if (node->parent->parent && node->parent->parent->kind == AstNodeKind::TypeExpression && node->parent->children.back() == node)
		searchFor = IdentifierSearchFor::Type;
	else if (node->parent->parent && node->parent->parent->kind == AstNodeKind::AttrUse && node->parent->children.back() == node)
		searchFor = IdentifierSearchFor::Attribute;
	else if (node->callParameters && node->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
		searchFor = IdentifierSearchFor::Struct;
	else if (node->callParameters)
		searchFor = IdentifierSearchFor::Function;

	// Default message
	Diagnostic* err;
	switch (searchFor)
	{
	case IdentifierSearchFor::Function:
		if (node->token.text[0] == '#')
			err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0706), node->token.c_str())};
		else if (node->token.text[0] == '@')
			err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0721), node->token.c_str())};
		else
			err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0712), node->token.c_str())};
		break;
	case IdentifierSearchFor::Attribute:
		err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0705), node->token.c_str())};
		break;
	case IdentifierSearchFor::Type:
		err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0727), node->token.c_str())};
		break;
	case IdentifierSearchFor::Struct:
		err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0726), node->token.c_str())};
		break;
	default:
		err = new Diagnostic{node->sourceFile, node->token, FMT(Err(Err0717), node->token.c_str())};
		break;
	}

	Vector<const Diagnostic*> notes;

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
			err = specDiag;
	}

	commonErrorNotes(context, {}, node, err, notes);
	context->report(*err, notes);
}
