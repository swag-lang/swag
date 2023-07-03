#pragma once
#include "Parser.h"
#include "Scope.h"
struct Parser;
struct AstFuncDecl;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;

struct Scoped
{
    Scoped(Parser* parser, Scope* newScope)
    {
        savedJob             = parser;
        savedScope           = parser->currentScope;
        parser->currentScope = newScope;
    }

    ~Scoped()
    {
        savedJob->currentScope = savedScope;
    }

    Parser* savedJob;
    Scope*  savedScope;
};

struct ScopedBreakable
{
    ScopedBreakable(Parser* parser, AstBreakable* newNode)
    {
        savedJob                 = parser;
        savedNode                = parser->currentBreakable;
        parser->currentBreakable = newNode;
    }

    ~ScopedBreakable()
    {
        savedJob->currentBreakable = savedNode;
    }

    Parser*       savedJob;
    AstBreakable* savedNode;
};

struct ScopedTryCatchAssume
{
    ScopedTryCatchAssume(Parser* parser, AstTryCatchAssume* newNode)
    {
        savedJob                      = parser;
        savedNode                     = parser->currentTryCatchAssume;
        parser->currentTryCatchAssume = newNode;
    }

    ~ScopedTryCatchAssume()
    {
        savedJob->currentTryCatchAssume = savedNode;
    }

    Parser*            savedJob;
    AstTryCatchAssume* savedNode;
};

struct ScopedFct
{
    ScopedFct(Parser* parser, AstFuncDecl* newFct)
    {
        savedJob           = parser;
        savedFct           = parser->currentFct;
        parser->currentFct = newFct;
    }

    ~ScopedFct()
    {
        savedJob->currentFct = savedFct;
    }

    Parser*      savedJob;
    AstFuncDecl* savedFct;
};

struct ScopedCompilerIfBlock
{
    ScopedCompilerIfBlock(Parser* parser, AstCompilerIfBlock* newIf)
    {
        savedJob                       = parser;
        savedIf                        = parser->currentCompilerIfBlock;
        parser->currentCompilerIfBlock = newIf;
    }

    ~ScopedCompilerIfBlock()
    {
        savedJob->currentCompilerIfBlock = savedIf;
    }

    Parser*             savedJob;
    AstCompilerIfBlock* savedIf;
};

struct ScopedFlags
{
    ScopedFlags(Parser* parser, uint64_t newFlags)
    {
        savedJob   = parser;
        savedFlags = parser->currentFlags;
        parser->currentFlags |= newFlags;
    }

    ~ScopedFlags()
    {
        savedJob->currentFlags = savedFlags;
    }

    Parser*  savedJob;
    uint64_t savedFlags;
};

struct ScopedStruct
{
    ScopedStruct(Parser* parser, Scope* structScope)
    {
        savedJob                   = parser;
        savedStruct                = parser->currentStructScope;
        parser->currentStructScope = structScope;
    }

    ~ScopedStruct()
    {
        savedJob->currentStructScope = savedStruct;
    }

    Parser* savedJob;
    Scope*  savedStruct;
};

struct ScopedSelfStruct
{
    ScopedSelfStruct(Parser* parser, Scope* structScope)
    {
        savedJob                       = parser;
        savedStruct                    = parser->currentSelfStructScope;
        parser->currentSelfStructScope = structScope;
    }

    ~ScopedSelfStruct()
    {
        savedJob->currentSelfStructScope = savedStruct;
    }

    Parser* savedJob;
    Scope*  savedStruct;
};
