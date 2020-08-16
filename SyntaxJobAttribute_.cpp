#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Scoped.h"
#include "DocHtmlHelper.h"
#include "DocContent.h"

bool SyntaxJob::doAttrDecl(AstNode* parent, AstNode** result)
{
    auto attrNode               = Ast::newNode<AstAttrDecl>(this, AstNodeKind::AttrDecl, sourceFile, parent);
    attrNode->semanticBeforeFct = SemanticJob::preResolveAttrDecl;
    attrNode->semanticFct       = SemanticJob::resolveAttrDecl;
    if (result)
        *result = attrNode;

    SWAG_CHECK(tokenizer.getToken(token));
    SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));

    attrNode->inheritTokenName(token);

    // Register attribute
    scoped_lock lk(currentScope->symTable.mutex);
    auto        typeInfo = g_Allocator.alloc0<TypeInfoFuncAttr>();
    typeInfo->declNode   = attrNode;

    auto newScope      = Ast::newScope(attrNode, attrNode->name, ScopeKind::Attribute, currentScope);
    attrNode->typeInfo = typeInfo;
    currentScope->symTable.registerSymbolNameNoLock(&context, attrNode, SymbolKind::Attribute);

    // Parameters
    {
        Scoped scoped(this, newScope);
        SWAG_CHECK(tokenizer.getToken(token));
        SWAG_CHECK(doFuncDeclParameters(attrNode, &attrNode->parameters));
    }

    SWAG_CHECK(eatSemiCol("after attribute definition"));

    return true;
}

bool SyntaxJob::doGlobalAttributeExpose(AstNode* parent, AstNode** result)
{
    uint32_t attr = 0;

    Scope* newScope = currentScope;

    switch (token.id)
    {
    case TokenId::KwdPrivate:
        attr = ATTRIBUTE_PRIVATE;
        SWAG_VERIFY(!(parent->attributeFlags & ATTRIBUTE_PUBLIC), error(token, "attribute 'private' and attribute 'public' are mutually exclusive"));
        SWAG_VERIFY(currentScope->isGlobal(), error(token, "a private definition must appear at file or namespace scope"));
        newScope = sourceFile->scopePrivate;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdPublic:
        attr = ATTRIBUTE_PUBLIC;
        SWAG_VERIFY(!(parent->attributeFlags & ATTRIBUTE_PRIVATE), error(token, "attribute 'private' and attribute 'public' are mutually exclusive"));
        SWAG_VERIFY(currentScope->isGlobalOrImpl(), error(token, "a public definition must appear at file or namespace scope"));
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    default:
        break;
    }

    switch (token.id)
    {
    case TokenId::KwdInternal:
        attr |= ATTRIBUTE_INTERNAL;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdReadOnly:
        attr |= ATTRIBUTE_READONLY;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    case TokenId::KwdReadWrite:
        attr |= ATTRIBUTE_READWRITE;
        SWAG_CHECK(tokenizer.getToken(token));
        break;
    default:
        break;
    }

    switch (token.id)
    {
    case TokenId::SymLeftCurly:
    case TokenId::KwdFunc:
    case TokenId::KwdVar:
    case TokenId::KwdLet:
    case TokenId::KwdConst:
    case TokenId::KwdEnum:
    case TokenId::KwdStruct:
    case TokenId::KwdInterface:
    case TokenId::KwdUnion:
    case TokenId::KwdAlias:
        break;

    default:
        if (attr == ATTRIBUTE_PRIVATE)
            return syntaxError(token, format("unexpected token '%s' after 'private' attribute", token.text.c_str()));
        return syntaxError(token, format("unexpected token '%s' after 'public' attribute", token.text.c_str()));
    }

    AstNode* topStmt = nullptr;

    {
        Scoped               scoped(this, newScope);
        ScopedAttributeFlags scopedAttributes(this, attr);
        SWAG_CHECK(doTopLevelInstruction(parent, &topStmt));
        if (result)
            *result = topStmt;
    }

    // Add original scope
    if (topStmt)
    {
        SWAG_RACE_CONDITION_WRITE(topStmt->raceConditionAlternativeScopes);
        topStmt->alternativeScopes.push_back(currentScope);
    }

    return true;
}

