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

void ModuleGenDocJob::computeUserBlock(UserBlock& result, const Utf8& txt)
{
    Vector<Utf8> lines;
    Utf8::tokenize(txt, '\n', lines);

    // Short description
    int startShortDesc = 0;
    while (startShortDesc < lines.size())
    {
        lines[startShortDesc].trim();
        if (!lines[startShortDesc].empty())
            break;
        startShortDesc++;
    }

    if (startShortDesc >= lines.size())
        return;

    int endShortDesc = startShortDesc + 1;
    while (endShortDesc < lines.size())
    {
        lines[endShortDesc].trim();
        if (lines[endShortDesc].empty())
            break;
        endShortDesc++;
    }

    for (int i = startShortDesc; i < endShortDesc; i++)
    {
        result.shortDesc += lines[i];
        if (i != endShortDesc - 1)
            result.shortDesc += "\n";
    }

    result.shortDesc.trim();
    if (result.shortDesc.back() != '.')
        result.shortDesc += ".";

    // The main description
    for (int i = endShortDesc + 1; i < lines.size(); i++)
    {
        result.desc += lines[i];
        result.desc += "\n";
    }
}

void ModuleGenDocJob::outputTitle(OneRef& c)
{
    int  level = 0;
    Utf8 name;

    switch (c.nodes[0]->kind)
    {
    case AstNodeKind::Namespace:
        level = 2;
        name  = "namespace";
        break;
    case AstNodeKind::FuncDecl:
        level = 3;
        name  = "func";
        break;
    case AstNodeKind::EnumDecl:
        level = 3;
        name  = "enum";
        break;
    case AstNodeKind::StructDecl:
        level = 3;
        name  = "struct";
        break;
    case AstNodeKind::InterfaceDecl:
        level = 3;
        name  = "interface";
        break;
    default:
        level = 3;
        break;
    }

    helpContent += Fmt("<h%d id=\"%s\">", level, c.fullName.c_str());

    Vector<Utf8> tkn;
    Utf8::tokenize(c.displayName, '.', tkn);

    helpContent += Fmt("<span class=\"titletype\">%s</span> ", name.c_str());

    helpContent += "<span class=\"titlelight\">";
    for (int i = 0; i < tkn.size() - 1; i++)
    {
        helpContent += tkn[i];
        helpContent += ".";
    }
    helpContent += "</span>";

    helpContent += "<span class=\"titlestrong\">";
    helpContent += tkn.back();
    helpContent += "</span>";

    helpContent += Fmt("</h%d>\n", level);
}

void ModuleGenDocJob::outputUserLine(const Utf8& user)
{
    if (user.empty())
        return;

    auto pz = user.c_str();
    while (*pz)
    {
        if (*pz == '<')
            helpContent += "&lt;";
        else if (*pz == '>')
            helpContent += "&gt;";
        else
            helpContent += *pz;
        pz++;
    }
}

void ModuleGenDocJob::outputUserBlock(const Utf8& user)
{
    if (user.empty())
        return;

    helpContent += "<div>\n";
    helpContent += "<p>\n";
    outputUserLine(user);
    helpContent += "</p>\n";
    helpContent += "</div>\n";
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

Utf8 ModuleGenDocJob::outputType(TypeInfo* typeInfo)
{
    auto typeRef = typeInfo;

    while (true)
    {
        if (typeRef->isPointer())
        {
            auto typePtr = CastTypeInfo<TypeInfoPointer>(typeRef, TypeInfoKind::Pointer);
            typeRef      = typePtr->pointedType;
            continue;
        }

        if (typeRef->isArray())
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(typeRef, TypeInfoKind::Array);
            typeRef      = typeArr->finalType;
            continue;
        }

        if (typeRef->isSlice())
        {
            auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeRef, TypeInfoKind::Slice);
            typeRef        = typeSlice->pointedType;
            continue;
        }

        break;
    }

    if (!typeRef->declNode || !typeRef->declNode->sourceFile)
        return typeInfo->name;

    typeRef->computeScopedNameExport();
    Vector<Utf8> tkn;
    Utf8::tokenize(typeRef->scopedNameExport, '.', tkn);

    // Remove the instantiated types to make the reference to the generic original type
    auto nameExport = typeRef->scopedNameExport;
    if (typeRef->isFromGeneric())
    {
        int p = nameExport.find("'");
        if (p != -1)
            nameExport.remove(p, nameExport.length() - p);
    }

    tkn[0].makeLower();
    return Fmt("<a href=\"%s.html#%s\">%s</a>", tkn[0].c_str(), nameExport.c_str(), typeInfo->name.c_str());
}

Utf8 ModuleGenDocJob::outputNode(AstNode* node)
{
    if (!node)
        return "";
    concat.clear();
    output.outputNode(outputCxt, concat, node);
    return Utf8{(const char*) concat.firstBucket->datas, (uint32_t) concat.bucketCount(concat.firstBucket)};
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

    if (!(root->flags & SCOPE_AUTO_GENERATED))
        collectNode(root->owner);

    for (auto c : root->childScopes)
        collectScopes(c);
}

