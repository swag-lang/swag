#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"

bool FormatAst::outputFuncDeclParameters(const AstNode* parameters, bool isMethod)
{
    if (!parameters)
    {
        CONCAT_FIXED_STR(concat, "()");
        return true;
    }

    concat->addChar('(');
    SWAG_CHECK(outputCommaChildren(parameters, isMethod ? 1 : 0));
    concat->addChar(')');
    return true;
}

bool FormatAst::outputFuncReturnType(const AstFuncDecl* funcNode)
{
    const auto typeFunc = funcNode->typeInfo ? castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure) : nullptr;

    auto returnNode = funcNode->returnType;
    if (returnNode && !returnNode->children.empty())
        returnNode = returnNode->firstChild();

    if (typeFunc && typeFunc->returnType && !typeFunc->returnType->isVoid())
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputType(returnNode, typeFunc->returnType));
    }
    else if (funcNode->returnType && funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(returnNode));
    }

    return true;
}

bool FormatAst::outputFuncSignature(const AstNode* node, const AstNode* genericParameters, const AstNode* parameters, const AstNode* validIf)
{
    bool isMethod = false;

    if (node->is(AstNodeKind::AttrDecl))
    {
        CONCAT_FIXED_STR(concat, "attr");
    }
    else if (node->is(AstNodeKind::FuncDecl) && node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_METHOD))
    {
        isMethod = true;
        CONCAT_FIXED_STR(concat, "mtd");
    }
    else
        CONCAT_FIXED_STR(concat, "func");

    if (genericParameters)
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC) || node->hasAstFlag(AST_IS_GENERIC))
            SWAG_CHECK(outputGenericParameters(genericParameters));
    }

    concat->addBlank();

    // const/impl for a function
    if (node->is(AstNodeKind::FuncDecl))
    {
        if (isMethod)
        {
            const auto varDecl  = castAst<AstVarDecl>(parameters->firstChild());
            const auto typeDecl = castAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
            if (typeDecl->typeFlags.has(TYPEFLAG_IS_CONST))
            {
                CONCAT_FIXED_STR(concat, "const");
                concat->addBlank();
            }
        }

        if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
        {
            CONCAT_FIXED_STR(concat, "impl");
            concat->addBlank();
        }
    }

    // Name
    concat->addString(node->token.text);

    // Parameters
    SWAG_CHECK(outputFuncDeclParameters(parameters, isMethod));

    // Return type, in case of function (not attr)
    if (node->is(AstNodeKind::FuncDecl))
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        SWAG_CHECK(outputFuncReturnType(funcNode));
    }

    // Throw
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "throw");
    }
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "assume");
    }

    // #validif must be exported
    if (validIf)
    {
        concat->addEol();
        indent++;
        concat->addIndent(indent);
        SWAG_CHECK(outputCompilerExpr(validIf));
        indent--;
    }

    return true;
}

