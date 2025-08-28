#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

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

    Diagnostic err{identifier, identifier->token, formErr(Wrn0002, Naming::kindName(symbol->kind).cstr(), identifier->resolvedSymbolOverload()->symbol->name.cstr()), DiagnosticLevel::Warning};
    const auto note1   = Diagnostic::hereIs(node, "this is the deprecated definition");
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
    if (funcDecl->hasAstFlag(AST_GENERIC | AST_GENERATED | AST_FROM_GENERIC))
        return true;
    if (funcDecl->hasAttribute(ATTRIBUTE_PUBLIC | ATTRIBUTE_INLINE | ATTRIBUTE_INIT_FUNC | ATTRIBUTE_DROP_FUNC | ATTRIBUTE_PREMAIN_FUNC | ATTRIBUTE_MAIN_FUNC | ATTRIBUTE_COMPILER))
        return true;

    if (one->node->resolvedSymbolName()->hasFlag(SYMBOL_USED) || one->isUsed)
        return true;
    if (funcDecl->token.text[0] == '_')
        return true;

    Diagnostic err{funcDecl, funcDecl->tokenName, formErr(Wrn0006, "function", "function", funcDecl->token.cstr()), DiagnosticLevel::Warning};
    if (!funcDecl->isSpecialFunctionName())
        err.hint = form("consider renaming it to [[_%s]] if this is intentional", funcDecl->token.cstr());
    return Report::report(err);
}

