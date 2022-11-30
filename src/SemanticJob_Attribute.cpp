#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Math.h"
#include "LanguageSpec.h"

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode)
{
    if (!checkNode)
        return true;

    if (checkNode->kind == AstNodeKind::Statement || checkNode->kind == AstNodeKind::StatementNoScope)
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
        return context->report({oneAttribute, Fmt(Err(Err0582), oneAttribute->typeInfo->getDisplayNameC(), TypeInfo::getArticleKindName(oneAttribute->typeInfo))});

    auto kind     = checkNode->kind;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage & AttributeUsage::All)
        return true;

    bool        isGlobalVar = kind == AstNodeKind::VarDecl && checkNode->ownerScope->isGlobalOrImpl();
    bool        isStructVar = kind == AstNodeKind::VarDecl && (checkNode->flags & AST_STRUCT_MEMBER);
    bool        isLocalVar  = kind == AstNodeKind::VarDecl && !isGlobalVar && !isStructVar;
    const char* specificMsg = nullptr;

    // Check specific hard coded attributes
    SWAG_ASSERT(oneAttribute->typeInfo->declNode);
    if (oneAttribute->typeInfo->declNode->sourceFile->isBootstrapFile)
    {
        if (oneAttribute->token.text == g_LangSpec->name_Complete)
        {
            if (kind == AstNodeKind::Switch)
                return true;
        }
        else if (oneAttribute->token.text == g_LangSpec->name_AttrUsage)
        {
            if (kind == AstNodeKind::AttrDecl)
                return true;
        }
        else if (oneAttribute->token.text == g_LangSpec->name_AttrMulti)
        {
            if (kind == AstNodeKind::AttrDecl)
                return true;
        }
        else if (oneAttribute->token.text == g_LangSpec->name_PrintBc)
        {
            if (kind == AstNodeKind::CompilerAst)
                return true;
        }
        else if (oneAttribute->token.text == g_LangSpec->name_Global)
        {
            if (isLocalVar)
                return true;
            specificMsg = "a local variable";
        }
        else if (oneAttribute->token.text == g_LangSpec->name_Align)
        {
            if (kind == AstNodeKind::VarDecl || kind == AstNodeKind::StructDecl)
                return true;
            specificMsg = "a variable or a struct";
        }
        else if (oneAttribute->token.text == g_LangSpec->name_Strict)
        {
            if (kind == AstNodeKind::Alias)
                return true;
            specificMsg = "a type alias";
        }
    }

    if (!specificMsg)
    {
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
    }

    switch (typeInfo->attributeUsage & AttributeUsage::All)
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
        Diagnostic diag{oneAttribute, Fmt(Err(Err0583), oneAttribute->token.ctext(), specificMsg)};
        Diagnostic note1{checkNode, Fmt(Nte(Nte0019), nakedName.c_str()), DiagnosticLevel::Note};
        Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, Fmt(Nte(Nte0025), oneAttribute->token.ctext()), DiagnosticLevel::Note};
        return context->report(diag, &note1, &note2);
    }
    else
    {
        auto nakedName = AstNode::getArticleKindName(checkNode);
        if (nakedName == "<node>")
        {
            Diagnostic diag{oneAttribute, Fmt(Err(Err0586), oneAttribute->token.ctext())};
            Diagnostic note1{oneAttribute->resolvedSymbolOverload->node, Fmt(Nte(Nte0025), oneAttribute->token.ctext()), DiagnosticLevel::Note};
            return context->report(diag, &note1);
        }
        else
        {
            auto       nakedName1 = AstNode::getKindName(checkNode);
            Diagnostic diag{oneAttribute, Fmt(Err(Err0588), oneAttribute->token.ctext(), nakedName.c_str())};
            Diagnostic note1{checkNode, Fmt(Nte(Nte0026), nakedName1.c_str()), DiagnosticLevel::Note};
            Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, Fmt(Nte(Nte0025), oneAttribute->token.ctext()), DiagnosticLevel::Note};
            return context->report(diag, &note1, &note2);
        }
    }
}

#define INHERIT(__c, __f)               \
    if (!(__c->attributeFlags & (__f))) \
        __c->attributeFlags |= attributeFlags & (__f);

void SemanticJob::inheritAttributesFromParent(AstNode* child)
{
    if (!child->parent)
        return;

    child->attributeFlags |= child->parent->attributeFlags & (ATTRIBUTE_SAFETY_MASK | ATTRIBUTE_SELECTIF_MASK);
}