bool FormatAst::outputFuncDecl(const AstFuncDecl* node)
{
    if (node->hasAttribute(ATTRIBUTE_AST_FUNC))
        CONCAT_FIXED_STR(concat, "#ast");
    else if (node->hasAttribute(ATTRIBUTE_RUN_FUNC))
        CONCAT_FIXED_STR(concat, "#run");
    else if (node->hasAttribute(ATTRIBUTE_TEST_FUNC))
        CONCAT_FIXED_STR(concat, "#test");
    else if (node->hasAttribute(ATTRIBUTE_MAIN_FUNC))
        CONCAT_FIXED_STR(concat, "#main");
    else if (node->hasAttribute(ATTRIBUTE_INIT_FUNC))
        CONCAT_FIXED_STR(concat, "#init");
    else if (node->hasAttribute(ATTRIBUTE_DROP_FUNC))
        CONCAT_FIXED_STR(concat, "#drop");
    else if (node->hasAttribute(ATTRIBUTE_PREMAIN_FUNC))
        CONCAT_FIXED_STR(concat, "#premain");
    else if (node->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
    {
        CONCAT_FIXED_STR(concat, "#message");
        concat->addChar('(');
        SWAG_CHECK(outputNode(node->parameters));
        concat->addChar(')');
    }
    else
        SWAG_CHECK(outputFuncSignature(node, node->genericParameters, node->parameters, nullptr));

    // Content, short lambda
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        if (node->content->is(AstNodeKind::Return))
            SWAG_CHECK(outputNode(node->content->firstChild()));
        else if (node->content->is(AstNodeKind::Try))
            SWAG_CHECK(outputNode(node->content->firstChild()->firstChild()));
        else
            SWAG_ASSERT(false);
        concat->addEol();
        return true;
    }

    // Content, short form
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=");
        concat->addBlank();
        SWAG_CHECK(outputNode(node->content->firstChild()));
        concat->addEol();
        return true;
    }

    // #validifx block
    if (node->validIf)
    {
        concat->addEol();
        indent++;
        concat->addIndent(indent);
        SWAG_CHECK(outputCompilerExpr(node->validIf));
        indent--;
    }

    if (!node->content)
    {
        concat->addChar(';');
        concat->addEol();
        return true;
    }

    // Content, normal function
    concat->addEol();
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();
    indent++;

    if (node->content->isNot(AstNodeKind::Statement))
    {
        concat->addIndent(indent);
        indent--;
        SWAG_CHECK(outputNode(node->content));
        indent++;
        concat->addEol();
    }
    else
    {
        for (auto c : node->subDecl)
        {
            concat->addIndent(indent);
            if (c->parent && c->parent->is(AstNodeKind::AttrUse))
                c = c->parent;
            SWAG_CHECK(outputNode(c));
            concat->addEol();
        }

        for (const auto c : node->content->children)
        {
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(c));
            concat->addEol();
        }
    }

    indent--;
    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputLambdaExpression(const AstNode* node)
{
    const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Closure capture parameters
    if (funcDecl->captureParameters)
    {
        CONCAT_FIXED_STR(concat, "closure");
        concat->addChar('|');

        bool first = true;
        for (const auto it : funcDecl->captureParameters->children)
        {
            const auto child = convertNode(it);
            if (!child)
                continue;

            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first = false;

            if (child->is(AstNodeKind::MakePointer))
            {
                concat->addChar('&');
                concat->addString(child->firstChild()->token.text);
            }
            else
            {
                concat->addString(child->token.text);
            }
        }

        concat->addChar('|');
    }
    else
    {
        CONCAT_FIXED_STR(concat, "func");
    }

    SWAG_CHECK(outputNode(funcDecl->parameters));
    SWAG_CHECK(outputFuncReturnType(funcDecl));

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        SWAG_ASSERT(funcDecl->content->is(AstNodeKind::Return));
        SWAG_CHECK(outputNode(funcDecl->content->firstChild()));
    }
    else if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=");
        concat->addBlank();
        SWAG_CHECK(outputNode(funcDecl->content->firstChild()));
    }
    else
    {
        SWAG_CHECK(outputNode(funcDecl->content));
    }

    return true;
}

bool FormatAst::outputFuncCallParams(const AstNode* node)
{
    const auto funcCallParams = castAst<AstFuncCallParams>(node, AstNodeKind::FuncCallParams);

    // Aliases
    if (!funcCallParams->aliasNames.empty())
    {
        concat->addChar('|');

        bool first = true;
        for (const auto& it : funcCallParams->aliasNames)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            concat->addString(it.text);
            first = false;
        }

        concat->addChar('|');
        concat->addBlank();
    }

    SWAG_CHECK(outputCommaChildren(node));
    return true;
}

bool FormatAst::outputTypeLambda(const AstNode* node)
{
    const auto typeNode = castAst<AstTypeLambda>(node);

    if (typeNode->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
    {
        CONCAT_FIXED_STR(concat, "#type");
        concat->addBlank();
    }

    if (node->is(AstNodeKind::TypeLambda))
        CONCAT_FIXED_STR(concat, "func");
    else
        CONCAT_FIXED_STR(concat, "closure");

    SWAG_CHECK(outputFuncDeclParameters(typeNode->parameters, false));

    if (typeNode->returnType)
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(typeNode->returnType));
    }

    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "throw");
    }
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "assume");
    }

    return true;
}

bool FormatAst::outputSpecCall(const Utf8& name, const AstNode* node)
{
    concat->addString(name);
    concat->addChar('(');

    SWAG_CHECK(outputNode(node->firstChild()));
    if (node->childCount() == 2)
    {
        concat->addChar(',');
        concat->addBlank();
        SWAG_CHECK(outputNode(node->lastChild()));
    }

    concat->addChar(')');
    return true;
}
