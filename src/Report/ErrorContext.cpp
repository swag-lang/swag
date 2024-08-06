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
                case ErrCxtStepKind::Generic:
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
                    name = form("%s.%s", overload->node->ownerStructScope->name.c_str(), overload->symbol->name.c_str());
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
                case ErrCxtStepKind::Generic:
                    msg             = formNte(Nte0101, name.c_str());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::Inline:
                    msg             = formNte(Nte0102, name.c_str());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::CompileTime:
                    msg             = formNte(Nte0097, name.c_str());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::Where:
                    if (exp.node->is(AstNodeKind::StructDecl))
                        msg = formNte(Nte0099, name.c_str());
                    else
                        msg = formNte(Nte0100, name.c_str());
                    exp.locIsToken = true;
                    break;
                case ErrCxtStepKind::WhereCall:
                    msg            = formNte(Nte0098, name.c_str());
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
        const auto note = Diagnostic::note(sourceNode->token.sourceFile->fromNode, toNte(Nte0103));
        notes.push_back(note);
    }
    else if (diagnostic.sourceFile && diagnostic.sourceFile->hasFlag(FILE_EXTERNAL) && diagnostic.sourceFile->hasFlag(FILE_FROM_AST) && sourceNode)
    {
        const auto note = Diagnostic::note(sourceNode, toNte(Nte0103));
        notes.push_back(note);
    }
}

bool ErrorContext::report(const Diagnostic& err, const Vector<const Diagnostic*>& notes)
{
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
    return report({node, formErr(Err0559, typeOverflow, maxValue)});
}
