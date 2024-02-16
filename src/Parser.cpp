#include "pch.h"
#include "Parser.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "JobThread.h"
#include "Module.h"
#include "Naming.h"
#include "Scoped.h"
#include "TypeManager.h"
#ifdef SWAG_STATS
#include "Timer.h"
#endif

bool Parser::error(AstNode* node, const Utf8& msg, const char* help, const char* hint) const
{
	Diagnostic        diag{node, msg.c_str()};
	const Diagnostic* note = nullptr;
	if (help)
		note = Diagnostic::note(help);
	if (hint)
		diag.hint = hint;
	return context->report(diag, note);
}

bool Parser::error(const Token& tk, const Utf8& msg, const char* help, const char* hint) const
{
	Diagnostic        diag{sourceFile, tk, msg.c_str()};
	const Diagnostic* note = nullptr;
	if (help)
		note = Diagnostic::note(help);
	if (hint)
		diag.hint = hint;
	return context->report(diag, note);
}

bool Parser::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help) const
{
	const Diagnostic  diag{sourceFile, startLocation, endLocation, msg.c_str()};
	const Diagnostic* note = nullptr;
	if (help)
		note = Diagnostic::note(help);
	return context->report(diag, note);
}

bool Parser::invalidTokenError(InvalidTokenError kind, const AstNode* parent)
{
	switch (token.id)
	{
	case TokenId::SymAmpersandAmpersand:
		if (kind == InvalidTokenError::EmbeddedInstruction)
			return error(token, FMT(Err(Err0323), "and", "&&"));
		break;
	case TokenId::SymVerticalVertical:
		if (kind == InvalidTokenError::EmbeddedInstruction)
			return error(token, FMT(Err(Err0323), "or", "||"));
		break;
	case TokenId::KwdElse:
		if (kind == InvalidTokenError::EmbeddedInstruction)
			return error(token, Err(Err0665));
		break;
	case TokenId::KwdElif:
		if (kind == InvalidTokenError::EmbeddedInstruction)
			return error(token, Err(Err0664));
		break;
	case TokenId::CompilerElse:
		if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
			return error(token, Err(Err0657));
		break;
	case TokenId::CompilerElseIf:
		if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
			return error(token, Err(Err0656));
		break;
	case TokenId::SymRightParen:
		if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
			return error(token, Err(Err0660));
		break;
	case TokenId::SymRightCurly:
		if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
			return error(token, Err(Err0673));
		break;
	case TokenId::SymRightSquare:
		if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
			return error(token, Err(Err0661));
		break;

	default:
		break;
	}

	Utf8 msg;
	Utf8 note;

	switch (kind)
	{
	case InvalidTokenError::TopLevelInstruction:
		msg = FMT(Err(Err0381), token.c_str());
		note = Nte(Nte0167);
		break;
	case InvalidTokenError::EmbeddedInstruction:
		msg = FMT(Err(Err0262), token.c_str());
		break;
	case InvalidTokenError::LeftExpression:
		msg = FMT(Err(Err0283), token.c_str());
		break;
	case InvalidTokenError::PrimaryExpression:

		// Bad character syntax as an expression
		if (token.id == TokenId::SymQuote)
		{
			const auto startToken = token;
			eatToken();
			const auto inToken = token;
			eatToken();
			if (token.id == TokenId::SymQuote)
			{
				const Diagnostic diag{sourceFile, startToken.startLocation, token.endLocation, FMT(Err(Err0237), inToken.c_str())};
				return context->report(diag);
			}

			token = startToken;
		}

		msg = FMT(Err(Err0283), token.c_str());
		if (parent)
		{
			if (Tokenizer::isKeyword(parent->tokenId))
			{
				const Utf8 forWhat = FMT("[[%s]]", parent->token.c_str());
				msg                = FMT(Err(Err0281), forWhat.c_str(), token.c_str());
			}
			else if (Tokenizer::isCompiler(parent->tokenId))
			{
				const Utf8 forWhat = FMT("the compiler directive [[%s]]", parent->token.c_str());
				msg                = FMT(Err(Err0281), forWhat.c_str(), token.c_str());
			}
			else if (Tokenizer::isSymbol(parent->tokenId))
			{
				const Utf8 forWhat = FMT("the symbol [[%s]]", parent->token.c_str());
				msg                = FMT(Err(Err0281), forWhat.c_str(), token.c_str());
			}
		}

		break;
	}

	return error(token, msg, note.empty() ? nullptr : note.c_str());
}

