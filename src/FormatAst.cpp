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
    Utf8 result;
    for (auto b = concat->firstBucket; b; b = b->nextBucket)
        result.append(reinterpret_cast<const char*>(b->data), concat->bucketCount(b));
    return result;
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
