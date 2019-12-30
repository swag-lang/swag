#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind)
{
    if (kind == AstNodeKind::Statement)
        return true;

    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_FUNC) && (kind == AstNodeKind::FuncDecl))
        return true;
    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_VAR) && (kind == AstNodeKind::VarDecl || kind == AstNodeKind::LetDecl))
        return true;
    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_STRUCT) && (kind == AstNodeKind::StructDecl))
        return true;
    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_INTERFACE) && kind == AstNodeKind::InterfaceDecl)
        return true;
    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_ENUM) && (kind == AstNodeKind::EnumDecl))
        return true;
    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_ENUMVALUE) && (kind == AstNodeKind::EnumValue))
        return true;

    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_STRUCTVAR) && (kind == AstNodeKind::VarDecl))
    {
        if (checkNode->ownerMainNode && checkNode->ownerMainNode->kind == AstNodeKind::StructDecl)
            return true;
    }

    if ((typeInfo->attributeFlags & TYPEINFO_ATTRIBUTE_GLOBALVAR) && (kind == AstNodeKind::VarDecl || kind == AstNodeKind::LetDecl))
    {
        if (checkNode->ownerScope->isGlobal())
            return true;
    }

    Diagnostic diag{oneAttribute, oneAttribute->token, format("attribute '%s' cannot be applied to %s", oneAttribute->name.c_str(), AstNode::getKindName(checkNode).c_str())};
    Diagnostic note1{checkNode, format("this is the %s", AstNode::getNakedKindName(checkNode).c_str()), DiagnosticLevel::Note};
    Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
    return context->report(diag, &note1, &note2);
}

bool SemanticJob::collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint32_t& flags)
{
    if (!attrUse)
        return true;

    auto          curAttr = attrUse;
    ComputedValue value;
    while (curAttr)
    {
        flags |= curAttr->attributeFlags;

        if ((flags & ATTRIBUTE_INTERNAL) && (flags & ATTRIBUTE_READONLY))
            return context->report({curAttr, "attribute 'internal' and attribute 'readonly' are mutually exclusive"});

        for (auto child : curAttr->childs)
        {
            // Check that the attribute matches the following declaration
            SWAG_CHECK(checkAttribute(context, child, forNode, kind));

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (result.attributes.find(typeInfo) != result.attributes.end())
            {
                Diagnostic diag{forNode, forNode->token, format("attribute '%s' assigned twice to '%s'", child->name.c_str(), forNode->name.c_str())};
                Diagnostic note{child, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                return context->report(diag, &note);
            }

            result.attributes.insert(typeInfo);
            result.values.insert(curAttr->values.begin(), curAttr->values.end());

            // Predefined attributes will mark some flags
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
            else if (child->name == "flags")
                flags |= ATTRIBUTE_FLAGS;
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
        }

        curAttr = curAttr->parentAttributes;
    }

    return true;
}

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node     = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Attribute));
    if (context->result == ContextResult::Pending)
        return true;

    SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Attribute));
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node          = CastAst<AstAttrUse>(context->node, AstNodeKind::AttrUse);
    auto nextStatement = node->childParentIdx < node->parent->childs.size() - 1 ? node->parent->childs[node->childParentIdx + 1] : nullptr;

    if (!nextStatement)
    {
        context->report({node, "attribute belongs to nothing (no valid statement after)"});
        return false;
    }

    AstNodeKind kind = nextStatement ? nextStatement->kind : AstNodeKind::Module;

    ComputedValue dummy;
    dummy.reg.b = true;

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
        dummy.reg.pointer                    = (uint8_t*) node;
        node->values[resolvedName->fullName] = {resolved->typeInfo, dummy};

        // And all its parameters
        if (identifier->callParameters)
        {
            for (auto one : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(one, AstNodeKind::FuncCallParam);
                SWAG_VERIFY(param->flags & AST_VALUE_COMPUTED, context->report({param, "attribute parameter cannot be evaluated at compile time"}));
                string attrFullName        = Scope::makeFullName(identifierRef->resolvedSymbolName->fullName, param->resolvedParameter->namedParam);
                node->values[attrFullName] = {param->typeInfo, param->computedValue};
            }
        }
    }

    if (nextStatement)
    {
        nextStatement->parentAttributes = node;
    }

    return true;
}
