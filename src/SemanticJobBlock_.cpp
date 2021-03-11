#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"

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
                    return context->report({fct, fct->token, format("not all control paths of %s return a value", fct->getNameForMessage().c_str())});
                return context->report({fct, fct->token, format("%s must return a value", fct->getNameForMessage().c_str())});
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
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->extension->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterExpr;
    node->postExpression->allocateExtension();
    node->postExpression->extension->byteCodeAfterFct = ByteCodeGenJob::emitForAfterPost;
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
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        for (auto switchCase : switchNode->cases)
        {
            switchCase->allocateExtension();
            switchCase->extension->alternativeScopes.push_back(typeEnum->scope);
        }
    }

    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);

    // Deal with complete
    SWAG_CHECK(collectAttributes(context, node, nullptr));
    SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_COMPLETE) || node->expression, context->report({node, node->token, "a switch without an expression cannot be marked as 'swag.complete'"}));

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
    SWAG_VERIFY(!typeSwitch->isNative(NativeTypeKind::Any), context->report({node->expression, "invalid switch type 'any', you need to cast to a concrete type"}));

    // Collect constant expressions, to avoid double definitions
    set<uint64_t> val64;
    set<Utf8>     valText;
    for (auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                if (typeSwitch->isNative(NativeTypeKind::String))
                {
                    if (valText.find(expr->computedValue.text) != valText.end())
                        return context->report({expr, format("switch value '%s' already defined", expr->computedValue.text.c_str())});
                    valText.insert(expr->computedValue.text);
                }
                else
                {
                    if (val64.find(expr->computedValue.reg.u64) != val64.end())
                    {
                        if (typeSwitch->flags & TYPEINFO_INTEGER)
                            return context->report({expr, format("switch value '%d' already defined", expr->computedValue.reg.u64)});
                        return context->report({expr, format("switch value '%f' already defined", expr->computedValue.reg.f64)});
                    }

                    val64.insert(expr->computedValue.reg.u64);
                }
            }
            else if (node->attributeFlags & ATTRIBUTE_COMPLETE)
            {
                return context->report({expr, "expression cannot be evaluated at compile time, and switch is 'swag.complete'"});
            }
        }
    }

    // When a switch is marked as complete, be sure every definitions have been covered
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
    {
        if (node->typeInfo->kind != TypeInfoKind::Enum)
        {
            return context->report({node, format("'swag.complete' attribute on a switch can only be used for an enum type ('%s' provided)", node->typeInfo->name.c_str())});
        }

        auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        if (typeSwitch->isNative(NativeTypeKind::String))
        {
            if (valText.size() != typeEnum->values.size())
            {
                for (auto one : typeEnum->values)
                {
                    if (valText.find(one->value.text) == valText.end())
                        return context->report({node, node->token, format("switch is incomplete (missing '%s.%s')", typeEnum->name.c_str(), one->namedParam.c_str())});
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
                        return context->report({node, node->token, format("switch is incomplete (missing '%s.%s')", typeEnum->name.c_str(), one->namedParam.c_str())});
                }
            }
        }
    }

    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, oneExpression));
        if (context->result != ContextResult::Done)
            return true;
        if (node->ownerSwitch->expression)
            SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression, CASTFLAG_COMPARE));
        else
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, oneExpression->typeInfo, nullptr, oneExpression, CASTFLAG_COMPARE));
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
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    auto expression = node->expression;
    SWAG_CHECK(resolveIntrinsicCountOf(context, expression, expression->typeInfo));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, expression->typeInfo, nullptr, expression, CASTFLAG_COERCE_FULL));
    node->typeInfo = expression->typeInfo;

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

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->expression->allocateExtension();
    node->expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterExpr;
    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveVisit(SemanticContext* context)
{
    auto job        = context->job;
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstVisit>(context->node, AstNodeKind::Visit);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    // Struct type : convert to a opVisit call
    auto     typeInfo      = node->expression->typeInfo;
    AstNode* newExpression = nullptr;
    if (typeInfo->kind == TypeInfoKind::Struct || !node->extraNameToken.text.empty())
    {
        SWAG_VERIFY(!(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report({node, node->token, "cannot visit a tuple"}));
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

    Utf8 alias0Name = node->aliasNames.empty() ? Utf8("@alias0") : node->aliasNames[0];
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? Utf8("@alias1") : node->aliasNames[1];
    Utf8 content;

    // Get back the expression string
    auto& concat = context->job->tmpConcat;
    concat.init(1024);
    Ast::OutputContext outputContext;
    SWAG_CHECK(Ast::output(outputContext, concat, node->expression));
    concat.addU8(0);
    SWAG_ASSERT(concat.firstBucket->nextBucket == nullptr);

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
            content += format("var %s = cast(const &%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
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
            content += format("var %s = cast(const &%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
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
        SWAG_VERIFY(!node->wantPointer, context->report({node, node->token, "cannot visit a type variadic by pointer"}));
        content += format("var %s = %s[@index]; ", alias0Name.c_str(), (const char*) concat.firstBucket->datas);
        content += format("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    else
    {
        return context->report({node->expression, format("invalid type '%s' for visit", typeInfo->name.c_str())});
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
    node->block->ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;

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
    SWAG_VERIFY(ownerBreakable, context->report({node, node->token, "'@index' can only be used inside a breakable loop"}));

    ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;

    // Take the type from the expression
    if (ownerBreakable->kind == AstNodeKind::Loop)
    {
        auto loopNode = CastAst<AstLoop>(ownerBreakable, AstNodeKind::Loop);
        if (loopNode->expression->typeInfo->flags & TYPEINFO_INTEGER)
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
        SWAG_VERIFY(breakable, context->report({node, format("unknown label '%s'", node->label.c_str())}));
        node->ownerBreakable = breakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'break' can only be used inside a breakable block"}));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveFallThrough(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->ownerBreakable && node->ownerBreakable->kind == AstNodeKind::Switch, context->report({node, node->token, "'fallthrough' can only be used inside a 'switch' block"}));
    node->ownerBreakable->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::SwitchCase && parent != node->ownerBreakable)
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->kind == AstNodeKind::SwitchCase, context->report({node, node->token, "'fallthrough' can only be used inside a 'case' block"}));
    node->switchCase = CastAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);

    // 'fallthrough' cannot be used on the last case, this has no sens
    auto switchBlock = CastAst<AstSwitch>(node->ownerBreakable, AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report({node, node->token, "'fallthrough' cannot be used in the last 'case' of the swtich"}));

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

        SWAG_VERIFY(breakable, context->report({node, format("unknown label '%s'", node->label.c_str())}));
        node->ownerBreakable = lastBreakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'continue' can only be used inside a breakable loop"}));
    SWAG_VERIFY(node->ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE, context->report({node, node->token, "'continue' can only be used inside a breakable loop"}));
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
                Diagnostic diag(node, node->token, format("label name '%s' already defined in the hierarchy", node->token.text.c_str()));
                Diagnostic note(check, check->token, "this is the other definition", DiagnosticLevel::Note);
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