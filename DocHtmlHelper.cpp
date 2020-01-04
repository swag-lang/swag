#include "pch.h"
#include "Concat.h"
#include "DocHtmlHelper.h"
#include "Scope.h"
#include "AstNode.h"
#include "TypeInfo.h"
#include "DocContent.h"

#define isBlank(__c) (__c == ' ' || __c == '\t')

namespace DocHtmlHelper
{
    // https://daringfireball.net/projects/markdown/syntax
    Utf8 markdown(const Utf8& msg, const Utf8& currentFile)
    {
        Utf8 result;
        result.reserve(msg.length() * 2);

        bool openEm     = false;
        bool openStrong = false;
        bool openCode1  = false;

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

            // [something]
            else if (pz[0] == '[' && pz[1] != pz[0])
            {
                Utf8 ref;
                Utf8 refFile = currentFile;
                pz++;
                while (*pz && *pz != ']')
                    ref += *pz++;
                if (*pz)
                    pz++;
                char* pz1 = strstr(refFile.buffer, ref.buffer);

                if (pz1 && pz1 != refFile.buffer)
                {
                    pz1 = strstr(pz1, ".");
                    if (pz1)
                        refFile.resize((int) (pz1 - refFile.buffer));
                    refFile += ".html";
                    result += format("<a href=\"%s\">%s</a>", refFile.c_str(), ref.c_str());
                }
                else
                {
                    pz1 = ref.buffer + ref.length() - 1;
                    while (pz1 != ref.buffer && *pz1 != '.')
                        pz1--;
                    if (pz1 == ref.buffer)
                    {
                        result += ref;
                        continue;
                    }

                    refFile = ref;
                    refFile += ".html";
                    ref = pz1 + 1;
                    result += format("<a href=\"%s\">%s</a>", refFile.c_str(), ref.c_str());
                }
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

            // `something
            else if (!openCode1 && pz[0] == '`' && !isBlank(pz[1]) && pz[1] != pz[0])
            {
                openCode1 = true;
                result += "<code>";
                pz += 1;
            }

            // something*
            else if (openCode1 && !isBlank(pz[0]) && pz[0] != '`' && pz[1] == '`' && pz[2] != pz[1])
            {
                openCode1 = false;
                result += *pz;
                result += "</code>";
                pz += 2;
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

        CONCAT_FIXED_STR(outFile, "<div class=\"content\">\n");
    }

    void htmlEnd(Concat& outFile)
    {
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

    void sectionTitle1(Concat& outFile, const Utf8& title)
    {
        CONCAT_FIXED_STR(outFile, "<h2>");
        outFile.addString(title);
        CONCAT_FIXED_STR(outFile, "</h2>\n");
    }

    void sectionTitle2(Concat& outFile, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<h3>");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h3>\n");
    }

    void sectionTitle3(Concat& outFile, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<h4>");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h4>\n");
    }

    void sectionTitle4(Concat& outFile, const Utf8& msg)
    {
        CONCAT_FIXED_STR(outFile, "<h5>");
        outFile.addString(msg);
        CONCAT_FIXED_STR(outFile, "</h5>\n");
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
        VectorNative<AstNode*> sorted;
        sorted.reserve((int) nodes.size());
        for (auto node : nodes)
            sorted.push_back(node);
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
            if (node->attributeFlags & ATTRIBUTE_NODOC)
                continue;
            DocHtmlHelper::startTableRow(outFile);
            auto refName = scope->fullname + "." + node->name.c_str() + ".html";
            DocHtmlHelper::tableNameCell(outFile, refName, node->name);
            DocHtmlHelper::tableDescCell(outFile, node->docContent ? node->docContent->docSummary : Utf8(""));
            DocHtmlHelper::endTableRow(outFile);
        }

        DocHtmlHelper::endTable(outFile);
    }

    void table(Concat& outFile, Scope* scope, const VectorNative<TypeInfoParam*>& params, bool specificRefFile)
    {
        VectorNative<TypeInfoParam*> sorted(params);
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
            auto refName = scope->fullname;
            if (specificRefFile)
                refName += "." + param->namedParam;
            refName += ".html";
            DocHtmlHelper::tableNameCell(outFile, refName, param->namedParam);
            DocHtmlHelper::tableDescCell(outFile, param->node->docContent ? param->node->docContent->docSummary : Utf8(""));
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