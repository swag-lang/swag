#pragma once
#include "Utf8.h"
struct AstNode;
struct JobContext;
struct TypeInfo;
struct JobContext;
struct Diagnostic;

enum class ErrorContextKind
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

struct ErrorContext
{
    AstNode*             node = nullptr;
    ErrorContextKind     type = ErrorContextKind::Note;
    Utf8                 msg  = "";
    Utf8                 hint = "";
    map<Utf8, TypeInfo*> replaceTypes;
    bool                 locIsToken = false;
    bool                 hide       = false;

    static void fillContext(JobContext* context, Diagnostic& diag, Vector<const Diagnostic*>& notes);
};

struct PushErrContext
{
    PushErrContext(JobContext* context, AstNode* node, ErrorContextKind kind, const Utf8& msg = "", const Utf8& hint = "", bool locIsToken = false);
    ~PushErrContext();
    JobContext* cxt;
};
