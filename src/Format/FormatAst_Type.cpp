#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputTypeExpression(FormatContext& context, AstNode* node)
{
    if (node->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
    {
        CONCAT_FIXED_STR(concat, "#type");
        concat->addBlank();
    }

    SWAG_CHECK(outputType(context, castAst<AstTypeExpression>(node)));
    return true;
}

bool FormatAst::outputType(FormatContext& context, AstTypeExpression* node)
{
    if (node->typeFlags.has(TYPEFLAG_IS_CODE))
    {
        CONCAT_FIXED_STR(concat, "code");
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_RETVAL_TYPE))
    {
        if (node->firstChild())
            SWAG_CHECK(outputNode(context, node->firstChild()));
        else
            CONCAT_FIXED_STR(concat, "retval");
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }

    if (node->arrayDim == UINT8_MAX)
    {
        CONCAT_FIXED_STR(concat, "[]");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        uint32_t i = 0;
        for (; i < node->arrayDim; i++)
        {
            if (i)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            SWAG_CHECK(outputNode(context, node->children[i]));
        }

        concat->addChar(']');
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->children[i]));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_SLICE))
    {
        CONCAT_FIXED_STR(concat, "[..]");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR) && node->typeFlags.has(TYPEFLAG_IS_PTR_ARITHMETIC))
    {
        concat->addChar('^');
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR))
    {
        concat->addChar('*');
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_REF) && node->typeFlags.has(TYPEFLAG_IS_MOVE_REF))
    {
        CONCAT_FIXED_STR(concat, "&&");
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_REF))
    {
        concat->addChar('&');
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->identifier)
    {
        SWAG_CHECK(outputNode(context, node->identifier));
    }
    else if (node->literalType == LiteralType::TypeType)
    {
        CONCAT_FIXED_STR(concat, "typeinfo");
        if (node->childCount())
            SWAG_CHECK(outputNode(context, node->firstChild()));
    }
    else if (node->typeFromLiteral && node->typeFromLiteral->isTypedVariadic())
    {
        SWAG_CHECK(outputNode(context, node->firstChild()));
        CONCAT_FIXED_STR(concat, "...");
    }
    else if (node->typeFromLiteral && node->typeFromLiteral->isVariadic())
    {
        CONCAT_FIXED_STR(concat, "...");
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->literalType);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat->addString(typeFromLiteral->name);
        if (node->childCount())
            SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    return true;
}

bool FormatAst::outputGenericParameters(FormatContext& context, AstNode* node)
{
    concat->addChar('(');

    bool first = true;
    for (const auto it : node->children)
    {
        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        const auto varDecl = castAst<AstVarDecl>(child, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->hasSpecFlag(AstVarDecl::SPEC_FLAG_FORCE_VAR))
        {
            CONCAT_FIXED_STR(concat, "var");
            concat->addBlank();
        }
        else if (varDecl->hasSpecFlag(AstVarDecl::SPEC_FLAG_FORCE_CONST))
        {
            CONCAT_FIXED_STR(concat, "const");
            concat->addBlank();
        }

        concat->addString(varDecl->token.text);

        if (varDecl->type)
        {
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(context, varDecl->type));
        }
        else if (varDecl->typeConstraint)
        {
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(context, varDecl->typeConstraint));
        }

        if (varDecl->assignment)
        {
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            SWAG_CHECK(outputNode(context, varDecl->assignment));
        }

        first = false;
    }

    concat->addChar(')');
    return true;
}
