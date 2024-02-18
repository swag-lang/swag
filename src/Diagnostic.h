#pragma once
#include "AstNode.h"
#include "Tokenizer.h"

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
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , startLocation{start}
        , endLocation{end}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const SourceLocation& start, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , startLocation{start}
        , endLocation{start}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const Token& token, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , sourceFile{file}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, Utf8 msg, Utf8 hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , startLocation{token.startLocation}
        , endLocation{token.endLocation}
        , hint{std::move(hint)}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        setup();
    }

    Diagnostic(AstNode* node, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        node->computeLocation(startLocation, endLocation);
        setup();
    }

    Diagnostic(AstNode* node, Utf8 msg, Utf8 hint, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , hint{std::move(hint)}
        , sourceFile{node->sourceFile}
        , sourceNode{node}
        , showSourceCode{true}
        , hasLocation{true}
    {
        node->computeLocation(startLocation, endLocation);
        setup();
    }

    Diagnostic(SourceFile* file, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
        , sourceFile{file}
    {
        setup();
    }

    Diagnostic(Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error)
        : textMsg{std::move(msg)}
        , errorLevel{level}
    {
        setup();
    }

    static constexpr uint32_t    MAX_INDENT_BLANKS       = 10;
    static constexpr const char* ERROR_MESSAGE_SEPARATOR = "$";

    static Diagnostic* note(const Utf8& msg) { return new Diagnostic{msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Token& token, const Utf8& msg) { return new Diagnostic{node, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg, const Utf8& hint) { return new Diagnostic{node, msg, hint, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const Token& token, const Utf8& msg) { return new Diagnostic{file, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg) { return new Diagnostic{file, start, end, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg) { return node ? new Diagnostic{node, msg, DiagnosticLevel::Note} : nullptr; }

    void setup();
    void addNote(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void addNote(AstNode* node, const Utf8& h);
    void addNote(const Token& token, const Utf8& h);

    void setupColors();
    void collectSourceCode();
    void sortRanges();
    void collectRanges();
    void printSourceCode() const;
    void printSourceLine() const;
    void printErrorLevel();
    void printMarginLineNo(int lineNo) const;
    void printMargin(bool eol = false, bool printLineNo = false, int lineNo = 0) const;
    void printPreRemarks() const;
    void printRemarks() const;
    void setColorRanges(DiagnosticLevel level) const;
    void alignRangeColumn(int& curColumn, int where, bool withCode = true) const;
    int  printRangesVerticalBars(size_t maxMarks);
    void printLastRangeHint(int curColumn);
    void printRanges();

    void reportCompact();
    void report();

    static void        tokenizeError(const Utf8& err, Vector<Utf8>& tokens);
    static Utf8        oneLiner(const Utf8& err);
    static bool        hastErrorId(const Utf8& textMsg);
    static Utf8        isType(TypeInfo* typeInfo);
    static Utf8        isType(const SymbolOverload* overload);
    static Utf8        isType(const AstNode* node);
    static Diagnostic* hereIs(AstNode* node, const char* msg = nullptr);
    static Diagnostic* hereIs(const SymbolOverload* overload);

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

    Vector<const Diagnostic*> notes;
    Vector<RangeHint>         ranges;
    Utf8                      lineCode;
    int                       lineCodeNum       = 0;
    int                       lineCodeNumDigits = 0;
    int                       lineCodeMaxDigits = 0;
    int                       minBlanks         = 0;
    LogColor                  errorColor;
    LogColor                  hintColor;
    LogColor                  marginBorderColor;
    LogColor                  rangeNoteColor;
    LogColor                  warningColor;
    LogColor                  noteTitleColor;
    LogColor                  noteColor;
    LogColor                  stackColor;
    LogColor                  preRemarkColor;
    LogColor                  autoRemarkColor;
    LogColor                  remarkColor;
    LogColor                  sourceFileColor;
    LogColor                  codeLineNoColor;

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
