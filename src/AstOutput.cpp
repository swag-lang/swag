#include "pch.h"
#include "AstOutput.h"
#include "Ast.h"
#include "AstFlags.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

void AstOutput::incIndentStatement(const AstNode* node, int& indent)
{
    if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
        return;
    if (node->kind != AstNodeKind::Statement)
        indent++;
}

void AstOutput::decIndentStatement(const AstNode* node, int& indent)
{
    if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
        return;
    if (node->kind != AstNodeKind::Statement)
        indent--;
}

bool AstOutput::outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node)
{
    CONCAT_FIXED_STR(concat, "(");
    int idx = 0;
    for (const auto p : node->childs)
    {
        if (idx)
            CONCAT_FIXED_STR(concat, ", ");
        concat.addString(p->token.text);

        const AstVarDecl* varDecl = castAst<AstVarDecl>(p, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
        if (varDecl->type)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(context, concat, varDecl->type));
        }
        else if (varDecl->typeConstraint)
        {
            CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(context, concat, varDecl->typeConstraint));
        }

        if (varDecl->assignment)
        {
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(context, concat, varDecl->assignment));
        }

        idx++;
    }

    CONCAT_FIXED_STR(concat, ")");
    return true;
}

bool AstOutput::outputAttrUse(OutputContext& context, Concat& concat, AstNode* node, bool& hasSomething)
{
    const auto nodeAttr = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);

    // Be sure this is not an empty attribute block (empty or that contains
    // other empty blocks)
    hasSomething  = true;
    auto scanAttr = nodeAttr;
    while (scanAttr->content && scanAttr->content->kind == AstNodeKind::Statement)
    {
        if (scanAttr->content->childs.empty())
        {
            hasSomething = false;
            break;
        }
        if (scanAttr->content->childs.size() > 1)
            break;
        if (scanAttr->content->childs[0]->kind != AstNodeKind::AttrUse)
            break;
        scanAttr = (AstAttrUse*) scanAttr->content->childs[0];
    }

    if (!hasSomething)
        return true;

    bool first = true;
    for (const auto s : nodeAttr->childs)
    {
        if (s == nodeAttr->content)
            continue;
        if (s->kind == AstNodeKind::AttrUse)
            continue;

        if (!first)
            CONCAT_FIXED_STR(concat, ", ");
        else
            CONCAT_FIXED_STR(concat, "#[");
        first = false;
        SWAG_CHECK(outputNode(context, concat, s));
    }

    if (!first)
    {
        concat.addChar(']');
    }

    return true;
}

bool AstOutput::outputFuncName(OutputContext& context, Concat& concat, const AstFuncDecl* node)
{
    concat.addString(node->token.text);
    return true;
}

bool AstOutput::outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validif)
{
    ScopeExportNode sen(context, node);

    if (node->kind == AstNodeKind::AttrDecl)
        CONCAT_FIXED_STR(concat, "attr");
    else
        CONCAT_FIXED_STR(concat, "func");

    if (genericParameters)
    {
        concat.addChar('\'');
        SWAG_CHECK(outputNode(context, concat, genericParameters));
    }

    concat.addChar(' ');

    if (node->kind == AstNodeKind::FuncDecl && node->hasSpecFlag(AstFuncDecl::SPECFLAG_IMPL))
        CONCAT_FIXED_STR(concat, "impl ");

    if (node->kind == AstNodeKind::FuncDecl)
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        outputFuncName(context, concat, funcNode);
    }
    else
        concat.addString(node->token.text);

    // Parameters
    if (parameters)
        SWAG_CHECK(outputNode(context, concat, parameters));
    else
        CONCAT_FIXED_STR(concat, "()");

    // Return type, in case of function (not attr)
    if (node->kind == AstNodeKind::FuncDecl)
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

        auto returnNode = funcNode->returnType;
        if (returnNode && !returnNode->childs.empty())
            returnNode = returnNode->childs.front();

        if (typeFunc->returnType && !typeFunc->returnType->isVoid())
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
        }
        else if (funcNode->returnType && funcNode->returnType->hasSpecFlag(AstFuncDecl::SPECFLAG_RETURN_DEFINED))
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, returnNode));
        }
    }

    // Throw
    if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_THROW))
        CONCAT_FIXED_STR(concat, " throw");
    else if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_ASSUME))
        CONCAT_FIXED_STR(concat, " assume");

    // #validif must be exported
    if (validif)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, validif));
        context.indent--;
    }

    CONCAT_FIXED_STR(concat, ";");
    concat.addEol();
    return true;
}

void AstOutput::removeLastBlankLine(Concat& concat)
{
    auto p = concat.currentSP;
    if (p == concat.lastBucket->data)
        return;
    p--;
    if (*p != '\n')
        return;

    while (p != concat.lastBucket->data)
    {
        p--;
        if (SWAG_IS_BLANK(*p))
            continue;
        if (*p == '\n')
        {
            concat.currentSP = p + 1;
            concat.eolCount--;
            return;
        }

        return;
    }
}

bool AstOutput::outputFunc(OutputContext& context, Concat& concat, AstFuncDecl* node)
{
    PushErrCxtStep ec(&context, node, ErrCxtStepKind::Export, nullptr);
    CONCAT_FIXED_STR(concat, "func");

    // Emit generic parameter, except if the function is an instance
    if (node->genericParameters)
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC) || node->hasAstFlag(AST_IS_GENERIC))
            SWAG_CHECK(outputGenericParameters(context, concat, node->genericParameters));
    }

    // Implementation
    if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_IMPL))
        CONCAT_FIXED_STR(concat, " impl ");

    // Name
    CONCAT_FIXED_STR(concat, " ");
    outputFuncName(context, concat, node);

    // Parameters
    if (node->parameters)
        SWAG_CHECK(outputNode(context, concat, node->parameters));
    else
        CONCAT_FIXED_STR(concat, "()");

    // Return type
    auto returnNode = node->returnType;
    if (returnNode && !returnNode->childs.empty())
        returnNode = returnNode->childs.front();

    const auto typeFunc = node->typeInfo ? castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure) : nullptr;
    if (typeFunc && typeFunc->returnType && !typeFunc->returnType->isVoid())
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
    }
    else if (node->returnType && node->returnType->hasSpecFlag(AstFuncDecl::SPECFLAG_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(context, concat, returnNode));
    }

    // Throw
    if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_THROW))
        CONCAT_FIXED_STR(concat, " throw");
    else if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_ASSUME))
        CONCAT_FIXED_STR(concat, " assume");

    // Content, short lambda
    if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_SHORT_LAMBDA))
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(node->content->kind == AstNodeKind::Return || node->content->kind == AstNodeKind::Try);
        SWAG_CHECK(outputNode(context, concat, node->content->childs.front()));
        concat.addEol();
        return true;
    }

    // #validifx block
    if (node->validif)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->validif));
        context.indent--;
    }
    else if (node->content)
    {
        concat.addEolIndent(context.indent);
    }

    if (!node->content)
        return true;

    // Content, normal function
    concat.addChar('{');
    context.indent++;
    concat.addEol();

    if (node->content->kind != AstNodeKind::Statement)
    {
        concat.addIndent(context.indent);
        context.indent--;
        SWAG_CHECK(outputNode(context, concat, node->content));
        context.indent++;
        concat.addEol();
    }
    else
    {
        for (auto c : node->subDecls)
        {
            concat.addIndent(context.indent);
            if (c->parent && c->parent->kind == AstNodeKind::AttrUse)
                c = c->parent;
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }

        for (const auto c : node->content->childs)
        {
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }
    }

    removeLastBlankLine(concat);
    context.indent--;
    concat.addIndent(context.indent);
    concat.addChar('}');
    concat.addEol();
    return true;
}

