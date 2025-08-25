#include "pch.h"
#include "Format/FormatAst.h"
#include "Main/CommandLine.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool FormatAst::outputIdentifier(const FormatContext& context, AstNode* node)
{
    const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);

    if (identifier->identifierExtension && identifier->identifierExtension->scopeUpMode != IdentifierScopeUpMode::None)
    {
        concat->addString("#up");
        if (identifier->identifierExtension->scopeUpMode == IdentifierScopeUpMode::Count &&
            identifier->identifierExtension->scopeUpValue.literalValue.u8 > 1)
        {
            concat->addChar('(');
            concat->addStringFormat("%d", identifier->identifierExtension->scopeUpValue.literalValue.u8);
            concat->addChar(')');
        }

        concat->addBlank();
    }

    concat->addString(node->token.text);
    if (identifier->genericParameters)
    {
        concat->addChar('\'');
        if (identifier->genericParameters->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar('(');

        {
            FormatContext cxt{context};
            cxt.beautifyAfter = false;
            SWAG_CHECK(outputNode(cxt, identifier->genericParameters));
        }

        if (identifier->genericParameters->hasAstFlag(AST_EXPR_IN_PARENTS))
            concat->addChar(')');

        beautifyAfter(context, identifier->genericParameters);
    }

    if (identifier->callParameters && !g_LangSpec->intrinsicConstants.contains(identifier->token.text))
    {
        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT_WAS_ASSIGN))
        {
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
        }

        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
            concat->addChar('{');
        else
            concat->addChar('(');

        {
            FormatContext cxt{context};
            cxt.beautifyAfter = false;
            SWAG_CHECK(outputNode(cxt, identifier->callParameters));
        }

        if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        {
            concat->addChar('}');
        }
        else if (identifier->callParameters->children.empty() ||
                 identifier->callParameters->lastChild()->children.empty() ||
                 identifier->callParameters->lastChild()->lastChild()->isNot(AstNodeKind::CompilerCode) ||
                 !identifier->callParameters->lastChild()->lastChild()->hasSpecFlag(AstCompilerCode::SPEC_FLAG_FROM_NEXT))
        {
            concat->addChar(')');
        }

        beautifyAfter(context, identifier->callParameters);
    }

    return true;
}

#pragma optimize("", off)
bool FormatAst::outputIdentifierRef(FormatContext& context, AstNode* node)
{
    if (node->hasAstFlag(AST_DISCARD))
    {
        concat->addString("discard");
        concat->addBlank();
    }

    if (node->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE | AstIdentifierRef::SPEC_FLAG_AUTO_WITH_SCOPE))
    {
        concat->addChar('.');
    }

    bool first = true;
    int  idx   = 0;
    for (const auto it : node->children)
    {
        if (g_CommandLine.patchMode)
        {
            if (!node->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE | AstIdentifierRef::SPEC_FLAG_AUTO_WITH_SCOPE))
            {
                idx++;
                if (it->token.text == "me" && it->hasAstFlag(AST_GENERATED))
                {
                    if (idx != node->children.size() &&
                        (!node->children[idx]->resolvedSymbolName() || !node->children[idx]->resolvedSymbolName()->is(SymbolKind::GenericType)))
                    {
                        concat->addString("me.");
                    }
                }
                else if (idx == 1 &&
                         it->resolvedSymbolName() &&
                         it->resolvedSymbolName()->is(SymbolKind::Function) &&
                         it->is(AstNodeKind::Identifier))
                {
                    const auto id = castAst<AstIdentifier>(it, AstNodeKind::Identifier);
                    if (id->callParameters &&
                        !id->callParameters->children.empty() &&
                        id->callParameters->firstChild()->hasAstFlag(AST_GENERATED) &&
                        id->callParameters->firstChild()->token.text == "me")
                    {
                        concat->addString("me.");
                    }
                }
            }
        }

        const auto child = convertNode(context, it);
        if (!child)
            continue;

        if (child->hasSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL))
        {
            SWAG_CHECK(outputNode(context, child));
            continue;
        }

        if (!first)
            concat->addChar('.');
        first = false;

        SWAG_CHECK(outputNode(context, child));
    }

    return true;
}
