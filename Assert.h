#pragma once
struct SourceFile;
struct AstNode;
struct Utf8;

#ifdef SWAG_HAS_ASSERT
struct DiagnosticInfosStep
{
    string      message;
    SourceFile* sourceFile = nullptr;
    AstNode*    node       = nullptr;
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
        g_diagnosticInfos.push();
    }

    ~PushDiagnosticInfos()
    {
        g_diagnosticInfos.pop();
    }
};

extern void swag_assert(const char* expr, const char* file, int line);

#define SWAG_ASSERT(__expr)                           \
    {                                                 \
        if (!(__expr))                                \
        {                                             \
            swag_assert(#__expr, __FILE__, __LINE__); \
        }                                             \
    }

#else
#define SWAG_ASSERT(__expr) \
    {                       \
    }
#endif