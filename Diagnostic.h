#pragma once
#include "SourceFile.h"
#include "Tokenizer.h"

enum DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Message,
};

struct Diagnostic
{
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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

    Diagnostic(SourceFile* file, const SourceLocation& start, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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

    Diagnostic(SourceFile* file, const Token& token, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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

    Diagnostic(SourceFile* file, const utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
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
    utf8            textMsg;
    DiagnosticLevel errorLevel;
    bool            hasFile          = false;
    bool            hasLocation      = false;
    bool            hasRangeLocation = false;
    bool            printSource      = false;
};
