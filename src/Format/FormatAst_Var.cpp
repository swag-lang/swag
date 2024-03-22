#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputChildrenVar(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    VectorNative<AstNode*> nodes;
    for (uint32_t i = start; i < node->childCount(); i++)
    {
        const auto it    = node->children[i];
        const auto child = convertNode(context, it);
        if (!child)
            continue;
        if (child->kind != AstNodeKind::VarDecl && child->kind != AstNodeKind::ConstDecl)
            break;
        nodes.push_back(child);
    }

    processed = nodes.size();
    if (!processed)
        return true;

    uint32_t maxLenName   = 0;
    uint32_t maxLenType   = 0;
    uint32_t maxLenAssign = 0;

    {
        PushFormatTmp fmt{this};
        FormatContext cxt{context};
        cxt.outputComments   = false;
        cxt.outputBlankLines = false;

        for (const auto child : nodes)
        {
            maxLenName = max(maxLenName, child->token.text.length());

            const auto var = castAst<AstVarDecl>(child);
            if (var->type)
            {
                maxLenName = max(maxLenName, child->token.text.length() + 1); // +1 because of ':'
                tmpConcat.clear();
                SWAG_CHECK(outputNode(cxt, var->type));
                maxLenType = max(maxLenType, tmpConcat.length());
            }

            if (var->assignment)
            {
                tmpConcat.clear();
                SWAG_CHECK(outputNode(cxt, var->assignment));
                if (!var->type)
                    maxLenType = max(maxLenType, tmpConcat.length() + 2);
                else
                    maxLenAssign = max(maxLenAssign, tmpConcat.length());
            }
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputVar(context, child, maxLenName, maxLenType, maxLenAssign));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputVar(FormatContext& context, AstNode* node, bool isSelf, uint32_t startColumn, uint32_t maxLenName, uint32_t maxLenType, uint32_t maxLenAssign)
{
    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);

    AstNode* scan = varNode;
    while (scan)
    {
        const auto to = scan->getTokenParse();
        if (to && !to->comments.commentJustAfter.empty())
        {
            node->inheritFormatFromAfter(nullptr, scan);
            break;
        }

        scan = scan->lastChild();
    }

    if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME | AstVarDecl::SPEC_FLAG_PRIVATE_NAME))
    {
        if (!varNode->multiNames.empty())
        {
            if (varNode->hasAstFlag(AST_EXPR_IN_PARENTS))
                concat->addChar('(');

            bool first = true;
            for (const auto& name : varNode->multiNames)
            {
                if (!first)
                {
                    concat->addChar(',');
                    concat->addBlank();
                }
                first = false;
                concat->addString(name);
            }

            if (varNode->hasAstFlag(AST_EXPR_IN_PARENTS))
                concat->addChar(')');
        }
        else if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_UNNAMED))
            concat->addChar('?');
        else
            concat->addString(varNode->token.text);
    }

    if (varNode->type)
    {
        if (!varNode->type->hasAstFlag(AST_GENERATED) || varNode->type->hasAstFlag(AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME | AstVarDecl::SPEC_FLAG_PRIVATE_NAME))
                {
                    concat->addChar(':');
                    concat->alignToColumn(startColumn + maxLenName);
                    concat->addBlank();
                }

                SWAG_CHECK(outputNode(context, varNode->type));
            }
        }
        else
        {
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            const auto typeExpr = castAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
            SWAG_ASSERT(!varNode->assignment);
            SWAG_ASSERT(typeExpr->identifier);
            SWAG_ASSERT(varNode->type->typeInfo && varNode->type->typeInfo->isTuple());
            const auto id = castAst<AstIdentifier>(typeExpr->identifier->lastChild(), AstNodeKind::Identifier);
            concat->addChar('{');
            SWAG_CHECK(outputNode(context, id->callParameters));
            concat->addChar('}');
        }
    }

    if (varNode->assignment)
    {
        concat->alignToColumn(startColumn + maxLenName);
        concat->addBlank();
        concat->addChar('=');
        if (!varNode->assignment->is(AstNodeKind::Move) && !varNode->assignment->is(AstNodeKind::NoDrop))
            concat->addBlank();
        SWAG_CHECK(outputNode(context, varNode->assignment));
    }

    if (maxLenType)
        maxLenType += 1;
    if (maxLenAssign)
        maxLenAssign += 3;

    concat->alignToColumn(startColumn + maxLenName + maxLenType + maxLenAssign);
    beautifyAfter(context, varNode);

    return true;
}

bool FormatAst::outputVar(FormatContext& context, AstNode* node, uint32_t maxLenName, uint32_t maxLenType, uint32_t maxLenAssign)
{
    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);

    if (varNode->attrUse)
    {
        bool hasSomething = true;
        SWAG_CHECK(outputAttrUse(context, varNode->attrUse, hasSomething));
        concat->addBlank();
    }

    if (varNode->hasAstFlag(AST_DECL_USING))
    {
        CONCAT_FIXED_STR(concat, "using");
        concat->addBlank();
    }

    if (varNode->is(AstNodeKind::ConstDecl))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }
    else if (varNode->isNot(AstNodeKind::FuncDeclParam) && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
            CONCAT_FIXED_STR(concat, "let");
        else
            CONCAT_FIXED_STR(concat, "var");
        concat->addBlank();
    }

    const bool isSelf = varNode->token.text == g_LangSpec->name_self;
    if (isSelf && varNode->type && castAst<AstTypeExpression>(varNode->type)->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        CONCAT_FIXED_STR(concat, "const");
        concat->addBlank();
    }

    const auto startColumn = concat->column;
    SWAG_CHECK(outputVar(context, varNode, isSelf, startColumn, maxLenName, maxLenType, maxLenAssign));
    return true;
}
