#include "pch.h"
#include "Syntax/Parser/Parser.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser_Push.h"
#include "Threading/JobThread.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#endif

bool Parser::error(AstNode* node, const Utf8& msg, const char* help, const char* hint) const
{
    Diagnostic err{node, msg};
    if (hint)
        err.hint = hint;
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::error(const Token& tk, const Utf8& msg, const char* help, const char* hint) const
{
    Diagnostic err{sourceFile, tk, msg};
    if (hint)
        err.hint = hint;
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help) const
{
    Diagnostic err{sourceFile, startLocation, endLocation, msg};
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::invalidTokenError(InvalidTokenError kind, const AstNode* parent)
{
    switch (tokenParse.token.id)
    {
        case TokenId::SymAmpersandAmpersand:
            if (kind == InvalidTokenError::EmbeddedInstruction)
                return error(tokenParse.token, formErr(Err0323, "and", "&&"));
            break;
        case TokenId::SymVerticalVertical:
            if (kind == InvalidTokenError::EmbeddedInstruction)
                return error(tokenParse.token, formErr(Err0323, "or", "||"));
            break;
        case TokenId::KwdElse:
            if (kind == InvalidTokenError::EmbeddedInstruction)
                return error(tokenParse.token, toErr(Err0665));
            break;
        case TokenId::KwdElif:
            if (kind == InvalidTokenError::EmbeddedInstruction)
                return error(tokenParse.token, toErr(Err0664));
            break;
        case TokenId::CompilerElse:
            if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
                return error(tokenParse.token, toErr(Err0657));
            break;
        case TokenId::CompilerElseIf:
            if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
                return error(tokenParse.token, toErr(Err0656));
            break;
        case TokenId::SymRightParen:
            if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
                return error(tokenParse.token, toErr(Err0660));
            break;
        case TokenId::SymRightCurly:
            if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
                return error(tokenParse.token, toErr(Err0673));
            break;
        case TokenId::SymRightSquare:
            if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
                return error(tokenParse.token, toErr(Err0661));
            break;
    }

    Utf8 msg;
    Utf8 note;

    switch (kind)
    {
        case InvalidTokenError::TopLevelInstruction:
            msg  = formErr(Err0381, tokenParse.token.c_str());
            note = toNte(Nte0167);
            break;
        case InvalidTokenError::EmbeddedInstruction:
            msg = formErr(Err0262, tokenParse.token.c_str());
            break;
        case InvalidTokenError::LeftExpression:
            msg = formErr(Err0283, tokenParse.token.c_str());
            break;
        case InvalidTokenError::PrimaryExpression:

            // Bad character syntax as an expression
            if (tokenParse.is(TokenId::SymQuote))
            {
                const auto startToken = tokenParse;
                eatToken();
                const auto inToken = tokenParse;
                eatToken();
                if (tokenParse.is(TokenId::SymQuote))
                {
                    const Diagnostic err{sourceFile, startToken.token.startLocation, tokenParse.token.endLocation, formErr(Err0237, inToken.token.c_str())};
                    return context->report(err);
                }

                tokenParse = startToken;
            }

            msg = formErr(Err0283, tokenParse.token.c_str());
            if (parent)
            {
                if (Tokenizer::isKeyword(parent->token.id))
                {
                    const Utf8 forWhat = form("[[%s]]", parent->token.c_str());
                    msg                = formErr(Err0281, forWhat.c_str(), tokenParse.token.c_str());
                }
                else if (Tokenizer::isCompiler(parent->token.id))
                {
                    const Utf8 forWhat = form("the compiler directive [[%s]]", parent->token.c_str());
                    msg                = formErr(Err0281, forWhat.c_str(), tokenParse.token.c_str());
                }
                else if (Tokenizer::isSymbol(parent->token.id))
                {
                    const Utf8 forWhat = form("the symbol [[%s]]", parent->token.c_str());
                    msg                = formErr(Err0281, forWhat.c_str(), tokenParse.token.c_str());
                }
            }

            break;
    }

    return error(tokenParse.token, msg, note.empty() ? nullptr : note.c_str());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    Diagnostic err{sourceFile, myToken.token, msg ? msg : formErr(Err0310, myToken.token.c_str()).c_str()};
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(formNte(Nte0125, myToken.token.c_str()));
    return context->report(err);
}

bool Parser::eatToken()
{
    prevTokenParse = tokenParse;
    SWAG_CHECK(tokenizer.nextToken(tokenParse));
    return true;
}

bool Parser::eatCloseToken(TokenId id, const SourceLocation& start, const char* msg)
{
    SWAG_ASSERT(msg);

    if (tokenParse.token.isNot(id))
    {
        const Utf8 related = Naming::tokenToName(id);
        const auto diagMsg = formErr(Err0545, Naming::tokenToName(id).c_str(), Naming::tokenToName(id).c_str(), msg, tokenParse.token.c_str());

        if (tokenParse.is(TokenId::EndOfFile))
        {
            const Diagnostic err{sourceFile, start, start, diagMsg};
            return context->report(err);
        }

        Diagnostic err{sourceFile, tokenParse.token, diagMsg};
        err.addNote(sourceFile, start, start, formNte(Nte0180, related.c_str()));
        return context->report(err);
    }

    SWAG_CHECK(eatToken());
    return true;
}

void Parser::prepareExpectTokenError()
{
    // If we expect a token, and the bad token is the first of the line, then
    // it's better to display the requested token at the end of the previous line
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        tokenParse.token.startLocation = prevTokenParse.token.endLocation;
        tokenParse.token.endLocation   = tokenParse.token.startLocation;
    }
}

bool Parser::eatTokenError(TokenId id, const Utf8& msg)
{
    if (tokenParse.token.isNot(id))
    {
        prepareExpectTokenError();
        const Diagnostic err{sourceFile, tokenParse.token, form(msg.c_str(), tokenParse.token.c_str())};
        return context->report(err);
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::eatToken(TokenId id, const char* msg)
{
    SWAG_ASSERT(msg);
    if (tokenParse.token.isNot(id))
    {
        prepareExpectTokenError();
        const Diagnostic err{sourceFile, tokenParse.token, formErr(Err0083, Naming::tokenToName(id).c_str(), Naming::tokenToName(id).c_str(), msg, tokenParse.token.c_str())};
        return context->report(err);
    }

    SWAG_CHECK(eatToken());
    return true;
}

bool Parser::eatSemiCol(const char* msg)
{
    SWAG_ASSERT(msg);

    if (tokenParse.isNot(TokenId::SymSemiColon) && tokenParse.isNot(TokenId::EndOfFile) && !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        if (tokenParse.is(TokenId::SymAsterisk))
        {
            const auto st = tokenParse;
            eatToken();
            if (tokenParse.is(TokenId::SymSlash))
            {
                tokenParse.token.startLocation = st.token.startLocation;
                return error(tokenParse.token, formErr(Err0680, msg));
            }

            tokenParse = st;
        }

        return error(tokenParse.token, formErr(Err0550, msg, tokenParse.token.c_str()));
    }

    if (tokenParse.is(TokenId::SymSemiColon))
        SWAG_CHECK(eatToken());
    return true;
}

bool Parser::saveEmbeddedAst(const Utf8& content, const AstNode* fromNode, Path& tmpFilePath, Utf8& tmpFileName, uint32_t& previousLogLine)
{
    const auto modl = fromNode->token.sourceFile->module;

    tmpFilePath = modl->publicPath;
    tmpFileName = form("%s%d.gwg", modl->name.c_str(), g_ThreadIndex);

    uint32_t   countEol = 0;
    const auto size     = content.length();
    for (uint32_t i = 0; i < size; i++)
    {
        if (content[i] == '\n')
            countEol++;
    }

    const Utf8 sourceCode = form("// %s:%d:%d:%d:%d\n",
                                 fromNode->token.sourceFile->path.c_str(),
                                 fromNode->token.startLocation.line + 1,
                                 fromNode->token.startLocation.column + 1,
                                 fromNode->token.endLocation.line + 1,
                                 fromNode->token.endLocation.column + 1);
    modl->contentJobGeneratedFile[g_ThreadIndex] += sourceCode;
    modl->countLinesGeneratedFile[g_ThreadIndex] += 1;
    previousLogLine = modl->countLinesGeneratedFile[g_ThreadIndex];

    modl->contentJobGeneratedFile[g_ThreadIndex] += content;
    modl->countLinesGeneratedFile[g_ThreadIndex] += countEol;

    modl->contentJobGeneratedFile[g_ThreadIndex] += "\n\n";
    modl->countLinesGeneratedFile[g_ThreadIndex] += 2;

    return true;
}

bool Parser::constructEmbeddedAst(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind, bool logGenerated, AstNode** result)
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
        g_CommandLine.output &&
        !fromNode->token.sourceFile->hasFlag(FILE_SHOULD_HAVE_ERROR) &&
        !fromNode->token.sourceFile->hasFlag(FILE_SHOULD_HAVE_WARNING) &&
        fromNode->token.sourceFile->module->buildCfg.backendDebugInfos)
    {
        SWAG_CHECK(saveEmbeddedAst(content, fromNode, tmpFilePath, tmpFileName, previousLogLine));
    }

    sourceFile = Allocator::alloc<SourceFile>();
    sourceFile->setExternalBuffer(content);
    sourceFile->addFlag(FILE_FROM_AST);
    sourceFile->module = parent->token.sourceFile->module;
    sourceFile->name   = tmpFileName;
    sourceFile->path   = tmpFilePath;
    sourceFile->path.append(tmpFileName);
    if (fromNode)
    {
        sourceFile->fromNode = fromNode;
        sourceFile->addFlag(fromNode->token.sourceFile->flags.mask(FILE_SHOULD_HAVE_ERROR));
        sourceFile->addFlag(fromNode->token.sourceFile->flags.mask(FILE_SHOULD_HAVE_WARNING));
    }

    currentScope       = parent->ownerScope;
    currentStructScope = parent->ownerStructScope;
    currentFct         = parent->ownerFct;
    currentInline      = parent->safeOwnerInline();

    if (kind == CompilerAstKind::MissingInterfaceMtd)
    {
        const auto impl    = castAst<AstImpl>(parent, AstNodeKind::Impl);
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
        sourceFile->fileForSourceLocation = parent->token.sourceFile;
        tokenizer.location                = parent->token.startLocation;
    }

    auto scopeFlags = parent->flags.mask(AST_IN_RUN_BLOCK | AST_NO_BACKEND);
    scopeFlags.add(AST_GENERATED);
    if (logGenerated)
        scopeFlags.add(AST_GENERATED_USER);

    ParserPushAstNodeFlags scopedFlags(this, scopeFlags);
    SWAG_CHECK(eatToken());

    if (!result)
        result = &dummyResult;

    while (true)
    {
        if (tokenParse.is(TokenId::EndOfFile))
            break;
        switch (kind)
        {
            case CompilerAstKind::TopLevelInstruction:
            case CompilerAstKind::MissingInterfaceMtd:
                SWAG_CHECK(doTopLevelInstruction(parent, result));
                break;
            case CompilerAstKind::StructVarDecl:
                SWAG_CHECK(doVarDecl(parent, result, AstNodeKind::VarDecl));
                break;
            case CompilerAstKind::EnumValue:
                SWAG_CHECK(doEnumValue(parent, result));
                break;
            case CompilerAstKind::EmbeddedInstruction:
                SWAG_CHECK(doEmbeddedInstruction(parent, result));
                break;
            case CompilerAstKind::Expression:
                SWAG_CHECK(doExpression(parent, EXPR_FLAG_NONE, result));
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }
    }

    return true;
}

void Parser::setup(ErrorContext* errorCxt, Module* mdl, SourceFile* file, ParserFlags flags)
{
    context    = errorCxt;
    module     = mdl;
    sourceFile = file;

    parserFlags = flags;
    if (parserFlags.has(PARSER_TRACK_FORMAT | PARSER_TRACK_DOCUMENTATION))
        tokenizer.tokenizeFlags.add(TOKENIZER_TRACK_COMMENTS);
}

bool Parser::generateAst()
{
    SWAG_ASSERT(context);
    SWAG_ASSERT(module && sourceFile);

    // First do the setup that does not need the source file to be loaded
#ifdef SWAG_STATS
    ++g_Stats.numFiles;
#endif

    // Setup root AST for file
    tokenParse.token.sourceFile = sourceFile;
    sourceFile->astRoot         = Ast::newNode<AstFile>(AstNodeKind::File, this, module->astRoot);

    currentScope     = module->scopeRoot;
    auto parentScope = module->scopeRoot;

    // Creates a top namespace with the module namespace name
    if (parentScope && module->isNot(ModuleKind::BootStrap) && module->isNot(ModuleKind::Runtime))
    {
        const auto moduleForNp = sourceFile->imported ? sourceFile->imported : sourceFile->module;

        Utf8 npName;
        npName.append(static_cast<const char*>(moduleForNp->buildCfg.moduleNamespace.buffer), static_cast<uint32_t>(moduleForNp->buildCfg.moduleNamespace.count));
        if (npName.empty())
            npName = moduleForNp->name;
        Ast::normalizeIdentifierName(npName);

        const auto namespaceNode = Ast::newNode<AstNameSpace>(AstNodeKind::Namespace, this, sourceFile->astRoot);
        namespaceNode->addSpecFlag(AstNameSpace::SPEC_FLAG_GENERATED_TOP_LEVEL);
        namespaceNode->token.text = npName;

        ScopedLock lk(parentScope->symTable.mutex);
        const auto symbol = parentScope->symTable.findNoLock(npName);
        if (!symbol)
        {
            const auto typeInfo     = makeType<TypeInfoNamespace>();
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
            parentScope = castTypeInfo<TypeInfoNamespace>(symbol->overloads[0]->typeInfo, TypeInfoKind::Namespace)->scope;
        }

        if (sourceFile->imported)
            parentScope->flags.add(SCOPE_IMPORTED);
        parentScope->flags.add(SCOPE_AUTO_GENERATED);
    }

    // One scope per file. We do NOT register the scope in the list of children
    // of the module scope, to avoid contention in // (and this is useless). That way,
    // no need to lock the module scope each time a file is encountered.
    const Utf8 scopeName = "__" + Path(sourceFile->name).replace_extension();
    Ast::normalizeIdentifierName(scopeName);
    sourceFile->scopeFile              = Ast::newScope(sourceFile->astRoot, scopeName, ScopeKind::File, nullptr);
    sourceFile->scopeFile->parentScope = parentScope;
    sourceFile->scopeFile->flags.add(SCOPE_FILE);

    // By default, everything is internal if it comes from the test folder, or from the configuration file
    if (sourceFile->hasFlag(FILE_FROM_TESTS) || sourceFile->hasFlag(FILE_CFG))
        currentScope = sourceFile->scopeFile;
    else
        currentScope = parentScope;
    sourceFile->astRoot->ownerScope = currentScope;

    // Make a copy of all #global using of the config file
    if (!sourceFile->hasFlag(FILE_CFG) && !sourceFile->imported && !sourceFile->hasFlag(FILE_EMBEDDED))
    {
        for (const auto s : module->buildParameters.globalUsing)
        {
            CloneContext cxt;
            cxt.parent             = sourceFile->astRoot;
            cxt.parentScope        = currentScope;
            cxt.removeFlags        = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
            const auto node        = s->clone(cxt);
            node->token.sourceFile = sourceFile;
        }
    }
    else if (sourceFile->hasFlag(FILE_EMBEDDED))
    {
        for (const auto s : sourceFile->globalUsingEmbedded)
        {
            CloneContext cxt;
            cxt.parent             = sourceFile->astRoot;
            cxt.parentScope        = currentScope;
            cxt.removeFlags        = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
            const auto node        = s->clone(cxt);
            node->token.sourceFile = sourceFile;
        }
    }

#ifdef SWAG_STATS
    Timer timer(&g_Stats.syntaxTime);
#endif

    tokenizer.setup(context, sourceFile);
    SWAG_CHECK(eatToken());

    // Module global comment must be put in module.swg
    if (!tokenParse.comments.justBefore.empty() && sourceFile->module->is(ModuleKind::Config))
        module->docComment = TokenComments::toString(tokenParse.comments.justBefore);

    // Parse !!!
    while (tokenParse.isNot(TokenId::EndOfFile))
        SWAG_CHECK(doTopLevelInstruction(sourceFile->astRoot, &dummyResult));

    return true;
}
