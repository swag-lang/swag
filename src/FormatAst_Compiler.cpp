#include "pch.h"
#include "Ast.h"
#include "FormatAst.h"
#include "Report.h"
#include "Semantic.h"

bool FormatAst::outputCompilerIf(const Utf8& name, const AstNode* node)
{
    const auto compilerIf = castAst<AstIf>(node, AstNodeKind::CompilerIf);
    concat->addString(name);
    concat->addBlank();
    SWAG_CHECK(outputNode(compilerIf->boolExpression));

    if (!compilerIf->ifBlock->childCount() || compilerIf->ifBlock->firstChild()->isNot(AstNodeKind::Statement))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "#do");
        concat->addEol();
        indent++;
        concat->addIndent(indent);
        if (!compilerIf->ifBlock->childCount())
            SWAG_CHECK(outputNode(compilerIf->ifBlock));
        else
            SWAG_CHECK(outputNode(compilerIf->ifBlock->firstChild()));
        indent--;
        concat->addEol();
    }
    else
    {
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(compilerIf->ifBlock->firstChild()));
    }

    if (!compilerIf->elseBlock)
        return true;

    if (!compilerIf->elseBlock->childCount() || compilerIf->elseBlock->firstChild()->is(AstNodeKind::CompilerIf))
    {
        concat->addIndent(indent);
        if (!compilerIf->elseBlock->childCount())
            SWAG_CHECK(outputCompilerIf("#elif", compilerIf->elseBlock));
        else
            SWAG_CHECK(outputCompilerIf("#elif", compilerIf->elseBlock->firstChild()));
    }
    else
    {
        concat->addIndent(indent);
        CONCAT_FIXED_STR(concat, "#else");
        if (compilerIf->elseBlock->firstChild()->isNot(AstNodeKind::Statement))
        {
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "#do");
            concat->addEol();
            indent++;
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(compilerIf->elseBlock->firstChild()));
            indent--;
            concat->addEol();
        }
        else
        {
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(compilerIf->elseBlock->firstChild()));
        }
    }

    return true;
}

bool FormatAst::outputCompilerSpecialValue(const AstNode* node) const
{
    switch (node->token.id)
    {
        case TokenId::CompilerSelf:
            CONCAT_FIXED_STR(concat, "#self");
            break;
        case TokenId::CompilerCallerFunction:
            CONCAT_FIXED_STR(concat, "#callerfunction");
            break;
        case TokenId::CompilerCallerLocation:
            CONCAT_FIXED_STR(concat, "#callerlocation");
            break;
        case TokenId::CompilerLocation:
            CONCAT_FIXED_STR(concat, "#location");
            break;
        case TokenId::CompilerOs:
            CONCAT_FIXED_STR(concat, "#os");
            break;
        case TokenId::CompilerSwagOs:
            CONCAT_FIXED_STR(concat, "#swagos");
            break;
        case TokenId::CompilerArch:
            CONCAT_FIXED_STR(concat, "#arch");
            break;
        case TokenId::CompilerCpu:
            CONCAT_FIXED_STR(concat, "#cpu");
            break;
        case TokenId::CompilerBackend:
            CONCAT_FIXED_STR(concat, "#backend");
            break;
        case TokenId::CompilerBuildCfg:
            CONCAT_FIXED_STR(concat, "#cfg");
            break;
        default:
            Report::internalError(const_cast<AstNode*>(node), "FormatAst::outputNode, unknown compiler function");
    }

    return true;
}

bool FormatAst::outputCompilerMixin(const AstNode* node)
{
    const auto compilerMixin = castAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
    CONCAT_FIXED_STR(concat, "#mixin");
    concat->addBlank();
    SWAG_CHECK(outputNode(node->firstChild()));
    if (!compilerMixin->replaceTokens.empty())
    {
        concat->addBlank();
        concat->addChar('{');
        concat->addBlank();
        for (const auto m : compilerMixin->replaceTokens)
        {
            switch (m.first)
            {
                case TokenId::KwdBreak:
                    CONCAT_FIXED_STR(concat, "break");
                    break;
                case TokenId::KwdContinue:
                    CONCAT_FIXED_STR(concat, "continue");
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }

            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            SWAG_CHECK(outputNode(m.second));
            concat->addChar(';');
            concat->addBlank();
        }
        concat->addChar('}');
    }

    return true;
}

bool FormatAst::outputCompilerExpr(const AstNode* node)
{
    if (node->is(AstNodeKind::CompilerRun) || node->is(AstNodeKind::CompilerRunExpression))
        CONCAT_FIXED_STR(concat, "#run");
    else if (node->is(AstNodeKind::CompilerAst))
        CONCAT_FIXED_STR(concat, "#ast");
    else if (node->is(AstNodeKind::CompilerValidIf))
        CONCAT_FIXED_STR(concat, "#validif");
    else if (node->is(AstNodeKind::CompilerValidIfx))
        CONCAT_FIXED_STR(concat, "#validifx");
    concat->addBlank();

    const auto front = node->firstChild();
    if (front->is(AstNodeKind::FuncDecl))
    {
        const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(front, AstNodeKind::FuncDecl);
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(funcDecl->content));
        concat->addEol();
    }
    else
    {
        SWAG_CHECK(outputNode(front));
        concat->addEol();
    }

    return true;
}
