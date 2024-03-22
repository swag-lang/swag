#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/AstFlags.h"

void FormatAst::clear() const
{
    concat->clear();
}

Utf8 FormatAst::getUtf8() const
{
    return concat->getUtf8();
}

AstNode* FormatAst::convertNode(FormatContext&, AstNode* node)
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

bool FormatAst::outputChildren(FormatContext& context, AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (child->kind == AstNodeKind::EnumValue)
        {
            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenEnumValues(context, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }

        if (child->kind == AstNodeKind::VarDecl || child->kind == AstNodeKind::ConstDecl)
        {
            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenVar(context, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }        

        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, child));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputCommaChildren(FormatContext& context, AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    bool first = true;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(context, child));
        first = false;
    }

    return true;
}
