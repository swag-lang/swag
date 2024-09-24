#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

AstInline* Ast::newInline(Parser* parser, AstNode* parent)
{
    const auto node = Ast::newNode<AstInline>(AstNodeKind::Inline, parser, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticAfterFct  = Semantic::resolveInlineAfter;
    node->extSemantic()->semanticBeforeFct = Semantic::resolveInlineBefore;
    node->setBcNotifyBefore(ByteCodeGen::emitInlineBefore);
    node->byteCodeFct = ByteCodeGen::emitInline;
    return node;
}

AstNode* Ast::newAffectOp(SpecFlags specFlags, const AttributeFlags& attributeFlags, Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstOp>(AstNodeKind::AffectOp, parser, parent);
    node->semanticFct = Semantic::resolveAffect;
    node->addAttribute(attributeFlags);
    node->addSpecFlag(specFlags);
    return node;
}

AstStruct* Ast::newStructDecl(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstStruct>(AstNodeKind::StructDecl, parser, parent);
    node->semanticFct = Semantic::resolveStruct;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticAfterFct = Semantic::postResolveStruct;
    return node;
}

AstNode* Ast::newFuncDeclParams(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, parser, parent);
    node->semanticFct = Semantic::resolveFuncDeclParams;
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN); // We do not want default assignations to generate bytecode
    return node;
}

AstFuncCallParams* Ast::newFuncCallGenParams(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstFuncCallParams>(AstNodeKind::FuncCallParams, parser, parent);
    node->semanticFct = Semantic::resolveFuncCallGenParams;
    return node;
}

AstFuncCallParams* Ast::newFuncCallParams(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstFuncCallParams>(AstNodeKind::FuncCallParams, parser, parent);
    node->semanticFct = Semantic::resolveFuncCallParams;
    return node;
}

AstFuncCallParam* Ast::newFuncCallParam(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstFuncCallParam>(AstNodeKind::FuncCallParam, parser, parent);
    node->semanticFct = Semantic::resolveFuncCallParam;
    return node;
}

AstVarDecl* Ast::newVarDecl(const Utf8& name, Parser* parser, AstNode* parent, AstNodeKind kind)
{
    const auto node   = Ast::newNode<AstVarDecl>(kind, parser, parent);
    node->token.text  = name;
    node->semanticFct = Semantic::resolveVarDecl;
    return node;
}

AstIntrinsicProp* Ast::newIntrinsicProp(TokenId id, Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstIntrinsicProp>(AstNodeKind::IntrinsicProp, parser, parent);
    node->token.id    = id;
    node->semanticFct = Semantic::resolveIntrinsicProperty;
    return node;
}

AstTypeExpression* Ast::newTypeExpression(Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstTypeExpression>(AstNodeKind::TypeExpression, parser, parent);
    node->semanticFct = Semantic::resolveType;
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticAfterFct = Semantic::postResolveType;
    return node;
}

AstIdentifier* Ast::newIdentifier(const AstIdentifierRef* identifierRef, const Utf8& name, Parser* parser, AstNode* parent)
{
    const auto node   = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, parent);
    node->token.text  = name;
    node->semanticFct = Semantic::resolveIdentifier;
    if (identifierRef)
        node->inheritOwners(identifierRef);
    return node;
}

AstIdentifierRef* Ast::newIdentifierRef(Parser* parser, AstNode* parent)
{
    const auto node = Ast::newNode<AstIdentifierRef>(AstNodeKind::IdentifierRef, parser, parent);
    node->allocateExtension(ExtensionKind::Semantic);
    node->extSemantic()->semanticBeforeFct = Semantic::preResolveIdentifierRef;
    node->semanticFct                      = Semantic::resolveIdentifierRef;
    node->byteCodeFct                      = ByteCodeGen::emitIdentifierRef;
    return node;
}

AstIdentifierRef* Ast::newIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent)
{
    SWAG_ASSERT(!name.empty());

    const auto node  = newIdentifierRef(parser, parent);
    node->token.text = name;

    const auto id   = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, node);
    id->semanticFct = Semantic::resolveIdentifier;
    id->token.text  = name;
    id->token.id    = TokenId::Identifier;
    id->inheritOwners(node);

    return node;
}

AstIdentifierRef* Ast::newMultiIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent)
{
    SWAG_ASSERT(!name.empty());

    const auto node = newIdentifierRef(parser, parent);
    SWAG_ASSERT(node->token.sourceFile);
    node->token.text = name;

    auto       pz    = name.buffer;
    const auto pzEnd = name.buffer + name.count;
    while (pz != pzEnd)
    {
        const auto pzStart = pz;
        while (*pz != '.' && pz != pzEnd)
            pz++;

        const auto id = Ast::newNode<AstIdentifier>(AstNodeKind::Identifier, parser, node);
        SWAG_ASSERT(id->token.sourceFile);
        id->semanticFct      = Semantic::resolveIdentifier;
        id->token.text.count = static_cast<uint32_t>(pz - pzStart);
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
