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
    if (!collectChildrenToAlign(context, STOP_CMT_BEFORE | STOP_EMPTY_LINE_BEFORE, node, start, nodes, processed, [](const AstNode* node) {
            if (node->kind != AstNodeKind::VarDecl && node->kind != AstNodeKind::ConstDecl)
                return true;
            return false;
        }))
        return true;

    uint32_t maxLenName = 0;
    uint32_t maxLenType = 0;

    {
        PushConcatFormatTmp fmt{this};
        FormatContext       cxt{context};
        cxt.outputComments   = false;
        cxt.outputBlankLines = false;

        for (const auto child : nodes)
        {
            const auto var = castAst<AstVarDecl>(child);

            uint32_t lenName = 0;
            uint32_t lenType = 0;

            lenName = child->token.text.length();

            tmpConcat.clear();
            SWAG_CHECK(outputVarHeader(cxt, var));
            lenName += tmpConcat.length();

            if (var->type)
            {
                lenName += 1; // +1 because of ':'
                tmpConcat.clear();
                SWAG_CHECK(outputNode(cxt, var->type));
                lenType += tmpConcat.length();
            }

            if (var->assignment)
            {
                tmpConcat.clear();
                SWAG_CHECK(outputNode(cxt, var->assignment));
                lenType += tmpConcat.length();
                lenType += 2; // Because of '= '
                if (var->type)
                    lenType += 1; // Because of the blank after the type, and before '='
            }

            maxLenName = max(maxLenName, lenName);
            maxLenType = max(maxLenType, lenType);
        }
    }

    for (const auto child : nodes)
    {
        concat->addIndent(context.indent);
        SWAG_CHECK(outputVar(context, child, maxLenName, maxLenType));
        concat->addEol();
    }

    return true;
}

bool FormatAst::outputVar(FormatContext& context, AstNode* node, bool isSelf, uint32_t startColumn, uint32_t maxLenName, uint32_t maxLenType)
{
    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
    inheritLastFormatAfter(nullptr, varNode);

    const uint32_t alignTypeBanks = node->hasAstFlag(AST_STRUCT_MEMBER) ? context.alignStructVarTypeAddBlanks : 0;

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
        concat->alignToColumn(startColumn + maxLenName + alignTypeBanks);
        concat->addBlank();
        concat->addChar('=');
        if (!varNode->assignment->is(AstNodeKind::Move) && !varNode->assignment->is(AstNodeKind::NoDrop))
            concat->addBlank();
        SWAG_CHECK(outputNode(context, varNode->assignment));
    }

    if (maxLenType)
    {
        maxLenType += 1;
        maxLenType += alignTypeBanks;
    }

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

    return true;
}

bool FormatAst::outputVar(FormatContext& context, AstNode* node, uint32_t maxLenName, uint32_t maxLenType)
{
    beautifyBefore(context, node);

    const auto startColumn = concat->column;
    SWAG_CHECK(outputVarHeader(context, node));

    const auto varNode = castAst<AstVarDecl>(node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl, AstNodeKind::FuncDeclParam);
    const bool isSelf  = varNode->token.text == g_LangSpec->name_self;
    SWAG_CHECK(outputVar(context, varNode, isSelf, startColumn, maxLenName, maxLenType));
    return true;
}
