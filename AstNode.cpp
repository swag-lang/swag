#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "Allocator.h"
#include "Generic.h"

bool AstNode::isConstant0()
{
    SWAG_ASSERT(typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return false;
    if (!(flags & AST_VALUE_COMPUTED))
        return false;

    switch (typeInfo->sizeOf)
    {
    case 1:
        return computedValue.reg.u8 == 0;
    case 2:
        return computedValue.reg.u16 == 0;
    case 4:
        return computedValue.reg.u32 == 0;
    case 8:
        return computedValue.reg.u64 == 0;
    }

    return false;
}

bool AstNode::isConstant1()
{
    SWAG_ASSERT(typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return false;
    if (!(flags & AST_VALUE_COMPUTED))
        return false;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        return computedValue.reg.u8 == 1;
    case NativeTypeKind::U16:
    case NativeTypeKind::S16:
        return computedValue.reg.u16 == 1;
    case NativeTypeKind::U32:
    case NativeTypeKind::S32:
    case NativeTypeKind::Char:
        return computedValue.reg.u32 == 1;
    case NativeTypeKind::U64:
    case NativeTypeKind::S64:
        return computedValue.reg.u64 == 1;
    case NativeTypeKind::F32:
        return computedValue.reg.f32 == 1;
    case NativeTypeKind::F64:
        return computedValue.reg.f64 == 1;
    }

    return false;
}

void AstNode::setPassThrough()
{
    semanticAfterFct  = nullptr;
    semanticBeforeFct = nullptr;
    semanticFct       = nullptr;
    byteCodeAfterFct  = nullptr;
    byteCodeBeforeFct = nullptr;
    byteCodeFct       = ByteCodeGenJob::emitPassThrough;
}

bool AstNode::isSameStackFrame(SymbolOverload* overload)
{
    if (overload->symbol->kind != SymbolKind::Variable)
        return true;
    if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        return true;
    if (!(overload->flags & OVERLOAD_VAR_FUNC_PARAM) && !(overload->flags & OVERLOAD_VAR_LOCAL))
        return true;

    auto nodeVar = overload->node;
    if ((flags & AST_RUN_BLOCK) != (nodeVar->flags & AST_RUN_BLOCK))
        return false;
    if (ownerFct != nodeVar->ownerFct)
        return false;

    return true;
}

void AstNode::inheritLocationFromChilds()
{
    if (childs.empty())
        return;
    for (auto child : childs)
        child->inheritLocationFromChilds();

    auto front = childs.front();
    if (token.startLocation.column == 0 && token.startLocation.line == 0)
        token.startLocation = front->token.startLocation;
    if (token.startLocation.line != front->token.endLocation.line || token.startLocation.column > front->token.endLocation.column)
        token.startLocation = front->token.startLocation;

    auto back = childs.back();
    if (token.endLocation.line != back->token.endLocation.line || token.endLocation.column < back->token.endLocation.column)
        token.endLocation = back->token.endLocation;
}

Utf8 AstNode::computeScopedName()
{
    auto& fullName = ownerScope->getFullName();
    if (fullName.empty())
        return name;
    return fullName + "." + name.c_str();
}

Utf8 AstNode::getKindName(AstNode* node)
{
    Utf8 result = getNakedKindName(node);
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::Namespace:
    case AstNodeKind::Alias:
    case AstNodeKind::FuncDeclParam:
    case AstNodeKind::StructDecl:
        return "a " + result;
    case AstNodeKind::EnumDecl:
    case AstNodeKind::EnumValue:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::Impl:
    case AstNodeKind::AttrDecl:
        return "an " + result;
    }

    return "<node>";
}

Utf8 AstNode::getNakedKindName(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
        if (node->ownerScope && node->ownerScope->isGlobal())
            return "global variable";
        if (node->ownerMainNode && node->ownerMainNode->kind == AstNodeKind::StructDecl)
            return "struct member";
        return "variable";
    case AstNodeKind::ConstDecl:
        return "constant";
    case AstNodeKind::FuncDecl:
        return "function";
    case AstNodeKind::AttrDecl:
        return "attribute declaration";
    case AstNodeKind::EnumDecl:
        return "enum";
    case AstNodeKind::EnumValue:
        return "enum value";
    case AstNodeKind::Namespace:
        return "namespace";
    case AstNodeKind::Alias:
        return "alias";
    case AstNodeKind::FuncDeclParam:
        return "parameter";
    case AstNodeKind::StructDecl:
        return "structure";
    case AstNodeKind::InterfaceDecl:
        return "interface";
    case AstNodeKind::Impl:
        return "implementation block";
    }

    return "<node>";
}

