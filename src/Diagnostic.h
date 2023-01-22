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
    RuntimeCallStack,
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
        , showSourceCode{true}
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
        , showSourceCode{true}
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
        , showSourceCode{true}
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
        , showSourceCode{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, const Utf8& msg, const Utf8& hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : sourceFile{node->sourceFile}
        , sourceNode{node}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , textMsg{msg}
        , hint{hint}
        , errorLevel{level}
        , hasFile{true}
        , hasLocation{true}
        , hasRangeLocation{true}
        , showSourceCode{true}
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
        , showSourceCode{true}
    {
        node->computeLocation(startLocation, endLocation);
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
        , showSourceCode{true}
    {
        node->computeLocation(startLocation, endLocation);
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

    void setup();
    void setRange2(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void setRange2(AstNode* node, const Utf8& h);
    void setRange2(const Token& node, const Utf8& h);

    void setupColors(bool verboseMode);
    void collectSourceCode();
    void collectRanges();
    void printSourceCode();
    void printSourceLine();
    void printErrorLevel();
    void printMargin(bool eol = false, bool maxDigits = false, int lineNo = 0);
    void printRemarks();
    void setColorRanges(DiagnosticLevel level, bool invertColors);
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
        SourceLocation startLocation;
        SourceLocation endLocation;
        Utf8           hint;
        const char*    c     = "^";
        int            range = 0;
    };

    vector<RangeHint> ranges;
    vector<Utf8>      lines;
    vector<int>       linesNo;
    uint32_t          minBlanks = UINT32_MAX;
    LogColor          verboseColor;
    LogColor          errorColor;
    LogColor          codeColor;
    LogColor          hintColor;
    LogColor          marginCodeColor;
    LogColor          hilightCodeColor;
    LogColor          rangeNoteColor;
    LogColor          warningColor;
    LogColor          noteColor;
    LogColor          stackColor;
    LogColor          remarkColor;
    LogColor          nativeCallStackColor;
    LogColor          sourceFileColor;
    bool              invertError = false;

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

    SourceFile* contextFile  = nullptr;
    SourceFile* sourceFile   = nullptr;
    AstNode*    sourceNode   = nullptr;
    AstNode*    raisedOnNode = nullptr;

    uint32_t stackLevel = 0;

    bool lowPrio               = false;
    bool showErrorLevel        = true;
    bool display               = true;
    bool currentStackLevel     = false;
    bool hasFile               = false;
    bool hasLocation           = false;
    bool hasRangeLocation      = false;
    bool hasRangeLocation2     = false;
    bool exceptionError        = false;
    bool showSourceCode        = false;
    bool showRange             = true;
    bool showMultipleCodeLines = true;
    bool showFileName          = true;
    bool forceSourceFile       = false;
    bool emptyMarginBefore     = true;
    bool fromException         = false;
};
