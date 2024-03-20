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
            VectorNative<AstNode*> nodes;
            for (uint32_t s = i; s < node->childCount(); s++)
            {
                const auto it1    = node->children[s];
                const auto child1 = convertNode(context, it1);
                if (!child1)
                    continue;
                if (child1->kind != AstNodeKind::EnumValue)
                    break;
                if (child1->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
                    break;
                nodes.push_back(child1);
            }

            if (!nodes.empty())
            {
                uint32_t maxLen = 0;
                for (const auto s : nodes)
                {
                    maxLen = max(maxLen, s->token.text.length());
                }

                context.equalIndent = maxLen;
                for (const auto s : nodes)
                {
                    concat->addIndent(context.indent);
                    SWAG_CHECK(outputEnumValue(context, s));
                    concat->addEol();
                }
                context.equalIndent = 0;

                i += nodes.size() - 1;
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
