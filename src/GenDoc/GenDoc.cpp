#include "pch.h"
#include "GenDoc/GenDoc.h"
#include "Main/Version.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

static constexpr auto START_NOTE      = "> NOTE:";
static constexpr auto START_TIP       = "> TIP:";
static constexpr auto START_WARNING   = "> WARNING:";
static constexpr auto START_ATTENTION = "> ATTENTION:";
static constexpr auto START_EXAMPLE   = "> EXAMPLE:";

void GenDoc::outputStyles()
{
    helpOutput += "<style>\n";

    helpOutput += R"(
    .container  { display: flex; flex-wrap: nowrap; flex-direction: row; margin: 0px auto; padding: 0px; }
    .left       { display: block; overflow-y: scroll; width: 500px; }
    .left-page  { margin: 10px; }
    .right      { display: block; width: 100%; }
    .right-page { max-width: 1024px; margin: 10px auto; }
    
    @media(min-width: 640px)  { .container { max-width: 640px; }}
    @media(min-width: 768px)  { .container { max-width: 768px; }}
    @media(min-width: 1024px) { .container { max-width: 1024px; }}
    @media(min-width: 1280px) { .container { max-width: 1280px; }}
    @media(min-width: 1536px) { .container { max-width: 1536px; }}
    
    @media screen and (max-width: 600px) {
        .left       { display: none; }
        .right-page { margin:  10px; }
    }
)";

    // Layout
    if (docKind != BuildCfgDocKind::Pages)
    {
        helpOutput += R"(
            .container { height: 100vh; }
            .right     { overflow-y: scroll; }
        )";
    }

    helpOutput += R"(
    html { font-family: ui-sans-serif, system-ui, "Segoe UI", Roboto, "Helvetica Neue", Arial, "Noto Sans", sans-serif; }
    body { margin: 0px; line-height: 1.3em; }     

    .container a        { color: DoggerBlue; }
    .container a:hover  { text-decoration: underline; }
    .container img      { margin: 0 auto; }
   
    .left a     { text-decoration: none; }
    .left ul    { list-style-type: none; margin-left: -20px; }
    .left h3    { background-color: Black; color: White; padding: 6px; }
    .right h1   { margin-top: 50px; margin-bottom: 50px; }
    .right h2   { margin-top: 35px; }

    .strikethrough-text { text-decoration: line-through; }
    .swag-watermark     { text-align:right; font-size: 80%; margin-top: 30px; }
    .swag-watermark a   { text-decoration: none; color: inherit; }

    .blockquote               { border-radius: 5px; border: 1px solid; margin: 20px; padding: 10px; }
    .blockquote-default       { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
    .blockquote-note          { border-color: #ADCEDD; background-color: #CDEEFD; }
    .blockquote-tip           { border-color: #BCCFBC; background-color: #DCEFDC; }
    .blockquote-warning       { border-color: #DFBDB3; background-color: #FFDDD3; }
    .blockquote-attention     { border-color: #DDBAB8; background-color: #FDDAD8; }
    .blockquote-example       { border: 2px solid LightGrey; }
    .blockquote-title-block   { margin-bottom: 10px; }
    .blockquote-title         { font-weight: bold; }
    .blockquote-default       p:first-child { margin-top: 0px; }
    .blockquote-default       p:last-child  { margin-bottom: 0px; }
    .blockquote               p:last-child  { margin-bottom: 0px; }

    .description-list-title   { font-weight: bold; font-style: italic; }
    .description-list-block   { margin-left: 30px; }

    .container table          { border: 1px solid LightGrey; border-collapse: collapse; font-size: 90%; margin-left: 20px; margin-right: 20px; }
    .container td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
    .container th             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; background-color: #eeeeee; }

    table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
    .api-item td              { font-size: revert; border: 0; }
    .api-item td:first-child  { width: 33%; white-space: nowrap; }
    .api-item-title-src-ref   { text-align:  right; }
    .api-item-title-src-ref a { color:       inherit; }
    .api-item-title-kind      { font-weight: normal; font-size: 80%; }
    .api-item-title-light     { font-weight: normal; }
    .api-item-title-strong    { font-weight: bold; font-size: 100%; }
    .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }

    table.table-enumeration           { width: calc(100% - 40px); }
    .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
    .table-enumeration td:last-child  { width: 100%; }
    .table-enumeration td.code-type   { background-color: #eeeeee; }
    .table-enumeration a              { text-decoration: none; color: inherit; }

    .code-inline  { background-color: #eeeeee; border-radius: 5px; border: 1px dotted #cccccc; padding: 0px 8px; font-size: 110%; font-family: monospace; display: inline-block; }
    .code-block   { background-color: #eeeeee; border-radius: 5px; border: 1px solid LightGrey; padding: 10px; margin: 20px; white-space: pre; overflow-x: auto; }
    .code-block   { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace; }
    .code-block a { color: inherit; }
)";

    const float    lum        = module->buildCfg.genDoc.syntaxColorLum;
    const uint32_t defaultCol = module->buildCfg.genDoc.syntaxDefaultColor;
    helpOutput += form("    .%s { color: #%x; }\n", SYN_CODE, defaultCol);
    helpOutput += form("    .%s { color: #%x; }\n", SYN_COMMENT, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxComment, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_COMPILER, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxCompiler, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_FUNCTION, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxFunction, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_CONSTANT, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxConstant, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_INTRINSIC, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxIntrinsic, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_TYPE, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxType, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_KEYWORD, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxKeyword, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_LOGIC, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxLogic, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_NUMBER, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxNumber, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_STRING, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxString, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_ATTRIBUTE, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxAttribute, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_REGISTER, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxRegister, lum));
    helpOutput += form("    .%s { color: #%x; }\n", SYN_INVALID, getSyntaxColorRgb(SyntaxColorMode::ForDoc, SyntaxColor::SyntaxInvalid, lum));

    helpOutput += "</style>\n";
}

Utf8 GenDoc::toRef(Utf8 str)
{
    str.replace(".", "_");
    str.replace(" ", "_");
    if (SWAG_IS_DIGIT(str[0]))
        return "_" + str;
    return str;
}

const char* GenDoc::tokenizeReference(const char* pz, Utf8& name, Utf8& link, bool acceptLink)
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

    if (!acceptLink)
        return pz + 1;

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
    const auto it = collectInvert.find(name);
    if (it != collectInvert.end())
        return form("<a href=\"#%s\">%s</a>", toRef(it->second).c_str(), name.c_str());

    Vector<Utf8> tkns;
    Utf8::tokenize(name, '.', tkns);
    if (tkns.size() <= 1)
        return "";

    if (tkns[0] == "Swag")
    {
        return form("<a href=\"swag.runtime.php#%s\">%s</a>", toRef(name).c_str(), name.c_str());
    }

    return "";
}

void GenDoc::outputCode(const Utf8& code, GenDocFlags flags)
{
    if (code.empty())
        return;

    // Remove eol at the start and at the end
    auto repl = code;
    repl.trimLeftEol();
    repl.trimRightEol();
    if (repl.empty())
        return;

    if (flags.has(GENDOC_CODE_BLOCK))
    {
        helpContent += "<div class=\"code-block\">";
    }

    // Kind of a hack for now... Try to keep references, but try to keep <> also...
    if (code.find("<a href") == -1)
    {
        repl.replace("<", "&lt;");
        repl.replace(">", "&gt;");
    }

    // Syntax coloration
    Utf8 codeText;
    if (flags.has(GENDOC_CODE_SYNTAX_COL))
    {
        SyntaxColorContext cxt;
        cxt.mode = SyntaxColorMode::ForDoc;
        codeText = doSyntaxColor(repl, cxt, true);
    }
    else
    {
        codeText = form("<span class=\"%s\">", SYN_CODE);
        codeText += repl;
        codeText += "</span>";
    }

    // References
    if (!flags.has(GENDOC_CODE_REFS))
        repl = std::move(codeText);
    else
    {
        repl.clear();
        auto pz = codeText.c_str();
        while (*pz)
        {
            if (SWAG_IS_ALPHA(*pz) || *pz == '_')
            {
                Utf8 nameToRef;
                while (SWAG_IS_AL_NUM(*pz) || *pz == '_' || *pz == '.')
                    nameToRef += *pz++;

                auto ref = findReference(nameToRef);
                if (ref.empty() && nameToRef.compareNoCase(module->name) && !strncmp(pz, "</span>.", 8))
                {
                    repl += "</span>";
                    pz += 8;
                }
                else if (ref.empty())
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

    if (flags.has(GENDOC_CODE_BLOCK))
        helpContent += "</div>\n";
}

void GenDoc::computeUserBlocks(Vector<UserBlock*>& blocks, const Vector<Utf8>& lines, bool shortDesc)
{
    uint32_t start = 0;
    while (start < lines.size())
    {
        auto blk = new UserBlock;

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
            else if (line.length() > 1 && SWAG_IS_DIGIT(line[0]) && line[1] == '.')
            {
                blk->kind = UserBlockKind::OrderedList;
            }
            else if (line.startsWith("+ "))
            {
                blk->kind = UserBlockKind::DescriptionList;
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
            const bool lastLine = start == lines.size() - 1;
            auto       line     = lines[start];
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
                    line.remove(0, 2 + (static_cast<uint32_t>(blk->kind) - static_cast<uint32_t>(UserBlockKind::Title1))); // #<blank>
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

                case UserBlockKind::OrderedList:
                    if (line.length() < 2 || !SWAG_IS_DIGIT(line[0]) || line[1] != '.')
                        mustEnd = true;
                    else
                    {
                        line.remove(0, 2); // <digit><dot>
                        line.trim();
                        blk->lines.push_back(line);
                    }
                    break;

                case UserBlockKind::DescriptionList:
                {
                    line.remove(0, 2); // +<blank>
                    line.trim();
                    auto title = line;
                    start++;

                    for (; start < lines.size(); start++)
                    {
                        auto line1 = lines[start];
                        if (line1.startsWith("    "))
                        {
                            line1.remove(0, 4);
                            blk->lines.push_back(line1);
                            continue;
                        }

                        if (line1.startsWith("\t"))
                        {
                            line1.remove(0, 1);
                            blk->lines.push_back(line1);
                            continue;
                        }

                        line1.trim();
                        if (line1.empty())
                        {
                            blk->lines.push_back(lines[start]);
                            continue;
                        }

                        break;
                    }

                    computeUserBlocks(blk->subBlocks, blk->lines, shortDesc);
                    blk->lines.clear();
                    blk->lines.push_back(title);
                    mustEnd = true;
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
                            blk->lines[0].remove(0, static_cast<uint32_t>(strlen(START_NOTE)) - 1);
                        else if (blk->kind == UserBlockKind::BlockquoteTip)
                            blk->lines[0].remove(0, static_cast<uint32_t>(strlen(START_TIP)) - 1);
                        else if (blk->kind == UserBlockKind::BlockquoteWarning)
                            blk->lines[0].remove(0, static_cast<uint32_t>(strlen(START_WARNING)) - 1);
                        else if (blk->kind == UserBlockKind::BlockquoteAttention)
                            blk->lines[0].remove(0, static_cast<uint32_t>(strlen(START_ATTENTION)) - 1);
                        else if (blk->kind == UserBlockKind::BlockquoteExample)
                            blk->lines[0].remove(0, static_cast<uint32_t>(strlen(START_EXAMPLE)) - 1);
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
    Utf8::tokenize(txt, '\n', lines, true, true);
    computeUserComments(result, lines, shortDesc);
}

void GenDoc::computeUserComments(UserComment& result, const Vector<Utf8>& lines, bool shortDesc)
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

    int  inCodeMode       = 0;
    bool inBoldMode       = false;
    bool inItalicMode     = false;
    bool inBoldItalicMode = false;
    bool inStrikeMode     = false;

    Utf8 result;

    auto pz = user.c_str();
    while (*pz)
    {
        char prevC = pz == user.c_str() ? '\0' : pz[-1];

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
                result += form(R"(<img src="%s" alt="%s">)", link.c_str(), name.c_str());
                pz = ppz;
                continue;
            }

            result += *pz++;
            continue;
        }

        // [[reference]] to create an html link to the current document
        if (*pz == '[' && pz[1] == '[' && !inCodeMode)
        {
            Utf8 name, link;
            auto ppz = tokenizeReference(pz + 1, name, link, false);
            if (ppz && ppz[0] == ']')
            {
                auto ref = findReference(name);
                if (!ref.empty())
                {
                    result += ref;
                    pz = ppz + 1;
                    continue;
                }
            }

            result += *pz++;
            result += *pz++;
            continue;
        }

        // [reference] to create an html link to the current document
        if (*pz == '[' && pz[1] != '[' && !inCodeMode)
        {
            Utf8 name, link;
            auto ppz = tokenizeReference(pz, name, link);
            if (ppz && !link.empty())
            {
                result += form("<a href=\"%s\">%s</a>", link.c_str(), name.c_str());
                pz = ppz;
                continue;
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

        // Bold+Italic
        if (prevC != '*' && pz[0] == '*' && pz[1] == '*' && pz[2] == '*' && pz[3] != '*' && !inCodeMode)
        {
            if ((!inBoldItalicMode && !SWAG_IS_BLANK(pz[3])) || inBoldItalicMode)
            {
                inBoldItalicMode = !inBoldItalicMode;
                if (inBoldItalicMode)
                    result += "<b><i>";
                else
                    result += "</i></b>";
                pz += 3;
                continue;
            }
        }

        // Strike through
        if (prevC != '~' && pz[0] == '~' && pz[1] == '~' && pz[2] != '~' && !inCodeMode)
        {
            if ((!inStrikeMode && !SWAG_IS_BLANK(pz[2])) || inStrikeMode)
            {
                inStrikeMode = !inStrikeMode;
                if (inStrikeMode)
                    result += "<span class=\"strikethrough-text\">";
                else
                    result += "</span>";
                pz += 2;
                continue;
            }
        }

        // 'word'
        if (*pz == '\'' && inCodeMode != 2)
        {
            if (inCodeMode)
            {
                inCodeMode = 0;
                result += "</span>";
                pz++;
                continue;
            }

            auto pz1 = pz + 1;
            while (*pz1 && !SWAG_IS_BLANK(*pz1) && *pz1 != '\'')
                pz1++;
            if (*pz1 == '\'')
            {
                inCodeMode = 1;
                result += "<span class=\"code-inline\">";
                pz++;
                continue;
            }

            result += *pz++;
            continue;
        }

        // embedded code line
        if (*pz == '`' && inCodeMode != 1)
        {
            if (inCodeMode)
                inCodeMode = 0;
            else
                inCodeMode = 2;
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
    if (inStrikeMode)
        result += "</span>";
    if (inBoldItalicMode)
        result += "</i></b>";
    if (inCodeMode)
        result += "</code>";

    return result;
}

void GenDoc::outputUserBlock(const UserBlock& user, int titleLevel, bool shortDescTd)
{
    if (user.lines.empty() && user.subBlocks.empty())
        return;

    uint32_t               startLine     = 0;
    uint32_t               tableColCount = 0;
    VectorNative<uint32_t> tableAlignCols;

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

            GenDocFlags flags = GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL;
            if (user.kind != UserBlockKind::CodeSwag)
                flags.remove(GENDOC_CODE_SYNTAX_COL);

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
            helpContent += form("%s ", quoteIcon.empty() ? "<i class=\"fa fa-info-circle\"></i> " : quoteIcon.c_str());
            Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleNote};
            helpContent += form("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Note" : quoteTitle.c_str());
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
            helpContent += form("%s ", quoteIcon.empty() ? "<i class=\"fa fa-lightbulb-o\"></i> " : quoteIcon.c_str());
            Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleTip};
            helpContent += form("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Tip" : quoteTitle.c_str());
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
            helpContent += form("%s ", quoteIcon.empty() ? "<i class=\"fa fa-exclamation-triangle\"></i> " : quoteIcon.c_str());
            Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleWarning};
            helpContent += form("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Warning" : quoteTitle.c_str());
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
            helpContent += form("%s ", quoteIcon.empty() ? "<i class=\"fa fa-ban\"></i> " : quoteIcon.c_str());
            Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleAttention};
            helpContent += form("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Attention" : quoteTitle.c_str());
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
            helpContent += form("%s ", quoteIcon.empty() ? "<i class=\"fa fa-magnifying-glass\"></i> " : quoteIcon.c_str());
            Utf8 quoteTitle{module->buildCfg.genDoc.quoteTitleExample};
            helpContent += form("<span class=\"blockquote-title\">%s</span>", quoteTitle.empty() ? "Example" : quoteTitle.c_str());
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
        case UserBlockKind::OrderedList:
            helpContent += "<ol>\n";
            break;

        case UserBlockKind::DescriptionList:
            helpContent += "<div class=\"description-list-title\"><p>";
            helpContent += getFormattedText(user.lines[0]);
            helpContent += "</p></div>\n";
            helpContent += "<div class=\"description-list-block\">\n";
            for (auto sub : user.subBlocks)
                outputUserBlock(*sub, titleLevel, false);
            helpContent += "</div>\n";
            startLine = 1;
            break;

        case UserBlockKind::Table:
        {
            helpContent += "<table class=\"table-markdown\">\n";

            Vector<Utf8> tkn0;
            Utf8::tokenize(user.lines[0], '|', tkn0, true, true);
            tkn0.erase(tkn0.begin());
            if (tkn0.back().empty())
                tkn0.erase(tkn0.end());
            tableColCount = tkn0.size();

            // Header ?
            if (user.lines.size() > 2)
            {
                Vector<Utf8> tkn1;
                Utf8::tokenize(user.lines[1], '|', tkn1, true, true);
                tkn1.erase(tkn1.begin());
                if (tkn1.back().empty())
                    tkn1.erase(tkn1.end());

                bool hasHeader = true;
                for (uint32_t it = 0; it < tkn0.size(); it++)
                {
                    if (it >= tkn1.size())
                    {
                        hasHeader = false;
                        break;
                    }

                    auto alignStr = tkn1[it];
                    if (alignStr.length() < 3)
                    {
                        hasHeader = false;
                        break;
                    }

                    int alignCol = -1;
                    if (alignStr[0] == ':' && alignStr.back() == '-')
                        alignCol = 0;
                    else if (alignStr[0] == '-' && alignStr.back() == '-')
                        alignCol = 0;
                    else if (alignStr[0] == ':' && alignStr.back() == ':')
                        alignCol = 1;
                    else if (alignStr[0] == '-' && alignStr.back() == ':')
                        alignCol = 2;
                    if (alignCol == -1)
                    {
                        hasHeader = false;
                        break;
                    }

                    while (alignStr.length() && (alignStr[0] == '-' || alignStr[0] == ':'))
                        alignStr.remove(0, 1);
                    if (alignStr.length())
                    {
                        hasHeader = false;
                        break;
                    }

                    tableAlignCols.push_back(alignCol);
                }

                if (!hasHeader)
                    tableAlignCols.clear();
                else
                {
                    helpContent += "<tr>";
                    for (uint32_t it = 0; it < tkn0.size(); it++)
                    {
                        switch (tableAlignCols[it])
                        {
                            case 0:
                                helpContent += form("<th style=\"text-align: left;\">");
                                break;
                            case 1:
                                helpContent += form("<th style=\"text-align: center;\">");
                                break;
                            case 2:
                                helpContent += form("<th style=\"text-align: right;\">");
                                break;
                            default:
                                break;
                        }

                        helpContent += getFormattedText(tkn0[it]);
                        helpContent += "</th>";
                    }

                    helpContent += "</tr>";
                    startLine = 2;
                }
            }
        }
        break;

        case UserBlockKind::Title1:
        case UserBlockKind::Title2:
        case UserBlockKind::Title3:
        case UserBlockKind::Title4:
        case UserBlockKind::Title5:
        case UserBlockKind::Title6:
        {
            // Update toc
            if (docKind == BuildCfgDocKind::Examples)
            {
                uint32_t myTitleLevel = static_cast<uint32_t>(user.kind) - static_cast<uint32_t>(UserBlockKind::Title1);
                addTocTitle(user.lines[0], user.lines[0], titleLevel + myTitleLevel);
            }

            uint32_t level = static_cast<uint32_t>(user.kind) - static_cast<uint32_t>(UserBlockKind::Title1);
            auto     ref   = getTocTitleRef();
            helpContent += form("<h%d id=\"%s\">", titleLevel + level + 1, ref.c_str());
            break;
        }
    }

    for (uint32_t i = startLine; i < user.lines.size(); i++)
    {
        auto line = user.lines[i];
        line.trim();

        if (user.kind == UserBlockKind::Table)
        {
            Vector<Utf8> tkn;
            Utf8::tokenize(line, '|', tkn);

            if (tkn.back().empty())
                tkn.pop_back();
            while (tkn.size() < tableColCount)
                tkn.push_back("");
            while (tkn.size() > tableColCount)
                tkn.pop_back();

            helpContent += "<tr>";
            for (uint32_t it = 0; it < tkn.size(); it++)
            {
                auto& t = tkn[it];

                if (it < tableAlignCols.size())
                {
                    switch (tableAlignCols[it])
                    {
                        case 0:
                            helpContent += form("<td style=\"text-align: left;\">");
                            break;
                        case 1:
                            helpContent += form("<td style=\"text-align: center;\">");
                            break;
                        case 2:
                            helpContent += form("<td style=\"text-align: right;\">");
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    helpContent += "<td>";
                }

                helpContent += getFormattedText(t);
                helpContent += "</td>";
            }
            helpContent += "</tr>\n";
        }
        else if (user.kind == UserBlockKind::List || user.kind == UserBlockKind::OrderedList)
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
        else if (user.kind == UserBlockKind::Paragraph)
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
        case UserBlockKind::OrderedList:
            helpContent += "</ol>\n";
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
            uint32_t level = static_cast<uint32_t>(user.kind) - static_cast<uint32_t>(UserBlockKind::Title1);
            helpContent += form("</h%d>\n", titleLevel + level + 1);
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
    const auto startHead = Utf8{module->buildCfg.genDoc.startHead};
    helpOutput += startHead;

    // Page title
    if (!titleContent.empty())
        helpOutput += form("<title>%s</title>\n", titleContent.c_str());

    // User icon
    const auto icon = Utf8{module->buildCfg.genDoc.icon};
    if (!icon.empty())
        helpOutput += form("<link rel=\"icon\" type=\"image/x-icon\" href=\"%s\">\n", icon.c_str());

    // User css ref
    const Utf8 css{module->buildCfg.genDoc.css};
    if (!css.empty())
        helpOutput += form("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n", css.c_str());

    // Font awesome reference
    if (module->buildCfg.genDoc.hasFontAwesome)
        helpOutput += "<script src=\"https://kit.fontawesome.com/f76be2b3ee.js\" crossorigin=\"anonymous\"></script>\n";

    // Predefined <style> section
    if (module->buildCfg.genDoc.hasStyleSection)
        outputStyles();

    // User end of the <head> section
    const auto endHead = Utf8{module->buildCfg.genDoc.endHead};
    helpOutput += endHead;

    helpOutput += "\n</head>\n";

    // Body
    /////////////////////////////////////

    helpOutput += "<body>\n";

    // User start of the <body> section
    const auto startBody = Utf8{module->buildCfg.genDoc.startBody};
    helpOutput += startBody;

    helpOutput += "\n<div class=\"container\">\n";

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
    helpOutput += helpContent;

    // Watermark
    if (module->buildCfg.genDoc.hasSwagWatermark)
    {
        helpOutput += "<div class=\"swag-watermark\">\n";
        const time_t t = time(nullptr);
        tm           nt;
        (void) localtime_s(&nt, &t);
        std::ostringstream oss;
        oss << std::put_time(&nt, "%d-%m-%Y");
        const std::string dateTime = oss.str();
        helpOutput += form("Generated on %s with <a href=\"https://swag-lang.org/index.php\">swag</a> %d.%d.%d", dateTime.c_str(), SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
        helpOutput += "</div>\n";
    }

    helpOutput += "</div>\n";
    helpOutput += "</div>\n";

    helpOutput += "</div>\n";

    // A script to restore the scroll on load/refresh
    helpOutput += R"(
    <script> 
		function getOffsetTop(element, parent) {
			let offsetTop = 0;
			while (element && element != parent) {
				offsetTop += element.offsetTop;
				element = element.offsetParent;
			}
			return offsetTop;
		}	
		document.addEventListener("DOMContentLoaded", function() {
			let hash = window.location.hash;
			if (hash)
			{
				let parentScrollable = document.querySelector('.right');
				if (parentScrollable)
				{
					let targetElement = parentScrollable.querySelector(hash);
					if (targetElement)
					{
						parentScrollable.scrollTop = getOffsetTop(targetElement, parentScrollable);
					}
				}
			}
        });
    </script>)";
    helpOutput += "\n";

    // User end of the <body> section
    const auto endBody = Utf8{module->buildCfg.genDoc.endBody};
    helpOutput += endBody;

    helpOutput += "</body>\n";
    helpOutput += "</html>\n";
}

Utf8 GenDoc::getTocTitleRef() const
{
    Utf8 ref;
    for (const auto& o : titleRefStack)
    {
        ref += o;
        ref += "_";
    }

    if (!ref.empty())
        ref.removeBack();

    return ref;
}

void GenDoc::addTocTitle(const Utf8& name, const Utf8& title, uint32_t titleLevel)
{
    if (tocLastTitleLevel < titleLevel)
    {
        while (tocLastTitleLevel < titleLevel)
        {
            helpToc += "<ul>\n";
            tocLastTitleLevel++;
            if (tocLastTitleLevel != titleLevel)
                titleRefStack.push_back("");
        }
    }
    else if (tocLastTitleLevel > titleLevel)
    {
        while (tocLastTitleLevel > titleLevel)
        {
            helpToc += "</ul>\n";
            tocLastTitleLevel--;
            titleRefStack.pop_back();
        }

        if (!titleRefStack.empty())
            titleRefStack.pop_back();
    }
    else if (tocLastTitleLevel == titleLevel)
    {
        if (!titleRefStack.empty())
            titleRefStack.pop_back();
    }

    titleRefStack.push_back(toRef(name));
    const auto ref = getTocTitleRef();
    helpToc += form("<li><a href=\"#%s\">%s</a></li>\n", ref.c_str(), title.c_str());
}

Utf8 GenDoc::getFileExtension(const Module* module)
{
    Utf8 extName{module->buildCfg.genDoc.outputExtension};
    if (!g_CommandLine.docExtension.empty())
        extName = g_CommandLine.docExtension;
    if (extName.empty())
        extName = ".html";
    return extName;
}

bool GenDoc::constructAndSave()
{
    constructPage();

    // Write for output
    if (g_CommandLine.outputDoc)
    {
        FILE* f = nullptr;
        if (fopen_s(&f, fullFileName, "wb"))
        {
            Report::errorOS(formErr(Err0085, fullFileName.c_str()));
            return false;
        }

        if (fwrite(helpOutput, 1, helpOutput.length(), f) != helpOutput.length())
        {
            Report::errorOS(formErr(Err0088, fullFileName.c_str()));
            (void) fclose(f);
            return false;
        }

        (void) fclose(f);
    }

    return true;
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
        titleContent = form("Module %s", module->name.c_str());

    if (docKind == BuildCfgDocKind::Pages)
    {
        if (!generatePages())
            return false;
        return true;
    }

    // Output filename
    auto       filePath = g_Workspace->targetPath;
    const Utf8 fileName{module->buildCfg.genDoc.outputName};
    if (fileName.empty())
    {
        filePath.append(g_Workspace->workspacePath.filename());
        filePath += ".";
        filePath += module->name;
    }
    else
    {
        filePath.append(fileName);
    }

    const auto extName = getFileExtension(module);
    filePath += extName;

    fullFileName = filePath;
    fullFileName.makeLower();

    // Titles
    helpToc += form("<h2>%s</h2>\n", titleToc.c_str());
    helpContent += form("<h1>%s</h1>\n", titleContent.c_str());

    switch (docKind)
    {
        case BuildCfgDocKind::Api:
            SWAG_CHECK(generateApi());
            break;
        case BuildCfgDocKind::Examples:
            SWAG_CHECK(generateExamples());
            break;
    }

    SWAG_CHECK(constructAndSave());
    return true;
}
