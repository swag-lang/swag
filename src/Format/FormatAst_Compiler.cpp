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
        concat->addString("#global");
        concat->addBlank();
        concat->addString("#if");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, ifNode->boolExpression));
        concat->addEol();
        SWAG_CHECK(outputChildrenEol(context, ifNode->ifBlock->firstChild()));
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
        concat->addChar(':');
        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, ifNode->ifBlock->lastChild()));
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
        concat->addString("#else");
        if ((ifNode->elseBlock->firstChild()->is(AstNodeKind::Statement) && !ifNode->elseBlock->firstChild()->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY)) ||
            ifNode->elseBlock->firstChild()->isNot(AstNodeKind::Statement))
        {
            concat->addChar(':');
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

bool FormatAst::outputCompilerSpecialValue(FormatContext&, AstNode* node) const
{
    switch (node->token.id)
    {
        case TokenId::CompilerSelf:
            concat->addString("#self");
            break;
        case TokenId::CompilerCallerFunction:
            concat->addString("#callerfunction");
            break;
        case TokenId::CompilerCallerLocation:
            concat->addString("#callerlocation");
            break;
        case TokenId::CompilerCurLocation:
            concat->addString("#curlocation");
            break;
        case TokenId::CompilerOs:
            concat->addString("#os");
            break;
        case TokenId::CompilerSwagOs:
            concat->addString("#swagos");
            break;
        case TokenId::CompilerArch:
            concat->addString("#arch");
            break;
        case TokenId::CompilerCpu:
            concat->addString("#cpu");
            break;
        case TokenId::CompilerBackend:
            concat->addString("#backend");
            break;
        case TokenId::CompilerBuildCfg:
            concat->addString("#cfg");
            break;
        default:
            Report::internalError(node, "FormatAst::outputNode, unknown compiler function");
    }

    return true;
}

bool FormatAst::outputCompilerInject(FormatContext& context, AstNode* node)
{
    const auto compilerMixin = castAst<AstCompilerInject>(node, AstNodeKind::CompilerInject);
    if (node->hasAstFlag(AST_DISCARD))
    {
        concat->addString("discard");
        concat->addBlank();
    }

    concat->addString("#inject");
    concat->addChar('(');
    SWAG_CHECK(outputNode(context, node->firstChild()));

    if (!compilerMixin->replaceTokens.empty())
    {
        for (const auto m : compilerMixin->replaceTokens)
        {
            concat->addChar(',');
            concat->addBlank();
            
            switch (m.first)
            {
                case TokenId::KwdBreak:
                    concat->addString("break");
                    break;
                case TokenId::KwdContinue:
                    concat->addString("continue");
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }

            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            SWAG_CHECK(outputNode(context, m.second));
        }
    }

    concat->addChar(')');
    return true;
}

bool FormatAst::outputCompilerConstraints(FormatContext& context, const VectorNative<AstNode*>& constraints)
{
    if (constraints.empty())
        return true;

    concat->addEol();
    context.indent++;
    concat->addIndent(context.indent);

    for (uint32_t i = 0; i < constraints.size(); i++)
    {
        const auto node = constraints[i];

        if (node->is(AstNodeKind::WhereConstraint))
            concat->addString("where");
        else if (node->is(AstNodeKind::VerifyConstraint))
            concat->addString("verify");

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

        if (i == constraints.size() - 1)
            break;

        concat->addEol();
        concat->addIndent(context.indent);
    }

    concat->addEol();
    context.indent--;

    return true;
}

bool FormatAst::outputCompilerExpr(FormatContext& context, const AstNode* node)
{
    if (node->is(AstNodeKind::CompilerRun) || node->is(AstNodeKind::CompilerRunExpression))
        concat->addString("#run");
    else if (node->is(AstNodeKind::CompilerAst))
        concat->addString("#ast");
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

bool FormatAst::outputCompilerImport(FormatContext&, AstNode* node) const
{
    const auto decl = castAst<AstCompilerImport>(node, AstNodeKind::CompilerImport);
    concat->addString("#import");
    concat->addChar('(');
    concat->addChar('"');
    concat->addString(node->token.text);
    concat->addChar('"');

    if (!decl->tokenLocation.text.empty())
    {
        concat->addChar(',');
        concat->addBlank();        
        concat->addString("location");
        concat->addChar(':');
        concat->addBlank();
        concat->addChar('"');
        concat->addString(decl->tokenLocation.text);
        concat->addChar('"');

        if (!decl->tokenVersion.text.empty())
        {
            concat->addChar(',');
            concat->addBlank();
            concat->addString("version");
            concat->addChar(':');
            concat->addBlank();
            concat->addChar('"');
            concat->addString(decl->tokenVersion.text);
            concat->addChar('"');
        }
    }

    concat->addChar(')');
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
        FormatContext cxt{context};
        cxt.canConcatStatement = context.keepSameCodeBlock;
        concat->addString("#code");
        concat->addBlank();
        SWAG_CHECK(outputNode(cxt, node->firstChild()));
        concat->removeLastChar('\n');
    }

    return true;
}

bool FormatAst::outputCompilerGlobal(FormatContext& context, const AstNode* node)
{
    concat->addString("#global");
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
