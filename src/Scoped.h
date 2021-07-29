#pragma once
#include "SyntaxJob.h"
#include "Scope.h"
struct SyntaxJob;
struct AstFuncDecl;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;

struct Scoped
{
    Scoped(SyntaxJob* job, Scope* newScope)
    {
        savedJob          = job;
        savedScope        = job->currentScope;
        job->currentScope = newScope;
    }

    ~Scoped()
    {
        savedJob->currentScope = savedScope;
    }

    SyntaxJob* savedJob;
    Scope*     savedScope;
};

struct ScopedBreakable
{
    ScopedBreakable(SyntaxJob* job, AstBreakable* newNode)
    {
        savedJob              = job;
        savedNode             = job->currentBreakable;
        job->currentBreakable = newNode;
    }

    ~ScopedBreakable()
    {
        savedJob->currentBreakable = savedNode;
    }

    SyntaxJob*    savedJob;
    AstBreakable* savedNode;
};

struct ScopedTryCatchAssume
{
    ScopedTryCatchAssume(SyntaxJob* job, AstTryCatchAssume* newNode)
    {
        savedJob                   = job;
        savedNode                  = job->currentTryCatchAssume;
        job->currentTryCatchAssume = newNode;
    }

    ~ScopedTryCatchAssume()
    {
        savedJob->currentTryCatchAssume = savedNode;
    }

    SyntaxJob*         savedJob;
    AstTryCatchAssume* savedNode;
};

struct ScopedFct
{
    ScopedFct(SyntaxJob* job, AstFuncDecl* newFct)
    {
        savedJob        = job;
        savedFct        = job->currentFct;
        job->currentFct = newFct;
    }

    ~ScopedFct()
    {
        savedJob->currentFct = savedFct;
    }

    SyntaxJob*   savedJob;
    AstFuncDecl* savedFct;
};

struct ScopedCompilerIfBlock
{
    ScopedCompilerIfBlock(SyntaxJob* job, AstCompilerIfBlock* newIf)
    {
        savedJob                    = job;
        savedIf                     = job->currentCompilerIfBlock;
        job->currentCompilerIfBlock = newIf;
    }

    ~ScopedCompilerIfBlock()
    {
        savedJob->currentCompilerIfBlock = savedIf;
    }

    SyntaxJob*          savedJob;
    AstCompilerIfBlock* savedIf;
};

struct ScopedFlags
{
    ScopedFlags(SyntaxJob* job, uint64_t newFlags)
    {
        savedJob   = job;
        savedFlags = job->currentFlags;
        job->currentFlags |= newFlags;
    }

    ~ScopedFlags()
    {
        savedJob->currentFlags = savedFlags;
    }

    SyntaxJob* savedJob;
    uint64_t   savedFlags;
};

struct ScopedStruct
{
    ScopedStruct(SyntaxJob* job, Scope* structScope)
    {
        savedJob                = job;
        savedStruct             = job->currentStructScope;
        job->currentStructScope = structScope;
    }

    ~ScopedStruct()
    {
        savedJob->currentStructScope = savedStruct;
    }

    SyntaxJob* savedJob;
    Scope*     savedStruct;
};

struct ScopedSelfStruct
{
    ScopedSelfStruct(SyntaxJob* job, Scope* structScope)
    {
        savedJob                    = job;
        savedStruct                 = job->currentSelfStructScope;
        job->currentSelfStructScope = structScope;
    }

    ~ScopedSelfStruct()
    {
        savedJob->currentSelfStructScope = savedStruct;
    }

    SyntaxJob* savedJob;
    Scope*     savedStruct;
};

struct ScopedLocation
{
    ScopedLocation(SyntaxJob* job, Token* token)
    {
        savedJob                  = job;
        savedToken                = job->currentTokenLocation;
        myToken.startLocation     = token->startLocation;
        myToken.endLocation       = token->endLocation;
        job->currentTokenLocation = &myToken;
    }

    ~ScopedLocation()
    {
        savedJob->currentTokenLocation = savedToken;
    }

    SyntaxJob* savedJob;
    Token      myToken;
    Token*     savedToken;
};
