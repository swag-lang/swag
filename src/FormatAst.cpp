#include "pch.h"
#include "FormatAst.h"
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
        node = subExportNode;
    if (node->hasAstFlag(AST_GENERATED) && !node->hasAstFlag(AST_GENERATED_USER))
        return nullptr;
    return node;
}

bool FormatAst::outputChildren(const AstNode* node)
{
    if (!node)
        return true;

    for (const auto it : node->children)
    {
        const auto child = convertNode(it);
        if (!child)
            continue;

        concat->addIndent(indent);
        SWAG_CHECK(outputNode(child));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputCommaChildren(const AstNode* node)
{
    if (!node)
        return true;

    bool first = true;
    for (const auto it : node->children)
    {
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
    CONCAT_FIXED_STR(concat, "namespace");
    concat->addBlank();
    concat->addString(node->token.text);
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
    return true;
}
