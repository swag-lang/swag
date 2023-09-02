#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Workspace.h"
#include "Version.h"
#include "SyntaxColor.h"
#pragma optimize("", off)

static const char* START_NOTE      = "> NOTE:";
static const char* START_TIP       = "> TIP:";
static const char* START_WARNING   = "> WARNING:";
static const char* START_ATTENTION = "> ATTENTION:";
static const char* START_EXAMPLE   = "> EXAMPLE:";

void GenDoc::outputStyles()
{
    helpOutput += "<style>\n";

    helpOutput += "\n\
        .container  { display: flex; flex-wrap: nowrap; flex-direction: row; margin: 0px auto; padding: 0px; }\n\
        .left       { display: block; overflow-y: scroll; width: 500px; }\n\
        .left-page  { margin: 10px; }\n\
        .right      { display: block; width: 100%; }\n\
        .right-page { max-width: 1024px; margin: 10px auto; }\n\
        \n\
        @media(min-width: 640px)  { .container { max-width: 640px; }}\n\
        @media(min-width: 768px)  { .container { max-width: 768px; }}\n\
        @media(min-width: 1024px) { .container { max-width: 1024px; }}\n\
        @media(min-width: 1280px) { .container { max-width: 1280px; }}\n\
        @media(min-width: 1536px) { .container { max-width: 1536px; }}\n\
        \n\
        @media screen and (max-width: 600px) {\n\
            .left       { display: none; }\n\
            .right-page { margin:  10px; }\n\
        }\n";

    // Layout
    if (docKind != BuildCfgDocKind::Pages)
    {
        helpOutput += "\n\
            .container { height: 100vh; }\n\
            .right     { overflow-y: scroll; }\n";
    }

    helpOutput += "\n\
        body { margin: 0px; line-height: 1.3em; }\n\
        \n\
        .container a        { color:           DoggerBlue; }\n\
        .container a:hover  { text-decoration: underline; }\n\
        .container img      { margin:          0 auto; }\n\
        \n\
        .left a     { text-decoration: none; }\n\
        .left ul    { list-style-type: none; margin-left: -20px; }\n\
        .left h3    { background-color: Black; color: White; padding: 6px; }\n\
        .right h1   { margin-top: 50px; margin-bottom: 50px; }\n\
        .right h2   { margin-top: 35px; }\n\
        \n\
        .blockquote {\n\
            border-radius:      5px;\n\
            border:             1px solid;\n\
            margin:             20px;\n\
            margin-left:        20px;\n\
            margin-right:       20px;\n\
            padding:            10px;\n\
        }\n\
        .blockquote-default {\n\
            border-color:       Orange;\n\
            border-left:        6px solid Orange;\n\
            background-color:   LightYellow;\n\
        }\n\
        .blockquote-note {\n\
            border-color:       #ADCEDD;\n\
            background-color:   #CDEEFD;\n\
        }\n\
        .blockquote-tip {\n\
            border-color:       #BCCFBC;\n\
            background-color:   #DCEFDC;\n\
        }\n\
        .blockquote-warning {\n\
            border-color:       #DFBDB3;\n\
            background-color:   #FFDDD3;\n\
        }\n\
        .blockquote-attention {\n\
            border-color:       #DDBAB8;\n\
            background-color:   #FDDAD8;\n\
        }\n\
        .blockquote-example {\n\
            border:             2px solid LightGrey;\n\
        }\n\
        .blockquote-default     p:first-child { margin-top: 0px; }\n\
        .blockquote-default     p:last-child  { margin-bottom: 0px; }\n\
        .blockquote             p:last-child  { margin-bottom: 0px; }\n\
        .blockquote-title-block { margin-bottom:   10px; }\n\
        .blockquote-title       { font-weight:     bold; }\n\
        \n\
        table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }\n\
        .api-item td:first-child  { width: 33%; white-space: nowrap; }\n\
        .api-item-title-src-ref   { text-align:  right; }\n\
        .api-item-title-src-ref a { color:       inherit; }\n\
        .api-item-title-kind      { font-weight: normal; font-size: 80%; }\n\
        .api-item-title-light     { font-weight: normal; }\n\
        .api-item-title-strong    { font-weight: bold; font-size: 100%; }\n\
        .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }\n\
        \n\
        table.table-enumeration           { border: 1px solid LightGrey; border-collapse: collapse; width: calc(100% - 40px); font-size: 90%; margin-left: 20px; margin-right: 20px; }\n\
        .table-enumeration td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }\n\
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }\n\
        .table-enumeration td:last-child  { width: 100%; }\n\
        .table-enumeration td.code-type   { background-color: #eeeeee; }\n\
        .table-enumeration a              { text-decoration: none; color: inherit; }\n\
        \n\
        .code-inline            { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 0px 8px; border-radius: 5px; border: 1px dotted #cccccc; }\n\
        .code-block {\n\
            background-color:   #eeeeee;\n\
            border-radius:      5px;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            margin:             20px;\n\
            white-space:        pre;\n\
            overflow-x:         auto;\n\
        }\n\
        .code-block a { color: inherit; }\n\
        \n";

    float    lum        = module->buildCfg.genDoc.syntaxColorLum;
    uint32_t defaultCol = module->buildCfg.genDoc.syntaxDefaultColor;
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_CODE, defaultCol);
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_COMMENT, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxComment, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_COMPILER, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxCompiler, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_FUNCTION, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxFunction, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_CONSTANT, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxConstant, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_INTRINSIC, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxIntrinsic, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_TYPE, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxType, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_KEYWORD, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxKeyword, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_LOGIC, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxLogic, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_NUMBER, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxNumber, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_STRING, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxString, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_ATTRIBUTE, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxAttribute, lum));
    helpOutput += Fmt("    .%s { color: #%x; }\n", SYN_INVALID, getSyntaxColor(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxInvalid, lum));

    helpOutput += "</style>\n";
}

