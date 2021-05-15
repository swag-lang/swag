#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    if (module->mustOptimizeBC(node) && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        node->boolExpression->flags |= AST_NO_BYTECODE;
        if (node->boolExpression->computedValue.reg.b)
        {
            if (node->elseBlock)
                node->elseBlock->flags |= AST_NO_BYTECODE;
        }
        else
        {
            node->ifBlock->flags |= AST_NO_BYTECODE;
        }

        return true;
    }

    node->byteCodeFct = ByteCodeGenJob::emitIf;
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterExpr;
    node->ifBlock->allocateExtension();
    node->ifBlock->extension->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterIf;

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not evaluate while if it's constant and false
    if (module->mustOptimizeBC(node) && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        if (!node->boolExpression->computedValue.reg.b)
        {
            node->boolExpression->flags |= AST_NO_BYTECODE;
            node->block->flags |= AST_NO_BYTECODE;
            return true;
        }
    }

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitWhileBeforeExpr;
    node->boolExpression->extension->byteCodeAfterFct  = ByteCodeGenJob::emitWhileAfterExpr;
    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveInlineBefore(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    context->expansionNode.push_back({node->parent, JobContext::ExpansionType::Inline});
    if (node->doneFlags & AST_DONE_RESOLVE_INLINED)
        return true;
    node->doneFlags |= AST_DONE_RESOLVE_INLINED;

    auto func = node->func;

    // For a return by copy, need to reserve room on the stack for the return result
    if (func->returnType && func->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
    {
        node->flags |= AST_TRANSIENT;
        node->concreteTypeInfoStorage = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += func->returnType->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    node->scope->startStackSize = node->ownerScope->startStackSize;

    // Register all function parameters as inline symbols
    if (func->parameters)
    {
        AstIdentifier* identifier = nullptr;
        if (node->parent->kind == AstNodeKind::Identifier)
            identifier = CastAst<AstIdentifier>(node->parent, AstNodeKind::Identifier, AstNodeKind::FuncCall);
        auto& symTable = node->parametersScope->symTable;
        for (int i = 0; i < func->parameters->childs.size(); i++)
        {
            auto funcParam = func->parameters->childs[i];

            // If the call parameter of the inlined function is constant, then we register it in a specific
            // constant scope, not in the caller (for mixins)/inline scope.
            // This is a separated scope because mixins do not have their own scope, and we must have a
            // different symbol registration for each constant value
            bool isConstant = false;
            if (identifier && identifier->callParameters)
            {
                for (int j = 0; j < identifier->callParameters->childs.size(); j++)
                {
                    auto callParam = CastAst<AstFuncCallParam>(identifier->callParameters->childs[j], AstNodeKind::FuncCallParam);
                    if (callParam->index != i)
                        continue;
                    if (!(callParam->flags & AST_VALUE_COMPUTED))
                        continue;
                    SWAG_ASSERT(node->parametersScope);
                    symTable.addSymbolTypeInfo(context,
                                               callParam,
                                               funcParam->typeInfo,
                                               SymbolKind::Variable,
                                               &callParam->computedValue,
                                               OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN | OVERLOAD_COMPUTED_VALUE,
                                               nullptr,
                                               callParam->computedValue.reg.offset,
                                               &funcParam->token.text);
                    isConstant = true;
                    break;
                }
            }

            if (!isConstant)
                node->parametersScope->symTable.addSymbolTypeInfo(context, funcParam, funcParam->typeInfo, SymbolKind::Variable, nullptr, OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN);
        }
    }

    // Sub declarations in an inline block have access to the 'parametersScope', so we must start resolving
    // them only when we have are filling the parameters
    resolveSubDecls(context, node->ownerFct);

    return true;
}

bool SemanticJob::resolveInlineAfter(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);
    auto fct  = node->func;

    // No need to check missing return for inline only, because the function has already been
    // checked as a separated function
    if (fct->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
    {
        if (fct->returnType && fct->returnType->typeInfo != g_TypeMgr.typeInfoVoid)
        {
            if (!(node->semFlags & AST_SEM_SCOPE_HAS_RETURN))
            {
                if (node->semFlags & AST_SEM_FCT_HAS_RETURN)
                    return context->report({fct, fct->token, format(Msg0748, fct->getDisplayName().c_str())});
                return context->report({fct, fct->token, format(Msg0606, fct->getDisplayName().c_str())});
            }
        }
    }

    context->expansionNode.pop_back();
    return true;
}

bool SemanticJob::resolveForBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstFor>(context->node, AstNodeKind::For);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveLoopBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveFor(SemanticContext* context)
{
    auto node = CastAst<AstFor>(context->node, AstNodeKind::For);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression));
    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->allocateExtension();
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->extension->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterExpr;

    node->postExpression->allocateExtension();
    node->postExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforePost;

    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveSwitchAfterExpr(SemanticContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node->parent, AstNodeKind::Switch);

    // For a switch on an enum, force a 'using' for each case
    if (node->typeInfo->kind == TypeInfoKind::Enum)
    {
        // :AutoScope
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        for (auto switchCase : switchNode->cases)
        {
            switchCase->allocateExtension();
            switchCase->extension->alternativeScopes.push_back(typeEnum->scope);
        }
    }

    // Same for a typeset
    if (node->typeInfo->kind == TypeInfoKind::TypeSet)
    {
        // :AutoScope
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::TypeSet);
        for (auto switchCase : switchNode->cases)
        {
            switchCase->allocateExtension();
            switchCase->extension->alternativeScopes.push_back(typeStruct->scope);
        }
    }

    // Automatic convert to 'kindOf'
    // This has no sens to do a switch on a 'typeset' or an 'any'. So instead of raising an error,
    // we implies the usage of '@kindof'. That way we have a switch on the underlying type.
    if (node->typeInfo->isNative(NativeTypeKind::Any) || node->typeInfo->kind == TypeInfoKind::TypeSet)
    {
        switchNode->beforeAutoCastType = node->typeInfo;
        node->byteCodeFct              = ByteCodeGenJob::emitImplicitKindOf;
        auto& typeTable                = node->sourceFile->module->typeTable;
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, node->typeInfo, &node->typeInfo, &node->computedValue.reg.u32, CONCRETE_SHOULD_WAIT));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);

    // Deal with complete
    SWAG_CHECK(collectAttributes(context, node, nullptr));
    SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_COMPLETE) || node->expression, context->report({node, node->token, Msg0607}));

    node->byteCodeFct = ByteCodeGenJob::emitSwitch;
    if (!node->expression)
    {
        node->allocateExtension();
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeSwitch;
        return true;
    }

    node->expression->allocateExtension();
    node->expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitSwitchAfterExpr;

    SWAG_CHECK(checkIsConcrete(context, node->expression));
    node->typeInfo = node->expression->typeInfo;

    auto typeSwitch = TypeManager::concreteType(node->typeInfo);

    // Verify switch expression type is valid
    SWAG_VERIFY(!typeSwitch->isNative(NativeTypeKind::Any), context->report({node->expression, Msg0608}));
    switch (typeSwitch->kind)
    {
    case TypeInfoKind::Slice:
    case TypeInfoKind::Array:
    case TypeInfoKind::Interface:
        return context->report({node->expression, format(Msg0609, typeSwitch->getDisplayName().c_str())});
    }

    SWAG_VERIFY(!node->cases.empty(), context->report({node, node->token, Msg0610}));

    // Collect constant expressions, to avoid double definitions
    set<uint64_t> val64;
    set<Utf8>     valText;
    for (auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                auto typeExpr = TypeManager::concreteType(expr->typeInfo);
                if (typeExpr->isNative(NativeTypeKind::String))
                {
                    if (valText.find(expr->computedValue.text) != valText.end())
                        return context->report({expr, format(Msg0611, expr->computedValue.text.c_str())});
                    valText.insert(expr->computedValue.text);
                }
                else
                {
                    auto value = expr->computedValue.reg.u64;
                    if (expr->flags & AST_VALUE_IS_TYPEINFO)
                        value = expr->computedValue.reg.offset;

                    if (val64.find(value) != val64.end())
                    {
                        if (expr->flags & AST_VALUE_IS_TYPEINFO)
                            return context->report({expr, format(Msg0611, expr->token.text.c_str())});
                        if (typeExpr->flags & TYPEINFO_INTEGER)
                            return context->report({expr, format(Msg0613, expr->computedValue.reg.u64)});
                        return context->report({expr, format(Msg0614, expr->computedValue.reg.f64)});
                    }

                    val64.insert(expr->computedValue.reg.u64);
                }
            }
            else if (node->attributeFlags & ATTRIBUTE_COMPLETE)
            {
                return context->report({expr, Msg0615});
            }
        }
    }

    // When a switch is marked as complete, be sure every definitions have been covered
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
    {
        // No default for a complete switch
        auto back = node->cases.back();
        SWAG_VERIFY(!back->expressions.empty(), context->report({back, back->token, Msg0616}));

        // For typeset, we need to test 'beforeAutoCastType', because the type of the switch is now to @kindof(originalType)
        if (node->typeInfo->kind != TypeInfoKind::Enum && !node->beforeAutoCastType)
            return context->report({node, node->token, format(Msg0617, node->typeInfo->getDisplayName().c_str())});
        if (node->beforeAutoCastType && node->beforeAutoCastType->kind != TypeInfoKind::TypeSet)
            return context->report({node, node->token, format(Msg0617, node->beforeAutoCastType->getDisplayName().c_str())});

        if (node->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
            if (typeSwitch->isNative(NativeTypeKind::String))
            {
                if (valText.size() != typeEnum->values.size())
                {
                    for (auto one : typeEnum->values)
                    {
                        if (valText.find(one->value.text) == valText.end())
                        {
                            Diagnostic diag{node, node->token, format(Msg0620, typeEnum->name.c_str(), one->namedParam.c_str())};
                            Diagnostic note{one->declNode, one->declNode->token, Msg0619, DiagnosticLevel::Note};
                            return context->report(diag, &note);
                        }
                    }
                }
            }
            else
            {
                if (val64.size() != typeEnum->values.size())
                {
                    for (auto one : typeEnum->values)
                    {
                        if (val64.find(one->value.reg.u64) == val64.end())
                        {
                            Diagnostic diag{node, node->token, format(Msg0620, typeEnum->name.c_str(), one->namedParam.c_str())};
                            Diagnostic note{one->declNode, one->declNode->token, Msg0619, DiagnosticLevel::Note};
                            return context->report(diag, &note);
                        }
                    }
                }
            }
        }
        else if (node->beforeAutoCastType && node->beforeAutoCastType->kind == TypeInfoKind::TypeSet)
        {
            auto typeSet = CastTypeInfo<TypeInfoStruct>(node->beforeAutoCastType, TypeInfoKind::TypeSet);
            if (val64.size() != typeSet->fields.size())
            {
                for (auto one : typeSet->fields)
                {
                    uint32_t offset;
                    auto&    typeTable = node->sourceFile->module->typeTable;
                    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, one->typeInfo, nullptr, &offset, CONCRETE_SHOULD_WAIT));
                    if (context->result != ContextResult::Done)
                        return true;
                    if (val64.find(offset) == val64.end())
                    {
                        Diagnostic diag{node, node->token, format(Msg0620, typeSet->name.c_str(), one->namedParam.c_str())};
                        Diagnostic note{one->declNode, one->declNode->token, Msg0619, DiagnosticLevel::Note};
                        return context->report(diag, &note);
                    }
                }
            }
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        if (oneExpression->kind == AstNodeKind::Range)
        {
            auto rangeNode = CastAst<AstRange>(oneExpression, AstNodeKind::Range);
            if (node->ownerSwitch->expression)
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionLow, CASTFLAG_COMPARE));
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionUp, CASTFLAG_COMPARE));
            }
            else
            {
                return context->report({rangeNode, Msg0337});
            }
        }
        else
        {
            SWAG_CHECK(checkIsConcreteOrType(context, oneExpression));
            if (context->result != ContextResult::Done)
                return true;

            // switch with an expression : compare case with the switch expression
            if (node->ownerSwitch->expression)
            {
                auto typeInfo = TypeManager::concreteType(node->ownerSwitch->expression->typeInfo);
                if (typeInfo->kind == TypeInfoKind::Struct)
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, "opEquals", nullptr, nullptr, node->ownerSwitch->expression, oneExpression));
                    if (context->result != ContextResult::Done)
                        return true;
                }
                else
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression, CASTFLAG_COMPARE));
            }

            // switch without an expression : a case is a boolean expressions
            else
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, oneExpression->typeInfo, nullptr, oneExpression, CASTFLAG_COMPARE));
            }
        }
    }

    if (node->ownerSwitch->expression)
        node->typeInfo = node->ownerSwitch->expression->typeInfo;

    if (!node->expressions.empty())
    {
        node->allocateExtension();
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeCase;
    }

    node->block->allocateExtension();
    node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->extension->byteCodeAfterFct  = ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstLoop>(context->node, AstNodeKind::Loop);

    if (node->expression)
    {
        // No range
        if (node->expression->kind != AstNodeKind::Range)
        {
            SWAG_CHECK(checkIsConcrete(context, node->expression));
            SWAG_CHECK(resolveIntrinsicCountOf(context, node->expression, node->expression->typeInfo));
            if (context->result != ContextResult::Done)
                return true;
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, node->expression->typeInfo, nullptr, node->expression, CASTFLAG_TRY_COERCE));
            node->typeInfo = node->expression->typeInfo;

            // Do not evaluate loop if it's constant and 0
            if (module->mustOptimizeBC(node) && (node->expression->flags & AST_VALUE_COMPUTED))
            {
                if (!node->expression->computedValue.reg.u64)
                {
                    node->expression->flags |= AST_NO_BYTECODE;
                    node->block->flags |= AST_NO_BYTECODE;
                    return true;
                }
            }
        }

        // Range
        else
        {
            auto rangeNode = CastAst<AstRange>(node->expression, AstNodeKind::Range);
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoInt, rangeNode->expressionLow->typeInfo, nullptr, rangeNode->expressionLow, CASTFLAG_TRY_COERCE));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoInt, rangeNode->expressionUp->typeInfo, nullptr, rangeNode->expressionUp, CASTFLAG_TRY_COERCE));
        }

        node->expression->allocateExtension();
        node->expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterExpr;
    }

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->allocateExtension();
    SWAG_ASSERT(!node->extension->byteCodeAfterFct || node->extension->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope);
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    if (!node->expression)
        node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitLoopBeforeBlock;

    return true;
}

