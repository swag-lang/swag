#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Math.h"
#include "Module.h"
#include "Naming.h"
#include "Semantic.h"
#include "SourceFile.h"
#include "TypeManager.h"

#define INHERIT_ATTR(__c, __f)                               \
    do                                                       \
    {                                                        \
        if (!(__c)->attributeFlags.has(__f))                 \
			(__c)->attributeFlags.add(attributeFlags.mask(__f)); \
    } while(0)

#define INHERIT_SAFETY(__c, __f)                                       \
    do                                                                 \
    {                                                                  \
        if (!(__c)->safetyOn.has(__f) && !(__c)->safetyOff.has(__f))   \
        {                                                              \
            (__c)->safetyOn.add(safetyOn.mask(__f));                   \
            (__c)->safetyOff.add(safetyOff.mask(__f));                 \
        }                                                              \
    } while(0)

#define CHECK_SAFETY_NAME(__name, __flag)       \
    do                                          \
    {                                           \
        if (w == g_LangSpec->__name)            \
        {                                       \
            done = true;                        \
            forNode->safetyOn.remove(__flag);   \
            forNode->safetyOff.remove(__flag);  \
            if (attrValue->reg.b)               \
                forNode->safetyOn.add(__flag);  \
            else                                \
                forNode->safetyOff.add(__flag); \
        }                                       \
    } while(0)

