#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match bootstrap.swg
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum AttributeUsage
{
    // Usage
    Enum           = 0x00000001,
    EnumValue      = 0x00000002,
    Field          = 0x00000004,
    GlobalVariable = 0x00000008,
    LocalVariable  = 0x00000010,
    Struct         = 0x00000020,
    Function       = 0x00000040,
    Attribute      = 0x00000080,
    Switch         = 0x00000100,
    All            = 0x0FFFFFFF,
    // Flags
    Multi = 0x80000000,
};

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind)
{
    if (kind == AstNodeKind::Statement)
        return true;
    if (oneAttribute->typeInfo->kind != TypeInfoKind::FuncAttr)
        return context->report({oneAttribute, format("'%s' is not a valid attribute, it's %s", oneAttribute->typeInfo->name.c_str(), TypeInfo::getArticleKindName(oneAttribute->typeInfo))});

    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);
    if (typeInfo->attributeUsage == AttributeUsage::All)
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::Function) && (kind == AstNodeKind::FuncDecl))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Attribute) && (kind == AstNodeKind::AttrDecl))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Struct) && (kind == AstNodeKind::StructDecl))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Enum) && (kind == AstNodeKind::EnumDecl))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::EnumValue) && (kind == AstNodeKind::EnumValue))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Switch) && (kind == AstNodeKind::Switch))
    {
        return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Field) && (kind == AstNodeKind::VarDecl))
    {
        if (checkNode->ownerMainNode && checkNode->ownerMainNode->kind == AstNodeKind::StructDecl)
            return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::GlobalVariable) && (kind == AstNodeKind::VarDecl))
    {
        if (checkNode->ownerScope->isGlobal())
            return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::LocalVariable) && (kind == AstNodeKind::VarDecl))
    {
        if (!checkNode->ownerScope->isGlobal())
            return true;
    }

    auto nakedName = AstNode::getKindName(checkNode);
    if (nakedName == "<node>")
    {
        Diagnostic diag{oneAttribute, format("attribute '%s' cannot be used in that context", oneAttribute->name.c_str())};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1);
    }
    else
    {
        Diagnostic diag{oneAttribute, format("attribute '%s' cannot be applied to %s", oneAttribute->name.c_str(), nakedName.c_str())};
        Diagnostic note1{checkNode, checkNode->token, format("this is the %s", nakedName.c_str()), DiagnosticLevel::Note};
        Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1, &note2);
    }
}

bool SemanticJob::collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint64_t& flags)
{
    // Predefined attributes
    if (kind == AstNodeKind::AttrDecl && context->sourceFile->isBootstrapFile)
    {
        if (forNode->name == "attributeUsage" || forNode->name == "attributeMulti")
        {
            auto typeAttr            = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
            typeAttr->attributeUsage = AttributeUsage::Attribute;
            return true;
        }
    }

    if (!attrUse)
        return true;

    auto          curAttr = attrUse;
    ComputedValue value;
    while (curAttr)
    {
        flags |= curAttr->attributeFlags;
        for (auto child : curAttr->childs)
        {
            // Check that the attribute matches the following declaration
            if (kind != AstNodeKind::AttrUse)
                SWAG_CHECK(checkAttribute(context, child, forNode, kind));

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute("swag.attributeMulti"))
            {
                if (result.isHere.find(typeInfo) != result.isHere.end())
                {
                    Diagnostic diag{forNode, forNode->token, format("attribute '%s' assigned twice to '%s' ('swag.attributeMulti' is not present in the declaration of '%s')", child->name.c_str(), forNode->name.c_str(), child->name.c_str())};
                    Diagnostic note{child, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
            }

            // Append attributes
            result.isHere.insert(typeInfo);

            // Attribute on an attribute : usage
            if (kind == AstNodeKind::AttrDecl)
            {
                if (curAttr->attributes.getValue("swag.attributeUsage", "usage", value))
                {
                    auto typeAttr            = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage = value.reg.u32;
                }

                if (curAttr->attributes.hasAttribute("swag.attributeMulti"))
                {
                    auto typeAttr = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage |= AttributeUsage::Multi;
                }
            }

            // Predefined attributes will mark some flags (to speed up detection)
            if (child->name == "constexpr")
                flags |= ATTRIBUTE_CONSTEXPR;
            else if (child->name == "printbc")
                flags |= ATTRIBUTE_PRINTBYTECODE;
            else if (child->name == "test")
                flags |= ATTRIBUTE_TEST_FUNC;
            else if (child->name == "compiler")
                flags |= ATTRIBUTE_COMPILER;
            else if (child->name == "pack")
                flags |= ATTRIBUTE_PACK;
            else if (child->name == "enumflags")
                flags |= ATTRIBUTE_FLAGS;
            else if (child->name == "enumindex")
                flags |= ATTRIBUTE_INDEX;
            else if (child->name == "foreign")
                flags |= ATTRIBUTE_FOREIGN;
            else if (child->name == "inline")
                flags |= ATTRIBUTE_INLINE;
            else if (child->name == "macro")
                flags |= ATTRIBUTE_MACRO;
            else if (child->name == "mixin")
                flags |= ATTRIBUTE_MIXIN;
            else if (child->name == "complete")
                flags |= ATTRIBUTE_COMPLETE;
            else if (child->name == "property")
                flags |= ATTRIBUTE_PROPERTY;
            else if (child->name == "nobss")
                flags |= ATTRIBUTE_NOBSS;
            else if (child->name == "noreturn")
                flags |= ATTRIBUTE_NORETURN;
            else if (child->name == "global")
                flags |= ATTRIBUTE_GLOBAL;
            else if (child->name == "nooptim")
                flags |= ATTRIBUTE_OPTIMIZEBC_OFF;
            else if (child->name == "safety")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("swag.safety", "value", attrValue);
                flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_ON : ATTRIBUTE_SAFETY_OFF;
            }
        }

        // Merge the result
        for (auto& oneAttr : curAttr->attributes.attributes)
            result.attributes.push_back(oneAttr);

        // No hierarchy
        if (kind == AstNodeKind::Switch)
            break;

        curAttr = curAttr->parentAttributes;
    }

    return true;
}

