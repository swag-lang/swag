#include "pch.h"
#include "Ast.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"

bool FormatAst::outputIf(const Utf8& name, const AstNode* node)
{
    const auto ifNode = castAst<AstIf>(node, AstNodeKind::If);
    concat->addString(name);
    concat->addBlank();

    if (ifNode->hasSpecFlag(AstIf::SPEC_FLAG_ASSIGN))
    {
        const auto varNode = castAst<AstVarDecl>(ifNode->firstChild(), AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        if (varNode->token.is(TokenId::KwdConst))
            CONCAT_FIXED_STR(concat, "const");
        else
            CONCAT_FIXED_STR(concat, "var");
        concat->addBlank();
        SWAG_CHECK(outputVarDecl(varNode, false));
    }
    else
        SWAG_CHECK(outputNode(ifNode->boolExpression));

    SWAG_CHECK(outputDoStatement(ifNode->ifBlock));
    if (ifNode->elseBlock)
    {
        if (ifNode->elseBlock->is(AstNodeKind::If))
        {
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputIf("elif", ifNode->elseBlock));
        }
        else
        {
            concat->addIndent(indent);
            CONCAT_FIXED_STR(concat, "else");
            SWAG_CHECK(outputDoStatement(ifNode->elseBlock));
        }
    }

    return true;
}

bool FormatAst::outputWhile(const AstNode* node)
{
    const auto whileNode = castAst<AstWhile>(node, AstNodeKind::While);
    CONCAT_FIXED_STR(concat, "while");
    concat->addBlank();
    SWAG_CHECK(outputNode(whileNode->boolExpression));
    SWAG_CHECK(outputDoStatement(whileNode->block));
    return true;
}

bool FormatAst::outputLoop(const AstNode* node)
{
    const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
    CONCAT_FIXED_STR(concat, "loop");
    if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
        CONCAT_FIXED_STR(concat, ",back");
    concat->addBlank();

    if (loopNode->specificName)
    {
        concat->addString(loopNode->specificName->token.text);
        concat->addChar(':');
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(loopNode->expression));
    SWAG_CHECK(outputDoStatement(loopNode->block));
    return true;
}

bool FormatAst::outputVisit(const AstNode* node)
{
    const auto visitNode = castAst<AstVisit>(node, AstNodeKind::Visit);
    CONCAT_FIXED_STR(concat, "visit");
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
        concat->addChar(':');
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(visitNode->expression));
    SWAG_CHECK(outputDoStatement(visitNode->block));
    return true;
}

bool FormatAst::outputFor(const AstNode* node)
{
    const auto forNode = castAst<AstFor>(node, AstNodeKind::For);
    CONCAT_FIXED_STR(concat, "for");
    concat->addBlank();
    SWAG_CHECK(outputNode(forNode->preExpression));
    concat->addChar(';');
    concat->addBlank();
    SWAG_CHECK(outputNode(forNode->boolExpression));
    concat->addChar(';');
    concat->addBlank();
    SWAG_CHECK(outputNode(forNode->postExpression));

    SWAG_CHECK(outputDoStatement(forNode->block));
    return true;
}

bool FormatAst::outputSwitch(const AstNode* node)
{
    const auto nodeSwitch = castAst<AstSwitch>(node, AstNodeKind::Switch);
    CONCAT_FIXED_STR(concat, "switch");
    concat->addBlank();
    SWAG_CHECK(outputNode(nodeSwitch->expression));
    concat->addEol();
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();

    for (const auto c : nodeSwitch->cases)
    {
        concat->addIndent(indent);
        if (c->expressions.empty())
        {
            beautifyBlankLine(c);
            CONCAT_FIXED_STR(concat, "default");
        }
        else
        {
            beautifyBlankLine(c);
            CONCAT_FIXED_STR(concat, "case");
            concat->addBlank();
            bool first = true;
            for (const auto it : c->expressions)
            {
                if (!first)
                {
                    concat->addChar(',');
                    concat->addBlank();
                }

                SWAG_CHECK(outputNode(it));
                first = false;
            }
        }

        concat->addChar(':');
        concat->addEol();
        indent++;
        SWAG_CHECK(outputNode(c->block));
        concat->addEol();
        indent--;
    }

    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
