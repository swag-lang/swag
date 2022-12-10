#include "pch.h"
#include "ErrorContext.h"
#include "Diagnostic.h"
#include "Job.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "SourceFile.h"
#include "TypeInfo.h"

PushErrContext::PushErrContext(JobContext* context, AstNode* node, ErrorContextKind kind, const Utf8& msg, const Utf8& hint)
    : cxt{context}
{
    ErrorContext expNode;
    expNode.node = node;
    expNode.type = kind;
    expNode.msg  = msg;
    expNode.hint = hint;
    context->errorContextStack.push_back(expNode);
}

PushErrContext::~PushErrContext()
{
    cxt->errorContextStack.pop_back();
}

void ErrorContext::fillContext(JobContext* context, const Diagnostic& diag, vector<const Diagnostic*>& notes)
{
    if (diag.errorLevel == DiagnosticLevel::Error)
        context->hasError = true;

    if (context->errorContextStack.size())
    {
        bool doneGeneric = false;
        bool doneInline  = false;

        for (auto& exp : context->errorContextStack)
        {
            switch (exp.type)
            {
            case ErrorContextKind::Generic:
                if (exp.node && exp.node->kind == AstNodeKind::VarDecl) // Can happen with automatic call of opIndexSuffix
                {
                    exp.hide = true;
                }
                else
                {
                    exp.hide    = doneGeneric;
                    doneGeneric = true;
                }
                break;

            case ErrorContextKind::Inline:
                exp.hide   = doneInline;
                doneInline = true;
                break;

            case ErrorContextKind::Note:
            case ErrorContextKind::Help:
                exp.hide = exp.msg.empty();
                break;
            }
        }

        for (int i = (int) context->errorContextStack.size() - 1; i >= 0; i--)
        {
            auto& exp = context->errorContextStack[i];
            if (exp.hide)
                continue;

            DiagnosticLevel level = DiagnosticLevel::Note;

            Utf8 name, msg;
            if (exp.node)
                name = exp.node->resolvedSymbolName ? exp.node->resolvedSymbolName->name : exp.node->token.text;

            switch (exp.type)
            {
            case ErrorContextKind::Note:
                msg = exp.msg;
                break;
            case ErrorContextKind::Help:
                msg = exp.msg;
                level = DiagnosticLevel::Help;
                break;
            case ErrorContextKind::Export:
                msg = Fmt(Err(Err0111), name.c_str());
                break;
            case ErrorContextKind::Generic:
                msg = Fmt(Err(Err0112), name.c_str());
                break;
            case ErrorContextKind::Inline:
                msg = Fmt(Err(Err0118), name.c_str());
                break;
            case ErrorContextKind::SelectIf:
                msg = Fmt(Err(Err0128), name.c_str());
                break;
            case ErrorContextKind::CheckIf:
                msg = Fmt(Err(Err0129), name.c_str());
                break;
            }

            Diagnostic* note = nullptr;
            if (exp.node)
                note = new Diagnostic{exp.node, exp.node->token, msg, level};
            else
                note = new Diagnostic{msg, level};

            note->hint = exp.hint;

            auto remark = Ast::computeGenericParametersReplacement(exp.replaceTypes);
            if (!remark.empty())
                note->remarks.insert(note->remarks.end(), remark.begin(), remark.end());

            notes.push_back(note);
        }
    }

    // Generated code
    auto sourceNode = diag.sourceNode;
    if (sourceNode && sourceNode->extension && sourceNode->extension->misc && sourceNode->extension->misc->exportNode)
        sourceNode = diag.sourceNode->extension->misc->exportNode;
    if (sourceNode && sourceNode->sourceFile && sourceNode->sourceFile->sourceNode && !sourceNode->sourceFile->fileForSourceLocation)
    {
        auto fileSourceNode = sourceNode->sourceFile->sourceNode;
        auto note           = new Diagnostic{fileSourceNode, Nte(Nte0004), DiagnosticLevel::Note};
        notes.push_back(note);
    }
}
