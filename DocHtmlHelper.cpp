#include "pch.h"
#include "Concat.h"
#include "DocHtmlHelper.h"
#include "Scope.h"
#include "AstNode.h"
#include "TypeInfo.h"

#define isBlank(__c) (__c == ' ' || __c == '\t')

namespace DocHtmlHelper
{
    // https://daringfireball.net/projects/markdown/syntax
    Utf8 markdown(const Utf8& msg)
    {
        Utf8 result;
        result.reserve(msg.size() * 2);

        bool openEm     = false;
        bool openStrong = false;

        const char* pz = msg.c_str();
        while (*pz)
        {
            // Escape
            if (pz[0] == '\\' && (pz[1] == '*' || pz[1] == '_'))
            {
                result += *pz++;
                if (*pz)
                    result += *pz++;
            }

            // *something
            else if (!openEm && (pz[0] == '*' || pz[0] == '_') && !isBlank(pz[1]) && pz[1] != pz[0])
            {
                openEm = true;
                result += "<em>";
                pz += 1;
            }

            // something*
            else if (openEm && !isBlank(pz[0]) && pz[0] != '*' && pz[0] != '_' && (pz[1] == '*' || pz[1] == '_') && pz[2] != pz[1])
            {
                openEm = false;
                result += *pz;
                result += "</em>";
                pz += 2;
            }

            // **something
            else if (!openStrong && (pz[0] == '*' || pz[0] == '_') && pz[1] == pz[0] && !isBlank(pz[2]))
            {
                openStrong = true;
                result += "<strong>";
                pz += 2;
            }

            // something**
            else if (openStrong && !isBlank(pz[0]) && (pz[1] == '*' || pz[1] == '_') && pz[2] == pz[1])
            {
                openStrong = false;
                result += *pz;
                result += "</strong>";
                pz += 3;
            }
            else
                result += *pz++;
        }

        return result;
    }

    void htmlStart(Concat& outFile)
    {
        CONCAT_FIXED_STR(outFile, "<html>\n");
        CONCAT_FIXED_STR(outFile, "<head>\n");
        CONCAT_FIXED_STR(outFile, "<link href=\"swag.documentation.css\" rel=\"stylesheet\" type=\"text/css\" />\n");
        CONCAT_FIXED_STR(outFile, "</head>\n");
        CONCAT_FIXED_STR(outFile, "<body>\n");

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

    void title(Concat& outFile, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<h1 class=\"content-title\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h1>\n");
        CONCAT_FIXED_STR(outFile, "<div style = \"clear: both;\"></div>\n");
    }

    void summary(Concat& outFile, const Utf8& msg)
    {
        if (msg.empty())
            return;
        CONCAT_FIXED_STR(outFile, "<div class=\"summary\">");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</div>\n");
    }

    void startSection(Concat& outFile, const Utf8& title)
    {
        CONCAT_FIXED_STR(outFile, "<div class=\"section\">\n");
        CONCAT_FIXED_STR(outFile, "<div class=\"block\">\n");
        //CONCAT_FIXED_STR(outFile, "<div class=\"title\">\n");
        outFile.addStringFormat("<h2 class=\"title\">%s</h2>\n", title.c_str());
        //CONCAT_FIXED_STR(outFile, "</div>\n");
        //CONCAT_FIXED_STR(outFile, "<div class=\"hr\"><hr/>\n");
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

    void tableNameCell(Concat& outFile, const Utf8& href, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"name-col\">\n");
        outFile.addStringFormat("<a href=\"%s\">", href.c_str());
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</a></td>\n");
    }

    void tableDescCell(Concat& outFile, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<td class=\"desc-col\">\n");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</td>\n");
    }

    void table(Concat& outFile, Scope* scope, const set<AstNode*>& nodes)
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
            if (node->kind == AstNodeKind::Namespace)
            {
                auto typeNamespace = CastTypeInfo<TypeInfoNamespace>(node->typeInfo, TypeInfoKind::Namespace);
                if (!(typeNamespace->scope->flags & SCOPE_FLAG_HAS_EXPORTS))
                    continue;
            }

            DocHtmlHelper::startTableRow(outFile);
            auto refName = scope->fullname + "." + node->name + ".html";
            DocHtmlHelper::tableNameCell(outFile, refName, node->name);
            DocHtmlHelper::tableDescCell(outFile, node->docSummary);
            DocHtmlHelper::endTableRow(outFile);
        }

        DocHtmlHelper::endTable(outFile);
    }

    void table(Concat& outFile, Scope* scope, const vector<TypeInfoParam*>& params)
    {
        vector<TypeInfoParam*> sorted(params.begin(), params.end());
        sort(sorted.begin(), sorted.end(), [](const auto* p1, const auto* p2) {
            return p1->namedParam < p2->namedParam;
        });

        DocHtmlHelper::startTable(outFile);
        DocHtmlHelper::startTableRow(outFile);
        CONCAT_FIXED_STR(outFile, "<th class=\"name-col\">Name</th>\n");
        CONCAT_FIXED_STR(outFile, "<th class=\"desc-col\">Description</th>\n");
        DocHtmlHelper::endTableRow(outFile);

        for (auto param : sorted)
        {
            DocHtmlHelper::startTableRow(outFile);
            auto refName = scope->fullname + "." + param->namedParam + ".html";
            DocHtmlHelper::tableNameCell(outFile, refName, param->namedParam);
            DocHtmlHelper::tableDescCell(outFile, param->node->docSummary);
            DocHtmlHelper::endTableRow(outFile);
        }

        DocHtmlHelper::endTable(outFile);
    }

    void origin(Concat& outFile, Scope* scope)
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