Utf8 GenDoc::toRef(Utf8 str)
{
    str.replace(".", "_");
    str.replace(" ", "_");
    return str;
}

const char* GenDoc::tokenizeReference(const char* pz, Utf8& name, Utf8& link)
{
    name.clear();
    link.clear();

    if (*pz != '[')
        return nullptr;
    pz++;

    while (*pz && *pz != ']')
        name += *pz++;
    if (*pz != ']')
        return nullptr;

    pz++;
    if (*pz != '(')
        return pz;
    pz++;

    while (*pz && *pz != ')')
        link += *pz++;
    if (*pz != ')')
        return nullptr;
    return pz + 1;
}

Utf8 GenDoc::findReference(const Utf8& name)
{
    auto it = collectInvert.find(name);
    if (it != collectInvert.end())
        return Fmt("<a href=\"#%s\">%s</a>", toRef(it->second).c_str(), name.c_str());

    Vector<Utf8> tkns;
    Utf8::tokenize(name, '.', tkns);
    if (tkns.size() <= 1)
        return "";

    if (tkns[0] == "Swag")
    {
        return Fmt("<a href=\"swag.runtime.php#%s\">%s</a>", toRef(name).c_str(), name.c_str());
    }

    return "";
}

void GenDoc::outputCode(const Utf8& code, uint32_t flags)
{
    if (code.empty())
        return;

    // Remove trailing eol
    auto repl = code;
    repl.trim();
    while (repl.length() && repl[0] == '\n')
    {
        repl.remove(0, 1);
        repl.trim();
    }
    while (repl.length() && repl.back() == '\n')
    {
        repl.removeBack();
        repl.trim();
    }
    if (repl.empty())
        return;

    if (flags & GENDOC_CODE_BLOCK)
    {
        helpContent += "<div class=\"code-block\">";
        helpContent += "<code>";
    }

    // Kind of a hack for now... Try to keep references, but try to keep <> also...
    if (code.find("<a href") == -1)
    {
        repl.replace("<", "&lt;");
        repl.replace(">", "&gt;");
    }

    // Syntax coloration
    Utf8 codeText;
    if (flags & GENDOC_CODE_SYNTAX_COL)
        codeText = syntaxColor(repl, SyntaxColorMode::ForDoc);
    else
    {
        codeText = Fmt("<span class=\"%s\">", SYN_CODE);
        codeText += repl;
        codeText += "</span>";
    }

    // References
    if (!(flags & GENDOC_CODE_REFS))
        repl = std::move(codeText);
    else
    {
        repl.clear();
        const char* pz = codeText.c_str();
        while (*pz)
        {
            if (SWAG_IS_ALPHA(*pz) || *pz == '_')
            {
                Utf8 nameToRef;
                while (SWAG_IS_ALNUM(*pz) || *pz == '_' || *pz == '.')
                    nameToRef += *pz++;

                auto ref = findReference(nameToRef);
                if (ref.empty())
                    repl += nameToRef;
                else
                    repl += ref;
            }
            else
            {
                repl += *pz++;
            }
        }
    }

    helpContent += repl;

    if (flags & GENDOC_CODE_BLOCK)
    {
        helpContent += "</code>\n";
        helpContent += "</div>\n";
    }
}

