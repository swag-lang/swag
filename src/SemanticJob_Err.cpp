#include "pch.h"
#include "SemanticJob.h"

bool SemanticJob::checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    if (test)
        return true;

    if (expression->hasSpecialFuncCall())
    {
        Diagnostic diag{expression, expression->token, Fmt(Err(Err0281), expression->typeInfo->getDisplayNameC())};
        diag.hint = Fmt(Nte(Nte1047), expression->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str());
        return context->report(diag, computeNonConstExprNote(expression));
    }

    if (errMsg.length() && errParam.length())
    {
        Diagnostic diag{expression, Fmt(errMsg.c_str(), errParam.c_str())};
        return context->report(diag, computeNonConstExprNote(expression));
    }

    Diagnostic diag{expression, errMsg.length() ? errMsg : Err(Err0798)};
    return context->report(diag, computeNonConstExprNote(expression));
}

bool SemanticJob::checkIsConstExpr(JobContext* context, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    return checkIsConstExpr(context, expression->flags & AST_CONST_EXPR, expression, errMsg, errParam);
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right)
{
    if (leftTypeInfo->isNative() && rightTypeInfo->isNative())
        return true;
    auto node = context->node;

    if (!leftTypeInfo->isNative())
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Nte(Nte1061);
        diag.addRange(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }
    else
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0005), node->token.ctext(), rightTypeInfo->getDisplayNameC())};
        diag.hint = Nte(Nte1061);
        diag.addRange(right, Diagnostic::isType(rightTypeInfo));
        return context->report(diag);
    }
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->isNative(), notAllowedError(context, node, typeInfo));
    return true;
}

bool SemanticJob::notAllowedError(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = Fmt(Err(Err0005), node->token.ctext(), typeInfo->getDisplayNameC());
    if (msg)
    {
        text += " ";
        text += msg;
    }

    Diagnostic diag{node, node->token, text};
    diag.hint = Nte(Nte1061);
    if (hintType)
        diag.addRange(hintType, Diagnostic::isType(typeInfo));
    return context->report(diag);
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->report({context->node, msg});
    return false;
}
