#pragma once
#include "Concat.h"

namespace DocHtmlHelper
{
    void htmlStart(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<html>\n");
        CONCAT_FIXED_STR(outFile, "<head>\n");
        CONCAT_FIXED_STR(outFile, "<link href=\"swag.documentation.css\" rel=\"stylesheet\" type=\"text/css\" />\n");
        CONCAT_FIXED_STR(outFile, "</head>\n");
        CONCAT_FIXED_STR(outFile, "<body>\n");

        CONCAT_FIXED_STR(outFile, "<a name=\"top\"></a>\n");
        CONCAT_FIXED_STR(outFile, "<a id=\"back-to-top\" href=\"#top\">Top!</a>\n");

        CONCAT_FIXED_STR(outFile, "<section class=\"page-description\"><div class=\"container\"><h1>Documentation</h1></div></section>\n");

        CONCAT_FIXED_STR(outFile, "<div class=\"sharpdoc\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"content\">\n");
    }

    void htmlEnd(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</div>\n");
        CONCAT_FIXED_STR(outFile, "</div>\n");

        CONCAT_FIXED_STR(outFile, "</body>\n");
        CONCAT_FIXED_STR(outFile, "</html>\n");
    }

    void title(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"content-title\">\n");
        CONCAT_FIXED_STR(outFile, "<h1 id=\"#top\" class=\"content-title\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h1></div>");
        CONCAT_FIXED_STR(outFile, "<div style = \"clear: both;\"></div>");
    }

    void summary(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"summary\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    void startSection(Concat& outFile, Utf8 title)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"section\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"block\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"title\">\n");
        outFile.addStringFormat("<span class=\"title\">%s</span>\n", title.c_str());
        CONCAT_FIXED_STR(outFile, "</div>\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"hr\"><hr/>\n");
    }

    void endSection(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</div>\n");
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    void startTable(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<table class=\"members\">\n");
    }

    void endTable(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</table>\n");
    }

    void startTableRow(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<tr>");
    }

    void endTableRow(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</tr>");
    }

    void tableNameCell(Concat& outFile, Utf8 href, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"name-col\">\n");
        outFile.addStringFormat("<a href=\"%s\">", href.c_str());
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</a></td>\n");
    }

    void tableDescCell(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"desc-col\">\n");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</td>\n");
    }

}; // namespace DocHtmlHelper