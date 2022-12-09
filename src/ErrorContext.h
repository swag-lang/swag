#pragma once
#include "Utf8.h"
struct AstNode;
struct JobContext;
struct TypeInfo;
struct JobContext;
struct Diagnostic;

enum class ErrorContextKind
{
    Generic,
    Inline,
    SelectIf,
    CheckIf,
    Node,
    Export,
    Message,
    Help,
};

struct ErrorContext
{
    AstNode*             node = nullptr;
    ErrorContextKind     type = ErrorContextKind::Node;
    Utf8                 msg  = "";
    Utf8                 hint = "";
    map<Utf8, TypeInfo*> replaceTypes;
    bool                 hide = false;

    static void fillContext(JobContext* context, const Diagnostic& diag, vector<const Diagnostic*>& notes);
};

struct PushErrContext
{
    PushErrContext(JobContext* context, AstNode* node, ErrorContextKind type);
    PushErrContext(JobContext* context, AstNode* node, const Utf8& msg, const Utf8& hint, ErrorContextKind kind = ErrorContextKind::Message);
    ~PushErrContext();
    JobContext* cxt;
};