void ModuleGenDocJob::generateToc()
{
    helpContent += Fmt("<h1>Module %s</h1>\n", module->name.c_str());

    int curLevel = 0;
    for (auto& c : allNodes)
    {
        Vector<Utf8> tkn;
        Utf8::tokenize(c.fullName, '.', tkn);

        // Display. The real name, and one level above.
        if (c.nodes[0]->kind == AstNodeKind::Namespace)
            c.displayName = c.fullName;
        else if (tkn.size() > 2)
        {
            c.displayName = tkn[tkn.size() - 2];
            c.displayName += ".";
            c.displayName += tkn[tkn.size() - 1];
        }

        if (tkn.size() > curLevel)
        {
            while (tkn.size() > curLevel)
            {
                helpContent += "<ul class=\"tocbullet\">\n";
                helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", c.fullName.c_str(), tkn[curLevel].c_str());
                curLevel++;
            }
        }
        else
        {
            while (tkn.size() < curLevel)
            {
                helpContent += "</ul>\n";
                curLevel--;
            }

            helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", c.fullName.c_str(), tkn.back().c_str());
        }
    }

    while (curLevel--)
        helpContent += "</ul>\n";
}

void ModuleGenDocJob::outputStyles()
{
    helpContent += "<style>\n";
    helpContent +=
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
        h2 {\n\
            margin-top:         50px;\n\
            margin-bottom:      50px;\n\
        }\n\
        h3 {\n\
            margin-top:         50px;\n\
            padding-bottom:     5px;\n\
            border-bottom:      2px solid LightGrey;\n\
            width:              100%;\n\
        }\n\
        .code {\n\
            background-color:   LightYellow;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              90%;\n\
            margin-left:        20px;\n\
        }\n\
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n";
    helpContent += "</style>\n";
}

JobResult ModuleGenDocJob::execute()
{
    // Setup
    concat.init();
    outputCxt.checkPublic = false;
    outputCxt.exportType  = [this](TypeInfo* typeInfo)
    { return outputType(typeInfo); };

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

    outputStyles();

    // Collect content
    collectScopes(module->scopeRoot);

    // Sort all nodes by scoped name order
    for (auto c : collect)
    {
        OneRef oneRef;
        oneRef.fullName    = c.first;
        oneRef.displayName = c.first;
        oneRef.nodes       = std::move(c.second);
        allNodes.push_back(oneRef);
    }

    sort(allNodes.begin(), allNodes.end(), [](OneRef& a, OneRef& b)
         { return strcmp(a.fullName.buffer, b.fullName.buffer) < 0; });

    // Main page (left and right parts, left is for table of content, right is for content)
    helpContent += "<div class=\"container\">\n";
    helpContent += "<div class=\"left\">\n";
    generateToc();
    helpContent += "</div>\n";

    // Right page start
    helpContent += "<div class=\"right\">\n";
    helpContent += "<div class=\"page\">\n";

    for (auto& c : allNodes)
    {
        outputTitle(c);

        switch (c.nodes[0]->kind)
        {
        case AstNodeKind::Namespace:
        {
            UserBlock userBlock;
            if (c.nodes[0]->hasExtMisc())
                computeUserBlock(userBlock, c.nodes[0]->extMisc()->docComment);
            outputUserBlock(userBlock.shortDesc);
            outputUserBlock(userBlock.desc);
            break;
        }

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
        {
            UserBlock userBlock;
            if (c.nodes[0]->hasExtMisc())
                computeUserBlock(userBlock, c.nodes[0]->extMisc()->docComment);
            outputUserBlock(userBlock.shortDesc);

            // Struct fields
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
                helpContent += outputType(varDecl->typeInfo);
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

            outputUserBlock(userBlock.desc);
            break;
        }

        case AstNodeKind::EnumDecl:
        {
            UserBlock userBlock;
            if (c.nodes[0]->hasExtMisc())
                computeUserBlock(userBlock, c.nodes[0]->extMisc()->docComment);
            outputUserBlock(userBlock.shortDesc);

            auto enumNode = CastAst<AstEnum>(c.nodes[0], AstNodeKind::EnumDecl);

            helpContent += "<table>\n";
            for (auto enumVal : enumNode->scope->symTable.allSymbols)
            {
                if (enumVal->nodes[0]->kind != AstNodeKind::EnumValue)
                    continue;

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

            outputUserBlock(userBlock.desc);
            break;
        }

        case AstNodeKind::FuncDecl:
        {
            Utf8 code;
            for (auto n : c.nodes)
            {
                UserBlock userBlock;
                if (n->hasExtMisc())
                    computeUserBlock(userBlock, n->extMisc()->docComment);

                auto funcNode = CastAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);
                code += "func";
                code += outputNode(funcNode->genericParameters);
                code += " ";
                code += funcNode->token.text;
                code += outputNode(funcNode->parameters);
                code += outputNode(funcNode->returnType);
                code += "\n";

                if (!userBlock.shortDesc.empty())
                {
                    outputCode(code);
                    code.clear();
                    outputUserBlock(userBlock.shortDesc);
                    outputUserBlock(userBlock.desc);
                }
            }

            outputCode(code);
            break;
        }
        }
    }

    helpContent += "</div>\n";
    helpContent += "</div>\n";
    helpContent += "</div>\n";
    helpContent += "</body>\n";
    helpContent += "</html>\n";

    // Write file
    fwrite(helpContent.c_str(), 1, helpContent.length(), f);
    fclose(f);

    return JobResult::ReleaseJob;
}
