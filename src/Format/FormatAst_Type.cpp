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
        concat->addString("#type");
        concat->addBlank();
    }

    SWAG_CHECK(outputType(context, castAst<AstTypeExpression>(node)));
    return true;
}

bool FormatAst::outputType(FormatContext& context, AstTypeExpression* node)
{
    if (node->typeFlags.has(TYPE_FLAG_IS_CODE))
    {
        concat->addString("code");
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_IS_RETVAL_TYPE))
    {
        if (node->firstChild())
            SWAG_CHECK(outputNode(context, node->firstChild()));
        else
            concat->addString("retval");
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_NULLABLE))
    {
        concat->addString("null");
        concat->addBlank();
    }
    
    if (node->typeFlags.has(TYPE_FLAG_IS_CONST))
    {
        concat->addString("const");
        concat->addBlank();
    }

    if (node->arrayDim == UINT8_MAX)
    {
        concat->addString("[]");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->arrayDim)
    {
        concat->addString("[");
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

    if (node->typeFlags.has(TYPE_FLAG_IS_SLICE))
    {
        concat->addString("[..]");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_IS_PTR) && node->typeFlags.has(TYPE_FLAG_IS_PTR_ARITHMETIC))
    {
        concat->addChar('^');
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_IS_PTR))
    {
        concat->addChar('*');
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_IS_REF) && node->typeFlags.has(TYPE_FLAG_IS_MOVE_REF))
    {
        concat->addString("&&");
        SWAG_CHECK(outputNode(context, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPE_FLAG_IS_REF))
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
        concat->addString("typeinfo");
        if (node->childCount())
            SWAG_CHECK(outputNode(context, node->firstChild()));
    }
    else if (node->typeFromLiteral && node->typeFromLiteral->isTypedVariadic())
    {
        SWAG_CHECK(outputNode(context, node->firstChild()));
        concat->addString("...");
    }
    else if (node->typeFromLiteral && node->typeFromLiteral->isVariadic())
    {
        concat->addString("...");
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
            concat->addString("var");
            concat->addBlank();
        }
        else if (varDecl->hasSpecFlag(AstVarDecl::SPEC_FLAG_FORCE_CONST))
        {
            concat->addString("const");
            concat->addBlank();
        }

        concat->addString(varDecl->token.text);

        if (varDecl->type)
        {
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(context, varDecl->type));
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

bool FormatAst::outputChildrenTypeAlias(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.alignTypeAlias)
        return true;

    VectorNative<AstNode*> nodes;
    if (!collectChildrenToAlign(context, STOP_CMT_BEFORE | STOP_EMPTY_LINE_BEFORE, node, start, nodes, processed, [](const AstNode* inNode) {
            if (inNode->kind != AstNodeKind::TypeAlias)
                return true;
            return false;
        }))
        return true;

    uint32_t maxLenName = 0;
    for (const auto child : nodes)
    {
        maxLenName = max(maxLenName, child->token.text.length());
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputTypeAlias(context, child, maxLenName));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputTypeAlias(FormatContext& context, AstNode* node, uint32_t maxLenName)
{
    beautifyBefore(context, node);

    concat->addString("alias");
    concat->addBlank();
    const auto startColumn = concat->column;
    concat->addString(node->token.text);
    concat->alignToColumn(startColumn + maxLenName);
    concat->addBlank();
    concat->addChar('=');
    concat->addBlank();
    SWAG_CHECK(outputNode(context, node->firstChild()));
    return true;
}
