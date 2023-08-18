#include "pch.h"
#include "ModuleGenDocJob.h"
#include "Module.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Workspace.h"
#include "Version.h"
#include "SyntaxColor.h"

const uint32_t COLLECT_TABLE_ZERO     = 0x00000000;
const uint32_t COLLECT_TABLE_SPECFUNC = 0x00000001;

static Utf8 toRef(Utf8 str)
{
    str.replace(".", "_");
    return str;
}

static bool canCollectNode(AstNode* node)
{
    if (node->token.text.length() > 2 && node->token.text[0] == '_' && node->token.text[1] == '_')
        return false;
    if (node->flags & AST_FROM_GENERIC)
        return false;
    if (node->flags & AST_GENERATED)
        return false;
    if (node->attributeFlags & ATTRIBUTE_NO_DOC)
        return false;
    if (!node->sourceFile)
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

    if (node->sourceFile->isRuntimeFile)
        return true;
    if (node->sourceFile->isBootstrapFile)
        return true;
    if (!node->sourceFile->forceExport && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
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
                else if (line[0] == '>')
                {
                    blk.kind = UserBlockKind::Blockquote;
                }
                else if (line[0] == '|')
                {
                    blk.kind = UserBlockKind::Table;
                }
                else if (line.length() > 1 && line[0] == '#' && SWAG_IS_BLANK(line[1]))
                {
                    blk.kind = UserBlockKind::Title1;
                }
                else if (line.length() > 2 && line[0] == '#' && line[1] == '#' && SWAG_IS_BLANK(line[2]))
                {
                    blk.kind = UserBlockKind::Title2;
                }
                else
                {
                    blk.lines.push_back(lines[start++]);
                }

                break;
            }
        }

        // The short description (first line) can end with '.'.
        if (!blk.lines.empty() && result.blocks.empty() && !blk.lines[0].empty() && blk.lines[0].back() == '.')
        {
            result.blocks.emplace_back(std::move(blk));
            continue;
        }

        for (; start < lines.size(); start++)
        {
            auto line = lines[start];
            line.trim();

            // End of the paragraph if empty line
            if (line.empty() && blk.kind != UserBlockKind::RawParagraph && blk.kind != UserBlockKind::Code)
                break;

            if (blk.kind == UserBlockKind::Blockquote && line[0] != '>')
                break;
            if (blk.kind != UserBlockKind::Blockquote && line[0] == '>')
                break;
            if (blk.kind == UserBlockKind::Table && line[0] != '|')
                break;
            if (blk.kind != UserBlockKind::Table && line[0] == '|')
                break;
            if (blk.kind != UserBlockKind::Title1 && line.length() > 1 && line[0] == '#' && SWAG_IS_BLANK(line[1]))
                break;
            if (blk.kind != UserBlockKind::Title2 && line.length() > 2 && line[0] == '#' && line[1] == '#' && SWAG_IS_BLANK(line[2]))
                break;

            if (line[0] == '>')
            {
                line.remove(0, 1);
                line.trim();
                line += "\n";
                blk.lines.push_back(line);
                continue;
            }

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

            if (blk.kind == UserBlockKind::Title1)
            {
                line.remove(0, 2);
                blk.lines.push_back(line);
                start++;
                break;
            }

            if (blk.kind == UserBlockKind::Title2)
            {
                line.remove(0, 3);
                blk.lines.push_back(line);
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

void ModuleGenDocJob::outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags)
{
    VectorNative<AstNode*> symbols;
    for (auto sym : scope->symTable.allSymbols)
    {
        for (auto n1 : sym->nodes)
        {
            if (n1->kind != kind)
                continue;

            if (kind == AstNodeKind::FuncDecl && (collectFlags & COLLECT_TABLE_SPECFUNC) && !n1->isSpecialFunctionName())
                continue;
            if (kind == AstNodeKind::FuncDecl && !(collectFlags & COLLECT_TABLE_SPECFUNC) && n1->isSpecialFunctionName())
                continue;

            if (!canCollectNode(n1))
                continue;
            symbols.push_back(n1);
        }
    }

    sort(symbols.begin(), symbols.end(), [](AstNode* a, AstNode* b)
         {
            auto a0 = a->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
            auto b0 = b->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
            return strcmp(a0.c_str(), b0.c_str()) < 0; });

    bool first = true;
    for (auto n1 : symbols)
    {
        if (first)
        {
            helpContent += Fmt("<h3>%s</h3>\n", title);
            helpContent += "<table class=\"enumeration\">\n";
            first = false;
        }

        // Name
        helpContent += "<tr>\n";
        helpContent += "<td class=\"enumeration\">\n";
        Utf8 name = n1->token.text;

        if (kind == AstNodeKind::FuncDecl)
        {
            AstFuncDecl* funcNode = CastAst<AstFuncDecl>(n1, AstNodeKind::FuncDecl);
            name += "(";
            if (funcNode->parameters && !funcNode->parameters->childs.empty())
            {
                bool firstParam = true;
                for (auto c : funcNode->parameters->childs)
                {
                    if (c->kind != AstNodeKind::FuncDeclParam)
                        continue;
                    AstVarDecl* varNode = CastAst<AstVarDecl>(c, AstNodeKind::FuncDeclParam);
                    if (!varNode->type && !varNode->typeInfo)
                        continue;
                    if (!firstParam)
                        name += ", ";
                    if (varNode->typeInfo && varNode->typeInfo->flags & TYPEINFO_SELF)
                        name += "self";
                    else if (varNode->typeInfo)
                        name += varNode->typeInfo->name;
                    else
                        name += getOutputNode(varNode->type);
                    firstParam = false;
                }
            }
            name += ")";
        }

        helpContent += Fmt("<a href=\"#%s\">%s</a>", toRef(n1->getScopedName()).c_str(), name.c_str());
        helpContent += "</td>\n";

        // Short desc
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
    helpContent += Fmt("<h3 class=\"content\" id=\"%s\">", toRef(c.fullName).c_str());

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
    if (c.nodes[0]->kind != AstNodeKind::Namespace)
    {
        helpContent += "<td class=\"srcref\">\n";
        auto srcModule = module ? module : g_Workspace->runtimeModule;
        if (srcModule->buildCfg.repoPath.buffer)
        {
            Path str = Utf8((const char*) srcModule->buildCfg.repoPath.buffer, (uint32_t) srcModule->buildCfg.repoPath.count).c_str();
            if (module)
            {
                Utf8 pathFile = c.nodes[0]->sourceFile->path.string();
                pathFile.remove(0, (uint32_t) srcModule->path.string().size() + 1);
                str.append(pathFile.c_str());
            }
            else
            {
                str.append(c.nodes[0]->sourceFile->name.c_str());
            }

            helpContent += Fmt("<a href=\"%s#L%d\" class=\"src\">[src]</a>", str.string().c_str(), c.nodes[0]->token.startLocation.line + 1);
        }

        helpContent += "</td>\n";
    }

    helpContent += "</tr>\n";
    helpContent += "</table>\n";
    helpContent += "</p>\n";
}

Utf8 ModuleGenDocJob::findReference(const Utf8& name)
{
    auto it = collectInvert.find(name);
    if (it != collectInvert.end())
        return Fmt("<a href=\"#%s\">%s</a>", toRef(it->second).c_str(), name.c_str());

    Vector<Utf8> tkns;
    Utf8::tokenize(name, '.', tkns);
    if (tkns.size() <= 1)
        return "";

    if (tkns[0] == "Swag")
    {
        return Fmt("<a href=\"swag.runtime.html#%s\">%s</a>", toRef(name).c_str(), name.c_str());
    }

    return "";
}

Utf8 ModuleGenDocJob::getReference(const Utf8& name)
{
    auto ref = findReference(name);
    if (ref.empty())
        return name;
    return ref;
}

Utf8 ModuleGenDocJob::getFormattedText(const Utf8& user)
{
    if (user.empty())
        return "";

    bool inCodeMode   = false;
    bool inBoldMode   = false;
    bool inItalicMode = false;
    Utf8 result;

    auto pz = user.c_str();
    while (*pz)
    {
        if (*pz == '<')
        {
            result += "&lt;";
            pz++;
            continue;
        }

        if (*pz == '>')
        {
            result += "&gt;";
            pz++;
            continue;
        }

        // [reference] to create an html link to the current document
        if (*pz == '[')
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
                    result += "[";
                    result += name;
                }
                else
                {
                    if (*pz == ']')
                        pz++;

                    auto ref = findReference(name);
                    if (!ref.empty())
                    {
                        result += ref;
                    }
                    else
                    {
                        if (startBracket)
                            result += "[";
                        result += name;
                        if (startBracket)
                            result += "]";
                    }
                }
                continue;
            }
            else if (startBracket)
                result += "[";
        }

        // Italic
        if (*pz == '*' && pz[1] != '*' && !SWAG_IS_BLANK(pz[1]) && (pz == user.c_str() || pz[-1] != '*'))
        {
            inItalicMode = !inItalicMode;
            if (inItalicMode)
                result += "<i>";
            else
                result += "</i>";
            pz += 1;
            continue;
        }

        // Bold
        if (*pz == '*' && pz[1] == '*' && pz[2] != '*' && !SWAG_IS_BLANK(pz[2]) && (pz == user.c_str() || pz[-1] != '*'))
        {
            inBoldMode = !inBoldMode;
            if (inBoldMode)
                result += "<b>";
            else
                result += "</b>";
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
                result += "<code class=\"incode\">";
                pz++;
                while (pz != pz1)
                    result += *pz++;
                result += "</code>";
            }
            pz++;
            continue;
        }

        // embedded code line
        if (*pz == '`')
        {
            inCodeMode = !inCodeMode;
            if (inCodeMode)
                result += "<code class=\"incode\">";
            else
                result += "</code>";
            pz++;
            continue;
        }

        result += *pz++;
    }

    // Be sure it's closed
    if (inBoldMode)
        result += "</b>";
    if (inItalicMode)
        result += "</i>";
    if (inCodeMode)
        result += "</code>";

    return result;
}

