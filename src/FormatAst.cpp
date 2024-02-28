#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

void FormatAst::incIndentStatement(const AstNode* node, int& indent)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        indent++;
}

void FormatAst::decIndentStatement(const AstNode* node, int& indent)
{
    if (node->is(AstNodeKind::CompilerIfBlock) && node->firstChild()->is(AstNodeKind::Statement))
        return;
    if (node->isNot(AstNodeKind::Statement))
        indent--;
}

bool FormatAst::outputGenericParameters(OutputContext& context, Concat& concat, AstNode* node)
{
    CONCAT_FIXED_STR(concat, "(");
    int idx = 0;
    for (const auto p : node->children)
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

bool FormatAst::outputAttrUse(OutputContext& context, Concat& concat, AstNode* node, bool& hasSomething)
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
        SWAG_CHECK(outputNode(context, concat, s));
    }

    if (!first)
    {
        concat.addChar(']');
    }

    return true;
}

bool FormatAst::outputFuncName(OutputContext&, Concat& concat, const AstFuncDecl* node)
{
    concat.addString(node->token.text);
    return true;
}

void FormatAst::removeLastBlankLine(Concat& concat)
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

bool FormatAst::outputFuncSignature(OutputContext& context, Concat& concat, AstNode* node, AstNode* genericParameters, AstNode* parameters, AstNode* validIf)
{
    ScopeExportNode sen(context, node);

    if (node->is(AstNodeKind::AttrDecl))
        CONCAT_FIXED_STR(concat, "attr");
    else
        CONCAT_FIXED_STR(concat, "func");

    if (genericParameters)
    {
        concat.addChar('\'');
        SWAG_CHECK(outputNode(context, concat, genericParameters));
    }

    concat.addChar(' ');

    if (node->is(AstNodeKind::FuncDecl) && node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
        CONCAT_FIXED_STR(concat, "impl ");

    if (node->is(AstNodeKind::FuncDecl))
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
    if (node->is(AstNodeKind::FuncDecl))
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

        auto returnNode = funcNode->returnType;
        if (returnNode && !returnNode->children.empty())
            returnNode = returnNode->firstChild();

        if (typeFunc->returnType && !typeFunc->returnType->isVoid())
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
        }
        else if (funcNode->returnType && funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(context, concat, returnNode));
        }
    }

    // Throw
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
        CONCAT_FIXED_STR(concat, " throw");
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
        CONCAT_FIXED_STR(concat, " assume");

    // #validif must be exported
    if (validIf)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, validIf));
        context.indent--;
    }

    CONCAT_FIXED_STR(concat, ";");
    concat.addEol();
    return true;
}

