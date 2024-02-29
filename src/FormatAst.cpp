#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"

const AstNode* FormatAst::convertNode(const AstNode* node)
{
    if (!node)
        return nullptr;
    if (const auto subExportNode = node->extraPointer<AstNode>(ExtraPointerKind::ExportNode))
        node = subExportNode;
    if (node->hasAstFlag(AST_GENERATED) && !node->hasAstFlag(AST_GENERATED_USER))
        return nullptr;
    return node;
}

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
    for (const auto it : node->children)
    {
        const auto child = convertNode(it);
        if (!child)
            continue;

        concat->addIndent(indent);
        SWAG_CHECK(outputNode(child));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputCommaChildren(const AstNode* node)
{
    bool first = true;
    for (const auto it : node->children)
    {
        const auto child = convertNode(it);
        if (!child)
            continue;

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(child));
        first = false;
    }

    return true;
}

bool FormatAst::outputLiteral(const AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
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
