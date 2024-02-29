#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"

void FormatAst::incIndentStatement(const AstNode* node, uint32_t& idt)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        idt++;
}

void FormatAst::decIndentStatement(const AstNode* node, uint32_t& idt)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        idt--;
}

bool FormatAst::outputChildren(const AstNode* node)
{
    for (const auto child : node->children)
    {
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(child));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputGenericParameters(AstNode* node)
{
    CONCAT_FIXED_STR(concat, "(");
    int idx = 0;
    for (const auto p : node->children)
    {
        if (idx)
            CONCAT_FIXED_STR(concat, ", ");
        concat->addString(p->token.text);

        const AstVarDecl* varDecl = castAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(varDecl->type));
        }
        else if (varDecl->typeConstraint)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(varDecl->typeConstraint));
        }

        if (varDecl->assignment)
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(varDecl->assignment));
        }

        idx++;
    }

    CONCAT_FIXED_STR(concat, ")");
    return true;
}

bool FormatAst::outputEnum(AstEnum* node)
{
    CONCAT_FIXED_STR(concat, "enum ");
    concat->addString(node->token.text);

    // Raw type
    if (node->firstChild()->childCount())
    {
        concat->addBlank();
        concat->addChar(':');
        concat->addBlank();
        SWAG_ASSERT(node->firstChild()->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(node->firstChild()));
    }

    concat->addEolIndent(indent);
    concat->addChar('{');
    concat->addEol();

    for (const auto c : node->children)
    {
        if (c->is(AstNodeKind::EnumValue))
        {
            concat->addIndent(indent + 1);

            if (c->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
            {
                CONCAT_FIXED_STR(concat, "using ");
                SWAG_CHECK(outputNode(c->firstChild()));
            }
            else
            {
                concat->addString(c->token.text);
                if (!c->children.empty())
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(c->firstChild()));
                }
            }

            concat->addEol();
        }
    }

    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputLiteral(AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo->isPointerNull())
    {
        CONCAT_FIXED_STR(concat, "null");
        return true;
    }

    if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        SWAG_CHECK(outputNode(node));
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