void GenDoc::computeUserBlocks(Vector<UserBlock*>& blocks, Vector<Utf8>& lines, bool shortDesc)
{
    // Remove trailing '\r'
    for (auto& l : lines)
    {
        if (l.length() && l.back() == '\r')
            l.removeBack();
    }

    int start = 0;
    while (start < lines.size())
    {
        UserBlock* blk = new UserBlock;

        // Start of a block
        for (; start < lines.size(); start++)
        {
            auto line = lines[start];
            line.trim();

            // Zap blank lines
            if (line.empty())
                continue;

            if (lines[start].startsWith("    ") || lines[start].startsWith("\t"))
            {
                blk->kind = UserBlockKind::CodeAuto;
            }
            else if (line.startsWith("---"))
            {
                blk->kind = UserBlockKind::ParagraphRaw;
                start++;
            }
            else if (line.startsWith("```swag"))
            {
                blk->kind = UserBlockKind::CodeSwag;
                start++;
            }
            else if (line.startsWith("```"))
            {
                blk->kind = UserBlockKind::CodeRaw;
                start++;
            }
            else if (line.startsWith(START_NOTE))
            {
                blk->kind = UserBlockKind::BlockquoteNote;
            }
            else if (line.startsWith(START_TIP))
            {
                blk->kind = UserBlockKind::BlockquoteTip;
            }
            else if (line.startsWith(START_WARNING))
            {
                blk->kind = UserBlockKind::BlockquoteWarning;
            }
            else if (line.startsWith(START_ATTENTION))
            {
                blk->kind = UserBlockKind::BlockquoteAttention;
            }
            else if (line.startsWith(START_EXAMPLE))
            {
                blk->kind = UserBlockKind::BlockquoteExample;
            }
            else if (line.startsWith(">"))
            {
                blk->kind = UserBlockKind::Blockquote;
            }
            else if (line.startsWith("|"))
            {
                blk->kind = UserBlockKind::Table;
            }
            else if (line.startsWith("* "))
            {
                blk->kind = UserBlockKind::List;
            }
            else if (line.startsWith("# "))
            {
                blk->kind = UserBlockKind::Title1;
            }
            else if (line.startsWith("## "))
            {
                blk->kind = UserBlockKind::Title2;
            }
            else if (line.startsWith("### "))
            {
                blk->kind = UserBlockKind::Title3;
            }
            else if (line.startsWith("#### "))
            {
                blk->kind = UserBlockKind::Title4;
            }
            else if (line.startsWith("##### "))
            {
                blk->kind = UserBlockKind::Title5;
            }
            else if (line.startsWith("###### "))
            {
                blk->kind = UserBlockKind::Title6;
            }
            else
            {
                blk->kind = UserBlockKind::Paragraph;
                blk->lines.push_back(lines[start++]);
            }

            break;
        }

        // The short description (first line) can end with '.'.
        if (shortDesc && !blk->lines.empty() && blocks.empty() && !blk->lines[0].empty() && blk->lines[0].back() == '.')
        {
            blocks.push_back(blk);
            continue;
        }

        for (; start < lines.size(); start++)
        {
            bool lastLine = start == lines.size() - 1;
            auto line     = lines[start];
            line.trim();

            bool mustEnd = false;
            switch (blk->kind)
            {
            case UserBlockKind::Title1:
            case UserBlockKind::Title2:
            case UserBlockKind::Title3:
            case UserBlockKind::Title4:
            case UserBlockKind::Title5:
            case UserBlockKind::Title6:
                line.remove(0, 2 + ((int) blk->kind - (int) UserBlockKind::Title1)); // #<blank>
                blk->lines.push_back(line);
                mustEnd = true;
                start++;
                break;

            case UserBlockKind::ParagraphRaw:
                if (line.startsWith("---"))
                {
                    mustEnd = true;
                    start++;
                }
                else
                    blk->lines.push_back(lines[start]);
                break;

            case UserBlockKind::CodeSwag:
            case UserBlockKind::CodeRaw:
                if (line.startsWith("```"))
                {
                    mustEnd = true;
                    start++;
                }
                else
                    blk->lines.push_back(lines[start]);
                break;

            case UserBlockKind::CodeAuto:
                if (lines[start].startsWith("    "))
                {
                    line = lines[start];
                    line.remove(0, 4);
                    blk->lines.push_back(line);
                }
                else if (lines[start].startsWith("\t"))
                {
                    line = lines[start];
                    line.remove(0, 1);
                    blk->lines.push_back(line);
                }
                else
                {
                    mustEnd = true;
                }
                break;

            case UserBlockKind::Paragraph:
                if (line.empty())
                    mustEnd = true;
                else if (line.startsWith("---"))
                    mustEnd = true;
                else if (line.startsWith("```"))
                    mustEnd = true;
                else if (line.startsWith(">"))
                    mustEnd = true;
                else if (line.startsWith("|"))
                    mustEnd = true;
                else if (line.startsWith("* "))
                    mustEnd = true;
                else if (line.startsWith("# "))
                    mustEnd = true;
                else if (line.startsWith("## "))
                    mustEnd = true;
                else if (line.startsWith("### "))
                    mustEnd = true;
                else
                    blk->lines.push_back(lines[start]);
                break;

            case UserBlockKind::Table:
                if (!line.startsWith("|"))
                    mustEnd = true;
                else
                    blk->lines.push_back(lines[start]);
                break;

            case UserBlockKind::List:
                if (!line.startsWith("* "))
                    mustEnd = true;
                else
                {
                    line.remove(0, 2); // *<blank>
                    line.trim();
                    blk->lines.push_back(line);
                }
                break;

            case UserBlockKind::Blockquote:
                if (line.startsWith(">"))
                {
                    auto line1 = line;
                    line1.remove(0, 1);
                    blk->lines.push_back(line1);
                }

                if (!line.startsWith(">") || lastLine)
                {
                    if (!lastLine)
                        mustEnd = true;
                    computeUserBlocks(blk->subBlocks, blk->lines, shortDesc);
                    blk->lines.clear();
                }

                break;

            case UserBlockKind::BlockquoteNote:
            case UserBlockKind::BlockquoteTip:
            case UserBlockKind::BlockquoteWarning:
            case UserBlockKind::BlockquoteAttention:
            case UserBlockKind::BlockquoteExample:
                if (line.startsWith(">"))
                {
                    auto line1 = line;
                    line1.remove(0, 1);
                    blk->lines.push_back(line1);
                }

                if (!line.startsWith(">") || lastLine)
                {
                    if (!lastLine)
                        mustEnd = true;
                    if (blk->kind == UserBlockKind::BlockquoteNote)
                        blk->lines[0].remove(0, (uint32_t) strlen(START_NOTE) - 1);
                    else if (blk->kind == UserBlockKind::BlockquoteTip)
                        blk->lines[0].remove(0, (uint32_t) strlen(START_TIP) - 1);
                    else if (blk->kind == UserBlockKind::BlockquoteWarning)
                        blk->lines[0].remove(0, (uint32_t) strlen(START_WARNING) - 1);
                    else if (blk->kind == UserBlockKind::BlockquoteAttention)
                        blk->lines[0].remove(0, (uint32_t) strlen(START_ATTENTION) - 1);
                    else if (blk->kind == UserBlockKind::BlockquoteExample)
                        blk->lines[0].remove(0, (uint32_t) strlen(START_EXAMPLE) - 1);
                    else
                        SWAG_ASSERT(false);
                    computeUserBlocks(blk->subBlocks, blk->lines, shortDesc);
                    blk->lines.clear();
                }

                break;

            default:
                SWAG_ASSERT(false);
                break;
            }

            if (mustEnd)
                break;
        }

        if (!blk->lines.empty() || !blk->subBlocks.empty())
            blocks.push_back(blk);
        else
            delete blk;
    }
}

