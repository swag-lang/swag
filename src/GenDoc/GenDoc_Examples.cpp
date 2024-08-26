#include "pch.h"
#include "GenDoc/GenDoc.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Wmf/Module.h"

void GenDoc::addTitle(const Utf8& title, int level)
{
    helpContent += form("<h%d id=\"%s\">", level, title.cstr());
    helpContent += title;
    helpContent += form("</h%d>", level);

    helpToc += form("<li><a href=\"#%s\">%s</a></li>\n", title.cstr(), title.cstr());
}

bool GenDoc::processMarkDownFile(const Path& fileName, int titleLevel)
{
    std::ifstream ifs(fileName.cstr());

    if (!ifs)
    {
        Report::errorOS(formErr(Err0097, fileName.cstr()));
        return false;
    }

    Vector<Utf8> lines;
    std::string  line;
    while (std::getline(ifs, line))
        lines.push_back(line);

    UserComment result;
    computeUserComments(result, lines, false);
    outputUserComment(result, titleLevel);

    return true;
}

bool GenDoc::processSourceFile(const Path& fileName, int titleLevel)
{
    std::ifstream ifs(fileName.cstr());

    if (!ifs)
    {
        Report::errorOS(formErr(Err0097, fileName.cstr()));
        return false;
    }

    Vector<Utf8> lines;
    std::string  line;
    while (std::getline(ifs, line))
        lines.push_back(line);

    uint32_t i = 0;
    while (i < lines.size())
    {
        Utf8 code;
        while (i < lines.size())
        {
            auto& l        = lines[i++];
            Utf8  lineTrim = l;
            lineTrim.trim();
            if (lineTrim.startsWith("/**"))
                break;
            code += l;
            code += "\n";
        }

        outputCode(code, GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);

        Vector<Utf8> linesUser;
        while (i < lines.size())
        {
            auto& l        = lines[i++];
            Utf8  lineTrim = l;
            lineTrim.trim();
            if (lineTrim.startsWith("*/"))
                break;
            linesUser.push_back(l);
        }

        UserComment result;
        computeUserComments(result, linesUser, false);
        outputUserComment(result, titleLevel);
    }

    return true;
}

bool GenDoc::generateExamples()
{
    std::ranges::sort(module->files, [](const SourceFile* a, const SourceFile* b) { return strcmp(a->name, b->name) < 0; });

    helpToc += "<ul>\n";

    // Parse all files
    tocLastTitleLevel = 1;
    for (const auto file : module->files)
    {
        const Path  path  = file->name;
        const Utf8& name  = path;
        Utf8        title = path.replace_extension();

        // Be sure we have the DDD_DDD_name format
        if (title.length() < 9 ||
            !SWAG_IS_DIGIT(title[0]) ||
            !SWAG_IS_DIGIT(title[1]) ||
            !SWAG_IS_DIGIT(title[2]) ||
            !SWAG_IS_DIGIT(title[4]) ||
            !SWAG_IS_DIGIT(title[5]) ||
            !SWAG_IS_DIGIT(title[6]) ||
            title[3] != '_' ||
            title[7] != '_')
        {
            continue;
        }

        int titleLevel = 1;
        if (title[4] != '0' || title[5] != '0' || title[6] != '0')
            titleLevel = 2;
        title.remove(0, 8);

        title.buffer[0] = static_cast<char>(toupper(title.buffer[0]));
        title.replace("_", " ");

        addTocTitle(name, title, titleLevel);

        helpContent += "\n";
        helpContent += form("<h%d id=\"%s\">", titleLevel + 1, getTocTitleRef().cstr());
        helpContent += title;
        helpContent += form("</h%d>", titleLevel + 1);

        if (file->hasFlag(FILE_MARK_DOWN))
        {
            if (!processMarkDownFile(file->path, titleLevel + 1))
                return false;
        }
        else
        {
            if (!processSourceFile(file->path, titleLevel + 1))
                return false;
        }
    }

    helpToc += "</ul>\n";
    return true;
}