bool SemanticError::warnUnusedVariables(SemanticContext* context, const Scope* scope)
{
    const auto node = context->node;
    if (!node->token.sourceFile || !node->token.sourceFile->module)
        return true;
    if (node->isEmptyFct())
        return true;
    if (node->hasAstFlag(AST_GENERIC))
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
            !overload->hasFlag(OVERLOAD_VAR_IS_LET) &&
            !overload->hasFlag(OVERLOAD_HAS_AFFECT) &&
            !overload->hasFlag(OVERLOAD_HAS_MAKE_POINTER) &&
            !overload->hasFlag(OVERLOAD_NOT_INITIALIZED) &&
            overload->hasFlag(OVERLOAD_VAR_LOCAL))
        {
            if (!overload->typeInfo->isStruct() && !overload->typeInfo->isArray())
            {
                if (!overload->hasFlag(OVERLOAD_VAR_HAS_ASSIGN))
                {
                    Diagnostic err{front, front->token, formErr(Wrn0004, sym->name.cstr()), DiagnosticLevel::Warning};
                    isOk = isOk && context->report(err);
                }
                else
                {
                    Diagnostic err{front, front->token, formErr(Wrn0003, sym->name.cstr()), DiagnosticLevel::Warning};
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
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).cstr(), Naming::kindName(overload).cstr(), sym->name.cstr());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(form("consider renaming it to [[_%s]] if this is intentional", sym->name.cstr()));
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

            if (front->isGeneratedMe())
            {
                if (funcDecl->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL | AstFuncDecl::SPEC_FLAG_DEFAULT_IMPL))
                    continue;
                const auto msg = formErr(Wrn0006, Naming::kindName(overload).cstr(), Naming::kindName(overload).cstr(), sym->name.cstr());
                Diagnostic err{front->ownerFct, front->ownerFct->token, msg, DiagnosticLevel::Warning};
                err.hint = "there is an implied first parameter [[me]]";
                err.addNote("consider using [[func]] instead of [[mtd]] to avoid the implicit [[me]] parameter.");
                isOk = isOk && context->report(err);
            }
            else
            {
                const auto msg = formErr(Wrn0006, Naming::kindName(overload).cstr(), Naming::kindName(overload).cstr(), sym->name.cstr());
                Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
                err.addNote(form("consider renaming it to [[_%s]] if this is intentional", sym->name.cstr()));
                isOk = isOk && context->report(err);
            }
        }
        else if (overload->hasFlag(OVERLOAD_VAR_CAPTURE))
        {
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).cstr(), Naming::kindName(overload).cstr(), sym->name.cstr());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(form("consider renaming it to [[_%s]] if this is intentional", sym->name.cstr()));
            isOk = isOk && context->report(err);
        }
        else if (overload->hasFlag(OVERLOAD_CONSTANT))
        {
            const auto msg = formErr(Wrn0006, Naming::kindName(overload).cstr(), Naming::kindName(overload).cstr(), sym->name.cstr());
            Diagnostic err{front, front->token, msg, DiagnosticLevel::Warning};
            err.addNote(form("consider renaming it to [[_%s]] if this is intentional", sym->name.cstr()));
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

bool SemanticError::warnWhereDoIf(SemanticContext* context)
{
    const auto     node       = context->node;
    const AstNode* block      = nullptr;
    const AstNode* expression = nullptr;

    if (node->is(AstNodeKind::Loop))
    {
        const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
        block               = loopNode->block;
        expression          = loopNode->expression;
        if (!block || block->isNot(AstNodeKind::Statement))
            return true;
        if (block->hasSpecFlag(AstStatement::SPEC_FLAG_IS_WHERE | AstStatement::SPEC_FLAG_CURLY))
            return true;
        if (block->childCount() != 1 || block->firstChild()->isNot(AstNodeKind::If))
            return true;
        const auto ifNode = castAst<AstIf>(block->firstChild(), AstNodeKind::If);
        if (ifNode->elseBlock)
            return true;
    }
    else if (node->is(AstNodeKind::ForEach))
    {
        const auto visitNode = castAst<AstVisit>(node, AstNodeKind::ForEach);
        block                = visitNode->block;
        expression           = visitNode->expression;
        if (!block || block->isNot(AstNodeKind::Statement))
            return true;
        if (block->hasSpecFlag(AstStatement::SPEC_FLAG_IS_WHERE | AstStatement::SPEC_FLAG_CURLY))
            return true;
        if (block->childCount() != 1 || block->firstChild()->isNot(AstNodeKind::If))
            return true;
        const auto ifNode = castAst<AstIf>(block->firstChild(), AstNodeKind::If);
        if (ifNode->elseBlock)
            return true;
    }
    else if (node->is(AstNodeKind::SwitchCase))
    {
        return true;
        /*const auto caseNode = castAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
        block               = caseNode->block;
        if (!caseNode->expressions.empty())
            expression = caseNode->expressions.back();
        if (block->childCount()!= 1 || block->firstChild()->isNot(AstNodeKind::If))
            return true;*/
    }

    Diagnostic err{block->firstChild(), block->firstChild()->token, formErr(Wrn0010, node->token.cstr()), DiagnosticLevel::Warning};
    if (expression)
    {
        SourceLocation loc;
        expression->computeLocation(loc, loc);
        err.addNote(loc, loc, "consider using [[where <expression>]] after this to replace the [[if]] statement");
    }
    else
    {
        err.addNote(node, node->token, "consider using [[where <expression>]] after this to replace the [[if]] statement");
    }

    SWAG_CHECK(context->report(err));
    return true;
}

bool SemanticError::warnElseDoIf(SemanticContext* context, const AstIf* ifNode)
{
    const auto elseBlock = ifNode->elseBlock;
    if (!elseBlock)
        return true;

    if (ifNode->is(AstNodeKind::CompilerIf) &&
        elseBlock->firstChild() &&
        elseBlock->firstChild()->is(AstNodeKind::Statement) &&
        !elseBlock->firstChild()->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY) &&
        elseBlock->firstChild()->firstChild() &&
        elseBlock->firstChild()->firstChild()->is(AstNodeKind::CompilerIf))
    {
        const Diagnostic err{ifNode->elseBlock->firstChild(), ifNode->elseBlock->firstChild()->token, toErr(Wrn0009), DiagnosticLevel::Warning};
        SWAG_CHECK(context->report(err));
    }

    if (ifNode->is(AstNodeKind::If) &&
        elseBlock->is(AstNodeKind::Statement) &&
        !elseBlock->hasSpecFlag(AstStatement::SPEC_FLAG_CURLY) &&
        elseBlock->firstChild() &&
        elseBlock->firstChild()->is(AstNodeKind::If))
    {
        const Diagnostic err{ifNode->elseBlock->firstChild(), ifNode->elseBlock->firstChild()->token, toErr(Wrn0008), DiagnosticLevel::Warning};
        SWAG_CHECK(context->report(err));
    }

    return true;
}
