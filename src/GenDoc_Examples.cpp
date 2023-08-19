#include "pch.h"
#include "GenDoc.h"
#include "Module.h"
#include "Report.h"
#include "ErrorIds.h"
#include "Vector.h"
#pragma optimize("", off)

void GenDoc::stateEnter(UserBlockKind st)
{
    switch (st)
    {
    case UserBlockKind::Paragraph:
        helpContent += "<p>\n";
        break;
    case UserBlockKind::List:
        helpContent += "<ul>\n";
        break;
    case UserBlockKind::Blockquote:
        helpContent += "<blockquote><p>\n";
        break;
    case UserBlockKind::Table:
        helpContent += "<table>\n";
        break;
    }
}

void GenDoc::stateLeave(UserBlockKind st)
{
    switch (st)
    {
    case UserBlockKind::Paragraph:
        helpContent += "</p>\n";
        break;
    case UserBlockKind::List:
        helpContent += "</ul>\n";
        break;
    case UserBlockKind::Blockquote:
        helpContent += "</p></blockquote>\n";
        break;
    case UserBlockKind::Table:
        helpContent += "</table>\n";
        break;
    case UserBlockKind::Code:
    case UserBlockKind::Test:
        outputCode(helpCode);
        helpCode.clear();
        break;
    }
}

void GenDoc::pushState(UserBlockKind st)
{
    if (state.size())
        stateLeave(state.back());
    stateEnter(st);
    state.push_back(st);
}

void GenDoc::popState()
{
    stateLeave(state.back());
    state.pop_back();
    if (state.size())
        stateEnter(state.back());
}

void GenDoc::addTitle(const Utf8& title, int level)
{
    helpContent += Fmt("<h%d id=\"%s\">", level, title.c_str());
    helpContent += title;
    helpContent += Fmt("</h%d>", level);

    helpToc += Fmt("<li><a href=\"#%s\">%s</a></li>\n", title.c_str(), title.c_str());
    popState();
    pushState(UserBlockKind::Paragraph);
}

