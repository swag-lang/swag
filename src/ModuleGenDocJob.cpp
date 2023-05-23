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

void ModuleGenDocJob::computeUserComment(UserComment& result, const Utf8& txt)
{
    Vector<Utf8> lines;
    Utf8::tokenize(txt, '\n', lines);
    int start = 0;

    for (auto& l : lines)
    {
        if (l.back() == '\r')
            l.removeBack();
    }

    while (start < lines.size())
    {
        UserBlock blk;

        // Zap blank lines at the start of the block
        for (; start < lines.size(); start++)
        {
            auto line = lines[start];
            line.trim();
            if (!line.empty())
            {
                if (line == "---")
                {
                    blk.kind = UserBlockKind::RawParagraph;
                    start++;
                }
                else if (line == "```")
                {
                    blk.kind = UserBlockKind::Code;
                    start++;
                }
                else
                {
                    blk.lines.push_back(lines[start++]);
                }

                break;
            }
        }

        for (; start < lines.size(); start++)
        {
            auto line = lines[start];
            line.trim();

            if (line.empty() && blk.kind != UserBlockKind::RawParagraph && blk.kind != UserBlockKind::Code)
                break;

            if (line == "---")
            {
                if (blk.kind == UserBlockKind::RawParagraph)
                    start++;
                break;
            }

            if (line == "```")
            {
                if (blk.kind == UserBlockKind::Code)
                    start++;
                break;
            }

            blk.lines.push_back(lines[start]);
        }

        if (!blk.lines.empty())
            result.blocks.emplace_back(std::move(blk));
    }

    // First block is the "short description"
    if (!result.blocks.empty() && result.blocks[0].kind == UserBlockKind::Paragraph)
    {
        result.shortDesc = std::move(result.blocks[0]);
        result.blocks.erase(result.blocks.begin());
        result.shortDesc.lines[0].trim();
        if (result.shortDesc.lines.back() != '.')
            result.shortDesc.lines[0] += '.';
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

void ModuleGenDocJob::outputUserLine(const Utf8& user, UserBlockKind curBlock)
{
    if (user.empty())
        return;

    bool inCodeMode = false;
    bool inBoldMode = false;

    auto pz = user.c_str();
    while (*pz)
    {
        if (*pz == '<')
        {
            helpContent += "&lt;";
            pz++;
            continue;
        }

        if (*pz == '>')
        {
            helpContent += "&gt;";
            pz++;
            continue;
        }

        if (curBlock == UserBlockKind::Code)
        {
            helpContent += *pz++;
            continue;
        }

        // Special styles, but not in a code block

        // Bold
        if (*pz == '*' && pz[1] == '*')
        {
            inBoldMode = !inBoldMode;
            if (inBoldMode)
                helpContent += "<b>";
            else
                helpContent += "</b>";
            pz += 2;
            continue;
        }

        // 'word'
        if (*pz == '\'')
        {
            auto pz1 = pz + 1;
            while (*pz1 && !SWAG_IS_BLANK(*pz1) && *pz1 != '\'')
                pz1++;
            if (*pz1 == '\'')
            {
                helpContent += "<code class = \"incode\">";
                pz++;
                while (pz != pz1)
                    helpContent += *pz++;
                helpContent += "</code>";
            }
            pz++;
            continue;
        }

        // embedded code line
        if (*pz == '`')
        {
            inCodeMode = !inCodeMode;
            if (inCodeMode)
                helpContent += "<code class = \"incode\">";
            else
                helpContent += "</code>";
            pz++;
            continue;
        }

        helpContent += *pz++;
    }
}

void ModuleGenDocJob::outputUserBlock(const UserBlock& user)
{
    if (user.lines.empty())
        return;

    if (user.kind == UserBlockKind::Code)
    {
        Utf8 block;
        for (auto& l : user.lines)
            block += l;
        outputCode(block);
        return;
    }

    switch (user.kind)
    {
    case UserBlockKind::Paragraph:
        helpContent += "<p>\n";
        break;
    case UserBlockKind::RawParagraph:
        helpContent += "<p style=\"white-space: break-spaces\">";
        break;
    }

    for (int i = 0; i < user.lines.size(); i++)
    {
        outputUserLine(user.lines[i], user.kind);

        // Add one line break after each line, except the last line from a raw block, because we do
        // not want one useless empty line
        if (user.kind == UserBlockKind::RawParagraph)
        {
            if (i != user.lines.size() - 1)
                helpContent += "\n";
        }
    }

    switch (user.kind)
    {
    case UserBlockKind::Paragraph:
        helpContent += "</p>\n";
        break;
    case UserBlockKind::RawParagraph:
        helpContent += "</p>\n";
        break;
    }
}

void ModuleGenDocJob::outputUserComment(const UserComment& user)
{
    for (auto& b : user.blocks)
        outputUserBlock(b);
}

void ModuleGenDocJob::outputCode(const Utf8& code)
{
    if (code.empty())
        return;
    helpContent += "<p class=\"code\">\n";
    helpContent += "<code style=\"white-space: break-spaces\">";
    helpContent += code;
    helpContent += "</code>\n";
    helpContent += "</p>\n";
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
    if (typeRef->isTuple())
        return typeInfo->name;

    switch (typeRef->kind)
    {
    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    case TypeInfoKind::Enum:
        break;
    default:
        return typeInfo->name;
    }

    typeRef->computeScopedNameExport();
    Vector<Utf8> tkn;
    Utf8::tokenize(typeRef->scopedNameExport, '.', tkn);
    if (tkn.size() < 2)
        return typeInfo->name;

    // Remove the instantiated types to make the reference to the generic original type
    auto nameExport = typeRef->scopedNameExport;
    int  p          = nameExport.find("'");
    if (p != -1)
        nameExport.remove(p, nameExport.length() - p);

    tkn[0].makeLower();

    if (typeRef->declNode->sourceFile->isRuntimeFile || typeRef->declNode->sourceFile->isBootstrapFile)
    {
        return Fmt("<a href=\"swag.runtime.html#%s\">%s</a>", nameExport.c_str(), typeInfo->name.c_str());
    }
    else
    {
        auto wkpName = typeRef->declNode->sourceFile->module->path;
        wkpName      = wkpName.parent_path();
        wkpName      = wkpName.parent_path();
        wkpName      = wkpName.filename();
        return Fmt("<a href=\"%s.%s.html#%s\">%s</a>", wkpName.string().c_str(), tkn[0].c_str(), nameExport.c_str(), typeInfo->name.c_str());
    }
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
    case AstNodeKind::ConstDecl:
        if (node->sourceFile && node->sourceFile->isRuntimeFile)
            name = node->getScopedName();
        else if (node->sourceFile && node->sourceFile->isBootstrapFile)
            name = node->getScopedName();
        else if (node->sourceFile && !node->sourceFile->forceExport && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
            return;
        name = node->getScopedName();
        break;

    default:
        break;
    }

    if (node->kind == AstNodeKind::ConstDecl)
    {
        auto pz = strrchr(name.c_str(), '.');
        SWAG_ASSERT(pz);
        auto len = (uint32_t) (size_t) (pz - name.c_str());
        name.remove(len, name.length() - len);
        name += ".";
        name += "Constants";
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
    if (root->name.length() > 2 && root->name[0] == '_' && root->name[1] == '_')
        return;

    if (!root->owner)
        return;

    if (!(root->flags & SCOPE_AUTO_GENERATED))
        collectNode(root->owner);

    for (auto c : root->childScopes)
    {
        collectScopes(c);
    }

    if (root->kind == ScopeKind::Namespace)
    {
        for (auto s : root->symTable.allSymbols)
        {
            if (s->kind == SymbolKind::Variable && !s->nodes.empty())
                collectNode(s->nodes[0]);
        }
    }
}

void ModuleGenDocJob::generateToc()
{
    if (!module)
        helpContent += "<h1>Swag Runtime</h1>\n";
    else
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
        .page {\n\
            width:  1000;\n\
            margin: 0 auto;\n\
        }\n\
        a {\n\
            text-decoration: none;\n\
        }\n\
        a:hover {\n\
            text-decoration: underline;\n\
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
        .incode {\n\
            background-color:   #eeeeee;\n\
            padding:            2px;\n\
            border: 1px dotted  #cccccc;\n\
        }\n\
        .code {\n\
            background-color:   LightYellow;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              90%;\n\
            margin-left:        20px;\n\
        }\n";
    helpContent += "</style>\n";
}

Utf8 ModuleGenDocJob::getDocComment(AstNode* node)
{
    if (node->hasExtMisc() && !node->extMisc()->docComment.empty())
        return node->extMisc()->docComment;

    while (node->parent && node->parent->kind == AstNodeKind::AttrUse)
    {
        if (node->parent->hasExtMisc() && !node->parent->extMisc()->docComment.empty())
            return node->parent->extMisc()->docComment;
        node = node->parent;
    }

    return "";
}

JobResult ModuleGenDocJob::execute()
{
    // Setup
    concat.init();
    outputCxt.checkPublic = false;
    outputCxt.exportType  = [this](TypeInfo* typeInfo)
    { return outputType(typeInfo); };

    auto filePath = g_Workspace->targetPath;
    if (!module)
    {
        filePath.append("swag.runtime.html");
    }
    else
    {
        filePath.append(g_Workspace->workspacePath.filename().string().c_str());
        filePath += ".";
        filePath += module->name.c_str();
        filePath += ".html";
    }

    Utf8 fileName = filePath.string();
    fileName.makeLower();

    FILE* f = nullptr;
    if (fopen_s(&f, fileName.c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fileName.c_str()));
        return JobResult::ReleaseJob;
    }

    helpContent += "<html>\n";
    helpContent += "<body>\n";

    helpContent += "<head>\n";
    helpContent += "<meta charset=\"UTF-8\">\n";
    // helpContent += "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n";
    helpContent += "</head>\n";

    outputStyles();

    // Collect content
    if (module)
        collectScopes(module->scopeRoot);
    else
    {
        collectScopes(g_Workspace->runtimeModule->scopeRoot);
        collectScopes(g_Workspace->bootstrapModule->scopeRoot);
    }

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
            UserComment userComment;
            auto        docComment = getDocComment(c.nodes[0]);
            if (!docComment.empty())
            {
                computeUserComment(userComment, docComment);
                outputUserComment(userComment);
            }
            break;
        }

        case AstNodeKind::ConstDecl:
        {
            Utf8 code;
            helpContent += "<table>\n";

            for (auto n : c.nodes)
            {
                auto varDecl = CastAst<AstVarDecl>(n, AstNodeKind::ConstDecl);
                helpContent += "<tr>\n";

                helpContent += "<td>\n";
                if (varDecl->typeInfo)
                    helpContent += outputType(varDecl->typeInfo);
                else
                    helpContent += outputNode(varDecl->type);
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                helpContent += varDecl->token.text;
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                auto subDocComment = getDocComment(n);
                if (!subDocComment.empty())
                    outputUserLine(subDocComment);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            outputCode(code);
            break;
        }

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
        {
            UserComment userComment;
            auto        docComment = getDocComment(c.nodes[0]);
            if (!docComment.empty())
            {
                computeUserComment(userComment, docComment);
                outputUserBlock(userComment.shortDesc);
            }

            // Struct fields
            auto structNode = CastAst<AstStruct>(c.nodes[0], AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);

            if (structNode->typeInfo && structNode->typeInfo->isGeneric())
            {
                Utf8 code;
                code += "struct ";
                code += structNode->token.text;
                code += outputNode(structNode->genericParameters);
                outputCode(code);
            }

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
                if (varDecl->typeInfo)
                    helpContent += outputType(varDecl->typeInfo);
                else
                    helpContent += outputNode(varDecl->type);
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                helpContent += structVal->name;
                helpContent += "</td>\n";

                helpContent += "<td>\n";
                auto subDocComment = getDocComment(structVal->nodes[0]);
                if (!subDocComment.empty())
                    outputUserLine(subDocComment);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";

            outputUserComment(userComment);
            break;
        }

        case AstNodeKind::EnumDecl:
        {
            UserComment userComment;
            auto        docComment = getDocComment(c.nodes[0]);
            if (!docComment.empty())
            {
                computeUserComment(userComment, docComment);
                outputUserBlock(userComment.shortDesc);
            }

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
                auto subDocComment = getDocComment(enumVal->nodes[0]);
                if (!subDocComment.empty())
                    outputUserLine(subDocComment);
                helpContent += "</td>\n";
                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";

            outputUserComment(userComment);
            break;
        }

        case AstNodeKind::FuncDecl:
        {
            Utf8 code;
            for (auto n : c.nodes)
            {
                UserComment userComment;
                auto        subDocComment = getDocComment(n);
                if (!subDocComment.empty())
                    computeUserComment(userComment, subDocComment);

                auto funcNode = CastAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);
                code += "func";
                code += outputNode(funcNode->genericParameters);
                code += " ";
                code += funcNode->token.text;
                code += outputNode(funcNode->parameters);
                code += outputNode(funcNode->returnType);
                code += "\n";

                if (!userComment.shortDesc.lines.empty() || !userComment.blocks.empty())
                {
                    outputCode(code);
                    code.clear();
                    outputUserBlock(userComment.shortDesc);
                    outputUserComment(userComment);
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
