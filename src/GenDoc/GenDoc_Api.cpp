#include "pch.h"
#include "GenDoc/GenDoc.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

constexpr uint32_t COLLECT_TABLE_ZERO      = 0x00000000;
constexpr uint32_t COLLECT_TABLE_SPEC_FUNC = 0x00000001;

namespace
{
    int sortOrder(AstNodeKind kind)
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
            case AstNodeKind::TypeAlias:
                return 5;
            case AstNodeKind::FuncDecl:
                return 6;
            case AstNodeKind::AttrDecl:
                return 7;
            default:
                return 8;
        }
    }

    bool canCollectNode(const AstNode* node)
    {
        if (node->token.text.length() > 2 && node->token.text[0] == '_' && node->token.text[1] == '_')
            return false;
        if (node->hasAstFlag(AST_FROM_GENERIC))
            return false;
        if (node->hasAstFlag(AST_GENERATED))
            return false;
        if (node->hasAttribute(ATTRIBUTE_NO_DOC))
            return false;
        if (!node->token.sourceFile)
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
            case AstNodeKind::TypeAlias:
                break;
            default:
                return false;
        }

        if (node->token.sourceFile->hasFlag(FILE_RUNTIME))
            return true;
        if (node->token.sourceFile->hasFlag(FILE_BOOTSTRAP))
            return true;
        if (!node->token.sourceFile->hasFlag(FILE_FORCE_EXPORT) && !node->hasAttribute(ATTRIBUTE_PUBLIC))
            return false;

        return true;
    }
}

Utf8 GenDoc::getDocComment(const AstNode* node)
{
    if (node->is(AstNodeKind::EnumValue) ||
        node->is(AstNodeKind::ConstDecl) ||
        node->is(AstNodeKind::TypeAlias) ||
        node->is(AstNodeKind::NameAlias) ||
        node->is(AstNodeKind::VarDecl))
    {
        auto scan = node;
        while (scan)
        {
            if (scan->hasExtMisc() && !scan->extMisc()->format.commentAfterSameLine.empty())
                return TokenFormat::toString(scan->extMisc()->format.commentAfterSameLine);
            scan = scan->lastChild();
        }

        return "";
    }

    if (node->hasExtMisc() && !node->extMisc()->format.commentJustBefore.empty())
        return TokenFormat::toString(node->extMisc()->format.commentJustBefore);

    while (node->parent && node->parent->is(AstNodeKind::AttrUse))
    {
        if (node->parent->hasExtMisc() && !node->parent->extMisc()->format.commentJustBefore.empty())
            return TokenFormat::toString(node->parent->extMisc()->format.commentJustBefore);
        node = node->parent;
    }

    return "";
}

