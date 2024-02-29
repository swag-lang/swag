#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

void FormatAst::incIndentStatement(const AstNode* node, uint32_t& idt)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        idt++;
}

void FormatAst::decIndentStatement(const AstNode* node, uint32_t& idt)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        idt--;
}

bool FormatAst::outputGenericParameters(AstNode* node)
{
    CONCAT_FIXED_STR(concat, "(");
    int idx = 0;
    for (const auto p : node->children)
    {
        if (idx)
            CONCAT_FIXED_STR(concat, ", ");
        concat->addString(p->token.text);

        const AstVarDecl* varDecl = castAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(varDecl->type));
        }
        else if (varDecl->typeConstraint)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(varDecl->typeConstraint));
        }

        if (varDecl->assignment)
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(varDecl->assignment));
        }

        idx++;
    }

    CONCAT_FIXED_STR(concat, ")");
    return true;
}

bool FormatAst::outputAttrUse(AstNode* node, bool& hasSomething)
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
        if (scanAttr->content->children[0]->isNot(AstNodeKind::AttrUse))
            break;
        scanAttr = castAst<AstAttrUse>(scanAttr->content->children[0]);
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

bool FormatAst::outputEnum(AstEnum* node)
{
    CONCAT_FIXED_STR(concat, "enum ");
    concat->addString(node->token.text);

    // Raw type
    if (node->firstChild()->childCount())
    {
        concat->addBlank();
        concat->addChar(':');
        concat->addBlank();
        SWAG_ASSERT(node->firstChild()->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(node->firstChild()));
    }

    concat->addEolIndent(indent);
    concat->addChar('{');
    concat->addEol();

    for (const auto c : node->children)
    {
        if (c->is(AstNodeKind::EnumValue))
        {
            concat->addIndent(indent + 1);

            if (c->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
            {
                CONCAT_FIXED_STR(concat, "using ");
                SWAG_CHECK(outputNode(c->firstChild()));
            }
            else
            {
                concat->addString(c->token.text);
                if (!c->children.empty())
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(c->firstChild()));
                }
            }

            concat->addEol();
        }
    }

    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputAttributesUsage(const TypeInfoFuncAttr* typeFunc)
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

bool FormatAst::outputAttributes(AstNode* /*node*/, const TypeInfo* typeInfo, const AttributeList& attributes)
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

bool FormatAst::outputAttributesGlobalUsing(const AstNode* node)
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

bool FormatAst::outputAttributes(AstNode* node, TypeInfo* typeInfo)
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

bool FormatAst::outputLiteral(AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo->isPointerNull())
    {
        CONCAT_FIXED_STR(concat, "null");
        return true;
    }

    if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        SWAG_CHECK(outputNode(node));
        return true;
    }

    if (typeInfo->isEnum())
    {
        const Utf8 str = Ast::enumToString(typeInfo, value.text, value.reg, true);
        concat->addString(str);
        return true;
    }

    SWAG_ASSERT(typeInfo->isNative());
    auto str = Ast::literalToString(typeInfo, value);
    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::String:
            concat->addString("\"" + str + "\"");
            break;
        case NativeTypeKind::Rune:
            concat->addString("\"" + str + "\"\'rune");
            break;
        case NativeTypeKind::Bool:
            concat->addString(str);
            break;
        default:
            if (!typeInfo->hasFlag(TYPEINFO_UNTYPED_INTEGER | TYPEINFO_UNTYPED_FLOAT))
            {
                str += '\'';
                str += typeInfo->name;
            }

            concat->addString(str);
            break;
    }

    return true;
}

bool FormatAst::outputVarDecl(const AstVarDecl* varNode, bool isSelf)
{
    if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
    {
        if (!varNode->publicName.empty())
            concat->addString(varNode->publicName);
        else
            concat->addString(varNode->token.text);
    }

    if (varNode->type)
    {
        if (!varNode->type->hasAstFlag(AST_GENERATED) || varNode->type->hasAstFlag(AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
                    CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(outputNode(varNode->type));
            }
        }
        else
        {
            CONCAT_FIXED_STR(concat, " = ");
            const auto typeExpr = castAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
            SWAG_ASSERT(!varNode->assignment);
            SWAG_ASSERT(typeExpr->identifier);
            SWAG_ASSERT(varNode->type->typeInfo && varNode->type->typeInfo->isTuple());
            const auto id = castAst<AstIdentifier>(typeExpr->identifier->lastChild(), AstNodeKind::Identifier);
            concat->addChar('{');
            SWAG_CHECK(outputNode(id->callParameters));
            concat->addChar('}');
        }
    }

    if (varNode->assignment)
    {
        CONCAT_FIXED_STR(concat, " = ");
        SWAG_CHECK(outputNode(varNode->assignment));
    }

    return true;
}

