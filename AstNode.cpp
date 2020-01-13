#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "Allocator.h"
#include "Generic.h"

void AstNode::setPassThrough()
{
    semanticAfterFct  = nullptr;
    semanticBeforeFct = nullptr;
    semanticFct       = nullptr;
    byteCodeAfterFct  = nullptr;
    byteCodeBeforeFct = nullptr;
    byteCodeFct       = ByteCodeGenJob::emitPassThrough;
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

void AstNode::computeFullName()
{
    scoped_lock lk(mutex);
    SWAG_ASSERT(ownerScope);
    if (ownerScope->fullname.empty())
        fullnameDot = name;
    else
        fullnameDot = ownerScope->fullname + "." + name.c_str();
}

Utf8 AstNode::getKindName(AstNode* node)
{
    Utf8 result = getNakedKindName(node);
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::LetDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::Namespace:
    case AstNodeKind::TypeAlias:
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
    case AstNodeKind::LetDecl:
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
    case AstNodeKind::TypeAlias:
        return "typealias";
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
    auto newNode = g_Allocator.alloc<AstNode>();

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

    castedTypeInfo         = from->castedTypeInfo;
    resolvedSymbolName     = from->resolvedSymbolName;
    resolvedSymbolOverload = from->resolvedSymbolOverload;

    parentAttributes = from->parentAttributes;
    parentAttributes = from->parentAttributes;
    attributeFlags   = from->attributeFlags;
    token            = from->token;

    semanticFct       = from->semanticFct;
    semanticBeforeFct = from->semanticBeforeFct;
    semanticAfterFct  = from->semanticAfterFct;
    byteCodeFct       = from->byteCodeFct;
    byteCodeBeforeFct = from->byteCodeBeforeFct;
    byteCodeAfterFct  = from->byteCodeAfterFct;

    computedValue        = from->computedValue;
    name                 = from->name;
    fullnameDot          = from->fullnameDot;
    sourceFile           = from->sourceFile;
    bc                   = from->bc;
    resultRegisterRC     = from->resultRegisterRC;
    fctCallStorageOffset = from->fctCallStorageOffset;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneHie)
    {
        cloneChilds(context, from);
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
    auto newNode = g_Allocator.alloc<AstVarDecl>();
    newNode->copyFrom(context, this);

    newNode->type       = findChildRef(type, newNode);
    newNode->assignment = findChildRef(assignment, newNode);

    // Is there an alias ?
    auto it = context.replaceNames.find(newNode->name);
    if (it != context.replaceNames.end())
        newNode->name = it->second;

    return newNode;
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstIdentifierRef>();
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
    auto newNode = g_Allocator.alloc<AstIdentifier>();
    newNode->copyFrom(context, this);

    auto idRef = context.parent;
    while (idRef->kind != AstNodeKind::IdentifierRef)
        idRef = idRef->parent;
    newNode->identifierRef     = CastAst<AstIdentifierRef>(idRef, AstNodeKind::IdentifierRef);
    newNode->callParameters    = findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);
    newNode->aliasNames        = aliasNames;

    return newNode;
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstFuncDecl>();

    newNode->copyFrom(context, this, false);
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;

    auto cloneContext     = context;
    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto parentScope      = Ast::newScope(newNode, newNode->name, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = parentScope;

    cloneContext.parentScope   = parentScope;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;

    cloneContext.parentScope = context.parentScope;
    newNode->returnType      = returnType ? returnType->clone(cloneContext) : nullptr;

    cloneContext.parentScope = parentScope;
    newNode->content         = content ? content->clone(cloneContext) : nullptr;

    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstAttrDecl>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    auto newNode    = g_Allocator.alloc<AstAttrUse>();
    newNode->values = values;
    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstFuncCallParam>();
    newNode->copyFrom(context, this);

    newNode->namedParam         = namedParam;
    newNode->namedParamNode     = namedParamNode;
    newNode->resolvedParameter  = resolvedParameter;
    newNode->index              = index;
    newNode->mustSortParameters = mustSortParameters;
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstIf>();
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

    auto newNode = g_Allocator.alloc<AstBreakContinue>();
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
    auto newNode = g_Allocator.alloc<AstLabelBreakable>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstWhile>();
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
    auto newNode = g_Allocator.alloc<AstFor>();

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
    auto newNode = g_Allocator.alloc<AstLoop>();

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
    auto newNode = g_Allocator.alloc<AstVisit>();
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
    auto newNode = g_Allocator.alloc<AstSwitch>();
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
    auto newNode = g_Allocator.alloc<AstSwitchCase>();
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
    auto newNode = g_Allocator.alloc<AstSwitchCaseBlock>();
    newNode->copyFrom(context, this);

    newNode->ownerCase = CastAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    newNode->isDefault = isDefault;
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstTypeExpression>();
    newNode->copyFrom(context, this);

    newNode->identifier     = findChildRef(identifier, newNode);
    newNode->ptrCount       = ptrCount;
    newNode->arrayDim       = arrayDim;
    newNode->isSlice        = isSlice;
    newNode->isConst        = isConst;
    newNode->isCode         = isCode;
    newNode->forceConstType = forceConstType;
    newNode->isSelf         = isSelf;
    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstTypeLambda>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstPointerDeRef::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstPointerDeRef>();
    newNode->copyFrom(context, this);

    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    return newNode;
}

AstNode* AstProperty::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstProperty>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->prop       = prop;
    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstExpressionList>();
    newNode->copyFrom(context, this);

    newNode->storageOffset        = storageOffset;
    newNode->storageOffsetSegment = storageOffsetSegment;
    newNode->listKind             = listKind;

    return newNode;
}

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstStruct>();
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

    return newNode;
}

AstNode* AstImpl::clone(CloneContext& context)
{
    SWAG_ASSERT(false);
    return nullptr;
}

AstNode* AstInit::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstInit>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstDrop::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstDrop>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstReturn>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstCompilerInline::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstCompilerInline>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);
    childs.back()->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerMacro::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstCompilerMacro>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    childs.back()->clone(cloneContext);

    return newNode;
}

AstNode* AstCompilerMixin::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstCompilerMixin>();
    newNode->copyFrom(context, this);
    newNode->replaceTokens = replaceTokens;
    return newNode;
}

AstNode* AstInline::clone(CloneContext& context)
{
    auto newNode = g_Allocator.alloc<AstInline>();
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
    auto newNode = g_Allocator.alloc<AstCompilerIfBlock>();
    newNode->copyFrom(context, this);
    return newNode;
}