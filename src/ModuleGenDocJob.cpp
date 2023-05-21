#include "pch.h"
#include "ModuleGenDocJob.h"
#include "Module.h"
#include "Scope.h"
#include "Utf8.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "AstNode.h"
#include "Workspace.h"

Utf8 ModuleGenDocJob::outputNode(AstNode* node)
{
    if (!node)
        return "";
    concat.clear();
    output.outputNode(outputCxt, concat, node);
    return Utf8{(const char*) concat.firstBucket->datas, (uint32_t) concat.bucketCount(concat.firstBucket)};
}

void ModuleGenDocJob::outputUserLine(const Utf8& user)
{
    helpContent += user;
}

void ModuleGenDocJob::outputUserBlock(const Utf8& user)
{
    helpContent += user;
}

void ModuleGenDocJob::outputCode(const Utf8& code)
{
    if (code.empty())
        return;
    helpContent += "<div class=\"code\">\n";
    helpContent += "<code style=\"white-space: break-spaces\">";
    helpContent += code;
    helpContent += "</code>\n";
    helpContent += "</div>\n";
}

void ModuleGenDocJob::collectNode(AstNode* node)
{
    if (node->token.text.length() > 2 && node->token.text[0] == '_' && node->token.text[1] == '_')
        return;
    if (node->flags & AST_FROM_GENERIC)
        return;
    if (node->flags & AST_GENERATED)
        return;

    Utf8 name;
    switch (node->kind)
    {
    case AstNodeKind::Namespace:
        name = node->getScopedName();
        break;

    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::EnumDecl:
        if (node->sourceFile && !node->sourceFile->forceExport && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
            return;
        name = node->getScopedName();
        break;

    default:
        break;
    }

    if (!name.empty())
    {
        auto it = collect.find(name);
        if (it != collect.end())
            collect[name].push_back(node);
        else
            collect[name] = {node};
    }
}

void ModuleGenDocJob::collectScopes(Scope* root)
{
    if (root->flags & SCOPE_IMPORTED)
        return;

    if (!root->owner)
        return;
    collectNode(root->owner);

    for (auto c : root->childScopes)
        collectScopes(c);
}

void ModuleGenDocJob::generateToc()
{
    int curLevel = 0;
    for (auto& c : allNodes)
    {
        Vector<Utf8> tkn;
        Utf8::tokenize(c.name, '.', tkn);

        tkn.erase(tkn.begin());
        if (tkn.empty())
            continue;

        if (tkn.size() > curLevel)
        {
            while (tkn.size() > curLevel)
            {
                helpContent += "<ul class=\"tocbullet\">\n";
                helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", c.name.c_str(), tkn[curLevel].c_str());
                curLevel++;
            }
        }
        else if (tkn.size() < curLevel)
        {
            helpContent += "</ul>\n";
            curLevel--;
        }
        else
        {
            helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", c.name.c_str(), tkn.back().c_str());
        }
    }

    while (curLevel--)
        helpContent += "</ul>\n";
}

JobResult ModuleGenDocJob::execute()
{
    // Setup
    concat.init();
    outputCxt.checkPublic = false;
    outputCxt.exportType  = [this](TypeInfo* typeInfo)
    {
        if (!typeInfo->declNode || !typeInfo->declNode->sourceFile)
            return typeInfo->name;

        typeInfo->computeScopedNameExport();
        Vector<Utf8> tkn;
        Utf8::tokenize(typeInfo->scopedNameExport, '.', tkn);

        tkn[0].makeLower();
        return Fmt("<a href=\"%s.html#%s\">%s</a>", tkn[0].c_str(), typeInfo->scopedNameExport.c_str(), typeInfo->name.c_str());
    };

    auto fileName = g_Workspace->targetPath;
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

    // helpContent += "<head>\n";
    // helpContent += "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n";
    // helpContent += "</head>\n";

    helpContent += "<style>\n";
    helpContent +=
        ".container {\n\
            display:        flex;\n\
            flex-wrap:      nowrap;\n\
            flex-direction: row;\n\
            height:         100%;\n\
            font-family:    arial;\n\
            line-height:    1.3em;\n\
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
            font-family: arial;\n\
            line-height: 1.3em;\n\
            height:      100%;\n\
        }\n\
        table {\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              100%;\n\
        }\n\
        td {\n\
            padding:            10px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
        }\n\
        td:last-child {\n\
            width:              100%;\n\
        }\n\
        .tocbullet {\n\
            list-style-type:    none;\n\
            margin-left:        -20px;\n\
        }\n\
        .code {\n\
            background-color:   LightYellow;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              100%;\n\
        }\n\
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n";
    helpContent += "</style>\n";

    // Collect content
    collectScopes(module->scopeRoot);

    // Sort all nodes by scoped name order
    for (auto c : collect)
        allNodes.push_back({c.first, std::move(c.second)});
    sort(allNodes.begin(), allNodes.end(), [](OneRef& a, OneRef& b)
         { return strcmp(a.name.buffer, b.name.buffer) < 0; });

    // Main page (left and right parts, left is for table of content, right is for content)
    helpContent += "<div class=\"container\">\n";
    helpContent += "<div class=\"left\">\n";
    generateToc();
    helpContent += "</div>\n";

    // Right page start
    helpContent += "<div class=\"right\">\n";
    helpContent += "<div class=\"page\">\n";
    helpContent += Fmt("<h1>Module %s</h1>\n", module->name.c_str());

    for (auto& c : allNodes)
    {
        switch (c.nodes[0]->kind)
        {
        case AstNodeKind::Namespace:
            helpContent += Fmt("<h2 id=\"%s\">Namespace %s</h2>\n", c.nodes[0]->getScopedName().c_str(), c.nodes[0]->token.ctext());
            break;

        case AstNodeKind::StructDecl:
        {
            helpContent += Fmt("<h3 id=\"%s\">Struct %s</h3>\n", c.nodes[0]->getScopedName().c_str(), c.nodes[0]->token.ctext());

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
                helpContent += outputNode(varDecl->type);
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                helpContent += structVal->name;
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                if (structVal->nodes[0]->hasExtMisc())
                    outputUserLine(structVal->nodes[0]->extMisc()->docComment);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            break;
        }

        case AstNodeKind::InterfaceDecl:
            helpContent += Fmt("<h3 id=\"%s\">Interface %s</h3>\n", c.nodes[0]->getScopedName().c_str(), c.nodes[0]->token.ctext());
            break;

        case AstNodeKind::EnumDecl:
        {
            helpContent += Fmt("<h3 id=\"%s\">Enum %s</h3>\n", c.nodes[0]->getScopedName().c_str(), c.nodes[0]->token.ctext());

            auto enumNode = CastAst<AstEnum>(c.nodes[0], AstNodeKind::EnumDecl);
            helpContent += "<table>\n";
            for (auto enumVal : enumNode->scope->symTable.allSymbols)
            {
                helpContent += "<tr>\n";
                helpContent += "<td>\n";
                helpContent += enumVal->name;
                helpContent += "</td>\n";
                helpContent += "<td>\n";
                if (enumVal->nodes[0]->hasExtMisc())
                    outputUserLine(enumVal->nodes[0]->extMisc()->docComment);
                helpContent += "</td>\n";
                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            break;
        }

        case AstNodeKind::FuncDecl:
            helpContent += Fmt("<h3 id=\"%s\">%s</h3>\n", c.nodes[0]->getScopedName().c_str(), c.nodes[0]->token.ctext());

            Utf8 code;
            for (auto n : c.nodes)
            {
                auto funcNode = CastAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);
                code += "func ";
                code += funcNode->token.text;
                code += outputNode(funcNode->parameters);
                code += outputNode(funcNode->genericParameters);
                code += outputNode(funcNode->returnType);
                code += "\n";

                if (funcNode->hasExtMisc() && !funcNode->extMisc()->docComment.empty())
                {
                    outputCode(code);
                    code.clear();
                    helpContent += "<div>\n";
                    helpContent += "<p>\n";
                    outputUserBlock(funcNode->extMisc()->docComment);
                    helpContent += "</p>\n";
                    helpContent += "</div>\n";
                }
            }

            outputCode(code);
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
