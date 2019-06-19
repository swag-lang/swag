#pragma once
#include "SyntaxJob.h"
#include "Scope.h"
struct SyntaxJob;
struct AstFuncDecl;

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
        savedJob                 = job;
        savedNode                = job->currentBreakable;
        newNode->parentBreakable = savedNode;
        job->currentBreakable    = newNode;
    }

    ~ScopedBreakable()
    {
        savedJob->currentBreakable = savedNode;
    }

    SyntaxJob*    savedJob;
    AstBreakable* savedNode;
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

struct ScopedFlags
{
    ScopedFlags(SyntaxJob* job, uint64_t newFlags)
    {
        savedJob          = job;
        savedFlags        = job->currentFlags;
        job->currentFlags = newFlags;
    }

    ~ScopedFlags()
    {
        savedJob->currentFlags = savedFlags;
    }

    SyntaxJob* savedJob;
    uint64_t   savedFlags;
};
