#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool FormatAst::outputLiteral(FormatContext& context, const AstNode* node)
{
    const auto literalNode = castAst<AstLiteral>(node, AstNodeKind::Literal);
    if (literalNode->literalType == LiteralType::TypeStringRaw)
        CONCAT_FIXED_STR(concat, "#\"");
    else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
        CONCAT_FIXED_STR(concat, "\"");
    else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
        CONCAT_FIXED_STR(concat, "\"\"\"");
    else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
        CONCAT_FIXED_STR(concat, "`");

    concat->addString(node->token.text);

    if (literalNode->literalType == LiteralType::TypeStringRaw)
        CONCAT_FIXED_STR(concat, "\"#");
    else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
        CONCAT_FIXED_STR(concat, "\"");
    else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
        CONCAT_FIXED_STR(concat, "\"\"\"");
    else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
        CONCAT_FIXED_STR(concat, "`");

    if (!node->children.empty())
    {
        concat->addChar('\'');
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    return true;
}

bool FormatAst::outputLiteral(FormatContext& context, const AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo->isPointerNull())
    {
        CONCAT_FIXED_STR(concat, "null");
        return true;
    }

    if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        SWAG_CHECK(outputNode(context, node));
        return true;
    }

    if (typeInfo->isEnum())
    {
        const Utf8 str = Ast::enumToString(typeInfo, value.text, value.reg, true);
        concat->addString(str);
        return true;
    }

    SWAG_ASSERT(typeInfo->isNative());
    auto str = Ast::literalToString(typeInfo, value);
    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::String:
            concat->addString("\"" + str + "\"");
            break;
        case NativeTypeKind::Rune:
            concat->addString("\"" + str + "\"\'rune");
            break;
        case NativeTypeKind::Bool:
            concat->addString(str);
            break;
        default:
            if (!typeInfo->hasFlag(TYPEINFO_UNTYPED_INTEGER | TYPEINFO_UNTYPED_FLOAT))
            {
                str += '\'';
                str += typeInfo->name;
            }

            concat->addString(str);
            break;
    }

    return true;
}

bool FormatAst::outputExpressionList(FormatContext& context, const AstNode* node)
{
    const auto exprNode = castAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
    if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        concat->addChar('{');
    else
        concat->addChar('[');

    SWAG_CHECK(outputCommaChildren(context, exprNode));

    if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        concat->addChar('}');
    else
        concat->addChar(']');
    return true;
}