bool SemanticJob::preResolveAttrDecl(SemanticContext* context)
{
    auto node     = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Attribute));
    if (context->result == ContextResult::Pending)
        return true;

    SWAG_CHECK(collectAttributes(context, typeInfo->attributes, node->parentAttributes, node, AstNodeKind::AttrDecl, node->attributeFlags));
    return true;
}

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node     = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Attribute));
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node = CastAst<AstAttrUse>(context->node, AstNodeKind::AttrUse);

    // Attribute use must not be the last one, because we need a valid statement after it
    if (node->childParentIdx >= node->parent->childs.size() - 1)
    {
        context->report({node, "attribute belongs to nothing (no valid statement after)"});
        return false;
    }

    // Collect all attributes next to this one
    auto idx           = node->childParentIdx + 1;
    auto nextStatement = node->parent->childs[idx];
    while (nextStatement->kind == AstNodeKind::AttrUse)
    {
        idx++;
        if (idx >= node->parent->childs.size())
            return context->report({node, "attribute belongs to nothing (no valid statement after)"});
        nextStatement = node->parent->childs[idx];
    }

    AstNodeKind kind = nextStatement ? nextStatement->kind : AstNodeKind::Module;

    for (auto child : node->childs)
    {
        SWAG_CHECK(checkAttribute(context, child, nextStatement, kind));

        // Collect parameters
        auto identifierRef = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        auto identifier    = static_cast<AstIdentifier*>(identifierRef->childs.back());

        // Be sure this is an attribute
        auto resolvedName = identifier->resolvedSymbolName;
        auto resolved     = identifier->resolvedSymbolOverload;
        if (resolvedName->kind != SymbolKind::Attribute)
        {
            Diagnostic diag{identifier, format("invalid attribute '%s'", resolvedName->name.c_str())};
            Diagnostic note{resolved->node, resolved->node->token, format("this is the definition of '%s'", resolvedName->name.c_str()), DiagnosticLevel::Note};
            context->report(diag, &note);
            return false;
        }

        // Register attribute itself
        OneAttribute oneAttribute;
        oneAttribute.name = resolvedName->getFullName();
        oneAttribute.node = node;

        // Register all call parameters, and their value
        uint32_t numParams = 0;
        if (identifier->callParameters)
        {
            numParams = identifier->callParameters->childs.count;
            for (auto one : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(one, AstNodeKind::FuncCallParam);
                SWAG_VERIFY(param->flags & AST_VALUE_COMPUTED, context->report({param, "attribute parameter cannot be evaluated at compile time"}));

                AttributeParameter attrParam;
                attrParam.name     = param->resolvedParameter->namedParam;
                attrParam.typeInfo = param->resolvedParameter->typeInfo;
                attrParam.value    = param->computedValue;
                oneAttribute.parameters.emplace_back(move(attrParam));
            }
        }

        // The rest (default parameters)
        auto funcDecl = CastAst<AstAttrDecl>(resolved->node, AstNodeKind::AttrDecl);
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(resolved->typeInfo, TypeInfoKind::FuncAttr);
        for (int i = numParams; i < (int) typeFunc->parameters.size(); i++)
        {
            auto param = CastAst<AstVarDecl>(funcDecl->parameters->childs[i], AstNodeKind::FuncDeclParam);
            SWAG_ASSERT(param->assignment);

            AttributeParameter attrParam;
            attrParam.name     = param->name;
            attrParam.typeInfo = param->typeInfo;
            attrParam.value    = param->assignment->computedValue;
            oneAttribute.parameters.emplace_back(move(attrParam));
        }

        node->attributes.attributes.emplace_back(move(oneAttribute));
    }

    if (nextStatement)
        nextStatement->parentAttributes = node;

    SymbolAttributes attributes;
    SWAG_CHECK(collectAttributes(context, attributes, node, node, AstNodeKind::AttrUse, node->attributeFlags));
    return true;
}