bool Semantic::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode)
{
    if (!checkNode || checkNode->kind == AstNodeKind::RefSubDecl)
        return true;

    if (checkNode->kind == AstNodeKind::Statement || checkNode->kind == AstNodeKind::StatementNoScope)
    {
        for (const auto s : checkNode->children)
            SWAG_CHECK(checkAttribute(context, oneAttribute, s));
        return true;
    }

    if (checkNode->kind == AstNodeKind::AttrUse)
    {
        const auto attrUse = castAst<AstAttrUse>(checkNode, AstNodeKind::AttrUse);
        if (checkNode->hasAstFlag(AST_GENERATED) && attrUse->content->kind == AstNodeKind::Namespace && attrUse->content->hasAstFlag(AST_GENERATED))
            SWAG_CHECK(checkAttribute(context, oneAttribute, attrUse->content->children.front()));
        else
            SWAG_CHECK(checkAttribute(context, oneAttribute, attrUse->content));
        return true;
    }

    SWAG_ASSERT(oneAttribute->typeInfo);
    if (!oneAttribute->typeInfo->isFuncAttr())
        return context->report({oneAttribute, FMT(Err(Err0218), oneAttribute->typeInfo->getDisplayNameC(), Naming::aKindName(oneAttribute->typeInfo).c_str())});

    const auto kind     = checkNode->kind;
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage & All)
        return true;

    const bool  isGlobalVar = kind == AstNodeKind::VarDecl && checkNode->ownerScope->isGlobalOrImpl();
    const bool  isStructVar = kind == AstNodeKind::VarDecl && checkNode->hasAstFlag(AST_STRUCT_MEMBER);
    const bool  isLocalVar  = kind == AstNodeKind::VarDecl && !isGlobalVar && !isStructVar;
    const bool  isFuncParam = kind == AstNodeKind::FuncDeclParam;
    const char* specificMsg = nullptr;

    // Check specific hard coded attributes
    SWAG_ASSERT(oneAttribute->typeInfo->declNode);
    if (oneAttribute->typeInfo->declNode->sourceFile->hasFlag(FILE_IS_BOOTSTRAP_FILE))
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
            if (kind == AstNodeKind::TypeAlias)
                return true;
            specificMsg = "a type alias";
        }
    }

    if (!specificMsg)
    {
        if (typeInfo->attributeUsage & Function && kind == AstNodeKind::FuncDecl)
            return true;
        if (typeInfo->attributeUsage & Struct && kind == AstNodeKind::StructDecl)
            return true;
        if (typeInfo->attributeUsage & Enum && kind == AstNodeKind::EnumDecl)
            return true;
        if (typeInfo->attributeUsage & EnumValue && kind == AstNodeKind::EnumValue)
            return true;
        if (typeInfo->attributeUsage & Variable && kind == AstNodeKind::VarDecl)
            return true;
        if (typeInfo->attributeUsage & Constant && kind == AstNodeKind::ConstDecl)
            return true;
        if (typeInfo->attributeUsage & StructVariable && isStructVar)
            return true;
        if (typeInfo->attributeUsage & GlobalVariable && isGlobalVar)
            return true;
        if (typeInfo->attributeUsage & FunctionParameter && isFuncParam)
            return true;
    }

    switch (typeInfo->attributeUsage & MaskType)
    {
    case Enum:
        specificMsg = "an enum";
        break;
    case EnumValue:
        specificMsg = "an enum value";
        break;
    case StructVariable:
        specificMsg = "a struct variable";
        break;
    case GlobalVariable:
        specificMsg = "a global variable";
        break;
    case Variable:
        specificMsg = "a variable";
        break;
    case Struct:
        specificMsg = "a struct";
        break;
    case Function:
        specificMsg = "a function";
        break;
    case FunctionParameter:
        specificMsg = "a function parameter";
        break;
    case File:
        specificMsg = "a file";
        break;
    case Constant:
        specificMsg = "a constant";
        break;
    default:
        break;
    }

    if (specificMsg)
    {
        const auto       nakedName = Naming::kindName(checkNode);
        const Diagnostic err{oneAttribute, FMT(Err(Err0491), oneAttribute->token.c_str(), specificMsg)};
        const auto       note1 = Diagnostic::note(checkNode, checkNode->token, FMT(Nte(Nte0024), nakedName.c_str()));
        return context->report(err, note1, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
    }

    const auto nakedName = Naming::aKindName(checkNode);
    if (nakedName == "<node>")
    {
        const Diagnostic err{oneAttribute, FMT(Err(Err0495), oneAttribute->token.c_str())};
        return context->report(err, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
    }

    const auto       nakedName1 = Naming::kindName(checkNode);
    const Diagnostic err{oneAttribute, FMT(Err(Err0492), oneAttribute->token.c_str(), nakedName.c_str())};
    const auto       note1 = Diagnostic::note(checkNode, checkNode->token, FMT(Nte(Nte0063), nakedName1.c_str()));
    return context->report(err, note1, Diagnostic::hereIs(oneAttribute->resolvedSymbolOverload));
}

void Semantic::inheritAttributesFromParent(AstNode* child)
{
    if (!child->parent)
        return;

    child->inheritAttribute(child->parent, ATTRIBUTE_MATCH_MASK);
    child->inheritAttribute(child->parent, ATTRIBUTE_OVERFLOW_MASK);
    child->safetyOn.add(child->parent->safetyOn);
    child->safetyOff.add(child->parent->safetyOff);
}

void Semantic::inheritAttributesFrom(AstNode* child, AttributeFlags attributeFlags, SafetyFlags safetyOn, SafetyFlags safetyOff)
{
    INHERIT_SAFETY(child, SAFETY_BOUND_CHECK);
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

    if (!child->hasAstFlag(AST_INTERNAL))
        INHERIT_ATTR(child, ATTRIBUTE_PUBLIC | ATTRIBUTE_INTERNAL | ATTRIBUTE_PRIVATE);
}

void Semantic::inheritAttributesFromOwnerFunc(AstNode* child)
{
    SWAG_ASSERT(child->kind == AstNodeKind::FuncDecl);
    SWAG_ASSERT(child->ownerFct);

    const auto attributeFlags = child->ownerFct->attributeFlags;
    const auto safetyOn       = child->ownerFct->safetyOn;
    const auto safetyOff      = child->ownerFct->safetyOff;

    child->addAttribute(attributeFlags.mask(ATTRIBUTE_PRINT_BC));
    child->addAttribute(attributeFlags.mask(ATTRIBUTE_PRINT_GEN_BC));
    inheritAttributesFrom(child, attributeFlags, safetyOn, safetyOff);
}

bool Semantic::collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result)
{
    const auto attrUse = forNode->hasExtOwner() ? forNode->extOwner()->ownerAttrUse : nullptr;
    SWAG_CHECK(collectAttributes(context, forNode, result, attrUse));
    setDefaultAccess(forNode);
    return true;
}

