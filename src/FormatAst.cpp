#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"

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
