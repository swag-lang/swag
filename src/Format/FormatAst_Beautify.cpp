#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Tokenizer/TokenParse.h"

void FormatAst::beautifyComment(FormatContext& context, Vector<TokenComment>& comments) const
{
    bool first = true;
    for (const auto& v : comments)
    {
        if (!first)
        {
            concat->addEol();
            concat->addIndent(context.indent);
        }
        first = false;

        if (v.flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
        {
            concat->addBlankLine();
            concat->addIndent(context.indent);
        }

        Utf8 cmt;
        if (!v.flags.has(TOKEN_PARSE_ONE_LINE_COMMENT))
            cmt += "/*";
        else
            cmt += "//";
        cmt += v.comment;
        if (!v.flags.has(TOKEN_PARSE_ONE_LINE_COMMENT))
            cmt += "*/";

        concat->addString(cmt);
    }

    comments.clear();
}

void FormatAst::beautifyBefore(FormatContext& context, const AstNode* node) const
{
    beautifyCommentBefore(context, node);
    beautifyBlankLine(context, node);
    beautifyCommentJustBefore(context, node);
}

void FormatAst::beautifyCommentBefore(FormatContext& context, const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentBefore.empty())
        return;
    beautifyComment(context, node->extMisc()->format.commentBefore);
    concat->addEol();
    concat->addIndent(context.indent);
}

void FormatAst::beautifyCommentJustBefore(FormatContext& context, const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentJustBefore.empty())
        return;
    beautifyComment(context, node->extMisc()->format.commentJustBefore);
    concat->addEol();
    concat->addIndent(context.indent);
}

void FormatAst::beautifyBlankLine(FormatContext& context, const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;

    if (node->hasExtMisc() && node->extMisc()->format.flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
    {
        node->extMisc()->format.flags.remove(TOKEN_PARSE_BLANK_LINE_BEFORE);
        concat->addBlankLine();
        concat->addIndent(context.indent);
    }
}

void FormatAst::beautifyAfter(FormatContext& context, const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentAfterSameLine.empty())
        return;
    concat->addBlank();
    beautifyComment(context, node->extMisc()->format.commentAfterSameLine);
}
