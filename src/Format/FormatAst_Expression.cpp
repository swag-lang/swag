#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputArrayPointerSlicing(FormatContext& context, AstNode* node)
{
    const auto arrayNode = castAst<AstArrayPointerSlicing>(node, AstNodeKind::ArrayPointerSlicing);
    SWAG_CHECK(outputNode(context, arrayNode->array));
    concat->addChar('[');
    SWAG_CHECK(outputNode(context, arrayNode->lowerBound));
    if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
        concat->addString("..<");
    else
        concat->addString("..");
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
        concat->addString("dref");
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

bool FormatAst::outputChildrenAffectEqual(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.alignAffectEqual)
        return true;

    VectorNative<AstNode*> nodes;
    if (!collectChildrenToAlign(context, STOP_CMT_BEFORE | STOP_EMPTY_LINE_BEFORE, node, start, nodes, processed, [](const AstNode* inNode) {
            if (inNode->kind != AstNodeKind::AffectOp || inNode->token.text != "=")
                return true;
            return false;
        }))
        return true;

    uint32_t maxLenName = 0;

    {
        PushConcatFormatTmp fmt{this, context};
        for (const auto child : nodes)
        {
            tmpConcat.clear();
            SWAG_CHECK(outputNode(fmt.cxt, child->firstChild()));
            maxLenName = std::max(maxLenName, tmpConcat.length());
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputAffectOp(context, child, maxLenName));
        addEOLOrSemi(context, child);
    }

    return true;
}

bool FormatAst::outputAffectOp(FormatContext& context, AstNode* node, uint32_t maxLenName)
{
    beautifyBefore(context, node);
    const auto startColumn = concat->column;
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->alignToColumn(startColumn + maxLenName);
    concat->addBlank();
    concat->addString(node->token.text);

    if (node->hasSpecFlag(AstOp::SPEC_FLAG_WRAP))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_sharp_wrap);
    }

    if (node->hasSpecFlag(AstOp::SPEC_FLAG_PROM))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_sharp_prom);
    }

    addBlank(node->secondChild());
    SWAG_CHECK(outputNode(context, node->secondChild()));
    beautifyAfter(context, node);
    return true;
}

bool FormatAst::outputFactorOp(FormatContext& context, const AstNode* node)
{
    if (node->childCount() < 2)
        return false;

    if (!context.countFactorOp)
        context.startFactorOpColumn = concat->column;
    context.countFactorOp++;

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);

    if (node->hasSpecFlag(AstOp::SPEC_FLAG_WRAP))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_sharp_wrap);
    }
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_PROM))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_sharp_prom);
    }

    concat->addBlank();

    if (const auto parse = getTokenParse(node->secondChild()))
    {
        if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE))
        {
            concat->addEol();
            concat->alignToColumn(context.startFactorOpColumn);
        }
    }

    SWAG_CHECK(outputNode(context, node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    context.countFactorOp--;
    return true;
}

bool FormatAst::outputBinaryOp(FormatContext& context, const AstNode* node)
{
    if (!context.countBinaryOp)
        context.startBinaryOpColumn = concat->column;
    context.countBinaryOp++;

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);
    concat->addBlank();

    if (const auto parse = getTokenParse(node->secondChild()))
    {
        if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE))
        {
            concat->addEol();
            concat->alignToColumn(context.startBinaryOpColumn);
        }
    }

    SWAG_CHECK(outputNode(context, node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    context.countBinaryOp--;
    return true;
}

bool FormatAst::outputNullConditionalExpression(FormatContext& context, const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addBlank();
    concat->addString("orelse");
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
    concat->addString("cast");

    if (node->hasSpecFlag(AstCast::SPEC_FLAG_OVERFLOW))
    {
        concat->addChar('<');
        concat->addString(g_LangSpec->name_overflow);
        concat->addChar('>');
    }
    else if (node->hasSpecFlag(AstCast::SPEC_FLAG_BIT))
    {
        concat->addChar('<');
        concat->addString(g_LangSpec->name_bit);
        concat->addChar('>');
    }
    else if (node->hasSpecFlag(AstCast::SPEC_FLAG_UN_CONST))
    {
        concat->addChar('<');
        concat->addString(g_LangSpec->name_unconst);
        concat->addChar('>');
    }

    concat->addChar('(');
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addChar(')');

    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->secondChild()));
    return true;
}
