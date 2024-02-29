#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputAttrUse(const AstNode* node, bool& hasSomething)
{
    const auto nodeAttr = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);

    // Be sure this is not an empty attribute block (empty or that contains
    // other empty blocks)
    hasSomething  = true;
    auto scanAttr = nodeAttr;
    while (scanAttr->content && scanAttr->content->is(AstNodeKind::Statement))
    {
        if (scanAttr->content->children.empty())
        {
            hasSomething = false;
            break;
        }
        if (scanAttr->content->childCount() > 1)
            break;
        if (scanAttr->content->firstChild()->isNot(AstNodeKind::AttrUse))
            break;
        scanAttr = castAst<AstAttrUse>(scanAttr->content->firstChild());
    }

    if (!hasSomething)
        return true;

    bool first = true;
    for (const auto s : nodeAttr->children)
    {
        if (s == nodeAttr->content)
            continue;
        if (s->is(AstNodeKind::AttrUse))
            continue;

        if (!first)
            CONCAT_FIXED_STR(concat, ", ");
        else
            CONCAT_FIXED_STR(concat, "#[");
        first = false;
        SWAG_CHECK(outputNode(s));
    }

    if (!first)
    {
        concat->addChar(']');
    }

    return true;
}

bool FormatAst::outputAttributesUsage(const TypeInfoFuncAttr* typeFunc) const
{
    bool first = true;
    concat->addIndent(indent);
    concat->addString("#[AttrUsage(");

#define ADD_ATTR_USAGE(__f, __n)                         \
    do                                                   \
    {                                                    \
        if (typeFunc->attributeUsage & (int) (__f))      \
        {                                                \
            if (!first)                                  \
                CONCAT_FIXED_STR(concat, "|");           \
            first = false;                               \
            CONCAT_FIXED_STR(concat, "AttributeUsage."); \
            CONCAT_FIXED_STR(concat, __n);               \
        }                                                \
    } while (0)

    ADD_ATTR_USAGE(AttributeUsage::Enum, "Enum");
    ADD_ATTR_USAGE(AttributeUsage::EnumValue, "EnumValue");
    ADD_ATTR_USAGE(AttributeUsage::StructVariable, "Field");
    ADD_ATTR_USAGE(AttributeUsage::GlobalVariable, "GlobalVariable");
    ADD_ATTR_USAGE(AttributeUsage::Variable, "Variable");
    ADD_ATTR_USAGE(AttributeUsage::Struct, "Struct");
    ADD_ATTR_USAGE(AttributeUsage::Function, "Function");
    ADD_ATTR_USAGE(AttributeUsage::FunctionParameter, "FunctionParameter");
    ADD_ATTR_USAGE(AttributeUsage::File, "File");
    ADD_ATTR_USAGE(AttributeUsage::Constant, "Constant");

    ADD_ATTR_USAGE(AttributeUsage::Multi, "Multi");
    ADD_ATTR_USAGE(AttributeUsage::Gen, "Gen");
    ADD_ATTR_USAGE(AttributeUsage::All, "All");

    concat->addString(")]");
    concat->addEol();
    return true;
}

bool FormatAst::outputAttributes(const AstNode* /*node*/, const TypeInfo* typeInfo, const AttributeList& attributes)
{
    const auto attr = &attributes;
    if (!attr->empty())
    {
        Set<AstNode*> done;
        bool          first = true;
        for (auto& one : attr->allAttributes)
        {
            // Be sure usage is valid
            if (typeInfo->isStruct() && one.typeFunc && !(one.typeFunc->attributeUsage & (All | Struct)))
                continue;
            if (typeInfo->isInterface() && one.typeFunc && !(one.typeFunc->attributeUsage & (All | Struct)))
                continue;
            if (typeInfo->isFuncAttr() && one.typeFunc && !(one.typeFunc->attributeUsage & (All | Function)))
                continue;
            if (typeInfo->isEnum() && one.typeFunc && !(one.typeFunc->attributeUsage & (All | Enum)))
                continue;

            if (one.node)
            {
                if (done.contains(one.node))
                    continue;
                done.insert(one.node);
                concat->addIndent(indent);
                bool hasSomething = true;
                SWAG_CHECK(outputAttrUse(one.node, hasSomething));
                concat->addEol();
            }
            else
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                else
                {
                    first = false;
                    concat->addIndent(indent);
                    CONCAT_FIXED_STR(concat, "#[");
                }

                concat->addString(one.name);
                if (!one.parameters.empty())
                {
                    concat->addChar('(');

                    for (uint32_t i = 0; i < one.parameters.size(); i++)
                    {
                        auto& oneParam = one.parameters[i];
                        if (i)
                            CONCAT_FIXED_STR(concat, ", ");
                        SWAG_CHECK(outputLiteral(one.node, oneParam.typeInfo, oneParam.value));
                    }

                    concat->addChar(')');
                }
            }
        }

        if (!first)
        {
            CONCAT_FIXED_STR(concat, "]");
            concat->addEol();
        }
    }

    return true;
}

bool FormatAst::outputAttributesGlobalUsing(const AstNode* node) const
{
    // Global using
    bool outputUsing = true;
    if (node->hasAstFlag(AST_STRUCT_MEMBER))
        outputUsing = false;
    if (outputUsing)
    {
        bool one = false;
        for (const auto& p : node->token.sourceFile->globalUsing)
        {
            if (p->getFullName() == "Swag")
                continue;

            if (!one)
            {
                concat->addIndent(indent);
                CONCAT_FIXED_STR(concat, "#[Using(");
                one = true;
            }
            else
            {
                CONCAT_FIXED_STR(concat, ", ");
            }

            concat->addString(p->getFullName());
        }

        if (one)
        {
            CONCAT_FIXED_STR(concat, ")]");
            concat->addEol();
        }
    }

    return true;
}

bool FormatAst::outputAttributes(const AstNode* node, TypeInfo* typeInfo)
{
    switch (node->kind)
    {
        case AstNodeKind::ConstDecl:
        case AstNodeKind::NameAlias:
            return true;
    }

    SWAG_CHECK(outputAttributesGlobalUsing(node));

    const AttributeList* attr = nullptr;
    switch (typeInfo->kind)
    {
        case TypeInfoKind::Struct:
        case TypeInfoKind::Interface:
        {
            const auto type = castTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
            attr            = &type->attributes;
            break;
        }
        case TypeInfoKind::FuncAttr:
        {
            const auto type = castTypeInfo<TypeInfoFuncAttr>(typeInfo, typeInfo->kind);
            attr            = &type->attributes;
            break;
        }
        case TypeInfoKind::Enum:
        {
            const auto type = castTypeInfo<TypeInfoEnum>(typeInfo, typeInfo->kind);
            attr            = &type->attributes;
            break;
        }
    }

    if (!attr)
        return true;
    SWAG_CHECK(outputAttributes(node, typeInfo, *attr));

    return true;
}
