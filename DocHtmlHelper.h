#pragma once
#include "Concat.h"
#include "Scope.h"
#include "AstNode.h"

namespace DocHtmlHelper
{
    inline void htmlStart(Concat& outFile)
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

    inline void htmlEnd(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</div>\n");
        CONCAT_FIXED_STR(outFile, "</div>\n");

        CONCAT_FIXED_STR(outFile, "</body>\n");
        CONCAT_FIXED_STR(outFile, "</html>\n");
    }

    inline void title(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"content-title\">\n");
        CONCAT_FIXED_STR(outFile, "<h1 id=\"#top\" class=\"content-title\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h1></div>\n");
        CONCAT_FIXED_STR(outFile, "<div style = \"clear: both;\"></div>\n");
    }

    inline void summary(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"summary\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    inline void startSection(Concat& outFile, Utf8 title)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"section\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"block\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"title\">\n");
        outFile.addStringFormat("<span class=\"title\">%s</span>\n", title.c_str());
        CONCAT_FIXED_STR(outFile, "</div>\n");
        //CONCAT_FIXED_STR(outFile, "<div class=\"hr\"><hr/>\n");
    }

    inline void endSection(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</div>\n");
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    inline void startTable(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<table class=\"members\">\n");
    }

    inline void endTable(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</table>\n");
    }

    inline void startTableRow(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<tr>");
    }

    inline void endTableRow(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "</tr>");
    }

    inline void tableNameCell(Concat& outFile, Utf8 href, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"name-col\">\n");
        outFile.addStringFormat("<a href=\"%s\">", href.c_str());
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</a></td>\n");
    }

    inline void tableDescCell(Concat& outFile, Utf8 msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"desc-col\">\n");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</td>\n");
    }

    inline void table(Concat& outFile, Scope* scope, set<AstNode*>& nodes)
    {
        vector<AstNode*> sorted(nodes.begin(), nodes.end());
        sort(sorted.begin(), sorted.end(), [](const auto* p1, const auto* p2) {
            return p1->name < p2->name;
        });

        DocHtmlHelper::startTable(outFile);
        DocHtmlHelper::startTableRow(outFile);
        CONCAT_FIXED_STR(outFile, "<th class=\"name-col\">Name</th>\n");
        CONCAT_FIXED_STR(outFile, "<th class=\"desc-col\">Description</th>\n");
        DocHtmlHelper::endTableRow(outFile);

        for (auto node : sorted)
        {
            DocHtmlHelper::startTableRow(outFile);
            auto refName = scope->fullname + "." + node->name + ".html";
            DocHtmlHelper::tableNameCell(outFile, refName, node->name);
            DocHtmlHelper::tableDescCell(outFile, "description");
            DocHtmlHelper::endTableRow(outFile);
        }

        DocHtmlHelper::endTable(outFile);
    }

    inline void origin(Concat& outFile, Scope* scope)
    {
        if (!scope->fullname.empty())
        {
            CONCAT_FIXED_STR(outFile, "<br/><div class=\"origin\">\n");
            auto parentRef = scope->fullname + ".html";
            outFile.addStringFormat("<strong>Namespace: </strong><a href=\"%s\">%s</a><br/>\n", parentRef.c_str(), scope->fullname.c_str());
            CONCAT_FIXED_STR(outFile, "</div>\n");
        }
    }

}; // namespace DocHtmlHelper