void GenDoc::outputTable(Scope* scope, AstNodeKind kind, const char* title, uint32_t collectFlags)
{
    MapUtf8<VectorNative<AstNode*>> symbolsMap;
    for (const auto sym : scope->symTable.allSymbols)
    {
        for (auto n1 : sym->nodes)
        {
            if (n1->kind != kind)
                continue;

            if (kind == AstNodeKind::FuncDecl && collectFlags & COLLECT_TABLE_SPEC_FUNC && !n1->isSpecialFunctionName())
                continue;
            if (kind == AstNodeKind::FuncDecl && !(collectFlags & COLLECT_TABLE_SPEC_FUNC) && n1->isSpecialFunctionName())
                continue;

            if (!canCollectNode(n1))
                continue;

            auto it = symbolsMap.find(n1->token.text);
            if (it != symbolsMap.end())
                it->second.push_back(n1);
            else
                symbolsMap[n1->token.text] = VectorNative{n1};
        }
    }

    VectorNative<AstNode*> symbols;
    for (auto& val : symbolsMap | std::views::values)
        symbols.append(val);
    std::ranges::sort(symbols, [](const AstNode* a, const AstNode* b) {
        const auto a0 = a->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
        const auto b0 = b->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
        return strcmp(a0, b0) < 0;
    });

    bool first = true;
    for (const auto n1 : symbols)
    {
        if (first)
        {
            helpContent += form("<h3>%s</h3>\n", title);
            helpContent += "<table class=\"table-enumeration\">\n";
            first = false;
        }

        helpContent += "<tr>\n";
        helpContent += "<td class=\"code-type\">";

        if (kind == AstNodeKind::FuncDecl)
        {
            // Deal with overloads. If multiple functions with the same name, then output parameters too
            Utf8 parameters;
            if (symbolsMap.find(n1->token.text)->second.size() > 1)
            {
                const AstFuncDecl* funcNode = castAst<AstFuncDecl>(n1, AstNodeKind::FuncDecl);
                parameters                  = "(";
                if (funcNode->parameters && !funcNode->parameters->children.empty())
                {
                    bool firstParam = true;
                    for (const auto c : funcNode->parameters->children)
                    {
                        if (c->isNot(AstNodeKind::FuncDeclParam))
                            continue;
                        const AstVarDecl* varNode = castAst<AstVarDecl>(c, AstNodeKind::FuncDeclParam);
                        if (!varNode->type && !varNode->typeInfo)
                            continue;
                        if (!firstParam)
                            parameters += ", ";
                        if (varNode->typeInfo && varNode->typeInfo->isSelf())
                            parameters += "self";
                        else if (varNode->typeInfo)
                            parameters += varNode->typeInfo->name;
                        else
                            parameters += getOutputNode(varNode->type);
                        firstParam = false;
                    }
                }
                parameters += ")";
            }

            helpContent += form(R"(<span class="%s"><a href="#%s">%s</a></span>)", SYN_FUNCTION, toRef(n1->getScopedName()).c_str(), n1->token.text.c_str());
            SyntaxColorContext cxt;
            cxt.mode = SyntaxColorMode::ForDoc;
            helpContent += syntaxColor(parameters, cxt);
        }
        else
        {
            SyntaxColorContext cxt;
            cxt.mode  = SyntaxColorMode::ForDoc;
            auto name = syntaxColor(n1->token.text, cxt);
            helpContent += form("<a href=\"#%s\">%s</a>", toRef(n1->getScopedName()).c_str(), name.c_str());
        }

        helpContent += "</td>\n";

        // Short desc
        helpContent += "<td>";
        auto subDocComment = getDocComment(n1);
        if (!subDocComment.empty())
        {
            UserComment subUserComment;
            computeUserComments(subUserComment, subDocComment);
            outputUserBlock(subUserComment.shortDesc, 1, true);
        }

        helpContent += "</td>\n";
        helpContent += "</tr>\n";
    }

    if (!first)
        helpContent += "</table>\n";
}

void GenDoc::outputTitle(OneRef& c)
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
        case AstNodeKind::TypeAlias:
            name = "type alias";
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    helpContent += "<p>\n";
    helpContent += "<table class=\"api-item\">\n";
    helpContent += "<tr>\n";
    helpContent += "<td class=\"api-item\">\n";
    helpContent += form("<span id=\"%s\">", toRef(c.fullName).c_str());

    Vector<Utf8> tkn;
    Utf8::tokenize(c.displayName, '.', tkn);

    helpContent += form("<span class=\"api-item-title-kind\">%s</span> ", name.c_str());

    helpContent += "<span class=\"api-item-title-light\">";
    for (uint32_t i = 0; i < tkn.size() - 1; i++)
    {
        helpContent += tkn[i];
        helpContent += ".";
    }
    helpContent += "</span>";

    helpContent += "<span class=\"api-item-title-strong\">";
    if (c.nodes[0]->is(AstNodeKind::ConstDecl))
        helpContent += "Constants";
    else if (c.nodes[0]->is(AstNodeKind::TypeAlias))
        helpContent += "Type Aliases";
    else
        helpContent += tkn.back();
    helpContent += "</span>";

    helpContent += "</span>\n";
    helpContent += "</td>\n";

    // Add a reference to the source code
    if (c.nodes[0]->isNot(AstNodeKind::Namespace))
    {
        helpContent += "<td class=\"api-item-title-src-ref\">\n";
        Path str = Utf8(module->buildCfg.repoPath);
        if (!str.empty())
        {
            if (module != g_Workspace->runtimeModule)
            {
                Utf8 pathFile = c.nodes[0]->token.sourceFile->path;
                pathFile.remove(0, module->path.length() + 1);
                str.append(pathFile.c_str());
            }
            else
            {
                str.append(c.nodes[0]->token.sourceFile->name.c_str());
            }

            helpContent += form(R"(<a href="%s#L%d" class="src">[src]</a>)", str.c_str(), c.nodes[0]->token.startLocation.line + 1);
        }

        helpContent += "</td>\n";
    }

    helpContent += "</tr>\n";
    helpContent += "</table>\n";
    helpContent += "</p>\n";
}

Utf8 GenDoc::getOutputNode(const AstNode* node)
{
    if (!node)
        return "";
    fmtAst.clear();
    FormatContext fmtCxt;
    fmtAst.outputNode(fmtCxt, node);
    return fmtAst.getUtf8();
}

void GenDoc::outputType(AstNode* node)
{
    auto typeInfo = node->typeInfo;
    if (typeInfo && typeInfo->kind == TypeInfoKind::Alias)
        typeInfo = castTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias)->rawType;

    if (typeInfo)
    {
        typeInfo->computeScopedNameExport();
        outputCode(typeInfo->scopedNameExport, GENDOC_CODE_REFS | GENDOC_CODE_SYNTAX_COL | GENDOC_CODE_NBSP);
    }
    else if (node->is(AstNodeKind::VarDecl) || node->is(AstNodeKind::ConstDecl))
    {
        const auto varDecl = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        outputCode(getOutputNode(varDecl->type), GENDOC_CODE_REFS | GENDOC_CODE_SYNTAX_COL | GENDOC_CODE_NBSP);
    }
    else if (node->is(AstNodeKind::TypeAlias))
    {
        const auto typeDecl = castAst<AstAlias>(node, AstNodeKind::TypeAlias);
        outputCode(getOutputNode(typeDecl->firstChild()), GENDOC_CODE_REFS | GENDOC_CODE_SYNTAX_COL | GENDOC_CODE_NBSP);
    }
}

