#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "SemanticError.h"

bool SemanticError::warnDeprecated(SemanticContext* context, AstNode* identifier)
{
    const auto node = identifier->resolvedSymbolOverload()->node;
    if (!node->hasAttribute(ATTRIBUTE_DEPRECATED))
        return true;

    const auto           symbol = identifier->resolvedSymbolOverload()->symbol;
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
    }

    Diagnostic err{identifier, identifier->token, formErr(Wrn0002, Naming::kindName(symbol->kind).c_str(), identifier->resolvedSymbolOverload()->symbol->name.c_str()), DiagnosticLevel::Warning};
    const auto note1   = Diagnostic::note(node, node->token, toNte(Nte0066));
    note1->canBeMerged = false;
    err.addNote(note1);

    if (v && v->text.empty())
        return context->report(err);

    err.addNote(v->text);
    return context->report(err);
}

bool SemanticError::warnUnusedFunction(const Module* moduleToGen, const ByteCode* one)
{
    if (moduleToGen->is(ModuleKind::Test))
        return true;
    if (!one->node || !one->node->token.sourceFile || !one->node->resolvedSymbolName())
        return true;
    if (one->node->token.sourceFile->hasFlag(FILE_EMBEDDED) || one->node->token.sourceFile->hasFlag(FILE_EXTERNAL) || one->node->token.sourceFile->imported)
        return true;
    if (one->node->token.sourceFile->hasFlag(FILE_RUNTIME) || one->node->token.sourceFile->hasFlag(FILE_BOOTSTRAP))
        return true;
    if (one->node->token.sourceFile->hasFlag(FILE_FORCE_EXPORT) || one->node->token.sourceFile->globalAttr.has(ATTRIBUTE_PUBLIC))
        return true;
    const auto funcDecl = castAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
    if (funcDecl->fromItfSymbol)
        return true;
    if (funcDecl->hasAstFlag(AST_IS_GENERIC | AST_GENERATED | AST_FROM_GENERIC))
        return true;
    if (funcDecl->hasAttribute(ATTRIBUTE_PUBLIC | ATTRIBUTE_INLINE | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_MAIN_FUNC |
                               ATTRIBUTE_COMPILER))
        return true;

    if (one->node->resolvedSymbolName()->hasFlag(SYMBOL_USED) || one->isUsed)
        return true;
    if (funcDecl->token.text[0] == '_')
        return true;

    Diagnostic err{funcDecl, funcDecl->tokenName, formErr(Wrn0006, "function", "function", funcDecl->token.c_str()), DiagnosticLevel::Warning};
    if (!funcDecl->isSpecialFunctionName())
        err.hint = formNte(Nte0082, funcDecl->token.c_str());
    return Report::report(err);
}

bool SemanticError::warnUnusedVariables(SemanticContext* context, const Scope* scope)
{
    const auto node = context->node;
    if (!node->token.sourceFile || !node->token.sourceFile->module)
        return true;
    if (node->isEmptyFct())
        return true;
    if (node->hasAstFlag(AST_IS_GENERIC))
        return true;
    if (scope->is(ScopeKind::Struct))
        return true;

    if (scope->isNot(ScopeKind::Inline) && scope->owner != node && !node->isParentOf(scope->owner))
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

        if (sym->is(SymbolKind::GenericType))
            continue;
        if (sym->name[0] == '#')
            continue;
        if (sym->name[0] == '_')
            continue;

        const auto front = sym->nodes.front();

        // Remove generated symbol, except when unpacking a tuple.
        if (front->isNot(AstNodeKind::VarDecl) || !front->hasSpecFlag(AstVarDecl::SPEC_FLAG_TUPLE_AFFECT))
        {
            if (front->hasAstFlag(AST_GENERATED))
                continue;
        }

        const auto overload = sym->overloads.front();
        if (overload->hasFlag(OVERLOAD_RETVAL))
            continue;

        // Check that variable has been changed
        if (sym->hasFlag(SYMBOL_USED) &&
            !overload->hasFlag(OVERLOAD_IS_LET) &&
            !overload->hasFlag(OVERLOAD_HAS_AFFECT) &&
            !overload->hasFlag(OVERLOAD_HAS_MAKE_POINTER) &&
            !overload->hasFlag(OVERLOAD_NOT_INITIALIZED) &&
            overload->hasFlag(OVERLOAD_VAR_LOCAL))
        {
            if (!overload->typeInfo->isStruct() && !overload->typeInfo->isArray())
            {
                if (!overload->hasFlag(OVERLOAD_VAR_HAS_ASSIGN))
                {
                    Diagnostic err{front, front->token, formErr(Wrn0004, sym->name.c_str()), DiagnosticLevel::Warning};
                    isOk = isOk && context->report(err);
                }
                else
                {
                    Diagnostic err{front, front->token, formErr(Wrn0003, sym->name.c_str()), DiagnosticLevel::Warning};
                    isOk = isOk && context->report(err);
                }
            }
        }

        if (sym->hasFlag(SYMBOL_USED))
            continue;
        if (overload->typeInfo->isCVariadic())
            continue;

        if (overload->hasFlag(OVERLOAD_VAR_LOCAL))
        {
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(formNte(Nte0082, sym->name.c_str()));
            isOk = isOk && context->report(err);
        }
        else if (overload->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
        {
            const auto funcDecl = castAst<AstFuncDecl>(front->findParent(AstNodeKind::FuncDecl), AstNodeKind::FuncDecl);

            // If the function is an interface implementation, no unused check
            if (funcDecl->fromItfSymbol)
                continue;
            // If this is a lambda expression
            if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IS_LAMBDA_EXPRESSION))
                continue;

            if (front->isGeneratedSelf())
            {
                const auto msg = formErr(Wrn0006, Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
                Diagnostic err{front->ownerFct, front->ownerFct->token, msg, DiagnosticLevel::Warning};
                err.hint = toNte(Nte0145);
                err.addNote(toNte(Nte0039));
                isOk = isOk && context->report(err);
            }
            else
            {
                const auto msg = formErr(Wrn0006, Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
                Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
                err.addNote(formNte(Nte0082, sym->name.c_str()));
                isOk = isOk && context->report(err);
            }
        }
        else if (overload->hasFlag(OVERLOAD_VAR_CAPTURE))
        {
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(formNte(Nte0082, sym->name.c_str()));
            isOk = isOk && context->report(err);
        }
        else if (overload->hasFlag(OVERLOAD_CONSTANT))
        {
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).c_str(), Naming::kindName(overload).c_str(), sym->name.c_str());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(formNte(Nte0082, sym->name.c_str()));
            isOk = isOk && context->report(err);
        }
    }

    return isOk;
}

bool SemanticError::warnUnreachableCode(SemanticContext* context)
{
    const auto node = context->node;

    // Return must be the last of its block
    if (node->parent->lastChild() != node)
    {
        if (node->parent->is(AstNodeKind::If))
        {
            const AstIf* ifNode = castAst<AstIf>(node->parent, AstNodeKind::If);
            if (ifNode->ifBlock == node || ifNode->elseBlock == node)
                return true;
        }

        const auto idx = node->childParentIdx();
        return context->report({node->parent->children[idx + 1], toErr(Wrn0005), DiagnosticLevel::Warning});
    }

    return true;
}