bool AstOutput::outputEnum(OutputContext& context, Concat& concat, AstEnum* node)
{
    PushErrCxtStep ec(&context, node, ErrCxtStepKind::Export, nullptr);

    CONCAT_FIXED_STR(concat, "enum ");
    concat.addString(node->token.text);

    // Raw type
    if (!node->childs.front()->childs.empty())
    {
        CONCAT_FIXED_STR(concat, " : ");
        SWAG_ASSERT(node->childs[0]->kind == AstNodeKind::EnumType);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
    }

    concat.addEolIndent(context.indent);
    concat.addChar('{');
    concat.addEol();

    for (const auto c : node->childs)
    {
        if (c->kind == AstNodeKind::EnumValue)
        {
            concat.addIndent(context.indent + 1);

            if (c->hasSpecFlag(AstEnumValue::SPECFLAG_HAS_USING))
            {
                CONCAT_FIXED_STR(concat, "using ");
                SWAG_CHECK(outputNode(context, concat, c->childs.front()));
            }
            else
            {
                concat.addString(c->token.text);
                if (!c->childs.empty())
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(context, concat, c->childs.front()));
                }
            }

            concat.addEol();
        }
    }

    concat.addIndent(context.indent);
    concat.addChar('}');
    concat.addEol();
    return true;
}

bool AstOutput::outputAttributesUsage(const OutputContext& context, Concat& concat, const TypeInfoFuncAttr* typeFunc)
{
    bool first = true;
    concat.addIndent(context.indent);
    concat.addString("#[AttrUsage(");

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
    } while(0)

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

    concat.addString(")]");
    concat.addEol();
    return true;
}

bool AstOutput::outputAttributes(OutputContext& context, Concat& concat, AstNode* node, const TypeInfo* typeInfo, const AttributeList& attributes)
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
                bool hasSomehting = true;
                concat.addIndent(context.indent);
                SWAG_CHECK(outputAttrUse(context, concat, one.node, hasSomehting));
                concat.addEol();
            }
            else
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                else
                {
                    first = false;
                    concat.addIndent(context.indent);
                    CONCAT_FIXED_STR(concat, "#[");
                }

                concat.addString(one.name);
                if (!one.parameters.empty())
                {
                    concat.addChar('(');

                    for (size_t i = 0; i < one.parameters.size(); i++)
                    {
                        auto& oneParam = one.parameters[i];
                        if (i)
                            CONCAT_FIXED_STR(concat, ", ");
                        SWAG_CHECK(outputLiteral(context, concat, one.node, oneParam.typeInfo, oneParam.value));
                    }

                    concat.addChar(')');
                }
            }
        }

        if (!first)
        {
            CONCAT_FIXED_STR(concat, "]");
            concat.addEol();
        }
    }

    return true;
}

bool AstOutput::outputAttributesGlobalUsing(const OutputContext& context, Concat& concat, const AstNode* node)
{
    // Global using
    bool one         = false;
    bool outputUsing = true;
    if (node->hasAstFlag(AST_STRUCT_MEMBER))
        outputUsing = false;
    if (outputUsing)
    {
        for (const auto& p : node->sourceFile->globalUsings)
        {
            if (p->getFullName() == "Swag")
                continue;

            if (!one)
            {
                concat.addIndent(context.indent);
                CONCAT_FIXED_STR(concat, "#[Using(");
                one = true;
            }
            else
            {
                CONCAT_FIXED_STR(concat, ", ");
            }

            concat.addString(p->getFullName());
        }

        if (one)
        {
            CONCAT_FIXED_STR(concat, ")]");
            concat.addEol();
        }
    }

    return true;
}

bool AstOutput::outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo)
{
    switch (node->kind)
    {
    case AstNodeKind::ConstDecl:
    case AstNodeKind::NameAlias:
        return true;
    default:
        break;
    }

    SWAG_CHECK(outputAttributesGlobalUsing(context, concat, node));

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
    default:
        break;
    }

    if (!attr)
        return true;
    SWAG_CHECK(outputAttributes(context, concat, node, typeInfo, *attr));

    return true;
}

bool AstOutput::outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
{
    if (typeInfo->isPointerNull())
    {
        CONCAT_FIXED_STR(concat, "null");
        return true;
    }

    if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        SWAG_CHECK(outputNode(context, concat, node));
        return true;
    }

    if (typeInfo->isEnum())
    {
        const Utf8 str = Ast::enumToString(typeInfo, value.text, value.reg, true);
        concat.addString(str);
        return true;
    }

    SWAG_ASSERT(typeInfo->isNative());
    auto str = Ast::literalToString(typeInfo, value);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::String:
        concat.addString("\"" + str + "\"");
        break;
    case NativeTypeKind::Rune:
        concat.addString("\"" + str + "\"\'rune");
        break;
    case NativeTypeKind::Bool:
        concat.addString(str);
        break;
    default:
        if (!typeInfo->hasFlag(TYPEINFO_UNTYPED_INTEGER | TYPEINFO_UNTYPED_FLOAT))
        {
            str += '\'';
            str += typeInfo->name;
        }

        concat.addString(str);
        break;
    }

    return true;
}

bool AstOutput::outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node)
{
    const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Closure capture parameters
    if (funcDecl->captureParameters)
    {
        CONCAT_FIXED_STR(concat, "closure|");

        bool first = true;
        for (const auto p : funcDecl->captureParameters->childs)
        {
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;

            if (p->kind == AstNodeKind::MakePointer)
            {
                concat.addChar('&');
                concat.addString(p->childs.front()->token.text);
            }
            else
            {
                concat.addString(p->token.text);
            }
        }

        concat.addChar('|');
    }
    else
    {
        CONCAT_FIXED_STR(concat, "func");
    }

    // Parameters
    concat.addChar('(');
    if (funcDecl->parameters && !funcDecl->parameters->childs.empty())
    {
        bool first = true;
        for (const auto p : funcDecl->parameters->childs)
        {
            if (p->hasAstFlag(AST_GENERATED) && !p->hasAstFlag(AST_GENERATED_USER))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");

            first            = false;
            const auto param = castAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->hasSpecFlag(AstVarDecl::SPECFLAG_UNNAMED))
                concat.addChar('?');
            else
            {
                concat.addString(p->token.text);
                if (!p->childs.empty())
                {
                    CONCAT_FIXED_STR(concat, ": ");
                    SWAG_CHECK(outputNode(context, concat, p->childs.front()));
                }

                if (param->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(context, concat, param->assignment));
                }
            }
        }
    }

    concat.addChar(')');

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPECFLAG_SHORT_LAMBDA))
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(funcDecl->content->kind == AstNodeKind::Return);
        SWAG_CHECK(outputNode(context, concat, funcDecl->content->childs.front()));
    }
    else
    {
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, funcDecl->content));
    }

    return true;
}

