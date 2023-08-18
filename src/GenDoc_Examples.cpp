#include "pch.h"
#include "GenDoc.h"
#include "Module.h"

void GenDoc::generateExamples()
{
    sort(module->files.begin(), module->files.end(), [](SourceFile* a, SourceFile* b)
         { return strcmp(a->name.c_str(), b->name.c_str()) < 0; });

    helpToc += "<ul class=\"tocbullet\">\n";

    int lastTitleLevel = 1;
    for (auto file : module->files)
    {
        Path path = file->name;
        path.replace_extension("");

        Utf8 name  = path.string();
        Utf8 title = name;

        int titleLevel = 0;
        while (title.length() > 4 && SWAG_IS_DIGIT(title[0]) && SWAG_IS_DIGIT(title[1]) && SWAG_IS_DIGIT(title[2]) && title[3] == '_')
        {
            title.remove(0, 4);
            titleLevel++;
        }

        title.buffer[0] = (char) toupper(title.buffer[0]);
        title.replace("_", " ");

        while (lastTitleLevel < titleLevel)
        {
            helpToc += "<ul class=\"tocbullet\">\n";
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
    }

    helpToc += "</ul>\n";
}
