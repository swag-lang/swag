#include "pch.h"
#include "FormatAst.h"
#include "Semantic.h"
#include "TokenParse.h"

void FormatAst::beautifyComment(Vector<TokenComment>& comments) const
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

    comments.clear();
}

void FormatAst::beautifyBefore(const AstNode* node) const
{
    beautifyCommentBefore(node);
    beautifyBlankLine(node);
    beautifyCommentJustBefore(node);
}

void FormatAst::beautifyCommentBefore(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentBefore.empty())
        return;
    beautifyComment(node->extMisc()->format.commentBefore);
    concat->addEol();
    concat->addIndent(indent);
}

void FormatAst::beautifyCommentJustBefore(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentJustBefore.empty())
        return;
    beautifyComment(node->extMisc()->format.commentJustBefore);
    concat->addEol();
    concat->addIndent(indent);
}

void FormatAst::beautifyBlankLine(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;

    if (node->hasExtMisc() && node->extMisc()->format.flags.has(TOKEN_PARSE_BLANK_LINE_BEFORE))
    {
        node->extMisc()->format.flags.remove(TOKEN_PARSE_BLANK_LINE_BEFORE);
        concat->addBlankLine();
        concat->addIndent(indent);
    }
}

void FormatAst::beautifyAfter(const AstNode* node) const
{
    if (!fmtFlags.has(FORMAT_FOR_BEAUTIFY))
        return;
    if (!node->hasExtMisc() || node->extMisc()->format.commentAfterSameLine.empty())
        return;
    concat->addBlank();
    beautifyComment(node->extMisc()->format.commentAfterSameLine);
}