void GenDoc::computeUserComments(UserComment& result, const Utf8& txt, bool shortDesc)
{
    if (txt.empty())
        return;

    Vector<Utf8> lines;
    Utf8::tokenize(txt, '\n', lines);
    computeUserComments(result, lines, shortDesc);
}

void GenDoc::computeUserComments(UserComment& result, Vector<Utf8>& lines, bool shortDesc)
{
    computeUserBlocks(result.blocks, lines, shortDesc);

    // First block is the "short description"
    if (shortDesc && !result.blocks.empty() && result.blocks[0]->kind == UserBlockKind::Paragraph)
    {
        result.shortDesc = std::move(*result.blocks[0]);
        result.blocks.erase(result.blocks.begin());
        result.shortDesc.lines[0].trim();
        if (result.shortDesc.lines.back().back() != '.')
            result.shortDesc.lines.back() += '.';
    }
}

Utf8 GenDoc::getFormattedText(const Utf8& user)
{
    if (user.empty())
        return "";

    bool inCodeMode   = false;
    bool inBoldMode   = false;
    bool inItalicMode = false;
    Utf8 result;

    auto pz = user.c_str();
    while (*pz)
    {
        char prevC = pz == user.c_str() ? 0 : pz[-1];

        // Escape char
        if (*pz == '\\')
        {
            if (pz[1] == 0)
            {
                result += "<br/>";
                pz++;
            }
            else
            {
                pz++;
                result += *pz++;
            }

            continue;
        }

        // Replace some characters for HTML
        if (*pz == '<')
        {
            result += "&lt;";
            pz++;
            continue;
        }

        if (*pz == '>')
        {
            result += "&gt;";
            pz++;
            continue;
        }

        // image ![](link)
        if (*pz == '!' && pz[1] == '[' && !inCodeMode)
        {
            Utf8 name;
            Utf8 link;
            auto ppz = tokenizeReference(pz + 1, name, link);
            if (ppz && !link.empty())
            {
                result += Fmt("<img src=\"%s\" alt=\"%s\">", link.c_str(), name.c_str());
                pz = ppz;
                continue;
            }

            result += *pz++;
            continue;
        }

        // [reference] to create an html link to the current document
        if (*pz == '[' && !inCodeMode)
        {
            Utf8 name;
            Utf8 link;
            auto ppz = tokenizeReference(pz, name, link);
            if (ppz)
            {
                if (!link.empty())
                {
                    result += Fmt("<a href=\"%s\">%s</a>", link.c_str(), name.c_str());
                    pz = ppz;
                    continue;
                }
                else if (!name.empty())
                {
                    auto ref = findReference(name);
                    if (!ref.empty())
                    {
                        result += ref;
                        pz = ppz;
                        continue;
                    }
                }
            }

            result += *pz++;
            continue;
        }

        // Italic
        if (prevC != '*' && pz[0] == '*' && pz[1] != '*' && !inCodeMode)
        {
            if ((!inItalicMode && !SWAG_IS_BLANK(pz[1])) || inItalicMode)
            {
                inItalicMode = !inItalicMode;
                if (inItalicMode)
                    result += "<i>";
                else
                    result += "</i>";
                pz += 1;
                continue;
            }
        }

        // Bold
        if (prevC != '*' && pz[0] == '*' && pz[1] == '*' && pz[2] != '*' && !inCodeMode)
        {
            if ((!inBoldMode && !SWAG_IS_BLANK(pz[2])) || inBoldMode)
            {
                inBoldMode = !inBoldMode;
                if (inBoldMode)
                    result += "<b>";
                else
                    result += "</b>";
                pz += 2;
                continue;
            }
        }

        // 'word'
        if (*pz == '\'')
        {
            if (inCodeMode)
            {
                inCodeMode = false;
                result += "</span>";
                pz++;
                continue;
            }

            auto pz1 = pz + 1;
            while (*pz1 && !SWAG_IS_BLANK(*pz1) && *pz1 != '\'')
                pz1++;
            if (*pz1 == '\'')
            {
                inCodeMode = true;
                result += "<span class=\"code-inline\">";
                pz++;
                continue;
            }

            result += *pz++;
            continue;
        }

        // embedded code line
        if (*pz == '`')
        {
            inCodeMode = !inCodeMode;
            if (inCodeMode)
                result += "<span class=\"code-inline\">";
            else
                result += "</span>";
            pz++;
            continue;
        }

        result += *pz++;
    }

    // Be sure it's closed
    if (inBoldMode)
        result += "</b>";
    if (inItalicMode)
        result += "</i>";
    if (inCodeMode)
        result += "</code>";

    return result;
}

