#pragma once
#include "Parser.h"
#include "Scope.h"
struct Parser;
struct AstFuncDecl;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;

struct Scoped
{
    Scoped(Parser* job, Scope* newScope)
    {
        savedJob          = job;
        savedScope        = job->currentScope;
        job->currentScope = newScope;
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
    ScopedBreakable(Parser* job, AstBreakable* newNode)
    {
        savedJob              = job;
        savedNode             = job->currentBreakable;
        job->currentBreakable = newNode;
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
    ScopedTryCatchAssume(Parser* job, AstTryCatchAssume* newNode)
    {
        savedJob                   = job;
        savedNode                  = job->currentTryCatchAssume;
        job->currentTryCatchAssume = newNode;
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
    ScopedFct(Parser* job, AstFuncDecl* newFct)
    {
        savedJob        = job;
        savedFct        = job->currentFct;
        job->currentFct = newFct;
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
    ScopedCompilerIfBlock(Parser* job, AstCompilerIfBlock* newIf)
    {
        savedJob                    = job;
        savedIf                     = job->currentCompilerIfBlock;
        job->currentCompilerIfBlock = newIf;
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
    ScopedFlags(Parser* job, uint64_t newFlags)
    {
        savedJob   = job;
        savedFlags = job->currentFlags;
        job->currentFlags |= newFlags;
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
    ScopedStruct(Parser* job, Scope* structScope)
    {
        savedJob                = job;
        savedStruct             = job->currentStructScope;
        job->currentStructScope = structScope;
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
    ScopedSelfStruct(Parser* job, Scope* structScope)
    {
        savedJob                    = job;
        savedStruct                 = job->currentSelfStructScope;
        job->currentSelfStructScope = structScope;
    }

    ~ScopedSelfStruct()
    {
        savedJob->currentSelfStructScope = savedStruct;
    }

    Parser* savedJob;
    Scope*  savedStruct;
};
