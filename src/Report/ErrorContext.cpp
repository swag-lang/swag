#include "pch.h"
#include "Report/ErrorContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Wmf/SourceFile.h"

PushErrCxtStep::PushErrCxtStep(ErrorContext*                context,
                               AstNode*                     node,
                               ErrCxtStepKind               kind,
                               const std::function<Utf8()>& err,
                               bool                         locIsToken) :
    cxt{context}
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

void ErrorContext::extract(Diagnostic& diagnostic, Vector<const Diagnostic*>& notes)
{
    diagnostic.contextNode = node;

    if (diagnostic.errorLevel == DiagnosticLevel::Error)
        hasError = true;

    if (!errCxtSteps.empty())
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
                case ErrCxtStepKind::Error:
                    if (!msg.empty())
                        diagnostic.textMsg = msg;
                    exp.hide = true;
                    break;

                case ErrCxtStepKind::DuringGeneric:
                    if (exp.node && exp.node->is(AstNodeKind::VarDecl)) // Can happen with automatic call of opIndexSuffix
                    {
                        exp.hide = true;
                    }
                    else
                    {
                        exp.hide    = doneGeneric;
                        doneGeneric = true;
                    }
                    break;

                case ErrCxtStepKind::DuringInline:
                    exp.hide   = doneInline;
                    doneInline = true;
                    break;

                case ErrCxtStepKind::DuringCompileTime:
                    exp.hide     = doneCompTime;
                    doneCompTime = true;
                    break;

                case ErrCxtStepKind::Note:
                    exp.hide = msg.empty();
                    break;

                default:
                    break;
            }
        }

        for (uint32_t i = errCxtSteps.size() - 1; i != UINT32_MAX; i--)
        {
            auto& exp = errCxtSteps[i];
            if (exp.hide)
                continue;

            constexpr auto level = DiagnosticLevel::Note;

            Utf8 msg;
            if (exp.err)
                msg = exp.err();

            Utf8 name;
            if (exp.node)
            {
                const auto overload = exp.node->resolvedSymbolOverload();
                const auto sym      = exp.node->resolvedSymbolName();

                if (overload && overload->node->ownerStructScope)
                    name = form("%s.%s", overload->node->ownerStructScope->name.cstr(), overload->symbol->name.cstr());
                else if (sym)
                    name = sym->name;
                else
                    name = exp.node->token.text;
            }

            Diagnostic* note = nullptr;
            switch (exp.type)
            {
                case ErrCxtStepKind::Note:
                    break;
                case ErrCxtStepKind::DuringGeneric:
                    msg             = formNte(Nte0093, name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringInline:
                    msg             = formNte(Nte0094, name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringCompileTime:
                    msg             = formNte(Nte0089, name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringWhere:
                    if (exp.node->is(AstNodeKind::StructDecl))
                        msg = formNte(Nte0092, name.cstr());
                    else
                        msg = formNte(Nte0091, name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringVerify:
                    msg             = formNte(Nte0090, name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::HereIs:
                {
                    ScopedLock lk(exp.node->mutex);
                    note = Diagnostic::hereIs(exp.node->resolvedSymbolOverload());
                    if (!note)
                        continue;
                    break;
                }
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
                note->fromContext = exp.fromContext;
            }

            notes.push_back(note);
        }
    }

    // From generated code
    auto       sourceNode = diagnostic.sourceNode ? diagnostic.sourceNode : diagnostic.contextNode;
    const auto exportNode = sourceNode ? sourceNode->extraPointer<AstNode>(ExtraPointerKind::ExportNode) : nullptr;
    if (exportNode)
        sourceNode = exportNode;
    if (sourceNode && sourceNode->token.sourceFile && sourceNode->token.sourceFile->fromNode && !sourceNode->token.sourceFile->fileForSourceLocation)
    {
        const auto note = Diagnostic::note(sourceNode->token.sourceFile->fromNode, toNte(Nte0095));
        notes.push_back(note);
    }
    else if (diagnostic.sourceFile && diagnostic.sourceFile->hasFlag(FILE_EXTERNAL) && diagnostic.sourceFile->hasFlag(FILE_FROM_AST) && sourceNode)
    {
        const auto note = Diagnostic::note(sourceNode, toNte(Nte0095));
        notes.push_back(note);
    }
}

bool ErrorContext::report(const Diagnostic& err, const Vector<const Diagnostic*>& notes)
{
    auto copyDiag  = err;
    auto copyNotes = notes;

    for (auto note : err.notes)
        copyNotes.push_back(note);
    copyDiag.notes.clear();

    extract(copyDiag, copyNotes);

    return Report::report(copyDiag, copyNotes);
}

bool ErrorContext::report(const Diagnostic& err, const Diagnostic* note, const Diagnostic* note1)
{
    Vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    return report(err, notes);
}

bool ErrorContext::checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue)
{
    if (value <= maxValue)
        return true;
    return report({node, formErr(Err0762, typeOverflow, maxValue)});
}
