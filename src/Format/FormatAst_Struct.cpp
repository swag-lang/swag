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
    outputChildrenEol(context, node);
    context.indent--;
    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputTupleDeclContent(const FormatContext& context, AstNode* node)
{
    FormatContext cxt{context};
    cxt.alignVarDecl = false;
    concat->addChar('{');
    if (!node->children.empty())
        concat->addBlank();
    outputChildrenChar(cxt, node, ',', 0, 0);
    if (!node->children.empty())
        concat->addBlank();
    concat->addChar('}');
    beautifyAfter(context, node);
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
            concat->addString("#[ExportType(\"nozero\")]");
            concat->addEol();
        }
    }

    bool       sameLine  = false;
    const bool anonymous = node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS);

    if (node->is(AstNodeKind::InterfaceDecl))
    {
        concat->addIndent(context.indent);
        concat->addString("interface");
    }
    else
    {
        SWAG_ASSERT(node->is(AstNodeKind::StructDecl));

        if (context.keepSameLineStruct)
            sameLine = node->constraints.empty() && !node->hasAttribute(ATTRIBUTE_OPAQUE) && !hasEOLInside(node->content);

        if (!sameLine)
            concat->addIndent(context.indent);

        if (node->hasSpecFlag(AstStruct::SPEC_FLAG_SPECIFIED_TYPE) || !node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
        {
            if (const auto structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl); structNode->hasSpecFlag(AstStruct::SPEC_FLAG_UNION))
                concat->addString("union");
            else
                concat->addString("struct");
        }
    }

    if (node->genericParameters && !node->genericParameters->hasAstFlag(AST_GENERATED))
        SWAG_CHECK(outputGenericParameters(context, node->genericParameters));

    if (!anonymous)
    {
        concat->addBlank();
        concat->addString(node->token.text);
    }

    if (sameLine)
    {
        if (!anonymous)
            concat->addBlank();
        SWAG_CHECK(outputTupleDeclContent(context, node->content));
        return true;
    }

    // Constrains
    for (const auto it : node->constraints)
    {
        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputNode(context, it));
        context.indent--;
    }

    concat->addEol();
    concat->addIndent(context.indent);

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

bool FormatAst::outputTypeTuple(const FormatContext& context, TypeInfo* typeInfo)
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
    concat->addString("impl");
    concat->addBlank();

    if (nodeImpl->hasSpecFlag(AstImpl::SPEC_FLAG_ENUM))
    {
        concat->addString("enum");
        concat->addBlank();
    }

    SWAG_CHECK(outputNode(context, nodeImpl->identifier));

    if (nodeImpl->identifierFor)
    {
        concat->addBlank();
        concat->addString("for");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, nodeImpl->identifierFor));
    }

    concat->addEol();
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();

    const uint32_t first = nodeImpl->identifierFor ? 2 : 1;
    context.indent++;
    SWAG_CHECK(outputChildrenEol(context, node, first));
    context.indent--;

    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