bool FormatAst::outputVar(const AstVarDecl* varNode)
{
    if (varNode->hasAstFlag(AST_DECL_USING))
        CONCAT_FIXED_STR(concat, "using ");

    if (varNode->is(AstNodeKind::ConstDecl))
    {
        CONCAT_FIXED_STR(concat, "const ");
    }
    else if (varNode->isNot(AstNodeKind::FuncDeclParam) && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
            CONCAT_FIXED_STR(concat, "let ");
        else
            CONCAT_FIXED_STR(concat, "var ");
    }

    const bool isSelf = varNode->token.text == g_LangSpec->name_self;
    if (isSelf && varNode->type && castAst<AstTypeExpression>(varNode->type)->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const ");
    }

    SWAG_CHECK(outputVarDecl(varNode, isSelf));
    return true;
}

bool FormatAst::outputScopeContent(const Module* moduleToGen, const Scope* scope)
{
    const auto publicSet = scope->publicSet;
    if (!publicSet)
        return true;

    // Stuff
    if (!publicSet->publicNodes.empty())
    {
        for (const auto one : publicSet->publicNodes)
        {
            SWAG_CHECK(outputAttributes(one, one->typeInfo));
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(one));
            concat->addEol();
        }
    }

    // Functions
    if (!publicSet->publicFunc.empty())
    {
        for (const auto one : publicSet->publicFunc)
        {
            AstFuncDecl* node = castAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);

            // Can be removed in case of special functions
            if (!node->hasAttribute(ATTRIBUTE_PUBLIC))
                continue;

            // Remap special functions to their generated equivalent
            node->computeFullNameForeignExport();
            concat->addIndent(indent);
            concat->addStringFormat(R"(#[Foreign("%s", "%s")])", moduleToGen->name.c_str(), node->fullnameForeignExport.c_str());
            concat->addEol();
            SWAG_CHECK(outputAttributes(node, node->typeInfo));

            concat->addIndent(indent);
            if (node->token.text == g_LangSpec->name_opInitGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opInit(using self);");
                concat->addEol();
            }
            else if (node->token.text == g_LangSpec->name_opDropGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opDrop(using self);");
                concat->addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostCopyGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostCopy(using self);");
                concat->addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostMoveGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostMove(using self);");
                concat->addEol();
            }
            else
                SWAG_CHECK(outputFuncSignature(node, nullptr, node->parameters, node->validIf));
        }
    }

    // Attributes
    if (!publicSet->publicAttr.empty())
    {
        for (const auto one : publicSet->publicAttr)
        {
            const auto              node     = castAst<AstAttrDecl>(one, AstNodeKind::AttrDecl);
            const TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(outputAttributesUsage(typeFunc));
            concat->addIndent(indent);
            SWAG_CHECK(outputFuncSignature(node, nullptr, node->parameters, nullptr));
        }
    }

    return true;
}

bool FormatAst::outputScope(Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->flags.has(SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags.has(SCOPE_IMPORTED))
        return true;

    forExport = true;

    // Namespace
    if (scope->is(ScopeKind::Namespace) && !scope->name.empty())
    {
        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
        {
            concat->addIndent(indent);
            CONCAT_FIXED_STR(concat, "namespace ");
            concat->addString(scope->name);
            concat->addEol();
            concat->addIndent(indent);
            concat->addChar('{');
            concat->addEol();
            indent++;
        }

        SWAG_CHECK(outputScopeContent(moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(moduleToGen, oneScope));

        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
        {
            indent--;
            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEol();
            concat->addEol();
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        concat->addIndent(indent);
        if (scope->is(ScopeKind::Impl))
        {
            const auto nodeImpl = castAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            const auto symbol   = nodeImpl->identifier->resolvedSymbolOverload();
            concat->addStringFormat("impl %s for ", symbol->node->getScopedName().c_str());
            concat->addString(scope->parentScope->name);
            concat->addEol();
        }
        else if (scope->is(ScopeKind::Enum))
        {
            CONCAT_FIXED_STR(concat, "impl enum ");
            concat->addString(scope->name);
            concat->addEol();
        }
        else
        {
            CONCAT_FIXED_STR(concat, "impl ");
            concat->addString(scope->name);
            concat->addEol();
        }

        concat->addIndent(indent);
        concat->addChar('{');
        concat->addEol();

        indent++;
        SWAG_CHECK(outputScopeContent(moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->is(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(moduleToGen, oneScope));
        }

        indent--;
        concat->addIndent(indent);
        concat->addChar('}');
        concat->addEol();
        concat->addEol();

        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->isNot(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(moduleToGen, oneScope));
        }
    }

    // Named scope
    else if (!scope->name.empty())
    {
        concat->addIndent(indent);
        concat->addChar('{');
        concat->addEol();

        indent++;
        SWAG_CHECK(outputScopeContent(moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(moduleToGen, oneScope));
        indent--;

        concat->addIndent(indent);
        concat->addChar('}');
        concat->addEol();
        concat->addEol();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(outputScopeContent(moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(moduleToGen, oneScope));
    }

    return true;
}
