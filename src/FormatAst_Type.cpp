#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "TypeManager.h"

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
    concat->addIndent(indent);
    CONCAT_FIXED_STR(concat, "enum");
    concat->addBlank();
    concat->addString(node->token.text);

    // Raw type
    if (node->firstChild() && node->firstChild()->childCount())
    {
        concat->addBlank();
        concat->addChar(':');
        concat->addBlank();
        SWAG_ASSERT(node->firstChild()->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(node->firstChild()));
    }

    concat->addEol();
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();

    indent++;
    for (const auto it : node->children)
    {
        const auto child = convertNode(it);
        if (!child)
            continue;
        if (!child->is(AstNodeKind::EnumValue))
            continue;

        beautifyBlankLine(child);
        concat->addIndent(indent);

        if (child->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
        {
            CONCAT_FIXED_STR(concat, "using");
            concat->addBlank();
            SWAG_CHECK(outputNode(child->firstChild()));
        }
        else
        {
            concat->addString(child->token.text);
            if (!child->children.empty())
            {
                concat->addBlank();
                concat->addChar('=');
                concat->addBlank();
                SWAG_CHECK(outputNode(child->firstChild()));
            }
        }

        concat->addEol();
    }

    indent--;
    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();

    return true;
}

bool FormatAst::outputGenericParameters(const AstNode* node)
{
    concat->addChar('(');

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

        concat->addString(child->token.text);

        const AstVarDecl* varDecl = castAst<AstVarDecl>(child, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(varDecl->type));
        }
        else if (varDecl->typeConstraint)
        {
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(varDecl->typeConstraint));
        }

        if (varDecl->assignment)
        {
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            SWAG_CHECK(outputNode(varDecl->assignment));
        }

        first = false;
    }

    concat->addChar(')');
    return true;
}
