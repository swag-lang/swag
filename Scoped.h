#pragma once
#include "SyntaxJob.h"
struct SyntaxJob;
struct Scope;

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