bool SemanticJob::resolveVisit(SemanticContext* context)
{
    auto job        = context->job;
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstVisit>(context->node, AstNodeKind::Visit);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    // Struct type : convert to a opVisit call
    auto     typeInfo      = TypeManager::concreteReference(node->expression->typeInfo);
    AstNode* newExpression = nullptr;
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_VERIFY(!(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report({node, node->token, Msg0624}));
        AstIdentifierRef* identifierRef = nullptr;
        bool              cloneParam    = false;
        if (node->expression->kind == AstNodeKind::IdentifierRef)
            identifierRef = (AstIdentifierRef*) Ast::clone(node->expression, node);
        else
        {
            cloneParam    = true;
            identifierRef = Ast::newIdentifierRef(sourceFile, node);
        }

        auto identifier        = Ast::newIdentifier(sourceFile, format("opVisit%s", node->extraNameToken.text.c_str()), identifierRef, identifierRef);
        identifier->aliasNames = node->aliasNames;
        identifier->inheritTokenLocation(node->token);

        // Generic parameters
        identifier->genericParameters = Ast::newFuncCallParams(sourceFile, identifier);
        identifier->genericParameters->flags |= AST_NO_BYTECODE;
        auto child                 = Ast::newFuncCallParam(sourceFile, identifier->genericParameters);
        child->typeInfo            = g_TypeMgr.typeInfoBool;
        child->computedValue.reg.b = node->wantPointer;
        child->flags |= AST_VALUE_COMPUTED | AST_NO_SEMANTIC;

        // Call with arguments
        identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
        newExpression              = identifierRef;

        // Need to add parameter
        if (cloneParam)
        {
            auto callParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
            Ast::clone(node->expression, callParam);
        }

        Ast::removeFromParent(node->block);
        Ast::addChildBack(node, node->block);
        node->expression->flags |= AST_NO_BYTECODE;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node->block);
        job->nodes.push_back(node);
        return true;
    }

    SWAG_VERIFY(node->extraNameToken.text.empty(), context->report({node, node->extraNameToken, format(Msg0625, typeInfo->getDisplayName().c_str())}));
    SWAG_VERIFY(node->aliasNames.size() <= 2, context->report({node, node->aliasNames[2], format(Msg0626, node->aliasNames.size())}));

    Utf8 alias0Name = node->aliasNames.empty() ? Utf8("@alias0") : node->aliasNames[0].text;
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? Utf8("@alias1") : node->aliasNames[1].text;
    Utf8 content;

    // Get back the expression string
    auto& concat = context->job->tmpConcat;
    concat.init(1024);
    Ast::OutputContext outputContext;
    SWAG_CHECK(Ast::output(outputContext, concat, node->expression));
    concat.addU8(0);
    SWAG_ASSERT(concat.firstBucket->nextBucket == nullptr);
    node->expression->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    int id = g_Global.uniqueID.fetch_add(1);

    // Multi dimensional array
    if (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Array)
    {
        auto typeArray   = (TypeInfoArray*) typeInfo;
        auto pointedType = typeArray->finalType;

        content += format("{ var __addr%u = cast(*%s) @dataof(%s); ", id, typeArray->finalType->name.c_str(), (const char*) concat.firstBucket->datas);
        content += format("const __count%u = @sizeof(%s) / %u; ", id, (const char*) concat.firstBucket->datas, typeArray->finalType->sizeOf);
        content += format("loop __count%u { ", id);
        if (node->wantPointer)
            content += format("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else if (pointedType->kind == TypeInfoKind::Struct)
        {
            pointedType->computeScopedName();
            content += format("var %s = cast(const *%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
        }
        else
            content += format("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += format("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // One dimensional array
    else if (typeInfo->kind == TypeInfoKind::Slice || typeInfo->kind == TypeInfoKind::Array)
    {
        TypeInfo* pointedType;
        if (typeInfo->kind == TypeInfoKind::Slice)
            pointedType = ((TypeInfoSlice*) typeInfo)->pointedType;
        else
            pointedType = ((TypeInfoArray*) typeInfo)->pointedType;

        content += format("{ var __addr%u = @dataof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += format("loop %s { ", (const char*) concat.firstBucket->datas);
        if (node->wantPointer)
            content += format("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else if (pointedType->kind == TypeInfoKind::Struct)
        {
            pointedType->computeScopedName();
            content += format("var %s = cast(const *%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
        }
        else
            content += format("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += format("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // String
    else if (typeInfo->isNative(NativeTypeKind::String))
    {
        content += format("{ var __addr%u = @dataof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += format("loop %s { ", (const char*) concat.firstBucket->datas);
        if (node->wantPointer)
            content += format("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else
            content += format("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += format("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // Variadic
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        content += format("{ loop %s { ", (const char*) concat.firstBucket->datas);
        SWAG_VERIFY(!node->wantPointer, context->report({node, node->token, Msg0627}));
        content += format("var %s = %s[@index]; ", alias0Name.c_str(), (const char*) concat.firstBucket->datas);
        content += format("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    else
    {
        PushErrHint errh(Msg0628);
        return context->report({node->expression, format(Msg0629, typeInfo->getDisplayName().c_str())});
    }

    SyntaxJob syntaxJob;
    syntaxJob.constructEmbedded(content, node, node, CompilerAstKind::EmbeddedInstruction, false);
    newExpression = node->childs.back();

    // First child is the let in the statement, and first child of this is the loop node
    auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
    Ast::removeFromParent(node->block);
    Ast::addChildBack(loopNode->block, node->block);
    SWAG_ASSERT(node->block);
    Ast::visit(node->block, [&](AstNode* x) {
        if (!x->ownerBreakable)
            x->ownerBreakable = loopNode; });
    node->block->flags &= ~AST_NO_SEMANTIC;
    loopNode->block->token.endLocation = node->block->token.endLocation;

    // Re-root the parent scope of the user block so that it points to the scope of the loop block
    auto ownerScope = node->block->ownerScope;
    ownerScope->parentScope->removeChildNoLock(ownerScope);
    ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;
    ownerScope->parentScope->addChildNoLock(ownerScope);

    job->nodes.pop_back();
    job->nodes.push_back(newExpression);
    job->nodes.push_back(node);

    return true;
}

bool SemanticJob::resolveGetErr(SemanticContext* context)
{
    auto node         = context->node;
    node->typeInfo    = g_TypeMgr.typeInfoString;
    node->byteCodeFct = ByteCodeGenJob::emitGetErr;
    return true;
}

bool SemanticJob::resolveIndex(SemanticContext* context)
{
    auto node = context->node;

    auto ownerBreakable = node->ownerBreakable;
    while (ownerBreakable && !(ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX))
        ownerBreakable = ownerBreakable->ownerBreakable;
    SWAG_VERIFY(ownerBreakable, context->report({node, node->token, Msg0630}));

    ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;

    // Take the type from the expression
    if (ownerBreakable->kind == AstNodeKind::Loop)
    {
        auto loopNode = CastAst<AstLoop>(ownerBreakable, AstNodeKind::Loop);
        if (loopNode->expression && loopNode->expression->typeInfo->flags & TYPEINFO_INTEGER)
            node->typeInfo = loopNode->expression->typeInfo;
    }

    if (!node->typeInfo)
        node->typeInfo = g_TypeMgr.typeInfoUInt;
    node->byteCodeFct = ByteCodeGenJob::emitIndex;
    return true;
}

bool SemanticJob::resolveBreak(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Break);

    // Label has been defined : search the corresponding LabelBreakable node
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::LabelBreakable || breakable->token.text != node->label))
            breakable = breakable->ownerBreakable;
        SWAG_VERIFY(breakable, context->report({node, format(Msg0631, node->label.c_str())}));
        node->ownerBreakable = breakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, Msg0632}));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveFallThrough(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->ownerBreakable && node->ownerBreakable->kind == AstNodeKind::Switch, context->report({node, node->token, Msg0633}));
    node->ownerBreakable->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::SwitchCase && parent != node->ownerBreakable)
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->kind == AstNodeKind::SwitchCase, context->report({node, node->token, Msg0634}));
    node->switchCase = CastAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);

    // 'fallthrough' cannot be used on the last case, this has no sens
    auto switchBlock = CastAst<AstSwitch>(node->ownerBreakable, AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report({node, node->token, Msg0635}));

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitFallThrough;

    return true;
}

bool SemanticJob::resolveContinue(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Continue);

    // Label has been defined : search the corresponding LabelBreakable node
    AstBreakable* lastBreakable = nullptr;
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::LabelBreakable || breakable->token.text != node->label))
        {
            if (breakable->kind != AstNodeKind::LabelBreakable)
                lastBreakable = breakable;
            breakable = breakable->ownerBreakable;
        }

        SWAG_VERIFY(breakable, context->report({node, format(Msg0631, node->label.c_str())}));
        node->ownerBreakable = lastBreakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, Msg0637}));
    SWAG_VERIFY(node->ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE, context->report({node, node->token, Msg0637}));
    node->ownerBreakable->continueList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitContinue;
    return true;
}

bool SemanticJob::resolveLabel(SemanticContext* context)
{
    // Be sure we don't have ghosting (the same label in a parent)
    auto node  = CastAst<AstLabelBreakable>(context->node, AstNodeKind::LabelBreakable);
    auto check = node->parent;
    while (check)
    {
        if (check->kind == AstNodeKind::LabelBreakable)
        {
            if (check->token.text == node->token.text)
            {
                Diagnostic diag(node, node->token, format(Msg0639, node->token.text.c_str()));
                Diagnostic note(check, check->token, Msg0884, DiagnosticLevel::Note);
                context->report(diag, &note);
            }
        }

        check = check->parent;
    }

    node->block->allocateExtension();
    node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitLabelBeforeBlock;
    node->block->extension->byteCodeAfterFct  = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}