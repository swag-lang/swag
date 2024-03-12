#pragma once
#include "Parser.h"
struct Parser;
struct AstFuncDecl;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;

struct ParserPushScope
{
    explicit ParserPushScope(Parser* parser, Scope* newScope) :
        savedParser(parser)
    {
        savedScope                = parser->currentScope;
        savedParser->currentScope = newScope;
    }

    ~ParserPushScope()
    {
        savedParser->currentScope = savedScope;
    }

    Parser* savedParser;
    Scope*  savedScope;
};

struct ParserPushBreakable
{
    explicit ParserPushBreakable(Parser* parser, AstBreakable* newNode) :
        savedParser(parser)
    {
        savedNode                     = parser->currentBreakable;
        savedParser->currentBreakable = newNode;
    }

    ~ParserPushBreakable()
    {
        savedParser->currentBreakable = savedNode;
    }

    Parser*       savedParser;
    AstBreakable* savedNode;
};

struct ParserPushTryCatchAssume
{
    explicit ParserPushTryCatchAssume(Parser* parser, AstTryCatchAssume* newNode) :
        savedParser(parser)
    {
        savedNode                          = parser->currentTryCatchAssume;
        savedParser->currentTryCatchAssume = newNode;
    }

    ~ParserPushTryCatchAssume()
    {
        savedParser->currentTryCatchAssume = savedNode;
    }

    Parser*            savedParser;
    AstTryCatchAssume* savedNode;
};

struct ParserPushFct
{
    explicit ParserPushFct(Parser* parser, AstFuncDecl* newFct) :
        savedParser(parser)
    {
        savedFct                = parser->currentFct;
        savedParser->currentFct = newFct;
    }

    ~ParserPushFct()
    {
        savedParser->currentFct = savedFct;
    }

    Parser*      savedParser;
    AstFuncDecl* savedFct;
};

struct ParserPushCompilerIfBlock
{
    explicit ParserPushCompilerIfBlock(Parser* parser, AstCompilerIfBlock* newIf) :
        savedParser(parser)
    {
        savedIf                        = parser->currentCompilerIfBlock;
        parser->currentCompilerIfBlock = newIf;
    }

    ~ParserPushCompilerIfBlock()
    {
        savedParser->currentCompilerIfBlock = savedIf;
    }

    Parser*             savedParser;
    AstCompilerIfBlock* savedIf;
};

struct ParserPushAstNodeFlags
{
    explicit ParserPushAstNodeFlags(Parser* parser, AstNodeFlags newFlags) :
        savedParser(parser)
    {
        savedFlags = parser->currentAstNodeFlags;
        savedParser->currentAstNodeFlags.add(newFlags);
    }

    ~ParserPushAstNodeFlags()
    {
        savedParser->currentAstNodeFlags = savedFlags;
    }

    Parser*      savedParser;
    AstNodeFlags savedFlags;
};

struct ParserPushStructScope
{
    explicit ParserPushStructScope(Parser* parser, Scope* structScope) :
        savedParser(parser)
    {
        savedStruct                     = parser->currentStructScope;
        savedParser->currentStructScope = structScope;
    }

    ~ParserPushStructScope()
    {
        savedParser->currentStructScope = savedStruct;
    }

    Parser* savedParser;
    Scope*  savedStruct;
};

struct ParserPushSelfStructScope
{
    explicit ParserPushSelfStructScope(Parser* parser, Scope* structScope) :
        savedParser(parser)
    {
        savedStruct                         = parser->currentSelfStructScope;
        savedParser->currentSelfStructScope = structScope;
    }

    ~ParserPushSelfStructScope()
    {
        savedParser->currentSelfStructScope = savedStruct;
    }

    Parser* savedParser;
    Scope*  savedStruct;
};

struct ParserPushFreezeFormat
{
    explicit ParserPushFreezeFormat(Parser* parser) :
        savedParser(parser)
    {
        savedParser->freezeFormat++;
    }

    ~ParserPushFreezeFormat()
    {
        SWAG_ASSERT(savedParser->freezeFormat);
        savedParser->freezeFormat--;
    }

    Parser* savedParser;
};
