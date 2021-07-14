#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "ErrorIds.h"

void SemanticJob::propagateAttributes(AstNode* child)
{
    if (!child->parent)
        return;
    child->attributeFlags |= child->parent->attributeFlags &
                             (ATTRIBUTE_SAFETY_MASK |
                              ATTRIBUTE_SELECTIF_OFF |
                              ATTRIBUTE_SELECTIF_ON);
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
        return context->report({oneAttribute, format(Msg0582, oneAttribute->typeInfo->getDisplayName().c_str(), TypeInfo::getArticleKindName(oneAttribute->typeInfo))});

    auto kind     = checkNode->kind;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage == AttributeUsage::All)
        return true;

    bool isGlobalVar = kind == AstNodeKind::VarDecl && checkNode->ownerScope->isGlobalOrImpl();
    bool isStructVar = kind == AstNodeKind::VarDecl && checkNode->ownerMainNode && checkNode->ownerMainNode->kind == AstNodeKind::StructDecl;
    bool isLocalVar  = kind == AstNodeKind::VarDecl && !isGlobalVar && !isStructVar;

    // Check specific hard coded attributes
    SWAG_ASSERT(oneAttribute->typeInfo->declNode);
    if (oneAttribute->typeInfo->declNode->sourceFile->isBootstrapFile)
    {
        if (oneAttribute->token.text == "Complete" && kind == AstNodeKind::Switch)
            return true;
        if (oneAttribute->token.text == "AttrUsage" && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == "AttrMulti" && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == "global" && isLocalVar)
            return true;
        if (oneAttribute->token.text == "Strict" && kind == AstNodeKind::Alias)
            return true;
        if (oneAttribute->token.text == "PrintBc" && kind == AstNodeKind::CompilerAst)
            return true;
        if (oneAttribute->token.text == "align" && (kind == AstNodeKind::VarDecl || kind == AstNodeKind::StructDecl))
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

    if ((typeInfo->attributeUsage & AttributeUsage::Variable) && (kind == AstNodeKind::VarDecl))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::Constant) && (kind == AstNodeKind::ConstDecl))
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::StructVariable) && isStructVar)
        return true;

    if ((typeInfo->attributeUsage & AttributeUsage::GlobalVariable) && isGlobalVar)
        return true;

    const char* specificMsg = nullptr;
    switch (typeInfo->attributeUsage)
    {
    case AttributeUsage::Function:
        specificMsg = "a function";
        break;
    case AttributeUsage::Struct:
        specificMsg = "a struct";
        break;
    case AttributeUsage::Enum:
        specificMsg = "an enum";
        break;
    case AttributeUsage::EnumValue:
        specificMsg = "an enum value";
        break;
    case AttributeUsage::StructVariable:
        specificMsg = "a struct variable";
        break;
    case AttributeUsage::GlobalVariable:
        specificMsg = "a global variable";
        break;
    case AttributeUsage::Constant:
        specificMsg = "a constant";
        break;
    }

    if (specificMsg)
    {
        auto       nakedName = AstNode::getKindName(checkNode);
        Diagnostic diag{oneAttribute, format(Msg0583, oneAttribute->token.text.c_str(), specificMsg)};
        Diagnostic note1{checkNode, checkNode->token, format(Msg0584, nakedName.c_str()), DiagnosticLevel::Note};
        Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1, &note2);
    }
    else
    {
        auto nakedName = AstNode::getArticleKindName(checkNode);
        if (nakedName == "<node>")
        {
            Diagnostic diag{oneAttribute, format(Msg0586, oneAttribute->token.text.c_str())};
            Diagnostic note1{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note1);
        }
        else
        {
            auto       nakedName1 = AstNode::getKindName(checkNode);
            Diagnostic diag{oneAttribute, format(Msg0588, oneAttribute->token.text.c_str(), nakedName.c_str())};
            Diagnostic note1{checkNode, checkNode->token, format(Msg0589, nakedName1.c_str()), DiagnosticLevel::Note};
            Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note1, &note2);
        }
    }
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes* result)
{
    auto attrUse = forNode->ownerAttrUse;
    SWAG_CHECK(collectAttributes(context, forNode, result, attrUse));
    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, SymbolAttributes* result, AstAttrUse* attrUse)
{
    if (!attrUse)
        attrUse = forNode->sourceFile->astAttrUse;
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
            if (!typeInfo->attributes.hasAttribute("Swag.AttrMulti"))
            {
                if (result && result->isHere.contains(typeInfo))
                {
                    Diagnostic diag{forNode, forNode->token, format(Msg0591, child->token.text.c_str(), forNode->token.text.c_str(), child->token.text.c_str())};
                    Diagnostic note{child, child->token, Msg0592, DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }
            }

            // Attribute on an attribute : usage
            if (forNode->kind == AstNodeKind::AttrDecl)
            {
                if (curAttr->attributes.getValue("Swag.AttrUsage", "usage", value))
                {
                    auto typeAttr            = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage = value.reg.u32;
                }

                if (curAttr->attributes.hasAttribute("Swag.AttrMulti"))
                {
                    auto typeAttr = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage |= AttributeUsage::Multi;
                }
            }

            // Predefined attributes will mark some flags (to speed up detection)
            if (child->token.text == "ConstExpr")
                flags |= ATTRIBUTE_CONSTEXPR;
            else if (child->token.text == "PrintBc")
                flags |= ATTRIBUTE_PRINT_BC;
            else if (child->token.text == "test")
                flags |= ATTRIBUTE_TEST_FUNC;
            else if (child->token.text == "compiler")
                flags |= ATTRIBUTE_COMPILER;
            else if (child->token.text == "EnumFlags")
                flags |= ATTRIBUTE_ENUM_FLAGS;
            else if (child->token.text == "EnumIndex")
                flags |= ATTRIBUTE_ENUM_INDEX;
            else if (child->token.text == "foreign")
                flags |= ATTRIBUTE_FOREIGN;
            else if (child->token.text == "Inline")
                flags |= ATTRIBUTE_INLINE;
            else if (child->token.text == "macro")
                flags |= ATTRIBUTE_MACRO;
            else if (child->token.text == "mixin")
                flags |= ATTRIBUTE_MIXIN;
            else if (child->token.text == "Complete")
                flags |= ATTRIBUTE_COMPLETE;
            else if (child->token.text == "Implicit")
                flags |= ATTRIBUTE_IMPLICIT;
            else if (child->token.text == "NoReturn")
                flags |= ATTRIBUTE_NO_RETURN;
            else if (child->token.text == "Discardable")
                flags |= ATTRIBUTE_DISCARDABLE;
            else if (child->token.text == "Deprecated")
                flags |= ATTRIBUTE_DEPRECATED;
            else if (child->token.text == "global")
                flags |= ATTRIBUTE_GLOBAL;
            else if (child->token.text == "tls")
                flags |= ATTRIBUTE_TLS;
            else if (child->token.text == "Strict")
                flags |= ATTRIBUTE_STRICT;
            else if (child->token.text == "Callback")
                flags |= ATTRIBUTE_CALLBACK;
            else if (child->token.text == "NoCopy")
                flags |= ATTRIBUTE_NO_COPY;
            else if (child->token.text == "Opaque")
                flags |= ATTRIBUTE_OPAQUE;
            else if (child->token.text == "Safety")
            {
                ComputedValue attrWhat;
                vector<Utf8>  what;
                curAttr->attributes.getValue("Swag.Safety", "what", attrWhat);
                attrWhat.text.trim();
                tokenize(attrWhat.text, '|', what);

                ComputedValue attrValue;
                curAttr->attributes.getValue("Swag.Safety", "value", attrValue);

                if (attrWhat.text.empty())
                {
                    flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_NULLPTR_ON : ATTRIBUTE_SAFETY_NULLPTR_OFF;
                    flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_CASTANY_ON : ATTRIBUTE_SAFETY_CASTANY_OFF;
                }

                for (auto& w : what)
                {
                    w.trim();
                    if (w == "nullptr")
                        flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_NULLPTR_ON : ATTRIBUTE_SAFETY_NULLPTR_OFF;
                    else if (w == "boundcheck")
                        flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    else if (w == "overflow")
                        flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    else if (w == "math")
                        flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    else if (w == "castany")
                        flags |= attrValue.reg.b ? ATTRIBUTE_SAFETY_CASTANY_ON : ATTRIBUTE_SAFETY_CASTANY_OFF;
                    else
                        return context->report({child, format(Msg0593, w.c_str())});
                }
            }
            else if (child->token.text == "optim")
            {
                ComputedValue attrWhat;
                vector<Utf8>  what;
                curAttr->attributes.getValue("Swag.optim", "what", attrWhat);
                attrWhat.text.trim();
                tokenize(attrWhat.text, '|', what);

                ComputedValue attrValue;
                curAttr->attributes.getValue("Swag.optim", "value", attrValue);

                if (attrWhat.text.empty())
                {
                    flags |= attrValue.reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF;
                    flags |= attrValue.reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF;
                }

                for (auto& w : what)
                {
                    w.trim();
                    if (w == "bytecode")
                        flags |= attrValue.reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF;
                    else if (w == "backend")
                        flags |= attrValue.reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF;
                    else
                        return context->report({child, format(Msg0594, w.c_str())});
                }
            }
            else if (child->token.text == "SelectIf")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("Swag.SelectIf", "value", attrValue);
                flags |= attrValue.reg.b ? ATTRIBUTE_SELECTIF_ON : ATTRIBUTE_SELECTIF_OFF;
            }
            else if (child->token.text == "pack")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("Swag.pack", "value", attrValue);
                SWAG_VERIFY(!attrValue.reg.u8 || isPowerOfTwo(attrValue.reg.u8), context->report({child, format(Msg0595, attrValue.reg.u8)}));
            }
            else if (child->token.text == "align")
            {
                ComputedValue attrValue;
                curAttr->attributes.getValue("Swag.align", "value", attrValue);
                SWAG_VERIFY(isPowerOfTwo(attrValue.reg.u8), context->report({child, format(Msg0596, attrValue.reg.u8)}));
            }

            // Append attributes
            if (result)
                result->isHere.insert(typeInfo);
        }

        // Merge the result
        if (result)
        {
            for (auto& oneAttr : curAttr->attributes.attributes)
                result->attributes.push_back(oneAttr);
        }

        if (!(curAttr->specFlags & AST_SPEC_ATTRUSE_GLOBAL) && !curAttr->ownerAttrUse)
            curAttr = forNode->sourceFile->astAttrUse;
        else
            curAttr = curAttr->ownerAttrUse;
    }

    return true;
}

