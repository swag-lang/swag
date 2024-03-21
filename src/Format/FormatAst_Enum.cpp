#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputChildrenEnumValues(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    VectorNative<AstNode*> nodes;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;
        if (child->kind != AstNodeKind::EnumValue)
            break;
        if (child->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
            break;
        nodes.push_back(child);
    }

    processed = nodes.size();
    if (!processed)
        return true;

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

    return true;
}

bool FormatAst::outputEnumValue(FormatContext& context, AstNode* node)
{
    const auto enumNode = castAst<AstEnumValue>(node, AstNodeKind::EnumValue);
    if (enumNode->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
    {
        CONCAT_FIXED_STR(concat, "using");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    concat->addString(node->token.text);

    if (node->childCount())
    {
        concat->alignBlanks(node->token.text.length(), context.equalIndent);
        concat->addBlank();
        concat->addChar('=');
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        concat->addBlank();
        beautifyAfter(context, node);
    }
    else
    {
        concat->alignBlanks(node->token.text.length(), context.equalIndent);
        beautifyAfter(context, node);
    }

    return true;
}

bool FormatAst::outputEnum(FormatContext& context, AstEnum* node)
{
    concat->addIndent(context.indent);
    CONCAT_FIXED_STR(concat, "enum");
    concat->addBlank();
    concat->addString(node->token.text);

    // Raw type
    uint32_t first = 0;
    if (node->firstChild() && node->firstChild()->childCount())
    {
        first = 1;
        concat->addBlank();
        concat->addChar(':');
        concat->addBlank();
        SWAG_ASSERT(node->firstChild()->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    concat->addEol();
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();
    context.indent++;
    SWAG_CHECK(outputChildren(context, node, first));
    context.indent--;
    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
