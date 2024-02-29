#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
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
            CONCAT_FIXED_STR(concat, "#[ExportType(\"nozero\")]");
            concat->addEolIndent(indent);
        }
    }

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

    concat->addEolIndent(indent);

    // #validif must be exported
    if (node->validif)
    {
        indent++;
        concat->addEolIndent(indent);
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

bool FormatAst::outputType(const AstTypeExpression* node)
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
            SWAG_CHECK(outputType(node, node->typeInfo));

            if (node->identifier)
            {
                const auto id = castAst<AstIdentifier>(node->identifier->lastChild(), AstNodeKind::Identifier);
                if (id->callParameters)
                {
                    if (id->hasAstFlag(AST_GENERATED))
                    {
                        CONCAT_FIXED_STR(concat, " = {");
                        SWAG_CHECK(outputNode(id->callParameters));
                        concat->addChar('}');
                    }
                    else
                    {
                        concat->addChar('{');
                        SWAG_CHECK(outputNode(id->callParameters));
                        concat->addChar('}');
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
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        for (int i = 0; i < node->arrayDim; i++)
        {
            if (i)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(node->children[i]));
        }

        CONCAT_FIXED_STR(concat, "] ");
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_SLICE))
    {
        CONCAT_FIXED_STR(concat, "[..] ");
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR) && node->typeFlags.has(TYPEFLAG_IS_PTR_ARITHMETIC))
    {
        concat->addChar('^');
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_PTR))
    {
        concat->addChar('*');
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->identifier)
    {
        SWAG_CHECK(outputNode(node->identifier));
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->literalType);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat->addString(typeFromLiteral->name);
    }

    return true;
}

bool FormatAst::outputType(const AstNode* /*node*/, TypeInfo* typeInfo)
{
    // Lambda
    /////////////////////////////////
    if (typeInfo->isLambdaClosure())
    {
        SWAG_ASSERT(typeInfo->declNode && typeInfo->declNode->is(AstNodeKind::TypeLambda));
        SWAG_CHECK(outputNode(typeInfo->declNode));
        return true;
    }

    // Tuple
    /////////////////////////////////
    if (typeInfo->isTuple())
    {
        SWAG_CHECK(outputTypeTuple(typeInfo));
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
    concat->addString(typeInfo->scopedNameExport);

    return true;
}

bool FormatAst::outputEnum(const AstEnum* node)
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

bool FormatAst::outputGenericParameters(const AstNode* node)
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
