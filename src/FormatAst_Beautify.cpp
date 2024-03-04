#include "pch.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "Semantic.h"
#include "TokenParse.h"

void FormatAst::beautifyComment(const Vector<TokenComment>& comments) const
{
    bool first = true;
    for (const auto& v : comments)
    {
        if (!first)
        {
            concat->addEol();
            concat->addIndent(indent);
        }
        first = false;

        if (v.flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
        {
            concat->addBlankLine();
            concat->addIndent(indent);
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
}

void FormatAst::beautifyCommentBefore(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->comments.before.empty())
        return;
    beautifyComment(node->extMisc()->comments.before);
    concat->addEol();
    concat->addIndent(indent);
}

void FormatAst::beautifyCommentJustBefore(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->comments.justBefore.empty())
        return;
    beautifyComment(node->extMisc()->comments.justBefore);
    concat->addEol();
    concat->addIndent(indent);
}

void FormatAst::beautifyBlankLine(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;

    if (node->formatFlags.has(FMTFLAG_BLANK_LINE_BEFORE))
    {
        concat->addBlankLine();
        concat->addIndent(indent);
    }
}

void FormatAst::beautifyCommentAfterSameLine(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->comments.afterSameLine.empty())
        return;
    concat->addBlank();
    beautifyComment(node->extMisc()->comments.afterSameLine);
}