bool FormatAst::outputFunc(OutputContext& context, Concat& concat, AstFuncDecl* node)
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
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
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
    if (returnNode && !returnNode->children.empty())
        returnNode = returnNode->firstChild();

    const auto typeFunc = node->typeInfo ? castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure) : nullptr;
    if (typeFunc && typeFunc->returnType && !typeFunc->returnType->isVoid())
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputType(context, concat, returnNode, typeFunc->returnType));
    }
    else if (node->returnType && node->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(context, concat, returnNode));
    }

    // Throw
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
        CONCAT_FIXED_STR(concat, " throw");
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
        CONCAT_FIXED_STR(concat, " assume");

    // Content, short lambda
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(node->content->is(AstNodeKind::Return) || node->content->is(AstNodeKind::Try));
        SWAG_CHECK(outputNode(context, concat, node->content->firstChild()));
        concat.addEol();
        return true;
    }

    // #validifx block
    if (node->validIf)
    {
        context.indent++;
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, node->validIf));
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

    if (node->content->isNot(AstNodeKind::Statement))
    {
        concat.addIndent(context.indent);
        context.indent--;
        SWAG_CHECK(outputNode(context, concat, node->content));
        context.indent++;
        concat.addEol();
    }
    else
    {
        for (auto c : node->subDecl)
        {
            concat.addIndent(context.indent);
            if (c->parent && c->parent->is(AstNodeKind::AttrUse))
                c = c->parent;
            SWAG_CHECK(outputNode(context, concat, c));
            concat.addEol();
        }

        for (const auto c : node->content->children)
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

bool FormatAst::outputEnum(OutputContext& context, Concat& concat, AstEnum* node)
{
    PushErrCxtStep ec(&context, node, ErrCxtStepKind::Export, nullptr);

    CONCAT_FIXED_STR(concat, "enum ");
    concat.addString(node->token.text);

    // Raw type
    if (!node->firstChild()->children.empty())
    {
        CONCAT_FIXED_STR(concat, " : ");
        SWAG_ASSERT(node->children[0]->is(AstNodeKind::EnumType));
        SWAG_CHECK(outputNode(context, concat, node->children[0]));
    }

    concat.addEolIndent(context.indent);
    concat.addChar('{');
    concat.addEol();

    for (const auto c : node->children)
    {
        if (c->is(AstNodeKind::EnumValue))
        {
            concat.addIndent(context.indent + 1);

            if (c->hasSpecFlag(AstEnumValue::SPEC_FLAG_HAS_USING))
            {
                CONCAT_FIXED_STR(concat, "using ");
                SWAG_CHECK(outputNode(context, concat, c->firstChild()));
            }
            else
            {
                concat.addString(c->token.text);
                if (!c->children.empty())
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(context, concat, c->firstChild()));
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

bool FormatAst::outputAttributesUsage(const OutputContext& context, Concat& concat, const TypeInfoFuncAttr* typeFunc)
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

    concat.addString(")]");
    concat.addEol();
    return true;
}

bool FormatAst::outputAttributes(OutputContext& context, Concat& concat, AstNode* /*node*/, const TypeInfo* typeInfo, const AttributeList& attributes)
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
                concat.addIndent(context.indent);
                bool hasSomething = true;
                SWAG_CHECK(outputAttrUse(context, concat, one.node, hasSomething));
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

                    for (uint32_t i = 0; i < one.parameters.size(); i++)
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

bool FormatAst::outputAttributesGlobalUsing(const OutputContext& context, Concat& concat, const AstNode* node)
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

bool FormatAst::outputAttributes(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo)
{
    switch (node->kind)
    {
        case AstNodeKind::ConstDecl:
        case AstNodeKind::NameAlias:
            return true;
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
    }

    if (!attr)
        return true;
    SWAG_CHECK(outputAttributes(context, concat, node, typeInfo, *attr));

    return true;
}

bool FormatAst::outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value)
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

bool FormatAst::outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node)
{
    const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Closure capture parameters
    if (funcDecl->captureParameters)
    {
        CONCAT_FIXED_STR(concat, "closure|");

        bool first = true;
        for (const auto p : funcDecl->captureParameters->children)
        {
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");
            first = false;

            if (p->is(AstNodeKind::MakePointer))
            {
                concat.addChar('&');
                concat.addString(p->firstChild()->token.text);
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
    if (funcDecl->parameters && !funcDecl->parameters->children.empty())
    {
        bool first = true;
        for (const auto p : funcDecl->parameters->children)
        {
            if (p->hasAstFlag(AST_GENERATED) && !p->hasAstFlag(AST_GENERATED_USER))
                continue;
            if (!first)
                CONCAT_FIXED_STR(concat, ", ");

            first            = false;
            const auto param = castAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED))
                concat.addChar('?');
            else
            {
                concat.addString(p->token.text);
                if (!p->children.empty())
                {
                    CONCAT_FIXED_STR(concat, ": ");
                    SWAG_CHECK(outputNode(context, concat, p->firstChild()));
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

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        CONCAT_FIXED_STR(concat, " => ");
        SWAG_ASSERT(funcDecl->content->is(AstNodeKind::Return));
        SWAG_CHECK(outputNode(context, concat, funcDecl->content->firstChild()));
    }
    else
    {
        concat.addEolIndent(context.indent);
        SWAG_CHECK(outputNode(context, concat, funcDecl->content));
    }

    return true;
}

bool FormatAst::outputVarDecl(OutputContext& context, Concat& concat, const AstVarDecl* varNode, bool isSelf)
{
    if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
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
                if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME))
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
            const auto id = castAst<AstIdentifier>(typeExpr->identifier->lastChild(), AstNodeKind::Identifier);
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

bool FormatAst::outputVar(OutputContext& context, Concat& concat, const AstVarDecl* varNode)
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

    SWAG_CHECK(outputVarDecl(context, concat, varNode, isSelf));
    return true;
}

bool FormatAst::outputScopeContent(OutputContext& context, Concat& concat, const Module* moduleToGen, const Scope* scope)
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
                SWAG_CHECK(outputFuncSignature(context, concat, node, nullptr, node->parameters, node->validIf));
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

bool FormatAst::outputScope(OutputContext& context, Concat& concat, Module* moduleToGen, Scope* scope)
{
    SWAG_ASSERT(moduleToGen);
    if (!scope->flags.has(SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags.has(SCOPE_IMPORTED))
        return true;

    context.forExport = true;

    // Namespace
    if (scope->is(ScopeKind::Namespace) && !scope->name.empty())
    {
        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
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

        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
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
        if (scope->is(ScopeKind::Impl))
        {
            const auto nodeImpl = castAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            const auto symbol   = nodeImpl->identifier->resolvedSymbolOverload();
            concat.addStringFormat("impl %s for ", symbol->node->getScopedName().c_str());
            concat.addString(scope->parentScope->name);
            concat.addEol();
        }
        else if (scope->is(ScopeKind::Enum))
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
            if (oneScope->is(ScopeKind::Impl))
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
            if (oneScope->isNot(ScopeKind::Impl))
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
