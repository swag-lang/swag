#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"

void FormatAst::clear() const
{
    concat->clear();
}

Utf8 FormatAst::getUtf8() const
{
    return concat->getUtf8();
}

bool FormatAst::hasEOLInside(AstNode* node)
{
    if (const auto parse = getTokenParse(node))
    {
        if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE | TOKEN_PARSE_EOL_AFTER))
            return true;
        if (!parse->comments.before.empty())
            return true;
        if (!parse->comments.justBefore.empty())
            return true;
        if (!parse->comments.after.empty())
            return true;
    }

    for (const auto s : node->children)
    {
        if (hasEOLInside(s))
            return true;
    }

    return false;
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

void FormatAst::inheritLastFormatAfter(const Parser* parser, AstNode* node)
{
    auto scan = node;
    while (scan)
    {
        const auto to = getTokenParse(scan);
        if (to && !to->comments.after.empty())
        {
            inheritFormatAfter(parser, node, scan);
            break;
        }

        scan = scan->lastChild();
    }
}

void FormatAst::inheritFormatBefore(const Parser* parser, AstNode* node, AstNode* other)
{
    if (other == node || !other)
        return;
    if (!parser->parserFlags.has(PARSER_TRACK_FORMAT) && !parser->parserFlags.has(PARSER_TRACK_DOCUMENTATION))
        return;
    inheritFormatBefore(parser, node, getTokenParse(other));
}

void FormatAst::inheritFormatAfter(const Parser* parser, AstNode* node, AstNode* other)
{
    if (other == node || !other)
        return;
    if (parser && !parser->parserFlags.has(PARSER_TRACK_FORMAT) && !parser->parserFlags.has(PARSER_TRACK_DOCUMENTATION))
        return;
    inheritFormatAfter(parser, node, getTokenParse(other));
}

void FormatAst::inheritFormatBefore(const Parser* parser, AstNode* node, TokenParse* tokenParse)
{
    if (!node)
        return;
    if (!tokenParse)
        return;
    if (!parser->parserFlags.has(PARSER_TRACK_FORMAT) && !parser->parserFlags.has(PARSER_TRACK_DOCUMENTATION))
        return;

    if (tokenParse->flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE) ||
        tokenParse->flags.has(TOKEN_PARSE_EOL_BEFORE) ||
        !tokenParse->comments.before.empty() ||
        !tokenParse->comments.justBefore.empty())
    {
        const auto tp = getOrCreateTokenParse(node);
        tp->flags.add(tokenParse->flags);
        tp->comments.before     = std::move(tokenParse->comments.before);
        tp->comments.justBefore = std::move(tokenParse->comments.justBefore);
    }
}

void FormatAst::inheritFormatAfter(const Parser* parser, AstNode* node, TokenParse* tokenParse)
{
    if (!tokenParse)
        return;
    if (parser && !parser->parserFlags.has(PARSER_TRACK_FORMAT) && !parser->parserFlags.has(PARSER_TRACK_DOCUMENTATION))
        return;

    if (tokenParse->flags.has(TOKEN_PARSE_EOL_AFTER) ||
        !tokenParse->comments.after.empty())
    {
        const auto tp = getOrCreateTokenParse(node);
        tp->flags.add(tokenParse->flags);
        tp->comments.after = std::move(tokenParse->comments.after);
    }
}

TokenParse* FormatAst::getTokenParse(AstNode* node)
{
    return node->extraPointer<TokenParse>(ExtraPointerKind::TokenParse);
}

const TokenParse* FormatAst::getTokenParse(const AstNode* node)
{
    return node->extraPointer<TokenParse>(ExtraPointerKind::TokenParse);
}

TokenParse* FormatAst::getOrCreateTokenParse(AstNode* node)
{
    TokenParse* tp = getTokenParse(node);
    if (!tp)
    {
        tp = Allocator::alloc<TokenParse>();
        node->addExtraPointer(ExtraPointerKind::TokenParse, tp);
    }

    return tp;
}

bool FormatAst::outputChildrenEol(FormatContext& context, AstNode* node, uint32_t start)
{
    if (!node)
        return true;

    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (child->kind == AstNodeKind::TypeAlias)
        {
            FormatContext cxt{context};
            cxt.alignTypeAlias = true;

            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenTypeAlias(cxt, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }

        if (child->kind == AstNodeKind::FuncDecl)
        {
            FormatContext cxt{context};
            cxt.alignShortFunc = true;

            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenFuncDecl(cxt, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }

        if (child->kind == AstNodeKind::EnumValue)
        {
            FormatContext cxt{context};
            cxt.alignEnumValue = true;

            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenEnumValues(cxt, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }

        if (child->kind == AstNodeKind::VarDecl || child->kind == AstNodeKind::ConstDecl)
        {
            FormatContext cxt{context};
            cxt.alignVarDecl = true;

            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenVar(cxt, node, i, processed));
            if (processed)
            {
                i += processed - 1;
                continue;
            }
        }

        if (child->kind == AstNodeKind::AffectOp && child->token.text == "=")
        {
            FormatContext cxt{context};
            cxt.alignAffectEqual = true;

            uint32_t processed = 0;
            SWAG_CHECK(outputChildrenAffectEqual(cxt, node, i, processed));
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

bool FormatAst::outputChildrenChar(FormatContext& context, AstNode* node, char c, uint32_t start)
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
            if (c)
                concat->addChar(c);
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(context, child));
        first = false;
    }

    return true;
}

bool FormatAst::outputChildrenBlank(FormatContext& context, AstNode* node, uint32_t start)
{
    return outputChildrenChar(context, node, 0, start);
}

bool FormatAst::collectChildrenToAlign(FormatContext&                       context,
                                       CollectFlags                         flags,
                                       AstNode*                             node,
                                       uint32_t                             start,
                                       VectorNative<AstNode*>&              nodes,
                                       uint32_t&                            processed,
                                       const std::function<bool(AstNode*)>& stopFn)
{
    nodes.clear();
    processed = 0;

    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
        {
            processed++;
            continue;
        }

        if (stopFn(child))
            break;

        if (!nodes.empty())
        {
            if (const auto parse = getTokenParse(child))
            {
                if (flags.has(STOP_CMT_BEFORE))
                {
                    if (!parse->comments.before.empty())
                        break;
                    if (!parse->comments.justBefore.empty())
                        break;
                }

                if (flags.has(STOP_EMPTY_LINE_BEFORE))
                {
                    if (parse->flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
                        break;
                }
            }
        }

        processed++;
        nodes.push_back(child);
    }

    if (nodes.size() <= 1)
    {
        processed = 0;
        return false;
    }

    return true;
}
