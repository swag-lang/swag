#pragma once
#include "Tokenizer.h"
#include "AstNode.h"
struct SourceFile;

enum DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Note,
    CallStack,
    CallStackInlined,
    TraceError,
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
        , printSource{true}
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
        , printSource{true}
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
        , printSource{true}
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
        , printSource{true}
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
        , printSource{true}
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
    void printSourceLine(int headerSize) const;
    void report(bool verboseMode = false) const;

    SourceLocation  startLocation;
    SourceLocation  endLocation;
    Utf8            textMsg;
    vector<Utf8>    remarks;
    Utf8            hint;
    DiagnosticLevel errorLevel;

    SourceFile* sourceFile = nullptr;
    AstNode*    sourceNode = nullptr;

    uint32_t stackLevel = 0;

    bool currentStackLevel     = false;
    bool hasFile               = false;
    bool hasLocation           = false;
    bool hasRangeLocation      = false;
    bool printSource           = false;
    bool showRange             = true;
    bool showMultipleCodeLines = true;
    bool exceptionError        = false;
};

extern thread_local Utf8 g_ErrorHint;

struct PushErrHint
{
    PushErrHint(const char* msg)
    {
        g_ErrorHint = msg;
    }

    ~PushErrHint()
    {
        g_ErrorHint = nullptr;
    }
};

struct PushErrContext
{
    PushErrContext(JobContext* context, AstNode* node, JobContext::ExpansionType type)
        : cxt{context}
    {
        JobContext::ExpansionNode expNode;
        expNode.type = type;
        expNode.node = node;
        context->expansionNodes.push_back(expNode);
    }

    PushErrContext(JobContext* context, AstNode* node, const Utf8& msg)
        : cxt{context}
    {
        JobContext::ExpansionNode expNode;
        expNode.node = node;
        expNode.type = JobContext::ExpansionType::Message;
        expNode.msg  = msg;
        context->expansionNodes.push_back(expNode);
    }

    PushErrContext(JobContext* context, const JobContext::ExpansionNode& expNode)
    {
        context->expansionNodes.push_back(expNode);
    }

    ~PushErrContext()
    {
        cxt->expansionNodes.pop_back();
    }

    JobContext* cxt;
};