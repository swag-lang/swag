#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "ByteCode.h"
#include "Module.h"
#include "Report.h"
#include "Naming.h"
#include "Naming.h"

bool SemanticJob::warnDeprecated(SemanticContext* context, AstNode* identifier)
{
    auto node = identifier->resolvedSymbolOverload->node;
    if (!(node->attributeFlags & ATTRIBUTE_DEPRECATED))
        return true;
    auto symbol = identifier->resolvedSymbolOverload->symbol;

    const ComputedValue* v = nullptr;
    switch (node->kind)
    {
    case AstNodeKind::FuncDecl:
    {
        auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        v             = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::EnumDecl:
    {
        auto typeInfo = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        v             = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::StructDecl:
    {
        auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        v             = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::InterfaceDecl:
    {
        auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);
        v             = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::EnumValue:
    {
        auto typeInfo = CastAst<AstEnumValue>(node, AstNodeKind::EnumValue);
        v             = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    default:
        break;
    }

    Diagnostic diag{identifier, identifier->token, Fmt(Err(Wrn0003), Naming::kindName(symbol->kind).c_str(), identifier->resolvedSymbolOverload->symbol->name.c_str()), DiagnosticLevel::Warning};
    auto       note1 = Diagnostic::note(node, node->token, Nte(Nte0031));
    note1->showRange = false;

    if (v && v->text.empty())
    {
        return context->report(diag, note1);
    }
    else
    {
        auto note2 = Diagnostic::note(v->text);
        return context->report(diag, note1, note2);
    }
}

bool SemanticJob::warnUnusedFunction(Module* moduleToGen, ByteCode* one)
{
    if (moduleToGen->kind == ModuleKind::Test)
        return true;
    if (!one->node || !one->node->sourceFile || !one->node->resolvedSymbolName)
        return true;
    if (one->node->sourceFile->isEmbedded || one->node->sourceFile->isExternal || one->node->sourceFile->imported)
        return true;
    if (one->node->sourceFile->isRuntimeFile || one->node->sourceFile->isBootstrapFile)
        return true;
    if (one->node->sourceFile->forceExport || one->node->sourceFile->globalAttr & ATTRIBUTE_PUBLIC)
        return true;
    auto funcDecl = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
    if (funcDecl->fromItfSymbol)
        return true;
    if (funcDecl->flags & (AST_IS_GENERIC | AST_GENERATED | AST_FROM_GENERIC))
        return true;
    if (funcDecl->attributeFlags & (ATTRIBUTE_PUBLIC | ATTRIBUTE_INLINE | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_COMPILER))
        return true;

    if (one->node->resolvedSymbolName->flags & SYMBOL_USED || one->isUsed)
        return true;
    if (funcDecl->token.text[0] == '_')
        return true;

    Diagnostic diag{funcDecl, funcDecl->tokenName, Fmt(Err(Wrn0008), funcDecl->token.ctext()), DiagnosticLevel::Warning};
    if (!funcDecl->isSpecialFunctionName())
        diag.hint = Fmt(Hnt(Hnt0092), funcDecl->token.ctext());
    return Report::report(diag);
}

bool SemanticJob::warnUnusedVariables(SemanticContext* context, Scope* scope)
{
    auto node = context->node;
    if (!node->sourceFile || !node->sourceFile->module)
        return true;
    if (node->isEmptyFct())
        return true;
    if (node->flags & AST_IS_GENERIC)
        return true;
    if (scope->kind == ScopeKind::Struct)
        return true;
    if (scope->owner != node && !node->isParentOf(scope->owner))
        return true;

    auto&      table = scope->symTable;
    ScopedLock lock(table.mutex);

    uint32_t cptDone = 0;
    bool     isOk    = true;
    for (uint32_t i = 0; i < table.mapNames.allocated && cptDone != table.mapNames.count; i++)
    {
        auto sym = table.mapNames.buffer[i].symbolName;
        if (!sym)
            continue;
        cptDone++;

        if (!sym->nodes.count || !sym->overloads.count)
            continue;
        if (sym->flags & SYMBOL_USED)
            continue;
        if (sym->kind == SymbolKind::GenericType)
            continue;
        if (sym->name[0] == '@')
            continue;
        if (sym->name[0] == '_')
            continue;

        auto front    = sym->nodes.front();
        auto overload = sym->overloads.front();

        if (front->flags & AST_GENERATED)
            continue;
        if (overload->typeInfo->isCVariadic())
            continue;
        if (overload->flags & OVERLOAD_RETVAL)
            continue;

        if (overload->flags & OVERLOAD_VAR_LOCAL)
        {
            Diagnostic diag{front, front->token, Fmt(Err(Wrn0002), Naming::kindName(overload).c_str(), sym->name.c_str()), DiagnosticLevel::Warning};
            diag.hint = Fmt(Hnt(Hnt0092), sym->name.c_str());
            isOk      = isOk && context->report(diag);
        }
        else if (overload->flags & OVERLOAD_VAR_FUNC_PARAM)
        {
            auto funcDecl = CastAst<AstFuncDecl>(front->findParent(AstNodeKind::FuncDecl), AstNodeKind::FuncDecl);

            // If the function is an interface implementation, no unused check
            if (funcDecl->fromItfSymbol)
                continue;
            // If this is a lambda expression
            if (funcDecl->specFlags & AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION)
                continue;

            if (front->isGeneratedSelf())
            {
                Diagnostic diag{front->ownerFct, front->ownerFct->token, Fmt(Err(Wrn0005), Naming::kindName(overload).c_str(), sym->name.c_str()), DiagnosticLevel::Warning};
                diag.hint = Hnt(Hnt0049);
                auto note = Diagnostic::help(Hlp(Hlp0042));
                isOk      = isOk && context->report(diag, note);
            }
            else
            {
                Diagnostic diag{front, front->token, Fmt(Err(Wrn0004), Naming::kindName(overload).c_str(), sym->name.c_str()), DiagnosticLevel::Warning};
                diag.hint = Fmt(Hnt(Hnt0092), sym->name.c_str());
                isOk      = isOk && context->report(diag);
            }
        }
        else if (overload->flags & OVERLOAD_VAR_CAPTURE)
        {
            Diagnostic diag{front, front->token, Fmt(Err(Wrn0006), Naming::kindName(overload).c_str(), sym->name.c_str()), DiagnosticLevel::Warning};
            diag.hint = Hnt(Hnt0026);
            isOk      = isOk && context->report(diag);
        }
        else if (overload->flags & OVERLOAD_CONSTANT)
        {
            Diagnostic diag{front, front->token, Fmt(Err(Wrn0007), Naming::kindName(overload).c_str(), sym->name.c_str()), DiagnosticLevel::Warning};
            diag.hint = Fmt(Hnt(Hnt0092), sym->name.c_str());
            isOk      = isOk && context->report(diag);
        }
    }

    return isOk;
}

bool SemanticJob::warnUnreachableCode(SemanticContext* context)
{
    auto node = context->node;

    // Return must be the last of its block
    if (node->parent->childs.back() != node)
    {
        if (node->parent->kind == AstNodeKind::If)
        {
            AstIf* ifNode = CastAst<AstIf>(node->parent, AstNodeKind::If);
            if (ifNode->ifBlock == node || ifNode->elseBlock == node)
                return true;
        }

        auto idx = node->childParentIdx();
        return context->report({node->parent->childs[idx + 1], Err(Wrn0001), DiagnosticLevel::Warning});
    }

    return true;
}
