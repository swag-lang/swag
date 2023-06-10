#pragma once
#pragma once
#include "Tokenizer.h"
#include "AstNode.h"
#include "ErrorIds.h"
struct SourceFile;
struct TypeInfo;
enum class LogColor;

enum class DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Note,
    Help,
};

struct Diagnostic
{
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : startLocation{start}
        , endLocation{end}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const SourceLocation& start, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : startLocation{start}
        , endLocation{start}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const Token& token, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, const Utf8& msg, const Utf8& hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , hint{hint}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{msg}
        , errorLevel{level}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        node->computeLocation(startLocation, endLocation);
        setup();
    }

    Diagnostic(AstNode* node, const Utf8& msg, const Utf8& hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : hint{hint}
        , textMsg{msg}
        , errorLevel{level}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        node->computeLocation(startLocation, endLocation);
        setup();
    }

    Diagnostic(SourceFile* file, const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{msg}
        , errorLevel{level}
        , sourceFile{file}
    {
        setup();
    }

    Diagnostic(const Utf8& msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{msg}
        , errorLevel{level}
    {
        setup();
    }

    // clang-format off
    static Diagnostic* note(const Utf8& msg) { return new Diagnostic{msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Token& token, const Utf8& msg) { return new Diagnostic{node, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg, const Utf8& hint) { return new Diagnostic{node, msg, hint, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const Token& token, const Utf8& msg) { return new Diagnostic{file, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg) { return new Diagnostic{file, start, end, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg) { return new Diagnostic{node, msg, DiagnosticLevel::Note}; }
    static Diagnostic* help(const Utf8& msg) { return new Diagnostic{msg, DiagnosticLevel::Help}; }
    static Diagnostic* help(AstNode* node, const Token& token, const Utf8& msg) { return new Diagnostic{node, token, msg, DiagnosticLevel::Help}; }
    static Diagnostic* help(SourceFile* file, const Token& token, const Utf8& msg) { return new Diagnostic{file, token, msg, DiagnosticLevel::Help}; }
    static Diagnostic* help(AstNode* node, const Utf8& msg) { return new Diagnostic{node, msg, DiagnosticLevel::Help}; }
    // clang-format on

    void setup();
    void addRange(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void addRange(AstNode* node, const Utf8& h);
    void addRange(const Token& node, const Utf8& h);

    void setupColors(bool verboseMode);
    void collectSourceCode();
    void sortRanges();
    void collectRanges();
    Utf8 syntax(const Utf8& line);
    void printSourceCode(bool verboseMode);
    void printSourceLine();
    void printErrorLevel();
    void printMargin(bool eol = false, bool maxDigits = false, int lineNo = 0);
    void printRemarks();
    void setColorRanges(DiagnosticLevel level);
    void printRanges();

    void reportCompact(bool verboseMode);
    void report(bool verboseMode = false);

    static Utf8        isType(TypeInfo* typeInfo);
    static Utf8        isType(AstNode* node);
    static Diagnostic* hereIs(SymbolOverload* overload, bool forceShowRange = false);
    static Diagnostic* hereIs(AstNode* node, bool forceShowRange = false);

    static const int MAX_LINE_DIGITS = 5;

    struct RangeHint
    {
        SourceLocation  startLocation;
        SourceLocation  endLocation;
        Utf8            hint;
        DiagnosticLevel errorLevel;
        int             width = 0;
    };

    Vector<RangeHint> ranges;
    Vector<Utf8>      lines;
    Vector<int>       linesNo;
    uint32_t          minBlanks = UINT32_MAX;
    LogColor          verboseColor;
    LogColor          errorColor;
    LogColor          codeColor;
    LogColor          hintColor;
    LogColor          marginCodeColor;
    LogColor          rangeNoteColor;
    LogColor          warningColor;
    LogColor          noteColor;
    LogColor          stackColor;
    LogColor          autoRemarkColor;
    LogColor          remarkColor;
    LogColor          sourceFileColor;

    SourceLocation startLocation;
    SourceLocation endLocation;
    Utf8           hint;

    Utf8            textMsg;
    Vector<Utf8>    remarks;
    Vector<Utf8>    autoRemarks;
    Utf8            noteHeader;
    DiagnosticLevel errorLevel;

    SourceFile* contextFile  = nullptr;
    SourceFile* sourceFile   = nullptr;
    AstNode*    sourceNode   = nullptr;
    AstNode*    raisedOnNode = nullptr;

    bool showSourceCode        = false;
    bool showRange             = true;
    bool showMultipleCodeLines = true;
    bool showFileName          = true;

    bool showErrorLevel    = true;
    bool display           = true;
    bool emptyMarginBefore = true;

    bool hasLocation       = false;
    bool lowPrio           = false;
    bool forceSourceFile   = false;
    bool criticalError     = false;
    bool isNote            = false;
};