bool GenDoc::processFile(const Path& fileName, int titleLevel)
{
    ifstream ifs(fileName);

    if (!ifs)
    {
        Report::errorOS(Fmt(Err(Err0524), fileName.c_str()));
        return false;
    }

    Vector<Utf8> lines;
    string       line;
    while (std::getline(ifs, line))
        lines.push_back(line);

    int cptBrace = 0;

    for (int i = 0; i < lines.size(); i++)
    {
        auto& l        = lines[i];
        auto  lineTrim = l;
        lineTrim.trim();

        auto curState = state.back();

        if (lineTrim.startsWith("# "))
        {
            lineTrim.remove(0, 2);
            lineTrim.trim();
            addTitle(lineTrim, titleLevel + 1);
            continue;
        }

        if (lineTrim.startsWith("## "))
        {
            lineTrim.remove(0, 3);
            lineTrim.trim();
            addTitle(lineTrim, titleLevel + 2);
            continue;
        }

        if (lineTrim.startsWith("### "))
        {
            lineTrim.remove(0, 4);
            lineTrim.trim();
            addTitle(lineTrim, titleLevel + 3);
            continue;
        }

        if (lineTrim.startsWith("/**"))
        {
            pushState(UserBlockKind::Paragraph);
            continue;
        }

        if (curState == UserBlockKind::Paragraph && lineTrim.startsWith(">"))
        {
            pushState(UserBlockKind::Blockquote);
            curState = state.back();
        }

        if (curState == UserBlockKind::Blockquote && !lineTrim.startsWith(">"))
        {
            popState();
            curState = state.back();
        }

        if (curState == UserBlockKind::Paragraph && lineTrim.startsWith("|"))
        {
            pushState(UserBlockKind::Table);
            curState = state.back();
        }

        if (curState == UserBlockKind::Table && !lineTrim.startsWith("|"))
        {
            popState();
            curState = state.back();
        }

        switch (curState)
        {
            //////////////////////////////////
        case UserBlockKind::Blockquote:
            lineTrim.remove(0, 1);
            lineTrim.trim();
            if (lineTrim.empty())
                helpContent += "</br>\n";
            else
                helpContent += getFormattedText(lineTrim);
            break;

            //////////////////////////////////
        case UserBlockKind::Paragraph:
            if (lineTrim.empty())
            {
                popState();
                pushState(UserBlockKind::Paragraph);
            }
            else if (lineTrim.startsWith("```"))
            {
                pushState(UserBlockKind::Code);
            }
            else if (lineTrim.startsWith("* "))
            {
                pushState(UserBlockKind::List);
                i -= 1;
            }
            else if (lineTrim.startsWith("*/"))
            {
                popState();
            }
            else
                helpContent += getFormattedText(lineTrim);
            break;

            //////////////////////////////////
        case UserBlockKind::Table:
        {
            lineTrim.remove(0, 1);
            lineTrim.trim();
            Vector<Utf8> cols;
            Utf8::tokenize(lineTrim, '|', cols);
            helpContent += "<tr>";
            for (auto& c : cols)
            {
                helpContent += "<td>";
                helpContent += getFormattedText(c);
                helpContent += "</td>";
            }
            helpContent += "</tr>\n";
            break;
        }

            //////////////////////////////////
        case UserBlockKind::List:
            if (!lineTrim.startsWith("* "))
            {
                popState();
                i -= 1;
                continue;
            }

            lineTrim.remove(0, 2);
            helpContent += "<li>";
            helpContent += getFormattedText(lineTrim);
            helpContent += "</li>\n";
            break;

            //////////////////////////////////
        case UserBlockKind::Code:
            if (lineTrim.startsWith("```"))
            {
                popState();
            }
            else if (lineTrim.startsWith("#test"))
            {
                pushState(UserBlockKind::Test);
                cptBrace = lineTrim.countOf('{');
            }
            else if (!lineTrim.empty() || !helpCode.empty())
            {
                helpCode += l;
                helpCode += "\n";
            }
            break;

            //////////////////////////////////
        case UserBlockKind::Test:
            if (!cptBrace)
            {
                cptBrace += lineTrim.countOf('{');
                continue;
            }

            cptBrace += lineTrim.countOf('{');
            cptBrace -= lineTrim.countOf('}');
            if (cptBrace <= 0)
            {
                popState();
                continue;
            }

            // Remove one indentation level, as this should be the norm to have one in a #test block
            if (l.startsWith("    "))
                l.remove(0, 4);
            else if (l.startsWith("\t"))
                l.remove(0, 1);

            helpCode += l;
            helpCode += "\n";
            break;
        }
    }

    // Unstack all states
    while (state.size())
    {
        stateLeave(state.back());
        state.pop_back();
    }

    return true;
}

bool GenDoc::generateExamples()
{
    helpToc += "<ul>\n";
    sort(module->files.begin(), module->files.end(), [](SourceFile* a, SourceFile* b)
         { return strcmp(a->name.c_str(), b->name.c_str()) < 0; });

    // Parse all files
    int lastTitleLevel  = 1;
    int firstTitleLevel = lastTitleLevel;
    for (auto file : module->files)
    {
        Path path = file->name;
        path.replace_extension("");

        Utf8 name  = path.string();
        Utf8 title = name;

        int titleLevel = firstTitleLevel;
        while (title.length() > 4 && SWAG_IS_DIGIT(title[0]) && SWAG_IS_DIGIT(title[1]) && SWAG_IS_DIGIT(title[2]) && title[3] == '_')
        {
            title.remove(0, 4);
            titleLevel++;
        }

        title.buffer[0] = (char) toupper(title.buffer[0]);
        title.replace("_", " ");

        while (lastTitleLevel < titleLevel)
        {
            helpToc += "<ul>\n";
            lastTitleLevel++;
        }

        while (lastTitleLevel > titleLevel)
        {
            helpToc += "</ul>\n";
            lastTitleLevel--;
        }

        helpToc += Fmt("<li><a href=\"#%s\">%s</a></li>\n", name.c_str(), title.c_str());

        helpContent += "\n";
        helpContent += Fmt("<h%d id=\"%s\">", titleLevel, name.c_str());
        helpContent += title;
        helpContent += Fmt("</h%d>", titleLevel);

        helpToc += "<ul>\n";
        stateEnter(UserBlockKind::Code);
        state.push_back(UserBlockKind::Code);
        if (!processFile(file->path, titleLevel))
            return false;
        helpToc += "</ul>\n";
    }

    helpToc += "</ul>\n";
    return true;
}
