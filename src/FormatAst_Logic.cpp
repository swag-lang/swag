#include "pch.h"
#include "Ast.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"

bool FormatAst::outputIf(const Utf8& name, const AstNode* node)
{
    const auto ifNode = castAst<AstIf>(node, AstNodeKind::If);
    CONCAT_FIXED_STR(concat, name);
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

    concat->addEol();
    concat->addIndent(indent);
    SWAG_CHECK(outputNode(ifNode->ifBlock));

    if (ifNode->elseBlock)
    {
        concat->addEol();
        concat->addIndent(indent);
        CONCAT_FIXED_STR(concat, "else");
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(ifNode->elseBlock));
    }

    return true;
}