bool Semantic::collectAttributes(SemanticContext* context, AstNode* forNode, AttributeList* result, AstAttrUse* attrUse)
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
        forNode->addAttribute(attributeFlags.maskInvert(ATTRIBUTE_OPTIM_MASK | ATTRIBUTE_MATCH_MASK | ATTRIBUTE_OVERFLOW_MASK | ATTRIBUTE_ACCESS_MASK));

        // Inherit some attributes and safety
        inheritAttributesFrom(forNode, attributeFlags, safetyOn, safetyOff);

        for (auto child : curAttr->children)
        {
            if (child == curAttr->content)
                continue;
            if (!child->typeInfo || !child->typeInfo->isFuncAttr())
                continue;

            auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute(g_LangSpec->name_Swag_AttrMulti))
            {
                if (isHereTmp.contains(typeInfo))
                {
                    Diagnostic err{child, FMT(Err(Err0068), child->token.c_str())};
                    return context->report(err);
                }

                if (isHereGlobal.contains(typeInfo))
                    continue;
            }

            // Attribute on an attribute : usage
            if (forNode->kind == AstNodeKind::AttrDecl)
            {
                auto typeAttr = castTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                auto value    = curAttr->attributes.getValue(g_LangSpec->name_Swag_AttrUsage, g_LangSpec->name_usage);

                if (value)
                    typeAttr->attributeUsage = value->reg.u32;

                if (curAttr->attributes.hasAttribute(g_LangSpec->name_Swag_AttrMulti))
                    typeAttr->attributeUsage |= Multi;

                // Some checks
                if (typeAttr->attributeUsage & Gen)
                {
                    auto what = typeAttr->attributeUsage;
                    if (!(what & (Struct | Enum)))
                        return context->report({child, Err(Err0221)});

                    what &= ~Struct;
                    what &= ~Enum;
                    what &= ~Gen;
                    if (typeAttr->attributeUsage & what)
                        return context->report({child, Err(Err0221)});
                }
            }

            if (typeInfo->attributeUsage & Gen)
            {
                flags.add(ATTRIBUTE_GEN);
            }

            // Predefined attributes will mark some flags (to speed up detection)
            auto it = g_LangSpec->attributesFlags.find(child->token.text);
            if (it)
            {
                flags.add(*it);

                //////
                if (*it == ATTRIBUTE_FOREIGN)
                {
                    auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_module);
                    SWAG_VERIFY(!attrParam->value.text.empty(), context->report({child, attrParam->token, Err(Err0325)}));
                    SWAG_VERIFY(attrParam->value.text.find(".", 0) == -1, context->report({child, attrParam->token, Err(Err0326)}));
                }

