#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool FormatAst::outputAttrUse(FormatContext& context, AstNode* node, bool& hasSomething)
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
        const auto child = convertNode(context, s);
        if (!child)
            continue;
        if (child->is(AstNodeKind::AttrUse))
            continue;

        if (first)
        {
            concat->addStringView("#[");
            first = false;
        }
        else
        {
            concat->addChar(',');
            concat->addBlank();
        }

        SWAG_CHECK(outputNode(context, child));
    }

    if (!first)
        concat->addChar(']');

    return true;
}

bool FormatAst::outputAttrUse(FormatContext& context, AstAttrUse* node)
{
    if (node->attributeFlags.has(ATTRIBUTE_PUBLIC))
    {
        if (node->hasAstFlag(AST_GLOBAL_NODE))
        {
            concat->addStringView("#global");
            concat->addBlank();
            concat->addStringView("public");
            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputChildrenEol(context, node->content));
        }
        else
        {
            concat->addStringView("public");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->content));
        }
        return true;
    }

    if (node->attributeFlags.has(ATTRIBUTE_PRIVATE))
    {
        if (node->hasAstFlag(AST_GLOBAL_NODE))
        {
            concat->addStringView("#global");
            concat->addBlank();
            concat->addStringView("private");
            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputChildrenEol(context, node->content));
        }
        else
        {
            concat->addStringView("private");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->content));
        }
        return true;
    }

    if (node->attributeFlags.has(ATTRIBUTE_INTERNAL))
    {
        if (node->hasAstFlag(AST_GLOBAL_NODE))
        {
            concat->addStringView("#global");
            concat->addBlank();
            concat->addStringView("internal");
            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputChildrenEol(context, node->content));
        }
        else
        {
            concat->addStringView("internal");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->content));
        }
        return true;
    }

    if (node->hasAstFlag(AST_GLOBAL_NODE))
    {
        concat->addStringView("#global");
        concat->addBlank();
        bool hasSomething = true;
        SWAG_CHECK(outputAttrUse(context, node, hasSomething));
        concat->addEol();
        concat->addIndent(context.indent);
        SWAG_CHECK(outputChildrenEol(context, node->content));
        return true;
    }

    bool hasSomething = true;
    SWAG_CHECK(outputAttrUse(context, node, hasSomething));
    if (!hasSomething)
        return true;
    beautifyAfter(context, node);
    concat->addEol();
    concat->addIndent(context.indent);
    SWAG_CHECK(outputNode(context, node->content));
    return true;
}

bool FormatAst::outputAttributesUsage(const FormatContext& context, const TypeInfoFuncAttr* typeFunc) const
{
    bool first = true;
    concat->addIndent(context.indent);
    concat->addStringView("#[AttrUsage(");

#define ADD_ATTR_USAGE(__f, __n)                         \
    do                                                   \
    {                                                    \
        if (typeFunc->attributeUsage.has(__f))           \
        {                                                \
            if (!first)                                  \
                concat->addChar('|');                    \
            first = false;                               \
            concat->addStringView("AttributeUsage."); \
            concat->addStringView(__n);               \
        }                                                \
    } while (0)

    ADD_ATTR_USAGE(ATTR_USAGE_ENUM, "Enum");
    ADD_ATTR_USAGE(ATTR_USAGE_ENUM_VALUE, "EnumValue");
    ADD_ATTR_USAGE(ATTR_USAGE_STRUCT_VAR, "Field");
    ADD_ATTR_USAGE(ATTR_USAGE_GLOBAL_VAR, "GlobalVariable");
    ADD_ATTR_USAGE(ATTR_USAGE_VAR, "Variable");
    ADD_ATTR_USAGE(ATTR_USAGE_STRUCT, "Struct");
    ADD_ATTR_USAGE(ATTR_USAGE_FUNC, "Function");
    ADD_ATTR_USAGE(ATTR_USAGE_FUNC_PARAM, "FunctionParameter");
    ADD_ATTR_USAGE(ATTR_USAGE_FILE, "File");
    ADD_ATTR_USAGE(ATTR_USAGE_CONSTANT, "Constant");
    ADD_ATTR_USAGE(ATTR_USAGE_MULTI, "Multi");
    ADD_ATTR_USAGE(ATTR_USAGE_GEN, "Gen");
    ADD_ATTR_USAGE(ATTR_USAGE_ALL, "All");

    concat->addStringView(")]");
    concat->addEol();
    return true;
}

bool FormatAst::outputAttributes(FormatContext& context, const TypeInfo* typeInfo, const AttributeList& attributes)
{
    const auto attr = &attributes;
    if (attr->empty())
        return true;

    Set<AstNode*> done;
    bool          first = true;

    for (auto& one : attr->allAttributes)
    {
        // Be sure usage is valid
        if (typeInfo->isStruct() && one.type && !one.type->attributeUsage.has(ATTR_USAGE_ALL | ATTR_USAGE_STRUCT))
            continue;
        if (typeInfo->isInterface() && one.type && !one.type->attributeUsage.has(ATTR_USAGE_ALL | ATTR_USAGE_STRUCT))
            continue;
        if (typeInfo->isFuncAttr() && one.type && !one.type->attributeUsage.has(ATTR_USAGE_ALL | ATTR_USAGE_FUNC))
            continue;
        if (typeInfo->isEnum() && one.type && !one.type->attributeUsage.has(ATTR_USAGE_ALL | ATTR_USAGE_ENUM))
            continue;

        if (one.node)
        {
            if (done.contains(one.node))
                continue;
            done.insert(one.node);
            concat->addIndent(context.indent);
            bool hasSomething = true;
            SWAG_CHECK(outputAttrUse(context, one.node, hasSomething));
            concat->addEol();
            continue;
        }

        if (!first)
        {
            concat->addChar(',');
            concat->addBlank();
        }
        else
        {
            first = false;
            concat->addIndent(context.indent);
            concat->addStringView("#[");
        }

        concat->addString(one.name);
        if (!one.parameters.empty())
        {
            concat->addChar('(');

            for (uint32_t i = 0; i < one.parameters.size(); i++)
            {
                auto& oneParam = one.parameters[i];
                if (i)
                {
                    concat->addChar(',');
                    concat->addBlank();
                }

                SWAG_CHECK(outputLiteral(context, one.node, oneParam.typeInfo, oneParam.value));
            }

            concat->addChar(')');
        }
    }

    if (!first)
    {
        concat->addStringView("]");
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputAttributesGlobalUsing(const FormatContext& context, const AstNode* node) const
{
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
                concat->addIndent(context.indent);
                concat->addStringView("#[Using(");
                one = true;
            }
            else
            {
                concat->addChar(',');
                concat->addBlank();
            }

            concat->addString(p->getFullName());
        }

        if (one)
        {
            concat->addStringView(")]");
            concat->addEol();
        }
    }

    return true;
}

bool FormatAst::outputAttributes(FormatContext& context, const AstNode* node, TypeInfo* typeInfo)
{
    switch (node->kind)
    {
        case AstNodeKind::ConstDecl:
        case AstNodeKind::NameAlias:
            return true;
    }

    SWAG_CHECK(outputAttributesGlobalUsing(context, node));

    if(!typeInfo)
        return true;

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
    SWAG_CHECK(outputAttributes(context, typeInfo, *attr));

    return true;
}
