#include "pch.h"
#include "ErrorContext.h"
#include "Diagnostic.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "Report.h"

PushErrCxtStep::PushErrCxtStep(ErrorContext* context, AstNode* node, ErrCxtStepKind kind, function<Utf8()> err, bool locIsToken)
    : cxt{context}
{
    ErrorCxtStep expNode;
    expNode.node       = node;
    expNode.type       = kind;
    expNode.err        = err;
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
            Utf8 msg;
            if (exp.err)
                msg = exp.err();

            switch (exp.type)
            {
            case ErrCxtStepKind::MsgPrio:
                if (diag.lowPrio)
                    diag.textMsg = msg;
                exp.hide = true;
                break;

            case ErrCxtStepKind::Hint2:
                exp.hide = true;
                if (exp.node)
                {
                    SourceLocation start, end;
                    exp.node->computeLocation(start, end);
                    if (start.line == end.line && start.line == diag.startLocation.line)
                        diag.addRange(start, end, msg);
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
                exp.hide = msg.empty();
                break;

            default:
                break;
            }
        }

        for (int i = (int) errCxtSteps.size() - 1; i >= 0; i--)
        {
            auto& exp = errCxtSteps[i];
            if (exp.hide)
                continue;

            DiagnosticLevel level = DiagnosticLevel::Note;

            Utf8 msg;
            if (exp.err)
                msg = exp.err();

            Utf8 name;
            if (exp.node)
                name = exp.node->resolvedSymbolName ? exp.node->resolvedSymbolName->name : exp.node->token.text;

            Diagnostic* note = nullptr;
            switch (exp.type)
            {
            case ErrCxtStepKind::Note:
                break;
            case ErrCxtStepKind::Help:
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
            default:
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

            notes.push_back(note);
        }
    }

    // Generated code
    auto sourceNode = diag.sourceNode;
    if (sourceNode && sourceNode->hasExtMisc() && sourceNode->extMisc()->exportNode)
        sourceNode = diag.sourceNode->extMisc()->exportNode;
    if (sourceNode && sourceNode->sourceFile && sourceNode->sourceFile->sourceNode && !sourceNode->sourceFile->fileForSourceLocation)
    {
        auto fileSourceNode = sourceNode->sourceFile->sourceNode;
        auto note           = Diagnostic::note(fileSourceNode, Nte(Nte0004));
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