AstNode* AstNode::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstNode>();

    if (flags & AST_NEED_SCOPE)
    {
        auto cloneContext        = context;
        cloneContext.parentScope = Ast::newScope(newNode, newNode->name, ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
        newNode->copyFrom(cloneContext, this);
    }
    else
    {
        newNode->copyFrom(context, this);
    }

    return newNode;
}

AstNode* AstNode::findChildRef(AstNode* ref, AstNode* fromChild)
{
    if (!ref)
        return nullptr;
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i] == ref)
            return fromChild->childs[i];
    }

    return nullptr;
}

AstNode* AstNode::findChildRefRec(AstNode* ref, AstNode* fromChild)
{
    if (!ref)
        return nullptr;
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i] == ref)
            return fromChild->childs[i];

        if (childs[i]->childs.count == fromChild->childs[i]->childs.count)
        {
            auto result = childs[i]->findChildRefRec(ref, fromChild->childs[i]);
            if (result)
                return result;
        }
    }

    return nullptr;
}

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneHie)
{
    kind  = from->kind;
    flags = from->flags;
    flags &= ~AST_IS_GENERIC;
    flags |= context.forceFlags;

    ownerStructScope = context.ownerStructScope ? context.ownerStructScope : from->ownerStructScope;
    ownerMainNode    = context.ownerMainNode ? context.ownerMainNode : from->ownerMainNode;
    ownerScope       = context.parentScope ? context.parentScope : from->ownerScope;
    ownerBreakable   = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable;
    ownerInline      = context.ownerInline ? context.ownerInline : from->ownerInline;
    ownerFct         = context.ownerFct ? context.ownerFct : from->ownerFct;

    // Replace a type by another one during generic instantiation
    typeInfo = Generic::doTypeSubstitution(context, from->typeInfo);
    if (typeInfo != from->typeInfo)
        flags |= AST_FROM_GENERIC;

    // This should not be copied. If will be recomputed if necessary.
    // This can cause some problems with inline functions and autocast, as inline functions are evaluated
    // as functions, and also each time they are inlined.
    //castedTypeInfo = from->castedTypeInfo;

    resolvedSymbolName     = from->resolvedSymbolName;
    resolvedSymbolOverload = from->resolvedSymbolOverload;

    parentAttributes = from->parentAttributes;
    parentAttributes = from->parentAttributes;
    attributeFlags   = from->attributeFlags | context.forceAttributeFlags;
    token            = from->token;

    semanticFct       = from->semanticFct;
    semanticBeforeFct = from->semanticBeforeFct;
    semanticAfterFct  = from->semanticAfterFct;
    byteCodeFct       = from->byteCodeFct;
    byteCodeBeforeFct = from->byteCodeBeforeFct;
    byteCodeAfterFct  = from->byteCodeAfterFct;

    computedValue        = from->computedValue;
    name                 = from->name;
    sourceFile           = from->sourceFile;
    bc                   = from->bc;
    resultRegisterRC     = from->resultRegisterRC;
    fctCallStorageOffset = from->fctCallStorageOffset;
    castOffset           = from->castOffset;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneHie)
    {
        cloneChilds(context, from);

        // Force semantic on specific nodes on generic instantiation
        if ((from->flags & AST_IS_GENERIC) && (from->flags & AST_SEMANTIC_ON_CLONE))
        {
            for (auto one : childs)
                one->flags &= ~AST_NO_SEMANTIC;
        }
    }
}

void AstNode::cloneChilds(CloneContext& context, AstNode* from)
{
    auto oldParent = context.parent;
    context.parent = this;
    auto num       = from->childs.size();
    for (int i = 0; i < num; i++)
        from->childs[i]->clone(context);
    context.parent = oldParent;
}

AstNode* AstVarDecl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstVarDecl>();
    newNode->copyFrom(context, this);
    newNode->publicName  = publicName;
    newNode->constAssign = constAssign;

    newNode->type       = (AstTypeExpression*) findChildRef(type, newNode);
    newNode->assignment = findChildRef(assignment, newNode);

    // Is there an alias ?
    auto it = context.replaceNames.find(newNode->name);
    if (it != context.replaceNames.end())
        newNode->name = it->second;

    return newNode;
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstIdentifierRef>();
    newNode->copyFrom(context, this);
    return newNode;
}

void AstIdentifierRef::computeName()
{
    name.clear();
    for (auto child : childs)
    {
        if (!name.empty())
            name += ".";
        name += child->name;
    }

    name.computeCrc();
}

