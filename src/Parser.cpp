#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "Parser.h"
#include "Diagnostic.h"
#include "Scoped.h"
#include "ErrorIds.h"
#include "JobThread.h"
#include "TypeManager.h"
#include "Timer.h"

bool Parser::error(AstNode* node, const Utf8& msg, const char* help, const char* hint)
{
    Diagnostic  diag{node, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = Diagnostic::help(help);
    if (hint)
        diag.hint = hint;
    return context->report(diag, note);
}

bool Parser::error(const Token& tk, const Utf8& msg, const char* help, const char* hint)
{
    Diagnostic  diag{sourceFile, tk, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = Diagnostic::help(help);
    if (hint)
        diag.hint = hint;
    return context->report(diag, note);
}

bool Parser::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help)
{
    Diagnostic  diag{sourceFile, startLocation, endLocation, msg.c_str()};
    Diagnostic* note = nullptr;
    if (help)
        note = Diagnostic::help(help);
    return context->report(diag, note);
}

bool Parser::invalidTokenError(InvalidTokenError kind)
{
    switch (token.id)
    {
    case TokenId::SymAmpersandAmpersand:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Fmt(Err(Syn0105), "&&", "and"));
        break;
    case TokenId::SymVerticalVertical:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Fmt(Err(Syn0105), "||", "or"));
        break;
    case TokenId::KwdElse:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Err(Syn0102));
        break;
    case TokenId::KwdElif:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return error(token, Err(Syn0101));
        break;
    case TokenId::CompilerElse:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, Err(Syn0098));
        break;
    case TokenId::CompilerElseIf:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return error(token, Err(Syn0097));
        break;

    case TokenId::SymRightParen:
        return error(token, Err(Syn0099));
    case TokenId::SymRightCurly:
        return error(token, Err(Syn0103));
    case TokenId::SymRightSquare:
        return error(token, Err(Syn0100));

    default:
        break;
    }

    Utf8 msg;
    Utf8 hint;

    switch (kind)
    {
    case InvalidTokenError::TopLevelInstruction:
        msg += Err(Syn0064);
        hint = Hnt(Hnt0014);
        break;
    case InvalidTokenError::EmbeddedInstruction:
        msg += Err(Syn0073);
        break;
    case InvalidTokenError::LeftExpression:
        msg += Err(Syn0059);
        break;
    case InvalidTokenError::LeftExpressionVar:
        msg += Err(Syn0068);
        hint = Fmt(Hnt(Hnt0129), token.ctext());
        break;
    case InvalidTokenError::PrimaryExpression:
        msg += Err(Syn0076);
        break;
    }

    if (Tokenizer::isSymbol(token.id))
        msg += Fmt(", found symbol '%s' ", token.ctext());
    else if (token.id == TokenId::Identifier)
        msg += Fmt(", found identifier '%s' ", token.ctext());
    else if (token.id == TokenId::NativeType)
        msg += Fmt(", found type '%s' ", token.ctext());
    else if (Tokenizer::isKeyword(token.id))
        msg += Fmt(", found keyword '%s' ", token.ctext());
    else
        msg += Fmt(", found '%s' ", token.ctext());

    return error(token, msg, nullptr, hint);
}

bool Parser::eatToken()
{
    SWAG_CHECK(tokenizer.nextToken(token));
    return true;
}

