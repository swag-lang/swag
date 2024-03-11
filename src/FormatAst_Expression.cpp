#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "Semantic.h"

bool FormatAst::outputArrayPointerSlicing(const AstNode* node)
{
    const auto arrayNode = castAst<AstArrayPointerSlicing>(node, AstNodeKind::ArrayPointerSlicing);
    SWAG_CHECK(outputNode(arrayNode->array));
    concat->addChar('[');
    SWAG_CHECK(outputNode(arrayNode->lowerBound));
    if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
        CONCAT_FIXED_STR(concat, "..<");
    else
        CONCAT_FIXED_STR(concat, "..");
    SWAG_CHECK(outputNode(arrayNode->upperBound));
    concat->addChar(']');
    return true;
}

bool FormatAst::outputArrayPointerIndex(const AstNode* node)
{
    const auto arrayNode = castAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);

    if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
    {
        if (arrayNode->parent->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar('(');
        CONCAT_FIXED_STR(concat, "dref");
        concat->addBlank();
        SWAG_CHECK(outputNode(arrayNode->array));
        if (arrayNode->parent->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar(')');
        return true;
    }

    SWAG_CHECK(outputNode(arrayNode->array));
    if (!arrayNode->array->is(AstNodeKind::ArrayPointerIndex) ||
        !arrayNode->array->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_MULTI_ACCESS) ||
        arrayNode->array->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL) != arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_SERIAL))
        concat->addChar('[');
    SWAG_CHECK(outputNode(arrayNode->access));
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

bool FormatAst::outputAffectOp(const AstNode* node)
{
    SWAG_CHECK(outputNode(node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);

    if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
        CONCAT_FIXED_STR(concat, ",over");
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_UP))
        CONCAT_FIXED_STR(concat, ",up");

    if (node->secondChild()->is(AstNodeKind::NoDrop) || node->secondChild()->is(AstNodeKind::Move))
        SWAG_CHECK(outputNode(node->secondChild()));
    else
    {
        concat->addBlank();
        SWAG_CHECK(outputNode(node->secondChild()));
    }

    return true;
}

bool FormatAst::outputFactorOp(const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
        CONCAT_FIXED_STR(concat, ",over");
    if (node->hasSpecFlag(AstOp::SPEC_FLAG_UP))
        CONCAT_FIXED_STR(concat, ",up");
    concat->addBlank();
    SWAG_CHECK(outputNode(node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    return true;
}

bool FormatAst::outputBinaryOp(const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(node->firstChild()));
    concat->addBlank();
    concat->addString(node->token.text);
    concat->addBlank();
    SWAG_CHECK(outputNode(node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');

    return true;
}

bool FormatAst::outputNullConditionalExpression(const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(node->firstChild()));
    concat->addBlank();
    CONCAT_FIXED_STR(concat, "orelse");
    concat->addBlank();
    SWAG_CHECK(outputNode(node->secondChild()));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');
    return true;
}

bool FormatAst::outputConditionalExpression(const AstNode* node)
{
    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar('(');

    SWAG_CHECK(outputNode(node->firstChild()));
    concat->addBlank();
    concat->addChar('?');
    concat->addBlank();
    SWAG_CHECK(outputNode(node->secondChild()));
    concat->addBlank();
    concat->addChar(':');
    concat->addBlank();
    SWAG_CHECK(outputNode(node->children[2]));

    if (node->hasAstFlag(AST_EXPR_IN_PARENTS))
        concat->addChar(')');
    return true;
}
