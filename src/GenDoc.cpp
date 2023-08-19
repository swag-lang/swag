#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Workspace.h"
#include "Version.h"
#include "SyntaxColor.h"

void GenDoc::outputStyles()
{
    helpOutput += "<style>\n";
    helpOutput +=
        ".container {\n\
            display:        flex;\n\
            flex-wrap:      nowrap;\n\
            flex-direction: row;\n\
            height:         100%;\n\
            line-height:    1.3em;\n\
            font-family:    Segoe UI;\n\
        }\n\
        .left {\n\
            display:    block;\n\
            overflow-y: scroll;\n\
            width:      650;\n\
            height:     100%;\n\
        }\n\
        .right {\n\
            display:     block;\n\
            overflow-y:  scroll;\n\
            width:       100%;\n\
            line-height: 1.3em;\n\
            height:      100%;\n\
        }\n\
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n\
        blockquote {\n\
            background-color:   LightYellow;\n\
            border-left:        6px solid Orange;\n\
            padding:            10px;\n\
            width:              90%;\n\
        }\n\
        a {\n\
            text-decoration: none;\n\
            color:           DoggerBlue;\n\
        }\n\
        a:hover {\n\
            text-decoration: underline;\n\
        }\n\
        a.src {\n\
            font-size:          90%;\n\
            color:              LightGrey;\n\
        }\n\
        table.enumeration {\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              100%;\n\
            font-size:          90%;\n\
        }\n\
        td.enumeration {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              30%;\n\
        }\n\
        td.tdname {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
            background-color:   #f8f8f8;\n\
        }\n\
        td.tdtype {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              auto;\n\
        }\n\
        td:last-child {\n\
            width:              auto;\n\
        }\n\
        .left ul {\n\
            list-style-type:    none;\n\
            margin-left:        -20px;\n\
        }\n\
        .titletype {\n\
            font-weight:        normal;\n\
            font-size:          80%;\n\
        }\n\
        .titlelight {\n\
            font-weight:        normal;\n\
        }\n\
        .titlestrong {\n\
            font-weight:        bold;\n\
            font-size:          100%;\n\
        }\n\
        .left h2 {\n\
            background-color:   Black;\n\
            color:              White;\n\
            padding:            6px;\n\
        }\n\
        .right h1 {\n\
            margin-top:         50px;\n\
            margin-bottom:      50px;\n\
        }\n\
        .right h2 {\n\
            margin-top:         35px;\n\
        }\n\
        table.item {\n\
            background-color:   Black;\n\
            color:              White;\n\
            width:              100%;\n\
            margin-top:         70px;\n\
            margin-right:       0px;\n\
            padding:            4px;\n\
            font-size:          110%;\n\
        }\n\
        .srcref {\n\
            text-align:         right;\n\
        }\n\
        .incode {\n\
            background-color:   #eeeeee;\n\
            padding:            2px;\n\
            border: 1px dotted  #cccccc;\n\
        }\n\
        .tdname .incode {\n\
            background-color:   revert;\n\
            padding:            2px;\n\
            border:             revert;\n\
        }\n\
        .addinfos {\n\
            font-size:          90%;\n\
            white-space:        break-spaces;\n\
            overflow-wrap:      break-word;\n\
        }\n\
        .code {\n\
            background-color:   #eeeeee;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              94%;\n\
            margin-left:        20px;\n\
        }\n\
    ";

    float lum = module ? module->buildCfg.docSyntaxColorLum : 0.5f;
    helpOutput += Fmt(".SyntaxCode { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxCode, lum));
    helpOutput += Fmt(".SyntaxComment { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxComment, lum));
    helpOutput += Fmt(".SyntaxCompiler { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxCompiler, lum));
    helpOutput += Fmt(".SyntaxFunction { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxFunction, lum));
    helpOutput += Fmt(".SyntaxConstant { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxConstant, lum));
    helpOutput += Fmt(".SyntaxIntrinsic { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxIntrinsic, lum));
    helpOutput += Fmt(".SyntaxType { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxType, lum));
    helpOutput += Fmt(".SyntaxKeyword { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxKeyword, lum));
    helpOutput += Fmt(".SyntaxLogic { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxLogic, lum));
    helpOutput += Fmt(".SyntaxNumber { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxNumber, lum));
    helpOutput += Fmt(".SyntaxString { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxString, lum));
    helpOutput += Fmt(".SyntaxAttribute { color: #%x; }\n", getSyntaxColor(SyntaxColor::SyntaxAttribute, lum));

    helpOutput += "</style>\n";
}

Utf8 GenDoc::toRef(Utf8 str)
{
    str.replace(".", "_");
    return str;
}

void GenDoc::outputCode(const Utf8& code, bool makeRefs)
{
    if (code.empty())
        return;
    helpContent += "<p class=\"code\">\n";
    helpContent += "<code style=\"white-space: break-spaces\">";

    auto repl = code;

    // Remove trailing eol
    repl.trim();
    while (repl.back() == '\n')
        repl.removeBack();

    // Kind of a hack for now... Try to keep references, but try to keep <> also...
    if (code.find("<a href") == -1)
    {
        repl.replace("<", "&lt;");
        repl.replace(">", "&gt;");
    }

    // Syntax coloration
    auto codeText = syntaxColor(repl, SyntaxColorMode::ForDoc);

    // References
    if (!makeRefs)
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
                repl += getReference(nameToRef);
            }
            else
            {
                repl += *pz++;
            }
        }
    }

    helpContent += repl;
    helpContent += "</code>\n";
    helpContent += "</p>\n";
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
        return Fmt("<a href=\"swag.runtime.html#%s\">%s</a>", toRef(name).c_str(), name.c_str());
    }

    return "";
}

Utf8 GenDoc::getReference(const Utf8& name)
{
    auto ref = findReference(name);
    if (ref.empty())
        return name;
    return ref;
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

        // [reference] to create an html link to the current document
        if (*pz == '[' && (SWAG_IS_ALPHA(pz[1]) || pz[1] == '#' || pz[1] == '@'))
        {
            pz++;

            Utf8 name;
            name += *pz++;
            while (*pz && *pz != ']')
                name += *pz++;

            if (*pz != ']')
            {
                result += "[";
                result += name;
                continue;
            }

            pz++;

            // Markdown reference
            if (*pz == '(')
            {
                Utf8 ref;
                pz++;
                while (*pz && *pz != ')')
                    ref += *pz++;
                result += Fmt("<a href=\"%s\">%s</a>", ref.c_str(), name.c_str());
                if (*pz) pz++;
                continue;
            }

            // Doc reference
            auto ref = findReference(name);
            if (!ref.empty())
            {
                result += ref;
                continue;
            }

            result += "[";
            result += name;
            result += "]";
            continue;
        }

        // Italic
        if (prevC != '*' && pz[0] == '*' && pz[1] != '*')
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
        if (prevC != '*' && pz[0] == '*' && pz[1] == '*' && pz[2] != '*')
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
            auto pz1 = pz + 1;
            while (*pz1 && !SWAG_IS_BLANK(*pz1) && *pz1 != '\'')
                pz1++;
            if (*pz1 == '\'')
            {
                result += "<code class=\"incode\">";
                pz++;
                while (pz != pz1)
                    result += *pz++;
                result += "</code>";
            }
            pz++;
            continue;
        }

        // embedded code line
        if (*pz == '`')
        {
            inCodeMode = !inCodeMode;
            if (inCodeMode)
                result += "<code class=\"incode\">";
            else
                result += "</code>";
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

void GenDoc::startPage()
{
    helpOutput.clear();
    helpContent.clear();
    helpToc.clear();

    helpOutput += "<html>\n";
    helpOutput += "<body>\n";

    helpOutput += "<head>\n";
    helpOutput += "<meta charset=\"UTF-8\">\n";

    // Css
    bool userCss = false;
    if (module && module->buildCfg.docCss.buffer && module->buildCfg.docCss.count)
    {
        Utf8 css{module->buildCfg.docCss};
        helpOutput += Fmt("<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s\">\n", css.c_str());
        userCss = true;
    }

    helpOutput += "</head>\n";

    // Embbeded styles
    if (!userCss)
        outputStyles();

    helpContent += "<blockquote>\n";
    helpContent += Fmt("<b>Work in progress</b>. Generated documentation (swag doc %d.%d.%d)", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    helpContent += "</blockquote>\n";
}

void GenDoc::endPage()
{
    helpOutput += "<div class=\"container\">\n";

    if (docKind != DocKind::Pages)
    {
        helpOutput += "<div class=\"left\">\n";
        helpOutput += helpToc;
        helpOutput += "</div>\n";
    }

    helpOutput += "<div class=\"right\">\n";
    helpOutput += "<div class=\"page\">\n";
    helpOutput += helpContent;
    helpOutput += "</div>\n";
    helpOutput += "</div>\n";

    helpOutput += "</div>\n";

    helpOutput += "</body>\n";
    helpOutput += "</html>\n";
}

bool GenDoc::generate(Module* mdl, DocKind kind)
{
    module  = mdl;
    docKind = kind;

    if (docKind == DocKind::Pages)
    {
        if (!generatePages())
            return false;
        return true;
    }

    // Output filename
    auto filePath = g_Workspace->targetPath;
    if (!module)
    {
        filePath.append("swag.runtime.html");
    }
    else
    {
        Utf8 fileName{module->buildCfg.docOutputName};
        if (fileName.empty())
        {
            filePath.append(g_Workspace->workspacePath.filename().string().c_str());
            filePath += ".";
            filePath += module->name.c_str();
            filePath += ".html";
        }
        else
        {
            filePath.append(fileName.c_str());
        }
    }

    fullFileName = filePath.string();
    fullFileName.makeLower();

    // Write for output
    FILE* f = nullptr;
    if (fopen_s(&f, fullFileName.c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
        return false;
    }

    startPage();

    // Titles
    if (!module)
    {
        helpToc += "<h1>Swag Runtime</h1>\n";
    }
    else
    {
        Utf8 titleToc{module->buildCfg.docTitleToc};
        if (titleToc.empty())
            titleToc = Fmt("Module %s", module->name.c_str());
        helpToc += Fmt("<h1>%s</h1>\n", titleToc.c_str());

        Utf8 titleContent{module->buildCfg.docTitleContent};
        if (titleContent.empty())
            titleContent = "Overview";
        helpContent += Fmt("<h1>%s</h1>\n", titleContent.c_str());
    }

    switch (docKind)
    {
    case DocKind::Api:
        if (!generateApi())
            return false;
        break;
    case DocKind::Examples:
        if (!generateExamples())
            return false;
        break;
    }

    endPage();

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
