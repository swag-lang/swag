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
#include "JobThread.h"

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
            return error(token, Fmt(Err(Err0891), "&&", "and"));
        break;
    case TokenId::SymVerticalVertical:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Fmt(Err(Err0891), "||", "or"));
        break;
    case TokenId::KwdElse:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Err(Err0323));
        break;
    case TokenId::KwdElif:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Err(Err0351));
        break;
    case TokenId::CompilerElse:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, Err(Err0324));
        break;
    case TokenId::CompilerElseIf:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, Err(Err0325));
        break;

    case TokenId::SymRightParen:
        return error(token, Err(Err0326));
    case TokenId::SymRightCurly:
        return error(token, Err(Err0327));
    case TokenId::SymRightSquare:
        return error(token, Err(Err0328));
    }

    Utf8 msg;
    Utf8 hint;

    switch (kind)
    {
    case InvalidTokenError::TopLevelInstruction:
        msg += Err(Err0895);
        hint = Hnt(Hnt0014);
        break;
    case InvalidTokenError::EmbeddedInstruction:
        msg += Err(Err0896);
        break;
    case InvalidTokenError::LeftExpression:
        msg += Err(Err0897);
        break;
    case InvalidTokenError::LeftExpressionVar:
        msg += Err(Err0898);
        break;
    case InvalidTokenError::PrimaryExpression:
        msg += Err(Err0899);
        break;
    }

    if (Tokenizer::isSymbol(token.id))
        msg += Fmt(", found symbol `%s` ", token.ctext());
    else if (token.id == TokenId::Identifier)
        msg += Fmt(", found identifier `%s` ", token.ctext());
    else if (token.id == TokenId::NativeType)
        msg += Fmt(", found type `%s` ", token.ctext());
    else
        msg += Fmt(", found `%s` ", token.ctext());

    switch (token.id)
    {
    case TokenId::Identifier:
        if (kind == InvalidTokenError::TopLevelInstruction)
        {
            Token nextToken;
            tokenizer.getToken(nextToken);
            if (nextToken.id == TokenId::SymEqual || nextToken.id == TokenId::SymColonEqual || nextToken.id == TokenId::SymColon)
                msg += ") did you miss `var` or `const` to declare a global variable ?";
        }
        break;
    }

    PushErrHint errh(hint);
    return error(token, msg);
}