bool Parser::eatCloseToken(TokenId id, const SourceLocation& start, const char* msg)
{
    SWAG_ASSERT(msg);

    if (token.id != id)
    {
        Utf8 related = Tokenizer::tokenToName(id);
        if (token.id == TokenId::EndOfFile)
        {
            Diagnostic diag{sourceFile, token, Fmt(Err(Syn0047), Tokenizer::tokenToName(id).c_str(), msg)};
            auto       note             = Diagnostic::note(sourceFile, start, start, Fmt(Nte(Nte0020), related.c_str()));
            note->showMultipleCodeLines = token.startLocation.line != start.line;
            return context->report(diag, note);
        }
        else
        {
            Diagnostic diag{sourceFile, token, Fmt(Err(Syn0048), Tokenizer::tokenToName(id).c_str(), token.ctext(), msg)};
            auto       note             = Diagnostic::note(sourceFile, start, start, Fmt(Nte(Nte0020), related.c_str()));
            note->showMultipleCodeLines = token.startLocation.line != start.line;
            return context->report(diag, note);
        }
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::eatToken(TokenId id, const char* msg)
{
    SWAG_ASSERT(msg);

    if (token.id != id)
    {
        if (token.id == TokenId::EndOfFile)
            SWAG_CHECK(error(token, Fmt(Err(Syn0047), Tokenizer::tokenToName(id).c_str(), msg)));
        else
            SWAG_CHECK(error(token, Fmt(Err(Syn0048), Tokenizer::tokenToName(id).c_str(), token.ctext(), msg)));
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::eatSemiCol(const char* msg)
{
    SWAG_ASSERT(msg);

    if (token.id != TokenId::SymSemiColon && token.id != TokenId::EndOfFile && !(token.flags & TOKENPARSE_LAST_EOL))
    {
        if (token.id == TokenId::SymAsterisk)
        {
            auto st = token;
            eatToken();
            if (token.id == TokenId::SymSlash)
            {
                token.startLocation = st.startLocation;
                return error(token, Fmt(Err(Syn0188), msg), nullptr, Hnt(Hnt0118));
            }

            token = st;
        }

        if (Tokenizer::isSymbol(token.id))
            return error(token, Fmt(Err(Syn0171), token.ctext(), msg));
        return error(token, Fmt(Err(Syn0037), token.ctext(), msg), nullptr, Hnt(Hnt0013));
    }

    if (token.id == TokenId::SymSemiColon)
        SWAG_CHECK(eatToken());
    return true;
}

bool Parser::saveEmbeddedAst(const Utf8& content, AstNode* fromNode, Path& tmpFilePath, Utf8& tmpFileName, uint32_t& previousLogLine)
{
    auto modl = fromNode->sourceFile->module;

    tmpFilePath = modl->publicPath;
    tmpFileName = Fmt("%s%d.gwg", modl->name.c_str(), g_ThreadIndex);

    uint32_t countEol = 0;
    auto     size     = content.length();
    for (uint32_t i = 0; i < size; i++)
    {
        if (content[i] == '\n')
            countEol++;
    }

    Utf8 sourceCode = Fmt("// %s:%d:%d:%d:%d\n", fromNode->sourceFile->path.string().c_str(), fromNode->token.startLocation.line + 1, fromNode->token.startLocation.column + 1, fromNode->token.endLocation.line + 1, fromNode->token.endLocation.column + 1);
    modl->contentJobGeneratedFile[g_ThreadIndex] += sourceCode;
    modl->countLinesGeneratedFile[g_ThreadIndex] += 1;
    previousLogLine = modl->countLinesGeneratedFile[g_ThreadIndex];

    modl->contentJobGeneratedFile[g_ThreadIndex] += content;
    modl->countLinesGeneratedFile[g_ThreadIndex] += countEol;

    modl->contentJobGeneratedFile[g_ThreadIndex] += "\n\n";
    modl->countLinesGeneratedFile[g_ThreadIndex] += 2;

    return true;
}

bool Parser::constructEmbeddedAst(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind, bool logGenerated)
{
    Utf8     tmpFileName     = "<generated>";
    Path     tmpFilePath     = "<generated>";
    uint32_t previousLogLine = 0;

    SWAG_ASSERT(context);
    SWAG_ASSERT(module);

    // Log the generated code in '<module>.gwg'
    if (logGenerated &&
        fromNode &&
        !g_CommandLine.scriptCommand &&
        !fromNode->sourceFile->shouldHaveError &&
        !fromNode->sourceFile->shouldHaveWarning && g_CommandLine.output &&
        fromNode->sourceFile->module->buildCfg.backendDebugInformations)
    {
        SWAG_CHECK(saveEmbeddedAst(content, fromNode, tmpFilePath, tmpFileName, previousLogLine));
    }

    sourceFile = Allocator::alloc<SourceFile>();
    sourceFile->setExternalBuffer(content);
    sourceFile->module = parent->sourceFile->module;
    sourceFile->name   = tmpFileName;
    sourceFile->path   = tmpFilePath;
    sourceFile->path.append(tmpFileName.c_str());
    if (fromNode)
    {
        sourceFile->sourceNode        = fromNode;
        sourceFile->shouldHaveError   = fromNode->sourceFile->shouldHaveError;
        sourceFile->shouldHaveWarning = fromNode->sourceFile->shouldHaveWarning;
    }

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

    tokenizer.setup(context, sourceFile);
    if (logGenerated)
    {
        sourceFile->offsetGetLine = previousLogLine;
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
        case CompilerAstKind::TopLevelInstruction:
        case CompilerAstKind::MissingInterfaceMtd:
            SWAG_CHECK(doTopLevelInstruction(parent, &dummyResult));
            break;
        case CompilerAstKind::StructVarDecl:
            SWAG_CHECK(doVarDecl(parent, &dummyResult, AstNodeKind::VarDecl));
            break;
        case CompilerAstKind::EnumValue:
            SWAG_CHECK(doEnumValue(parent, &dummyResult));
            break;
        case CompilerAstKind::EmbeddedInstruction:
            SWAG_CHECK(doEmbeddedInstruction(parent, &dummyResult));
            break;
        case CompilerAstKind::Expression:
            SWAG_CHECK(doExpression(parent, EXPR_FLAG_NONE, &dummyResult));
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    return true;
}

void Parser::setup(ErrorContext* errorCxt, Module* mdl, SourceFile* file)
{
    context                 = errorCxt;
    module                  = mdl;
    sourceFile              = file;
    tokenizer.trackComments = g_CommandLine.genDoc;
}

bool Parser::generateAst()
{
    SWAG_ASSERT(context);
    SWAG_ASSERT(module && sourceFile);

    // First do the setup that does not need the source file to be loaded
#ifdef SWAG_STATS
    g_Stats.numFiles++;
#endif

    // Setup root ast for file
    sourceFile->astRoot = Ast::newNode<AstNode>(this, AstNodeKind::File, sourceFile, module->astRoot);

    currentScope     = module->scopeRoot;
    auto parentScope = module->scopeRoot;

    // Creates a top namespace with the module namespace name
    if (module->kind != ModuleKind::BootStrap && module->kind != ModuleKind::Runtime)
    {
        auto moduleForNp = sourceFile->imported ? sourceFile->imported : sourceFile->module;

        Utf8 npName;
        npName.append((const char*) moduleForNp->buildCfg.moduleNamespace.buffer, (int) moduleForNp->buildCfg.moduleNamespace.count);
        if (npName.empty())
            npName = moduleForNp->name;
        Ast::normalizeIdentifierName(npName);

        auto namespaceNode        = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, sourceFile->astRoot);
        namespaceNode->token.text = npName;

        ScopedLock lk(parentScope->symTable.mutex);
        auto       symbol = parentScope->symTable.findNoLock(npName);
        if (!symbol)
        {
            auto typeInfo           = makeType<TypeInfoNamespace>();
            typeInfo->name          = npName;
            typeInfo->scope         = Ast::newScope(namespaceNode, npName, ScopeKind::Namespace, parentScope);
            namespaceNode->typeInfo = typeInfo;
            AddSymbolTypeInfo toAdd;
            toAdd.node       = namespaceNode;
            toAdd.typeInfo   = typeInfo;
            toAdd.kind       = SymbolKind::Namespace;
            toAdd.symbolName = parentScope->symTable.registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, &npName);
            parentScope->symTable.addSymbolTypeInfoNoLock(context, toAdd);
            parentScope = typeInfo->scope;
        }
        else
        {
            parentScope = CastTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
        }

        if (sourceFile->imported)
            parentScope->flags |= SCOPE_IMPORTED;
        parentScope->flags |= SCOPE_AUTO_GENERATED;
    }

    // One scope per file. We do NOT register the scope in the list of childs
    // of the module scope, to avoid contention in // (and this is useless). That way,
    // no need to lock the module scope each time a file is encountered.
    Utf8 scopeName = "__" + Path(sourceFile->name).replace_extension().string();
    Ast::normalizeIdentifierName(scopeName);
    sourceFile->scopeFile              = Ast::newScope(sourceFile->astRoot, scopeName, ScopeKind::File, nullptr);
    sourceFile->scopeFile->parentScope = parentScope;
    sourceFile->scopeFile->flags |= SCOPE_FILE;

    // By default, everything is internal if it comes from the test folder, or from the configuration file
    if (sourceFile->fromTests || sourceFile->isCfgFile)
        currentScope = sourceFile->scopeFile;
    else
        currentScope = parentScope;
    sourceFile->astRoot->ownerScope = currentScope;

    // Make a copy of all #global using of the config file
    if (!sourceFile->isCfgFile && !sourceFile->imported && !sourceFile->isEmbedded)
    {
        for (auto s : module->buildParameters.globalUsings)
        {
            CloneContext cxt;
            cxt.parent       = sourceFile->astRoot;
            cxt.parentScope  = currentScope;
            cxt.removeFlags  = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
            auto node        = s->clone(cxt);
            node->sourceFile = sourceFile;
        }
    }
    else if (sourceFile->isEmbedded)
    {
        for (auto s : sourceFile->globalUsingsEmbedded)
        {
            CloneContext cxt;
            cxt.parent       = sourceFile->astRoot;
            cxt.parentScope  = currentScope;
            cxt.removeFlags  = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
            auto node        = s->clone(cxt);
            node->sourceFile = sourceFile;
        }
    }

#ifdef SWAG_STATS
    Timer timer(&g_Stats.syntaxTime);
#endif

    tokenizer.setup(context, sourceFile);
    SWAG_CHECK(eatToken());

    if (!tokenizer.comment.empty())
    {
        // Module global comment must be put in module.swg
        if (sourceFile->module->kind == ModuleKind::Config)
        {
            module->docComment = std::move(tokenizer.comment);
        }
    }

    while (token.id != TokenId::EndOfFile)
        SWAG_CHECK(doTopLevelInstruction(sourceFile->astRoot, &dummyResult));

    return true;
}
