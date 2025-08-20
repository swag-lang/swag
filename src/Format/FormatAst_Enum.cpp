#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputChildrenEnumValues(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.style.alignEnumValue)
        return true;

    VectorNative<AstNode*> nodes;
    if (!collectChildrenToAlign(context, STOP_CMT_BEFORE, node, start, nodes, processed, [](const AstNode* inNode) {
        if (inNode->isNot(AstNodeKind::EnumValue))
            return true;
        if (inNode->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
            return true;
        return false;
    }))
        return true;

    uint32_t maxLenName  = 0;
    uint32_t maxLenValue = 0;

    {
        PushConcatFormatTmp fmt{this, context};
        for (const auto child : nodes)
        {
            maxLenName = std::max(maxLenName, child->token.text.length());

            if (child->childCount())
            {
                tmpConcat.clear();
                SWAG_CHECK(outputNode(fmt.cxt, child->firstChild()));
                maxLenValue = std::max(maxLenValue, tmpConcat.length());
            }
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputEnumValue(context, child, maxLenName, maxLenValue));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputEnumValue(FormatContext& context, AstNode* node, uint32_t maxLenName, uint32_t maxLenValue)
{
    const auto enumNode = castAst<AstEnumValue>(node, AstNodeKind::EnumValue);
    inheritLastFormatAfter(nullptr, enumNode);

    if (enumNode->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
    {
        beautifyBefore(context, node);
        concat->addString("using");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    beautifyBefore(context, node);
    const auto startColumn = concat->column;
    concat->addString(node->token.text);
    concat->alignToColumn(startColumn + maxLenName);

    if (node->childCount())
    {
        concat->addBlank();
        concat->addChar('=');
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    if (maxLenValue)
        maxLenValue += 3;
    if (context.style.alignEnumValue)
        concat->alignToColumn(startColumn + maxLenName + maxLenValue + context.style.addBlanksBeforeAlignedLastLineComments);

    beautifyAfter(context, node);
    return true;
}

bool FormatAst::outputEnum(FormatContext& context, AstEnum* node)
{
    bool sameLine = false;
    if (context.style.keepSameLineEnum)
        sameLine = !hasEOLInside(node);

    concat->addIndent(context.indent);
    concat->addString("enum");
    concat->addBlank();
    concat->addString(node->token.text);

    // Raw type
    uint32_t first = 0;
    if (node->firstChild() && node->firstChild()->childCount())
    {
        first = 1;
        concat->addChar(':');
        concat->addBlank();
        SWAG_ASSERT(node->firstChild()->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    if (!sameLine)
    {
        concat->addEol();
        concat->addIndent(context.indent);
        concat->addChar('{');
        concat->addEol();
        context.indent++;
        SWAG_CHECK(outputChildrenEol(context, node, first));
        context.indent--;
        concat->addIndent(context.indent);
        concat->addChar('}');
        concat->addEol();
    }
    else
    {
        if (node->firstChild() && node->firstChild()->is(AstNodeKind::EnumType))
            first = 1;
        FormatContext cxt{context};
        cxt.style.alignEnumValue = false;
        concat->addBlank();
        concat->addChar('{');
        concat->addBlank();
        SWAG_CHECK(outputChildrenChar(cxt, node, ',', 0, first));
        concat->addBlank();
        concat->addChar('}');
        concat->addEol();
    }

    return true;
}
