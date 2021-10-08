#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scoped.h"
#include "Timer.h"
#include "Workspace.h"
#include "ErrorIds.h"
#include "LoadSourceFileJob.h"

bool SyntaxJob::verifyError(const Token& tk, bool expr, const Utf8& msg)
{
    if (!expr)
        return error(tk, msg);
    return true;
}

bool SyntaxJob::invalidTokenError(InvalidTokenError kind)
{
    switch (token.id)
    {
    case TokenId::SymAmpersandAmpersand:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Utf8::format(g_E[Err0891], "&&", "and"));
        break;
    case TokenId::SymVerticalVertical:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Utf8::format(g_E[Err0891], "||", "or"));
        break;
    case TokenId::KwdElse:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, g_E[Err0323]);
        break;
    case TokenId::CompilerElse:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, g_E[Err0324]);
        break;
    case TokenId::CompilerElseIf:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, g_E[Err0325]);
        break;

    case TokenId::SymRightParen:
        return error(token, g_E[Err0326]);
    case TokenId::SymRightCurly:
        return error(token, g_E[Err0327]);
    case TokenId::SymRightSquare:
        return error(token, g_E[Err0328]);
    }

    Utf8 msg;
    Utf8 hint;

    switch (kind)
    {
    case InvalidTokenError::TopLevelInstruction:
        msg += g_E[Err0895];
        hint = g_E[Hnt0014];
        break;
    case InvalidTokenError::EmbeddedInstruction:
        msg += g_E[Err0896];
        break;
    case InvalidTokenError::LeftExpression:
        msg += g_E[Err0897];
        break;
    case InvalidTokenError::LeftExpressionVar:
        msg += g_E[Err0898];
        break;
    case InvalidTokenError::PrimaryExpression:
        msg += g_E[Err0899];
        break;
    }

    if (Tokenizer::isSymbol(token.id))
        msg += Utf8::format(g_E[Nte0049], token.text.c_str());
    else if (token.id == TokenId::Identifier)
        msg += Utf8::format(g_E[Nte0048], token.text.c_str());
    else if (token.id == TokenId::NativeType)
        msg += Utf8::format(g_E[Nte0050], token.text.c_str());
    else
        msg += Utf8::format(g_E[Nte0047], token.text.c_str());

    switch (token.id)
    {
    case TokenId::Identifier:
        if (kind == InvalidTokenError::TopLevelInstruction)
        {
            Token nextToken;
            tokenizer.getToken(nextToken);
            if (nextToken.id == TokenId::SymEqual || nextToken.id == TokenId::SymColonEqual || nextToken.id == TokenId::SymColon)
                msg += g_E[Nte0051];
        }
        break;
    }

    PushErrHint errh(hint);
    return error(token, msg);
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
            SWAG_CHECK(error(token, Utf8::format(g_E[Err0329], g_LangSpec->tokenToName(id).c_str(), msg)));
        else
            SWAG_CHECK(error(token, Utf8::format(g_E[Err0330], g_LangSpec->tokenToName(id).c_str(), token.text.c_str(), msg)));
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool SyntaxJob::eatTokenNoEOL(TokenId id, const char* msg)
{
    auto lastToken = token;
    SWAG_CHECK(eatToken(id, msg));
    if (tokenizer.lastTokenIsEOL)
        SWAG_CHECK(error(lastToken, Utf8::format(g_E[Err0858], lastToken.text.c_str(), msg)));
    return true;
}

bool SyntaxJob::eatSemiCol(const char* msg)
{
    if (token.id != TokenId::SymSemiColon && token.id != TokenId::EndOfFile && !tokenizer.lastTokenIsEOL)
    {
        if (!msg)
            msg = "";

        PushErrHint errh(g_E[Hnt0013]);
        SWAG_CHECK(error(token, Utf8::format(g_E[Err0331], token.text.c_str(), msg)));
    }

    if (token.id == TokenId::SymSemiColon)
        SWAG_CHECK(eatToken());
    return true;
}

