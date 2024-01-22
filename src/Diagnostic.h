#pragma once
#include "Tokenizer.h"
#include "AstNode.h"
#include "ErrorIds.h"
struct SourceFile;
struct TypeInfo;
struct SyntaxColorContext;
enum class LogColor;

enum class DiagnosticLevel
{
    Error,
    Warning,
    Verbose,
    Note,
    Panic,
    Exception,
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

    static const uint32_t        MAX_INDENT_BLANKS       = 10;
    static constexpr const char* ERROR_MESSAGE_SEPARATOR = "$";

    // clang-format off
    static Diagnostic* note(const Utf8& msg) { return new Diagnostic{msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Token& token, const Utf8& msg) { return new Diagnostic{node, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg, const Utf8& hint) { return new Diagnostic{node, msg, hint, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const Token& token, const Utf8& msg) { return new Diagnostic{file, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg) { return new Diagnostic{file, start, end, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg) { return node ? new Diagnostic{node, msg, DiagnosticLevel::Note} : nullptr; }
    // clang-format on

    void setup();
    void addRange(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void addRange(AstNode* node, const Utf8& h);
    void addRange(const Token& node, const Utf8& h);

    void setupColors();
    void collectSourceCode();
    void sortRanges();
    void collectRanges();
    void printSourceCode();
    void printSourceLine();
    void printErrorLevel();
    void printMarginLineNo(int lineNo);
    void printMargin(bool eol = false, bool maxDigits = false, int lineNo = 0);
    void printPreRemarks();
    void printRemarks();
    void setColorRanges(DiagnosticLevel level);
    int  alignRangeColumn(int curColumn, int where, bool withCode = true);
    int  printRangesVerticalBars(size_t maxMarks);
    void printLastRangeHint(int curColumn);
    void printRanges();

    void reportCompact();
    void report();

    static void        tokenizeError(const Utf8& err, Vector<Utf8>& tokens);
    static Utf8        oneLiner(const Utf8& err);
    static bool        hastErrorId(const Utf8& textMsg);
    static Utf8        isType(TypeInfo* typeInfo);
    static Utf8        isType(SymbolOverload* overload);
    static Utf8        isType(AstNode* node);
    static Diagnostic* hereIs(AstNode* node);
    static Diagnostic* hereIs(SymbolOverload* overload);

    struct RangeHint
    {
        SourceLocation  startLocation;
        SourceLocation  endLocation;
        Utf8            hint;
        DiagnosticLevel errorLevel;
        int             width     = 0;
        int             mid       = 0;
        bool            hasBar    = false;
        bool            mergeNext = false;
    };

    Utf8            textMsg;
    Vector<Utf8>    preRemarks;
    Vector<Utf8>    remarks;
    Vector<Utf8>    autoRemarks;
    DiagnosticLevel errorLevel;

    Vector<RangeHint> ranges;
    Utf8              lineCode;
    int               lineCodeNum       = 0;
    int               lineCodeNumDigits = 0;
    int               lineCodeMaxDigits = 0;
    int               minBlanks         = 0;
    LogColor          errorColor;
    LogColor          hintColor;
    LogColor          marginBorderColor;
    LogColor          rangeNoteColor;
    LogColor          warningColor;
    LogColor          noteTitleColor;
    LogColor          noteColor;
    LogColor          stackColor;
    LogColor          preRemarkColor;
    LogColor          autoRemarkColor;
    LogColor          remarkColor;
    LogColor          sourceFileColor;
    LogColor          codeLineNoColor;

    SourceLocation startLocation;
    SourceLocation endLocation;
    Utf8           hint;

    SourceFile* contextFile = nullptr;
    SourceFile* sourceFile  = nullptr;
    AstNode*    sourceNode  = nullptr;
    AstNode*    contextNode = nullptr;

    bool showSourceCode = false;
    bool showErrorLevel = true;
    bool showFileName   = true;

    bool canBeMerged = true;
    bool display     = true;
    bool hasLocation = false;
    bool hasContent  = false;
};
