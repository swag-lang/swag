#include "pch.h"
#include "ErrorContext.h"
#include "Diagnostic.h"
#include "Job.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "SourceFile.h"
#include "TypeInfo.h"

PushErrContext::PushErrContext(JobContext* context, AstNode* node, ErrorContextKind type)
    : cxt{context}
{
    ErrorContext expNode;
    expNode.type = type;
    expNode.node = node;
    context->errorContextStack.push_back(expNode);
}

PushErrContext::PushErrContext(JobContext* context, AstNode* node, const Utf8& msg, const Utf8& hint, ErrorContextKind kind)
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

            case ErrorContextKind::Message:
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

            auto        first       = exp.node;
            const char* kindName    = nullptr;
            const char* kindArticle = "";
            bool        showContext = true;
            Utf8        hint        = exp.hint;
            switch (exp.type)
            {
            case ErrorContextKind::Message:
            {
                showContext = false;
                if (first)
                {
                    auto note  = new Diagnostic{first, first->token, exp.msg, DiagnosticLevel::Note};
                    note->hint = exp.hint;
                    notes.push_back(note);
                }
                else
                {
                    auto note  = new Diagnostic{exp.msg, DiagnosticLevel::Note};
                    note->hint = exp.hint;
                    notes.push_back(note);
                }
                break;
            }
            case ErrorContextKind::Help:
            {
                showContext = false;
                if (first)
                {
                    auto note  = new Diagnostic{first, first->token, exp.msg, DiagnosticLevel::Help};
                    note->hint = exp.hint;
                    notes.push_back(note);
                }
                else
                {
                    auto note  = new Diagnostic{exp.msg, DiagnosticLevel::Help};
                    note->hint = exp.hint;
                    notes.push_back(note);
                }
                break;
            }
            case ErrorContextKind::Export:
                kindName    = Err(Err0111);
                kindArticle = "of ";
                break;
            case ErrorContextKind::Generic:
                kindName    = Err(Err0112);
                kindArticle = "of ";
                break;
            case ErrorContextKind::Inline:
                kindName    = Err(Err0118);
                kindArticle = "of ";
                break;
            case ErrorContextKind::SelectIf:
                kindName    = Err(Err0128);
                kindArticle = "to ";
                break;
            case ErrorContextKind::CheckIf:
                kindName    = Err(Err0129);
                kindArticle = "to ";
                break;
            case ErrorContextKind::Node:
                kindName    = "when solving";
                kindArticle = "";
                switch (first->kind)
                {
                case AstNodeKind::AffectOp:
                    kindName    = "when solving affectation";
                    kindArticle = "to ";
                    first       = first->childs.front();
                    hint        = Hint::isType(first->typeInfo);
                    break;
                case AstNodeKind::Return:
                {
                    auto returnNode = CastAst<AstReturn>(first, AstNodeKind::Return);
                    if (returnNode->resolvedFuncDecl)
                    {
                        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(returnNode->resolvedFuncDecl->typeInfo, TypeInfoKind::FuncAttr);
                        first         = returnNode->resolvedFuncDecl->returnType;
                        if (!first->childs.empty())
                            first = first->childs.front();
                        auto note = new Diagnostic{first, Fmt(Nte(Nte0067), typeFunc->returnType->getDisplayNameC()), DiagnosticLevel::Note};
                        notes.push_back(note);
                        showContext = false;
                    }
                    else
                        showContext = false;
                }
                break;
                default:
                    showContext = false;
                    break;
                }

                break;
            }

            if (showContext)
            {
                auto name = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->token.text;
                if (name.empty())
                    name = first->token.text;

                Utf8 msg;
                if (!name.empty())
                    msg = Fmt(Nte(Nte0002), kindName, kindArticle, name.c_str());
                else
                    msg = Fmt(Nte(Nte0003), kindName);
                auto note  = new Diagnostic{first, first->token, msg, DiagnosticLevel::Note};
                note->hint = hint;

                auto remark = Ast::computeGenericParametersReplacement(exp.replaceTypes);
                if (!remark.empty())
                    note->remarks.insert(note->remarks.end(), remark.begin(), remark.end());

                notes.push_back(note);
            }
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
