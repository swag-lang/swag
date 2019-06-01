#pragma once
#include "SyntaxJob.h"
struct SyntaxJob;
struct Scope;
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