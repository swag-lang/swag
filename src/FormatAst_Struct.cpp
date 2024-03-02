#include "pch.h"
#include "Ast.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputStructDeclContent(const AstNode* node)
{
    concat->addChar('{');
    concat->addEol();
    indent++;
    outputChildren(node);
    indent--;
    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputTupleDeclContent(const AstNode* node)
{
    concat->addChar('{');
    outputCommaChildren(node);
    concat->addChar('}');
    return true;
}

bool FormatAst::outputStructDecl(const AstStruct* node)
{
    // If we need to export as opaque, and the struct has init values, then we add the
    // #[Swag.ExportType] attribute
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            concat->addIndent(indent);
            CONCAT_FIXED_STR(concat, "#[ExportType(\"nozero\")]");
            concat->addEol();
        }
    }

    concat->addIndent(indent);
    if (node->is(AstNodeKind::InterfaceDecl))
        CONCAT_FIXED_STR(concat, "interface");
    else
    {
        SWAG_ASSERT(node->is(AstNodeKind::StructDecl));
        if (const auto structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl); structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION))
            CONCAT_FIXED_STR(concat, "union");
        else
            CONCAT_FIXED_STR(concat, "struct");
    }

    if (node->genericParameters)
        SWAG_CHECK(outputGenericParameters(node->genericParameters));

    if (!node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
    {
        concat->addBlank();
        concat->addString(node->token.text);
    }

    concat->addEol();
    concat->addIndent(indent);

    // #validif must be exported
    if (node->validif)
    {
        indent++;
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(node->validif));
        indent--;
    }

    // Opaque export. Just simulate structure with the correct size.
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        concat->addChar('{');
        concat->addEol();

        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);

        // Everything in the structure is not initialized
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            SWAG_ASSERT(!typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES));
            concat->addIndent(indent + 1);
            concat->addStringFormat("padding: [%llu] u8 = ?", typeStruct->sizeOf);
            concat->addEol();
        }

        // Everything in the structure is initialized to zero
        else
        {
            concat->addIndent(indent + 1);
            concat->addStringFormat("padding: [%llu] u8", typeStruct->sizeOf);
            concat->addEol();
        }

        concat->addIndent(indent);
        concat->addChar('}');
    }
    else
    {
        SWAG_CHECK(outputNode(node->content));
    }

    return true;
}

bool FormatAst::outputTypeTuple(TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concretePtrRef(typeInfo);
    SWAG_ASSERT(typeInfo->isTuple());
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    const auto nodeStruct = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SWAG_CHECK(outputTupleDeclContent(nodeStruct->content));
    return true;
}

bool FormatAst::outputImpl(const AstNode* node)
{
    const auto nodeImpl = castAst<AstImpl>(node, AstNodeKind::Impl);

    concat->addBlankLine();
    concat->addIndent(indent);
    CONCAT_FIXED_STR(concat, "impl");
    concat->addBlank();
    concat->addString(nodeImpl->identifier->token.text);
    if (nodeImpl->identifierFor)
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "for");
        concat->addBlank();
        concat->addString(nodeImpl->identifierFor->token.text);
    }
    else if (nodeImpl->scope && nodeImpl->scope->kind == ScopeKind::Enum)
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "enum");
        concat->addBlank();
    }

    concat->addEol();
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();

    const uint32_t first = nodeImpl->identifierFor ? 2 : 1;
    for (uint32_t i = first; i < node->childCount(); i++)
    {
        const auto child = node->children[i];
        indent++;
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(child));
        concat->addEol();
        indent--;
        concat->addEol();
    }

    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
