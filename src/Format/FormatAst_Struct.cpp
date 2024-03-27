#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputStructDeclContent(FormatContext& context, AstNode* node)
{
    concat->addChar('{');
    concat->addEol();
    context.indent++;
    outputChildren(context, node);
    context.indent--;
    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputTupleDeclContent(FormatContext& context, AstNode* node)
{
    concat->addChar('{');
    outputCommaChildren(context, node);
    concat->addChar('}');
    return true;
}

bool FormatAst::outputStructDecl(FormatContext& context, AstStruct* node)
{
    if (node->hasSpecFlag(AstStruct::SPEC_FLAG_GENERIC_PARAM))
    {
        SWAG_CHECK(outputTupleDeclContent(context, node->content));
        return true;
    }

    // If we need to export as opaque, and the struct has init values, then we add the
    // #[Swag.ExportType] attribute
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            concat->addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "#[ExportType(\"nozero\")]");
            concat->addEol();
        }
    }

    bool sameLineAnonymous = true;
    if (node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
    {
        for (const auto s : node->content->children)
        {
            if (const auto parse = getTokenParse(s))
            {
                if (parse->flags.has(TOKEN_PARSE_EOL_BEFORE | TOKEN_PARSE_EOL_AFTER))
                {
                    sameLineAnonymous = false;
                    break;
                }
            }
        }
    }

    if (!node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS) || !sameLineAnonymous)
        concat->addIndent(context.indent);

    if (node->is(AstNodeKind::InterfaceDecl))
    {
        CONCAT_FIXED_STR(concat, "interface");
    }
    else
    {
        SWAG_ASSERT(node->is(AstNodeKind::StructDecl));
        if (node->hasSpecFlag(AstStruct::SPEC_FLAG_SPECIFIED_TYPE) || !node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
        {
            if (const auto structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl); structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION))
                CONCAT_FIXED_STR(concat, "union");
            else
                CONCAT_FIXED_STR(concat, "struct");
        }
    }

    if (node->genericParameters && !node->genericParameters->hasAstFlag(AST_GENERATED))
    {
        SWAG_CHECK(outputGenericParameters(context, node->genericParameters));
    }

    if (!node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
    {
        concat->addBlank();
        concat->addString(node->token.text);
    }
    else if (sameLineAnonymous)
    {
        SWAG_CHECK(outputTupleDeclContent(context, node->content));
        return true;
    }

    concat->addEol();
    concat->addIndent(context.indent);

    // #validif must be exported
    if (node->validif)
    {
        context.indent++;
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, node->validif));
        context.indent--;
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
            concat->addIndent(context.indent + 1);
            concat->addStringFormat("padding: [%llu] u8 = ?", typeStruct->sizeOf);
            concat->addEol();
        }

        // Everything in the structure is initialized to zero
        else
        {
            concat->addIndent(context.indent + 1);
            concat->addStringFormat("padding: [%llu] u8", typeStruct->sizeOf);
            concat->addEol();
        }

        concat->addIndent(context.indent);
        concat->addChar('}');
    }
    else
    {
        SWAG_CHECK(outputNode(context, node->content));
    }

    return true;
}

bool FormatAst::outputTypeTuple(FormatContext& context, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concretePtrRef(typeInfo);
    SWAG_ASSERT(typeInfo->isTuple());
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    const auto nodeStruct = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SWAG_CHECK(outputTupleDeclContent(context, nodeStruct->content));
    return true;
}

bool FormatAst::outputImpl(FormatContext& context, AstNode* node)
{
    const auto nodeImpl = castAst<AstImpl>(node, AstNodeKind::Impl);

    concat->addIndent(context.indent);
    CONCAT_FIXED_STR(concat, "impl");
    concat->addBlank();

    if (nodeImpl->hasSpecFlag(AstImpl::SPEC_FLAG_ENUM))
    {
        CONCAT_FIXED_STR(concat, "enum");
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(context, nodeImpl->identifier));

    if (nodeImpl->identifierFor)
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "for");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, nodeImpl->identifierFor));
    }

    concat->addEol();
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();

    const uint32_t first = nodeImpl->identifierFor ? 2 : 1;
    context.indent++;
    SWAG_CHECK(outputChildren(context, node, first));
    context.indent--;

    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
