#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"

void SemanticJob::propagateAttributes(AstNode* child)
{
    if (!child->parent)
        return;
    child->attributeFlags |= child->parent->attributeFlags & (ATTRIBUTE_SAFETY_OFF | ATTRIBUTE_SAFETY_ON | ATTRIBUTE_NO_RETURN);
}

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode)
{
    if (!checkNode)
        return true;

    if (checkNode->kind == AstNodeKind::Statement)
    {
        for (auto s : checkNode->childs)
            SWAG_CHECK(checkAttribute(context, oneAttribute, s));
        return true;
    }

    if (checkNode->kind == AstNodeKind::AttrUse)
    {
        auto attrUse = CastAst<AstAttrUse>(checkNode, AstNodeKind::AttrUse);
        SWAG_CHECK(checkAttribute(context, oneAttribute, attrUse->content));
        return true;
    }

    SWAG_ASSERT(oneAttribute->typeInfo);
    if (oneAttribute->typeInfo->kind != TypeInfoKind::FuncAttr)
        return context->report({oneAttribute, format("'%s' is not a valid attribute, it's %s", oneAttribute->typeInfo->name.c_str(), TypeInfo::getArticleKindName(oneAttribute->typeInfo))});

    auto kind     = checkNode->kind;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage == AttributeUsage::All)
        return true;

    // Check specific hard coded attributes
    SWAG_ASSERT(oneAttribute->typeInfo->declNode);
    if (oneAttribute->typeInfo->declNode->sourceFile->isBootstrapFile)
    {
        if (oneAttribute->token.text == "complete" && kind == AstNodeKind::Switch)
            return true;
        if (oneAttribute->token.text == "attributeUsage" && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == "attributeMulti" && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == "noreturn" && kind == AstNodeKind::CompilerMixin)
            return true;
        if (oneAttribute->token.text == "global" && kind == AstNodeKind::VarDecl)
            if (!checkNode->ownerScope->isGlobalOrImpl())
                return true;
        if (oneAttribute->token.text == "strict" && kind == AstNodeKind::Alias)
            return true;
    }

    if ((typeInfo->attributeUsage & AttributeUsage::Function) && (kind == AstNodeKind::FuncDecl))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::Struct) && (kind == AstNodeKind::StructDecl))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::Enum) && (kind == AstNodeKind::EnumDecl))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::EnumValue) && (kind == AstNodeKind::EnumValue))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::Field) && (kind == AstNodeKind::VarDecl))
        if (checkNode->ownerMainNode && checkNode->ownerMainNode->kind == AstNodeKind::StructDecl)
            return true;

    if ((typeInfo->attributeUsage & AttributeUsage::GlobalVariable) && (kind == AstNodeKind::VarDecl))
        if (checkNode->ownerScope->isGlobalOrImpl())
            return true;

    auto nakedName = AstNode::getArticleKindName(checkNode);
    if (nakedName == "<node>")
    {
        Diagnostic diag{oneAttribute, format("attribute '%s' cannot be used in that context", oneAttribute->token.text.c_str())};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1);
    }
    else
    {
        Diagnostic diag{oneAttribute, format("attribute '%s' cannot be applied to %s", oneAttribute->token.text.c_str(), nakedName.c_str())};
        Diagnostic note1{checkNode, checkNode->token, format("this is the %s", nakedName.c_str()), DiagnosticLevel::Note};
        Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1, &note2);
    }
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes* result)
{
    auto attrUse = forNode->ownerAttrUse;
    if (!attrUse)
        return true;
    SWAG_CHECK(collectAttributes(context, forNode, result, attrUse));
    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes* result, AstAttrUse* attrUse)
{
    if (!attrUse)
        return true;

    // Already done
    if (result && !result->empty())
        return true;

    auto&         flags   = forNode->attributeFlags;
    auto          curAttr = attrUse;
    ComputedValue value;
    while (curAttr)
    {
        // Inherit flags
        auto curFlags = curAttr->attributeFlags;
        if (flags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE | ATTRIBUTE_PROTECTED))
            curFlags &= ~(ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE | ATTRIBUTE_PROTECTED);
        flags |= curFlags;

        for (auto child : curAttr->childs)
        {
            if (child == curAttr->content)
                continue;
            if (!child->typeInfo || child->typeInfo->kind != TypeInfoKind::FuncAttr)
                continue;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute("swag.attributeMulti"))
            {
                if (result && result->isHere.contains(typeInfo))
                {
                    Diagnostic diag{forNode, forNode->token, format("attribute '%s' assigned twice to '%s' ('swag.attributeMulti' is not present in the declaration of '%s')", child->token.text.c_str(), forNode->token.text.c_str(), child->token.text.c_str())};
                    Diagnostic note{child, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
            }

            // Append attributes
            if (result)
                result->isHere.insert(typeInfo);

            // Attribute on an attribute : usage
            if (forNode->kind == AstNodeKind::AttrDecl)
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
            if (child->token.text == "constexpr")
                flags |= ATTRIBUTE_CONSTEXPR;
            else if (child->token.text == "printbc")
                flags |= ATTRIBUTE_PRINT_BC;
            else if (child->token.text == "test")
                flags |= ATTRIBUTE_TEST_FUNC;
            else if (child->token.text == "compiler")
                flags |= ATTRIBUTE_COMPILER;
            else if (child->token.text == "enumflags")
                flags |= ATTRIBUTE_ENUM_FLAGS;
            else if (child->token.text == "enumindex")
                flags |= ATTRIBUTE_ENUM_INDEX;
            else if (child->token.text == "foreign")
                flags |= ATTRIBUTE_FOREIGN;
            else if (child->token.text == "inline")
                flags |= ATTRIBUTE_INLINE;
            else if (child->token.text == "macro")
                flags |= ATTRIBUTE_MACRO;
            else if (child->token.text == "mixin")
                flags |= ATTRIBUTE_MIXIN;
            else if (child->token.text == "complete")
                flags |= ATTRIBUTE_COMPLETE;
            else if (child->token.text == "property")
                flags |= ATTRIBUTE_PROPERTY;
            else if (child->token.text == "nobss")
                flags |= ATTRIBUTE_NO_BSS;
            else if (child->token.text == "noreturn")
                flags |= ATTRIBUTE_NO_RETURN;
            else if (child->token.text == "global")
                flags |= ATTRIBUTE_GLOBAL;
            else if (child->token.text == "strict")
                flags |= ATTRIBUTE_STRICT;
            else if (child->token.text == "callback")
                flags |= ATTRIBUTE_CALLBACK;
            else if (child->token.text == "safety")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("swag.safety", "value", attrValue);
                flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_ON : ATTRIBUTE_SAFETY_OFF;
            }
            else if (child->token.text == "optim")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("swag.optim", "value", attrValue);
                flags |= attrValue.reg.b ? ATTRIBUTE_OPTIM_ON : ATTRIBUTE_OPTIM_OFF;
            }
            else if (child->token.text == "pack")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("swag.pack", "value", attrValue);
                SWAG_VERIFY(attrValue.reg.u8 <= 8, context->report({child, format("'swag.pack' value must be in the range [0, 8] ('%d' provided)", attrValue.reg.u8)}));
            }
        }

        // Merge the result
        if (result)
        {
            for (auto& oneAttr : curAttr->attributes.attributes)
                result->attributes.push_back(oneAttr);
        }

        curAttr = curAttr->ownerAttrUse;
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

    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));
    return true;
}

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node     = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Attribute));

    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicAttribute(node);
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node = CastAst<AstAttrUse>(context->node->parent, AstNodeKind::AttrUse);
    SWAG_VERIFY(node->content, context->report({node, "invalid attribute usage"}));

    for (auto child : node->childs)
    {
        if (child == node->content)
            continue;

        SWAG_CHECK(checkAttribute(context, child, node->content));

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
            attrParam.name     = param->token.text;
            attrParam.typeInfo = param->typeInfo;
            attrParam.value    = param->assignment->computedValue;
            oneAttribute.parameters.emplace_back(move(attrParam));
        }

        node->attributes.attributes.emplace_back(move(oneAttribute));
    }

    SWAG_CHECK(collectAttributes(context, node, nullptr, node));
    return true;
}