void GenDoc::outputUserBlock(const UserBlock& user, int titleLevel, bool shortDescTd)
{
    if (user.lines.empty() && user.subBlocks.empty())
        return;

    switch (user.kind)
    {
    case UserBlockKind::CodeSwag:
    case UserBlockKind::CodeRaw:
    case UserBlockKind::CodeAuto:
    {
        Utf8 block;
        for (auto& l : user.lines)
        {
            block += l;
            block += "\n";
        }

        uint32_t flags = GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL;
        if (user.kind != UserBlockKind::CodeSwag)
            flags &= ~GENDOC_CODE_SYNTAX_COL;

        outputCode(block, flags);
        return;
    }

    case UserBlockKind::ParagraphRaw:
        helpContent += "<p style=\"white-space: break-spaces\">";
        break;

    case UserBlockKind::Blockquote:
        helpContent += "<div class=\"blockquote blockquote-default\">\n";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
        break;

    case UserBlockKind::BlockquoteNote:
    {
        helpContent += "<div class=\"blockquote blockquote-note\">\n";
        helpContent += "<div class=\"blockquote-title-block\">";
        Utf8 quoteIcon{module->buildCfg.genDoc.quoteIconNote};
        helpContent += Fmt("%s ", quoteIcon.empty() ? "<i class=\"fa fa-info-circle\"></i> " : quoteIcon.c_str());
        Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleNote};
        helpContent += Fmt("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Note" : quoteTitle.c_str());
        helpContent += "</div>";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
    }
    break;
    case UserBlockKind::BlockquoteTip:
    {
        helpContent += "<div class=\"blockquote blockquote-tip\">\n";
        helpContent += "<div class=\"blockquote-title-block\">";
        Utf8 quoteIcon{module->buildCfg.genDoc.quoteIconTip};
        helpContent += Fmt("%s ", quoteIcon.empty() ? "<i class=\"fa fa-lightbulb-o\"></i> " : quoteIcon.c_str());
        Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleTip};
        helpContent += Fmt("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Tip" : quoteTitle.c_str());
        helpContent += "</div>";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
    }
    break;
    case UserBlockKind::BlockquoteWarning:
    {
        helpContent += "<div class=\"blockquote blockquote-warning\">\n";
        helpContent += "<div class=\"blockquote-title-block\">";
        Utf8 quoteIcon{module->buildCfg.genDoc.quoteIconWarning};
        helpContent += Fmt("%s ", quoteIcon.empty() ? "<i class=\"fa fa-exclamation-triangle\"></i> " : quoteIcon.c_str());
        Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleWarning};
        helpContent += Fmt("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Warning" : quoteTitle.c_str());
        helpContent += "</div>";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
    }
    break;
    case UserBlockKind::BlockquoteAttention:
    {
        helpContent += "<div class=\"blockquote blockquote-attention\">\n";
        helpContent += "<div class=\"blockquote-title-block\">";
        Utf8 quoteIcon{module->buildCfg.genDoc.quoteIconAttention};
        helpContent += Fmt("%s ", quoteIcon.empty() ? "<i class=\"fa fa-ban\"></i> " : quoteIcon.c_str());
        Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleAttention};
        helpContent += Fmt("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Attention" : quoteTitle.c_str());
        helpContent += "</div>";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
    }
    break;
    case UserBlockKind::BlockquoteExample:
    {
        helpContent += "<div class=\"blockquote blockquote-example\">\n";
        helpContent += "<div class=\"blockquote-title-block\">";
        Utf8 quoteIcon{module->buildCfg.genDoc.quoteIconExample};
        helpContent += Fmt("%s ", quoteIcon.empty() ? "<i class=\"fa fa-magnifying-glass\"></i> " : quoteIcon.c_str());
        Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleExample};
        helpContent += Fmt("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Example" : quoteTitle.c_str());
        helpContent += "</div>";
        for (auto sub : user.subBlocks)
            outputUserBlock(*sub, titleLevel, false);
    }
    break;

    case UserBlockKind::Paragraph:
        if (!shortDescTd)
            helpContent += "<p>";
        break;

    case UserBlockKind::List:
        helpContent += "<ul>\n";
        break;

    case UserBlockKind::Table:
        helpContent += "<table class=\"table-enumeration\">\n";
        break;

    case UserBlockKind::Title1:
    case UserBlockKind::Title2:
    case UserBlockKind::Title3:
    case UserBlockKind::Title4:
    case UserBlockKind::Title5:
    case UserBlockKind::Title6:
    {
        int level = ((int) user.kind - (int) UserBlockKind::Title1);
        helpContent += Fmt("<h%d id=\"%s\">", titleLevel + level + 1, toRef(user.lines[0]).c_str());
        break;
    }
    }

    int32_t tableColCount = UINT32_MAX;
    for (int i = 0; i < user.lines.size(); i++)
    {
        auto line = user.lines[i];
        line.trim();

        if (user.kind == UserBlockKind::Table)
        {
            Vector<Utf8> tkn;
            Utf8::tokenize(line, '|', tkn);

            if (tkn.back().empty())
                tkn.erase(tkn.end());
            while (i && tkn.size() < tableColCount)
                tkn.push_back("");
            while (i && tkn.size() > tableColCount)
                tkn.erase(tkn.end());
            tableColCount = (uint32_t) tkn.size();

            helpContent += "<tr>";
            for (int it = 0; it < tkn.size(); it++)
            {
                auto& t = tkn[it];
                helpContent += "<td>";
                helpContent += getFormattedText(t);
                helpContent += "</td>";
            }
            helpContent += "</tr>\n";
        }
        else if (user.kind == UserBlockKind::List)
        {
            helpContent += "<li>";
            helpContent += getFormattedText(line);
            helpContent += "</li>\n";
        }
        else if (user.kind == UserBlockKind::ParagraphRaw)
        {
            helpContent += user.lines[i];

            // Add one line break after each line, except the last line from a raw block, because we do
            // not want one useless empty line
            if (i != user.lines.size() - 1)
                helpContent += "\n";
        }
        else if (user.kind == UserBlockKind::Paragraph ||
                 user.kind == UserBlockKind::Blockquote ||
                 user.kind == UserBlockKind::BlockquoteNote ||
                 user.kind == UserBlockKind::BlockquoteTip ||
                 user.kind == UserBlockKind::BlockquoteWarning ||
                 user.kind == UserBlockKind::BlockquoteAttention ||
                 user.kind == UserBlockKind::BlockquoteExample)
        {
            if (line.empty())
                helpContent += "</p><p>";
            else
            {
                helpContent += getFormattedText(user.lines[i]);
                helpContent += " ";
            }
        }
        else if (user.kind == UserBlockKind::Title1 ||
                 user.kind == UserBlockKind::Title2 ||
                 user.kind == UserBlockKind::Title3 ||
                 user.kind == UserBlockKind::Title4 ||
                 user.kind == UserBlockKind::Title5 ||
                 user.kind == UserBlockKind::Title6)
        {
            helpContent += getFormattedText(user.lines[i]);
            helpContent += " ";

            // Update toc
            if (docKind == BuildCfgDocKind::Examples)
            {
                int myTitleLevel = (int) user.kind - (int) UserBlockKind::Title1;
                addTocTitle(toRef(user.lines[i]), user.lines[i], titleLevel + myTitleLevel - 1);
            }
        }
        else
        {
            helpContent += getFormattedText(user.lines[i]);
            helpContent += " ";
        }
    }

    switch (user.kind)
    {
    case UserBlockKind::ParagraphRaw:
        helpContent += "</p>\n";
        break;
    case UserBlockKind::Paragraph:
        if (!shortDescTd)
            helpContent += "</p>\n";
        break;
    case UserBlockKind::Blockquote:
    case UserBlockKind::BlockquoteNote:
    case UserBlockKind::BlockquoteTip:
    case UserBlockKind::BlockquoteWarning:
    case UserBlockKind::BlockquoteAttention:
    case UserBlockKind::BlockquoteExample:
        helpContent += "</div>\n";
        break;
    case UserBlockKind::List:
        helpContent += "</ul>\n";
        break;
    case UserBlockKind::Table:
        helpContent += "</table>\n";
        break;
    case UserBlockKind::Title1:
    case UserBlockKind::Title2:
    case UserBlockKind::Title3:
    case UserBlockKind::Title4:
    case UserBlockKind::Title5:
    case UserBlockKind::Title6:
    {
        int level = ((int) user.kind - (int) UserBlockKind::Title1);
        helpContent += Fmt("</h%d>\n", titleLevel + level + 1);
        break;
    }
    }
}

