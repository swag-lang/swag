#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputChildrenVar(FormatContext& context, AstNode* node, uint32_t start, uint32_t& processed)
{
    processed = 0;
    if (!context.alignVarDecl)
        return true;

    VectorNative<AstNode*> nodes;

    CollectFlags flags = STOP_CMT_BEFORE;
    if (!node->findParentOrMe(AstNodeKind::StructContent))
        flags.add(STOP_EMPTY_LINE_BEFORE);

    {
        PushConcatFormatTmp fmt{this, context};
        if (!collectChildrenToAlign(context, flags, node, start, nodes, processed, [&](const AstNode* n) {
                if (n->kind != AstNodeKind::VarDecl && n->kind != AstNodeKind::ConstDecl)
                    return true;
                const auto var = castAst<AstVarDecl>(n);
                tmpConcat.clear();
                SWAG_CHECK(outputNode(fmt.cxt, var->type));
                if (tmpConcat.totalEol > 1)
                    return true;
                return false;
            }))
            return true;
    }

    uint32_t maxLenName = 0;
    uint32_t maxLenType = 0;

    {
        PushConcatFormatTmp fmt{this, context};
        for (const auto child : nodes)
        {
            const auto var = castAst<AstVarDecl>(child);

            uint32_t lenName = 0;
            uint32_t lenType = 0;

            tmpConcat.clear();
            SWAG_CHECK(outputVarName(context, var));
            lenName = tmpConcat.length();

            tmpConcat.clear();
            SWAG_CHECK(outputVarHeader(fmt.cxt, var));
            lenName += tmpConcat.length();

            if (var->type)
            {
                lenName += 1; // +1 because of ':'
                tmpConcat.clear();
                SWAG_CHECK(outputNode(fmt.cxt, var->type));
                lenType += tmpConcat.length();
            }

            if (var->assignment)
            {
                tmpConcat.clear();
                SWAG_CHECK(outputNode(fmt.cxt, var->assignment));
                lenType += tmpConcat.length();
                lenType += 2; // Because of '= '
                if (var->type)
                    lenType += 1; // Because of the blank after the type, and before '='
            }

            maxLenName = max(maxLenName, lenName);
            maxLenType = max(maxLenType, lenType);
        }
    }

    for (uint32_t i = 0; i < nodes.size(); i++)
    {
        auto child = nodes[i];

        concat->addIndent(context.indent);
        if (i != nodes.size() - 1 && nodes[i + 1]->hasSpecFlag(AstVarDecl::SPEC_FLAG_EXTRA_DECL))
        {
            FormatContext cxt{context};
            cxt.alignVarDecl = false;
            SWAG_CHECK(outputVar(cxt, child, maxLenName, maxLenType));
        }
        else
            SWAG_CHECK(outputVar(context, child, maxLenName, maxLenType));

        while (i != nodes.size() - 1 && nodes[i + 1]->hasSpecFlag(AstVarDecl::SPEC_FLAG_EXTRA_DECL))
        {
            i++;
            child = nodes[i];

            FormatContext cxt{context};
            cxt.alignVarDecl = false;
            concat->addChar(',');
            concat->addBlank();
            const auto varNode = castAst<AstVarDecl>(child, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
            SWAG_CHECK(outputVarContent(cxt, varNode, 0, maxLenName, maxLenType));
        }

        addEOLOrSemi(context, child);
    }

    return true;
}

bool FormatAst::outputVarName(FormatContext&, const AstVarDecl* varNode) const
{
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

    return true;
}

bool FormatAst::outputVarContent(FormatContext& context, AstNode* node, uint32_t startColumn, uint32_t maxLenName, uint32_t maxLenType)
{
    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
    const bool isSelf  = varNode->token.is(g_LangSpec->name_self);
    inheritLastFormatAfter(nullptr, varNode);

    const uint32_t alignTypeBanks = node->hasAstFlag(AST_STRUCT_MEMBER) ? context.alignStructVarTypeAddBlanks : 0;

    SWAG_CHECK(outputVarName(context, varNode));

    const auto totalEol = concat->totalEol;
    if (varNode->type)
    {
        if (!varNode->type->hasAstFlag(AST_GENERATED) || varNode->type->hasAstFlag(AST_GENERATED_USER))
        {
            if (!isSelf)
            {
                if (!varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_AUTO_NAME | AstVarDecl::SPEC_FLAG_PRIVATE_NAME))
                {
                    concat->addChar(':');
                    if (context.alignVarDecl)
                        concat->alignToColumn(startColumn + maxLenName + alignTypeBanks);
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
        if (context.alignVarDecl)
            concat->alignToColumn(startColumn + maxLenName + alignTypeBanks);
        concat->addBlank();
        concat->addChar('=');
        addBlank(varNode->assignment);
        SWAG_CHECK(outputNode(context, varNode->assignment));
    }

    if (maxLenType)
    {
        maxLenType += 1;
        maxLenType += alignTypeBanks;
    }

    // Align comment only if the type didn't output some eol
    if (concat->totalEol == totalEol && context.alignVarDecl && maxLenName && maxLenType)
        concat->alignToColumn(startColumn + maxLenName + maxLenType + context.addBlanksBeforeAlignedLastLineComments);

    beautifyAfter(context, varNode);
    return true;
}

bool FormatAst::outputVarHeader(FormatContext& context, AstNode* node)
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
        concat->addString("using");
        concat->addBlank();
    }

    if (varNode->is(AstNodeKind::ConstDecl))
    {
        concat->addString("const");
        concat->addBlank();
    }
    else if (varNode->isNot(AstNodeKind::FuncDeclParam) && !varNode->hasAstFlag(AST_STRUCT_MEMBER))
    {
        if (varNode->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET))
            concat->addString("let");
        else
            concat->addString("var");
        concat->addBlank();
    }

    const bool isSelf = varNode->token.is(g_LangSpec->name_self);
    if (isSelf && varNode->type && castAst<AstTypeExpression>(varNode->type)->typeFlags.has(TYPEFLAG_IS_CONST))
    {
        concat->addString("const");
        concat->addBlank();
    }

    return true;
}

bool FormatAst::outputVar(FormatContext& context, AstNode* node, uint32_t maxLenName, uint32_t maxLenType)
{
    beautifyBefore(context, node);

    const auto startColumn = concat->column;
    SWAG_CHECK(outputVarHeader(context, node));

    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
    SWAG_CHECK(outputVarContent(context, varNode, startColumn, maxLenName, maxLenType));
    return true;
}