AstNode* AstIdentifier::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstIdentifier>();
    newNode->copyFrom(context, this);

    auto idRef = context.parent;
    while (idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->parent;
    newNode->identifierRef     = CastAst<AstIdentifierRef>(idRef, AstNodeKind::IdentifierRef);
    newNode->callParameters    = findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);
    newNode->aliasNames        = aliasNames;

    // Check if we need to replace the name with a type substitution
    // That way the new resolveIdentifier will just try to keep the typeinfo
    auto it = context.replaceTypes.find(newNode->name);
    if (it != context.replaceTypes.end())
    {
        if (!it->second->isNative(NativeTypeKind::Undefined))
            newNode->name = it->second->name;
        newNode->typeInfo = it->second;
        if (newNode->typeInfo->declNode)
        {
            newNode->resolvedSymbolName     = newNode->typeInfo->declNode->resolvedSymbolName;
            newNode->resolvedSymbolOverload = newNode->typeInfo->declNode->resolvedSymbolOverload;
        }

        newNode->flags |= AST_FROM_GENERIC_REPLACE;
        newNode->flags |= AST_FROM_GENERIC;
    }

    return newNode;
}

Utf8 AstFuncDecl::getNameForMessage()
{
    if (flags & AST_SPECIAL_COMPILER_FUNC)
        return format("'%s' block", token.text.c_str());
    return format("function '%s'", name.c_str());
}

void AstFuncDecl::computeFullNameForeign(bool forExport)
{
    scoped_lock lk(mutex);
    if (!fullnameForeign.empty())
        return;

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
    if (!forExport)
    {
        ComputedValue value;
        if (typeFunc->attributes.getValue("swag.foreign", "function", value) && !value.text.empty())
            fullnameForeign = value.text;
        else
            fullnameForeign = name;
        return;
    }

    SWAG_ASSERT(ownerScope);

    auto nameForeign = computeScopedName();

    // If the symbol has overload, i.e. more than one definition, then we
    // append the type
    if (resolvedSymbolName && resolvedSymbolName->overloads.size() > 1)
    {
        nameForeign += "@@";
        nameForeign += typeFunc->name;
    }

    fullnameForeign = nameForeign;

    // Normalize name
    auto len = nameForeign.length();
    auto pz  = nameForeign.buffer;
    auto pzd = fullnameForeign.buffer;
    for (int i = 0; i < len; i++)
    {
        if (pz[0] == '-' && pz[1] == '>')
            break;

        if (*pz == ' ')
        {
            pz++;
        }
        else if (*pz == ',' || *pz == '\'')
        {
            *pzd++ = '@';
            pz++;
        }
        else
            *pzd++ = *pz++;
    }

    *pzd++ = 0;

    fullnameForeign.count = (uint32_t)(pzd - fullnameForeign.buffer);
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstFuncDecl>();

    newNode->copyFrom(context, this, false);
    newNode->endToken    = endToken;
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;

    auto cloneContext     = context;
    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto functionScope    = Ast::newScope(newNode, newNode->name, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = functionScope;

    cloneContext.parentScope   = functionScope;
    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;

    //cloneContext.parentScope = context.parentScope;
    newNode->returnType = returnType ? returnType->clone(cloneContext) : nullptr;
    if (newNode->returnType)
        newNode->returnType->ownerScope = context.parentScope ? context.parentScope : ownerScope;

    if (content)
    {
        auto bodyScope           = Ast::newScope(newNode, newNode->name, ScopeKind::FunctionBody, functionScope);
        cloneContext.parentScope = bodyScope;
        newNode->content         = content->clone(cloneContext);
        bodyScope->owner         = newNode->content;

        // We need to duplicate sub functions, and register the symbol in the new corresponding scope
        for (auto f : subFunctions)
        {
            scoped_lock lk(f->mutex);

            // A sub function node has the root of the file as parent, but has the correct scope. We need to find
            // the duplicated parent node that corresponds to the original one, in order to get the corresponding new
            // scope (if a sub function is declared inside an if statement scope for example, we need the duplicated
            // sub function to be registered in the corresponding new scope).
            auto newScopeNode = findChildRefRec(f->ownerScope->owner, newNode);
            SWAG_ASSERT(newScopeNode);
            auto subFuncScope = newScopeNode->ownerScope;

            cloneContext.parentScope = subFuncScope;
            cloneContext.parent      = sourceFile->astRoot;
            auto subFunc             = (AstFuncDecl*) f->clone(cloneContext);

            subFunc->typeInfo = subFunc->typeInfo->clone();
            subFunc->typeInfo->flags &= ~TYPEINFO_GENERIC;
            subFunc->typeInfo->declNode = subFunc;

            subFunc->doneFlags |= AST_DONE_FILE_JOB_PASS;
            subFunc->content->flags &= ~AST_NO_SEMANTIC;
            newNode->subFunctions.push_back(subFunc);
            subFunc->resolvedSymbolName     = subFuncScope->symTable.registerSymbolName(nullptr, subFunc, SymbolKind::Function);
            subFunc->resolvedSymbolOverload = nullptr;
        }
    }
    else
    {
        newNode->content = nullptr;
    }

    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstAttrDecl>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstAttrUse>();
    newNode->copyFrom(context, this);
    newNode->attributes = attributes;
    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstFuncCallParam>();
    newNode->copyFrom(context, this);

    newNode->namedParam         = namedParam;
    newNode->namedParamNode     = namedParamNode;
    newNode->resolvedParameter  = resolvedParameter;
    newNode->index              = index;
    newNode->mustSortParameters = mustSortParameters;
    return newNode;
}

AstNode* AstBinaryOpNode::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstBinaryOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstConditionalOpNode::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstConditionalOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstIf>();
    newNode->copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = findChildRef(ifBlock, newNode);
    newNode->elseBlock      = findChildRef(elseBlock, newNode);
    return newNode;
}

