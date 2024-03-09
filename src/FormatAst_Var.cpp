#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"

bool FormatAst::outputVarDecl(const AstVarDecl* varNode, bool isSelf)
{
    if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
    {
        if (!varNode->multiNames.empty())
        {
            if (varNode->hasAstFlag(AST_EXPR_IN_PARENTS))
                concat->addChar('(');

            bool first = true;
            for (const auto& name : varNode->multiNames)
            {
                if (!first)
                {
                    concat->addChar(',');
                    concat->addBlank();
                }
                first = false;
                concat->addString(name);
            }

            if (varNode->hasAstFlag(AST_EXPR_IN_PARENTS))
                concat->addChar(')');
        }
        else if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED))
            concat->addChar('?');
        else
            concat->addString(varNode->token.text);
    }

    if (varNode->type)
    {
        if (!varNode->type->hasAstFlag(AST_GENERATED) || varNode->type->hasAstFlag(AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
                {
                    concat->addChar(':');
                    concat->addBlank();
                }

                SWAG_CHECK(outputNode(varNode->type));
            }
        }
        else
        {
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            const auto typeExpr = castAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
            SWAG_ASSERT(!varNode->assignment);
            SWAG_ASSERT(typeExpr->identifier);
            SWAG_ASSERT(varNode->type->typeInfo && varNode->type->typeInfo->isTuple());
            const auto id = castAst<AstIdentifier>(typeExpr->identifier->lastChild(), AstNodeKind::Identifier);
            concat->addChar('{');
            SWAG_CHECK(outputNode(id->callParameters));
            concat->addChar('}');
        }
    }

    if (varNode->assignment)
    {
        concat->addBlank();
        concat->addChar('=');
        if (!varNode->assignment->is(AstNodeKind::Move) && !varNode->assignment->is(AstNodeKind::NoDrop))
            concat->addBlank();
        SWAG_CHECK(outputNode(varNode->assignment));
    }

    return true;
}

bool FormatAst::outputVar(const AstVarDecl* varNode)
{
    if (varNode->hasAstFlag(AST_DECL_USING))
    {
        CONCAT_FIXED_STR(concat, "using");
        concat->addBlank();
    }

    if (varNode->is(AstNodeKind::ConstDecl))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }
    else if (varNode->isNot(AstNodeKind::FuncDeclParam) && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
            CONCAT_FIXED_STR(concat, "let");
        else
            CONCAT_FIXED_STR(concat, "var");
        concat->addBlank();
    }

    const bool isSelf = varNode->token.text == g_LangSpec->name_self;
    if (isSelf && varNode->type && castAst<AstTypeExpression>(varNode->type)->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }

    SWAG_CHECK(outputVarDecl(varNode, isSelf));
    return true;
}
