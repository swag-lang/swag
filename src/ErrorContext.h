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
    SelectIf,
    CheckIf,
    Export,
};

struct ErrorContext
{
    AstNode*             node = nullptr;
    ErrorContextKind     type = ErrorContextKind::Note;
    Utf8                 msg  = "";
    Utf8                 hint = "";
    map<Utf8, TypeInfo*> replaceTypes;
    bool                 hide = false;

    static void fillContext(JobContext* context, const Diagnostic& diag, vector<const Diagnostic*>& notes);
};

struct PushErrContext
{
    PushErrContext(JobContext* context, AstNode* node, ErrorContextKind kind, const Utf8& msg = "", const Utf8& hint = "");
    ~PushErrContext();
    JobContext* cxt;
};