void SemanticJob::inheritAttributesFromOwnerFunc(AstNode* child)
{
    SWAG_ASSERT(child->kind == AstNodeKind::FuncDecl);
    SWAG_ASSERT(child->ownerFct);

    auto attributeFlags = child->ownerFct->attributeFlags;

    child->attributeFlags |= attributeFlags & ATTRIBUTE_PRINT_BC;

    INHERIT(child, ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_CAST_ON | ATTRIBUTE_SAFETY_CAST_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_OVERFLOW_OFF);

    INHERIT(child, ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
    INHERIT(child, ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result)
{
    auto attrUse = forNode->extension ? forNode->extension->ownerAttrUse : nullptr;
    SWAG_CHECK(collectAttributes(context, forNode, result, attrUse));
    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result, AstAttrUse* attrUse)
{
    if (!attrUse)
        attrUse = forNode->sourceFile->astAttrUse;
    if (!attrUse)
        return true;

    // Already done
    if (result && !result->empty())
        return true;

    VectorNative<TypeInfoFuncAttr*> isHereGlobal;
    VectorNative<TypeInfoFuncAttr*> isHereTmp;
    auto&                           flags   = forNode->attributeFlags;
    auto                            curAttr = attrUse;
    while (curAttr)
    {
        isHereTmp.clear();

        auto attributeFlags = curAttr->attributeFlags;

        // Inherit all simple flags
        forNode->attributeFlags |= attributeFlags & ~(ATTRIBUTE_SAFETY_MASK | ATTRIBUTE_OPTIM_MASK | ATTRIBUTE_SELECTIF_MASK | ATTRIBUTE_EXPOSE_MASK);

        // Inherit with condition
        INHERIT(forNode, ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_CAST_ON | ATTRIBUTE_SAFETY_CAST_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_OVERFLOW_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_SWITCH_ON | ATTRIBUTE_SAFETY_SWITCH_OFF);

        INHERIT(forNode, ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
        INHERIT(forNode, ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);

        INHERIT(forNode, ATTRIBUTE_SELECTIF_MASK);

        if (!(forNode->flags & AST_PRIVATE))
            INHERIT(forNode, ATTRIBUTE_EXPOSE_MASK);

        for (auto child : curAttr->childs)
        {
            if (child == curAttr->content)
                continue;
            if (!child->typeInfo || child->typeInfo->kind != TypeInfoKind::FuncAttr)
                continue;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute(g_LangSpec->name_Swag_AttrMulti))
            {
                if (isHereTmp.contains(typeInfo))
                {
                    Diagnostic diag{forNode, Fmt(Err(Err0591), child->token.ctext(), child->token.ctext())};
                    Diagnostic note{child, Nte(Nte0032), DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }

                if (isHereGlobal.contains(typeInfo))
                    continue;
            }

            // Attribute on an attribute : usage
            if (forNode->kind == AstNodeKind::AttrDecl)
            {
                auto typeAttr = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                auto value    = curAttr->attributes.getValue(g_LangSpec->name_Swag_AttrUsage, g_LangSpec->name_usage);

                if (value)
                    typeAttr->attributeUsage = value->reg.u32;

                if (curAttr->attributes.hasAttribute(g_LangSpec->name_Swag_AttrMulti))
                    typeAttr->attributeUsage |= AttributeUsage::Multi;

                // Some checks
                if (typeAttr->attributeUsage & AttributeUsage::Gen)
                {
                    auto what = typeAttr->attributeUsage;
                    if (!(what & (AttributeUsage::Struct | AttributeUsage::Enum)))
                        return context->report({curAttr, Err(Err0852)});

                    what &= ~AttributeUsage::Struct;
                    what &= ~AttributeUsage::Enum;
                    what &= ~AttributeUsage::Gen;
                    if (typeAttr->attributeUsage & what)
                        return context->report({curAttr, Err(Err0852)});
                }
            }

            if (typeInfo->attributeUsage & AttributeUsage::Gen)
            {
                flags |= ATTRIBUTE_GEN;
            }

            // Predefined attributes will mark some flags (to speed up detection)
            auto it = g_LangSpec->attributesFlags.find(child->token.text);
            if (it)
            {
                flags |= *it;

                //////
                if (*it == ATTRIBUTE_FOREIGN)
                {
                    auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_module);
                    SWAG_VERIFY(!attrParam->value.text.empty(), context->report({child, attrParam->token, Err(Err0811)}));
                    SWAG_VERIFY(attrParam->value.text.find(".", 0) == -1, context->report({child, attrParam->token, Err(Err0813)}));
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Using)
            {
                auto id = CastAst<AstIdentifier>(child->childs.back(), AstNodeKind::Identifier);
                SWAG_VERIFY(id->callParameters && id->callParameters->childs.size() >= 1, context->report({id, Err(Err0601)}));
                for (auto c : id->callParameters->childs)
                {
                    SWAG_ASSERT(c->flags & AST_VALUE_IS_TYPEINFO);
                    SWAG_ASSERT(c->computedValue);
                    auto ptr       = (ConcreteTypeInfo*) c->computedValue->storageSegment->address(c->computedValue->storageOffset);
                    auto typeChild = context->sourceFile->module->typeTable.getRealType(c->computedValue->storageSegment, ptr);
                    SWAG_ASSERT(typeChild);
                    Scope* scope;
                    switch (typeChild->kind)
                    {
                    case TypeInfoKind::Namespace:
                    {
                        auto typeInfo1 = CastTypeInfo<TypeInfoNamespace>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    case TypeInfoKind::Enum:
                    {
                        auto typeInfo1 = CastTypeInfo<TypeInfoEnum>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    case TypeInfoKind::Struct:
                    {
                        auto typeInfo1 = CastTypeInfo<TypeInfoStruct>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    default:
                        return context->report({c, Fmt(Err(Err0695), typeChild->getDisplayNameC())});
                    }

                    forNode->addAlternativeScope(scope);
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_ExportType)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_ExportType, g_LangSpec->name_what);
                SWAG_ASSERT(attrParam);
                auto attrWhat = &attrParam->value;
                auto text     = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                for (auto& w : what)
                {
                    w.trim();
                    if (w == g_LangSpec->name_methods)
                        flags |= ATTRIBUTE_EXPORT_TYPE_METHODS;
                    else if (w == g_LangSpec->name_nozero)
                        flags |= ATTRIBUTE_EXPORT_TYPE_NOZERO;
                    else
                    {
                        Diagnostic note{Hlp(Hlp0013), DiagnosticLevel::Help};
                        return context->report({child, attrParam->token, Fmt(Err(Err0599), w.c_str())}, &note);
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Safety)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Safety, g_LangSpec->name_what);
                SWAG_ASSERT(attrParam);
                auto attrWhat = &attrParam->value;
                auto text     = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                auto attrValue = curAttr->attributes.getValue(g_LangSpec->name_Swag_Safety, g_LangSpec->name_value);
                SWAG_ASSERT(attrValue);

                if (text.empty())
                {
                    flags &= ~ATTRIBUTE_SAFETY_MASK;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_CAST_ON : ATTRIBUTE_SAFETY_CAST_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_SWITCH_ON : ATTRIBUTE_SAFETY_SWITCH_OFF;
                }

                for (auto& w : what)
                {
                    w.trim();
                    if (w == g_LangSpec->name_boundcheck)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    }
                    else if (w == g_LangSpec->name_overflow)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_OVERFLOW_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    }
                    else if (w == g_LangSpec->name_switch)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_SWITCH_ON | ATTRIBUTE_SAFETY_SWITCH_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_SWITCH_ON : ATTRIBUTE_SAFETY_SWITCH_OFF;
                    }
                    else if (w == g_LangSpec->name_math)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    }
                    else if (w == g_LangSpec->name_cast)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_CAST_ON | ATTRIBUTE_SAFETY_CAST_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_CAST_ON : ATTRIBUTE_SAFETY_CAST_OFF;
                    }
                    else
                    {
                        Diagnostic note{Hlp(Hlp0011), DiagnosticLevel::Help};
                        return context->report({child, attrParam->token, Fmt(Err(Err0593), w.c_str())}, &note);
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Optim)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Optim, g_LangSpec->name_what);
                SWAG_ASSERT(attrParam);
                auto attrWhat = &attrParam->value;
                auto text     = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                auto attrValue = curAttr->attributes.getValue(g_LangSpec->name_Swag_Optim, g_LangSpec->name_value);
                SWAG_ASSERT(attrValue);

                if (text.empty())
                {
                    flags &= ~(ATTRIBUTE_OPTIM_MASK);
                    flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF;
                }

                for (auto& w : what)
                {
                    w.trim();
                    if (w == g_LangSpec->name_bytecode)
                    {
                        flags &= ~(ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF;
                    }
                    else if (w == g_LangSpec->name_backend)
                    {
                        flags &= ~(ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF;
                    }
                    else
                    {
                        Diagnostic note{Hlp(Hlp0012), DiagnosticLevel::Help};
                        return context->report({child, attrParam->token, Fmt(Err(Err0594), w.c_str())}, &note);
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_SelectIf)
            {
                auto attrValue = curAttr->attributes.getValue(g_LangSpec->name_Swag_SelectIf, g_LangSpec->name_value);
                SWAG_ASSERT(attrValue);
                flags &= ~(ATTRIBUTE_SELECTIF_MASK);
                flags |= attrValue->reg.b ? ATTRIBUTE_SELECTIF_ON : ATTRIBUTE_SELECTIF_OFF;
            }

            //////
            else if (child->token.text == g_LangSpec->name_Pack)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Pack, g_LangSpec->name_value);
                SWAG_ASSERT(attrParam);
                auto attrValue = &attrParam->value;
                SWAG_VERIFY(!attrValue->reg.u8 || isPowerOfTwo(attrValue->reg.u8), context->report({child, attrParam->token, Fmt(Err(Err0595), attrValue->reg.u8)}));
            }

            //////
            else if (child->token.text == g_LangSpec->name_Align)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
                SWAG_ASSERT(attrParam);
                auto attrValue = &attrParam->value;
                SWAG_VERIFY(isPowerOfTwo(attrValue->reg.u8), context->report({child, attrParam->token, Fmt(Err(Err0596), attrValue->reg.u8)}));
            }

            // Remember attributes that's here
            isHereTmp.push_back_once(typeInfo);
            isHereGlobal.push_back_once(typeInfo);
        }

        // Merge the result
        if (result)
            result->add(curAttr->attributes);

        if (!(curAttr->specFlags & AST_SPEC_ATTRUSE_GLOBAL) && (!curAttr->extension || !curAttr->extension->ownerAttrUse))
            curAttr = forNode->sourceFile->astAttrUse;
        else
            curAttr = curAttr->extension ? curAttr->extension->ownerAttrUse : nullptr;
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
    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Attribute);
    SWAG_CHECK(node->resolvedSymbolOverload);

    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicAttribute(node);
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node = CastAst<AstAttrUse>(context->node->parent, AstNodeKind::AttrUse);
    SWAG_VERIFY(node->content || (node->specFlags & AST_SPEC_ATTRUSE_GLOBAL), context->report({node, Err(Err0597)}));

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
            Diagnostic diag{identifier, Fmt(Err(Err0598), resolvedName->name.c_str())};
            Diagnostic note{resolved->node, Fmt(Nte(Nte0029), resolvedName->name.c_str()), DiagnosticLevel::Note};
            context->report(diag, &note);
            return false;
        }

        // Check that global attribute is authorized
        if (node->specFlags & AST_SPEC_ATTRUSE_GLOBAL)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeInfo->attributeUsage & AttributeUsage::File))
            {
                Diagnostic diag{identifier, Fmt(Err(Err0600), resolvedName->name.c_str())};
                Diagnostic note{resolved->node, Fmt(Nte(Nte0029), resolvedName->name.c_str()), DiagnosticLevel::Note};
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
                SWAG_CHECK(checkIsConstExpr(context, param->flags & AST_VALUE_COMPUTED, param, Err(Err0602)));

                AttributeParameter attrParam;
                attrParam.token      = one->token;
                attrParam.token.text = param->resolvedParameter->namedParam;
                attrParam.typeInfo   = param->resolvedParameter->typeInfo;
                attrParam.value      = *param->computedValue;
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
            attrParam.token    = param->token;
            attrParam.typeInfo = param->typeInfo;
            attrParam.value    = *param->assignment->computedValue;
            oneAttribute.parameters.emplace_back(move(attrParam));
        }

        oneAttribute.typeFunc = typeFunc;
        node->attributes.emplace(oneAttribute);
    }

    SWAG_CHECK(collectAttributes(context, node, nullptr, node));
    return true;
}
