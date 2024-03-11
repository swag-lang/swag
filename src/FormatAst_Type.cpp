#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputTypeExpression(const AstNode* node)
{
    if (node->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
    {
        CONCAT_FIXED_STR(concat, "#type");
        concat->addBlank();
    }

    SWAG_CHECK(outputType(castAst<AstTypeExpression>(node)));
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
                        concat->addBlank();
                        concat->addChar('=');
                        concat->addBlank();
                    }

                    concat->addChar('{');
                    SWAG_CHECK(outputNode(id->callParameters));
                    concat->addChar('}');
                }
            }

            return true;
        }
    }

    if (node->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }

    if (node->arrayDim == UINT8_MAX)
    {
        CONCAT_FIXED_STR(concat, "[]");
        concat->addBlank();
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        uint32_t i = 0;
        for (; i < node->arrayDim; i++)
        {
            if (i)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            SWAG_CHECK(outputNode(node->children[i]));
        }

        concat->addChar(']');
        concat->addBlank();
        SWAG_CHECK(outputNode(node->children[i]));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_SLICE))
    {
        CONCAT_FIXED_STR(concat, "[..]");
        concat->addBlank();
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

    if (node->typeFlags.has(TYPEFLAG_IS_REF) && node->typeFlags.has(TYPEFLAG_IS_MOVE_REF))
    {
        CONCAT_FIXED_STR(concat, "&&");
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->typeFlags.has(TYPEFLAG_IS_REF))
    {
        concat->addChar('&');
        SWAG_CHECK(outputNode(node->firstChild()));
        return true;
    }

    if (node->identifier)
    {
        SWAG_CHECK(outputNode(node->identifier));
    }
    else if (node->literalType == LiteralType::TypeType)
    {
        CONCAT_FIXED_STR(concat, "typeinfo");
        if (node->childCount())
            SWAG_CHECK(outputNode(node->firstChild()));
    }
    else if (node->typeFromLiteral == g_TypeMgr->typeInfoVariadic)
    {
        if (node->childCount())
            SWAG_CHECK(outputNode(node->firstChild()));
        CONCAT_FIXED_STR(concat, "...");
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->literalType);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat->addString(typeFromLiteral->name);
        if (node->childCount())
            SWAG_CHECK(outputNode(node->firstChild()));
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
    uint32_t first = 0;
    if (node->firstChild() && node->firstChild()->childCount())
    {
        first = 1;
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
    for (uint32_t it = first; it < node->children.size(); it++)
    {
        const auto child = convertNode(node->children[it]);
        if (!child)
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
            SWAG_CHECK(outputNode(child));
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