bool AstOutput::outputVarDecl(OutputContext& context, Concat& concat, const AstVarDecl* varNode, bool isSelf)
{
    if (!varNode->hasSpecFlag(AstVarDecl::SPECFLAG_AUTO_NAME))
    {
        if (!varNode->publicName.empty())
            concat.addString(varNode->publicName);
        else
            concat.addString(varNode->token.text);
    }

    if (varNode->type)
    {
        if (!varNode->type->hasAstFlag(AST_GENERATED) || varNode->type->hasAstFlag(AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!varNode->hasSpecFlag(AstVarDecl::SPECFLAG_AUTO_NAME))
                    CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(outputNode(context, concat, varNode->type));
            }
        }
        else
        {
            CONCAT_FIXED_STR(concat, " = ");
            const auto typeExpr = castAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
            SWAG_ASSERT(!varNode->assignment);
            SWAG_ASSERT(typeExpr->identifier);
            SWAG_ASSERT(varNode->type->typeInfo && varNode->type->typeInfo->isTuple());
            const auto id = castAst<AstIdentifier>(typeExpr->identifier->childs.back(), AstNodeKind::Identifier);
            concat.addChar('{');
            SWAG_CHECK(outputNode(context, concat, id->callParameters));
            concat.addChar('}');
        }
    }

    if (varNode->assignment)
    {
        CONCAT_FIXED_STR(concat, " = ");
        SWAG_CHECK(outputNode(context, concat, varNode->assignment));
    }

    return true;
}

bool AstOutput::outputVar(OutputContext& context, Concat& concat, const AstVarDecl* varNode)
{
    if (varNode->hasAstFlag(AST_DECL_USING))
        CONCAT_FIXED_STR(concat, "using ");

    if (varNode->kind == AstNodeKind::ConstDecl)
    {
        CONCAT_FIXED_STR(concat, "const ");
    }
    else if (varNode->kind != AstNodeKind::FuncDeclParam && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPECFLAG_IS_LET))
            CONCAT_FIXED_STR(concat, "let ");
        else
            CONCAT_FIXED_STR(concat, "var ");
    }

    const bool isSelf = varNode->token.text == g_LangSpec->name_self;
    if (isSelf && varNode->type && ((AstTypeExpression*) varNode->type)->typeFlags & TYPEFLAG_IS_CONST)
    {
        CONCAT_FIXED_STR(concat, "const ");
    }

    SWAG_CHECK(outputVarDecl(context, concat, varNode, isSelf));
    return true;
}

bool AstOutput::outputStruct(OutputContext& context, Concat& concat, AstStruct* node)
{
    // If we need to export as opaque, and the struct has init values, then we add the
    // #[Swag.ExportType] attribute
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        {
            CONCAT_FIXED_STR(concat, "#[ExportType(\"nozero\")]");
            concat.addEolIndent(context.indent);
        }
    }

    PushErrCxtStep ec(&context, node, ErrCxtStepKind::Export, nullptr);

    if (node->kind == AstNodeKind::InterfaceDecl)
        CONCAT_FIXED_STR(concat, "interface");
    else
    {
        SWAG_ASSERT(node->kind == AstNodeKind::StructDecl);
        const auto structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl);
        if (structNode->hasSpecFlag(AstStruct::SPECFLAG_UNION))
            CONCAT_FIXED_STR(concat, "union");
        else
            CONCAT_FIXED_STR(concat, "struct");
    }

    if (node->genericParameters)
        SWAG_CHECK(outputGenericParameters(context, concat, node->genericParameters));

    if (!node->hasSpecFlag(AstStruct::SPECFLAG_ANONYMOUS))
    {
        CONCAT_FIXED_STR(concat, " ");
        concat.addString(node->token.text);
    }

    concat.addEolIndent(context.indent);

    // #validif must be exported
    if (node->validif)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->validif));
        context.indent--;
    }

    // Opaque export. Just simulate structure with the correct size.
    if (node->hasAttribute(ATTRIBUTE_OPAQUE))
    {
        concat.addChar('{');
        concat.addEol();

        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct);

        // Everything in the structure is not initialized
        if (typeStruct->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
        {
            SWAG_ASSERT(!typeStruct->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES));
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8 = ?", typeStruct->sizeOf);
            concat.addEol();
        }

        // Everything in the structure is initialized to zero
        else
        {
            concat.addIndent(context.indent + 1);
            concat.addStringFormat("padding: [%llu] u8", typeStruct->sizeOf);
            concat.addEol();
        }

        concat.addIndent(context.indent);
        concat.addChar('}');
    }
    else
    {
        SWAG_CHECK(outputNode(context, concat, node->content));
    }

    return true;
}

bool AstOutput::outputTypeTuple(OutputContext& context, Concat& concat, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concretePtrRef(typeInfo);
    SWAG_ASSERT(typeInfo->isTuple());
    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    const auto nodeStruct = castAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
    SWAG_CHECK(outputStruct(context, concat, nodeStruct));
    return true;
}

