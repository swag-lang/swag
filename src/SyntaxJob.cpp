#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Timer.h"
#include "SemanticJob.h"
#include "Workspace.h"

thread_local Pool<SyntaxJob> g_Pool_syntaxJob;

bool SyntaxJob::verifyError(const Token& tk, bool expr, const Utf8& msg)
{
    if (!expr)
        return syntaxError(tk, msg);
    return true;
}

bool SyntaxJob::syntaxError(const Token& tk, const Utf8& msg)
{
    Utf8 full = "syntax error";
    if (!msg.empty())
    {
        full += ", ";
        full += msg;
    }

    error(tk, full);
    return false;
}

bool SyntaxJob::syntaxError(AstNode* node, const Utf8& msg)
{
    Utf8 full = "syntax error";
    if (!msg.empty())
    {
        full += ", ";
        full += msg;
    }

    error(node, full);
    return false;
}

bool SyntaxJob::invalidTokenError(InvalidTokenError kind)
{
    if (Ast::lastGeneratedNode)
    {
        switch (token.id)
        {
        case TokenId::KwdNoDrop:
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdMove)
                return syntaxError(token, "'nodrop' instruction must be placed before 'move'");
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdNoDrop)
                return syntaxError(token, "'nodrop' instruction defined twice");
            break;
        case TokenId::KwdMove:
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdMove)
                return syntaxError(token, "'move' instruction defined twice");
            break;
        }
    }

    switch (token.id)
    {
    case TokenId::KwdElse:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return syntaxError(token, "'else' without a corresponding 'if'");
        break;
    case TokenId::CompilerElse:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return syntaxError(token, "'#else' without a corresponding '#if'");
        break;
    case TokenId::CompilerElseIf:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return syntaxError(token, "'#elif' without a corresponding '#if'");
        break;

    case TokenId::SymRightParen:
        return syntaxError(token, "')' without a corresponding opening '('");
    case TokenId::SymRightCurly:
        return syntaxError(token, "'}' without a corresponding opening '{'");
    case TokenId::SymRightSquare:
        return syntaxError(token, "']' without a corresponding opening '['");
    }

    Utf8 msg;
    switch (token.id)
    {
    case TokenId::Identifier:
        msg = format("identifier '%s' ", token.text.c_str());
        break;
    default:
        msg = format("'%s' ", token.text.c_str());
        break;
    }

    switch (kind)
    {
    case InvalidTokenError::TopLevelInstruction:
        msg += "is invalid as a top level instruction";
        break;
    case InvalidTokenError::EmbeddedInstruction:
        msg += "is invalid as an embedded instruction";
        break;
    case InvalidTokenError::LeftExpression:
        msg += "is invalid as a left expression";
        break;
    case InvalidTokenError::LeftExpressionVar:
        msg += "is invalid as a left expression for variable declaration";
        break;
    case InvalidTokenError::PrimaryExpression:
        msg += "is invalid as an expression";
        break;
    }

    switch (token.id)
    {
    case TokenId::Identifier:
        if (kind == InvalidTokenError::TopLevelInstruction)
        {
            Token nextToken;
            tokenizer.getToken(nextToken);
            if (nextToken.id == TokenId::SymEqual || nextToken.id == TokenId::SymColonEqual || nextToken.id == TokenId::SymColon)
            {
                msg += ", do you miss 'var' or 'const' to declare a global variable ?";
            }
        }
        break;
    }

    return syntaxError(token, msg);
}

bool SyntaxJob::error(AstNode* node, const Utf8& msg)
{
    sourceFile->report({node, msg.c_str()});
    return false;
}

bool SyntaxJob::error(const Token& tk, const Utf8& msg)
{
    sourceFile->report({sourceFile, tk, msg.c_str()});
    return false;
}

bool SyntaxJob::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg)
{
    sourceFile->report({sourceFile, startLocation, endLocation, msg.c_str()});
    return false;
}

