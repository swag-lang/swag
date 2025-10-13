#pragma once
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Syntax/Tokenizer/Token.h"

struct SourceFile;
struct TypeInfo;
struct SyntaxColorContext;
struct Log;
struct LogWriteContext;
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

enum class HintPart
{
    ErrorLevel,
    Text,
};

struct Diagnostic
{
    Diagnostic(SourceFile* file, const SourceLocation& start, const SourceLocation& end, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        startLocation{start},
        endLocation{end},
        sourceFile{file},
        showSourceCode{true},
        hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const SourceLocation& start, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        startLocation{start},
        endLocation{start},
        sourceFile{file},
        showSourceCode{true},
        hasLocation{true}
    {
        setup();
    }

    Diagnostic(SourceFile* file, const Token& token, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        startLocation{token.startLocation},
        endLocation{token.endLocation},
        sourceFile{file},
        showSourceCode{true},
        hasLocation{true}
    {
        replaceTokenName(token);
        setup();
    }

    Diagnostic(SourceFile* file, const TokenParse& tokenParse, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        startLocation{tokenParse.token.startLocation},
        endLocation{tokenParse.token.endLocation},
        sourceFile{file},
        showSourceCode{true},
        hasLocation{true}
    {
        replaceTokenName(tokenParse);
        setup();
    }

    Diagnostic(AstNode* node, const Token& token, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        startLocation{token.startLocation},
        endLocation{token.endLocation},
        sourceFile{node->token.sourceFile},
        sourceNode{node},
        showSourceCode{true},
        hasLocation{true}
    {
        replaceTokenName(token);
        setup();
    }

    Diagnostic(AstNode* node, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        sourceFile{node->token.sourceFile},
        sourceNode{node},
        showSourceCode{true},
        hasLocation{true}
    {
        replaceTokenName(node->token);
        node->computeLocation(startLocation, endLocation);
        setup();
    }

    Diagnostic(SourceFile* file, Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level},
        sourceFile{file}
    {
        setup();
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    Diagnostic(Utf8 msg, DiagnosticLevel level = DiagnosticLevel::Error) :
        textMsg{std::move(msg)},
        errorLevel{level}
    {
        setup();
    }

    static constexpr auto ERROR_MESSAGE_SEPARATOR = "$";

    static Diagnostic* note(const Utf8& msg) { return new Diagnostic{msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Token& token, const Utf8& msg) { return new Diagnostic{node, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(AstNode* node, const Utf8& msg) { return new Diagnostic{node, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const Token& token, const Utf8& msg) { return new Diagnostic{file, token, msg, DiagnosticLevel::Note}; }
    static Diagnostic* note(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg) { return new Diagnostic{file, start, end, msg, DiagnosticLevel::Note}; }

    void        setup();
    void        colorizeNoteHeader(Utf8& r) const;
    static Utf8 preprocess(const Utf8& textMsg);
    static Utf8 replaceHighLight(const Utf8& textMsg);
    void        replaceTokenName(const Token& token);
    void        replaceTokenName(const TokenParse& tokenParse);
    bool        containsText(const Utf8& txt) const;
    void        setForceFromContext(bool b) { forceFromContext = b; }
    void        addNote(const SourceLocation& start, const SourceLocation& end, const Utf8& h);
    void        addNote(AstNode* node, const Token& token, const Utf8& msg);
    void        addNote(const AstNode* node, const Utf8& h);
    void        addNote(const Token& token, const Utf8& h);
    void        addNote(const Utf8& msg);
    void        addNote(const char* msg);
    void        addNote(const Diagnostic* note);
    void        addNote(SourceFile* file, const Token& token, const Utf8& msg);
    void        addNote(SourceFile* file, const SourceLocation& start, const SourceLocation& end, const Utf8& msg);
    bool        hasNotes() const { return !notes.empty(); }
    bool        hasSomething() const { return !textMsg.empty() || !preRemarks.empty() || !remarks.empty() || !autoRemarks.empty() || !notes.empty() || !ranges.empty(); }

    void     preprocess();
    void     setupColors();
    void     collectSourceCode();
    void     sortRanges();
    void     collectRanges();
    void     printSourceCode(Log* log, uint32_t num, const Utf8& line) const;
    void     printSourceCode(Log* log) const;
    void     printSourceLine(Log* log) const;
    Utf8     getErrorLevelTitle() const;
    void     printMarginLineNo(Log* log, uint32_t lineNo) const;
    void     printMargin(Log* log, bool eol = false, bool printLineNo = false, uint32_t lineNo = 0) const;
    void     printRemarks(Log* log, const Vector<Utf8>& what, LogColor color) const;
    void     setColorRanges(Log* log, DiagnosticLevel level, HintPart part, LogWriteContext* logCxt = nullptr) const;
    void     alignRangeColumn(Log* log, uint32_t& curColumn, uint32_t where, bool withCode = true) const;
    uint32_t printRangesVerticalBars(Log* log, size_t maxMarks);
    void     printLastRangeHint(Log* log, uint32_t curColumn);
    void     printRanges(Log* log);

    void reportCompact(Log* log);
    void report(Log* log);

    static void        removeErrorId(Utf8& textMsg);
    static bool        hasErrorId(const Utf8& textMsg);
    static Utf8        getErrorId(const Utf8& textMsg);
    static void        tokenizeError(const Utf8& err, Vector<Utf8>& tokens);
    static Utf8        oneLiner(const Utf8& err);
    static Utf8        isType(TypeInfo* typeInfo);
    static Diagnostic* isType(AstNode* node, ToConcreteFlags flags = CONCRETE_FUNC);
    static Diagnostic* hereIs(AstNode* node, const char* msg = nullptr);
    static Diagnostic* hereIs(const SymbolOverload* overload);

    struct RangeHint
    {
        SourceLocation  startLocation;
        SourceLocation  endLocation;
        Utf8            msg;
        DiagnosticLevel errorLevel;
        uint32_t        width     = 0;
        uint32_t        mid       = 0;
        bool            hasBar    = false;
        bool            mergeNext = false;
    };

    Utf8            errorId;
    Utf8            textMsg;
    Vector<Utf8>    preRemarks;
    Vector<Utf8>    remarks;
    Vector<Utf8>    autoRemarks;
    DiagnosticLevel errorLevel;

    Vector<const Diagnostic*> notes;
    Vector<RangeHint>         ranges;
    Utf8                      lineCode;
    Utf8                      lineCodePrev;
    uint32_t                  lineCodeNum       = 0;
    uint32_t                  lineCodeNumPrev   = 0;
    uint32_t                  lineCodeNumDigits = 0;
    uint32_t                  lineCodeMaxDigits = 0;
    uint32_t                  minBlanks         = 0;
    LogColor                  errorColor;
    LogColor                  errorColorHighLight;
    LogColor                  errorColorHint;
    LogColor                  errorColorHintHighLight;
    LogColor                  warningColor;
    LogColor                  warningColorHighLight;
    LogColor                  warningColorHint;
    LogColor                  warningColorHintHighLight;
    LogColor                  noteHeaderColor;
    LogColor                  noteColor;
    LogColor                  noteColorHighLight;
    LogColor                  noteColorHint;
    LogColor                  noteColorHintHighLight;
    LogColor                  marginBorderColor;
    LogColor                  marginBorderColorContext;
    LogColor                  autoRemarkColor;
    LogColor                  sourceFileColor;
    LogColor                  sourceFileColorContext;
    LogColor                  codeLineNoColor;
    LogColor                  codeLineNoColorContext;
    LogColor                  stackColor;

    SourceLocation startLocation;
    SourceLocation endLocation;

    SourceFile* contextFile = nullptr;
    SourceFile* sourceFile  = nullptr;
    AstNode*    sourceNode  = nullptr;
    AstNode*    contextNode = nullptr;

    bool showSourceCode   = false;
    bool showErrorLevel   = true;
    bool showFileName     = true;
    bool forceFromContext = false;

    bool canBeMerged = true;
    bool display     = true;
    bool hasLocation = false;
    bool hasContent  = false;
    bool fromContext = false;
};