bool AstOutput::outputType(OutputContext& context, Concat& concat, AstTypeExpression* node)
{
    if (node->typeFlags & TYPEFLAG_IS_RETVAL)
    {
        CONCAT_FIXED_STR(concat, "retval");
        return true;
    }

    if (node->typeInfo)
    {
        // Identifier can have an export node, so in that case we need to export by node, not by type, in
        // order to export the real node (for example for an array of lambdas/closures)
        if (!node->identifier ||
            !node->identifier->hasExtMisc() ||
            !node->identifier->extMisc()->exportNode)
        {
            SWAG_CHECK(outputType(context, concat, node, node->typeInfo));

            if (node->identifier)
            {
                const auto id = castAst<AstIdentifier>(node->identifier->childs.back(), AstNodeKind::Identifier);
                if (id->callParameters)
                {
                    if (id->hasAstFlag(AST_GENERATED))
                    {
                        CONCAT_FIXED_STR(concat, " = {");
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                    else
                    {
                        concat.addChar('{');
                        SWAG_CHECK(outputNode(context, concat, id->callParameters));
                        concat.addChar('}');
                    }
                }
            }

            return true;
        }
    }

    if (node->typeFlags & TYPEFLAG_IS_CONST)
        CONCAT_FIXED_STR(concat, "const ");

    if (node->arrayDim == UINT8_MAX)
    {
        CONCAT_FIXED_STR(concat, "[] ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        return true;
    }

    if (node->arrayDim)
    {
        CONCAT_FIXED_STR(concat, "[");
        for (int i = 0; i < node->arrayDim; i++)
        {
            if (i)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs[i]));
        }

        CONCAT_FIXED_STR(concat, "] ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        return true;
    }

    if (node->typeFlags & TYPEFLAG_IS_SLICE)
    {
        CONCAT_FIXED_STR(concat, "[..] ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        return true;
    }

    if (node->typeFlags & TYPEFLAG_IS_PTR && node->typeFlags & TYPEFLAG_IS_PTR_ARITHMETIC)
    {
        concat.addChar('^');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        return true;
    }

    if (node->typeFlags & TYPEFLAG_IS_PTR)
    {
        concat.addChar('*');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        return true;
    }

    if (node->identifier)
    {
        SWAG_CHECK(outputNode(context, concat, node->identifier));
    }
    else
    {
        auto typeFromLiteral = node->typeFromLiteral;
        if (!typeFromLiteral)
            typeFromLiteral = TypeManager::literalTypeToType(node->literalType);
        SWAG_ASSERT(typeFromLiteral);
        SWAG_ASSERT(!typeFromLiteral->name.empty());
        concat.addString(typeFromLiteral->name);
    }

    return true;
}

bool AstOutput::outputType(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo)
{
    // Lambda
    /////////////////////////////////
    if (typeInfo->isLambdaClosure())
    {
        SWAG_ASSERT(typeInfo->declNode && typeInfo->declNode->kind == AstNodeKind::TypeLambda);
        SWAG_CHECK(outputNode(context, concat, typeInfo->declNode));
        return true;
    }

    // Tuple
    /////////////////////////////////
    if (typeInfo->isTuple())
    {
        SWAG_CHECK(outputTypeTuple(context, concat, typeInfo));
        return true;
    }

    // Other types
    /////////////////////////////////

    // When this is a reference, take the original (struct) type instead
    if (typeInfo->isAutoConstPointerRef())
    {
        const auto typeRef = castTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        typeInfo           = typeRef->pointedType;
    }

    if (typeInfo->isSelf())
    {
        if (typeInfo->isConst())
            CONCAT_FIXED_STR(concat, "const self");
        else
            CONCAT_FIXED_STR(concat, "self");
        return true;
    }

    // Export type name
    typeInfo->computeScopedNameExport();
    SWAG_ASSERT(!typeInfo->scopedNameExport.empty());
    concat.addString(typeInfo->scopedNameExport);

    return true;
}

bool AstOutput::outputNode(OutputContext& context, Concat& concat, AstNode* node)
{
    if (!node)
        return true;
    if (node->hasExtMisc() && node->extMisc()->exportNode)
        node = node->extMisc()->exportNode;
    if (node->hasAstFlag(AST_GENERATED) && !node->hasAstFlag(AST_GENERATED_USER))
        return true;

    // Prepend some stuff
    if (node->hasExtMisc() && node->extMisc()->isNamed)
    {
        concat.addString(node->extMisc()->isNamed->token.text);
        CONCAT_FIXED_STR(concat, ": ");
    }

    switch (node->kind)
    {
    case AstNodeKind::KeepRef:
        CONCAT_FIXED_STR(concat, "ref ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;
    case AstNodeKind::MoveRef:
        CONCAT_FIXED_STR(concat, "moveref ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::CompilerCode:
        concat.addChar(')');
        concat.addEolIndent(context.indent);
        concat.addChar('{');
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addEolIndent(context.indent);
        concat.addChar('}');
        concat.addEolIndent(context.indent);
        break;

    case AstNodeKind::With:
    {
        concat.addString("with ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;
    }

    case AstNodeKind::Throw:
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::Try:
    case AstNodeKind::Assume:
    {
        if (node->hasSpecFlag(AstTryCatchAssume::SPECFLAG_GENERATED) && node->hasSpecFlag(AstTryCatchAssume::SPECFLAG_BLOCK))
        {
            context.indent += 1;
            for (const auto c : node->childs.front()->childs)
            {
                SWAG_CHECK(outputNode(context, concat, c));
                concat.addEolIndent(context.indent);
            }

            context.indent -= 1;
        }
        else
        {
            if (node->hasAstFlag(AST_DISCARD))
                CONCAT_FIXED_STR(concat, "discard ");
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        }
        break;
    }

    case AstNodeKind::Catch:
    case AstNodeKind::TryCatch:
        if (node->hasAstFlag(AST_DISCARD))
            CONCAT_FIXED_STR(concat, "discard ");
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::FuncDeclType:
        if (!node->childs.empty())
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        }
        break;

    case AstNodeKind::FuncDeclParams:
    {
        concat.addChar('(');
        bool first = true;
        for (auto c : node->childs)
        {
            if (c->hasExtMisc() && c->extMisc()->exportNode)
                c = c->extMisc()->exportNode;
            if (c->hasAstFlag(AST_GENERATED) && !c->hasAstFlag(AST_GENERATED_USER))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            SWAG_CHECK(outputNode(context, concat, c));
        }
        concat.addChar(')');
        break;
    }

    case AstNodeKind::FuncDecl:
    {
        const auto nodeFunc = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        SWAG_CHECK(outputFunc(context, concat, nodeFunc));
        break;
    }

    case AstNodeKind::EnumType:
        if (!node->childs.empty())
            SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::EnumDecl:
    {
        const auto nodeEnum = castAst<AstEnum>(node, AstNodeKind::EnumDecl);
        SWAG_CHECK(outputEnum(context, concat, nodeEnum));
        break;
    }

    case AstNodeKind::StructContent:
    case AstNodeKind::TupleContent:
        CONCAT_FIXED_STR(concat, "{ ");
        concat.addEol();
        context.indent++;
        for (const auto c : node->childs)
        {
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }

        context.indent--;
        concat.addIndent(context.indent);
        concat.addChar('}');
        concat.addEol();
        break;

    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
    {
        const auto nodeStruct = castAst<AstStruct>(node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_CHECK(outputStruct(context, concat, nodeStruct));
        break;
    }

    case AstNodeKind::Defer:
        CONCAT_FIXED_STR(concat, "defer ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        concat.addEol();
        break;

    case AstNodeKind::AttrUse:
    {
        bool hasSomething = true;
        outputAttrUse(context, concat, node, hasSomething);
        if (!hasSomething)
            break;
        concat.addEolIndent(context.indent);
        const auto nodeAttr = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);
        SWAG_CHECK(outputNode(context, concat, nodeAttr->content));
        break;
    }

    case AstNodeKind::ExplicitNoInit:
        CONCAT_FIXED_STR(concat, "undefined");
        break;

    case AstNodeKind::Index:
        CONCAT_FIXED_STR(concat, "#index");
        break;
    case AstNodeKind::GetErr:
        CONCAT_FIXED_STR(concat, "@err");
        break;
    case AstNodeKind::Init:
        CONCAT_FIXED_STR(concat, "@init(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::Drop:
        CONCAT_FIXED_STR(concat, "@drop(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::PostMove:
        CONCAT_FIXED_STR(concat, "@postmove(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;
    case AstNodeKind::PostCopy:
        CONCAT_FIXED_STR(concat, "@postcopy(");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (node->childs.size() == 2)
        {
            CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, node->childs.back()));
        }
        concat.addChar(')');
        break;

    case AstNodeKind::Break:
    {
        const auto nodeBreak = castAst<AstBreakContinue>(node, AstNodeKind::Break);
        CONCAT_FIXED_STR(concat, "break ");
        concat.addString(nodeBreak->label.text);
        break;
    }

    case AstNodeKind::Continue:
    {
        const auto nodeContinue = castAst<AstBreakContinue>(node, AstNodeKind::Continue);
        CONCAT_FIXED_STR(concat, "continue ");
        concat.addString(nodeContinue->label.text);
        break;
    }

    case AstNodeKind::MakePointer:
    {
        concat.addChar('&');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::MakePointerLambda:
    {
        const auto lambdaNode = castAst<AstMakePointer>(node, AstNodeKind::MakePointerLambda);
        SWAG_CHECK(outputLambdaExpression(context, concat, lambdaNode->lambda));
        break;
    }

    case AstNodeKind::NoDrop:
        CONCAT_FIXED_STR(concat, "nodrop ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::Move:
        CONCAT_FIXED_STR(concat, "move ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::ArrayPointerSlicing:
    {
        const auto arrayNode = castAst<AstArrayPointerSlicing>(node, AstNodeKind::ArrayPointerSlicing);
        SWAG_CHECK(outputNode(context, concat, arrayNode->array));
        concat.addChar('[');
        SWAG_CHECK(outputNode(context, concat, arrayNode->lowerBound));
        if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPECFLAG_EXCLUDE_UP))
            CONCAT_FIXED_STR(concat, "..<");
        else
            CONCAT_FIXED_STR(concat, "..");
        SWAG_CHECK(outputNode(context, concat, arrayNode->upperBound));
        concat.addChar(']');
        break;
    }

    case AstNodeKind::ArrayPointerIndex:
    {
        const auto arrayNode = castAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);
        if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPECFLAG_IS_DEREF))
        {
            CONCAT_FIXED_STR(concat, "dref ");
            SWAG_CHECK(outputNode(context, concat, arrayNode->array));
        }
        else
        {
            SWAG_CHECK(outputNode(context, concat, arrayNode->array));
            concat.addChar('[');
            SWAG_CHECK(outputNode(context, concat, arrayNode->access));
            concat.addChar(']');
        }
        break;
    }

    case AstNodeKind::ExpressionList:
    {
        const auto exprNode = castAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
        if (exprNode->hasSpecFlag(AstExpressionList::SPECFLAG_FOR_TUPLE))
            concat.addChar('{');
        else
            concat.addChar('[');

        int idx = 0;
        for (const auto child : exprNode->childs)
        {
            if (child->hasAstFlag(AST_GENERATED))
                continue;
            if (idx++)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, child));
        }

        if (exprNode->hasSpecFlag(AstExpressionList::SPECFLAG_FOR_TUPLE))
            concat.addChar('}');
        else
            concat.addChar(']');
        break;
    }

    case AstNodeKind::CompilerLoad:
        CONCAT_FIXED_STR(concat, "#load ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    case AstNodeKind::CompilerInclude:
        CONCAT_FIXED_STR(concat, "#include ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerRun:
    case AstNodeKind::CompilerRunExpression:
    case AstNodeKind::CompilerAst:
    case AstNodeKind::CompilerValidIf:
    case AstNodeKind::CompilerValidIfx:
    {
        if (node->kind == AstNodeKind::CompilerRun || node->kind == AstNodeKind::CompilerRunExpression)
            CONCAT_FIXED_STR(concat, "#run ");
        else if (node->kind == AstNodeKind::CompilerAst)
            CONCAT_FIXED_STR(concat, "#ast ");
        else if (node->kind == AstNodeKind::CompilerValidIf)
            CONCAT_FIXED_STR(concat, "#validif ");
        else if (node->kind == AstNodeKind::CompilerValidIfx)
            CONCAT_FIXED_STR(concat, "#validifx ");

        const auto front = node->childs.front();
        if (front->kind == AstNodeKind::FuncDecl)
        {
            const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(front, AstNodeKind::FuncDecl);
            incIndentStatement(funcDecl->content, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, funcDecl->content));
            decIndentStatement(funcDecl->content, context.indent);
            concat.addEol();
        }
        else
        {
            SWAG_CHECK(outputNode(context, concat, front));
            concat.addEol();
        }
        break;
    }

    case AstNodeKind::CompilerIfBlock:
        if (node->childs.front()->kind != AstNodeKind::Statement)
            CONCAT_FIXED_STR(concat, "#do ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerMacro:
        CONCAT_FIXED_STR(concat, "#macro");
        incIndentStatement(node->childs.front(), context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        decIndentStatement(node->childs.front(), context.indent);
        break;

    case AstNodeKind::CompilerMixin:
    {
        const auto compilerMixin = castAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
        CONCAT_FIXED_STR(concat, "#mixin ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        if (!compilerMixin->replaceTokens.empty())
        {
            CONCAT_FIXED_STR(concat, " { ");
            for (const auto m : compilerMixin->replaceTokens)
            {
                switch (m.first)
                {
                case TokenId::KwdBreak:
                    CONCAT_FIXED_STR(concat, "break");
                    break;
                case TokenId::KwdContinue:
                    CONCAT_FIXED_STR(concat, "continue");
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
                }

                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(outputNode(context, concat, m.second));
                CONCAT_FIXED_STR(concat, "; ");
            }
            concat.addChar('}');
        }
        break;
    }

    case AstNodeKind::CompilerPrint:
        CONCAT_FIXED_STR(concat, "#print ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    case AstNodeKind::CompilerError:
        CONCAT_FIXED_STR(concat, "#error ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    case AstNodeKind::CompilerWarning:
        CONCAT_FIXED_STR(concat, "#warning ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;

    case AstNodeKind::CompilerAssert:
        CONCAT_FIXED_STR(concat, "#assert ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        if (node->childs.size() > 1)
        {
            concat.addChar(',');
            SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        }

        break;

    case AstNodeKind::CompilerIf:
    {
        const auto compilerIf = castAst<AstIf>(node, AstNodeKind::CompilerIf);
        CONCAT_FIXED_STR(concat, "#if ");
        SWAG_CHECK(outputNode(context, concat, compilerIf->boolExpression));

        incIndentStatement(compilerIf->ifBlock, context.indent);
        concat.addEolIndent(context.indent);

        SWAG_CHECK(outputNode(context, concat, compilerIf->ifBlock));
        decIndentStatement(compilerIf->ifBlock, context.indent);

        if (compilerIf->elseBlock)
        {
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "#else ");

            if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
            {
                incIndentStatement(compilerIf->elseBlock, context.indent);
                concat.addEolIndent(context.indent);
            }

            SWAG_CHECK(outputNode(context, concat, compilerIf->elseBlock));
            if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
            {
                decIndentStatement(compilerIf->elseBlock, context.indent);
            }
        }
        break;
    }

    case AstNodeKind::If:
    {
        const auto compilerIf = castAst<AstIf>(node, AstNodeKind::If, AstNodeKind::CompilerIf);
        CONCAT_FIXED_STR(concat, "if ");

        if (compilerIf->hasSpecFlag(AstIf::SPECFLAG_ASSIGN))
        {
            const auto varNode = castAst<AstVarDecl>(compilerIf->childs.front(), AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
            if (varNode->tokenId == TokenId::KwdConst)
                CONCAT_FIXED_STR(concat, "const ");
            else
                CONCAT_FIXED_STR(concat, "var ");
            SWAG_CHECK(outputVarDecl(context, concat, varNode, false));
        }
        else
            SWAG_CHECK(outputNode(context, concat, compilerIf->boolExpression));

        incIndentStatement(compilerIf->ifBlock, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, compilerIf->ifBlock));
        decIndentStatement(compilerIf->ifBlock, context.indent);

        if (compilerIf->elseBlock)
        {
            concat.addEolIndent(context.indent);
            CONCAT_FIXED_STR(concat, "else");
            incIndentStatement(compilerIf->elseBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, compilerIf->elseBlock));
            decIndentStatement(compilerIf->elseBlock, context.indent);
        }
        break;
    }

    case AstNodeKind::For:
    {
        const auto forNode = castAst<AstFor>(node, AstNodeKind::For);
        CONCAT_FIXED_STR(concat, "for ");
        SWAG_CHECK(outputNode(context, concat, forNode->preExpression));
        CONCAT_FIXED_STR(concat, "; ");
        SWAG_CHECK(outputNode(context, concat, forNode->boolExpression));
        CONCAT_FIXED_STR(concat, "; ");
        SWAG_CHECK(outputNode(context, concat, forNode->postExpression));
        incIndentStatement(forNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, forNode->block));
        decIndentStatement(forNode->block, context.indent);
        break;
    }

    case AstNodeKind::Visit:
    {
        const auto visitNode = castAst<AstVisit>(node, AstNodeKind::Visit);
        CONCAT_FIXED_STR(concat, "visit ");

        if (visitNode->hasSpecFlag(AstVisit::SPECFLAG_WANT_POINTER))
            concat.addChar('&');

        bool first = true;
        for (auto& a : visitNode->aliasNames)
        {
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            concat.addString(a.text);
        }

        if (!visitNode->aliasNames.empty())
            CONCAT_FIXED_STR(concat, ": ");
        SWAG_CHECK(outputNode(context, concat, visitNode->expression));
        incIndentStatement(visitNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, visitNode->block));
        decIndentStatement(visitNode->block, context.indent);
        break;
    }

    case AstNodeKind::Loop:
    {
        const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
        CONCAT_FIXED_STR(concat, "loop");
        if (loopNode->hasSpecFlag(AstLoop::SPECFLAG_BACK))
            CONCAT_FIXED_STR(concat, ",back");
        concat.addChar(' ');

        if (loopNode->specificName)
        {
            concat.addString(loopNode->specificName->token.text);
            CONCAT_FIXED_STR(concat, ": ");
        }

        SWAG_CHECK(outputNode(context, concat, loopNode->expression));
        incIndentStatement(loopNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, loopNode->block));
        decIndentStatement(loopNode->block, context.indent);
        break;
    }

    case AstNodeKind::While:
    {
        const auto whileNode = castAst<AstWhile>(node, AstNodeKind::While);
        CONCAT_FIXED_STR(concat, "while ");
        SWAG_CHECK(outputNode(context, concat, whileNode->boolExpression));
        incIndentStatement(whileNode->block, context.indent);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, whileNode->block));
        decIndentStatement(whileNode->block, context.indent);
        break;
    }

    case AstNodeKind::CompilerSpecialValue:
    {
        switch (node->tokenId)
        {
        case TokenId::CompilerSelf:
            CONCAT_FIXED_STR(concat, "#self");
            break;
        case TokenId::CompilerCallerFunction:
            CONCAT_FIXED_STR(concat, "#callerfunction");
            break;
        case TokenId::CompilerCallerLocation:
            CONCAT_FIXED_STR(concat, "#callerlocation");
            break;
        case TokenId::CompilerLocation:
            CONCAT_FIXED_STR(concat, "#location");
            break;
        case TokenId::CompilerOs:
            CONCAT_FIXED_STR(concat, "#os");
            break;
        case TokenId::CompilerSwagOs:
            CONCAT_FIXED_STR(concat, "#swagos");
            break;
        case TokenId::CompilerArch:
            CONCAT_FIXED_STR(concat, "#arch");
            break;
        case TokenId::CompilerCpu:
            CONCAT_FIXED_STR(concat, "#cpu");
            break;
        case TokenId::CompilerBackend:
            CONCAT_FIXED_STR(concat, "#backend");
            break;
        case TokenId::CompilerBuildCfg:
            CONCAT_FIXED_STR(concat, "#cfg");
            break;
        default:
            Report::internalError(node, "Ast::outputNode, unknown compiler function");
        }
        break;
    }

    case AstNodeKind::ConditionalExpression:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, " ? (");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        CONCAT_FIXED_STR(concat, ") : (");
        SWAG_CHECK(outputNode(context, concat, node->childs[2]));
        concat.addChar(')');
        break;

    case AstNodeKind::TypeAlias:
    {
        CONCAT_FIXED_STR(concat, "typealias ");
        concat.addString(node->token.text);
        CONCAT_FIXED_STR(concat, " = ");
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }
    case AstNodeKind::NameAlias:
    {
        CONCAT_FIXED_STR(concat, "namealias ");
        concat.addString(node->token.text);
        CONCAT_FIXED_STR(concat, " = ");
        const auto back = node->childs.back();
        if (back->resolvedSymbolName)
            concat.addString(back->resolvedSymbolName->getFullName());
        else
            SWAG_CHECK(outputNode(context, concat, back));
        break;
    }

    case AstNodeKind::ConstDecl:
    case AstNodeKind::VarDecl:
    case AstNodeKind::FuncDeclParam:
    {
        const AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
        SWAG_CHECK(outputVar(context, concat, varDecl));
        break;
    }

    case AstNodeKind::Using:
    {
        CONCAT_FIXED_STR(concat, "using ");
        int idx = 0;
        for (const auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ", ");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        break;
    }

    case AstNodeKind::Return:
    {
        CONCAT_FIXED_STR(concat, "return ");
        if (!node->childs.empty())
            SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::IntrinsicDefined:
    case AstNodeKind::IntrinsicLocation:
        concat.addString(node->token.text);
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        concat.addChar(')');
        break;

    case AstNodeKind::IntrinsicProp:
    {
        const auto propertyNode = castAst<AstIntrinsicProp>(node, AstNodeKind::IntrinsicProp);
        concat.addString(propertyNode->token.text);
        concat.addChar('(');
        int idx = 0;
        for (const auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ",");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        concat.addChar(')');
        break;
    }

    case AstNodeKind::IdentifierRef:
    {
        if (node->hasAstFlag(AST_DISCARD))
            CONCAT_FIXED_STR(concat, "discard ");
        if (node->hasSpecFlag(AstIdentifierRef::SPECFLAG_AUTO_SCOPE))
            CONCAT_FIXED_STR(concat, ".");
        int idx = 0;
        for (const auto child : node->childs)
        {
            if (idx)
                CONCAT_FIXED_STR(concat, ".");
            SWAG_CHECK(outputNode(context, concat, child));
            idx++;
        }

        break;
    }

    case AstNodeKind::Identifier:
    {
        const auto ident = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (ident->identifierExtension && ident->identifierExtension->scopeUpMode != IdentifierScopeUpMode::None)
        {
            CONCAT_FIXED_STR(concat, "#up");
            if (ident->identifierExtension->scopeUpMode == IdentifierScopeUpMode::Count &&
                ident->identifierExtension->scopeUpValue.literalValue.u8 > 1)
            {
                concat.addChar('(');
                concat.addStringFormat("%d", ident->identifierExtension->scopeUpValue.literalValue.u8);
                concat.addChar(')');
            }

            concat.addChar(' ');
        }
    }

    case AstNodeKind::FuncCall:
    {
        const auto identifier = static_cast<AstIdentifier*>(node);
        concat.addString(node->token.text);

        if (identifier->genericParameters)
        {
            concat.addChar('\'');
            concat.addChar('(');
            SWAG_CHECK(outputNode(context, concat, identifier->genericParameters));
            concat.addChar(')');
        }

        if (identifier->callParameters)
        {
            if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
                concat.addChar('{');
            else
                concat.addChar('(');
            SWAG_CHECK(outputNode(context, concat, identifier->callParameters));
            if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
                concat.addChar('}');
            else if (identifier->callParameters->childs.empty())
                concat.addChar(')');
            else if (identifier->callParameters->childs.back()->childs.back()->kind != AstNodeKind::CompilerCode)
                concat.addChar(')');
        }

        break;
    }

    case AstNodeKind::Switch:
    {
        const auto nodeSwitch = castAst<AstSwitch>(node, AstNodeKind::Switch);
        CONCAT_FIXED_STR(concat, "switch ");
        SWAG_CHECK(outputNode(context, concat, nodeSwitch->expression));
        concat.addEolIndent(context.indent);
        concat.addChar('{');
        concat.addEolIndent(context.indent);

        for (const auto c : nodeSwitch->cases)
        {
            if (c->expressions.empty())
                CONCAT_FIXED_STR(concat, "default");
            else
            {
                CONCAT_FIXED_STR(concat, "case ");
                bool first = true;
                for (const auto e : c->expressions)
                {
                    if (!first)
                        CONCAT_FIXED_STR(concat, ", ");
                    SWAG_CHECK(outputNode(context, concat, e));
                    first = false;
                }
            }
            concat.addChar(':');
            concat.addEolIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, c->block));
            concat.addEolIndent(context.indent);
        }

        concat.addChar('}');
        concat.addEolIndent(context.indent);
        break;
    }

    case AstNodeKind::StatementNoScope:
    {
        for (const auto child : node->childs)
        {
            if (child->hasAstFlag(AST_GENERATED))
                continue;
            context.indent++;
            SWAG_CHECK(outputNode(context, concat, child));
            context.indent--;
            concat.addEolIndent(context.indent);
        }

        break;
    }

    case AstNodeKind::Statement:
    case AstNodeKind::SwitchCaseBlock:
        concat.addChar('{');
        concat.addEol();
        for (const auto child : node->childs)
        {
            concat.addIndent(context.indent + 1);
            context.indent++;
            SWAG_CHECK(outputNode(context, concat, child));
            context.indent--;
            concat.addEol();
        }

        removeLastBlankLine(concat);
        concat.addIndent(context.indent);
        concat.addChar('}');
        concat.addEolIndent(context.indent);
        break;

    case AstNodeKind::FuncCallParam:
    {
        SWAG_CHECK(outputNode(context, concat, node->childs.front()));
        break;
    }

    case AstNodeKind::FuncCallParams:
    {
        const auto funcCallParams = castAst<AstFuncCallParams>(node, AstNodeKind::FuncCallParams);

        // Aliases
        bool first = true;
        if (!funcCallParams->aliasNames.empty())
        {
            CONCAT_FIXED_STR(concat, "|");
            for (auto& t : funcCallParams->aliasNames)
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                concat.addString(t.text);
                first = false;
            }

            CONCAT_FIXED_STR(concat, "| ");
        }

        first = true;
        for (const auto child : funcCallParams->childs)
        {
            if (child->hasAstFlag(AST_GENERATED))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;
            SWAG_CHECK(outputNode(context, concat, child));
        }

        break;
    }

    case AstNodeKind::SingleOp:
        concat.addString(node->token.text);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::NullConditionalExpression:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addString(" orelse ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::AffectOp:
    {
        const auto opNode = castAst<AstOp>(node, AstNodeKind::AffectOp);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        if (opNode->hasSpecFlag(AstOp::SPECFLAG_OVERFLOW))
            CONCAT_FIXED_STR(concat, ",over");
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;
    }

    case AstNodeKind::FactorOp:
    {
        const auto opNode = castAst<AstOp>(node, AstNodeKind::FactorOp);
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        if (opNode->hasSpecFlag(AstOp::SPECFLAG_OVERFLOW))
            CONCAT_FIXED_STR(concat, ",over");
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        concat.addChar(')');
        break;
    }

    case AstNodeKind::BinaryOp:
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        concat.addChar(' ');
        concat.addString(node->token.text);
        concat.addChar(' ');
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        concat.addChar(')');
        break;

    case AstNodeKind::AutoCast:
        CONCAT_FIXED_STR(concat, "acast ");
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::Cast:
        CONCAT_FIXED_STR(concat, "cast");
        if (node->hasSpecFlag(AstCast::SPECFLAG_OVERFLOW))
            CONCAT_FIXED_STR(concat, ",over");
        if (node->hasSpecFlag(AstCast::SPECFLAG_BIT))
            CONCAT_FIXED_STR(concat, ",bit");
        concat.addChar('(');
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        CONCAT_FIXED_STR(concat, ") ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::TypeExpression:
    {
        auto typeNode = static_cast<AstTypeExpression*>(node);
        if (typeNode->hasSpecFlag(AstType::SPECFLAG_FORCE_TYPE))
            concat.addString("#type ");
        SWAG_CHECK(outputType(context, concat, typeNode));
        break;
    }

    case AstNodeKind::Literal:
    {
        const auto literalNode = castAst<AstLiteral>(node, AstNodeKind::Literal);
        if (literalNode->literalType == LiteralType::TT_STRING_RAW)
            CONCAT_FIXED_STR(concat, "#\"");
        else if (literalNode->literalType == LiteralType::TT_STRING || literalNode->literalType == LiteralType::TT_STRING_ESCAPE)
            CONCAT_FIXED_STR(concat, "\"");
        else if (literalNode->literalType == LiteralType::TT_STRING_MULTILINE || literalNode->literalType == LiteralType::TT_STRING_MULTILINE_ESCAPE)
            CONCAT_FIXED_STR(concat, "\"\"\"");
        else if (literalNode->literalType == LiteralType::TT_CHARACTER || literalNode->literalType == LiteralType::TT_CHARACTER_ESCAPE)
            CONCAT_FIXED_STR(concat, "`");

        concat.addString(node->token.text);

        if (literalNode->literalType == LiteralType::TT_STRING_RAW)
            CONCAT_FIXED_STR(concat, "\"#");
        else if (literalNode->literalType == LiteralType::TT_STRING || literalNode->literalType == LiteralType::TT_STRING_ESCAPE)
            CONCAT_FIXED_STR(concat, "\"");
        else if (literalNode->literalType == LiteralType::TT_STRING_MULTILINE || literalNode->literalType == LiteralType::TT_STRING_MULTILINE_ESCAPE)
            CONCAT_FIXED_STR(concat, "\"\"\"");
        else if (literalNode->literalType == LiteralType::TT_CHARACTER || literalNode->literalType == LiteralType::TT_CHARACTER_ESCAPE)
            CONCAT_FIXED_STR(concat, "`");

        if (!node->childs.empty())
        {
            CONCAT_FIXED_STR(concat, "'");
            SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        }
        break;
    }

    case AstNodeKind::ScopeBreakable:
        CONCAT_FIXED_STR(concat, "#scope ");
        concat.addString(node->token.text);
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        break;

    case AstNodeKind::Range:
        SWAG_CHECK(outputNode(context, concat, node->childs[0]));
        if (node->hasSpecFlag(AstRange::SPECFLAG_EXCLUDE_UP))
            CONCAT_FIXED_STR(concat, " until ");
        else
            CONCAT_FIXED_STR(concat, " to ");
        SWAG_CHECK(outputNode(context, concat, node->childs[1]));
        break;

    case AstNodeKind::TypeLambda:
    case AstNodeKind::TypeClosure:
    {
        const AstTypeLambda* typeNode = static_cast<AstTypeLambda*>(node);
        if (typeNode->hasSpecFlag(AstType::SPECFLAG_FORCE_TYPE))
            concat.addString("#type ");
        if (node->kind == AstNodeKind::TypeLambda)
            CONCAT_FIXED_STR(concat, "func");
        else
            CONCAT_FIXED_STR(concat, "closure");
        if (!typeNode->parameters)
            CONCAT_FIXED_STR(concat, "()");
        else
            SWAG_CHECK(outputNode(context, concat, typeNode->parameters));
        if (typeNode->returnType)
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, typeNode->returnType));
        }

        if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_THROW))
            CONCAT_FIXED_STR(concat, " throw");
        else if (node->hasSpecFlag(AstFuncDecl::SPECFLAG_ASSUME))
            CONCAT_FIXED_STR(concat, " assume");
        break;
    }

    default:
        return Report::internalError(node, "Ast::outputNode, unknown funcDecl");
    }

    return true;
}

