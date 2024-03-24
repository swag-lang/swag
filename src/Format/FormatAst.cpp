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
        const auto to = scan->getTokenParse();
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
    inheritFormatBefore(parser, node, other->getTokenParse());
}

void FormatAst::inheritFormatAfter(const Parser* parser, AstNode* node, AstNode* other)
{
    if (other == node || !other)
        return;
    if (parser && !parser->parserFlags.has(PARSER_TRACK_FORMAT) && !parser->parserFlags.has(PARSER_TRACK_DOCUMENTATION))
        return;
    inheritFormatAfter(parser, node, other->getTokenParse());
}

void FormatAst::inheritFormatBefore(const Parser* parser, AstNode* node, TokenParse* tokenParse)
{
    if(!node)
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
        const auto tp = node->getOrCreateTokenParse();
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
        const auto tp = node->getOrCreateTokenParse();
        tp->flags.add(tokenParse->flags);
        tp->comments.after = std::move(tokenParse->comments.after);
    }
}
