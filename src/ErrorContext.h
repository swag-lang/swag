#pragma once
#include "Utf8.h"
#include "Map.h"
struct AstNode;
struct JobContext;
struct TypeInfo;
struct ErrorContext;
struct Diagnostic;
struct SourceFile;

enum class ErrCxtStepKind
{
    Note,
    Help,
    Generic,
    Inline,
    CompileTime,
    ValidIf,
    ValidIfx,
    Export,
    Hint2,
    HereIs,
    MsgPrio,
};

struct ErrorCxtStep
{
    AstNode*                   node = nullptr;
    ErrCxtStepKind             type = ErrCxtStepKind::Note;
    VectorMap<Utf8, TypeInfo*> replaceTypes;
    bool                       locIsToken = false;
    bool                       hide       = false;
    function<Utf8()>           err;
};

struct PushErrCxtStep
{
    PushErrCxtStep(ErrorContext* context, AstNode* node, ErrCxtStepKind kind, function<Utf8()> err, bool locIsToken = false);
    ~PushErrCxtStep();
    ErrorContext* cxt;
};

struct ErrorContext
{
    void extract(Diagnostic& diag, Vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& diag, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& diag, const Vector<const Diagnostic*>& notes);
    bool checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue);

    void reset()
    {
        errCxtSteps.clear();
        sourceFile  = nullptr;
        node        = nullptr;
        hasError    = false;
        silentError = 0;
    }

    Vector<ErrorCxtStep> errCxtSteps;

    SourceFile* sourceFile = nullptr;
    AstNode*    node       = nullptr;

    uint32_t silentError = false;

    bool hasError = false;
};