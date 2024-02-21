#include "pch.h"
#include "ErrorContext.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Report.h"
#include "Scope.h"
#include "SourceFile.h"

PushErrCxtStep::PushErrCxtStep(ErrorContext* context, AstNode* node, ErrCxtStepKind kind, const function<Utf8()>& err, bool locIsToken) :
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
                    exp.hide = msg.empty();
                    break;

                default:
                    break;
            }
        }

        for (int i = static_cast<int>(errCxtSteps.size()) - 1; i >= 0; i--)
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
                if (exp.node->resolvedSymbolOverload() && exp.node->resolvedSymbolOverload()->node->ownerStructScope)
                    name = FMT("%s.%s", exp.node->resolvedSymbolOverload()->node->ownerStructScope->name.c_str(), exp.node->resolvedSymbolOverload()->symbol->name.c_str());
                else if (exp.node->resolvedSymbolName())
                    name = exp.node->resolvedSymbolName()->name;
                else
                    name = exp.node->token.text;
            }

            const Diagnostic* note = nullptr;
            switch (exp.type)
            {
                case ErrCxtStepKind::Note:
                    break;
                case ErrCxtStepKind::Export:
                    msg = FMT(Nte(Nte0097), name.c_str());
                    break;
                case ErrCxtStepKind::Generic:
                    msg            = FMT(Nte(Nte0095), name.c_str());
                    exp.locIsToken = true;
                    break;
                case ErrCxtStepKind::Inline:
                    msg            = FMT(Nte(Nte0096), name.c_str());
                    exp.locIsToken = true;
                    break;
                case ErrCxtStepKind::CompileTime:
                    msg            = FMT(Nte(Nte0091), name.c_str());
                    exp.locIsToken = true;
                    break;
                case ErrCxtStepKind::ValidIf:
                    if (exp.node->kind == AstNodeKind::StructDecl)
                        msg = FMT(Nte(Nte0092), name.c_str());
                    else
                        msg = FMT(Nte(Nte0093), name.c_str());
                    exp.locIsToken = true;
                    break;
                case ErrCxtStepKind::ValidIfx:
                    msg            = FMT(Nte(Nte0094), name.c_str());
                    exp.locIsToken = true;
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
            }

            notes.push_back(note);
        }
    }

    // From generated code
    auto sourceNode = diagnostic.sourceNode ? diagnostic.sourceNode : diagnostic.contextNode;
    if (sourceNode && sourceNode->hasExtraPointer(ExtraPointerKind::ExportNode))
        sourceNode = sourceNode->extraPointer<AstNode>(ExtraPointerKind::ExportNode);
    if (sourceNode && sourceNode->sourceFile && sourceNode->sourceFile->fromNode && !sourceNode->sourceFile->fileForSourceLocation)
    {
        const auto note = Diagnostic::note(sourceNode->sourceFile->fromNode, Nte(Nte0098));
        notes.push_back(note);
    }
    else if (diagnostic.sourceFile && diagnostic.sourceFile->hasFlag(FILE_IS_EXTERNAL) && diagnostic.sourceFile->hasFlag(FILE_IS_FROM_AST) && sourceNode)
    {
        const auto note = Diagnostic::note(sourceNode, Nte(Nte0098));
        notes.push_back(note);
    }
}

bool ErrorContext::report(const Diagnostic& err, const Vector<const Diagnostic*>& notes)
{
    if (silentError)
        return false;

    auto copyDiag  = err;
    auto copyNotes = notes;
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
    return report({node, FMT(Err(Err0046), typeOverflow, maxValue)});
}