void GenDoc::outputUserComment(const UserComment& user, int titleLevel)
{
    for (auto& b : user.blocks)
        outputUserBlock(*b, titleLevel);
}

void GenDoc::constructPage()
{
    helpOutput.clear();
    helpOutput += "<!DOCTYPE html>\n";
    helpOutput += "<html>\n";

    // Head
    /////////////////////////////////////
    helpOutput += "<head>\n";
    helpOutput += "<meta charset=\"UTF-8\">\n";
    helpOutput += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n";
    helpOutput += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";

    // User start of the <head> section
    Utf8 startHead = Utf8{module->buildCfg.genDoc.startHead};
    helpOutput += startHead;

    // Page title
    if (!titleContent.empty())
        helpOutput += Fmt("<title>%s</title>\n", titleContent.c_str());

    // User icon
    Utf8 icon = Utf8{module->buildCfg.genDoc.icon};
    if (!icon.empty())
        helpOutput += Fmt("<link rel=\"icon\" type=\"image/x-icon\" href=\"%s\">\n", icon.c_str());

    // User css ref
    Utf8 css{module->buildCfg.genDoc.css};
    if (!css.empty())
        helpOutput += Fmt("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n", css.c_str());

    // Font awesome reference
    if (module->buildCfg.genDoc.hasFontAwesome)
        helpOutput += "<script src=\"https://kit.fontawesome.com/f76be2b3ee.js\" crossorigin=\"anonymous\"></script>\n";

    // Predefined <style> section
    if (module->buildCfg.genDoc.hasStyleSection)
        outputStyles();

    // User end of the <head> section
    Utf8 endHead = Utf8{module->buildCfg.genDoc.endHead};
    helpOutput += endHead;

    helpOutput += "</head>\n";

    // Body
    /////////////////////////////////////

    helpOutput += "<body>\n";

    // User start of the <body> section
    Utf8 startBody = Utf8{module->buildCfg.genDoc.startBody};
    helpOutput += startBody;

    helpOutput += "<div class=\"container\">\n";

    if (docKind != BuildCfgDocKind::Pages)
    {
        helpOutput += "<div class=\"left\">\n";
        helpOutput += "<div class=\"left-page\">\n";
        helpOutput += helpToc;
        helpOutput += "</div>\n";
        helpOutput += "</div>\n";
    }

    helpOutput += "<div class=\"right\">\n";
    helpOutput += "<div class=\"right-page\">\n";

    helpOutput += "<div class=\"blockquote blockquote-warning\">\n";
    helpOutput += Fmt("<b>Work in progress</b>. Generated documentation (swag doc %d.%d.%d)", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    helpOutput += "</div>\n";
    helpOutput += helpContent;

    helpOutput += "</div>\n";
    helpOutput += "</div>\n";

    helpOutput += "</div>\n";

    // User end of the <body> section
    Utf8 endBody = Utf8{module->buildCfg.genDoc.endBody};
    helpOutput += endBody;

    helpOutput += "</body>\n";
    helpOutput += "</html>\n";
}

void GenDoc::addTocTitle(const Utf8& name, const Utf8& title, int titleLevel)
{
    while (tocLastTitleLevel < titleLevel)
    {
        helpToc += "<ul>\n";
        tocLastTitleLevel++;
    }

    while (tocLastTitleLevel > titleLevel)
    {
        helpToc += "</ul>\n";
        tocLastTitleLevel--;
    }

    helpToc += Fmt("<li><a href=\"#%s\">%s</a></li>\n", name.c_str(), title.c_str());
}

bool GenDoc::generate(Module* mdl, BuildCfgDocKind kind)
{
    module  = mdl;
    docKind = kind;
    SWAG_ASSERT(module);

    // Setup runtime module documentation
    if (module == g_Workspace->runtimeModule)
    {
        setSlice(module->buildCfg.genDoc.outputName, "swag.runtime");
        setSlice(module->buildCfg.genDoc.outputExtension, ".php");
        setSlice(module->buildCfg.genDoc.titleContent, "Swag Runtime");
        setSlice(module->buildCfg.genDoc.css, "css/style.css");
        setSlice(module->buildCfg.genDoc.icon, "favicon.ico");
        setSlice(module->buildCfg.genDoc.startHead, "<?php include('common/start-head.php'); ?>");
        setSlice(module->buildCfg.genDoc.endHead, "<?php include('common/end-head.php'); ?>");
        setSlice(module->buildCfg.genDoc.startBody, "<?php include('common/start-body.php'); ?>");
        setSlice(module->buildCfg.repoPath, "https://github.com/swag-lang/swag/blob/master/bin/runtime");
    }

    titleToc = Utf8{module->buildCfg.genDoc.titleToc};
    if (titleToc.empty())
        titleToc = "Table of Contents";
    titleContent = Utf8{module->buildCfg.genDoc.titleContent};
    if (titleContent.empty())
        titleContent = Fmt("Module %s", module->name.c_str());

    if (docKind == BuildCfgDocKind::Pages)
    {
        if (!generatePages())
            return false;
        return true;
    }

    // Output filename
    auto filePath = g_Workspace->targetPath;
    Utf8 fileName{module->buildCfg.genDoc.outputName};
    if (fileName.empty())
    {
        filePath.append(g_Workspace->workspacePath.filename().string().c_str());
        filePath += ".";
        filePath += module->name.c_str();
    }
    else
    {
        filePath.append(fileName.c_str());
    }

    Utf8 extName{module->buildCfg.genDoc.outputExtension};
    if (extName.empty())
        extName = ".html";
    filePath += extName.c_str();

    fullFileName = filePath.string();
    fullFileName.makeLower();

    // Write for output
    FILE* f = nullptr;
    if (fopen_s(&f, fullFileName.c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
        return false;
    }

    // Titles
    helpToc += Fmt("<h2>%s</h2>\n", titleToc.c_str());
    helpContent += Fmt("<h1>%s</h1>\n", titleContent.c_str());

    switch (docKind)
    {
    case BuildCfgDocKind::Api:
        if (!generateApi())
            return false;
        break;
    case BuildCfgDocKind::Examples:
        if (!generateExamples())
            return false;
        break;
    }

    constructPage();

    // Write file
    if (fwrite(helpOutput.c_str(), 1, helpOutput.length(), f) != helpOutput.length())
    {
        Report::errorOS(Fmt(Err(Err0525), fullFileName.c_str()));
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