void AstBreakable::copyFrom(CloneContext& context, AstBreakable* from)
{
    AstNode::copyFrom(context, from, false);
    breakableFlags = from->breakableFlags;
    registerIndex  = from->registerIndex;
}

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    auto it = context.replaceTokens.find(token.id);
    if (it != context.replaceTokens.end())
    {
        CloneContext cloneContext = context;
        cloneContext.replaceTokens.clear();
        return it->second->clone(cloneContext);
    }

    auto newNode = g_Allocator.alloc0<AstBreakContinue>();
    newNode->copyFrom(context, this);
    newNode->label = label;

    if (context.ownerBreakable)
    {
        if (newNode->kind == AstNodeKind::Break)
            context.ownerBreakable->breakList.push_back(newNode);
        else if (newNode->kind == AstNodeKind::Continue)
            context.ownerBreakable->continueList.push_back(newNode);
    }

    return newNode;
}

AstNode* AstLabelBreakable::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstLabelBreakable>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstWhile>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstFor::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstFor>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstLoop::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstLoop>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->specificName = findChildRef(specificName, specificName);
    newNode->expression   = findChildRef(expression, newNode);
    newNode->block        = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstVisit::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstVisit>();
    newNode->copyFrom(context, this);

    newNode->extraNameToken   = extraNameToken;
    newNode->wantPointerToken = wantPointerToken;
    newNode->expression       = findChildRef(expression, newNode);
    newNode->block            = findChildRef(block, newNode);
    newNode->aliasNames       = aliasNames;
    newNode->wantPointer      = wantPointer;

    return newNode;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstSwitch>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->block      = findChildRef(block, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstSwitchCase>();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = CastAst<AstSwitch>(context.parent, AstNodeKind::Switch);
    newNode->isDefault   = isDefault;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstSwitchCaseBlock>();
    newNode->copyFrom(context, this);

    newNode->ownerCase = CastAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    newNode->isDefault = isDefault;
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstTypeExpression>();
    newNode->copyFrom(context, this);

    newNode->identifier  = findChildRef(identifier, newNode);
    newNode->literalType = literalType;
    newNode->ptrCount    = ptrCount;
    newNode->arrayDim    = arrayDim;
    newNode->typeFlags   = typeFlags;
    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstTypeLambda>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstArrayPointerSlicing::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstArrayPointerSlicing>();
    newNode->copyFrom(context, this);

    newNode->array      = findChildRef(array, newNode);
    newNode->lowerBound = findChildRef(lowerBound, newNode);
    newNode->upperBound = findChildRef(upperBound, newNode);
    return newNode;
}

AstNode* AstArrayPointerIndex::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstArrayPointerIndex>();
    newNode->copyFrom(context, this);

    newNode->array   = findChildRef(array, newNode);
    newNode->access  = findChildRef(access, newNode);
    newNode->isDeref = isDeref;
    return newNode;
}

