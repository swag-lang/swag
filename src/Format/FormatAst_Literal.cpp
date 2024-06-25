#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool FormatAst::outputLiteral(FormatContext& context, AstNode* node)
{
    const auto literalNode = castAst<AstLiteral>(node, AstNodeKind::Literal);
    if (literalNode->literalType == LiteralType::TypeStringRaw)
        concat->addString(R"(#")");
    else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
        concat->addString(R"(")");
    else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
        concat->addString(R"(""")");
    else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
        concat->addString("`");

    concat->addString(node->token.text);

    if (literalNode->literalType == LiteralType::TypeStringRaw)
        concat->addString(R"("#)");
    else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
        concat->addString(R"(")");
    else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
        concat->addString(R"(""")");
    else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
        concat->addString("`");

    if (!node->children.empty())
    {
        concat->addString(R"(')");
        SWAG_CHECK(outputNode(context, node->firstChild()));
    }

    return true;
}

bool FormatAst::outputLiteral(FormatContext& context, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo->isPointerNull())
    {
        concat->addString("null");
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

bool FormatAst::outputExpressionList(FormatContext& context, AstNode* node)
{
    const auto exprNode = castAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
    if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        concat->addChar('{');
    else
        concat->addChar('[');

    uint32_t addIndent = 0;
    AstNode* prevChild = nullptr;

    for (uint32_t i = 0; i < exprNode->childCount(); i++)
    {
        const auto it    = exprNode->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (prevChild)
        {
            concat->addChar(',');
            beautifyAfter(context, prevChild);
            if (!concat->indent)
                concat->addBlank();
        }

        if (const auto parse = getTokenParse(child))
        {
            if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE))
            {
                if (!prevChild)
                {
                    context.indent++;
                    addIndent = 1;
                }

                concat->addEol();
                concat->addIndent(context.indent);
            }
        }

        FormatContext cxt{context};
        cxt.beautifyAfter = false;
        SWAG_CHECK(outputNode(cxt, child));
        prevChild = child;
    }

    context.indent -= addIndent;

    if (const auto parse = getTokenParse(exprNode))
    {
        if (parse->flags.has(TOKEN_PARSE_EOL_AFTER))
        {
            concat->addEol();
            concat->addIndent(context.indent);
        }
    }

    if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        concat->addChar('}');
    else
        concat->addChar(']');
    return true;
}
