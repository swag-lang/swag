#pragma once
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
        setup();
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
        setup();
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
        setup();
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
        setup();
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
        setup();
    }

    Diagnostic(AstNode* node, const Utf8& msg, const Utf8& hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{node->sourceFile}
        , sourceNode{node}
        , textMsg{msg}
        , hint{hint}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSource{true}
    {
        node->computeEndLocation();
        startLocation = node->token.startLocation;
        endLocation   = node->token.endLocation;
        setup();
    }

    Diagnostic(SourceFile* file, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{file}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{false}
    {
        setup();
    }

    Diagnostic(const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{nullptr}
        , textMsg{msg}
        , errorLevel{level}
        , hasFile{false}
        , hasLocation{false}
    {
        setup();
    }

    void        setup();
    bool        mustPrintCode() const;
    void        printSourceLine() const;
    static void printMargin(bool verboseMode, bool eol = false);
    void        report(bool verboseMode = false) const;
    void        setRange2(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void        setRange2(AstNode* node, const Utf8& h);
    void        setRange2(const Token& node, const Utf8& h);

    static Utf8        isType(TypeInfo* typeInfo);
    static Diagnostic* hereIs(SymbolOverload* overload, bool forceShowRange = false);

    SourceLocation startLocation;
    SourceLocation endLocation;
    Utf8           hint;

    SourceLocation startLocation2;
    SourceLocation endLocation2;
    Utf8           hint2;

    Utf8            textMsg;
    vector<Utf8>    remarks;
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
    bool hasRangeLocation2     = false;
    bool exceptionError        = false;
    bool showSource            = false;
    bool showRange             = true;
    bool showMultipleCodeLines = true;
    bool showFileName          = true;
    bool forceSourceFile       = false;
};
