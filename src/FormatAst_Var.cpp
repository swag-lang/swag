#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputVarDecl(const AstVarDecl* varNode, bool isSelf)
{
    if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
    {
        if (!varNode->publicName.empty())
            concat->addString(varNode->publicName);
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
                    CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(outputNode(varNode->type));
            }
        }
        else
        {
            CONCAT_FIXED_STR(concat, " = ");
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
        CONCAT_FIXED_STR(concat, " = ");
        SWAG_CHECK(outputNode(varNode->assignment));
    }

    return true;
}

bool FormatAst::outputVar(const AstVarDecl* varNode)
{
    if (varNode->hasAstFlag(AST_DECL_USING))
        CONCAT_FIXED_STR(concat, "using ");

    if (varNode->is(AstNodeKind::ConstDecl))
    {
        CONCAT_FIXED_STR(concat, "const ");
    }
    else if (varNode->isNot(AstNodeKind::FuncDeclParam) && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
            CONCAT_FIXED_STR(concat, "let ");
        else
            CONCAT_FIXED_STR(concat, "var ");
    }

    const bool isSelf = varNode->token.text == g_LangSpec->name_self;
    if (isSelf && varNode->type && castAst<AstTypeExpression>(varNode->type)->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const ");
    }

    SWAG_CHECK(outputVarDecl(varNode, isSelf));
    return true;
}
