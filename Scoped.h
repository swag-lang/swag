#pragma once
#include "SyntaxJob.h"
#include "Scope.h"
struct SyntaxJob;
struct AstFuncDecl;
struct AstCompilerIfBlock;

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

struct ScopedAttributesFlags
{
    ScopedAttributesFlags(SyntaxJob* job, uint32_t newFlags)
    {
        savedJob                    = job;
        savedFlags                  = job->currentAttributesFlags;
        job->currentAttributesFlags = newFlags;
    }

    ~ScopedAttributesFlags()
    {
        savedJob->currentAttributesFlags = savedFlags;
    }

    SyntaxJob* savedJob;
    uint32_t   savedFlags;
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

struct ScopedMainNode
{
    ScopedMainNode(SyntaxJob* job, AstNode* mainNode)
    {
        savedJob             = job;
        savedMainNode        = job->currentMainNode;
        job->currentMainNode = mainNode;
    }

    ~ScopedMainNode()
    {
        savedJob->currentMainNode = savedMainNode;
    }

    SyntaxJob* savedJob;
    AstNode*   savedMainNode;
};
