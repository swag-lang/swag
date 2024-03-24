#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool FormatAst::outputStatement(FormatContext& context, AstNode* node)
{
    const auto stmt = castAst<AstStatement>(node, AstNodeKind::Statement);

    // Multi affectation a, b = ? is a syntax sugar for a special statement block
    if (stmt->hasSpecFlag(AstStatement::SPEC_FLAG_MULTI_AFFECT))
    {
        bool first = true;
        for (const auto c : stmt->children)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first         = false;
            const auto op = castAst<AstOp>(c, AstNodeKind::AffectOp);
            SWAG_CHECK(outputNode(context, op->firstChild()));
        }

        concat->addBlank();
        const auto firstOp = castAst<AstOp>(stmt->firstChild(), AstNodeKind::AffectOp);
        concat->addString(firstOp->token.text);
        concat->addBlank();
        SWAG_CHECK(outputNode(context, firstOp->secondChild()));
        concat->addEol();
        return true;
    }

    // Tuple unpacking (a, b) = ? is a syntax sugar for a special statement block
    if (stmt->hasSpecFlag(AstStatement::SPEC_FLAG_TUPLE_UNPACKING))
    {
        concat->addChar('(');

        bool first = true;
        for (uint32_t it = 1; it < stmt->children.size(); it++)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first            = false;
            const auto child = stmt->children[it];
            if (child->is(AstNodeKind::AffectOp))
            {
                const auto op = castAst<AstOp>(child, AstNodeKind::AffectOp);
                SWAG_CHECK(outputNode(context, op->firstChild()));
            }
            else if (child->is(AstNodeKind::IdentifierRef))
                SWAG_CHECK(outputNode(context, child));
            else
                SWAG_ASSERT(false);
        }

        concat->addChar(')');
        concat->addBlank();
        concat->addChar('=');
        concat->addBlank();
        const auto firstOp = castAst<AstVarDecl>(stmt->firstChild(), AstNodeKind::VarDecl);
        SWAG_CHECK(outputNode(context, firstOp->firstChild()));
        concat->addEol();
        return true;
    }

    if (node->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY))
    {
        concat->addEol();
        concat->addIndent(context.indent);
        concat->addChar('{');
        concat->addEol();
        context.indent++;
        SWAG_CHECK(outputChildren(context, node));
        context.indent--;
        concat->addIndent(context.indent);
        concat->addChar('}');
        concat->addEol();
    }
    else
    {
        SWAG_CHECK(outputNode(context, node->firstChild()));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputDoStatement(FormatContext& context, AstNode* node)
{
    if (!node->childCount())
    {
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, node));
    }
    else if (node->is(AstNodeKind::Statement) && !node->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "do");
        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, node->firstChild()));
        context.indent--;
        concat->addEol();
    }
    else
    {
        outputNode(context, node);
    }

    return true;
}

bool FormatAst::outputNamespace(FormatContext& context, AstNode* node)
{
    if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_GENERATED_TOP_LEVEL))
    {
        SWAG_CHECK(outputChildren(context, node));
        return true;
    }

    if (node->hasAstFlag(AST_GLOBAL_NODE))
    {
        CONCAT_FIXED_STR(concat, "#global");
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "namespace");
        concat->addBlank();
        concat->addString(node->token.text);
        concat->addEol();
        outputChildren(context, node);
        return true;
    }

    if (!node->hasSpecFlag(AstNameSpace::SPEC_FLAG_PRIVATE))
    {
        concat->addIndent(context.indent);

        if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_USING))
        {
            CONCAT_FIXED_STR(concat, "using");
            concat->addBlank();
        }

        CONCAT_FIXED_STR(concat, "namespace");
        concat->addBlank();
        concat->addString(node->token.text);
    }

    if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_NO_CURLY))
    {
        outputChildren(context, node);
    }
    else
    {
        concat->addEol();
        concat->addIndent(context.indent);
        concat->addChar('{');
        concat->addEol();
        context.indent++;
        outputChildren(context, node);
        context.indent--;
        concat->addIndent(context.indent);
        concat->addChar('}');
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputDefer(FormatContext& context, AstNode* node)
{
    const auto deferNode = castAst<AstDefer>(node, AstNodeKind::Defer);

    CONCAT_FIXED_STR(concat, "defer");
    switch (deferNode->deferKind)
    {
        case DeferKind::Error:
            concat->addChar('(');
            CONCAT_FIXED_STR(concat, "err");
            concat->addChar(')');
            break;
        case DeferKind::NoError:
            concat->addChar('(');
            CONCAT_FIXED_STR(concat, "noerr");
            concat->addChar(')');
            break;
    }

    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
    concat->addEol();
    return true;
}

bool FormatAst::outputTryAssume(FormatContext& context, const AstNode* node)
{
    if (node->hasSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED) && node->hasSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK))
    {
        context.indent++;
        outputChildren(context, node->firstChild());
        context.indent--;
        return true;
    }

    if (node->hasAstFlag(AST_DISCARD))
    {
        CONCAT_FIXED_STR(concat, "discard");
        concat->addBlank();
    }

    concat->addString(node->token.text);
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
    return true;
}

bool FormatAst::outputCatch(FormatContext& context, const AstNode* node)
{
    if (node->hasAstFlag(AST_DISCARD))
    {
        CONCAT_FIXED_STR(concat, "discard");
        concat->addBlank();
    }

    concat->addString(node->token.text);
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
    return true;
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

        if (child->kind == AstNodeKind::AffectOp && child->token.text == "=")
        {
            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenAffectOp(context, node, i, processed));
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

bool FormatAst::outputCommaChildren(const FormatContext& context, AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    FormatContext cxt{context};
    cxt.alignStructVarTypeAddBlanks = 0;

    bool first = true;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(cxt, it);
        if (!child)
            continue;

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(cxt, child));
        first = false;
    }

    return true;
}
