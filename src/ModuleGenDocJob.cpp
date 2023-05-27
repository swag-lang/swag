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

static bool canCollectNode(AstNode* node)
{
    if (node->token.text.length() > 2 && node->token.text[0] == '_' && node->token.text[1] == '_')
        return false;
    if (node->flags & AST_FROM_GENERIC)
        return false;
    if (node->flags & AST_GENERATED)
        return false;

    switch (node->kind)
    {
    case AstNodeKind::Namespace:
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::EnumDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::AttrDecl:
        break;
    default:
        return false;
    }

    if (node->sourceFile && node->sourceFile->isRuntimeFile)
        return true;
    if (node->sourceFile && node->sourceFile->isBootstrapFile)
        return true;
    if (node->kind == AstNodeKind::FuncDecl && node->sourceFile && !node->sourceFile->forceExport && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return false;
    if (node->kind == AstNodeKind::AttrDecl && node->sourceFile && !node->sourceFile->forceExport && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return false;

    return true;
}

void ModuleGenDocJob::computeUserComments(UserComment& result, const Utf8& txt)
{
    if (txt.empty())
        return;

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
        if (result.shortDesc.lines.back().back() != '.')
            result.shortDesc.lines.back() += '.';
    }
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

void ModuleGenDocJob::outputTable(Scope* scope, AstNodeKind kind, const char* title)
{
    VectorNative<AstNode*> symbols;
    for (auto structVal : scope->symTable.allSymbols)
    {
        for (auto n1 : structVal->nodes)
        {
            if (n1->kind != kind)
                continue;
            if (!canCollectNode(n1))
                continue;
            symbols.push_back(n1);
        }
    }

    sort(symbols.begin(), symbols.end(), [](AstNode* a, AstNode* b)
         { return strcmp(a->token.ctext(), b->token.ctext()) < 0; });

    bool first = true;
    for (auto n1 : symbols)
    {
        if (first)
        {
            helpContent += Fmt("<h3>%s</h3>\n", title);
            helpContent += "<table class=\"enumeration\">\n";
            first = false;
        }

        helpContent += "<tr>\n";
        helpContent += "<td class=\"enumeration\">\n";
        helpContent += Fmt("<a href=\"#%s\">%s</a>", n1->getScopedName().c_str(), n1->token.ctext());
        helpContent += "</td>\n";

        helpContent += "<td class=\"enumeration\">\n";
        auto subDocComment = getDocComment(n1);
        if (!subDocComment.empty())
        {
            UserComment subUserComment;
            computeUserComments(subUserComment, subDocComment);
            outputUserBlock(subUserComment.shortDesc);
        }

        helpContent += "</td>\n";
        helpContent += "</tr>\n";
    }

    if (!first)
        helpContent += "</table>\n";
}

void ModuleGenDocJob::outputTitle(OneRef& c)
{
    Utf8 name;

    switch (c.nodes[0]->kind)
    {
    case AstNodeKind::Namespace:
        name = "namespace";
        break;
    case AstNodeKind::FuncDecl:
        name = "func";
        break;
    case AstNodeKind::EnumDecl:
        name = "enum";
        break;
    case AstNodeKind::StructDecl:
        name = "struct";
        break;
    case AstNodeKind::InterfaceDecl:
        name = "interface";
        break;
    case AstNodeKind::AttrDecl:
        name = "attr";
        break;
    case AstNodeKind::ConstDecl:
        name = "const";
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    helpContent += "<p>\n";
    helpContent += "<table class=\"h3\">\n";
    helpContent += "<tr>\n";
    helpContent += "<td class=\"h3\">\n";
    helpContent += Fmt("<h3 class=\"content\" id=\"%s\">", c.fullName.c_str());

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
    if (c.nodes[0]->kind == AstNodeKind::ConstDecl)
        helpContent += "Constants";
    else
        helpContent += tkn.back();
    helpContent += "</span>";

    helpContent += "</h3>\n";
    helpContent += "</td>\n";

    // Add a reference to the source code
    helpContent += "<td class=\"srcref\">\n";
    auto srcModule = module ? module : g_Workspace->runtimeModule;
    if (srcModule->buildCfg.docModulePath.buffer)
    {
        Path str = Utf8((const char*) srcModule->buildCfg.docModulePath.buffer, (uint32_t) srcModule->buildCfg.docModulePath.count).c_str();
        if (module)
        {
            Utf8 pathFile = c.nodes[0]->sourceFile->path.string();
            pathFile.remove(0, (uint32_t) srcModule->path.string().size());
            str.append(pathFile.c_str());
        }
        else
        {
            str.append(c.nodes[0]->sourceFile->name.c_str());
        }

        helpContent += Fmt("<a href=\"%s#L%d\" class=\"src\">src</a>", str.string().c_str(), c.nodes[0]->token.startLocation.line + 1);
    }

    helpContent += "</td>\n";
    helpContent += "</tr>\n";
    helpContent += "</table>\n";
    helpContent += "</p>\n";
}

void ModuleGenDocJob::outputUserLine(const Utf8& user, bool autoRef)
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

        // [reference] to create an html link to the current document
        if (autoRef || *pz == '[')
        {
            bool startBracket = false;
            if (*pz == '[')
            {
                startBracket = true;
                pz++;
            }

            if (SWAG_IS_ALPHA(*pz) || *pz == '#' || *pz == '@')
            {
                Utf8 name;
                name += *pz++;
                while (*pz && (SWAG_IS_ALNUM(*pz) || *pz == '_' || *pz == '.'))
                    name += *pz++;

                if (startBracket && *pz != ']')
                {
                    helpContent += "[";
                    helpContent += name;
                }
                else
                {
                    if (*pz == ']')
                        pz++;
                    auto it = collectInvert.find(name);
                    if (it != collectInvert.end())
                        helpContent += Fmt("<a href=\"%s#%s\">%s</a>", fileName.c_str(), it->second.c_str(), name.c_str());
                    else
                    {
                        if (startBracket)
                            helpContent += "[";
                        helpContent += name;
                        if (startBracket)
                            helpContent += "]";
                    }
                }
                continue;
            }
            else if (startBracket)
                helpContent += "[";
        }

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

    bool startList = false;
    for (int i = 0; i < user.lines.size(); i++)
    {
        auto line = user.lines[i];
        line.trim();
        if (line.length() && line[0] == '*')
        {
            if (!startList)
            {
                startList = true;
                helpContent += "<ul>\n";
            }

            helpContent += "<li>";
            line.remove(0, 1);
            outputUserLine(line);
            helpContent += "</li>\n";
        }
        else
        {
            if (startList)
            {
                startList = false;
                helpContent += "</ul>\n";
            }

            outputUserLine(user.lines[i]);
        }

        // Add one line break after each line, except the last line from a raw block, because we do
        // not want one useless empty line
        if (user.kind == UserBlockKind::RawParagraph)
        {
            if (i != user.lines.size() - 1)
                helpContent += "\n";
        }
    }

    if (startList)
    {
        startList = false;
        helpContent += "</ul>\n";
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
    if (!canCollectNode(node))
        return;

    Utf8 name = node->getScopedName();
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

void ModuleGenDocJob::generateTocCateg(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes)
{
    if (pendingNodes.empty())
        return;

    for (auto& n : pendingNodes)
    {
        Vector<Utf8> tkn;
        Utf8::tokenize(n->fullName, '.', tkn);

        if (kind == AstNodeKind::FuncDecl && tkn.size() > 1)
        {
            n->tocName = tkn[tkn.size() - 2];
            n->tocName += ".";
        }

        n->tocName += tkn[tkn.size() - 1];
    }

    bool recom = true;
    while (recom)
    {
        recom = false;

        SetUtf8 here;
        SetUtf8 conflict;

        for (auto& n : pendingNodes)
        {
            if (here.contains(n->tocName))
                conflict.insert(n->tocName);
            else
                here.insert(n->tocName);
        }

        for (auto& n : pendingNodes)
        {
            if (conflict.contains(n->tocName))
            {
                Vector<Utf8> tkn;
                Utf8::tokenize(n->fullName, '.', tkn);
                Vector<Utf8> tkn1;
                Utf8::tokenize(n->tocName, '.', tkn1);

                if (tkn.size() != tkn1.size())
                {
                    n->tocName.insert(0, ".");
                    n->tocName.insert(0, tkn[tkn.size() - tkn1.size() - 1]);
                    recom = true;
                }
            }
        }
    }

    // Invert references
    for (auto& n : pendingNodes)
        collectInvert[n->tocName] = n->fullName;

    sort(pendingNodes.begin(), pendingNodes.end(), [](OneRef* a, OneRef* b)
         { return strcmp(a->tocName.c_str(), b->tocName.c_str()) < 0; });

    if (first)
    {
        helpContent += Fmt("<h2>%s</h2>\n", sectionName);
        first = false;
    }

    helpContent += Fmt("<h3>%s</h3>\n", categName);
    helpContent += "<ul class=\"tocbullet\">\n";
    for (auto& t : pendingNodes)
        helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", t->fullName.c_str(), t->tocName.c_str());
    helpContent += "</ul>\n";

    pendingNodes.clear();
}

void ModuleGenDocJob::generateTocSection(AstNodeKind kind, const char* sectionName)
{
    bool first = true;
    Utf8 lastCateg;

    Vector<OneRef*> pendingNodes;
    for (auto& c : allNodes)
    {
        if (c.nodes[0]->kind != kind)
            continue;

        if (c.category != lastCateg)
        {
            generateTocCateg(first, kind, sectionName, lastCateg, pendingNodes);
            lastCateg = c.category;
        }

        pendingNodes.push_back(&c);
    }

    generateTocCateg(first, kind, sectionName, lastCateg, pendingNodes);
}

void ModuleGenDocJob::generateToc()
{
    sort(allNodes.begin(), allNodes.end(), [this](OneRef& a, OneRef& b)
         {
            int s0 = sortOrder(a.nodes[0]->kind);
            int s1 = sortOrder(b.nodes[0]->kind);
            if (s0 != s1)
                return s0 < s1;
            if (a.category == b.category)
                return strcmp(a.fullName.buffer, b.fullName.buffer) < 0;
            return strcmp(a.category.c_str(), b.category.c_str()) < 0; });

    if (!module)
        helpContent += "<h1>Swag Runtime</h1>\n";
    else
        helpContent += Fmt("<h1>Module %s</h1>\n", module->name.c_str());

    generateTocSection(AstNodeKind::Namespace, "Namespaces");
    generateTocSection(AstNodeKind::StructDecl, "Structs");
    generateTocSection(AstNodeKind::InterfaceDecl, "Interfaces");
    generateTocSection(AstNodeKind::EnumDecl, "Enums");
    generateTocSection(AstNodeKind::ConstDecl, "Constants");
    generateTocSection(AstNodeKind::AttrDecl, "Attributes");
    generateTocSection(AstNodeKind::FuncDecl, "Functions");
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
        a.src {\n\
            font-size:          90%;\n\
            color:              Grey;\n\
        }\n\
        table.enumeration {\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              100%;\n\
            font-size:          90%;\n\
        }\n\
        td.enumeration {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
        }\n\
        td.tdname {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
            background-color:   #f8f8f8;\n\
        }\n\
        td.tdtype {\n\
            padding:            6px;\n\
            border:             1px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              20%;\n\
            width:              auto;\n\
        }\n\
        td:last-child {\n\
            width:              auto;\n\
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
        h2.content {\n\
            margin-top:         50px;\n\
            margin-bottom:      50px;\n\
        }\n\
        table.h3 {\n\
            border-bottom:      2px solid LightGrey;\n\
            border-collapse:    collapse;\n\
            width:              100%;\n\
            margin-top:         50px;\n\
        }\n\
        h3.content {\n\
            margin-bottom:      2px;\n\
        }\n\
        .srcref {\n\
            text-align:         right;\n\
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

int ModuleGenDocJob::sortOrder(AstNodeKind kind)
{
    switch (kind)
    {
    case AstNodeKind::Namespace:
        return 0;
    case AstNodeKind::StructDecl:
        return 1;
    case AstNodeKind::InterfaceDecl:
        return 2;
    case AstNodeKind::EnumDecl:
        return 3;
    case AstNodeKind::ConstDecl:
        return 4;
    case AstNodeKind::FuncDecl:
        return 5;
    case AstNodeKind::AttrDecl:
        return 6;
    default:
        return 7;
    }
}

void ModuleGenDocJob::generateContent()
{
    sort(allNodes.begin(), allNodes.end(), [this](OneRef& a, OneRef& b)
         { return strcmp(a.fullName.buffer, b.fullName.buffer) < 0; });

    for (int i = 0; i < allNodes.size(); i++)
    {
        auto& c  = allNodes[i];
        auto  n0 = c.nodes[0];

        switch (n0->kind)
        {
        case AstNodeKind::Namespace:
        {
            outputTitle(c);

            UserComment userComment;
            auto        docComment = getDocComment(n0);
            if (!docComment.empty())
            {
                computeUserComments(userComment, docComment);
                outputUserComment(userComment);
            }

            auto namespaceDecl = CastAst<AstNameSpace>(n0, AstNodeKind::Namespace);
            if (namespaceDecl->typeInfo)
            {
                auto scope = CastTypeInfo<TypeInfoNamespace>(namespaceDecl->typeInfo, namespaceDecl->typeInfo->kind)->scope;
                outputTable(scope, AstNodeKind::StructDecl, "Structs");
                outputTable(scope, AstNodeKind::EnumDecl, "Enums");
                outputTable(scope, AstNodeKind::FuncDecl, "Functions");
                outputTable(scope, AstNodeKind::FuncDecl, "Attributes");
            }

            break;
        }

        case AstNodeKind::ConstDecl:
        {
            outputTitle(c);

            helpContent += "<table class=\"enumeration\">\n";

            for (int j = i; j < allNodes.size(); j++)
            {
                auto& c1 = allNodes[j];
                auto  n  = c1.nodes[0];
                if (n->kind != AstNodeKind::ConstDecl)
                {
                    i = j - 1;
                    break;
                }

                helpContent += "<tr>\n";

                helpContent += Fmt("<td id=\"%s\" class=\"tdname\">\n", n->getScopedName().c_str());
                helpContent += n->token.ctext();
                helpContent += "</td>\n";

                helpContent += "<td class=\"tdtype\">\n";
                auto varDecl = CastAst<AstVarDecl>(n, AstNodeKind::ConstDecl);
                outputUserLine(varDecl->typeInfo ? varDecl->typeInfo->name : outputNode(varDecl->type), true);
                helpContent += "</td>\n";

                helpContent += "<td class=\"enumeration\">\n";
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);
                outputUserBlock(subUserComment.shortDesc);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";
            break;
        }

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
        {
            outputTitle(c);

            UserComment userComment;
            auto        docComment = getDocComment(n0);
            if (!docComment.empty())
            {
                computeUserComments(userComment, docComment);
                outputUserBlock(userComment.shortDesc);
            }

            auto structNode = CastAst<AstStruct>(n0, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);

            // Output signature if structure is generic
            if (structNode->typeInfo && structNode->typeInfo->isGeneric())
            {
                Utf8 code;
                code += "struct ";
                code += structNode->token.text;
                code += outputNode(structNode->genericParameters);
                outputCode(code);
            }

            // Fields
            bool first = true;
            for (auto structVal : structNode->scope->symTable.allSymbols)
            {
                auto n1 = structVal->nodes[0];
                if (n1->kind != AstNodeKind::VarDecl && n1->kind != AstNodeKind::ConstDecl)
                    continue;
                if (!(n1->flags & AST_STRUCT_MEMBER))
                    continue;
                if (structVal->name.find("item") == 0)
                    continue;

                if (first)
                {
                    helpContent += "<table class=\"enumeration\">\n";
                    first = false;
                }

                auto varDecl = CastAst<AstVarDecl>(n1, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

                helpContent += "<tr>\n";

                helpContent += "<td class=\"tdname\">\n";
                helpContent += structVal->name;
                helpContent += "</td>\n";

                helpContent += "<td class=\"tdtype\">\n";
                outputUserLine(varDecl->typeInfo ? varDecl->typeInfo->name : outputNode(varDecl->type), true);
                helpContent += "</td>\n";

                helpContent += "<td class=\"enumeration\">\n";
                UserComment subUserComment;
                auto        subDocComment = getDocComment(varDecl);
                computeUserComments(subUserComment, subDocComment);
                outputUserBlock(subUserComment.shortDesc);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            if (!first)
                helpContent += "</table>\n";

            outputUserComment(userComment);

            // Functions
            outputTable(structNode->scope, AstNodeKind::FuncDecl, "Functions");
            break;
        }

        case AstNodeKind::EnumDecl:
        {
            outputTitle(c);

            UserComment userComment;
            auto        docComment = getDocComment(n0);
            if (!docComment.empty())
            {
                computeUserComments(userComment, docComment);
                outputUserBlock(userComment.shortDesc);
            }

            auto enumNode = CastAst<AstEnum>(n0, AstNodeKind::EnumDecl);

            helpContent += "<table class=\"enumeration\">\n";
            for (auto enumVal : enumNode->scope->symTable.allSymbols)
            {
                if (enumVal->nodes[0]->kind != AstNodeKind::EnumValue)
                    continue;

                helpContent += "<tr>\n";
                helpContent += "<td class=\"tdname\">\n";
                helpContent += enumVal->name;
                helpContent += "</td>\n";

                helpContent += "<td class=\"enumeration\">\n";
                UserComment subUserComment;
                auto        subDocComment = getDocComment(enumVal->nodes[0]);
                computeUserComments(subUserComment, subDocComment);
                outputUserBlock(subUserComment.shortDesc);
                helpContent += "</td>\n";

                helpContent += "</tr>\n";
            }

            helpContent += "</table>\n";

            outputUserComment(userComment);
            break;
        }

        case AstNodeKind::FuncDecl:
        {
            outputTitle(c);

            Utf8 code;
            for (auto n : c.nodes)
            {
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);

                auto funcNode = CastAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);

                if (n->attributeFlags & ATTRIBUTE_MACRO)
                    code += "#[Swag.Macro]\n";
                else if (n->attributeFlags & ATTRIBUTE_MIXIN)
                    code += "#[Swag.Mixin]\n";

                code += "func";
                code += outputNode(funcNode->genericParameters);
                code += " ";
                code += funcNode->token.text;
                if (!funcNode->parameters)
                    code += "()";
                else
                    code += outputNode(funcNode->parameters);
                code += outputNode(funcNode->returnType);
                if (funcNode->specFlags & AstFuncDecl::SPECFLAG_THROW)
                    code += " throw";
                code += "\n";

                if (!subUserComment.shortDesc.lines.empty() || !subUserComment.blocks.empty())
                {
                    outputCode(code);
                    code.clear();
                    outputUserBlock(subUserComment.shortDesc);
                    outputUserComment(subUserComment);
                }
            }

            outputCode(code);
            break;
        }

        case AstNodeKind::AttrDecl:
        {
            outputTitle(c);

            Utf8 code;
            for (auto n : c.nodes)
            {
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);

                auto funcNode = CastAst<AstAttrDecl>(n, AstNodeKind::AttrDecl);

                code += "attr ";
                code += funcNode->token.text;
                if (funcNode->parameters)
                    code += outputNode(funcNode->parameters);
                code += "\n";

                if (!subUserComment.shortDesc.lines.empty() || !subUserComment.blocks.empty())
                {
                    outputCode(code);
                    code.clear();
                    outputUserBlock(subUserComment.shortDesc);
                    outputUserComment(subUserComment);
                }
            }

            outputCode(code);
            break;
        }
        }
    }
}

JobResult ModuleGenDocJob::execute()
{
    // Setup
    concat.init();
    outputCxt.checkPublic = false;
    outputCxt.exportType  = [this](TypeInfo* typeInfo)
    { return typeInfo->name; };

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

    fileName = filePath.string();
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
        static const char* p = "https://github.com/swag-lang/swag/blob/master/bin/runtime";

        g_Workspace->runtimeModule->buildCfg.docModulePath.buffer = (void*) p;
        g_Workspace->runtimeModule->buildCfg.docModulePath.count  = strlen(p);

        collectScopes(g_Workspace->runtimeModule->scopeRoot);

        collectScopes(g_Workspace->bootstrapModule->scopeRoot);
    }

    // Sort all nodes by scoped name order
    for (auto c : collect)
    {
        OneRef oneRef;

        if (module)
        {
            // A namespace can be defined in multiple places, so do not add a category depending on folder,
            // as sourceFile can be irrelevant
            if (c.second[0]->kind != AstNodeKind::Namespace)
            {
                oneRef.category = c.second[0]->sourceFile->path.parent_path().string();
                oneRef.category.remove(0, (uint32_t) c.second[0]->sourceFile->module->path.string().size());
                if (oneRef.category.length() <= 5)
                    oneRef.category.clear();
                else
                    oneRef.category.remove(0, 5); // remove /src/
                oneRef.category.replace("\\", "/");
            }
        }

        oneRef.fullName = c.first;
        oneRef.nodes    = std::move(c.second);

        Vector<Utf8> tkn;
        Utf8::tokenize(oneRef.fullName, '.', tkn);

        if (oneRef.nodes[0]->kind == AstNodeKind::Namespace)
            oneRef.displayName = oneRef.fullName;
        else if (tkn.size() <= 2)
            oneRef.displayName = oneRef.fullName;
        else
        {
            oneRef.displayName = tkn[tkn.size() - 2];
            oneRef.displayName += ".";
            oneRef.displayName += tkn[tkn.size() - 1];
        }

        allNodes.push_back(oneRef);
    }

    // Main page (left and right parts, left is for table of content, right is for content)
    helpContent += "<div class=\"container\">\n";
    helpContent += "<div class=\"left\">\n";
    generateToc();
    helpContent += "</div>\n";

    // Right page start
    helpContent += "<div class=\"right\">\n";
    helpContent += "<div class=\"page\">\n";
    generateContent();
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