void GenDoc::collectNode(AstNode* node)
{
    if (!canCollectNode(node))
        return;

    const Utf8 name = node->getScopedName();
    if (!name.empty())
    {
        const auto it = collect.find(name);
        if (it != collect.end())
            collect[name].push_back(node);
        else
            collect[name] = VectorNative{node};
    }
}

void GenDoc::collectScopes(Scope* root)
{
    if (root->flags.has(SCOPE_IMPORTED))
        return;
    if (root->name.length() > 2 && root->name[0] == '_' && root->name[1] == '_')
        return;

    if (!root->owner)
        return;

    const auto count = collect.size();

    for (const auto c : root->childScopes)
        collectScopes(c);

    if (root->is(ScopeKind::Namespace))
    {
        for (const auto s : root->symTable.allSymbols)
        {
            if (s->nodes.empty())
                continue;
            if (s->is(SymbolKind::Variable) || s->is(SymbolKind::TypeAlias))
                collectNode(s->nodes[0]);
        }
    }

    // If something is exported inside a namespace, then force the namespace to be exported too
    if (collect.size() != count && root->is(ScopeKind::Namespace) && !root->owner->hasAttribute(ATTRIBUTE_PUBLIC))
        root->owner->addAttribute(ATTRIBUTE_PUBLIC);

    if (!root->flags.has(SCOPE_AUTO_GENERATED))
        collectNode(root->owner);
}

void GenDoc::generateTocCateg(bool& first, AstNodeKind kind, const char* sectionName, const char* categName, Vector<OneRef*>& pendingNodes)
{
    if (pendingNodes.empty())
        return;

    for (const auto& n : pendingNodes)
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

    bool restart = true;
    while (restart)
    {
        restart = false;

        SetUtf8 here;
        SetUtf8 conflict;

        for (const auto& n : pendingNodes)
        {
            if (here.contains(n->tocName))
                conflict.insert(n->tocName);
            else
                here.insert(n->tocName);
        }

        for (const auto& n : pendingNodes)
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
                    restart = true;
                }
            }
        }
    }

    // Invert references
    for (const auto& n : pendingNodes)
    {
        collectInvert[n->tocName]  = n->fullName;
        collectInvert[n->fullName] = n->fullName;
    }

    std::ranges::sort(pendingNodes, [](const OneRef* a, const OneRef* b) {
        return strcmp(a->tocName, b->tocName) < 0;
    });

    if (first)
    {
        helpToc += form("<h3>%s</h3>\n", sectionName);
        first = false;
    }

    helpToc += form("<h4>%s</h4>\n", categName);
    helpToc += "<ul>\n";
    for (const auto& t : pendingNodes)
        helpToc += form("<li><a href=\"#%s\">%s</a></li>\n", toRef(t->fullName).c_str(), t->tocName.c_str());
    helpToc += "</ul>\n";

    pendingNodes.clear();
}