bool SyntaxJob::constructEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind, bool logGenerated)
{
    Utf8     tmpFileName     = "<generated>";
    Utf8     tmpFilePath     = "<generated>";
    uint32_t previousLogLine = 0;

    // Log the generated code in '<module>.swg'
    if (logGenerated && !fromNode->sourceFile->numTestErrors && !fromNode->sourceFile->numTestWarnings && g_CommandLine->output)
    {
        auto modl = fromNode->sourceFile->module;
        if (modl->buildCfg.backendDebugInformations)
        {
            auto publicPath = modl->publicPath;
            tmpFilePath     = publicPath;
            tmpFileName     = modl->name + ".gwg";

            publicPath += tmpFileName;

            uint32_t    countEol = 0;
            const char* pz       = content.c_str();
            for (int i = 0; i < content.length(); i++)
            {
                if (*pz == '\n')
                    countEol++;
                pz++;
            }

            ScopedLock lk(modl->mutexGeneratedFile);

            if (!modl->handleGeneratedFile)
            {
                if (modl->firstGenerated)
                    fopen_s(&modl->handleGeneratedFile, publicPath.c_str(), "wN");
                else
                    fopen_s(&modl->handleGeneratedFile, publicPath.c_str(), "a+N");
                if (!modl->handleGeneratedFile)
                {
                    g_Log.errorOS(Utf8::format(g_E[Err0524], publicPath.c_str()));
                    return false;
                }
            }

            modl->firstGenerated = false;

            Utf8 sourceCode = Utf8::format("// %s:%d:%d:%d:%d\n", fromNode->sourceFile->path.c_str(), fromNode->token.startLocation.line + 1, fromNode->token.startLocation.column + 1, fromNode->token.endLocation.line + 1, fromNode->token.endLocation.column + 1);
            fwrite(sourceCode.c_str(), sourceCode.length(), 1, modl->handleGeneratedFile);
            modl->countLinesGeneratedFile += 1;
            previousLogLine = modl->countLinesGeneratedFile;

            fwrite(content.c_str(), content.length(), 1, modl->handleGeneratedFile);
            modl->countLinesGeneratedFile += countEol;

            static const char* eol = "\n\n";
            fwrite(eol, 1, 2, modl->handleGeneratedFile);
            modl->countLinesGeneratedFile += 2;
        }
    }

    SourceFile* tmpFile      = g_Allocator.alloc<SourceFile>();
    tmpFile->externalContent = content;
    tmpFile->setExternalBuffer((char*) tmpFile->externalContent.c_str(), tmpFile->externalContent.length());
    tmpFile->module = parent->sourceFile->module;
    tmpFile->name   = tmpFileName;
    tmpFile->path   = tmpFilePath;
    tmpFile->path += tmpFileName;
    tmpFile->sourceNode = fromNode;
    sourceFile          = tmpFile;
    currentScope        = parent->ownerScope;
    currentStructScope  = parent->ownerStructScope;
    currentFct          = parent->ownerFct;
    currentInline       = parent->ownerInline;

    tokenizer.setFile(sourceFile);
    if (logGenerated)
    {
        tmpFile->getLineOffset    = previousLogLine;
        tokenizer.location.column = 0;
        tokenizer.location.line   = previousLogLine;
    }
    else
    {
        sourceFile->fileForSourceLocation = parent->sourceFile;
        tokenizer.location                = parent->token.startLocation;
    }

    ScopedFlags scopedFlags(this, AST_GENERATED | (parent->flags & (AST_RUN_BLOCK | AST_NO_BACKEND)));
    SWAG_CHECK(eatToken());
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

    // First do the setup that does not need to source file to be loaded
    if (!setupDone)
    {
        setupDone          = true;
        baseContext        = &context;
        context.job        = this;
        context.sourceFile = sourceFile;
        if (g_CommandLine->stats)
            g_Stats.numFiles++;

        tokenizer.setFile(sourceFile);

        module = sourceFile->module;

        // Setup root ast for file
        sourceFile->astRoot = Ast::newNode<AstNode>(this, AstNodeKind::File, sourceFile, module->astRoot);

        // Creates a top namespace with the module namespace name
        currentScope     = module->scopeRoot;
        auto parentScope = module->scopeRoot;

        Utf8 npName;
        if (sourceFile->imported)
        {
            npName.append((const char*) sourceFile->imported->buildCfg.moduleNamespace.buffer, (int) sourceFile->imported->buildCfg.moduleNamespace.count);
            if (npName.empty())
                npName = sourceFile->imported->name;
        }
        else if (module->kind != ModuleKind::BootStrap && module->kind != ModuleKind::Runtime)
        {
            npName.append((const char*) module->buildCfg.moduleNamespace.buffer, (int) module->buildCfg.moduleNamespace.count);
            if (npName.empty())
                npName = module->name;
        }

        if (!npName.empty())
        {
            module->namespaceName = npName;

            auto namespaceNode        = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, sourceFile->astRoot);
            namespaceNode->token.text = npName;

            ScopedLock lk(parentScope->symTable.mutex);
            auto       symbol = parentScope->symTable.findNoLock(npName);
            if (!symbol)
            {
                auto typeInfo  = allocType<TypeInfoNamespace>();
                typeInfo->name = npName;
                auto newScope  = Ast::newScope(namespaceNode, npName, ScopeKind::Namespace, parentScope);
                if (sourceFile->imported)
                    newScope->flags |= SCOPE_IMPORTED;
                newScope->flags |= SCOPE_AUTO_GENERATED;
                typeInfo->scope         = newScope;
                namespaceNode->typeInfo = typeInfo;
                parentScope->symTable.addSymbolTypeInfoNoLock(&context, namespaceNode, typeInfo, SymbolKind::Namespace);
                parentScope = newScope;
            }
            else
            {
                parentScope = CastTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
                if (sourceFile->imported)
                    parentScope->flags |= SCOPE_IMPORTED;
                parentScope->flags |= SCOPE_AUTO_GENERATED;
            }
        }

        // One private scope per file. We do NOT register the scope in the list of childs
        // of the module scope, to avoid contention in // (and this is useless). That way,
        // no need to lock the module scope each time a file is encountered.
        sourceFile->computePrivateScopeName();
        sourceFile->scopePrivate              = Ast::newScope(sourceFile->astRoot, sourceFile->scopeName, ScopeKind::File, nullptr);
        sourceFile->scopePrivate->parentScope = parentScope;
        sourceFile->scopePrivate->flags |= SCOPE_ROOT_PRIVATE | SCOPE_PRIVATE;

        // By default, everything is private if it comes from the test folder, or for the configuration file
        if (sourceFile->fromTests || sourceFile->isCfgFile)
            currentScope = sourceFile->scopePrivate;
        else
            currentScope = parentScope;
        sourceFile->astRoot->ownerScope = currentScope;
    }

    // Error reading file ?
    if (sourceFile->numErrors)
        return JobResult::ReleaseJob;

    // Then load the file
    if (!sourceFile->buffer)
    {
        auto loadJob        = g_Allocator.alloc<LoadSourceFileJob>();
        loadJob->sourceFile = sourceFile;
        loadJob->addDependentJob(this);
        jobsToAdd.push_back(loadJob);
        return JobResult::KeepJobAlive;
    }

    bool ok = eatToken();
    while (true)
    {
        // If there's an error, then we must stop at syntax pass
        if (!ok)
        {
            sourceFile->buildPass = min(sourceFile->buildPass, BuildPass::Syntax);
            module->setBuildPass(sourceFile->buildPass);
            return JobResult::ReleaseJob;
        }

        if (token.id == TokenId::EndOfFile)
            break;

        // Ask for lexer only
        if (sourceFile->buildPass < BuildPass::Syntax)
            ok = eatToken();
        else
            ok = doTopLevelInstruction(sourceFile->astRoot);
    }

    return JobResult::ReleaseJob;
}
