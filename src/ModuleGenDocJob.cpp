#include "pch.h"
#include "ModuleGenDocJob.h"
#include "Module.h"
#include "Scope.h"
#include "Utf8.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "AstNode.h"

void ModuleGenDocJob::outputNode(AstNode* node)
{
    concat.clear();
    output.outputNode(outputCxt, concat, node);
    helpContent += Utf8{(const char*) concat.firstBucket->datas, (uint32_t) concat.bucketCount(concat.firstBucket)};
}

void ModuleGenDocJob::collectNodes(Scope* root)
{
    // Generated
    if (root->name.length() > 2 && root->name[0] == '_' && root->name[1] == '_')
        return;
    if (!root->owner)
        return;
    if (root->owner && root->owner->flags & AST_FROM_GENERIC)
        return;
    if (root->owner && root->owner->flags & AST_GENERATED)
        return;

    Utf8 name;
    switch (root->owner->kind)
    {
    case AstNodeKind::Namespace:
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::EnumDecl:
        name = root->owner->getScopedName();
        break;
    default:
        break;
    }

    if (!name.empty())
    {
        auto it = collect.find(name);
        if (it != collect.end())
            collect[name].push_back(root->owner);
        else
            collect[name] = {root->owner};
    }

    for (auto c : root->childScopes)
        collectNodes(c);
}

JobResult ModuleGenDocJob::execute()
{
    concat.init();

    Path fileName = "f:/";
    fileName.append(module->name.c_str());
    fileName += ".html";

    FILE* f = nullptr;
    if (fopen_s(&f, fileName.string().c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fileName.string().c_str()));
        return JobResult::ReleaseJob;
    }

    helpContent += "<html>\n";
    helpContent += "<body>\n";

    helpContent += "<style>\n";
    helpContent +=
        ".container {\n\
            display:        flex;\n\
            flex-wrap:      nowrap;\n\
            flex-direction: row;\n\
            height:         100 %;\n\
            font-family:    arial;\n\
            line-height:    1.3em;\n\
        }\n\
        .left {\n\
            display:    block;\n\
            overflow-y: scroll;\n\
            width:      450;\n\
            height:     100%;\n\
        }\n\
        .right {\n\
            display:     block;\n\
            overflow-y:  scroll;\n\
            width:       100 %;\n\
            font-family: arial;\n\
            line-height: 1.3em;\n\
            height:      100 %;\n\
        }\n\
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n";
    helpContent += "</style>\n";

    // Main page (left and right parts, left is for table of content, right is for content)
    helpContent += "<div class=\"container\">\n";
    helpContent += "<div class=\"left\">\n";
    // seekToc : = g_Buf.length; // This will be the place where we will insert the 'g_Toc' table of content
    helpContent += "</div>\n";

    // Right page start
    helpContent += "<div class=\"right\">\n";
    helpContent += "<div class=\"page\">\n";
    helpContent += Fmt("<h1>Module %s</h1>\n", module->name.c_str());

    // Table of content start
    helpContent += "<ul>\n";

    collectNodes(module->scopeRoot);

    struct One
    {
        Utf8                   name;
        VectorNative<AstNode*> nodes;
    };

    // Sort all nodes by scoped name order
    Vector<One> allNodes;
    for (auto c : collect)
        allNodes.push_back({c.first, std::move(c.second)});
    sort(allNodes.begin(), allNodes.end(), [](One& a, One& b)
         { return strcmp(a.name.buffer, b.name.buffer) < 0; });

    for (auto& c : allNodes)
    {
        switch (c.nodes[0]->kind)
        {
        case AstNodeKind::Namespace:
            helpContent += Fmt("<h2>Namespace %s</h2>\n", c.nodes[0]->token.ctext());
            break;

        case AstNodeKind::StructDecl:
        {
            helpContent += Fmt("<h3>Struct %s</h3>\n", c.nodes[0]->token.ctext());

            auto structNode = CastAst<AstStruct>(c.nodes[0], AstNodeKind::StructDecl);
            helpContent += "<table>\n";
            for (auto structVal : structNode->scope->symTable.allSymbols)
            {
                if (structVal->nodes[0]->kind != AstNodeKind::VarDecl)
                    continue;
                if (structVal->name.find("item") == 0)
                    continue;

                auto varDecl = CastAst<AstVarDecl>(structVal->nodes[0], AstNodeKind::VarDecl);

                helpContent += "<tr>\n";

                helpContent += "<td>\n";
                outputNode(varDecl->type);
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                helpContent += structVal->name;
                helpContent += "</td>\n";
                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            break;
        }

        case AstNodeKind::InterfaceDecl:
            helpContent += Fmt("<h3>Interface %s</h3>\n", c.nodes[0]->token.ctext());
            break;

        case AstNodeKind::EnumDecl:
        {
            helpContent += Fmt("<h3>Enum %s</h3>\n", c.nodes[0]->token.ctext());

            auto enumNode = CastAst<AstEnum>(c.nodes[0], AstNodeKind::EnumDecl);
            helpContent += "<table>\n";
            for (auto enumVal : enumNode->scope->symTable.allSymbols)
            {
                helpContent += "<tr>\n";
                helpContent += "<td>\n";
                helpContent += enumVal->name;
                helpContent += "</td>\n";
                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            break;
        }

        case AstNodeKind::FuncDecl:
            helpContent += Fmt("<h3>%s</h3>\n", c.nodes[0]->token.ctext());
            break;
        }
    }

    helpContent += "</div>\n";
    helpContent += "</div>\n";
    helpContent += "</div>\n";
    helpContent += "</body>\n";
    helpContent += "</html>\n";

    fwrite(helpContent.c_str(), 1, helpContent.length(), f);
    fclose(f);

    return JobResult::ReleaseJob;
}
