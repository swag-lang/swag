#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputStruct(OutputContext& context, Concat& concat, AstStruct* node)
{
    // If we need to export as opaque, and the struct has init values, then we add the
    // #[Swag.ExportType] attribute
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            CONCAT_FIXED_STR(concat, "#[ExportType(\"nozero\")]");
            concat.addEolIndent(context.indent);
        }
    }

    PushErrCxtStep ec(&context, node, ErrCxtStepKind::Export, nullptr);

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
        SWAG_CHECK(outputGenericParameters(context, concat, node->genericParameters));

    if (!node->hasSpecFlag(AstStruct::SPEC_FLAG_ANONYMOUS))
    {
        CONCAT_FIXED_STR(concat, " ");
        concat.addString(node->token.text);
    }

    concat.addEolIndent(context.indent);

    // #validif must be exported
    if (node->validif)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->validif));
        context.indent--;
    }

    // Opaque export. Just simulate structure with the correct size.
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        concat.addChar('{');
        concat.addEol();

        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);

        // Everything in the structure is not initialized
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            SWAG_ASSERT(!typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES));
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8 = ?", typeStruct->sizeOf);
            concat.addEol();
        }

        // Everything in the structure is initialized to zero
        else
        {
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8", typeStruct->sizeOf);
            concat.addEol();
        }

        concat.addIndent(context.indent);
        concat.addChar('}');
    }
    else
    {
        SWAG_CHECK(outputNode(context, concat, node->content));
    }

    return true;
}

bool FormatAst::outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concretePtrRef(typeInfo);
    SWAG_ASSERT(typeInfo->isTuple());
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    const auto nodeStruct = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SWAG_CHECK(outputStruct(context, concat, nodeStruct));
    return true;
}

bool FormatAst::outputType(OutputContext& context, Concat& concat, AstTypeExpression* node)
{
    if (node->typeFlags.has(TYPEFLAG_IS_RETVAL))
    {
        CONCAT_FIXED_STR(concat, "retval");
        return true;
    }

    if (node->typeInfo)
    {
        // Identifier can have an export node, so in that case we need to export by node, not by type, in
        // order to export the real node (for example for an array of lambdas/closures)
        if (!node->identifier || !node->identifier->hasExtraPointer(ExtraPointerKind::ExportNode))
        {
            SWAG_CHECK(outputType(context, concat, node, node->typeInfo));

            if (node->identifier)
            {
                const auto id = castAst<AstIdentifier>(node->identifier->lastChild(), AstNodeKind::Identifier);
                if (id->callParameters)
                {
                    if (id->hasAstFlag(AST_GENERATED))
                    {
                        CONCAT_FIXED_STR(concat, " = {");
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                    else
                    {
                        concat.addChar('{');
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                }
            }

            return true;
        }
    }

    if (node->typeFlags.has(TYPEFLAG_IS_CONST))
        CONCAT_FIXED_STR(concat, "const ");

    if (node->arrayDim == UINT8_MAX)
    {
        CONCAT_FIXED_STR(concat, "[] ");
        SWAG_CHECK(outputNode(context, concat, node->firstChild()));
        return true;
    }

    if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        for (int i = 0; i < node->arrayDim; i++)
        {
            if (i)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->children[i]));
        }

        CONCAT_FIXED_STR(concat, "] ");
        SWAG_CHECK(outputNode(context, concat, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_SLICE))
    {
        CONCAT_FIXED_STR(concat, "[..] ");
        SWAG_CHECK(outputNode(context, concat, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR) && node->typeFlags.has(TYPEFLAG_IS_PTR_ARITHMETIC))
    {
        concat.addChar('^');
        SWAG_CHECK(outputNode(context, concat, node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR))
    {
        concat.addChar('*');
        SWAG_CHECK(outputNode(context, concat, node->firstChild()));
        return true;
    }

    if (node->identifier)
    {
        SWAG_CHECK(outputNode(context, concat, node->identifier));
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->literalType);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat.addString(typeFromLiteral->name);
    }

    return true;
}

bool FormatAst::outputType(OutputContext& context, Concat& concat, AstNode* /*node*/, TypeInfo* typeInfo)
{
    // Lambda
    /////////////////////////////////
    if (typeInfo->isLambdaClosure())
    {
        SWAG_ASSERT(typeInfo->declNode && typeInfo->declNode->is(AstNodeKind::TypeLambda));
        SWAG_CHECK(outputNode(context, concat, typeInfo->declNode));
        return true;
    }

    // Tuple
    /////////////////////////////////
    if (typeInfo->isTuple())
    {
        SWAG_CHECK(outputTypeTuple(context, concat, typeInfo));
        return true;
    }

    // Other types
    /////////////////////////////////

    // When this is a reference, take the original (struct) type instead
    if (typeInfo->isAutoConstPointerRef())
    {
        const auto typeRef = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        typeInfo           = typeRef->pointedType;
    }

    if (typeInfo->isSelf())
    {
        if (typeInfo->isConst())
            CONCAT_FIXED_STR(concat, "const self");
        else
            CONCAT_FIXED_STR(concat, "self");
        return true;
    }

    // Export type name
    typeInfo->computeScopedNameExport();
    SWAG_ASSERT(!typeInfo->scopedNameExport.empty());
    concat.addString(typeInfo->scopedNameExport);

    return true;
}
