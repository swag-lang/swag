#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Semantic.h"

AstInline* Ast::newInline(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node = Ast::newNode<AstInline>(AstNodeKind::Inline, parser, parent, sourceFile);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticAfterFct  = Semantic::resolveInlineAfter;
    node->extSemantic()->semanticBeforeFct = Semantic::resolveInlineBefore;
    node->setBcNotifyBefore(ByteCodeGen::emitInlineBefore);
    node->byteCodeFct = ByteCodeGen::emitInline;
    return node;
}

AstNode* Ast::newAffectOp(SourceFile* sourceFile, AstNode* parent, SpecFlags specFlags, AttributeFlags attributeFlags, Parser* parser)
{
    const auto node   = Ast::newNode<AstOp>(AstNodeKind::AffectOp, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveAffect;
    node->addAttribute(attributeFlags);
    node->addSpecFlag(specFlags);
    return node;
}

AstStruct* Ast::newStructDecl(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstStruct>(AstNodeKind::StructDecl, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveStruct;
    return node;
}

AstNode* Ast::newFuncDeclParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveFuncDeclParams;
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN); // We do not want default assignations to generate bytecode
    return node;
}

AstFuncCallParams* Ast::newFuncCallGenParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstFuncCallParams>(AstNodeKind::FuncCallParams, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveFuncCallGenParams;
    return node;
}

AstFuncCallParams* Ast::newFuncCallParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstFuncCallParams>(AstNodeKind::FuncCallParams, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveFuncCallParams;
    return node;
}

AstFuncCallParam* Ast::newFuncCallParam(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstFuncCallParam>(AstNodeKind::FuncCallParam, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveFuncCallParam;
    return node;
}

AstVarDecl* Ast::newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser, AstNodeKind kind)
{
    const auto node   = Ast::newNode<AstVarDecl>(kind, parser, parent, sourceFile);
    node->token.text  = name;
    node->semanticFct = Semantic::resolveVarDecl;
    return node;
}

AstIntrinsicProp* Ast::newIntrinsicProp(SourceFile* sourceFile, TokenId id, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstIntrinsicProp>(AstNodeKind::IntrinsicProp, parser, parent, sourceFile);
    node->token.id    = id;
    node->semanticFct = Semantic::resolveIntrinsicProperty;
    return node;
}

AstTypeExpression* Ast::newTypeExpression(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstTypeExpression>(AstNodeKind::TypeExpression, parser, parent, sourceFile);
    node->semanticFct = Semantic::resolveType;
    return node;
}

AstIdentifier* Ast::newIdentifier(SourceFile* sourceFile, const Utf8& name, const AstIdentifierRef* identifierRef, AstNode* parent, Parser* parser)
{
    const auto node   = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, parent, sourceFile);
    node->token.text  = name;
    node->semanticFct = Semantic::resolveIdentifier;
    if (identifierRef)
        node->inheritOwners(identifierRef);
    return node;
}

AstIdentifierRef* Ast::newIdentifierRef(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    const auto node = Ast::newNode<AstIdentifierRef>(AstNodeKind::IdentifierRef, parser, parent, sourceFile);
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

    const auto id   = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, node, sourceFile);
    id->semanticFct = Semantic::resolveIdentifier;
    id->token.text  = name;
    id->token.id    = TokenId::Identifier;
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

        const auto id        = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, node, sourceFile);
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

        id->token.id = TokenId::Identifier;

        if (pz != pzEnd)
            pz++;
    }

    return node;
}
