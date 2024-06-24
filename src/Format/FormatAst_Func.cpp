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
        concat->addString("()");
        return true;
    }

    concat->addChar('(');
    SWAG_CHECK(outputChildrenChar(context, parameters, ',', 0, isMethod ? 1 : 0));
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
        concat->addString("->");
        SWAG_CHECK(outputNode(context, returnNode));
    }

    return true;
}

bool FormatAst::outputFuncSignature(FormatContext& context, AstNode* node, AstNode* genericParameters, AstNode* parameters, const AstNode* validIf)
{
    bool isMethod = false;

    if (node->is(AstNodeKind::AttrDecl))
    {
        concat->addString("attr");
    }
    else if (node->is(AstNodeKind::FuncDecl) && node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_METHOD))
    {
        isMethod = true;
        concat->addString("mtd");
    }
    else
        concat->addString("func");

    if (genericParameters)
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC) || node->hasAstFlag(AST_GENERIC))
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
                concat->addString("const");
                concat->addBlank();
            }
        }

        if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
        {
            concat->addString("impl");
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
        concat->addString("throw");
    }
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
    {
        concat->addBlank();
        concat->addString("assume");
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
        concat->addString("#ast");
    else if (funcDecl->hasAttribute(ATTRIBUTE_RUN_FUNC))
        concat->addString("#run");
    else if (funcDecl->hasAttribute(ATTRIBUTE_TEST_FUNC))
        concat->addString("#test");
    else if (funcDecl->hasAttribute(ATTRIBUTE_MAIN_FUNC))
        concat->addString("#main");
    else if (funcDecl->hasAttribute(ATTRIBUTE_INIT_FUNC))
        concat->addString("#init");
    else if (funcDecl->hasAttribute(ATTRIBUTE_DROP_FUNC))
        concat->addString("#drop");
    else if (funcDecl->hasAttribute(ATTRIBUTE_PREMAIN_FUNC))
        concat->addString("#premain");
    else if (funcDecl->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
    {
        concat->addString("#message");
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
        concat->addString("=>");
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
        concat->addChar('=');
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

    // Content on same line
    if (context.keepSameLineFuncBody &&
        funcDecl->subDecl.empty() &&
        funcDecl->content &&
        (funcDecl->content->is(AstNodeKind::Statement) || funcDecl->content->is(AstNodeKind::Try) || funcDecl->content->is(AstNodeKind::Assume)) &&
        !funcDecl->hasAttribute(ATTRIBUTE_SHARP_FUNC) &&
        !hasEOLInside(funcDecl->content))
    {
        FormatContext cxt{context};
        cxt.canConcatStatement = true;
        concat->noEol++;
        concat->addBlank();
        concat->addChar('{');

        auto inside = funcDecl->content;
        if (funcDecl->content->is(AstNodeKind::Try) || funcDecl->content->is(AstNodeKind::Assume))
            inside = inside->firstChild();

        if (inside->childCount())
            concat->addBlank();
        SWAG_CHECK(outputChildrenChar(cxt, inside, ';', ';', 0));
        if (inside->childCount())
            concat->addBlank();
        
        concat->addChar('}');
        concat->noEol--;
        concat->addEol();
    }

    // Content, normal function
    else
    {
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
    }

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
        concat->addString("closure");
        SWAG_CHECK(outputClosureArguments(context, funcDecl));
    }
    else
    {
        concat->addString("func");
    }

    SWAG_CHECK(outputFuncDeclParameters(context, funcDecl->parameters, false));
    SWAG_CHECK(outputFuncReturnType(context, funcDecl));

    if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
    {
        concat->addBlank();
        concat->addString("=>");
        concat->addBlank();
        SWAG_ASSERT(funcDecl->content->is(AstNodeKind::Return));
        SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
    }
    else if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_FORM))
    {
        concat->addBlank();
        concat->addString("=");
        concat->addBlank();
        SWAG_CHECK(outputNode(context, funcDecl->content->firstChild()));
    }
    else
    {
        FormatContext cxt{context};
        cxt.canConcatStatement = context.keepSameLineFuncBody;
        concat->addBlank();
        SWAG_CHECK(outputNode(cxt, funcDecl->content));
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
        if (!node->children.empty())
            concat->addBlank();
    }

    SWAG_CHECK(outputChildrenChar(context, node, ',', 0, 0));
    concat->removeLastChar('\n');
    return true;
}

bool FormatAst::outputTypeLambda(FormatContext& context, AstNode* node)
{
    const auto typeNode = castAst<AstTypeLambda>(node);

    if (typeNode->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
    {
        concat->addString("#type");
        concat->addBlank();
    }

    if (node->is(AstNodeKind::TypeLambda))
        concat->addString("func");
    else
        concat->addString("closure");

    FormatContext cxt{context};
    cxt.alignVarDecl = false;
    SWAG_CHECK(outputFuncDeclParameters(cxt, typeNode->parameters, false));

    if (typeNode->returnType)
    {
        concat->addString("->");
        SWAG_CHECK(outputNode(context, typeNode->returnType));
    }

    if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
    {
        concat->addBlank();
        concat->addString("throw");
    }
    else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
    {
        concat->addBlank();
        concat->addString("assume");
    }

    return true;
}

bool FormatAst::outputInit(FormatContext& context, AstNode* node)
{
    const auto init = castAst<AstInit>(node, AstNodeKind::Init);
    concat->addString("@init");

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
            concat->addString("@drop");
            break;
        case AstNodeKind::PostCopy:
            concat->addString("@postcopy");
            break;
        case AstNodeKind::PostMove:
            concat->addString("@postmove");
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
