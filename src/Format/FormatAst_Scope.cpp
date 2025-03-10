#include "pch.h"
#include "Format/FormatAst.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool FormatAst::outputScopeContent(FormatContext& context, Module* module, const Scope* scope)
{
    const auto publicSet = scope->publicSet;
    if (!publicSet)
        return true;

    // Stuff
    if (!publicSet->publicNodes.empty())
    {
        for (const auto one : publicSet->publicNodes)
        {
            concat->addBlankLine();
            SWAG_CHECK(outputAttributes(context, one, one->typeInfo));
            concat->addIndent(context.indent);
            SWAG_CHECK(outputNode(context, one));
            concat->addEol();
            concat->addBlankLine();
        }
    }

    // Exported functions (signatures)
    if (!publicSet->publicFunc.empty())
    {
        for (const auto one : publicSet->publicFunc)
        {
            concat->addBlankLine();

            // Remap special functions to their generated equivalent
            AstFuncDecl* funcNode = castAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            funcNode->computeFullNameForeignExport();
            concat->addIndent(context.indent);
            concat->addStringFormat(R"(#[Foreign("%s", "%s")])", module->name.cstr(), funcNode->fullnameForeignExport.cstr());
            concat->addEol();
            SWAG_CHECK(outputAttributes(context, funcNode, funcNode->typeInfo));

            concat->addIndent(context.indent);
            if (funcNode->token.is(g_LangSpec->name_opInitGenerated))
            {
                concat->addString("mtd opInit();");
                concat->addEol();
            }
            else if (funcNode->token.is(g_LangSpec->name_opDropGenerated))
            {
                concat->addString("mtd opDrop();");
                concat->addEol();
            }
            else if (funcNode->token.is(g_LangSpec->name_opPostCopyGenerated))
            {
                concat->addString("mtd opPostCopy();");
                concat->addEol();
            }
            else if (funcNode->token.is(g_LangSpec->name_opPostMoveGenerated))
            {
                concat->addString("mtd opPostMove();");
                concat->addEol();
            }
            else
            {
                SWAG_CHECK(outputFuncSignature(context, funcNode, nullptr, funcNode->parameters, &funcNode->constraints));
                concat->removeLastChar('\n');
                concat->addChar(';');
                concat->addEol();
            }

            concat->addBlankLine();
        }
    }

    // Attributes
    if (!publicSet->publicAttr.empty())
    {
        for (const auto one : publicSet->publicAttr)
        {
            concat->addBlankLine();
            const auto              node     = castAst<AstAttrDecl>(one, AstNodeKind::AttrDecl);
            const TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            SWAG_CHECK(outputAttributesUsage(context, typeFunc));
            concat->addIndent(context.indent);
            SWAG_CHECK(outputFuncSignature(context, node, nullptr, node->parameters, nullptr));
            concat->addEol();
            concat->addBlankLine();
        }
    }

    // Impl
    if (!publicSet->publicImpl.empty())
    {
        for (const auto one : publicSet->publicImpl)
        {
            outputScope(context, module, one);
        }
    }

    return true;
}

bool FormatAst::outputScopeContentAndChildren(FormatContext& context, Module* module, const Scope* scope)
{
    SWAG_CHECK(outputScopeContent(context, module, scope));
    for (const auto oneScope : scope->childrenScopes)
        SWAG_CHECK(outputScope(context, module, oneScope));
    return true;
}

bool FormatAst::outputScopeBlock(FormatContext& context, Module* module, const Scope* scope)
{
    concat->addIndent(context.indent);
    concat->addChar('{');
    concat->addEol();
    context.indent++;
    SWAG_CHECK(outputScopeContentAndChildren(context, module, scope));
    context.indent--;
    concat->addIndent(context.indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputScope(FormatContext& context, Module* module, Scope* scope)
{
    SWAG_ASSERT(module);
    if (!scope->flags.has(SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags.has(SCOPE_IMPORTED))
        return true;

    // Namespace
    if (scope->is(ScopeKind::Namespace) && !scope->name.empty())
    {
        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
        {
            concat->addBlankLine();
            concat->addIndent(context.indent);
            concat->addString("namespace");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
            outputScopeBlock(context, module, scope);
            concat->addBlankLine();
        }
        else
        {
            SWAG_CHECK(outputScopeContentAndChildren(context, module, scope));
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        concat->addBlankLine();
        concat->addIndent(context.indent);
        if (scope->is(ScopeKind::Impl))
        {
            const auto nodeImpl = castAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            const auto symbol   = nodeImpl->identifier->resolvedSymbolOverload();
            concat->addString("impl");
            concat->addBlank();
            concat->addString(symbol->node->getScopedName());
            concat->addBlank();
            concat->addString("for");
            concat->addBlank();
            concat->addString(scope->parentScope->getFullName());
            concat->addEol();
        }
        else if (scope->is(ScopeKind::Enum))
        {
            concat->addString("impl");
            concat->addBlank();
            concat->addString("enum");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
        }
        else
        {
            concat->addString("impl");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
        }

        concat->addIndent(context.indent);
        concat->addChar('{');
        concat->addEol();

        context.indent++;
        SWAG_CHECK(outputScopeContent(context, module, scope));
        for (const auto oneScope : scope->childrenScopes)
        {
            if (oneScope->is(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(context, module, oneScope));
        }

        context.indent--;
        concat->addIndent(context.indent);
        concat->addChar('}');
        concat->addEol();

        for (const auto oneScope : scope->childrenScopes)
        {
            if (oneScope->isNot(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(context, module, oneScope));
        }

        concat->addBlankLine();
    }

    // Named scope
    else if (!scope->name.empty())
    {
        concat->addBlankLine();
        outputScopeBlock(context, module, scope);
        concat->addBlankLine();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(outputScopeContentAndChildren(context, module, scope));
    }

    return true;
}
