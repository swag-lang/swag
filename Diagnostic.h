#pragma once
#include "Tokenizer.h"
#include "AstNode.h"
struct SourceFile;

enum DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Message,
    Note,
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

    Diagnostic(SourceFile* file, AstNode* node, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , printSource{true}
    {
        node->inheritLocationFromChilds();
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

    void report(bool verboseMode = false) const;
    void defaultColor(bool verboseMode) const;

    SourceFile*     sourceFile = nullptr;
    SourceLocation  startLocation;
    SourceLocation  endLocation;
    Utf8            textMsg;
    DiagnosticLevel errorLevel;
    bool            hasFile          = false;
    bool            hasLocation      = false;
    bool            hasRangeLocation = false;
    bool            printSource      = false;
    bool            showRange        = true;
};

struct ErrorContext
{
    SourceFile*                sourceFile;
    vector<struct AstNode*>    genericInstanceTree;
    vector<struct SourceFile*> genericInstanceTreeFile;

    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const vector<const Diagnostic*>& notes);
};
