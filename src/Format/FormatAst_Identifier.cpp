#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool FormatAst::outputIdentifier(FormatContext& context, const AstNode* node)
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

        SWAG_CHECK(outputNode(context, identifier->genericParameters, false));

        if (identifier->genericParameters->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar(')');

        beautifyAfter(context, identifier->genericParameters);
    }

    if (identifier->callParameters)
    {
        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
            concat->addChar('{');
        else
            concat->addChar('(');

        SWAG_CHECK(outputNode(context, identifier->callParameters, false));

        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        {
            concat->addChar('}');
        }
        else if (identifier->callParameters->children.empty() ||
                 identifier->callParameters->lastChild()->children.empty() ||
                 identifier->callParameters->lastChild()->lastChild()->isNot(AstNodeKind::CompilerCode) ||
                 !identifier->callParameters->lastChild()->lastChild()->hasSpecFlag(AstCompilerCode::SPEC_FLAG_FROM_NEXT))
        {
            concat->addChar(')');
        }

        beautifyAfter(context, identifier->callParameters);
    }

    return true;
}

bool FormatAst::outputIdentifierRef(FormatContext& context, const AstNode* node)
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
    for (const auto it : node->children)
    {
        const auto child = convertNode(context, it);
        if (!child)
        {
            first = false;
            continue;
        }

        if (child->hasSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL))
        {
            SWAG_CHECK(outputNode(context, child));
            continue;
        }

        if (!first)
            concat->addChar('.');
        first = false;

        SWAG_CHECK(outputNode(context, child));
    }

    return true;
}
