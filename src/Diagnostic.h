#pragma once
#include "Tokenizer.h"
#include "AstNode.h"
struct SourceFile;
struct TypeInfo;
enum class LogColor;

enum class DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Note,
    CallStack,
    CallStackInlined,
    TraceError,
    Help,
};

struct Diagnostic
{
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , startLocation{start}
        , endLocation{end}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSource{true}
    {
    }

    Diagnostic(SourceFile* file, const SourceLocation& start, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , startLocation{start}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{false}
        , showSource{true}
    {
    }

    Diagnostic(SourceFile* file, const Token& token, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSource{true}
    {
    }

    Diagnostic(AstNode* node, const Token& token, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{node->sourceFile}
        , sourceNode{node}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSource{true}
    {
    }

    Diagnostic(AstNode* node, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{node->sourceFile}
        , sourceNode{node}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSource{true}
    {
        node->computeEndLocation();
        startLocation = node->token.startLocation;
        endLocation   = node->token.endLocation;
    }

    Diagnostic(SourceFile* file, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{false}
    {
    }

    Diagnostic(const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{nullptr}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{false}
        , hasLocation{false}
    {
    }

    bool mustPrintCode() const;
    void printSourceLine() const;
    void printMargin(LogColor color, bool eol = false) const;
    void report(bool verboseMode = false) const;

    SourceLocation  startLocation;
    SourceLocation  endLocation;
    Utf8            textMsg;
    vector<Utf8>    remarks;
    Utf8            hint;
    Utf8            noteHeader;
    DiagnosticLevel errorLevel;

    SourceFile* contextFile = nullptr;
    SourceFile* sourceFile  = nullptr;
    AstNode*    sourceNode  = nullptr;

    uint32_t stackLevel = 0;

    bool currentStackLevel     = false;
    bool hasFile               = false;
    bool hasLocation           = false;
    bool hasRangeLocation      = false;
    bool exceptionError        = false;
    bool showSource            = false;
    bool showRange             = true;
    bool showMultipleCodeLines = true;
    bool showFileName          = true;
};

struct PushErrContext
{
    PushErrContext(JobContext* context, AstNode* node, JobContext::ErrorContextType type)
        : cxt{context}
    {
        JobContext::ErrorContext expNode;
        expNode.type = type;
        expNode.node = node;
        context->errorContextStack.push_back(expNode);
    }

    PushErrContext(JobContext* context, AstNode* node, const Utf8& msg, const Utf8& hint, DiagnosticLevel level = DiagnosticLevel::Note)
        : cxt{context}
    {
        JobContext::ErrorContext expNode;
        expNode.node  = node;
        expNode.type  = JobContext::ErrorContextType::Message;
        expNode.level = level;
        expNode.msg   = msg;
        expNode.hint  = hint;
        context->errorContextStack.push_back(expNode);
    }

    ~PushErrContext()
    {
        cxt->errorContextStack.pop_back();
    }

    JobContext* cxt;
};

namespace Hint
{
    Utf8 isType(TypeInfo* typeInfo);
} // namespace Hint

#define Err(__num) g_E[__num]
#define Nte(__num) g_E[__num]
#define Hnt(__num) g_E[__num]
#define Hlp(__num) g_E[__num]