bool Parser::invalidIdentifierError(const TokenParse& tokenParse, const char* msg) const
{
	const Diagnostic* note = nullptr;

	if (Tokenizer::isKeyword(tokenParse.id))
		note = Diagnostic::note(FMT(Nte(Nte0125), tokenParse.c_str()));

	const Diagnostic diag{sourceFile, token, msg ? msg : FMT(Err(Err0310), token.c_str()).c_str()};
	return context->report(diag, note);
}

bool Parser::eatToken()
{
	prevToken = token;
	SWAG_CHECK(tokenizer.nextToken(token));
	return true;
}

bool Parser::eatCloseToken(TokenId id, const SourceLocation& start, const char* msg)
{
	SWAG_ASSERT(msg);

	if (token.id != id)
	{
		const Utf8 related = Naming::tokenToName(id);
		const auto diagMsg = FMT(Err(Err0545), Naming::tokenToName(id).c_str(), Naming::tokenToName(id).c_str(), msg, token.c_str());

		if (token.id == TokenId::EndOfFile)
		{
			const Diagnostic diag{sourceFile, start, start, diagMsg};
			return context->report(diag);
		}
		const Diagnostic diag{sourceFile, token, diagMsg};
		const auto       note = Diagnostic::note(sourceFile, start, start, FMT(Nte(Nte0180), related.c_str()));
		return context->report(diag, note);
	}

	SWAG_CHECK(eatToken());
	return true;
}

void Parser::prepareExpectTokenError()
{
	// If we expect a token, and the bad token is the first of the line, then
	// it's better to display the requested token at the end of the previous line
	if (token.flags & TOKENPARSE_LAST_EOL)
	{
		token.startLocation = prevToken.endLocation;
		token.endLocation   = token.startLocation;
	}
}

bool Parser::eatTokenError(TokenId id, const Utf8& err)
{
	if (token.id != id)
	{
		prepareExpectTokenError();
		const Diagnostic diag{sourceFile, token, FMT(err.c_str(), token.c_str())};
		return context->report(diag);
	}

	SWAG_CHECK(eatToken());
	return true;
}

bool Parser::eatToken(TokenId id, const char* msg)
{
	SWAG_ASSERT(msg);
	if (token.id != id)
	{
		prepareExpectTokenError();
		const Diagnostic diag{sourceFile, token, FMT(Err(Err0083), Naming::tokenToName(id).c_str(), Naming::tokenToName(id).c_str(), msg, token.c_str())};
		return context->report(diag);
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
			const auto st = token;
			eatToken();
			if (token.id == TokenId::SymSlash)
			{
				token.startLocation = st.startLocation;
				return error(token, FMT(Err(Err0680), msg));
			}

			token = st;
		}

		return error(token, FMT(Err(Err0550), msg, token.c_str()));
	}

	if (token.id == TokenId::SymSemiColon)
		SWAG_CHECK(eatToken());
	return true;
}

