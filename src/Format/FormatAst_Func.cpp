#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputFuncDeclParameters(FormatContext& context, AstNode* parameters, bool isMethod)
{
    if (!parameters)
    {
        CONCAT_FIXED_STR(concat, "()");
        return true;
    }

    concat->addChar('(');
    SWAG_CHECK(outputChildrenComma(context, parameters, isMethod ? 1 : 0));
    concat->addChar(')');
    beautifyAfter(context, parameters);

    return true;
}

bool FormatAst::outputFuncReturnType(FormatContext& context, const AstFuncDecl* funcNode)
{
    auto returnNode = funcNode->returnType;
    if (returnNode && !returnNode->children.empty())
        returnNode = returnNode->firstChild();

    if (funcNode->returnType && funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(context, returnNode));
    }

    return true;
}

bool FormatAst::outputFuncSignature(FormatContext& context, AstNode* node, AstNode* genericParameters, AstNode* parameters, const AstNode* validIf)
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
            SWAG_CHECK(outputGenericParameters(context, genericParameters));
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
    SWAG_CHECK(outputFuncDeclParameters(context, parameters, isMethod));

    // Return type, in case of function (not attr)
    if (node->is(AstNodeKind::FuncDecl))
    {
        const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        SWAG_CHECK(outputFuncReturnType(context, funcNode));
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
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputCompilerExpr(context, validIf));
        context.indent--;
    }

    return true;
}

bool FormatAst::outputFuncDecl(FormatContext& context, AstNode* node, uint32_t maxLenSignature)
{
    beautifyBefore(context, node);

    const auto funcDecl    = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
    const auto startColumn = concat->column;

    if (!funcDecl->content)
        inheritLastFormatAfter(nullptr, funcDecl);

    if (funcDecl->hasAttribute(ATTRIBUTE_AST_FUNC))
        CONCAT_FIXED_STR(concat, "#ast");
    else if (funcDecl->hasAttribute(ATTRIBUTE_RUN_FUNC))
        CONCAT_FIXED_STR(concat, "#run");
    else if (funcDecl->hasAttribute(ATTRIBUTE_TEST_FUNC))
        CONCAT_FIXED_STR(concat, "#test");
    else if (funcDecl->hasAttribute(ATTRIBUTE_MAIN_FUNC))
        CONCAT_FIXED_STR(concat, "#main");
    else if (funcDecl->hasAttribute(ATTRIBUTE_INIT_FUNC))
        CONCAT_FIXED_STR(concat, "#init");
    else if (funcDecl->hasAttribute(ATTRIBUTE_DROP_FUNC))
        CONCAT_FIXED_STR(concat, "#drop");
    else if (funcDecl->hasAttribute(ATTRIBUTE_PREMAIN_FUNC))
        CONCAT_FIXED_STR(concat, "#premain");
    else if (funcDecl->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
    {
        CONCAT_FIXED_STR(concat, "#message");
        concat->addChar('(');
        SWAG_CHECK(outputNode(context, funcDecl->parameters));
        concat->addChar(')');
    }
    else
        SWAG_CHECK(outputFuncSignature(context, funcDecl, funcDecl->genericParameters, funcDecl->parameters, nullptr));

    // Content, short lambda
    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->alignToColumn(startColumn + maxLenSignature);
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        if (funcDecl->content->is(AstNodeKind::Return))
            SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
        else if (funcDecl->content->is(AstNodeKind::Try))
            SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()->firstChild()));
        else
            SWAG_ASSERT(false);
        concat->addEol();
        return true;
    }

    // Content, short form
    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
    {
        concat->alignToColumn(startColumn + maxLenSignature);
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
        concat->addEol();
        return true;
    }

    // #validifx block
    if (funcDecl->validIf)
    {
        concat->addEol();
        context.indent++;
        concat->addIndent(context.indent);
        SWAG_CHECK(outputCompilerExpr(context, funcDecl->validIf));
        context.indent--;
    }

    if (!funcDecl->content)
    {
        concat->addChar(';');
        beautifyAfter(context, funcDecl);
        concat->addEol();
        return true;
    }

    // Content, normal function
    concat->addEol();
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();
    context.indent++;

    if (funcDecl->content->isNot(AstNodeKind::Statement))
    {
        context.indent--;
        SWAG_CHECK(outputNode(context, funcDecl->content));
        context.indent++;
    }
    else
    {
        for (auto c : funcDecl->subDecl)
        {
            concat->addIndent(context.indent);
            if (c->parent && c->parent->is(AstNodeKind::AttrUse))
                c = c->parent;
            SWAG_CHECK(outputNode(context, c));
            concat->addEol();
        }

        SWAG_CHECK(outputChildrenEol(context, funcDecl->content));
    }

    context.indent--;
    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputClosureArguments(FormatContext& context, const AstFuncDecl* funcNode)
{
    concat->addChar('|');

    bool first = true;
    for (const auto it : funcNode->captureParameters->children)
    {
        auto child = convertNode(context, it);
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
            if (child->hasSpecFlag(AstMakePointer::SPEC_FLAG_TO_REF))
            {
                CONCAT_FIXED_STR(concat, "ref");
                concat->addBlank();
            }
            else
                concat->addChar('&');
            child = child->firstChild();
        }

        SWAG_CHECK(outputNode(context, child));
    }

    concat->addChar('|');
    return true;
}

