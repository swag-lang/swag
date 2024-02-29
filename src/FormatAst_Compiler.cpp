#include "pch.h"
#include "FormatAst.h"
#include "Ast.h"
#include "Semantic.h"

bool FormatAst::outputCompilerIf(const Utf8& name, const AstNode* node)
{
    const auto compilerIf = castAst<AstIf>(node, AstNodeKind::CompilerIf);
    concat->addString(name);
    concat->addBlank();
    SWAG_CHECK(outputNode(compilerIf->boolExpression));

    if (compilerIf->ifBlock->firstChild()->isNot(AstNodeKind::Statement))
    {
        concat->addBlank();
        CONCAT_FIXED_STR(concat, "#do");
        concat->addEol();
        indent++;
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(compilerIf->ifBlock->firstChild()));
        indent--;
        concat->addEol();
    }
    else
    {
        concat->addEol();
        concat->addIndent(indent);
        SWAG_CHECK(outputNode(compilerIf->ifBlock->firstChild()));
    }

    if (!compilerIf->elseBlock)
        return true;

    if (compilerIf->elseBlock->firstChild()->is(AstNodeKind::CompilerIf))
    {
        concat->addIndent(indent);
        SWAG_CHECK(outputCompilerIf("#elif", compilerIf->elseBlock->firstChild()));
    }
    else
    {
        concat->addIndent(indent);
        CONCAT_FIXED_STR(concat, "#else");
        if (compilerIf->elseBlock->firstChild()->isNot(AstNodeKind::Statement))
        {
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "#do");
            concat->addEol();
            indent++;
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(compilerIf->elseBlock->firstChild()));
            indent--;
            concat->addEol();
        }
        else
        {
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(compilerIf->elseBlock->firstChild()));
        }
    }

    return true;
}