void ModuleGenDocJob::outputUserBlock(const UserBlock& user)
{
    if (user.lines.empty())
        return;

    if (user.kind == UserBlockKind::Code)
    {
        Utf8 block;
        for (auto& l : user.lines)
        {
            block += l;
            block += "\n";
        }

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
    case UserBlockKind::Blockquote:
        helpContent += "<blockquote><p>";
        break;
    case UserBlockKind::Table:
        helpContent += "<table class=\"enumeration\">\n";
        break;
    case UserBlockKind::Title1:
        helpContent += "<h2>\n";
        break;
    case UserBlockKind::Title2:
        helpContent += "<h3>\n";
        break;
    }

    bool startList = false;
    for (int i = 0; i < user.lines.size(); i++)
    {
        auto line = user.lines[i];
        line.trim();

        if (user.kind == UserBlockKind::Table)
        {
            Vector<Utf8> tkn;
            Utf8::tokenize(line, '|', tkn);
            helpContent += "<tr>";
            for (int it = 0; it < tkn.size(); it++)
            {
                auto& t = tkn[it];
                if (it == 0)
                    helpContent += "<td class=\"tdname\">";
                else
                    helpContent += "<td class=\"tdtype\">";
                helpContent += getFormattedText(t);
                helpContent += "</td>";
            }
            helpContent += "</tr>\n";
        }

        // <li>
        else if (line.length() && line[0] == '*')
        {
            if (!startList)
            {
                startList = true;
                helpContent += "<ul>\n";
            }

            helpContent += "<li>";
            line.remove(0, 1);
            helpContent += getFormattedText(line);
            helpContent += "</li>\n";
        }
        else
        {
            if (startList)
            {
                startList = false;
                helpContent += "</ul>\n";
            }

            helpContent += getFormattedText(user.lines[i]);
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
    case UserBlockKind::Blockquote:
        helpContent += "</p></blockquote>\n";
        break;
    case UserBlockKind::Table:
        helpContent += "</table>\n";
        break;
    case UserBlockKind::Title1:
        helpContent += "</h2>\n";
        break;
    case UserBlockKind::Title2:
        helpContent += "</h3>\n";
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

    // Kind of a hack for now... Try to keep references, but try to keep <> also...
    auto repl = code;
    if (code.find("<a href") == -1)
    {
        repl.replace("<", "&lt;");
        repl.replace(">", "&gt;");
    }

    // Syntax coloration
    auto codeText = syntaxColor(repl, SyntaxColorMode::ForDoc);

    // References
    repl.clear();
    const char* pz = codeText.c_str();
    while (*pz)
    {
        if (SWAG_IS_ALPHA(*pz) || *pz == '_')
        {
            Utf8 nameToRef;
            while (SWAG_IS_ALNUM(*pz) || *pz == '_' || *pz == '.')
                nameToRef += *pz++;
            repl += getReference(nameToRef);
        }
        else
        {
            repl += *pz++;
        }
    }

    helpContent += repl;
    helpContent += "</code>\n";
    helpContent += "</p>\n";
}

Utf8 ModuleGenDocJob::getOutputNode(AstNode* node)
{
    if (!node)
        return "";
    concat.clear();
    output.outputNode(outputCxt, concat, node);
    return Utf8{(const char*) concat.firstBucket->datas, (uint32_t) concat.bucketCount(concat.firstBucket)};
}

void ModuleGenDocJob::outputType(AstNode* node)
{
    if (node->typeInfo)
    {
        node->typeInfo->computeScopedNameExport();
        helpContent += getReference(node->typeInfo->scopedNameExport);
    }
    else if (node->kind == AstNodeKind::VarDecl || node->kind == AstNodeKind::ConstDecl)
    {
        auto varDecl = CastAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        helpContent += getReference(getOutputNode(varDecl->type));
    }
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
    {
        collectInvert[n->tocName]  = n->fullName;
        collectInvert[n->fullName] = n->fullName;
    }

    sort(pendingNodes.begin(), pendingNodes.end(), [](OneRef* a, OneRef* b)
         { return strcmp(a->tocName.c_str(), b->tocName.c_str()) < 0; });

    if (first)
    {
        helpContent += Fmt("<h2 class=\"section\">%s</h2>\n", sectionName);
        first = false;
    }

    helpContent += Fmt("<h3 class=\"categ\">%s</h3>\n", categName);
    helpContent += "<ul class=\"tocbullet\">\n";
    for (auto& t : pendingNodes)
        helpContent += Fmt("<li><a href=\"#%s\">%s</a></li>\n", toRef(t->fullName).c_str(), t->tocName.c_str());
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
        blockquote {\n\
            background-color:   LightYellow;\n\
            border-left:        6px solid Orange;\n\
            padding:            10px;\n\
            width:              90%;\n\
        }\n\
        a {\n\
            text-decoration: none;\n\
            color:           DoggerBlue;\n\
        }\n\
        a:hover {\n\
            text-decoration: underline;\n\
        }\n\
        a.src {\n\
            font-size:          90%;\n\
            color:              LightGrey;\n\
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
            width:              30%;\n\
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
        h1.content {\n\
            margin-top:         50px;\n\
            margin-bottom:      50px;\n\
        }\n\
        h2.section {\n\
            background-color:   Black;\n\
            color:              White;\n\
            padding:            6px;\n\
        }\n\
        table.h3 {\n\
            background-color:   Black;\n\
            color:              White;\n\
            width:              100%;\n\
            margin-top:         70px;\n\
            margin-right:       0px;\n\
        }\n\
        h3.content {\n\
            margin-bottom:      2px;\n\
            margin-right:       0px;\n\
            width:              100%;\n\
        }\n\
        .srcref {\n\
            text-align:         right;\n\
        }\n\
        .incode {\n\
            background-color:   #eeeeee;\n\
            padding:            2px;\n\
            border: 1px dotted  #cccccc;\n\
        }\n\
        .tdname .incode {\n\
            background-color:   revert;\n\
            padding:            2px;\n\
            border:             revert;\n\
        }\n\
        .addinfos {\n\
            font-size:          90%;\n\
            white-space:        break-spaces;\n\
            overflow-wrap:      break-word;\n\
        }\n\
        .code {\n\
            background-color:   #eeeeee;\n\
            border:             1px solid LightGrey;\n\
            padding:            10px;\n\
            width:              94%;\n\
            margin-left:        20px;\n\
        }\n\
        .SyntaxCode      { color: #000000; }\n\
        .SyntaxComment   { color: #6A9955; }\n\
        .SyntaxCompiler  { color: #AAAAAA; }\n\
        .SyntaxFunction  { color: #FF7411; }\n\
        .SyntaxConstant  { color: #569cd6; }\n\
        .SyntaxIntrinsic { color: #DCDCAA; }\n\
        .SyntaxType      { color: #4EC9B0; }\n\
        .SyntaxKeyword   { color: #569cd6; }\n\
        .SyntaxLogic     { color: #D8A0DF; }\n\
        .SyntaxNumber    { color: #B5CEA8; }\n\
        .SyntaxString    { color: #CE9178; }\n\
        .SyntaxAttribute { color: #AAAAAA; }\n\
    ";
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
    helpContent += "<blockquote>\n";
    helpContent += Fmt("<b>Work in progress</b>. Generated documentation (swag doc %d.%d.%d)", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    helpContent += "</blockquote>\n";

    // Output module description
    if (module)
    {
        UserComment userComment;
        computeUserComments(userComment, module->docComment);
        if (!userComment.shortDesc.lines.empty())
        {
            helpContent += "<h1 class=\"content\">Overview</h1>\n";
            outputUserBlock(userComment.shortDesc);
            outputUserComment(userComment);
        }
    }

    sort(allNodes.begin(), allNodes.end(), [this](OneRef& a, OneRef& b)
         { 
            if (a.nodes[0]->kind == AstNodeKind::ConstDecl && b.nodes[0]->kind != AstNodeKind::ConstDecl)
                return true;
            if (a.nodes[0]->kind != AstNodeKind::ConstDecl && b.nodes[0]->kind == AstNodeKind::ConstDecl)
                return false;
            return strcmp(a.fullName.buffer, b.fullName.buffer) < 0; });

    helpContent += "<h1 class=\"content\">Content</h1>\n";

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
                outputTable(scope, AstNodeKind::StructDecl, "Structs", COLLECT_TABLE_ZERO);
                outputTable(scope, AstNodeKind::EnumDecl, "Enums", COLLECT_TABLE_ZERO);
                outputTable(scope, AstNodeKind::FuncDecl, "Functions", COLLECT_TABLE_ZERO);
                outputTable(scope, AstNodeKind::AttrDecl, "Attributes", COLLECT_TABLE_ZERO);
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

                helpContent += Fmt("<td id=\"%s\" class=\"tdname\">\n", toRef(n->getScopedName()).c_str());
                helpContent += n->token.ctext();
                helpContent += "</td>\n";

                helpContent += "<td class=\"tdtype\">\n";
                auto varDecl = CastAst<AstVarDecl>(n, AstNodeKind::ConstDecl);
                outputType(varDecl);
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
                code += getOutputNode(structNode->genericParameters);
                outputCode(code);
            }

            // Fields
            if (!(structNode->attributeFlags & ATTRIBUTE_OPAQUE))
            {
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
                    if (varDecl->flags & AST_DECL_USING)
                        helpContent += "<b>using</b> ";
                    helpContent += structVal->name;
                    helpContent += "</td>\n";

                    helpContent += "<td class=\"tdtype\">\n";
                    outputType(varDecl);
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
            }

            outputUserComment(userComment);

            // Functions
            outputTable(structNode->scope, AstNodeKind::FuncDecl, "Functions", COLLECT_TABLE_ZERO);
            outputTable(structNode->scope, AstNodeKind::FuncDecl, "Special Functions", COLLECT_TABLE_SPECFUNC);
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
                code += getOutputNode(funcNode->genericParameters);
                code += " ";
                code += funcNode->token.text;
                if (!funcNode->parameters)
                    code += "()";
                else
                    code += getOutputNode(funcNode->parameters);
                code += getOutputNode(funcNode->returnType);
                if (funcNode->specFlags & AstFuncDecl::SPECFLAG_THROW)
                    code += " throw";
                code += "\n";

                if (!subUserComment.shortDesc.lines.empty())
                {
                    outputUserBlock(subUserComment.shortDesc);
                    outputCode(code);
                    code.clear();
                }
            }

            outputCode(code);

            for (auto n : c.nodes)
            {
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);
                outputUserComment(subUserComment);
            }

            break;
        }

        case AstNodeKind::AttrDecl:
        {
            outputTitle(c);

            for (auto n : c.nodes)
            {
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);
                outputUserBlock(subUserComment.shortDesc);

                auto attrNode = CastAst<AstAttrDecl>(n, AstNodeKind::AttrDecl);
                auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(attrNode->typeInfo, TypeInfoKind::FuncAttr);

                helpContent += "<div class=\"addinfos\">";
                helpContent += "<b>Usage</b>: ";
                if (typeInfo->attributeUsage & AttributeUsage::All)
                    helpContent += "all ";
                if (typeInfo->attributeUsage & AttributeUsage::Function)
                    helpContent += "function ";
                if (typeInfo->attributeUsage & AttributeUsage::FunctionParameter)
                    helpContent += "func-param ";
                if (typeInfo->attributeUsage & AttributeUsage::Enum)
                    helpContent += "enum ";
                if (typeInfo->attributeUsage & AttributeUsage::EnumValue)
                    helpContent += "enum-value ";
                if (typeInfo->attributeUsage & AttributeUsage::Struct)
                    helpContent += "struct ";
                if (typeInfo->attributeUsage & AttributeUsage::StructVariable)
                    helpContent += "struct-var ";
                if (typeInfo->attributeUsage & AttributeUsage::Variable)
                    helpContent += "var ";
                if (typeInfo->attributeUsage & AttributeUsage::GlobalVariable)
                    helpContent += "global-var ";
                if (typeInfo->attributeUsage & AttributeUsage::Constant)
                    helpContent += "const ";
                if (typeInfo->attributeUsage & AttributeUsage::Multi)
                    helpContent += "multi ";
                helpContent += "\n";
                helpContent += "</div>\n";

                Utf8 code;
                code += "attr ";
                code += attrNode->token.text;
                if (attrNode->parameters)
                    code += getOutputNode(attrNode->parameters);
                code += "\n";
                outputCode(code);
            }

            for (auto n : c.nodes)
            {
                UserComment subUserComment;
                auto        subDocComment = getDocComment(n);
                computeUserComments(subUserComment, subDocComment);
                outputUserComment(subUserComment);
            }

            break;
        }
        }
    }
}

JobResult ModuleGenDocJob::execute()
{
    concat.init();

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

    fullFileName = filePath.string();
    fullFileName.makeLower();
    fileName = filePath.filename().string();

    FILE* f = nullptr;
    if (fopen_s(&f, fullFileName.c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), fullFileName.c_str()));
        return JobResult::ReleaseJob;
    }

    helpContent += "<html>\n";
    helpContent += "<body>\n";

    helpContent += "<head>\n";
    helpContent += "<meta charset=\"UTF-8\">\n";

    // Css
    bool userCss = false;
    if (module && module->buildCfg.docCss.buffer && module->buildCfg.docCss.count)
    {
        Utf8 css{(const char*) module->buildCfg.docCss.buffer, (uint32_t) module->buildCfg.docCss.count};
        helpContent += Fmt("<link rel=\"stylesheet\" type=\"text/css\" href=\"/%s\">\n", css.c_str());
        userCss = true;
    }

    helpContent += "</head>\n";

    // Embbed styles
    if (!userCss)
        outputStyles();

    // Collect content
    if (module)
        collectScopes(module->scopeRoot);
    else
    {
        static const char* p = "https://github.com/swag-lang/swag/blob/master/bin/runtime";

        g_Workspace->runtimeModule->buildCfg.repoPath.buffer = (void*) p;
        g_Workspace->runtimeModule->buildCfg.repoPath.count  = strlen(p);

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
                auto len        = (uint32_t) c.second[0]->sourceFile->module->path.string().size();
                if (oneRef.category.length() <= len + 5) // +5 because of /src/
                    oneRef.category.clear();
                else
                    oneRef.category.remove(0, len + 5);
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

        sort(oneRef.nodes.begin(), oneRef.nodes.end(), [](AstNode* a, AstNode* b)
             { 
                auto a0 = a->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
                auto b0 = b->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
                return strcmp(a0.c_str(), b0.c_str()) < 0; });
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
