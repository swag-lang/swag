#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Semantic.h"

AstInline* Ast::newInline(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node = Ast::newNode<AstInline>(parser, AstNodeKind::Inline, sourceFile, parent);
	node->allocateExtension(ExtensionKind::Semantic);
	node->extSemantic()->semanticAfterFct  = Semantic::resolveInlineAfter;
	node->extSemantic()->semanticBeforeFct = Semantic::resolveInlineBefore;
	node->setBcNotifyBefore(ByteCodeGen::emitInlineBefore);
	node->byteCodeFct = ByteCodeGen::emitInline;
	return node;
}

AstNode* Ast::newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, Parser* parser)
{
	const auto node   = Ast::newNode<AstOp>(parser, AstNodeKind::AffectOp, sourceFile, parent);
	node->semanticFct = Semantic::resolveAffect;
	node->addAttribute(attributeFlags);
	node->addSpecFlag(opFlags);
	return node;
}

AstStruct* Ast::newStructDecl(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstStruct>(parser, AstNodeKind::StructDecl, sourceFile, parent);
	node->semanticFct = Semantic::resolveStruct;
	return node;
}

AstNode* Ast::newFuncDeclParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstNode>(parser, AstNodeKind::FuncDeclParams, sourceFile, parent);
	node->semanticFct = Semantic::resolveFuncDeclParams;
	node->addAstFlag(AST_NO_BYTECODE_CHILDREN); // We do not want default assignations to generate bytecode
	return node;
}

AstFuncCallParams* Ast::newFuncCallGenParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
	node->semanticFct = Semantic::resolveFuncCallGenParams;
	return node;
}

AstFuncCallParams* Ast::newFuncCallParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
	node->semanticFct = Semantic::resolveFuncCallParams;
	return node;
}

AstFuncCallParam* Ast::newFuncCallParam(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstFuncCallParam>(parser, AstNodeKind::FuncCallParam, sourceFile, parent);
	node->semanticFct = Semantic::resolveFuncCallParam;
	return node;
}

AstVarDecl* Ast::newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser, AstNodeKind kind)
{
	const auto node   = Ast::newNode<AstVarDecl>(parser, kind, sourceFile, parent);
	node->token.text  = name;
	node->semanticFct = Semantic::resolveVarDecl;
	return node;
}

AstIntrinsicProp* Ast::newIntrinsicProp(SourceFile* sourceFile, TokenId id, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstIntrinsicProp>(parser, AstNodeKind::IntrinsicProp, sourceFile, parent);
	node->tokenId     = id;
	node->semanticFct = Semantic::resolveIntrinsicProperty;
	return node;
}

AstTypeExpression* Ast::newTypeExpression(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstTypeExpression>(parser, AstNodeKind::TypeExpression, sourceFile, parent);
	node->semanticFct = Semantic::resolveType;
	return node;
}

AstIdentifier* Ast::newIdentifier(SourceFile* sourceFile, const Utf8& name, const AstIdentifierRef* identifierRef, AstNode* parent, Parser* parser)
{
	const auto node   = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, parent);
	node->token.text  = name;
	node->semanticFct = Semantic::resolveIdentifier;
	if (identifierRef)
		node->inheritOwners(identifierRef);
	return node;
}

AstIdentifierRef* Ast::newIdentifierRef(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
	const auto node = Ast::newNode<AstIdentifierRef>(parser, AstNodeKind::IdentifierRef, sourceFile, parent);
	node->allocateExtension(ExtensionKind::Semantic);
	node->extSemantic()->semanticBeforeFct = Semantic::preResolveIdentifierRef;
	node->semanticFct                      = Semantic::resolveIdentifierRef;
	node->byteCodeFct                      = ByteCodeGen::emitIdentifierRef;
	return node;
}

AstIdentifierRef* Ast::newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser)
{
	SWAG_ASSERT(!name.empty());

	const auto node  = newIdentifierRef(sourceFile, parent, parser);
	node->token.text = name;

	const auto id   = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, node);
	id->semanticFct = Semantic::resolveIdentifier;
	id->token.text  = name;
	id->tokenId     = TokenId::Identifier;
	id->inheritOwners(node);

	return node;
}

AstIdentifierRef* Ast::newMultiIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser)
{
	SWAG_ASSERT(!name.empty());

	const auto node  = newIdentifierRef(sourceFile, parent, parser);
	node->token.text = name;

	auto       pz    = name.buffer;
	const auto pzEnd = name.buffer + name.count;
	while (pz != pzEnd)
	{
		const auto pzStart = pz;
		while (*pz != '.' && pz != pzEnd)
			pz++;

		const auto id        = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, node);
		id->semanticFct      = Semantic::resolveIdentifier;
		id->token.text.count = static_cast<int>(pz - pzStart);
		if (name.allocated)
		{
			id->token.text = Utf8{pzStart, id->token.text.count};
		}
		else
		{
			id->token.text.buffer    = pzStart;
			id->token.text.allocated = name.allocated;
		}

		id->tokenId = TokenId::Identifier;

		if (pz != pzEnd)
			pz++;
	}

	return node;
}