#define EXCLUSIVE(__a, __b) ((*it == (__a) && (flags.has(__b))) || (*it == (__b) && (flags.has(__a))))

                if (EXCLUSIVE(ATTRIBUTE_TLS, ATTRIBUTE_COMPILER))
                    return context->report({child, Err(Err0048)});
                if (EXCLUSIVE(ATTRIBUTE_INLINE, ATTRIBUTE_NO_INLINE))
                    return context->report({child, Err(Err0049)});
                if (EXCLUSIVE(ATTRIBUTE_MACRO, ATTRIBUTE_INLINE))
                    return context->report({child, Err(Err0050)});
                if (EXCLUSIVE(ATTRIBUTE_MACRO, ATTRIBUTE_MIXIN))
                    return context->report({child, Err(Err0051)});
                if (EXCLUSIVE(ATTRIBUTE_MIXIN, ATTRIBUTE_INLINE))
                    return context->report({child, Err(Err0052)});
            }

            //////
            else if (child->token.text == g_LangSpec->name_Using)
            {
                auto id = castAst<AstIdentifier>(child->children.back(), AstNodeKind::Identifier);
                id->addAstFlag(AST_NO_SEMANTIC);

                SWAG_VERIFY(id->callParameters && !id->callParameters->children.empty(), context->report({id, Err(Err0541)}));
                for (auto c : id->callParameters->children)
                {
                    auto ptr       = c->getConstantGenTypeInfo();
                    auto typeChild = context->sourceFile->module->typeGen.getRealType(c->computedValue->storageSegment, ptr);
                    SWAG_ASSERT(typeChild);
                    Scope* scope;
                    switch (typeChild->kind)
                    {
                    case TypeInfoKind::Namespace:
                    {
                        auto typeInfo1 = castTypeInfo<TypeInfoNamespace>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    case TypeInfoKind::Enum:
                    {
                        auto typeInfo1 = castTypeInfo<TypeInfoEnum>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    case TypeInfoKind::Struct:
                    {
                        auto typeInfo1 = castTypeInfo<TypeInfoStruct>(typeChild, typeChild->kind);
                        scope          = typeInfo1->scope;
                        break;
                    }
                    default:
                        return context->report({c, FMT(Err(Err0472), typeChild->getDisplayNameC())});
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
                        flags.add(ATTRIBUTE_EXPORT_TYPE_METHODS);
                    else if (w == g_LangSpec->name_nozero)
                        flags.add(ATTRIBUTE_EXPORT_TYPE_NO_ZERO);
                    else
                    {
                        return context->report({child, attrParam->token, FMT(Err(Err0280), w.c_str())});
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

                        CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUND_CHECK);
                        CHECK_SAFETY_NAME(name_overflow, SAFETY_OVERFLOW);
                        CHECK_SAFETY_NAME(name_math, SAFETY_MATH);
                        CHECK_SAFETY_NAME(name_any, SAFETY_ANY);
                        CHECK_SAFETY_NAME(name_switch, SAFETY_SWITCH);
                        CHECK_SAFETY_NAME(name_unreachable, SAFETY_UNREACHABLE);
                        CHECK_SAFETY_NAME(name_bool, SAFETY_BOOL);
                        CHECK_SAFETY_NAME(name_nan, SAFETY_NAN);
                        CHECK_SAFETY_NAME(name_sanity, SAFETY_SANITY);
                        CHECK_SAFETY_NAME(name_null, SAFETY_NULL);

                        if (!done)
                        {
                            return context->report({child, attrParam->token, FMT(Err(Err0373), w.c_str())});
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
                        flags.remove(ATTRIBUTE_OPTIM_MASK);
                        flags.add(attrValue->reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF);
                        flags.add(attrValue->reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF);
                    }

                    for (auto& w : what)
                    {
                        w.trim();
                        if (w == g_LangSpec->name_bytecode)
                        {
                            flags.remove(ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
                            flags.add(attrValue->reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF);
                        }
                        else if (w == g_LangSpec->name_backend)
                        {
                            flags.remove(ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
                            flags.add(attrValue->reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF);
                        }
                        else
                        {
                            return context->report({child, attrParam->token, FMT(Err(Err0353), w.c_str())});
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
                        flags.remove(ATTRIBUTE_MATCH_MASK);
                        if (!attrValue->reg.b)
                            flags.add(ATTRIBUTE_MATCH_VALIDIF_OFF | ATTRIBUTE_MATCH_SELF_OFF);
                    }

                    for (auto& w : what)
                    {
                        w.trim();
                        if (w == g_LangSpec->name_validif)
                        {
                            if (!attrValue->reg.b)
                                flags.add(ATTRIBUTE_MATCH_VALIDIF_OFF);
                            else
                                flags.remove(ATTRIBUTE_MATCH_VALIDIF_OFF);
                        }
                        else if (w == g_LangSpec->name_self)
                        {
                            if (!attrValue->reg.b)
                                flags.add(ATTRIBUTE_MATCH_SELF_OFF);
                            else
                                flags.remove(ATTRIBUTE_MATCH_SELF_OFF);
                        }
                        else
                        {
                            return context->report({child, attrParam->token, FMT(Err(Err0324), w.c_str())});
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
                SWAG_VERIFY(!attrValue->reg.u8 || Math::isPowerOfTwo(attrValue->reg.u8), context->report({child, attrParam->token, FMT(Err(Err0354), attrValue->reg.u8)}));
            }

            //////
            else if (child->token.text == g_LangSpec->name_Align)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
                SWAG_ASSERT(attrParam);
                auto attrValue = &attrParam->value;
                SWAG_VERIFY(Math::isPowerOfTwo(attrValue->reg.u8), context->report({child, attrParam->token, FMT(Err(Err0190), attrValue->reg.u8)}));
            }

            //////
            else if (child->token.text == g_LangSpec->name_Overflow)
            {
                auto attrParam = curAttr->attributes.getParam(g_LangSpec->name_Swag_Overflow, g_LangSpec->name_value);
                SWAG_ASSERT(attrParam);
                flags.remove(ATTRIBUTE_CAN_OVERFLOW_ON | ATTRIBUTE_CAN_OVERFLOW_OFF);
                if (attrParam->value.reg.b)
                    flags.add(ATTRIBUTE_CAN_OVERFLOW_ON);
                else
                    flags.add(ATTRIBUTE_CAN_OVERFLOW_OFF);
            }

            // Remember attributes that's here
            isHereTmp.push_back_once(typeInfo);
            isHereGlobal.push_back_once(typeInfo);
        }

        // Merge the result
        if (result)
            result->add(curAttr->attributes);

        if (!curAttr->hasSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL) && (!curAttr->hasExtOwner() || !curAttr->extOwner()->ownerAttrUse))
            curAttr = forNode->sourceFile->astAttrUse;
        else
            curAttr = curAttr->hasExtOwner() ? curAttr->extOwner()->ownerAttrUse : nullptr;
    }

    return true;
}

bool Semantic::preResolveAttrDecl(SemanticContext* context)
{
    const auto node     = castAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));
    return true;
}

bool Semantic::resolveAttrDecl(SemanticContext* context)
{
    const auto node     = castAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Attribute;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(node->resolvedSymbolOverload);

    if (node->hasAttribute(ATTRIBUTE_PUBLIC))
        node->ownerScope->addPublicAttribute(node);
    return true;
}

bool Semantic::resolveAttrUse(SemanticContext* context)
{
    auto node = castAst<AstAttrUse>(context->node->parent, AstNodeKind::AttrUse);
    SWAG_VERIFY(node->content || node->hasSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL), context->report({node, Err(Err0485)}));
    SWAG_CHECK(resolveAttrUse(context, node));
    return true;
}

bool Semantic::resolveAttrUse(SemanticContext* context, AstAttrUse* node)
{
    for (auto child : node->children)
    {
        if (child == node->content)
            continue;
        if (child->kind != AstNodeKind::IdentifierRef)
            continue;

        if (node->content)
            SWAG_CHECK(checkAttribute(context, child, node->content));

        // Collect parameters
        auto identifierRef = castAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        auto identifier    = castAst<AstIdentifier>(identifierRef->children.back());

        // Be sure this is an attribute
        auto resolvedName = identifier->resolvedSymbolName;
        auto resolved     = identifier->resolvedSymbolOverload;
        if (resolvedName->kind != SymbolKind::Attribute)
        {
            Diagnostic err{identifier, identifier->token, FMT(Err(Err0217), resolvedName->name.c_str(), Naming::aKindName(resolvedName->kind).c_str())};
            context->report(err, Diagnostic::hereIs(resolved));
            return false;
        }

        // Check that global attribute is authorized
        if (node->hasSpecFlag(AstAttrUse::SPEC_FLAG_GLOBAL))
        {
            auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeInfo->attributeUsage & File))
            {
                Diagnostic err{identifier, identifier->token, FMT(Err(Err0493), resolvedName->name.c_str())};
                context->report(err, Diagnostic::hereIs(resolved));
                return false;
            }
        }

        // Register attribute itself
        OneAttribute oneAttribute;
        oneAttribute.name  = resolvedName->getFullName();
        oneAttribute.node  = node;
        oneAttribute.child = child;

        // Register all call parameters, and their value
        uint32_t numParams = 0;
        if (identifier->callParameters)
        {
            numParams = identifier->callParameters->children.count;
            for (auto one : identifier->callParameters->children)
            {
                auto param = castAst<AstFuncCallParam>(one, AstNodeKind::FuncCallParam);
                SWAG_CHECK(checkIsConstExpr(context, param->hasComputedValue(), param, Err(Err0037)));
                SWAG_CHECK(TypeManager::makeCompatibles(context, param->resolvedParameter->typeInfo, param->typeInfo, nullptr, param, CAST_FLAG_EXPLICIT));

                AttributeParameter attrParam;
                attrParam.token      = one->token;
                attrParam.token.text = param->resolvedParameter->name;
                attrParam.typeInfo   = param->resolvedParameter->typeInfo;
                attrParam.value      = *param->computedValue;
                oneAttribute.parameters.emplace_back(std::move(attrParam));
            }
        }

        // The rest (default parameters)
        auto funcDecl    = castAst<AstAttrDecl>(resolved->node, AstNodeKind::AttrDecl);
        auto typeFunc    = castTypeInfo<TypeInfoFuncAttr>(resolved->typeInfo, TypeInfoKind::FuncAttr);
        auto countParams = static_cast<int>(typeFunc->parameters.size());
        if (typeFunc->isFctVariadic())
            countParams--;
        for (int i = numParams; i < countParams; i++)
        {
            auto param = castAst<AstVarDecl>(funcDecl->parameters->children[i], AstNodeKind::FuncDeclParam);
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
        auto funcDeclParam = castAst<AstVarDecl>(node->content, AstNodeKind::FuncDeclParam);
        list               = &funcDeclParam->attributes;
        forNode            = node->content;
    }

    SWAG_CHECK(collectAttributes(context, forNode, list, node));
    return true;
}
