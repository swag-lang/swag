#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool FormatAst::outputArrayPointerSlicing(FormatContext& context, AstNode* node)
{
    const auto arrayNode = castAst<AstArrayPointerSlicing>(node, AstNodeKind::ArrayPointerSlicing);
    SWAG_CHECK(outputNode(context, arrayNode->array));
    concat->addChar('[');
    SWAG_CHECK(outputNode(context, arrayNode->lowerBound));
    if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
        CONCAT_FIXED_STR(concat, "..<");
    else
        CONCAT_FIXED_STR(concat, "..");
    SWAG_CHECK(outputNode(context, arrayNode->upperBound));
    concat->addChar(']');
    return true;
}

bool FormatAst::outputArrayPointerIndex(FormatContext& context, AstNode* node)
{
    const auto arrayNode = castAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);

    if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
    {
        if (arrayNode->parent->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar('(');
        CONCAT_FIXED_STR(concat, "dref");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, arrayNode->array));
        if (arrayNode->parent->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar(')');
        return true;
    }

    SWAG_CHECK(outputNode(context, arrayNode->array));
    if (!arrayNode->array->is(AstNodeKind::ArrayPointerIndex) ||
        !arrayNode->array->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_MULTI_ACCESS) ||
        arrayNode->array->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL) != arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
        concat->addChar('[');
    SWAG_CHECK(outputNode(context, arrayNode->access));
    if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_MULTI_ACCESS) &&
        arrayNode->parent->is(AstNodeKind::ArrayPointerIndex) &&
        arrayNode->parent->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL) == arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
    {
        concat->addChar(',');
        concat->addBlank();
    }
    else
        concat->addChar(']');

    return true;
}

bool FormatAst::outputChildrenAffectOp(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.alignEnumValue)
        return true;

    VectorNative<AstNode*> nodes;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
        {
            processed++;
            continue;
        }

        if (child->kind != AstNodeKind::AffectOp || child->token.text != "=")
            break;

        if (!nodes.empty())
        {
            if (const auto parse = child->firstChild()->getTokenParse())
            {
                if (!parse->comments.before.empty())
                    break;
                if (!parse->comments.justBefore.empty())
                    break;
                if (parse->flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
                    break;
            }
        }

        processed++;
        nodes.push_back(child);
    }

    if (nodes.size() <= 1)
    {
        processed = 0;
        return true;
    }

    uint32_t maxLenName  = 0;
    uint32_t maxLenValue = 0;

    {
        PushConcatFormatTmp fmt{this};
        FormatContext       cxt{context};
        cxt.outputComments   = false;
        cxt.outputBlankLines = false;

        for (const auto child : nodes)
        {
            tmpConcat.clear();
            SWAG_CHECK(outputNode(cxt, child->firstChild()));
            maxLenName = max(maxLenName, tmpConcat.length());

            tmpConcat.clear();
            SWAG_CHECK(outputNode(cxt, child->secondChild()));
            maxLenValue = max(maxLenValue, tmpConcat.length());
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputAffectOp(context, child, maxLenName, maxLenValue));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputAffectOp(FormatContext& context, const AstNode* node, uint32_t maxLenName, uint32_t maxLenValue)
{
    const auto startColumn = concat->column;
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->alignToColumn(startColumn + maxLenName);
    concat->addBlank();
    concat->addString(node->token.text);

    if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
        CONCAT_FIXED_STR(concat, ",over");
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_UP))
        CONCAT_FIXED_STR(concat, ",up");

    if (!node->secondChild()->is(AstNodeKind::NoDrop) && !node->secondChild()->is(AstNodeKind::Move))
        concat->addBlank();

    SWAG_CHECK(outputNode(context, node->secondChild()));
    return true;
}

bool FormatAst::outputFactorOp(FormatContext& context, const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
        CONCAT_FIXED_STR(concat, ",over");
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_UP))
        CONCAT_FIXED_STR(concat, ",up");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    return true;
}

bool FormatAst::outputBinaryOp(FormatContext& context, const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    const auto curColumn = concat->column;
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);
    concat->addBlank();

    if (const auto parse = node->secondChild()->getTokenParse())
    {
        if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE))
        {
            concat->addEol();
            concat->alignToColumn(curColumn);
        }
    }

    SWAG_CHECK(outputNode(context, node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    return true;
}

bool FormatAst::outputNullConditionalExpression(FormatContext& context, const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    CONCAT_FIXED_STR(concat, "orelse");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');
    return true;
}

bool FormatAst::outputConditionalExpression(FormatContext& context, AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addChar('?');
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->secondChild()));
    concat->addBlank();
    concat->addChar(':');
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->children[2]));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');
    return true;
}

bool FormatAst::outputCast(FormatContext& context, const AstNode* node)
{
    CONCAT_FIXED_STR(concat, "cast");
    if (node->hasSpecFlag(AstCast::SPEC_FLAG_OVERFLOW))
        CONCAT_FIXED_STR(concat, ",over");
    if (node->hasSpecFlag(AstCast::SPEC_FLAG_BIT))
        CONCAT_FIXED_STR(concat, ",bit");
    if (node->hasSpecFlag(AstCast::SPEC_FLAG_UN_CONST))
        CONCAT_FIXED_STR(concat, ",unconst");

    concat->addChar('(');
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addChar(')');

    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->secondChild()));
    return true;
}
