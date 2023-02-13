#pragma once
#include "Utf8.h"
struct AstNode;
struct JobContext;
struct TypeInfo;
struct JobContext;
struct Diagnostic;

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
    AstNode*             node = nullptr;
    ErrCxtStepKind       type = ErrCxtStepKind::Note;
    Utf8                 msg  = "";
    Utf8                 hint = "";
    map<Utf8, TypeInfo*> replaceTypes;
    bool                 locIsToken = false;
    bool                 hide       = false;
};

struct PushErrCxtStep
{
    PushErrCxtStep(JobContext* context, AstNode* node, ErrCxtStepKind kind, const Utf8& msg = "", const Utf8& hint = "", bool locIsToken = false);
    ~PushErrCxtStep();
    JobContext* cxt;
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
        node        = nullptr;
        hasError    = false;
        silentError = 0;
    }

    Vector<ErrorCxtStep> errCxtSteps;
    AstNode*             node        = nullptr;
    bool                 hasError    = false;
    uint32_t             silentError = false;
};