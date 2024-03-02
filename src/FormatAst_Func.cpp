#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Semantic.h"

bool FormatAst::outputFuncName(const AstFuncDecl* node) const
{
    concat->addString(node->token.text);
    return true;
}

bool FormatAst::outputFuncSignature(AstNode* node, const AstNode* genericParameters, const AstNode* parameters, const AstNode* validIf)
{
    if (node->is(AstNodeKind::AttrDecl))
        CONCAT_FIXED_STR(concat, "attr");
    else
        CONCAT_FIXED_STR(concat, "func");

    if (genericParameters)
    {
        concat->addChar('\'');
        SWAG_CHECK(outputNode(genericParameters));
    }

    concat->addBlank();

    if (node->is(AstNodeKind::FuncDecl) && node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
    {
        CONCAT_FIXED_STR(concat, "impl");
        concat->addBlank();
    }

    if (node->is(AstNodeKind::FuncDecl))
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        SWAG_CHECK(outputFuncName(funcNode));
    }
    else
        concat->addString(node->token.text);

    // Parameters
    if (parameters)
        SWAG_CHECK(outputNode(parameters));
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
            SWAG_CHECK(outputType(returnNode, typeFunc->returnType));
        }
        else if (funcNode->returnType && funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
        {
            CONCAT_FIXED_STR(concat, "->");
            SWAG_CHECK(outputNode(returnNode));
        }
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
        indent++;
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputCompilerExpr(validIf));
        indent--;
    }

    CONCAT_FIXED_STR(concat, ";");
    concat->addEol();
    return true;
}

bool FormatAst::outputFuncDeclReturnType(const AstNode* node)
{
    if (!node)
        return true;
    CONCAT_FIXED_STR(concat, "->");
    SWAG_CHECK(outputNode(node));
    return true;
}

bool FormatAst::outputFuncDeclParams(const AstNode* node)
{
    concat->addChar('(');
    outputCommaChildren(node);
    concat->addChar(')');
    return true;
}

bool FormatAst::outputFuncDecl(const AstFuncDecl* node)
{
    CONCAT_FIXED_STR(concat, "func");

    // Emit generic parameter, except if the function is an instance
    if (node->genericParameters)
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC) || node->hasAstFlag(AST_IS_GENERIC))
            SWAG_CHECK(outputGenericParameters(node->genericParameters));
    }

    // Implementation
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "impl");
    }

    // Name
    concat->addBlank();
    SWAG_CHECK(outputFuncName(node));

    // Parameters
    if (node->parameters)
        SWAG_CHECK(outputNode(node->parameters));
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
        SWAG_CHECK(outputType(returnNode, typeFunc->returnType));
    }
    else if (node->returnType && node->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(returnNode));
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

    // Content, short lambda
    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        SWAG_ASSERT(node->content->is(AstNodeKind::Return) || node->content->is(AstNodeKind::Try));
        SWAG_CHECK(outputNode(node->content->firstChild()));
        concat->addEol();
        return true;
    }

    // #validifx block
    if (node->validIf)
    {
        indent++;
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(node->validIf));
        indent--;
    }
    else if (node->content)
    {
        concat->addEol();
    }

    if (!node->content)
        return true;

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
        for (const auto p : funcDecl->captureParameters->children)
        {
            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first = false;

            if (p->is(AstNodeKind::MakePointer))
            {
                concat->addChar('&');
                concat->addString(p->firstChild()->token.text);
            }
            else
            {
                concat->addString(p->token.text);
            }
        }

        concat->addChar('|');
    }
    else
    {
        CONCAT_FIXED_STR(concat, "func");
    }

    // Parameters
    concat->addChar('(');
    if (funcDecl->parameters && !funcDecl->parameters->children.empty())
    {
        bool first = true;
        for (const auto p : funcDecl->parameters->children)
        {
            if (p->hasAstFlag(AST_GENERATED) && !p->hasAstFlag(AST_GENERATED_USER))
                continue;

            if (!first)
            {
                concat->addChar(',');
                concat->addBlank();
            }

            first            = false;
            const auto param = castAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
            if (param->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED))
                concat->addChar('?');
            else
            {
                concat->addString(p->token.text);
                if (!p->children.empty())
                {
                    concat->addChar(':');
                    concat->addBlank();
                    SWAG_CHECK(outputNode(p->firstChild()));
                }

                if (param->assignment)
                {
                    concat->addBlank();
                    concat->addChar('=');
                    concat->addBlank();
                    SWAG_CHECK(outputNode(param->assignment));
                }
            }
        }
    }

    concat->addChar(')');

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        SWAG_ASSERT(funcDecl->content->is(AstNodeKind::Return));
        SWAG_CHECK(outputNode(funcDecl->content->firstChild()));
    }
    else
    {
        concat->addEol();
        concat->addIndent(indent);
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

    if (!typeNode->parameters)
        CONCAT_FIXED_STR(concat, "()");
    else
        SWAG_CHECK(outputNode(typeNode->parameters));

    SWAG_CHECK(outputFuncDeclReturnType(typeNode->returnType));

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
