#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Semantic.h"

void FormatAst::clear() const
{
    concat->clear();
}

Utf8 FormatAst::getUtf8() const
{
    return concat->getUtf8();
}

const AstNode* FormatAst::convertNode(const AstNode* node)
{
    if (!node)
        return nullptr;

    if (const auto subExportNode = node->extraPointer<AstNode>(ExtraPointerKind::ExportNode))
    {
        node = subExportNode;
        if (node->hasAstFlag(AST_GENERATED_EXCEPT_EXPORT))
            return node;
    }

    if (node->hasAstFlag(AST_GENERATED) && !node->hasAstFlag(AST_GENERATED_USER))
        return nullptr;

    return node;
}

bool FormatAst::outputChildren(const AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(it);
        if (!child)
            continue;

        concat->addIndent(indent);
        SWAG_CHECK(outputNode(child));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputCommaChildren(const AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    bool first = true;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(it);
        if (!child)
            continue;

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(child));
        first = false;
    }

    return true;
}

bool FormatAst::outputStatement(const AstNode* node)
{
    const auto stmt = castAst<AstStatement>(node, AstNodeKind::Statement);

    // Multi affectation a, b = ? is a syntax sugar for a special statement block
    if (stmt->hasSpecFlag(AstStatement::SPEC_FLAG_MULTI_AFFECT))
    {
        bool first = true;
        for (const auto c : stmt->children)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first         = false;
            const auto op = castAst<AstOp>(c, AstNodeKind::AffectOp);
            SWAG_CHECK(outputNode(op->firstChild()));
        }

        concat->addBlank();
        const auto firstOp = castAst<AstOp>(stmt->firstChild(), AstNodeKind::AffectOp);
        concat->addString(firstOp->token.text);
        concat->addBlank();
        SWAG_CHECK(outputNode(firstOp->secondChild()));
        concat->addEol();
        return true;
    }

    // Tuple unpacking (a, b) = ? is a syntax sugar for a special statement block
    if (stmt->hasSpecFlag(AstStatement::SPEC_FLAG_TUPLE_UNPACKING))
    {
        concat->addChar('(');

        bool first = true;
        for (uint32_t it = 1; it < stmt->children.size(); it++)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first            = false;
            const auto child = stmt->children[it];
            if (child->is(AstNodeKind::AffectOp))
            {
                const auto op = castAst<AstOp>(child, AstNodeKind::AffectOp);
                SWAG_CHECK(outputNode(op->firstChild()));
            }
            else if (child->is(AstNodeKind::IdentifierRef))
                SWAG_CHECK(outputNode(child));
            else
                SWAG_ASSERT(false);
        }

        concat->addChar(')');
        concat->addBlank();
        concat->addChar('=');
        concat->addBlank();
        const auto firstOp = castAst<AstVarDecl>(stmt->firstChild(), AstNodeKind::VarDecl);
        SWAG_CHECK(outputNode(firstOp->firstChild()));
        concat->addEol();
        return true;
    }

    if (node->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY))
    {
        concat->addEol();
        concat->addIndent(indent);
        concat->addChar('{');
        concat->addEol();
        indent++;
        SWAG_CHECK(outputChildren(node));
        indent--;
        concat->addIndent(indent);
        concat->addChar('}');
        concat->addEol();
    }
    else
    {
        SWAG_CHECK(outputNode(node->firstChild()));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputDoStatement(const AstNode* node)
{
    if (!node->childCount())
    {
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(node));
    }
    else if (node->is(AstNodeKind::Statement) && !node->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "do");
        concat->addEol();
        indent++;
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(node->firstChild()));
        indent--;
        concat->addEol();
    }
    else
    {
        outputNode(node);
    }

    return true;
}

bool FormatAst::outputNamespace(const AstNode* node)
{
    if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_GENERATED_TOP_LEVEL))
    {
        SWAG_CHECK(outputChildren(node));
        return true;
    }

    if (node->hasAstFlag(AST_GLOBAL_NODE))
    {
        CONCAT_FIXED_STR(concat, "#global");
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "namespace");
        concat->addBlank();
        concat->addString(node->token.text);
        concat->addEol();
        outputChildren(node);
        return true;
    }

    concat->addIndent(indent);

    if (!node->hasSpecFlag(AstNameSpace::SPEC_FLAG_PRIVATE))
    {
        CONCAT_FIXED_STR(concat, "namespace");
        concat->addBlank();
        concat->addString(node->token.text);
    }

    if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_NO_CURLY))
    {
        concat->addBlank();
        outputNode(node->firstChild());
    }
    else
    {
        concat->addEol();
        concat->addIndent(indent);
        concat->addChar('{');
        concat->addEol();
        indent++;
        outputChildren(node);
        indent--;
        concat->addIndent(indent);
        concat->addChar('}');
        concat->addEol();
    }

    return true;
}