bool SyntaxJob::error(AstNode* node, const Utf8& msg, const char* help)
{
    Diagnostic  diag{node, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = new Diagnostic{help, DiagnosticLevel::Help};
    sourceFile->report(diag, note);
    return false;
}

bool SyntaxJob::error(const Token& tk, const Utf8& msg, const char* help, const char* hint)
{
    Diagnostic  diag{sourceFile, tk, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = new Diagnostic{help, DiagnosticLevel::Help};
    if (hint)
        diag.hint = hint;
    sourceFile->report(diag, note);
    return false;
}

bool SyntaxJob::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help)
{
    Diagnostic  diag{sourceFile, startLocation, endLocation, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = new Diagnostic{help, DiagnosticLevel::Help};
    sourceFile->report(diag, note);
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
            SWAG_CHECK(error(token, Fmt(Err(Err0329), g_LangSpec->tokenToName(id).c_str(), msg)));
        else
            SWAG_CHECK(error(token, Fmt(Err(Err0330), g_LangSpec->tokenToName(id).c_str(), token.ctext(), msg)));
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool SyntaxJob::eatTokenNoEOL(TokenId id, const char* msg)
{
    auto lastToken = token;
    SWAG_CHECK(eatToken(id, msg));
    if (token.lastTokenIsEOL)
        SWAG_CHECK(error(lastToken, Fmt(Err(Err0858), lastToken.ctext(), msg)));
    return true;
}

bool SyntaxJob::eatSemiCol(const char* msg)
{
    if (token.id != TokenId::SymSemiColon && token.id != TokenId::EndOfFile && !token.lastTokenIsEOL)
    {
        if (!msg)
            msg = "";

        if (Tokenizer::isSymbol(token.id))
        {
            SWAG_CHECK(error(token, Fmt(Err(Err0837), token.ctext(), msg)));
        }
        else
        {
            PushErrHint errh(Hnt(Hnt0013));
            SWAG_CHECK(error(token, Fmt(Err(Err0331), token.ctext(), msg)));
        }
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
    SWAG_ASSERT(module);

    // Log the generated code in '<module>.swg'
    if (logGenerated && !fromNode->sourceFile->numTestErrors && !fromNode->sourceFile->numTestWarnings && g_CommandLine->output)
    {
        auto modl = fromNode->sourceFile->module;
        if (modl->buildCfg.backendDebugInformations && !g_CommandLine->scriptCommand)
        {
            auto publicPath = modl->publicPath;
            tmpFilePath     = publicPath;
            tmpFileName     = Fmt("%s%d.gwg", modl->name.c_str(), g_ThreadIndex);

            publicPath += tmpFileName;

            uint32_t    countEol = 0;
            const char* pz       = content.c_str();
            for (int i = 0; i < content.length(); i++)
            {
                if (*pz == '\n')
                    countEol++;
                pz++;
            }

            FILE* h = modl->handleGeneratedFile[g_ThreadIndex];

            if (!h)
            {
                if (modl->appendGeneratedFile[g_ThreadIndex])
                    fopen_s(&h, publicPath.c_str(), "a+N");
                else
                    fopen_s(&h, publicPath.c_str(), "wN");
                modl->handleGeneratedFile[g_ThreadIndex] = h;

                if (!h)
                {
                    modl->numErrors++;
                    g_Log.errorOS(Fmt(Err(Err0524), publicPath.c_str()));
                    return false;
                }
            }

            modl->appendGeneratedFile[g_ThreadIndex] = true;

            Utf8 sourceCode = Fmt("// %s:%d:%d:%d:%d\n", fromNode->sourceFile->path.c_str(), fromNode->token.startLocation.line + 1, fromNode->token.startLocation.column + 1, fromNode->token.endLocation.line + 1, fromNode->token.endLocation.column + 1);
            fwrite(sourceCode.c_str(), sourceCode.length(), 1, h);
            modl->countLinesGeneratedFile[g_ThreadIndex] += 1;
            previousLogLine = modl->countLinesGeneratedFile[g_ThreadIndex];

            fwrite(content.c_str(), content.length(), 1, h);
            modl->countLinesGeneratedFile[g_ThreadIndex] += countEol;

            static const char* eol = "\n\n";
            fwrite(eol, 1, 2, h);
            modl->countLinesGeneratedFile[g_ThreadIndex] += 2;
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
    tmpFile->numTestErrors.store(fromNode->sourceFile->numTestErrors);
    tmpFile->numTestWarnings.store(fromNode->sourceFile->numTestWarnings);
    sourceFile         = tmpFile;
    currentScope       = parent->ownerScope;
    currentStructScope = parent->ownerStructScope;
    currentFct         = parent->ownerFct;
    currentInline      = parent->ownerInline;

    if (kind == CompilerAstKind::MissingInterfaceMtd)
    {
        auto impl          = CastAst<AstImpl>(parent, AstNodeKind::Impl);
        currentScope       = impl->scope;
        currentStructScope = impl->structScope;
    }

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

    auto sflags = parent->flags & (AST_RUN_BLOCK | AST_NO_BACKEND);
    sflags |= AST_GENERATED;
    if (logGenerated)
        sflags |= AST_GENERATED_USER;
    ScopedFlags scopedFlags(this, sflags);
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
        case CompilerAstKind::MissingInterfaceMtd:
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
    // First do the setup that does not need the source file to be loaded
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

        // One scope per file. We do NOT register the scope in the list of childs
        // of the module scope, to avoid contention in // (and this is useless). That way,
        // no need to lock the module scope each time a file is encountered.
        sourceFile->computeFileScopeName();
        sourceFile->scopeFile              = Ast::newScope(sourceFile->astRoot, sourceFile->scopeName, ScopeKind::File, nullptr);
        sourceFile->scopeFile->parentScope = parentScope;
        sourceFile->scopeFile->flags |= SCOPE_FILE;

        // By default, everything is private if it comes from the test folder, or from the configuration file
        if (sourceFile->fromTests || sourceFile->isCfgFile)
            currentScope = sourceFile->scopeFile;
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

    sourceFile->duringSyntax = true;

    Timer timer(&g_Stats.syntaxTime);
    bool  ok = eatToken();
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

    sourceFile->duringSyntax = false;
    return JobResult::ReleaseJob;
}