bool AstOutput::outputScopeContent(OutputContext& context, Concat& concat, const Module* moduleToGen, const Scope* scope)
{
    const auto publicSet = scope->publicSet;
    if (!publicSet)
        return true;

    // Stuff
    if (!publicSet->publicNodes.empty())
    {
        for (const auto one : publicSet->publicNodes)
        {
            SWAG_CHECK(outputAttributes(context, concat, one, one->typeInfo));
            concat.addIndent(context.indent);
            SWAG_CHECK(outputNode(context, concat, one));
            concat.addEol();
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
            concat.addIndent(context.indent);
            concat.addStringFormat(R"(#[Foreign("%s", "%s")])", moduleToGen->name.c_str(), node->fullnameForeignExport.c_str());
            concat.addEol();
            SWAG_CHECK(outputAttributes(context, concat, node, node->typeInfo));

            concat.addIndent(context.indent);
            if (node->token.text == g_LangSpec->name_opInitGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opInit(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opDropGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opDrop(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostCopyGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostCopy(using self);");
                concat.addEol();
            }
            else if (node->token.text == g_LangSpec->name_opPostMoveGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostMove(using self);");
                concat.addEol();
            }
            else
                SWAG_CHECK(outputFuncSignature(context, concat, node, nullptr, node->parameters, node->validif));
        }
    }

    // Attributes
    if (!publicSet->publicAttr.empty())
    {
        for (const auto one : publicSet->publicAttr)
        {
            const auto              node     = castAst<AstAttrDecl>(one, AstNodeKind::AttrDecl);
            const TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(outputAttributesUsage(context, concat, typeFunc));
            concat.addIndent(context.indent);
            SWAG_CHECK(outputFuncSignature(context, concat, node, nullptr, node->parameters, nullptr));
        }
    }

    return true;
}

bool AstOutput::outputScope(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!(scope->flags & SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags & SCOPE_IMPORTED)
        return true;

    context.forExport = true;

    // Namespace
    if (scope->kind == ScopeKind::Namespace && !scope->name.empty())
    {
        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "namespace ");
            concat.addString(scope->name);
            concat.addEol();
            concat.addIndent(context.indent);
            concat.addChar('{');
            concat.addEol();
            context.indent++;
        }

        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));

        if (!(scope->flags & SCOPE_AUTO_GENERATED))
        {
            context.indent--;
            removeLastBlankLine(concat);
            concat.addIndent(context.indent);
            concat.addChar('}');
            concat.addEol();
            concat.addEol();
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        concat.addIndent(context.indent);
        if (scope->kind == ScopeKind::Impl)
        {
            const auto nodeImpl = castAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            const auto symbol   = nodeImpl->identifier->resolvedSymbolOverload;
            concat.addStringFormat("impl %s for ", symbol->node->getScopedName().c_str());
            concat.addString(scope->parentScope->name);
            concat.addEol();
        }
        else if (scope->kind == ScopeKind::Enum)
        {
            CONCAT_FIXED_STR(concat, "impl enum ");
            concat.addString(scope->name);
            concat.addEol();
        }
        else
        {
            CONCAT_FIXED_STR(concat, "impl ");
            concat.addString(scope->name);
            concat.addEol();
        }

        concat.addIndent(context.indent);
        concat.addChar('{');
        concat.addEol();

        context.indent++;
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->kind == ScopeKind::Impl)
                continue;
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        }

        context.indent--;
        removeLastBlankLine(concat);
        concat.addIndent(context.indent);
        concat.addChar('}');
        concat.addEol();
        concat.addEol();

        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->kind != ScopeKind::Impl)
                continue;
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        }
    }

    // Named scope
    else if (!scope->name.empty())
    {
        concat.addIndent(context.indent);
        concat.addChar('{');
        concat.addEol();

        context.indent++;
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
        context.indent--;

        removeLastBlankLine(concat);
        concat.addIndent(context.indent);
        concat.addChar('}');
        concat.addEol();
        concat.addEol();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(outputScopeContent(context, concat, moduleToGen, scope));
        for (const auto oneScope : scope->childScopes)
            SWAG_CHECK(outputScope(context, concat, moduleToGen, oneScope));
    }

    return true;
}
