#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Math.h"
#include "LanguageSpec.h"
#include "Naming.h"

#define INHERIT_ATTR(__c, __f)          \
    if (!(__c->attributeFlags & (__f))) \
        __c->attributeFlags |= attributeFlags & (__f);

#define INHERIT_SAFETY(__c, __f)                               \
    if (!(__c->safetyOn & (__f)) && !(__c->safetyOff & (__f))) \
    {                                                          \
        __c->safetyOn |= safetyOn & (__f);                     \
        __c->safetyOff |= safetyOff & (__f);                   \
    }

#define CHECK_SAFETY_NAME(__name, __flag) \
    if (w == g_LangSpec->__name)          \
    {                                     \
        done = true;                      \
        forNode->safetyOn &= ~__flag;     \
        forNode->safetyOff &= ~__flag;    \
        if (attrValue->reg.b)             \
            forNode->safetyOn |= __flag;  \
        else                              \
            forNode->safetyOff |= __flag; \
    }

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
    if (!oneAttribute->typeInfo->isFuncAttr())
        return context->report({oneAttribute, Fmt(Err(Err0582), oneAttribute->typeInfo->getDisplayNameC(), Naming::aKindName(oneAttribute->typeInfo).c_str())});

    auto kind     = checkNode->kind;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage & AttributeUsage::All)
        return true;

    bool        isGlobalVar = kind == AstNodeKind::VarDecl && checkNode->ownerScope->isGlobalOrImpl();
    bool        isStructVar = kind == AstNodeKind::VarDecl && (checkNode->flags & AST_STRUCT_MEMBER);
    bool        isLocalVar  = kind == AstNodeKind::VarDecl && !isGlobalVar && !isStructVar;
    bool        isFuncParam = kind == AstNodeKind::FuncDeclParam;
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
        if ((typeInfo->attributeUsage & AttributeUsage::FunctionParameter) && isFuncParam)
            return true;
    }

    switch (typeInfo->attributeUsage & AttributeUsage::MaskType)
    {
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
    case AttributeUsage::Variable:
        specificMsg = "a variable";
        break;
    case AttributeUsage::Struct:
        specificMsg = "a struct";
        break;
    case AttributeUsage::Function:
        specificMsg = "a function";
        break;
    case AttributeUsage::FunctionParameter:
        specificMsg = "a function parameter";
        break;
    case AttributeUsage::File:
        specificMsg = "a file";
        break;
    case AttributeUsage::Constant:
        specificMsg = "a constant";
        break;
    }

    if (specificMsg)
    {
        auto       nakedName = Naming::kindName(checkNode);
        Diagnostic diag{oneAttribute, Fmt(Err(Err0583), oneAttribute->token.ctext(), specificMsg)};
        diag.hint  = Hnt(Hnt0096);
        auto note1 = Diagnostic::note(checkNode, Fmt(Nte(Nte0019), nakedName.c_str()));
        return context->report(diag, note1, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
    }
    else
    {
        auto nakedName = Naming::aKindName(checkNode);
        if (nakedName == "<node>")
        {
            Diagnostic diag{oneAttribute, Fmt(Err(Err0586), oneAttribute->token.ctext())};
            diag.hint = Hnt(Hnt0096);
            return context->report(diag, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
        }
        else
        {
            auto       nakedName1 = Naming::kindName(checkNode);
            Diagnostic diag{oneAttribute, Fmt(Err(Err0588), oneAttribute->token.ctext(), nakedName.c_str())};
            diag.hint  = Hnt(Hnt0096);
            auto note1 = Diagnostic::note(checkNode, checkNode->token, Fmt(Nte(Nte0026), nakedName1.c_str()));
            return context->report(diag, note1, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
        }
    }
}

void SemanticJob::inheritAttributesFromParent(AstNode* child)
{
    if (!child->parent)
        return;

    child->attributeFlags |= child->parent->attributeFlags & ATTRIBUTE_MATCH_MASK;
    child->attributeFlags |= child->parent->attributeFlags & ATTRIBUTE_OVERFLOW_MASK;
    child->safetyOn |= child->parent->safetyOn;
    child->safetyOff |= child->parent->safetyOff;
}

void SemanticJob::inheritAttributesFrom(AstNode* child, uint64_t attributeFlags, uint16_t safetyOn, uint16_t safetyOff)
{
    INHERIT_SAFETY(child, SAFETY_BOUNDCHECK);
    INHERIT_SAFETY(child, SAFETY_OVERFLOW);
    INHERIT_SAFETY(child, SAFETY_MATH);
    INHERIT_SAFETY(child, SAFETY_ANY);
    INHERIT_SAFETY(child, SAFETY_SWITCH);
    INHERIT_SAFETY(child, SAFETY_UNREACHABLE);
    INHERIT_SAFETY(child, SAFETY_BOOL);
    INHERIT_SAFETY(child, SAFETY_NAN);
    INHERIT_SAFETY(child, SAFETY_SANITY);

    INHERIT_ATTR(child, ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
    INHERIT_ATTR(child, ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
    INHERIT_ATTR(child, ATTRIBUTE_CAN_OVERFLOW_ON | ATTRIBUTE_CAN_OVERFLOW_OFF);
    INHERIT_ATTR(child, ATTRIBUTE_MATCH_VALIDIF_OFF | ATTRIBUTE_MATCH_SELF_OFF);
    if (!(child->flags & AST_PRIVATE))
        INHERIT_ATTR(child, ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE);
}

void SemanticJob::inheritAttributesFromOwnerFunc(AstNode* child)
{
    SWAG_ASSERT(child->kind == AstNodeKind::FuncDecl);
    SWAG_ASSERT(child->ownerFct);

    auto attributeFlags = child->ownerFct->attributeFlags;
    auto safetyOn       = child->ownerFct->safetyOn;
    auto safetyOff      = child->ownerFct->safetyOff;

    child->attributeFlags |= attributeFlags & ATTRIBUTE_PRINT_BC;
    inheritAttributesFrom(child, attributeFlags, safetyOn, safetyOff);
}

bool SemanticJob::collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result)
{
    auto attrUse = forNode->hasExtOwner() ? forNode->extOwner()->ownerAttrUse : nullptr;
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
        auto safetyOn       = curAttr->safetyOn;
        auto safetyOff      = curAttr->safetyOff;

        // Inherit all simple flags
        forNode->attributeFlags |= attributeFlags & ~(ATTRIBUTE_OPTIM_MASK | ATTRIBUTE_MATCH_MASK | ATTRIBUTE_OVERFLOW_MASK | ATTRIBUTE_EXPOSE_MASK);

        // Inherit some attributes and safety
        inheritAttributesFrom(forNode, attributeFlags, safetyOn, safetyOff);

        for (auto child : curAttr->childs)
        {
            if (child == curAttr->content)
                continue;
            if (!child->typeInfo || !child->typeInfo->isFuncAttr())
                continue;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute(g_LangSpec->name_Swag_AttrMulti))
            {
                if (isHereTmp.contains(typeInfo))
                {
                    Diagnostic diag{forNode, Fmt(Err(Err0591), child->token.ctext())};
                    auto       note = Diagnostic::note(child, Nte(Nte0032));
                    return context->report(diag, note);
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

                if (*it == ATTRIBUTE_INLINE && (flags & ATTRIBUTE_NO_INLINE))
                    return context->report({child, Err(Err0083)});
                if (*it == ATTRIBUTE_NO_INLINE && (flags & ATTRIBUTE_INLINE))
                    return context->report({child, Err(Err0691)});
                if (*it == ATTRIBUTE_TLS && (flags & ATTRIBUTE_COMPILER))
                    return context->report({child, Err(Err0159)});
                if (*it == ATTRIBUTE_COMPILER && (flags & ATTRIBUTE_TLS))
                    return context->report({child, Err(Err0147)});
            }

            //////
            else if (child->token.text == g_LangSpec->name_Using)
            {
                auto id = CastAst<AstIdentifier>(child->childs.back(), AstNodeKind::Identifier);
                id->flags |= AST_NO_SEMANTIC;

                SWAG_VERIFY(id->callParameters && id->callParameters->childs.size() >= 1, context->report({id, Err(Err0601)}));
                for (auto c : id->callParameters->childs)
                {
                    SWAG_ASSERT(c->flags & AST_VALUE_IS_TYPEINFO);
                    SWAG_ASSERT(c->computedValue);
                    auto ptr       = (ExportedTypeInfo*) c->computedValue->storageSegment->address(c->computedValue->storageOffset);
                    auto typeChild = context->sourceFile->module->typeGen.getRealType(c->computedValue->storageSegment, ptr);
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
                Vector<Utf8> what;
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
                        auto note = Diagnostic::help(Hlp(Hlp0013));
                        return context->report({child, attrParam->token, Fmt(Err(Err0599), w.c_str())}, note);
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Safety)
            {
                VectorNative<const OneAttribute*> allAttrs;
                Vector<Utf8>                      what;

                curAttr->attributes.getAttributes(allAttrs, g_LangSpec->name_Swag_Safety);
                for (auto attr : allAttrs)
                {
                    auto attrParam = attr->getParam(g_LangSpec->name_what);
                    SWAG_ASSERT(attrParam);
                    auto attrValue = attr->getValue(g_LangSpec->name_value);
                    SWAG_ASSERT(attrValue);

                    auto attrWhat = &attrParam->value;
                    auto text     = attrWhat->text;
                    text.trim();
                    Utf8::tokenize(text, '|', what);

                    if (text.empty())
                    {
                        if (attrValue->reg.b)
                            forNode->safetyOn = SAFETY_ALL;
                        else
                            forNode->safetyOff = SAFETY_ALL;
                    }

                    for (auto& w : what)
                    {
                        w.trim();
                        bool done = false;

                        CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUNDCHECK);
                        CHECK_SAFETY_NAME(name_overflow, SAFETY_OVERFLOW);
                        CHECK_SAFETY_NAME(name_math, SAFETY_MATH);
                        CHECK_SAFETY_NAME(name_any, SAFETY_ANY);
                        CHECK_SAFETY_NAME(name_switch, SAFETY_SWITCH);
                        CHECK_SAFETY_NAME(name_unreachable, SAFETY_UNREACHABLE);
                        CHECK_SAFETY_NAME(name_bool, SAFETY_BOOL);
                        CHECK_SAFETY_NAME(name_nan, SAFETY_NAN);
                        CHECK_SAFETY_NAME(name_sanity, SAFETY_SANITY);

                        if (!done)
                        {
                            auto note = Diagnostic::help(Hlp(Hlp0011));
                            return context->report({child, attrParam->token, Fmt(Err(Err0593), w.c_str())}, note);
                        }
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Optim)
            {
                VectorNative<const OneAttribute*> allAttrs;
                Vector<Utf8>                      what;

                curAttr->attributes.getAttributes(allAttrs, g_LangSpec->name_Swag_Optim);
                for (auto attr : allAttrs)
                {
                    auto attrParam = attr->getParam(g_LangSpec->name_what);
                    SWAG_ASSERT(attrParam);
                    auto attrValue = attr->getValue(g_LangSpec->name_value);
                    SWAG_ASSERT(attrValue);

                    auto attrWhat = &attrParam->value;
                    auto text     = attrWhat->text;
                    text.trim();
                    Utf8::tokenize(text, '|', what);

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
                            auto note = Diagnostic::help(Hlp(Hlp0012));
                            return context->report({child, attrParam->token, Fmt(Err(Err0594), w.c_str())}, note);
                        }
                    }
                }
            }

            //////
            else if (child->token.text == g_LangSpec->name_Match)
            {
                VectorNative<const OneAttribute*> allAttrs;
                Vector<Utf8>                      what;

                curAttr->attributes.getAttributes(allAttrs, g_LangSpec->name_Swag_Match);
                for (auto attr : allAttrs)
                {
                    auto attrParam = attr->getParam(g_LangSpec->name_what);
                    SWAG_ASSERT(attrParam);
                    auto attrValue = attr->getValue(g_LangSpec->name_value);
                    SWAG_ASSERT(attrValue);

                    auto attrWhat = &attrParam->value;
                    auto text     = attrWhat->text;
                    text.trim();
                    Utf8::tokenize(text, '|', what);

                    if (text.empty())
                    {
                        flags &= ~ATTRIBUTE_MATCH_MASK;
                        if (!attrValue->reg.b)
                            flags |= ATTRIBUTE_MATCH_VALIDIF_OFF | ATTRIBUTE_MATCH_SELF_OFF;
                    }

                    for (auto& w : what)
                    {
                        w.trim();
                        if (w == g_LangSpec->name_validif)
                        {
                            if (!attrValue->reg.b)
                                flags |= ATTRIBUTE_MATCH_VALIDIF_OFF;
                            else
                                flags &= ~ATTRIBUTE_MATCH_VALIDIF_OFF;
                        }
                        else if (w == g_LangSpec->name_self)
                        {
                            if (!attrValue->reg.b)
                                flags |= ATTRIBUTE_MATCH_SELF_OFF;
                            else
                                flags &= ~ATTRIBUTE_MATCH_SELF_OFF;
                        }
                        else
                        {
                            auto note = Diagnostic::help(Hlp(Hlp0048));
                            return context->report({child, attrParam->token, Fmt(Err(Err0693), w.c_str())}, note);
                        }
                    }
                }
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

            //////
            else if (child->token.text == g_LangSpec->name_Overflow)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Overflow, g_LangSpec->name_value);
                SWAG_ASSERT(attrParam);
                flags &= ~(ATTRIBUTE_CAN_OVERFLOW_ON | ATTRIBUTE_CAN_OVERFLOW_OFF);
                if (attrParam->value.reg.b)
                    flags |= ATTRIBUTE_CAN_OVERFLOW_ON;
                else
                    flags |= ATTRIBUTE_CAN_OVERFLOW_OFF;
            }

            // Remember attributes that's here
            isHereTmp.push_back_once(typeInfo);
            isHereGlobal.push_back_once(typeInfo);
        }

        // Merge the result
        if (result)
            result->add(curAttr->attributes);

        if (!(curAttr->specFlags & AstAttrUse::SPECFLAG_GLOBAL) && (!curAttr->hasExtOwner() || !curAttr->extOwner()->ownerAttrUse))
            curAttr = forNode->sourceFile->astAttrUse;
        else
            curAttr = curAttr->hasExtOwner() ? curAttr->extOwner()->ownerAttrUse : nullptr;
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

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Attribute;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);

    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicAttribute(node);
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node = CastAst<AstAttrUse>(context->node->parent, AstNodeKind::AttrUse);
    SWAG_VERIFY(node->content || (node->specFlags & AstAttrUse::SPECFLAG_GLOBAL), context->report({node, Err(Err0597)}));
    SWAG_CHECK(resolveAttrUse(context, node));
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context, AstAttrUse* node)
{
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
            Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0598), resolvedName->name.c_str())};
            context->report(diag, Diagnostic::hereIs(resolved));
            return false;
        }

        // Check that global attribute is authorized
        if (node->specFlags & AstAttrUse::SPECFLAG_GLOBAL)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeInfo->attributeUsage & AttributeUsage::File))
            {
                Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0600), resolvedName->name.c_str())};
                context->report(diag, Diagnostic::hereIs(resolved));
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
                attrParam.token.text = param->resolvedParameter->name;
                attrParam.typeInfo   = param->resolvedParameter->typeInfo;
                attrParam.value      = *param->computedValue;
                oneAttribute.parameters.emplace_back(std::move(attrParam));
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
            oneAttribute.parameters.emplace_back(std::move(attrParam));
        }

        oneAttribute.typeFunc = typeFunc;
        node->attributes.emplace(oneAttribute);
    }

    AstNode*       forNode = node;
    AttributeList* list    = nullptr;
    if (node->content && node->content->kind == AstNodeKind::FuncDeclParam)
    {
        auto funcDeclParam = CastAst<AstVarDecl>(node->content, AstNodeKind::FuncDeclParam);
        list               = &funcDeclParam->attributes;
        forNode            = node->content;
    }

    SWAG_CHECK(collectAttributes(context, forNode, list, node));
    return true;
}