bool Parser::saveEmbeddedAst(const Utf8& content, const AstNode* fromNode, Path& tmpFilePath, Utf8& tmpFileName, uint32_t& previousLogLine)
{
	const auto modl = fromNode->sourceFile->module;

	tmpFilePath = modl->publicPath;
	tmpFileName = FMT("%s%d.gwg", modl->name.c_str(), g_ThreadIndex);

	uint32_t   countEol = 0;
	const auto size     = content.length();
	for (uint32_t i = 0; i < size; i++)
	{
		if (content[i] == '\n')
			countEol++;
	}

	const Utf8 sourceCode = FMT("// %s:%d:%d:%d:%d\n", fromNode->sourceFile->path.string().c_str(), fromNode->token.startLocation.line + 1,
	                            fromNode->token.startLocation.column + 1, fromNode->token.endLocation.line + 1, fromNode->token.endLocation.column + 1);
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
		!fromNode->sourceFile->shouldHaveError &&
		!fromNode->sourceFile->shouldHaveWarning && g_CommandLine.output &&
		fromNode->sourceFile->module->buildCfg.backendDebugInfos)
	{
		SWAG_CHECK(saveEmbeddedAst(content, fromNode, tmpFilePath, tmpFileName, previousLogLine));
	}

	sourceFile = Allocator::alloc<SourceFile>();
	sourceFile->setExternalBuffer(content);
	sourceFile->isFromAst = true;
	sourceFile->module    = parent->sourceFile->module;
	sourceFile->name      = tmpFileName;
	sourceFile->path      = tmpFilePath;
	sourceFile->path.append(tmpFileName);
	if (fromNode)
	{
		sourceFile->fromNode          = fromNode;
		sourceFile->shouldHaveError   = fromNode->sourceFile->shouldHaveError;
		sourceFile->shouldHaveWarning = fromNode->sourceFile->shouldHaveWarning;
	}

	currentScope       = parent->ownerScope;
	currentStructScope = parent->ownerStructScope;
	currentFct         = parent->ownerFct;
	currentInline      = parent->ownerInline;

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
		sourceFile->fileForSourceLocation = parent->sourceFile;
		tokenizer.location                = parent->token.startLocation;
	}

	auto sflags = parent->flags & (AST_IN_RUN_BLOCK | AST_NO_BACKEND);
	sflags |= AST_GENERATED;
	if (logGenerated)
		sflags |= AST_GENERATED_USER;

	ScopedFlags scopedFlags(this, sflags);
	SWAG_CHECK(eatToken());

	if (!result)
		result = &dummyResult;

	while (true)
	{
		if (token.id == TokenId::EndOfFile)
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
	++g_Stats.numFiles;
#endif

	// Setup root ast for file
	sourceFile->astRoot = Ast::newNode<AstNode>(this, AstNodeKind::File, sourceFile, module->astRoot);

	currentScope     = module->scopeRoot;
	auto parentScope = module->scopeRoot;

	// Creates a top namespace with the module namespace name
	if (module->kind != ModuleKind::BootStrap && module->kind != ModuleKind::Runtime)
	{
		const auto moduleForNp = sourceFile->imported ? sourceFile->imported : sourceFile->module;

		Utf8 npName;
		npName.append(static_cast<const char*>(moduleForNp->buildCfg.moduleNamespace.buffer), static_cast<int>(moduleForNp->buildCfg.moduleNamespace.count));
		if (npName.empty())
			npName = moduleForNp->name;
		Ast::normalizeIdentifierName(npName);

		const auto namespaceNode  = Ast::newNode<AstNameSpace>(this, AstNodeKind::Namespace, sourceFile, sourceFile->astRoot);
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
			parentScope->flags |= SCOPE_IMPORTED;
		parentScope->flags |= SCOPE_AUTO_GENERATED;
	}

	// One scope per file. We do NOT register the scope in the list of childs
	// of the module scope, to avoid contention in // (and this is useless). That way,
	// no need to lock the module scope each time a file is encountered.
	const Utf8 scopeName = "__" + Path(sourceFile->name).replace_extension().string();
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
		for (const auto s : module->buildParameters.globalUsing)
		{
			CloneContext cxt;
			cxt.parent       = sourceFile->astRoot;
			cxt.parentScope  = currentScope;
			cxt.removeFlags  = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
			const auto node  = s->clone(cxt);
			node->sourceFile = sourceFile;
		}
	}
	else if (sourceFile->isEmbedded)
	{
		for (const auto s : sourceFile->globalUsingsEmbedded)
		{
			CloneContext cxt;
			cxt.parent       = sourceFile->astRoot;
			cxt.parentScope  = currentScope;
			cxt.removeFlags  = AST_NO_SEMANTIC; // because of :FirstPassCfgNoSem
			const auto node  = s->clone(cxt);
			node->sourceFile = sourceFile;
		}
	}

#ifdef SWAG_STATS
	Timer timer(&g_Stats.syntaxTime);
#endif

	tokenizer.setup(context, sourceFile);
	SWAG_CHECK(eatToken());

	// Module global comment must be put in module.swg
	if (!tokenizer.comment.empty() && sourceFile->module->kind == ModuleKind::Config)
		module->docComment = std::move(tokenizer.comment);

	// Parse !!!
	while (token.id != TokenId::EndOfFile)
		SWAG_CHECK(doTopLevelInstruction(sourceFile->astRoot, &dummyResult));

	return true;
}