bool SyntaxJob::eatToken()
{
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::eatToken(TokenId id, const char* msg)
{
    if (token.id != id)
    {
        if (!msg)
            msg = "";
        if (token.id == TokenId::EndOfFile)
            SWAG_CHECK(syntaxError(token, format("missing '%s' %s", g_LangSpec.tokenToName(id).c_str(), msg)));
        else
            SWAG_CHECK(syntaxError(token, format("'%s' is expected instead of '%s' %s", g_LangSpec.tokenToName(id).c_str(), token.text.c_str(), msg)));
    }

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::eatSemiCol(const char* msg)
{
    if (token.id != TokenId::SymSemiColon && token.id != TokenId::EndOfFile && !tokenizer.lastTokenIsEOL)
    {
        if (!msg)
            msg = "";
        SWAG_CHECK(syntaxError(token, format("';' or a end of line is expected instead of '%s' %s", token.text.c_str(), msg)));
    }

    if (token.id == TokenId::SymSemiColon)
        SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::constructEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind, bool logGenerated)
{
    Utf8     tmpFileName     = "<generated>";
    Utf8     tmpFilePath     = "<generated>";
    uint32_t previousLogLine = 0;

    // Log the generated code in 'generated.swg'
    if (logGenerated && !fromNode->sourceFile->numTestErrors)
    {
        auto modl       = fromNode->sourceFile->module;
        Utf8 publicPath = g_Workspace.getPublicPath(modl, true);
        tmpFilePath     = publicPath;
        tmpFileName     = modl->name + ".generated.swg";
        publicPath += tmpFileName;

        uint32_t    countEol = 0;
        const char* pz       = content.c_str();
        for (int i = 0; i < content.length(); i++)
        {
            if (*pz == '\n')
                countEol++;
            pz++;
        }

        unique_lock lk(modl->mutexGeneratedFile);

        previousLogLine = modl->countLinesGeneratedFile;
        if (!modl->handleGeneratedFile)
        {
            if (modl->firstGenerated)
                fopen_s(&modl->handleGeneratedFile, publicPath.c_str(), "wN");
            else
                fopen_s(&modl->handleGeneratedFile, publicPath.c_str(), "a+N");
            if (!modl->handleGeneratedFile)
            {
                fromNode->sourceFile->report({fromNode, fromNode->token, format("cannot open file '%s' for writing", publicPath.c_str())});
                return false;
            }
        }

        modl->firstGenerated = false;

        Utf8 sourceCode = format("// %s:%d:%d:%d:%d\n", fromNode->sourceFile->path.c_str(), fromNode->token.startLocation.line + 1, fromNode->token.startLocation.column + 1, fromNode->token.endLocation.line + 1, fromNode->token.endLocation.column + 1);
        fwrite(sourceCode.c_str(), sourceCode.length(), 1, modl->handleGeneratedFile);
        modl->countLinesGeneratedFile += 1;

        fwrite(content.c_str(), content.length(), 1, modl->handleGeneratedFile);
        modl->countLinesGeneratedFile += countEol;

        static char eol = '\n';
        fwrite(&eol, 1, 1, modl->handleGeneratedFile);
        fwrite(&eol, 1, 1, modl->handleGeneratedFile);
        modl->countLinesGeneratedFile += 2;
    }

    SourceFile* tmpFile      = g_Allocator.alloc<SourceFile>();
    tmpFile->externalContent = content;
    tmpFile->setExternalBuffer((char*) tmpFile->externalContent.c_str(), tmpFile->externalContent.length());
    tmpFile->module     = parent->sourceFile->module;
    tmpFile->name       = tmpFileName;
    tmpFile->path       = tmpFilePath;
    tmpFile->sourceNode = fromNode;
    sourceFile          = tmpFile;
    currentScope        = parent->ownerScope;
    currentStructScope  = parent->ownerStructScope;
    currentMainNode     = parent->ownerMainNode;
    currentFct          = parent->ownerFct;
    currentInline       = parent->ownerInline;

    tokenizer.setFile(sourceFile);
    if (logGenerated)
    {
        tokenizer.location.column = 0;
        tokenizer.location.line   = previousLogLine;
    }

    ScopedFlags scopedFlags(this, AST_GENERATED | (parent->flags & (AST_RUN_BLOCK | AST_NO_BACKEND)));
    SWAG_CHECK(tokenizer.getToken(token));
    while (true)
    {
        if (token.id == TokenId::EndOfFile)
            break;
        switch (kind)
        {
        case CompilerAstKind::EmbeddedInstruction:
            SWAG_CHECK(doEmbeddedInstruction(parent));
            break;
        case CompilerAstKind::TopLevelInstruction:
            SWAG_CHECK(doTopLevelInstruction(parent));
            break;
        case CompilerAstKind::StructVarDecl:
            SWAG_CHECK(doVarDecl(parent, nullptr, AstNodeKind::VarDecl));
            break;
        case CompilerAstKind::EnumValue:
            SWAG_CHECK(doEnumValue(parent));
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    return true;
}

JobResult SyntaxJob::execute()
{
    Timer timer(&g_Stats.syntaxTime);
    timer.start();

    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    g_Stats.numFiles++;
    Ast::lastGeneratedNode = nullptr;

    tokenizer.setFile(sourceFile);

    module = sourceFile->module;

    // Setup root ast for file
    sourceFile->astRoot = Ast::newNode<AstNode>(this, AstNodeKind::File, sourceFile, module->astRoot);

    // Creates a namespace if this is an imported file
    currentScope     = module->scopeRoot;
    auto parentScope = module->scopeRoot;
    if (sourceFile->imported)
    {
        SWAG_ASSERT(!sourceFile->forceNamespace.empty());
        auto namespaceNode                = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, sourceFile->astRoot);
        namespaceNode->semanticFct        = SemanticJob::resolveNamespace;
        namespaceNode->token.text         = sourceFile->forceNamespace;
        namespaceNode->importedModuleName = sourceFile->imported->name;

        scoped_lock lk(parentScope->symTable.mutex);
        auto        symbol = parentScope->symTable.findNoLock(sourceFile->forceNamespace);
        if (!symbol)
        {
            auto typeInfo  = allocType<TypeInfoNamespace>();
            typeInfo->name = sourceFile->forceNamespace;
            auto newScope  = Ast::newScope(namespaceNode, sourceFile->forceNamespace, ScopeKind::Namespace, parentScope);
            newScope->flags |= SCOPE_IMPORTED;
            typeInfo->scope         = newScope;
            namespaceNode->typeInfo = typeInfo;
            parentScope->symTable.addSymbolTypeInfoNoLock(&context, namespaceNode, typeInfo, SymbolKind::Namespace);
            parentScope->addPublicNamespace(namespaceNode);
            parentScope = newScope;
        }
        else
        {
            parentScope = CastTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
            parentScope->flags |= SCOPE_IMPORTED;
            namespaceNode                     = CastAst<AstNameSpace>(parentScope->owner, AstNodeKind::Namespace);
            namespaceNode->importedModuleName = sourceFile->imported->name;
        }
    }

    // One private scope per file. We do NOT register the scope in the list of childs
    // of the module scope, to avoid contention in // (and this is useless). That way,
    // no need to lock the module scope each time a file is encountered.
    sourceFile->computePrivateScopeName();
    sourceFile->scopePrivate              = Ast::newScope(sourceFile->astRoot, sourceFile->scopeName, ScopeKind::File, nullptr);
    sourceFile->scopePrivate->parentScope = currentScope;
    sourceFile->scopePrivate->flags |= SCOPE_ROOT_PRIVATE | SCOPE_PRIVATE;

    // By default, everything is private if it comes from the test folder
    if (sourceFile->fromTests)
        currentScope = sourceFile->scopePrivate;
    else
        currentScope = parentScope;
    sourceFile->astRoot->ownerScope = currentScope;

    bool ok = tokenizer.getToken(token);
    while (true)
    {
        // If there's an error, then we must stop at syntax pass
        if (!ok)
        {
            sourceFile->buildPass = min(sourceFile->buildPass, BuildPass::Syntax);
            sourceFile->module->setBuildPass(sourceFile->buildPass);
            return JobResult::ReleaseJob;
        }

        if (token.id == TokenId::EndOfFile)
            break;

        // Ask for lexer only
        if (sourceFile->buildPass < BuildPass::Syntax)
            ok = tokenizer.getToken(token);
        else
            ok = doTopLevelInstruction(sourceFile->astRoot);
    }

    timer.stop();
    return JobResult::ReleaseJob;
}