void GenDoc::generateTocSection(AstNodeKind kind, const char* sectionName)
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

void GenDoc::generateToc()
{
    std::ranges::sort(allNodes, [](OneRef& a, OneRef& b) {
        const int s0 = sortOrder(a.nodes[0]->kind);
        const int s1 = sortOrder(b.nodes[0]->kind);
        if (s0 != s1)
            return s0 < s1;
        if (a.category == b.category)
            return strcmp(a.fullName.buffer, b.fullName.buffer) < 0;
        return strcmp(a.category, b.category) < 0;
    });

    generateTocSection(AstNodeKind::Namespace, "Namespaces");
    generateTocSection(AstNodeKind::StructDecl, "Structs");
    generateTocSection(AstNodeKind::InterfaceDecl, "Interfaces");
    generateTocSection(AstNodeKind::EnumDecl, "Enums");
    generateTocSection(AstNodeKind::ConstDecl, "Constants");
    generateTocSection(AstNodeKind::TypeAlias, "Type Aliases");
    generateTocSection(AstNodeKind::AttrDecl, "Attributes");
    generateTocSection(AstNodeKind::FuncDecl, "Functions");
}

void GenDoc::generateContent()
{
    // Output module description
    UserComment moduleComment;
    computeUserComments(moduleComment, module->docComment, false);
    outputUserComment(moduleComment);

    std::ranges::sort(allNodes, [](OneRef& a, OneRef& b) {
        if (a.nodes[0]->is(AstNodeKind::ConstDecl) && b.nodes[0]->isNot(AstNodeKind::ConstDecl))
            return true;
        if (a.nodes[0]->isNot(AstNodeKind::ConstDecl) && b.nodes[0]->is(AstNodeKind::ConstDecl))
            return false;
        if (a.nodes[0]->is(AstNodeKind::TypeAlias) && b.nodes[0]->isNot(AstNodeKind::TypeAlias))
            return true;
        if (a.nodes[0]->isNot(AstNodeKind::TypeAlias) && b.nodes[0]->is(AstNodeKind::TypeAlias))
            return false;
        return strcmp(a.fullName.buffer, b.fullName.buffer) < 0;
    });

    // Output content
    helpContent += "<h1>Content</h1>\n";

    for (uint32_t i = 0; i < allNodes.size(); i++)
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

                auto namespaceDecl = castAst<AstNameSpace>(n0, AstNodeKind::Namespace);
                if (namespaceDecl->typeInfo)
                {
                    auto scope = castTypeInfo<TypeInfoNamespace>(namespaceDecl->typeInfo, namespaceDecl->typeInfo->kind)->scope;
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

                helpContent += "<table class=\"table-enumeration\">\n";

                for (uint32_t j = i; j < allNodes.size(); j++)
                {
                    auto& c1 = allNodes[j];
                    auto  n  = c1.nodes[0];
                    if (n->isNot(AstNodeKind::ConstDecl))
                    {
                        i = j - 1;
                        break;
                    }

                    helpContent += "<tr>\n";

                    helpContent += form("<td id=\"%s\">", toRef(n->getScopedName()).c_str());
                    helpContent += n->token.text;
                    helpContent += "</td>\n";

                    helpContent += "<td class=\"code-type\">";
                    auto varDecl = castAst<AstVarDecl>(n, AstNodeKind::ConstDecl);
                    outputType(varDecl);
                    helpContent += "</td>\n";

                    helpContent += "<td>";
                    UserComment subUserComment;
                    auto        subDocComment = getDocComment(n);
                    computeUserComments(subUserComment, subDocComment);
                    outputUserBlock(subUserComment.shortDesc, 1, true);
                    helpContent += "</td>\n";

                    helpContent += "</tr>\n";
                }

                helpContent += "</table>\n";
                break;
            }

            case AstNodeKind::TypeAlias:
            {
                outputTitle(c);

                helpContent += "<table class=\"table-enumeration\">\n";

                for (uint32_t j = i; j < allNodes.size(); j++)
                {
                    auto& c1 = allNodes[j];
                    auto  n  = c1.nodes[0];
                    if (n->isNot(AstNodeKind::TypeAlias))
                    {
                        i = j - 1;
                        break;
                    }

                    helpContent += "<tr>\n";

                    helpContent += form("<td id=\"%s\">", toRef(n->getScopedName()).c_str());
                    helpContent += n->token.text;
                    helpContent += "</td>\n";

                    helpContent += "<td class=\"code-type\">";
                    auto typeDecl = castAst<AstAlias>(n, AstNodeKind::TypeAlias);
                    outputType(typeDecl);
                    helpContent += "</td>\n";

                    helpContent += "<td>";
                    UserComment subUserComment;
                    auto        subDocComment = getDocComment(n);
                    computeUserComments(subUserComment, subDocComment);
                    outputUserBlock(subUserComment.shortDesc, 1, true);
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

                auto structNode = castAst<AstStruct>(n0, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);

                // Output signature if structure is generic
                if (structNode->typeInfo && structNode->typeInfo->isGeneric())
                {
                    Utf8 code;
                    code += "struct ";
                    code += structNode->token.text;
                    code += getOutputNode(structNode->genericParameters);
                    outputCode(code, GENDOC_CODE_REFS | GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);
                }

                // Fields
                if (!structNode->hasAttribute(ATTRIBUTE_OPAQUE))
                {
                    bool first = true;
                    for (auto structVal : structNode->scope->symTable.allSymbols)
                    {
                        auto n1 = structVal->nodes[0];
                        if (n1->isNot(AstNodeKind::VarDecl) && n1->isNot(AstNodeKind::ConstDecl))
                            continue;
                        if (!n1->hasAstFlag(AST_STRUCT_MEMBER))
                            continue;
                        if (structVal->name.find("item") == 0)
                            continue;

                        if (first)
                        {
                            helpContent += "<table class=\"table-enumeration\">\n";
                            first = false;
                        }

                        auto varDecl = castAst<AstVarDecl>(n1, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

                        helpContent += "<tr>\n";

                        helpContent += "<td>";
                        Utf8 n2;
                        if (varDecl->hasAstFlag(AST_DECL_USING))
                            n2 += "using ";
                        n2 += structVal->name;
                        helpContent += n2;
                        helpContent += "</td>\n";

                        helpContent += "<td class=\"code-type\">";
                        outputType(varDecl);
                        helpContent += "</td>\n";

                        helpContent += "<td>";
                        UserComment subUserComment;
                        auto        subDocComment = getDocComment(varDecl);
                        computeUserComments(subUserComment, subDocComment);
                        outputUserBlock(subUserComment.shortDesc, 1, true);
                        helpContent += "</td>\n";

                        helpContent += "</tr>\n";
                    }

                    if (!first)
                        helpContent += "</table>\n";
                }

                outputUserComment(userComment);

                // Functions
                outputTable(structNode->scope, AstNodeKind::FuncDecl, "Functions", COLLECT_TABLE_ZERO);
                outputTable(structNode->scope, AstNodeKind::FuncDecl, "Special Functions", COLLECT_TABLE_SPEC_FUNC);
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

                auto enumNode = castAst<AstEnum>(n0, AstNodeKind::EnumDecl);

                helpContent += "<table class=\"table-enumeration\">\n";
                for (auto enumVal : enumNode->scope->symTable.allSymbols)
                {
                    if (enumVal->nodes[0]->isNot(AstNodeKind::EnumValue))
                        continue;

                    helpContent += "<tr>\n";
                    helpContent += "<td>";
                    helpContent += enumVal->name;
                    helpContent += "</td>\n";

                    helpContent += "<td>";
                    UserComment subUserComment;
                    auto        subDocComment = getDocComment(enumVal->nodes[0]);
                    computeUserComments(subUserComment, subDocComment);
                    outputUserBlock(subUserComment.shortDesc, 1, true);
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

                    auto funcNode = castAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);

                    if (n->hasAttribute(ATTRIBUTE_MACRO))
                        code += "#[Swag.Macro]\n";
                    else if (n->hasAttribute(ATTRIBUTE_MIXIN))
                        code += "#[Swag.Mixin]\n";

                    code += "func";
                    code += getOutputNode(funcNode->genericParameters);
                    code += " ";
                    code += funcNode->token.text;
                    if (!funcNode->parameters)
                        code += "()";
                    else
                        code += getOutputNode(funcNode->parameters);

                    if (funcNode->typeInfo)
                    {
                        auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                        if (typeFunc->returnType && !typeFunc->returnType->isVoid())
                        {
                            typeFunc->returnType->computeScopedNameExport();
                            code += "->";
                            code += typeFunc->returnType->scopedNameExport;
                        }
                    }
                    else
                    {
                        code += getOutputNode(funcNode->returnType);
                    }

                    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
                        code += " throw";
                    else if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
                        code += " assume";
                    code += "\n";

                    if (!subUserComment.shortDesc.lines.empty())
                    {
                        outputUserBlock(subUserComment.shortDesc);
                        outputCode(code, GENDOC_CODE_REFS | GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);
                        code.clear();
                    }
                }

                outputCode(code, GENDOC_CODE_REFS | GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);

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

                    auto attrNode = castAst<AstAttrDecl>(n, AstNodeKind::AttrDecl);
                    auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(attrNode->typeInfo, TypeInfoKind::FuncAttr);

                    helpContent += "<div class=\"api-additional-infos\">";
                    helpContent += "<b>Usage</b>: ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_ALL))
                        helpContent += "all ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_FUNC))
                        helpContent += "function ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_FUNC_PARAM))
                        helpContent += "func-param ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_ENUM))
                        helpContent += "enum ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_ENUM_VALUE))
                        helpContent += "enum-value ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_STRUCT))
                        helpContent += "struct ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_STRUCT_VAR))
                        helpContent += "struct-var ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_VAR))
                        helpContent += "var ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_GLOBAL_VAR))
                        helpContent += "global-var ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_CONSTANT))
                        helpContent += "const ";
                    if (typeInfo->attributeUsage.has(ATTR_USAGE_MULTI))
                        helpContent += "multi ";
                    helpContent += "\n";
                    helpContent += "</div>\n";

                    Utf8 code;
                    code += "attr ";
                    code += attrNode->token.text;
                    if (attrNode->parameters)
                        code += getOutputNode(attrNode->parameters);
                    code += "\n";
                    outputCode(code, GENDOC_CODE_REFS | GENDOC_CODE_BLOCK | GENDOC_CODE_SYNTAX_COL);
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
            default:
                break;
        }
    }
}

