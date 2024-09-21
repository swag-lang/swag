#pragma once
#include "Semantic/Generic/Generic.h"

struct AstNode;
struct JobContext;
struct TypeInfo;
struct ErrorContext;
struct Diagnostic;
struct GenericReplaceType;
struct SourceFile;

enum class ErrCxtStepKind
{
    Error,
    Note,
    HereIs,
    DuringGeneric,
    DuringInline,
    DuringCompileTime,
    DuringWhere,
    DuringVerify,
};

struct ErrorCxtStep
{
    VectorMap<Utf8, GenericReplaceType> replaceTypes;
    std::function<Utf8()>               err;
    AstNode*                            node        = nullptr;
    ErrCxtStepKind                      type        = ErrCxtStepKind::Note;
    bool                                locIsToken  = false;
    bool                                fromContext = false;
    bool                                hide        = false;
};

struct PushErrCxtStep
{
                  PushErrCxtStep(ErrorContext* context, AstNode* node, ErrCxtStepKind kind, const std::function<Utf8()>& err, bool locIsToken = false);
    ~             PushErrCxtStep();
    ErrorContext* cxt;
};

struct ErrorContext
{
    void extract(Diagnostic& diagnostic, Vector<const Diagnostic*>& notes);
    bool report(const Diagnostic& err, const Diagnostic* note = nullptr, const Diagnostic* note1 = nullptr);
    bool report(const Diagnostic& err, const Vector<const Diagnostic*>& notes);
    bool checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue);

    void reset()
    {
        errCxtSteps.clear();
        sourceFile = nullptr;
        node       = nullptr;
        hasError   = false;
    }

    Vector<ErrorCxtStep> errCxtSteps;

    SourceFile* sourceFile = nullptr;
    AstNode*    node       = nullptr;

    bool hasError = false;
};
