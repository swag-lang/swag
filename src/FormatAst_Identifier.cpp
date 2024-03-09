#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "Semantic.h"

bool FormatAst::outputIdentifier(const AstNode* node)
{
    const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);

    if (identifier->identifierExtension && identifier->identifierExtension->scopeUpMode != IdentifierScopeUpMode::None)
    {
        CONCAT_FIXED_STR(concat, "#up");
        if (identifier->identifierExtension->scopeUpMode == IdentifierScopeUpMode::Count &&
            identifier->identifierExtension->scopeUpValue.literalValue.u8 > 1)
        {
            concat->addChar('(');
            concat->addStringFormat("%d", identifier->identifierExtension->scopeUpValue.literalValue.u8);
            concat->addChar(')');
        }

        concat->addBlank();
    }

    if (identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_SELF))
        CONCAT_FIXED_STR(concat, "Self");
    else
        concat->addString(node->token.text);

    if (identifier->genericParameters)
    {
        concat->addChar('\'');
        if (identifier->genericParameters->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar('(');

        SWAG_CHECK(outputNode(identifier->genericParameters, false));

        if (identifier->genericParameters->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar(')');

        beautifyCommentAfterSameLine(identifier->genericParameters);
    }

    if (identifier->callParameters)
    {
        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
            concat->addChar('{');
        else
            concat->addChar('(');

        SWAG_CHECK(outputNode(identifier->callParameters, false));

        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
            concat->addChar('}');
        else if (identifier->callParameters->children.empty() || identifier->callParameters->lastChild()->children.empty())
            concat->addChar(')');
        else if (identifier->callParameters->lastChild()->lastChild()->isNot(AstNodeKind::CompilerCode))
            concat->addChar(')');

        beautifyCommentAfterSameLine(identifier->callParameters);
    }

    return true;
}

bool FormatAst::outputIdentifierRef(const AstNode* node)
{
    if (node->hasAstFlag(AST_DISCARD))
    {
        CONCAT_FIXED_STR(concat, "discard");
        concat->addBlank();
    }

    if (node->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE))
    {
        concat->addChar('.');
    }

    bool first = true;
    for (const auto child : node->children)
    {
        if (!first)
            concat->addChar('.');
        SWAG_CHECK(outputNode(child));
        first = false;
    }

    return true;
}
