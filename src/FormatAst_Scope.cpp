#include "pch.h"
#include "Ast.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

bool FormatAst::outputScopeContent(const Module* module, const Scope* scope)
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
            SWAG_CHECK(outputAttributes(one, one->typeInfo));
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(one));
            concat->addEol();
            concat->addBlankLine();
        }
    }

    // Exported functions (signatures)
    if (!publicSet->publicFunc.empty())
    {
        for (const auto one : publicSet->publicFunc)
        {
            // Can be removed in case of special functions
            if (!one->hasAttribute(ATTRIBUTE_PUBLIC))
                continue;

            concat->addBlankLine();

            // Remap special functions to their generated equivalent
            AstFuncDecl* funcNode = castAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            funcNode->computeFullNameForeignExport();
            concat->addIndent(indent);
            concat->addStringFormat(R"(#[Foreign("%s", "%s")])", module->name.c_str(), funcNode->fullnameForeignExport.c_str());
            concat->addEol();
            SWAG_CHECK(outputAttributes(funcNode, funcNode->typeInfo));

            concat->addIndent(indent);
            if (funcNode->token.text == g_LangSpec->name_opInitGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opInit(using self);");
                concat->addEol();
            }
            else if (funcNode->token.text == g_LangSpec->name_opDropGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opDrop(using self);");
                concat->addEol();
            }
            else if (funcNode->token.text == g_LangSpec->name_opPostCopyGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostCopy(using self);");
                concat->addEol();
            }
            else if (funcNode->token.text == g_LangSpec->name_opPostMoveGenerated)
            {
                CONCAT_FIXED_STR(concat, "func opPostMove(using self);");
                concat->addEol();
            }
            else
                SWAG_CHECK(outputFuncSignature(funcNode, nullptr, funcNode->parameters, funcNode->validIf));

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
            SWAG_CHECK(outputAttributesUsage(typeFunc));
            concat->addIndent(indent);
            SWAG_CHECK(outputFuncSignature(node, nullptr, node->parameters, nullptr));
            concat->addBlankLine();
        }
    }

    return true;
}

bool FormatAst::outputScopeContentAndChilds(Module* module, const Scope* scope)
{
    SWAG_CHECK(outputScopeContent(module, scope));
    for (const auto oneScope : scope->childScopes)
        SWAG_CHECK(outputScope(module, oneScope));
    return true;
}

bool FormatAst::outputScopeBlock(Module* module, const Scope* scope)
{
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();
    indent++;
    SWAG_CHECK(outputScopeContentAndChilds(module, scope));
    indent--;
    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}

bool FormatAst::outputScope(Module* module, Scope* scope)
{
    SWAG_ASSERT(module);
    if (!scope->flags.has(SCOPE_FLAG_HAS_EXPORTS))
        return true;
    if (scope->flags.has(SCOPE_IMPORTED))
        return true;

    forExport = true;

    // Namespace
    if (scope->is(ScopeKind::Namespace) && !scope->name.empty())
    {
        if (!scope->flags.has(SCOPE_AUTO_GENERATED))
        {
            concat->addBlankLine();
            concat->addIndent(indent);
            CONCAT_FIXED_STR(concat, "namespace");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
            outputScopeBlock(module, scope);
            concat->addBlankLine();
        }
        else
        {
            SWAG_CHECK(outputScopeContentAndChilds(module, scope));
        }
    }

    // Impl
    else if (!scope->isGlobal() && scope->isGlobalOrImpl() && !scope->name.empty())
    {
        concat->addBlankLine();
        concat->addIndent(indent);
        if (scope->is(ScopeKind::Impl))
        {
            const auto nodeImpl = castAst<AstImpl>(scope->owner, AstNodeKind::Impl);
            const auto symbol   = nodeImpl->identifier->resolvedSymbolOverload();
            CONCAT_FIXED_STR(concat, "impl");
            concat->addBlank();
            concat->addString(symbol->node->getScopedName());
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "for");
            concat->addBlank();
            concat->addString(scope->parentScope->name);
            concat->addEol();
        }
        else if (scope->is(ScopeKind::Enum))
        {
            CONCAT_FIXED_STR(concat, "impl");
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "enum");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
        }
        else
        {
            CONCAT_FIXED_STR(concat, "impl");
            concat->addBlank();
            concat->addString(scope->name);
            concat->addEol();
        }

        concat->addIndent(indent);
        concat->addChar('{');
        concat->addEol();

        indent++;
        SWAG_CHECK(outputScopeContent(module, scope));
        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->is(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(module, oneScope));
        }

        indent--;
        concat->addIndent(indent);
        concat->addChar('}');
        concat->addEol();

        for (const auto oneScope : scope->childScopes)
        {
            if (oneScope->isNot(ScopeKind::Impl))
                continue;
            SWAG_CHECK(outputScope(module, oneScope));
        }

        concat->addBlankLine();
    }

    // Named scope
    else if (!scope->name.empty())
    {
        concat->addBlankLine();
        outputScopeBlock(module, scope);
        concat->addBlankLine();
    }

    // Unnamed scope
    else
    {
        SWAG_CHECK(outputScopeContentAndChilds(module, scope));
    }

    return true;
}

bool FormatAst::outputNamespace(const AstNode* node)
{
    if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_GENERATED_TOP_LEVEL))
    {
        SWAG_CHECK(outputChildren(node));
        return true;
    }

    concat->addIndent(indent);
    CONCAT_FIXED_STR(concat, "namespace");
    concat->addBlank();
    concat->addString(node->token.text);
    concat->addEol();
    concat->addIndent(indent);
    concat->addChar('{');
    concat->addEol();
    indent++;
    outputChildren(node);
    indent--;
    concat->addIndent(indent);
    concat->addChar('}');
    concat->addEol();
    return true;
}
