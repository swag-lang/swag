#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"

bool FormatAst::outputCompilerIf(FormatContext& context, const Utf8& name, AstNode* node)
{
    const auto ifNode = castAst<AstIf>(node, AstNodeKind::CompilerIf);

    if (ifNode->hasAstFlag(AST_GLOBAL_NODE))
    {
        CONCAT_FIXED_STR(concat, "#global");
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "#if");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, ifNode->boolExpression));
        concat->addEol();
        SWAG_CHECK(outputChildren(context, ifNode->ifBlock->firstChild()));
        return true;
    }

    concat->addString(name);
    concat->addBlank();
    SWAG_CHECK(outputNode(context, ifNode->boolExpression));

    if (!ifNode->ifBlock->childCount())
    {
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, ifNode->ifBlock));
    }
    else if ((ifNode->ifBlock->firstChild()->is(AstNodeKind::Statement) && !ifNode->ifBlock->firstChild()->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY)) ||
             ifNode->ifBlock->firstChild()->isNot(AstNodeKind::Statement))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "#do");
        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, ifNode->ifBlock->firstChild()));
        context.indent--;
        concat->addEol();
    }
    else
    {
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, ifNode->ifBlock->firstChild()));
    }

    if (!ifNode->elseBlock)
        return true;

    if (!ifNode->elseBlock->childCount())
    {
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, ifNode->elseBlock));
    }
    else if (ifNode->elseBlock->firstChild()->is(AstNodeKind::CompilerIf))
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputCompilerIf(context, "#elif", ifNode->elseBlock->firstChild()));
    }
    else
    {
        concat->addIndent(context.indent);
        CONCAT_FIXED_STR(concat, "#else");
        if ((ifNode->elseBlock->firstChild()->is(AstNodeKind::Statement) && !ifNode->elseBlock->firstChild()->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY)) ||
            ifNode->elseBlock->firstChild()->isNot(AstNodeKind::Statement))
        {
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "#do");
            concat->addEol();
            context.indent++;
            concat->addIndent(context.indent);
            SWAG_CHECK(outputNode(context, ifNode->elseBlock->firstChild()));
            context.indent--;
            concat->addEol();
        }
        else
        {
            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputNode(context, ifNode->elseBlock->firstChild()));
        }
    }

    return true;
}

bool FormatAst::outputCompilerSpecialValue(FormatContext& context, AstNode* node) const
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

bool FormatAst::outputCompilerMixin(FormatContext& context, AstNode* node)
{
    const auto compilerMixin = castAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
    CONCAT_FIXED_STR(concat, "#mixin");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
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
            SWAG_CHECK(outputNode(context, m.second));
            concat->addChar(';');
            concat->addBlank();
        }
        concat->addChar('}');
    }

    return true;
}

bool FormatAst::outputCompilerExpr(FormatContext& context, const AstNode* node)
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
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, funcDecl->content));
    }
    else
    {
        SWAG_CHECK(outputNode(context, front));
    }

    concat->addEol();
    return true;
}

bool FormatAst::outputCompilerExport(FormatContext& context, AstNode* node) const
{
    const auto decl = castAst<AstCompilerImport>(node, AstNodeKind::CompilerImport);
    CONCAT_FIXED_STR(concat, "#import");
    concat->addBlank();
    concat->addChar('"');
    concat->addString(node->token.text);
    concat->addChar('"');

    if (!decl->tokenLocation.text.empty())
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "location");
        concat->addChar('=');
        concat->addChar('"');
        concat->addString(decl->tokenLocation.text);
        concat->addChar('"');
    }

    if (!decl->tokenVersion.text.empty())
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "version");
        concat->addChar('=');
        concat->addChar('"');
        concat->addString(decl->tokenVersion.text);
        concat->addChar('"');
    }

    concat->addEol();
    return true;
}

bool FormatAst::outputCompilerCode(FormatContext& context, AstNode* node)
{
    const auto code = castAst<AstCompilerCode>(node, AstNodeKind::CompilerCode);
    if (code->hasSpecFlag(AstCompilerCode::SPEC_FLAG_FROM_NEXT))
    {
        concat->addChar(')');
        concat->addEol();
        concat->addIndent(context.indent);
        concat->addChar('{');
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, node->firstChild()));
        concat->addEol();
        concat->addIndent(context.indent);
        concat->addChar('}');
        concat->addEol();
        concat->addIndent(context.indent);
    }
    else
    {
        CONCAT_FIXED_STR(concat, "#code");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    return true;
}

bool FormatAst::outputCompilerGlobal(FormatContext& context, const AstNode* node)
{
    CONCAT_FIXED_STR(concat, "#global");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
    if (node->secondChild())
    {
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->secondChild()));
    }

    concat->addEol();
    return true;
}
