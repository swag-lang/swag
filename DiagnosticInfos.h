#pragma once
#include "CommandLine.h"
struct SourceFile;
struct AstNode;
struct Utf8;
struct ByteCodeInstruction;

#ifdef SWAG_HAS_ASSERT
struct DiagnosticInfosStep
{
    string               message;
    SourceFile*          sourceFile = nullptr;
    AstNode*             node       = nullptr;
    ByteCodeInstruction* ip         = nullptr;
    const char*          user       = nullptr;
};

struct DiagnosticInfos
{
    void push()
    {
        DiagnosticInfosStep step;
        steps.push_back(step);
    }

    void pop()
    {
        if (!steps.empty())
            steps.pop_back();
    }

    DiagnosticInfosStep& last()
    {
        return steps.back();
    }

    void clear()
    {
        steps.clear();
    }

    void reportError(const Utf8& msg);
    void log();

    vector<DiagnosticInfosStep> steps;
};

extern thread_local DiagnosticInfos g_diagnosticInfos;

struct PushDiagnosticInfos
{
    PushDiagnosticInfos()
    {
        if (g_CommandLine.devMode)
            g_diagnosticInfos.push();
    }

    ~PushDiagnosticInfos()
    {
        if (g_CommandLine.devMode)
            g_diagnosticInfos.pop();
    }
};
#endif