AstNode* AstIntrinsicProp::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstIntrinsicProp>();
    newNode->copyFrom(context, this);

    newNode->typeOfAsType  = typeOfAsType;
    newNode->typeOfAsConst = typeOfAsConst;
    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstExpressionList>();
    newNode->copyFrom(context, this);
    newNode->forTuple = forTuple;
    return newNode;
}

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstStruct>();
    newNode->copyFrom(context, this, false);
    newNode->packing = packing;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->name, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    cloneContext.ownerMainNode    = newNode;

    newNode->scope = cloneContext.parentScope;
    newNode->alternativeScopes.push_back(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;

    if (newNode->typeInfo)
    {
        newNode->typeInfo = newNode->typeInfo->clone();
        newNode->typeInfo->flags &= ~TYPEINFO_GENERIC;
        newNode->typeInfo->declNode = newNode;
        newNode->typeInfo->forceComputeName();
    }

    return newNode;
}

AstNode* AstImpl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstImpl>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, newNode->name, ScopeKind::Impl, context.parentScope ? context.parentScope : ownerScope);
    SWAG_ASSERT(cloneContext.ownerStructScope); // Should be setup in generic instantiation
    cloneContext.ownerMainNode = newNode;
    newNode->scope             = cloneContext.parentScope;

    for (auto c : childs)
    {
        auto newChild = c->clone(cloneContext);

        if (newChild->kind == AstNodeKind::FuncDecl)
        {
            auto newFunc = CastAst<AstFuncDecl>(newChild, AstNodeKind::FuncDecl);

            // Resolution of 'impl' needs all functions to have their symbol registered in the 'impl' scope,
            // in order to be able to wait for the resolution of all function before solving the 'impl'
            // implementation.
            newNode->scope->symTable.registerSymbolName(nullptr, newFunc, SymbolKind::Function);

            // The function inside the implementation was generic, so be sure we can now solve
            // its content
            newFunc->content->flags &= ~AST_NO_SEMANTIC;

            // Be sure we have a specific no generic typeinfo
            SWAG_ASSERT(newFunc->typeInfo);
            newFunc->typeInfo = newFunc->typeInfo->clone();
            newFunc->typeInfo->flags &= ~TYPEINFO_GENERIC;
            newFunc->typeInfo->declNode = newFunc;
            newFunc->typeInfo->forceComputeName();
        }
    }

    newNode->identifier    = findChildRef(identifier, newNode);
    newNode->identifierFor = findChildRef(identifierFor, newNode);

    // Cloning an impl block should be called only for generic interface implementation
    SWAG_ASSERT(newNode->identifierFor);

    return newNode;
}

AstNode* AstEnum::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstEnum>();
    newNode->copyFrom(context, this, false);

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->name, ScopeKind::Enum, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    cloneContext.ownerMainNode    = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->scope = cloneContext.parentScope;

    SWAG_ASSERT(newNode->typeInfo);
    newNode->typeInfo = newNode->typeInfo->clone();
    newNode->typeInfo->flags &= ~TYPEINFO_GENERIC;
    newNode->typeInfo->declNode = newNode;

    return newNode;
}

AstNode* AstInit::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstInit>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstDrop::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstDrop>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstReturn>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstCompilerInline::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerInline>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);
    childs.back()->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerMacro::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerMacro>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    childs.back()->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerMixin::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerMixin>();
    newNode->copyFrom(context, this);
    newNode->replaceTokens = replaceTokens;
    return newNode;
}

AstNode* AstInline::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstInline>();
    newNode->copyFrom(context, this, false);
    newNode->func = func;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);

    newNode->scope = cloneContext.parentScope;
    func->content->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerIfBlock::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerIfBlock>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstCompilerAst::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerAst>();
    newNode->copyFrom(context, this);
    newNode->embeddedKind = embeddedKind;

    // If #ast has an embedded function, we need to restore the semantic pass on that function
    // content now
    if (newNode->childs.size() > 1)
    {
        auto func = CastAst<AstFuncDecl>(newNode->childs.front(), AstNodeKind::FuncDecl);
        func->flags &= ~AST_NO_SEMANTIC;
        func->content->flags &= ~AST_NO_SEMANTIC;
    }

    return newNode;
}

AstNode* AstCompilerRun::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc0<AstCompilerRun>();
    newNode->copyFrom(context, this);

    // If #run has an embedded function, we need to restore the semantic pass on that function
    // content now
    if (newNode->childs.size() > 1)
    {
        auto func = CastAst<AstFuncDecl>(newNode->childs.front(), AstNodeKind::FuncDecl);
        func->flags &= ~AST_NO_SEMANTIC;
        func->content->flags &= ~AST_NO_SEMANTIC;
    }

    return newNode;
}