bool SyntaxJob::doAttrUse(AstNode* parent, AstNode** result)
{
    auto attrBlockNode         = Ast::newNode<AstAttrUse>(this, AstNodeKind::AttrUse, sourceFile, parent);
    attrBlockNode->semanticFct = SemanticJob::resolveAttrUse;
    if (result)
        *result = attrBlockNode;

    while (token.id == TokenId::SymAttrStart)
    {
        SWAG_CHECK(tokenizer.getToken(token));
        while (token.id == TokenId::Identifier)
        {
            SWAG_CHECK(doIdentifierRef(attrBlockNode));
            if (token.id != TokenId::SymRightSquare)
            {
                SWAG_CHECK(eatToken(TokenId::SymComma));
                SWAG_VERIFY(token.id == TokenId::Identifier, syntaxError(token, format("invalid attribute name '%s'", token.text.c_str())));
            }
        }

        SWAG_CHECK(eatToken(TokenId::SymRightSquare));
    }

    return true;
}

bool SyntaxJob::doDocComment(AstNode* parent, AstNode** result)
{
    auto attrBlockNode = Ast::newNode<AstNode>(this, AstNodeKind::DocComment, sourceFile, parent);
    if (result)
        *result = attrBlockNode;
    attrBlockNode->docContent = g_Allocator.alloc<DocContent>();

    int pass = 0;
    while (token.id == TokenId::DocComment)
    {
        auto trimLeft = token.text;
        trimLeft.trim();

        // Start of a category with '#' : directly fill the doc content
        if (trimLeft.length() && trimLeft[0] == '#')
            pass = 3;

        switch (pass)
        {
        // Summary
        case 0:
            if (token.text.empty())
            {
                pass = 1;
            }
            else
            {
                if (!attrBlockNode->docContent->docSummary.empty())
                    attrBlockNode->docContent->docSummary += " ";
                attrBlockNode->docContent->docSummary += token.text;
            }
            break;

        // Between summary and description
        case 1:
            if (token.text.empty())
                break;
            pass = 2;

        // Description
        case 2:
            if (!token.text.empty() && token.text[0] == '#')
                pass = 3;
            else
                attrBlockNode->docContent->docDescription += token.text + "\n";
            break;

        // Content
        case 3:
            attrBlockNode->docContent->docContent += token.text + "\n";
            break;
        }

        SWAG_CHECK(tokenizer.getToken(token));
    }

    vector<Utf8> embeddedCode;
    attrBlockNode->computeScopedName();
    attrBlockNode->docContent->docSummary     = DocHtmlHelper::markdown(attrBlockNode->docContent->docSummary, attrBlockNode->computeScopedName(), embeddedCode);
    attrBlockNode->docContent->docDescription = DocHtmlHelper::markdown(attrBlockNode->docContent->docDescription, attrBlockNode->computeScopedName(), embeddedCode);
    attrBlockNode->docContent->docContent     = DocHtmlHelper::markdown(attrBlockNode->docContent->docContent, attrBlockNode->computeScopedName(), embeddedCode);

    // Construct a #test per embedded code to be able to run examples
    SyntaxJob embeddedJob;
    for (auto& oneCode : embeddedCode)
    {
        Utf8 code;
        code = "#test {\n";
        code += format("using %s\n", attrBlockNode->ownerScope->getFullName().c_str());
        code += oneCode;
        code += "}\n";

        SWAG_CHECK(embeddedJob.constructEmbedded(code, sourceFile->astRoot, attrBlockNode, CompilerAstKind::TopLevelInstruction));
        //sourceFile->astRoot->childs.back()->attributeFlags |= ATTRIBUTE_PRINTBYTECODE;
    }

    return true;
}