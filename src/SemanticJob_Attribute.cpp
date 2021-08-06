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
        return context->report({oneAttribute, Utf8::format(Msg0582, oneAttribute->typeInfo->getDisplayName().c_str(), TypeInfo::getArticleKindName(oneAttribute->typeInfo))});

    auto kind     = checkNode->kind;
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(checkNode);

    if (typeInfo->attributeUsage == AttributeUsage::All)
        return true;

    bool isGlobalVar = kind == AstNodeKind::VarDecl && checkNode->ownerScope->isGlobalOrImpl();
    bool isStructVar = kind == AstNodeKind::VarDecl && (checkNode->flags & AST_STRUCT_MEMBER);
    bool isLocalVar  = kind == AstNodeKind::VarDecl && !isGlobalVar && !isStructVar;

    // Check specific hard coded attributes
    SWAG_ASSERT(oneAttribute->typeInfo->declNode);
    if (oneAttribute->typeInfo->declNode->sourceFile->isBootstrapFile)
    {
        if (oneAttribute->token.text == g_LangSpec.name_Complete && kind == AstNodeKind::Switch)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_AttrUsage && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_AttrMulti && kind == AstNodeKind::AttrDecl)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_Global && isLocalVar)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_Strict && kind == AstNodeKind::Alias)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_PrintBc && kind == AstNodeKind::CompilerAst)
            return true;
        if (oneAttribute->token.text == g_LangSpec.name_Align && (kind == AstNodeKind::VarDecl || kind == AstNodeKind::StructDecl))
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
        Diagnostic diag{oneAttribute, Utf8::format(Msg0583, oneAttribute->token.text.c_str(), specificMsg)};
        Diagnostic note1{checkNode, checkNode->token, Utf8::format(Msg0584, nakedName.c_str()), DiagnosticLevel::Note};
        Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, Utf8::format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note1, &note2);
    }
    else
    {
        auto nakedName = AstNode::getArticleKindName(checkNode);
        if (nakedName == "<node>")
        {
            Diagnostic diag{oneAttribute, Utf8::format(Msg0586, oneAttribute->token.text.c_str())};
            Diagnostic note1{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, Utf8::format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note1);
        }
        else
        {
            auto       nakedName1 = AstNode::getKindName(checkNode);
            Diagnostic diag{oneAttribute, Utf8::format(Msg0588, oneAttribute->token.text.c_str(), nakedName.c_str())};
            Diagnostic note1{checkNode, checkNode->token, Utf8::format(Msg0589, nakedName1.c_str()), DiagnosticLevel::Note};
            Diagnostic note2{oneAttribute->resolvedSymbolOverload->node, oneAttribute->resolvedSymbolOverload->node->token, Utf8::format(Msg0587, oneAttribute->token.text.c_str()), DiagnosticLevel::Note};
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

void SemanticJob::inheritAttributesFromMainFunc(AstNode* child)
{
    SWAG_ASSERT(child->kind == AstNodeKind::FuncDecl);
    SWAG_ASSERT(child->ownerFct);

    auto attributeFlags = child->ownerFct->attributeFlags;

    child->attributeFlags |= attributeFlags & ATTRIBUTE_PRINT_BC;

    INHERIT(child, ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_CASTANY_ON | ATTRIBUTE_SAFETY_CASTANY_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
    INHERIT(child, ATTRIBUTE_SAFETY_NULLPTR_ON | ATTRIBUTE_SAFETY_NULLPTR_OFF);
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

        // Inherit flags
        auto attributeFlags = curAttr->attributeFlags;
        if (flags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE | ATTRIBUTE_PROTECTED))
            attributeFlags &= ~(ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE | ATTRIBUTE_PROTECTED);

        forNode->attributeFlags |= attributeFlags & ~(ATTRIBUTE_SAFETY_MASK | ATTRIBUTE_OPTIM_MASK | ATTRIBUTE_SELECTIF_MASK);
        INHERIT(forNode, ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_CASTANY_ON | ATTRIBUTE_SAFETY_CASTANY_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_NULLPTR_ON | ATTRIBUTE_SAFETY_NULLPTR_OFF);
        INHERIT(forNode, ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_OVERFLOW_OFF);
        INHERIT(forNode, ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
        INHERIT(forNode, ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
        INHERIT(forNode, ATTRIBUTE_SELECTIF_ON | ATTRIBUTE_SELECTIF_OFF);

        for (auto child : curAttr->childs)
        {
            if (child == curAttr->content)
                continue;
            if (!child->typeInfo || child->typeInfo->kind != TypeInfoKind::FuncAttr)
                continue;

            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeInfo->attributes.hasAttribute(g_LangSpec.name_Swag_AttrMulti))
            {
                if (isHereTmp.contains(typeInfo))
                {
                    Diagnostic diag{forNode, forNode->token, Utf8::format(Msg0591, child->token.text.c_str(), forNode->token.text.c_str(), child->token.text.c_str())};
                    Diagnostic note{child, child->token, Msg0592, DiagnosticLevel::Note};
                    return context->report(diag, &note);
                }

                if (isHereGlobal.contains(typeInfo))
                    continue;
            }

            // Attribute on an attribute : usage
            if (forNode->kind == AstNodeKind::AttrDecl)
            {
                auto value = curAttr->attributes.getValue(g_LangSpec.name_Swag_AttrUsage, g_LangSpec.name_usage);
                if (value)
                {
                    auto typeAttr            = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage = value->reg.u32;
                }

                if (curAttr->attributes.hasAttribute(g_LangSpec.name_Swag_AttrMulti))
                {
                    auto typeAttr = CastTypeInfo<TypeInfoFuncAttr>(forNode->typeInfo, TypeInfoKind::FuncAttr);
                    typeAttr->attributeUsage |= AttributeUsage::Multi;
                }
            }

            // Predefined attributes will mark some flags (to speed up detection)
            auto it = g_LangSpec.attributesFlags.find(child->token.text);
            if (it)
                flags |= *it;

            // Else search by name (attributes with parameters)
            else if (child->token.text == g_LangSpec.name_ExportType)
            {
                auto attrWhat = curAttr->attributes.getValue(g_LangSpec.name_Swag_ExportType, g_LangSpec.name_what);
                SWAG_ASSERT(attrWhat);
                auto text = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                for (auto& w : what)
                {
                    w.trim();
                    if (w == g_LangSpec.name_methods)
                        flags |= ATTRIBUTE_EXPORT_TYPE_METHODS;
                    else
                        return context->report({child, Utf8::format(Msg0599, w.c_str())});
                }
            }
            else if (child->token.text == g_LangSpec.name_Safety)
            {
                auto attrWhat = curAttr->attributes.getValue(g_LangSpec.name_Swag_Safety, g_LangSpec.name_what);
                SWAG_ASSERT(attrWhat);
                auto text = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                auto attrValue = curAttr->attributes.getValue(g_LangSpec.name_Swag_Safety, g_LangSpec.name_value);
                SWAG_ASSERT(attrValue);

                if (text.empty())
                {
                    flags &= ~ATTRIBUTE_SAFETY_MASK;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_NULLPTR_ON : ATTRIBUTE_SAFETY_NULLPTR_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_CASTANY_ON : ATTRIBUTE_SAFETY_CASTANY_OFF;
                }

                for (auto& w : what)
                {
                    w.trim();
                    if (w == g_LangSpec.name_nullptr)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_NULLPTR_ON | ATTRIBUTE_SAFETY_NULLPTR_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_NULLPTR_ON : ATTRIBUTE_SAFETY_NULLPTR_OFF;
                    }
                    else if (w == g_LangSpec.name_boundcheck)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_BOUNDCHECK_ON : ATTRIBUTE_SAFETY_BOUNDCHECK_OFF;
                    }
                    else if (w == g_LangSpec.name_overflow)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_OVERFLOW_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_OVERFLOW_ON : ATTRIBUTE_SAFETY_OVERFLOW_OFF;
                    }
                    else if (w == g_LangSpec.name_math)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_MATH_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_MATH_ON : ATTRIBUTE_SAFETY_MATH_OFF;
                    }
                    else if (w == g_LangSpec.name_castany)
                    {
                        flags &= ~(ATTRIBUTE_SAFETY_CASTANY_ON | ATTRIBUTE_SAFETY_CASTANY_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_SAFETY_CASTANY_ON : ATTRIBUTE_SAFETY_CASTANY_OFF;
                    }
                    else
                        return context->report({child, Utf8::format(Msg0593, w.c_str())});
                }
            }
            else if (child->token.text == g_LangSpec.name_Optim)
            {
                auto attrWhat = curAttr->attributes.getValue(g_LangSpec.name_Swag_Optim, g_LangSpec.name_what);
                SWAG_ASSERT(attrWhat);
                auto text = attrWhat->text;
                text.trim();
                vector<Utf8> what;
                Utf8::tokenize(text, '|', what);

                auto attrValue = curAttr->attributes.getValue(g_LangSpec.name_Swag_Optim, g_LangSpec.name_value);
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
                    if (w == g_LangSpec.name_bytecode)
                    {
                        flags &= ~(ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BYTECODE_ON : ATTRIBUTE_OPTIM_BYTECODE_OFF;
                    }
                    else if (w == g_LangSpec.name_backend)
                    {
                        flags &= ~(ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF);
                        flags |= attrValue->reg.b ? ATTRIBUTE_OPTIM_BACKEND_ON : ATTRIBUTE_OPTIM_BACKEND_OFF;
                    }
                    else
                        return context->report({child, Utf8::format(Msg0594, w.c_str())});
                }
            }
            else if (child->token.text == g_LangSpec.name_SelectIf)
            {
                auto attrValue = curAttr->attributes.getValue(g_LangSpec.name_Swag_SelectIf, g_LangSpec.name_value);
                SWAG_ASSERT(attrValue);
                flags &= ~(ATTRIBUTE_SELECTIF_MASK);
                flags |= attrValue->reg.b ? ATTRIBUTE_SELECTIF_ON : ATTRIBUTE_SELECTIF_OFF;
            }
            else if (child->token.text == g_LangSpec.name_Pack)
            {
                auto attrValue = curAttr->attributes.getValue(g_LangSpec.name_Swag_Pack, g_LangSpec.name_value);
                SWAG_ASSERT(attrValue);
                SWAG_VERIFY(!attrValue->reg.u8 || isPowerOfTwo(attrValue->reg.u8), context->report({child, Utf8::format(Msg0595, attrValue->reg.u8)}));
            }
            else if (child->token.text == g_LangSpec.name_Align)
            {
                auto attrValue = curAttr->attributes.getValue(g_LangSpec.name_Swag_Align, g_LangSpec.name_value);
                SWAG_ASSERT(attrValue);
                SWAG_VERIFY(isPowerOfTwo(attrValue->reg.u8), context->report({child, Utf8::format(Msg0596, attrValue->reg.u8)}));
            }

            // Remember attributes that's here
            isHereTmp.insert(typeInfo);
            isHereGlobal.insert(typeInfo);
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
            Diagnostic diag{identifier, Utf8::format(Msg0598, resolvedName->name.c_str())};
            Diagnostic note{resolved->node, resolved->node->token, Utf8::format(Msg0018, resolvedName->name.c_str()), DiagnosticLevel::Note};
            context->report(diag, &note);
            return false;
        }

        // Check that global attribute is authorized
        if (node->specFlags & AST_SPEC_ATTRUSE_GLOBAL)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
            if (!(typeInfo->attributeUsage & AttributeUsage::File))
            {
                Diagnostic diag{identifier, Utf8::format(Msg0600, resolvedName->name.c_str())};
                Diagnostic note{resolved->node, resolved->node->token, Utf8::format(Msg0018, resolvedName->name.c_str()), DiagnosticLevel::Note};
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
                attrParam.value    = *param->computedValue;
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
            attrParam.value    = *param->assignment->computedValue;
            oneAttribute.parameters.emplace_back(move(attrParam));
        }

        node->attributes.emplace(oneAttribute);
    }

    SWAG_CHECK(collectAttributes(context, node, nullptr, node));
    return true;
}
