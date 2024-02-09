#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "SemanticError.h"

bool SemanticError::warnDeprecated(SemanticContext* context, AstNode* identifier)
{
    const auto node = identifier->resolvedSymbolOverload->node;
    if (!node->hasAttribute(ATTRIBUTE_DEPRECATED))
        return true;

    const auto           symbol = identifier->resolvedSymbolOverload->symbol;
    const ComputedValue* v      = nullptr;
    switch (node->kind)
    {
    case AstNodeKind::FuncDecl:
    {
        const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        v                   = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::EnumDecl:
    {
        const auto typeInfo = castTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        v                   = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::StructDecl:
    {
        const auto typeInfo = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        v                   = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::InterfaceDecl:
    {
        const auto typeInfo = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);
        v                   = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    case AstNodeKind::EnumValue:
    {
        const auto typeInfo = castAst<AstEnumValue>(node, AstNodeKind::EnumValue);
        v                   = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Deprecated, g_LangSpec->name_msg);
        break;
    }
    default:
        break;
    }

    const Diagnostic diag{identifier, identifier->token, FMT(Err(Wrn0002), Naming::kindName(symbol->kind).c_str(), identifier->resolvedSymbolOverload->symbol->name.c_str()),
                          DiagnosticLevel::Warning};
    const auto note1   = Diagnostic::note(node, node->token, Nte(Nte0066));
    note1->canBeMerged = false;

    if (v && v->text.empty())
    {
        return context->report(diag, note1);
    }
    else
    {
        const auto note2 = Diagnostic::note(v->text);
        return context->report(diag, note1, note2);
    }
}

bool SemanticError::warnUnusedFunction(const Module* moduleToGen, const ByteCode* one)
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
    const auto funcDecl = castAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
    if (funcDecl->fromItfSymbol)
        return true;
    if (funcDecl->flags & (AST_IS_GENERIC | AST_GENERATED | AST_FROM_GENERIC))
        return true;
    if (funcDecl->hasAttribute(ATTRIBUTE_PUBLIC | ATTRIBUTE_INLINE | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_MAIN_FUNC |
                               ATTRIBUTE_COMPILER))
        return true;

    if (one->node->resolvedSymbolName->flags & SYMBOL_USED || one->isUsed)
        return true;
    if (funcDecl->token.text[0] == '_')
        return true;

    Diagnostic diag{funcDecl, funcDecl->tokenName, FMT(Err(Wrn0006), "function", "function", funcDecl->token.ctext()), DiagnosticLevel::Warning};
    if (!funcDecl->isSpecialFunctionName())
        diag.hint = FMT(Nte(Nte0082), funcDecl->token.ctext());
    return Report::report(diag);
}

bool SemanticError::warnUnusedVariables(SemanticContext* context, const Scope* scope)
{
    const auto node = context->node;
    if (!node->sourceFile || !node->sourceFile->module)
        return true;
    if (node->isEmptyFct())
        return true;
    if (node->flags & AST_IS_GENERIC)
        return true;
    if (scope->kind == ScopeKind::Struct)
        return true;

    if (scope->kind != ScopeKind::Inline && scope->owner != node && !node->isParentOf(scope->owner))
        return true;

    const auto& table = scope->symTable;
    ScopedLock  lock(table.mutex);

    uint32_t cptDone = 0;
    bool     isOk    = true;
    for (uint32_t i = 0; i < table.mapNames.allocated && cptDone != table.mapNames.count; i++)
    {
        const auto sym = table.mapNames.buffer[i].symbolName;
        if (!sym)
            continue;
        cptDone++;

        if (!sym->nodes.count || !sym->overloads.count)
            continue;

        if (sym->kind == SymbolKind::GenericType)
            continue;
        if (sym->name[0] == '#')
            continue;
        if (sym->name[0] == '_')
            continue;

        const auto front = sym->nodes.front();

        // Remove generated symbol, except when unpacking a tuple.
        if (front->kind != AstNodeKind::VarDecl || !(front->specFlags & AstVarDecl::SPECFLAG_TUPLE_AFFECT))
        {
            if (front->flags & AST_GENERATED)
                continue;
        }

        const auto overload = sym->overloads.front();
        if (overload->flags & OVERLOAD_RETVAL)
            continue;

        // Check that variable has been changed
        if ((sym->flags & SYMBOL_USED) &&
            !(overload->flags & OVERLOAD_IS_LET) &&
            !(overload->flags & OVERLOAD_HAS_AFFECT) &&
            !(overload->flags & OVERLOAD_HAS_MAKE_POINTER) &&
            !(overload->flags & OVERLOAD_NOT_INITIALIZED) &&
            (overload->flags & OVERLOAD_VAR_LOCAL))
        {
            if (!overload->typeInfo->isStruct() && !overload->typeInfo->isArray())
            {
                if (!(overload->flags & OVERLOAD_VAR_HAS_ASSIGN))
                {
                    Diagnostic diag{front, front->token, FMT(Err(Wrn0004), sym->name.c_str()), DiagnosticLevel::Warning};
                    isOk = isOk && context->report(diag);
                }
                else
                {
                    Diagnostic diag{front, front->token, FMT(Err(Wrn0003), sym->name.c_str()), DiagnosticLevel::Warning};
                    isOk = isOk && context->report(diag);
                }
            }
        }

        if (sym->flags & SYMBOL_USED)
            continue;
        if (overload->typeInfo->isCVariadic())
            continue;

        if (overload->flags & OVERLOAD_VAR_LOCAL)
        {
            const auto msg = FMT(Err(Wrn0006), Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic diag{front, front->token, msg, DiagnosticLevel::Warning};
            const auto note = Diagnostic::note(FMT(Nte(Nte0082), sym->name.c_str()));
            isOk            = isOk && context->report(diag, note);
        }
        else if (overload->flags & OVERLOAD_VAR_FUNC_PARAM)
        {
            const auto funcDecl = castAst<AstFuncDecl>(front->findParent(AstNodeKind::FuncDecl), AstNodeKind::FuncDecl);

            // If the function is an interface implementation, no unused check
            if (funcDecl->fromItfSymbol)
                continue;
            // If this is a lambda expression
            if (funcDecl->specFlags & AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION)
                continue;

            if (front->isGeneratedSelf())
            {
                const auto msg = FMT(Err(Wrn0006), Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
                Diagnostic diag{front->ownerFct, front->ownerFct->token, msg, DiagnosticLevel::Warning};
                diag.hint       = Nte(Nte0145);
                const auto note = Diagnostic::note(Nte(Nte0039));
                isOk            = isOk && context->report(diag, note);
            }
            else
            {
                const auto msg = FMT(Err(Wrn0006), Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
                Diagnostic diag{front, front->token, msg, DiagnosticLevel::Warning};
                const auto note = Diagnostic::note(FMT(Nte(Nte0082), sym->name.c_str()));
                isOk            = isOk && context->report(diag, note);
            }
        }
        else if (overload->flags & OVERLOAD_VAR_CAPTURE)
        {
            const auto msg = FMT(Err(Wrn0006), Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic diag{front, front->token, msg, DiagnosticLevel::Warning};
            const auto note = Diagnostic::note(FMT(Nte(Nte0082), sym->name.c_str()));
            isOk            = isOk && context->report(diag, note);
        }
        else if (overload->flags & OVERLOAD_CONSTANT)
        {
            const auto msg = FMT(Err(Wrn0006), Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic diag{front, front->token, msg, DiagnosticLevel::Warning};
            const auto note = Diagnostic::note(FMT(Nte(Nte0082), sym->name.c_str()));
            isOk            = isOk && context->report(diag, note);
        }
    }

    return isOk;
}

bool SemanticError::warnUnreachableCode(SemanticContext* context)
{
    const auto node = context->node;

    // Return must be the last of its block
    if (node->parent->childs.back() != node)
    {
        if (node->parent->kind == AstNodeKind::If)
        {
            const AstIf* ifNode = castAst<AstIf>(node->parent, AstNodeKind::If);
            if (ifNode->ifBlock == node || ifNode->elseBlock == node)
                return true;
        }

        const auto idx = node->childParentIdx();
        return context->report({node->parent->childs[idx + 1], Err(Wrn0005), DiagnosticLevel::Warning});
    }

    return true;
}