bool FormatAst::outputLambdaExpression(FormatContext& context, AstNode* node)
{
    const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

    // Closure capture parameters
    if (funcDecl->captureParameters)
    {
        CONCAT_FIXED_STR(concat, "closure");
        SWAG_CHECK(outputClosureArguments(context, funcDecl));
    }
    else
    {
        CONCAT_FIXED_STR(concat, "func");
    }

    SWAG_CHECK(outputFuncDeclParameters(context, funcDecl->parameters, false));
    SWAG_CHECK(outputFuncReturnType(context, funcDecl));

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=>");
        concat->addBlank();
        SWAG_ASSERT(funcDecl->content->is(AstNodeKind::Return));
        SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
    }
    else if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "=");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
    }
    else
    {
        SWAG_CHECK(outputNode(context, funcDecl->content));
    }

    return true;
}

bool FormatAst::outputFuncCallParams(FormatContext& context, AstNode* node)
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

    SWAG_CHECK(outputChildrenComma(context, node));
    return true;
}

bool FormatAst::outputTypeLambda(FormatContext& context, AstNode* node)
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

    FormatContext cxt{context};
    cxt.alignVarDecl = false;
    SWAG_CHECK(outputFuncDeclParameters(cxt, typeNode->parameters, false));

    if (typeNode->returnType)
    {
        CONCAT_FIXED_STR(concat, "->");
        SWAG_CHECK(outputNode(context, typeNode->returnType));
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

bool FormatAst::outputInit(FormatContext& context, AstNode* node)
{
    const auto init = castAst<AstInit>(node, AstNodeKind::Init);
    CONCAT_FIXED_STR(concat, "@init");

    concat->addChar('(');
    SWAG_CHECK(outputNode(context, init->expression));
    if (init->count)
    {
        concat->addChar(',');
        concat->addBlank();
        SWAG_CHECK(outputNode(context, init->count));
    }
    concat->addChar(')');

    if (init->parameters)
    {
        concat->addChar('(');
        SWAG_CHECK(outputNode(context, init->parameters));
        concat->addChar(')');
        beautifyAfter(context, init->parameters);
    }

    return true;
}

bool FormatAst::outputDropCopyMove(FormatContext& context, AstNode* node)
{
    const auto drop = castAst<AstInit>(node, AstNodeKind::Drop, AstNodeKind::PostCopy, AstNodeKind::PostMove);
    switch (drop->kind)
    {
        case AstNodeKind::Drop:
            CONCAT_FIXED_STR(concat, "@drop");
            break;
        case AstNodeKind::PostCopy:
            CONCAT_FIXED_STR(concat, "@postcopy");
            break;
        case AstNodeKind::PostMove:
            CONCAT_FIXED_STR(concat, "@postmove");
            break;
    }

    concat->addChar('(');
    SWAG_CHECK(outputNode(context, drop->expression));
    if (drop->count)
    {
        concat->addChar(',');
        concat->addBlank();
        SWAG_CHECK(outputNode(context, drop->count));
    }
    concat->addChar(')');

    return true;
}

bool FormatAst::outputChildrenFuncDecl(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.alignShortFunc)
        return true;

    VectorNative<AstNode*> nodes;
    if (!collectChildrenToAlign(context, STOP_CMT_BEFORE | STOP_EMPTY_LINE_BEFORE, node, start, nodes, processed, [](const AstNode* node) {
            if (node->kind != AstNodeKind::FuncDecl)
                return true;
            if (!node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM | AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
                return true;
            return false;
        }))
        return true;

    uint32_t maxLenSignature = 0;

    {
        PushConcatFormatTmp fmt{this, context};
        for (const auto child : nodes)
        {
            const auto funcDecl = castAst<AstFuncDecl>(child, AstNodeKind::FuncDecl);
            tmpConcat.clear();
            SWAG_CHECK(outputFuncSignature(fmt.cxt, funcDecl, funcDecl->genericParameters, funcDecl->parameters, nullptr));
            maxLenSignature = max(maxLenSignature, tmpConcat.length());
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputFuncDecl(context, child, maxLenSignature));
        concat->addEol();
    }

    return true;
}
