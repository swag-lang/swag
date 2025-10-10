#include "pch.h"

#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorContext.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/SourceFile.h"

PushErrCxtStep::PushErrCxtStep(ErrorContext*                context,
                               AstNode*                     node,
                               ErrCxtStepKind               kind,
                               const std::function<Utf8()>& err,
                               bool                         locIsToken)
{
    ErrorCxtStep expNode;
    expNode.node       = node;
    expNode.type       = kind;
    expNode.err        = err;
    expNode.locIsToken = locIsToken;
    context->errCxtSteps.push_back(expNode);
    cxt = context;
}

PushErrCxtStep::~PushErrCxtStep()
{
    if (cxt)
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

            // Determine if we have the same location in a previous note
            bool hasSameNode = false;
            if (exp.node &&
                diagnostic.sourceFile == exp.node->token.sourceFile &&
                diagnostic.startLocation.line == exp.node->token.startLocation.line)
            {
                hasSameNode = true;
            }

            for (const auto note : notes)
            {
                if (exp.node &&
                    note->sourceFile == exp.node->token.sourceFile &&
                    note->startLocation.line == exp.node->token.startLocation.line)
                {
                    hasSameNode = true;
                }
            }

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
                    if (hasSameNode || !diagnostic.sourceNode || diagnostic.sourceNode->hasAstFlag(AST_IN_MIXIN))
                        exp.hide = true;
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
                    msg             = form("occurred during the generic instantiation of [[%s]]", name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringInline:
                    msg             = form("occurred during the inline expansion of [[%s]]", name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringCompileTime:
                    msg             = "occurred during compile-time evaluation";
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringSanity:
                    msg             = "occurred during the sanity pass ([[#[Swag.Sanity]]] is enabled)";
                    exp.fromContext = true;
                    break;

                case ErrCxtStepKind::DuringWhere:
                    if (exp.node->is(AstNodeKind::StructDecl))
                        msg = form("occurred during the [[where]] check of the struct [[%s]]", name.cstr());
                    else
                        msg = form("occurred during the [[where]] check of the call to [[%s]]", name.cstr());
                    exp.locIsToken  = true;
                    exp.fromContext = true;
                    break;
                case ErrCxtStepKind::DuringVerify:
                    msg             = form("occurred during the [[verify]] check of the call to [[%s]]", name.cstr());
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
        const auto note = Diagnostic::note(sourceNode->token.sourceFile->fromNode, "occurred in generated code");
        notes.push_back(note);
    }
    else if (diagnostic.sourceFile && diagnostic.sourceFile->hasFlag(FILE_EXTERNAL) && diagnostic.sourceFile->hasFlag(FILE_FROM_AST) && sourceNode)
    {
        const auto note = Diagnostic::note(sourceNode, "occurred in generated code");
        notes.push_back(note);
    }
}

bool ErrorContext::report(const Diagnostic& err, const Vector<const Diagnostic*>& notes)
{
    const auto copyDiag  = const_cast<Diagnostic*>(&err);
    auto       copyNotes = notes;
    extract(*copyDiag, copyNotes);
    return Report::report(*copyDiag, copyNotes);
}

bool ErrorContext::checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue)
{
    if (value <= maxValue)
        return true;
    const Diagnostic err{node, formErr(Err0762, typeOverflow, maxValue)};
    return report(err);
}

bool ErrorContext::overflowError(AstNode* loc, SafetyMsg msgKind, const TypeInfo* type, const void* val0, const void* val1)
{
    if (!loc)
        loc = node;
    SWAG_ASSERT(loc);
    Diagnostic err{loc, loc->token, ByteCodeGen::safetyMsg(msgKind, type)};
    if (type && val0 && val1)
    {
        Utf8 n;
        if (!type->isNativeIntegerSigned())
        {
            switch (type->sizeOf)
            {
                case 1:
                    n = form("operands are [[%u]] and [[%u]]", *static_cast<const uint8_t*>(val0), *static_cast<const uint8_t*>(val1));
                    break;
                case 2:
                    n = form("operands are [[%u]] and [[%u]]", *static_cast<const uint16_t*>(val0), *static_cast<const uint16_t*>(val1));
                    break;
                case 4:
                    n = form("operands are [[%u]] and [[%u]]", *static_cast<const uint32_t*>(val0), *static_cast<const uint32_t*>(val1));
                    break;
                case 8:
                    n = form("operands are [[%llu]] and [[%llu]]", *static_cast<const uint64_t*>(val0), *static_cast<const uint64_t*>(val1));
                    break;
            }
        }
        else
        {
            switch (type->sizeOf)
            {
                case 1:
                    n = form("operands are [[%d]] and [[%d]]", *static_cast<const int8_t*>(val0), *static_cast<const int8_t*>(val1));
                    break;
                case 2:
                    n = form("operands are [[%d]] and [[%d]]", *static_cast<const int16_t*>(val0), *static_cast<const int16_t*>(val1));
                    break;
                case 4:
                    n = form("operands are [[%d]] and [[%d]]", *static_cast<const int32_t*>(val0), *static_cast<const int32_t*>(val1));
                    break;
                case 8:
                    n = form("operands are [[%lld]] and [[%lld]]", *static_cast<const int64_t*>(val0), *static_cast<const int64_t*>(val1));
                    break;
            }
        }

        err.addNote(n);
    }

    return report(err);
}