bool GenDoc::generateApi()
{
    // Collect content
    if (module != g_Workspace->runtimeModule)
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

        if (module != g_Workspace->runtimeModule)
        {
            // A namespace can be defined in multiple places, so do not add a category depending on folder,
            // as sourceFile can be irrelevant
            if (c.second[0]->isNot(AstNodeKind::Namespace))
            {
                oneRef.category = c.second[0]->token.sourceFile->path.parent_path();
                const auto len  = c.second[0]->token.sourceFile->module->path.length();
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

        if (oneRef.nodes[0]->is(AstNodeKind::Namespace))
            oneRef.displayName = oneRef.fullName;
        else if (tkn.size() <= 2)
            oneRef.displayName = oneRef.fullName;
        else
        {
            oneRef.displayName = tkn[tkn.size() - 2];
            oneRef.displayName += ".";
            oneRef.displayName += tkn[tkn.size() - 1];
        }

        std::ranges::sort(oneRef.nodes, [](const AstNode* a, const AstNode* b) {
            const auto a0 = a->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
            const auto b0 = b->typeInfo->computeWhateverName(COMPUTE_SCOPED_NAME);
            return strcmp(a0, b0) < 0;
        });
        allNodes.push_back(oneRef);
    }

    generateToc();
    generateContent();
    return true;
}
