#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputIf(FormatContext& context, const Utf8& name, AstNode* node)
{
    const auto ifNode = castAst<AstIf>(node, AstNodeKind::If);
    concat->addString(name);
    concat->addBlank();

    if (ifNode->hasSpecFlag(AstIf::SPEC_FLAG_ASSIGN))
    {
        const auto varNode = castAst<AstVarDecl>(ifNode->firstChild(), AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        if (varNode->is(AstNodeKind::ConstDecl))
            concat->addString("const");
        else if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET))
            concat->addString("let");
        else
            concat->addString("var");
        concat->addBlank();
        SWAG_CHECK(outputVarContent(context, varNode));
    }
    
    SWAG_CHECK(outputNode(context, ifNode->boolExpression));
    SWAG_CHECK(outputDoStatement(context, ifNode->ifBlock));
    
    if (ifNode->elseBlock)
    {
        if (ifNode->elseBlock->is(AstNodeKind::If))
        {
            concat->addIndent(context.indent);
            beautifyBefore(context, ifNode->elseBlock);
            SWAG_CHECK(outputIf(context, "elif", ifNode->elseBlock));
        }
        else
        {
            concat->addIndent(context.indent);
            beautifyBefore(context, ifNode->elseBlock);
            concat->addString("else");
            SWAG_CHECK(outputDoStatement(context, ifNode->elseBlock));
        }
    }

    return true;
}

bool FormatAst::outputWhile(FormatContext& context, AstNode* node)
{
    const auto whileNode = castAst<AstWhile>(node, AstNodeKind::While);
    concat->addString("while");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, whileNode->boolExpression));
    SWAG_CHECK(outputDoStatement(context, whileNode->block));
    return true;
}

bool FormatAst::outputLoop(FormatContext& context, AstNode* node)
{
    const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
    concat->addString("loop");
    if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_back);
    }

    if (loopNode->specificName)
    {
        concat->addBlank();
        concat->addString(loopNode->specificName->token.text);
        concat->addBlank();
        concat->addString("in");
    }

    if (loopNode->expression)
    {
        concat->addBlank();
        SWAG_CHECK(outputNode(context, loopNode->expression));
    }

    SWAG_CHECK(outputDoStatement(context, loopNode->block));
    return true;
}

bool FormatAst::outputVisit(FormatContext& context, AstNode* node)
{
    const auto visitNode = castAst<AstVisit>(node, AstNodeKind::Visit);
    concat->addString("visit");
    if (!visitNode->extraNameToken.text.empty())
    {
        concat->addChar('(');
        concat->addString(visitNode->extraNameToken.text);
        concat->addChar(')');
    }

    if (visitNode->hasSpecFlag(AstVisit::SPEC_FLAG_BACK))
    {
        concat->addBlank();
        concat->addString(g_LangSpec->name_back);
    }

    concat->addBlank();
    if (visitNode->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        concat->addChar('&');

    bool first = true;
    for (const auto& a : visitNode->aliasNames)
    {
        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        concat->addString(a.text);
        first = false;
    }

    if (!visitNode->aliasNames.empty())
    {
        concat->addBlank();
        concat->addString("in");
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(context, visitNode->expression));
    SWAG_CHECK(outputDoStatement(context, visitNode->block));
    return true;
}

bool FormatAst::outputFor(FormatContext& context, AstNode* node)
{
    const auto forNode = castAst<AstFor>(node, AstNodeKind::For);
    concat->addString("for");

    if (forNode->preExpression && forNode->preExpression->is(AstNodeKind::Statement))
    {
        concat->addBlank();
        concat->addChar('{');
        concat->addBlank();

        for (const auto c : forNode->preExpression->children)
        {
            SWAG_CHECK(outputNode(context, c));
            concat->addChar(';');
            concat->addBlank();
        }

        concat->addChar('}');
        concat->addBlank();
    }
    else
    {
        concat->addBlank();
        SWAG_CHECK(outputNode(context, forNode->preExpression));
        concat->addChar(';');
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(context, forNode->boolExpression));
    concat->addChar(';');
    concat->addBlank();
    SWAG_CHECK(outputNode(context, forNode->postExpression));

    SWAG_CHECK(outputDoStatement(context, forNode->block));
    return true;
}

bool FormatAst::outputSwitch(FormatContext& context, AstNode* node)
{
    const auto nodeSwitch = castAst<AstSwitch>(node, AstNodeKind::Switch);
    concat->addString("switch");
    concat->addBlank();
    SWAG_CHECK(outputNode(context, nodeSwitch->expression));
    concat->addEol();
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();

    for (const auto c : nodeSwitch->cases)
    {
        concat->addIndent(context.indent);
        if (c->expressions.empty())
        {
            beautifyBefore(context, c);
            concat->addString("default");
        }
        else
        {
            beautifyBefore(context, c);
            concat->addString("case");
            concat->addBlank();
            bool first = true;
            for (const auto it : c->expressions)
            {
                if (!first)
                {
                    concat->addChar(',');
                    concat->addBlank();
                }

                SWAG_CHECK(outputNode(context, it));
                first = false;
            }
        }

        auto block = c->block;

        if (c->hasSpecFlag(AstSwitchCase::SPEC_FLAG_HAS_WHERE))
        {
            concat->addChar(' ');
            concat->addString("where");
            concat->addChar(' ');

            const AstIf* ifNode = castAst<AstIf>(block->firstChild(), AstNodeKind::If);
            SWAG_CHECK(outputNode(context, ifNode->boolExpression));
            block = ifNode->ifBlock;
        }

        concat->addChar(':');

        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, block));
        concat->addEol();
        context.indent--;
    }

    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