bool SemanticJob::preResolveAttrDecl(SemanticContext* context)
{
    auto node     = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
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
    SWAG_VERIFY(node->content || (node->specFlags & AST_SPEC_ATTRUSE_GLOBAL), context->report({node, Msg0597}));

    for (auto child : node->childs)
    {
        if (child == node->content)
            continue;
        if (child->kind != AstNodeKind::IdentifierRef)
            continue;

        if (node->content)
            SWAG_CHECK(checkAttribute(context, child, node->content));

        // Collect parameters
        auto identifierRef = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        auto identifier    = static_cast<AstIdentifier*>(identifierRef->childs.back());

        // Be sure this is an attribute
        auto resolvedName = identifier->resolvedSymbolName;
        auto resolved     = identifier->resolvedSymbolOverload;
        if (resolvedName->kind != SymbolKind::Attribute)
        {
            Diagnostic diag{identifier, format(Msg0598, resolvedName->name.c_str())};
            Diagnostic note{resolved->node, resolved->node->token, format(Msg0018, resolvedName->name.c_str()), DiagnosticLevel::Note};
            context->report(diag, &note);
            return false;
        }

        // Check that global attribute is authorized
        if (node->specFlags & AST_SPEC_ATTRUSE_GLOBAL)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeInfo->attributeUsage & AttributeUsage::File))
            {
                Diagnostic diag{identifier, format(Msg0600, resolvedName->name.c_str())};
                Diagnostic note{resolved->node, resolved->node->token, format(Msg0018, resolvedName->name.c_str()), DiagnosticLevel::Note};
                context->report(diag, &note);
                return false;
            }
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
                SWAG_VERIFY(param->flags & AST_VALUE_COMPUTED, context->report({param, Msg0602}));

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
