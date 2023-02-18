
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Parser.h"

AstInline* Ast::newInline(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node = Ast::newNode<AstInline>(parser, AstNodeKind::Inline, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticAfterFct  = SemanticJob::resolveInlineAfter;
    node->extension->semantic->semanticBeforeFct = SemanticJob::resolveInlineBefore;
    node->allocateExtension(ExtensionKind::ByteCode);
    node->extension->bytecode->byteCodeBeforeFct = ByteCodeGenJob::emitInlineBefore;
    node->byteCodeFct                            = ByteCodeGenJob::emitInline;
    return node;
}

AstNode* Ast::newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, Parser* parser)
{
    auto node         = Ast::newNode<AstOp>(parser, AstNodeKind::AffectOp, sourceFile, parent, 2);
    node->semanticFct = SemanticJob::resolveAffect;
    node->specFlags |= opFlags;
    node->attributeFlags |= attributeFlags;
    return node;
}

AstStruct* Ast::newStructDecl(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstStruct>(parser, AstNodeKind::StructDecl, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveStruct;
    return node;
}

AstNode* Ast::newFuncDeclParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstNode>(parser, AstNodeKind::FuncDeclParams, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFuncDeclParams;
    node->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
    return node;
}

AstFuncCallParams* Ast::newFuncCallGenParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFuncCallGenParams;
    return node;
}

AstFuncCallParams* Ast::newFuncCallParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFuncCallParams;
    return node;
}

AstFuncCallParam* Ast::newFuncCallParam(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstFuncCallParam>(parser, AstNodeKind::FuncCallParam, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveFuncCallParam;
    return node;
}

AstVarDecl* Ast::newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser, AstNodeKind kind)
{
    auto node         = Ast::newNode<AstVarDecl>(parser, kind, sourceFile, parent, 2);
    node->token.text  = name;
    node->semanticFct = SemanticJob::resolveVarDecl;
    return node;
}

AstIntrinsicProp* Ast::newIntrinsicProp(SourceFile* sourceFile, TokenId id, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstIntrinsicProp>(parser, AstNodeKind::IntrinsicProp, sourceFile, parent);
    node->token.id    = id;
    node->semanticFct = SemanticJob::resolveIntrinsicProperty;
    return node;
}

AstTypeExpression* Ast::newTypeExpression(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstTypeExpression>(parser, AstNodeKind::TypeExpression, sourceFile, parent);
    node->semanticFct = SemanticJob::resolveType;
    return node;
}

AstIdentifier* Ast::newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, Parser* parser)
{
    auto node         = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, parent);
    node->token.text  = name;
    node->semanticFct = SemanticJob::resolveIdentifier;
    if (identifierRef)
        node->inheritOwners(identifierRef);
    return node;
}

AstIdentifierRef* Ast::newIdentifierRef(SourceFile* sourceFile, AstNode* parent, Parser* parser)
{
    auto node = Ast::newNode<AstIdentifierRef>(parser, AstNodeKind::IdentifierRef, sourceFile, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveIdentifierRef;
    node->semanticFct                            = SemanticJob::resolveIdentifierRef;
    node->byteCodeFct                            = ByteCodeGenJob::emitIdentifierRef;
    return node;
}

AstIdentifierRef* Ast::newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser)
{
    SWAG_ASSERT(!name.empty());

    auto node        = Ast::newIdentifierRef(sourceFile, parent, parser);
    node->token.text = name;
    if (parser && !parser->currentTokenLocation)
        node->inheritTokenLocation(parser->token);

    Utf8 str;
    int  cpt = 0;
    auto pz  = name.buffer;
    while (*pz && cpt != name.count)
    {
        auto pzStart = pz;
        while (*pz && *pz != '.' && cpt != name.count)
        {
            pz++;
            cpt++;
        }

        auto id         = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, node);
        id->semanticFct = SemanticJob::resolveIdentifier;
        str.buffer      = pzStart;
        str.count       = (int) (pz - pzStart);
        str.allocated   = name.allocated;
        id->token.text  = str;

        id->token.id = TokenId::Identifier;
        if (parser && !parser->currentTokenLocation)
            id->inheritTokenLocation(parser->token);
        id->inheritOwners(node);

        if (*pz && cpt != name.count)
        {
            cpt++;
            pz++;
        }
    }

    str.buffer = nullptr; // to avoid free on destruction
    return node;
}
