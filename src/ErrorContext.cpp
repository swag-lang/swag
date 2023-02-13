#include "pch.h"
#include "ErrorContext.h"
#include "Diagnostic.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"

PushErrCxtStep::PushErrCxtStep(JobContext* context, AstNode* node, ErrCxtStepKind kind, const Utf8& msg, const Utf8& hint, bool locIsToken)
    : cxt{context}
{
    ErrorCxtStep expNode;
    expNode.node       = node;
    expNode.type       = kind;
    expNode.msg        = msg;
    expNode.hint       = hint;
    expNode.locIsToken = locIsToken;
    context->errCxtSteps.push_back(expNode);
}

PushErrCxtStep::~PushErrCxtStep()
{
    cxt->errCxtSteps.pop_back();
}

void ErrorContext::extract(Diagnostic& diag, Vector<const Diagnostic*>& notes)
{
    diag.raisedOnNode = node;

    if (diag.errorLevel == DiagnosticLevel::Error)
        hasError = true;

    if (errCxtSteps.size())
    {
        bool doneGeneric  = false;
        bool doneInline   = false;
        bool doneCompTime = false;

        for (auto& exp : errCxtSteps)
        {
            switch (exp.type)
            {
            case ErrCxtStepKind::MsgPrio:
                if (diag.lowPrio)
                    diag.textMsg = exp.msg;
                exp.hide = true;
                break;

            case ErrCxtStepKind::Hint2:
                exp.hide = true;
                if (exp.node)
                {
                    SourceLocation start, end;
                    exp.node->computeLocation(start, end);
                    if (start.line == end.line && start.line == diag.startLocation.line)
                        diag.addRange(start, end, exp.hint);
                }

                break;

            case ErrCxtStepKind::Generic:
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

            case ErrCxtStepKind::Inline:
                exp.hide   = doneInline;
                doneInline = true;
                break;
            case ErrCxtStepKind::CompileTime:
                exp.hide     = doneCompTime;
                doneCompTime = true;
                break;

            case ErrCxtStepKind::Note:
            case ErrCxtStepKind::Help:
                exp.hide = exp.msg.empty();
                break;
            }
        }

        for (int i = (int) errCxtSteps.size() - 1; i >= 0; i--)
        {
            auto& exp = errCxtSteps[i];
            if (exp.hide)
                continue;

            DiagnosticLevel level = DiagnosticLevel::Note;

            Utf8 name, msg;
            if (exp.node)
                name = exp.node->resolvedSymbolName ? exp.node->resolvedSymbolName->name : exp.node->token.text;

            Diagnostic* note = nullptr;
            switch (exp.type)
            {
            case ErrCxtStepKind::Note:
                msg = exp.msg;
                break;
            case ErrCxtStepKind::Help:
                msg   = exp.msg;
                level = DiagnosticLevel::Help;
                break;
            case ErrCxtStepKind::Export:
                msg = Fmt(Nte(Nte0060), name.c_str());
                break;
            case ErrCxtStepKind::Generic:
                msg            = Fmt(Nte(Nte0061), name.c_str());
                exp.locIsToken = true;
                break;
            case ErrCxtStepKind::Inline:
                msg            = Fmt(Nte(Nte0059), name.c_str());
                exp.locIsToken = true;
                break;
            case ErrCxtStepKind::CompileTime:
                msg            = Fmt(Nte(Nte0072), name.c_str());
                exp.locIsToken = true;
                break;
            case ErrCxtStepKind::ValidIf:
                if (exp.node->kind == AstNodeKind::StructDecl)
                    msg = Fmt(Nte(Nte0078), name.c_str());
                else
                    msg = Fmt(Nte(Nte0054), name.c_str());
                exp.locIsToken = true;
                break;
            case ErrCxtStepKind::ValidIfx:
                msg            = Fmt(Nte(Nte0033), name.c_str());
                exp.locIsToken = true;
                break;
            case ErrCxtStepKind::HereIs:
                note = Diagnostic::hereIs(exp.node->resolvedSymbolOverload);
                if (!note)
                    continue;
                break;
            }

            if (!note)
            {
                if (exp.node && exp.locIsToken)
                    note = new Diagnostic{exp.node, exp.node->token, msg, level};
                else if (exp.node)
                    note = new Diagnostic{exp.node, msg, level};
                else
                    note = new Diagnostic{msg, level};
            }

            note->hint = exp.hint;
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

bool ErrorContext::report(const Diagnostic& diag, const Vector<const Diagnostic*>& notes)
{
    if (silentError)
        return false;

    auto copyDiag  = diag;
    auto copyNotes = notes;
    extract(copyDiag, copyNotes);
    return Report::report(copyDiag, copyNotes);
}

bool ErrorContext::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    Vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    return report(diag, notes);
}

bool ErrorContext::checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue)
{
    if (value <= maxValue)
        return true;
    return report({node, Fmt(Err(Err0505), typeOverflow, maxValue)});
}