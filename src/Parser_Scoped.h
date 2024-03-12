#pragma once
#include "Parser.h"
struct Parser;
struct AstFuncDecl;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;

struct Scoped
{
    Scoped(Parser* parser, Scope* newScope) :
        savedParser(parser)
    {
        savedScope                = parser->currentScope;
        savedParser->currentScope = newScope;
    }

    ~Scoped()
    {
        savedParser->currentScope = savedScope;
    }

    Parser* savedParser;
    Scope*  savedScope;
};

struct ScopedBreakable
{
    ScopedBreakable(Parser* parser, AstBreakable* newNode) :
        savedParser(parser)
    {
        savedNode                     = parser->currentBreakable;
        savedParser->currentBreakable = newNode;
    }

    ~ScopedBreakable()
    {
        savedParser->currentBreakable = savedNode;
    }

    Parser*       savedParser;
    AstBreakable* savedNode;
};

struct ScopedTryCatchAssume
{
    ScopedTryCatchAssume(Parser* parser, AstTryCatchAssume* newNode) :
        savedParser(parser)
    {
        savedNode                          = parser->currentTryCatchAssume;
        savedParser->currentTryCatchAssume = newNode;
    }

    ~ScopedTryCatchAssume()
    {
        savedParser->currentTryCatchAssume = savedNode;
    }

    Parser*            savedParser;
    AstTryCatchAssume* savedNode;
};

struct ScopedFct
{
    ScopedFct(Parser* parser, AstFuncDecl* newFct) :
        savedParser(parser)
    {
        savedFct                = parser->currentFct;
        savedParser->currentFct = newFct;
    }

    ~ScopedFct()
    {
        savedParser->currentFct = savedFct;
    }

    Parser*      savedParser;
    AstFuncDecl* savedFct;
};

struct ScopedCompilerIfBlock
{
    ScopedCompilerIfBlock(Parser* parser, AstCompilerIfBlock* newIf) :
        savedParser(parser)
    {
        savedIf                        = parser->currentCompilerIfBlock;
        parser->currentCompilerIfBlock = newIf;
    }

    ~ScopedCompilerIfBlock()
    {
        savedParser->currentCompilerIfBlock = savedIf;
    }

    Parser*             savedParser;
    AstCompilerIfBlock* savedIf;
};

struct ScopedFlags
{
    ScopedFlags(Parser* parser, AstNodeFlags newFlags) :
        savedParser(parser)
    {
        savedFlags = parser->currentAstNodeFlags;
        savedParser->currentAstNodeFlags.add(newFlags);
    }

    ~ScopedFlags()
    {
        savedParser->currentAstNodeFlags = savedFlags;
    }

    Parser*      savedParser;
    AstNodeFlags savedFlags;
};

struct ScopedStruct
{
    ScopedStruct(Parser* parser, Scope* structScope) :
        savedParser(parser)
    {
        savedStruct                     = parser->currentStructScope;
        savedParser->currentStructScope = structScope;
    }

    ~ScopedStruct()
    {
        savedParser->currentStructScope = savedStruct;
    }

    Parser* savedParser;
    Scope*  savedStruct;
};

struct ScopedSelfStruct
{
    ScopedSelfStruct(Parser* parser, Scope* structScope) :
        savedParser(parser)
    {
        savedStruct                         = parser->currentSelfStructScope;
        savedParser->currentSelfStructScope = structScope;
    }

    ~ScopedSelfStruct()
    {
        savedParser->currentSelfStructScope = savedStruct;
    }

    Parser* savedParser;
    Scope*  savedStruct;
};

struct ScopedFreezeFormat
{
    ScopedFreezeFormat(Parser* parser) :
        savedParser(parser)
    {
        savedParser->freezeFormat++;
    }

    ~ScopedFreezeFormat()
    {
        SWAG_ASSERT(savedParser->freezeFormat);
        savedParser->freezeFormat--;
    }

    Parser* savedParser;
};
