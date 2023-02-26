#include "pch.h"
#include "Ast.h"
#include "Generic.h"
#include "ErrorIds.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "ByteCode.h"

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneHie)
{
    kind = from->kind;

    flags = from->flags;
    flags &= ~AST_IS_GENERIC;
    flags |= context.forceFlags;
    flags &= ~context.removeFlags;

    // Copy some specific flags
    doneFlags |= from->doneFlags & AST_DONE_INLINED;
    doneFlags |= from->doneFlags & AST_DONE_CHECK_ATTR;
    doneFlags |= from->doneFlags & AST_DONE_STRUCT_CONVERT;
    doneFlags |= from->doneFlags & AST_DONE_CLOSURE_FIRST_PARAM;
    doneFlags |= from->doneFlags & AST_DONE_AST_BLOCK;
    doneFlags |= from->doneFlags & AST_DONE_SPEC_SCOPE;

    semFlags |= from->semFlags & AST_SEM_STRUCT_REGISTERED;
    semFlags |= from->semFlags & AST_SEM_SPEC_STACKSIZE;
    semFlags |= context.forceSemFlags;

    ownerStructScope     = context.ownerStructScope ? context.ownerStructScope : from->ownerStructScope;
    ownerScope           = context.parentScope ? context.parentScope : from->ownerScope;
    ownerBreakable       = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable;
    ownerInline          = context.ownerInline ? context.ownerInline : from->ownerInline;
    ownerFct             = (context.ownerFct || (context.cloneFlags & CLONE_FORCE_OWNER_FCT)) ? context.ownerFct : from->ownerFct;
    ownerCompilerIfBlock = context.ownerCompilerIfBlock ? context.ownerCompilerIfBlock : from->ownerCompilerIfBlock;

    // We do not want a defer statement to have some defers in the same scope, otherwise it's infinite
    if (context.ownerDeferScope)
    {
        context.ownerDeferScope->doneDefer.push_back(this);
        context.ownerDeferScope->doneDrop.push_back(this);
    }

    // Update direct node references
    for (const auto& p : context.nodeRefsToUpdate)
    {
        if (*p.ref == from)
        {
            *p.ref = this;
        }
    }

    if (context.ownerTryCatchAssume || (from->extension && from->extension->owner && from->extension->owner->ownerTryCatchAssume))
    {
        allocateExtension(ExtensionKind::Owner);
        extension->owner->ownerTryCatchAssume = context.ownerTryCatchAssume ? context.ownerTryCatchAssume : from->extension->owner->ownerTryCatchAssume;
    }

    // Replace a type by another one during generic instantiation
    typeInfo = Generic::doTypeSubstitution(context.replaceTypes, from->typeInfo);
    if (typeInfo != from->typeInfo)
        flags |= AST_FROM_GENERIC;

    // This should not be copied. It will be recomputed if necessary.
    // This can cause some problems with inline functions and autocast, as inline functions are evaluated
    // as functions, and also each time they are inlined.
    if (context.cloneFlags & CLONE_RAW)
        castedTypeInfo = from->castedTypeInfo;

    resolvedSymbolName     = from->resolvedSymbolName;
    resolvedSymbolOverload = from->resolvedSymbolOverload;

    token = from->token;
    if (context.forceLocation)
    {
        token.startLocation = context.forceLocation->startLocation;
        token.endLocation   = context.forceLocation->endLocation;
    }

    semanticFct = from->semanticFct;
    byteCodeFct = from->byteCodeFct;
    if (from->extension)
    {
        if (from->extMisc())
        {
            allocateExtension(ExtensionKind::Misc);
            extMisc()->resolvedUserOpSymbolOverload = from->extMisc()->resolvedUserOpSymbolOverload;
            extMisc()->collectTypeInfo              = from->extMisc()->collectTypeInfo;
            extMisc()->exportNode                   = from->extMisc()->exportNode;
            extMisc()->castOffset                   = from->extMisc()->castOffset;
            extMisc()->castItf                      = from->extMisc()->castItf;
            extMisc()->stackOffset                  = from->extMisc()->stackOffset;
            extMisc()->anyTypeSegment               = from->extMisc()->anyTypeSegment;
            extMisc()->anyTypeOffset                = from->extMisc()->anyTypeOffset;
            extMisc()->alternativeScopes            = from->extMisc()->alternativeScopes;
            extMisc()->isNamed                      = from->extMisc()->isNamed;
        }

        if (from->extension->semantic)
        {
            allocateExtension(ExtensionKind::Semantic);
            extension->semantic->semanticBeforeFct = from->extension->semantic->semanticBeforeFct;
            extension->semantic->semanticAfterFct  = from->extension->semantic->semanticAfterFct;
        }

        if (from->extension->bytecode)
        {
            allocateExtension(ExtensionKind::ByteCode);
            extByteCode()->byteCodeBeforeFct = from->extension->bytecode->byteCodeBeforeFct;
            extByteCode()->byteCodeAfterFct  = from->extension->bytecode->byteCodeAfterFct;
        }
    }

    if (from->computedValue)
    {
        computedValue  = Allocator::alloc<ComputedValue>();
        *computedValue = *from->computedValue;

        // Update typeinfo pointer if it has changed
        if (flags & AST_VALUE_IS_TYPEINFO && typeInfo != from->typeInfo)
            computedValue->reg.pointer = (uint8_t*) typeInfo;
    }

    token.text = from->token.text;
    sourceFile = from->sourceFile;

    attributeFlags = from->attributeFlags;
    safetyOn       = from->safetyOn;
    safetyOff      = from->safetyOff;
    specFlags      = from->specFlags;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneHie)
    {
        cloneChilds(context, from);

        // Force semantic on specific nodes on generic instantiation
        if ((from->flags & AST_IS_GENERIC) && (from->semFlags & AST_SEM_ON_CLONE))
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
    {
        // Do not duplicate a struct if it's a child of something else (i.e. another struct), because
        // in case of generics, we do want the normal generic stuff to be done (cloning)
        if (from->childs[i]->kind != AstNodeKind::StructDecl)
            from->childs[i]->clone(context);
    }

    context.parent = oldParent;
}

void AstNode::releaseChilds()
{
    for (auto c : childs)
        c->release();
    childs.release();
}

void AstNode::release()
{
#ifdef SWAG_STATS
    g_Stats.releaseNodes++;
#endif

    if (flags & AST_NEED_SCOPE)
        ownerScope->release();

    if (extension)
    {
        if (extension->bytecode)
        {
            if (extension->bytecode->bc)
                extension->bytecode->bc->release();
            Allocator::free<AstNode::ExtensionByteCode>(extension->bytecode);
        }

        if (extension->semantic)
        {
            Allocator::free<AstNode::ExtensionSemantic>(extension->semantic);
        }

        if (extension->owner)
        {
            for (auto c : extension->owner->nodesToFree)
                c->release();
            Allocator::free<AstNode::ExtensionOwner>(extension->owner);
        }

        if (extMisc())
        {
            Allocator::free<AstNode::ExtensionMisc>(extMisc());
        }

        Allocator::free<AstNode::Extension>(extension);
    }

    // Prerelease, if we need to childs to be alive
    switch (kind)
    {
    case AstNodeKind::FuncDecl:
    {
        auto funcDecl = CastAst<AstFuncDecl>(this, AstNodeKind::FuncDecl);
        if (funcDecl->content)
            funcDecl->content->ownerScope->release();
        break;
    }
    }

    // Release childs first
    for (auto c : childs)
        c->release();

    // Then destruct node
    switch (kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDeclParam:
        Allocator::free<AstVarDecl>(this);
        break;
    case AstNodeKind::IdentifierRef:
        Allocator::free<AstIdentifierRef>(this);
        break;
    case AstNodeKind::Identifier:
    case AstNodeKind::FuncCall:
        Allocator::free<AstIdentifier>(this);
        break;
    case AstNodeKind::FuncDecl:
        Allocator::free<AstFuncDecl>(this);
        break;
    case AstNodeKind::AttrDecl:
        Allocator::free<AstAttrDecl>(this);
        break;
    case AstNodeKind::AttrUse:
        Allocator::free<AstAttrUse>(this);
        break;
    case AstNodeKind::FuncCallParam:
        Allocator::free<AstFuncCallParam>(this);
        break;
    case AstNodeKind::BinaryOp:
        Allocator::free<AstBinaryOpNode>(this);
        break;
    case AstNodeKind::ConditionalExpression:
        Allocator::free<AstConditionalOpNode>(this);
        break;
    case AstNodeKind::If:
    case AstNodeKind::CompilerIf:
        Allocator::free<AstIf>(this);
        break;
    case AstNodeKind::SubstBreakContinue:
        Allocator::free<AstSubstBreakContinue>(this);
        break;
    case AstNodeKind::Break:
    case AstNodeKind::Continue:
    case AstNodeKind::FallThrough:
        Allocator::free<AstBreakContinue>(this);
        break;
    case AstNodeKind::ScopeBreakable:
        Allocator::free<AstScopeBreakable>(this);
        break;
    case AstNodeKind::While:
        Allocator::free<AstWhile>(this);
        break;
    case AstNodeKind::For:
        Allocator::free<AstFor>(this);
        break;
    case AstNodeKind::Loop:
        Allocator::free<AstLoop>(this);
        break;
    case AstNodeKind::Visit:
        Allocator::free<AstVisit>(this);
        break;
    case AstNodeKind::Switch:
        Allocator::free<AstSwitch>(this);
        break;
    case AstNodeKind::SwitchCase:
        Allocator::free<AstSwitchCase>(this);
        break;
    case AstNodeKind::SwitchCaseBlock:
        Allocator::free<AstSwitchCaseBlock>(this);
        break;
    case AstNodeKind::TypeExpression:
        Allocator::free<AstTypeExpression>(this);
        break;
    case AstNodeKind::TypeLambda:
    case AstNodeKind::TypeClosure:
        Allocator::free<AstTypeLambda>(this);
        break;
    case AstNodeKind::ArrayPointerSlicing:
        Allocator::free<AstArrayPointerSlicing>(this);
        break;
    case AstNodeKind::ArrayPointerIndex:
        Allocator::free<AstArrayPointerIndex>(this);
        break;
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
        Allocator::free<AstStruct>(this);
        break;
    case AstNodeKind::Impl:
        Allocator::free<AstImpl>(this);
        break;
    case AstNodeKind::EnumDecl:
        Allocator::free<AstEnum>(this);
        break;
    case AstNodeKind::EnumValue:
        Allocator::free<AstEnumValue>(this);
        break;
    case AstNodeKind::Init:
        Allocator::free<AstInit>(this);
        break;
    case AstNodeKind::Drop:
    case AstNodeKind::PostCopy:
    case AstNodeKind::PostMove:
        Allocator::free<AstDropCopyMove>(this);
        break;
    case AstNodeKind::Return:
        Allocator::free<AstReturn>(this);
        break;
    case AstNodeKind::CompilerInline:
        Allocator::free<AstCompilerInline>(this);
        break;
    case AstNodeKind::CompilerMacro:
        Allocator::free<AstCompilerMacro>(this);
        break;
    case AstNodeKind::CompilerMixin:
        Allocator::free<AstCompilerMixin>(this);
        break;
    case AstNodeKind::Inline:
        Allocator::free<AstInline>(this);
        break;
    case AstNodeKind::CompilerIfBlock:
        Allocator::free<AstCompilerIfBlock>(this);
        break;
    case AstNodeKind::CompilerRun:
    case AstNodeKind::CompilerRunExpression:
    case AstNodeKind::CompilerValidIf:
    case AstNodeKind::CompilerValidIfx:
    case AstNodeKind::CompilerAst:
        Allocator::free<AstCompilerSpecFunc>(this);
        break;
    case AstNodeKind::Namespace:
        Allocator::free<AstNameSpace>(this);
        break;
    case AstNodeKind::Try:
    case AstNodeKind::Catch:
    case AstNodeKind::TryCatch:
    case AstNodeKind::Assume:
    case AstNodeKind::Throw:
        Allocator::free<AstTryCatchAssume>(this);
        break;
    case AstNodeKind::Alias:
        Allocator::free<AstAlias>(this);
        break;
    case AstNodeKind::Cast:
    case AstNodeKind::AutoCast:
        Allocator::free<AstCast>(this);
        break;
    case AstNodeKind::FuncCallParams:
        Allocator::free<AstFuncCallParams>(this);
        break;
    case AstNodeKind::Range:
        Allocator::free<AstRange>(this);
        break;
    case AstNodeKind::MakePointer:
    case AstNodeKind::MakePointerLambda:
        Allocator::free<AstMakePointer>(this);
        break;
    case AstNodeKind::AffectOp:
    case AstNodeKind::FactorOp:
        Allocator::free<AstOp>(this);
        break;
    case AstNodeKind::Defer:
        Allocator::free<AstDefer>(this);
        break;
    case AstNodeKind::ExpressionList:
        Allocator::free<AstExpressionList>(this);
        break;
    case AstNodeKind::With:
        Allocator::free<AstWith>(this);
        break;
    case AstNodeKind::Literal:
        Allocator::free<AstLiteral>(this);
        break;
    default:
        Allocator::free<AstNode>(this);
        break;
    }
}

AstNode* AstNode::clone(CloneContext& context)
{
    switch (kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDeclParam:
        return clone<AstVarDecl>(this, context);
    case AstNodeKind::IdentifierRef:
        return clone<AstIdentifierRef>(this, context);
    case AstNodeKind::Identifier:
    case AstNodeKind::FuncCall:
        return clone<AstIdentifier>(this, context);
    case AstNodeKind::FuncDecl:
        return clone<AstFuncDecl>(this, context);
    case AstNodeKind::AttrDecl:
        return clone<AstAttrDecl>(this, context);
    case AstNodeKind::AttrUse:
        return clone<AstAttrUse>(this, context);
    case AstNodeKind::FuncCallParam:
        return clone<AstFuncCallParam>(this, context);
    case AstNodeKind::BinaryOp:
        return clone<AstBinaryOpNode>(this, context);
    case AstNodeKind::ConditionalExpression:
        return clone<AstConditionalOpNode>(this, context);
    case AstNodeKind::If:
    case AstNodeKind::CompilerIf:
        return clone<AstIf>(this, context);
    case AstNodeKind::SubstBreakContinue:
        return clone<AstSubstBreakContinue>(this, context);
    case AstNodeKind::Break:
    case AstNodeKind::Continue:
    case AstNodeKind::FallThrough:
        return clone<AstBreakContinue>(this, context);
    case AstNodeKind::ScopeBreakable:
        return clone<AstScopeBreakable>(this, context);
    case AstNodeKind::While:
        return clone<AstWhile>(this, context);
    case AstNodeKind::For:
        return clone<AstFor>(this, context);
    case AstNodeKind::Loop:
        return clone<AstLoop>(this, context);
    case AstNodeKind::Visit:
        return clone<AstVisit>(this, context);
    case AstNodeKind::Switch:
        return clone<AstSwitch>(this, context);
    case AstNodeKind::SwitchCase:
        return clone<AstSwitchCase>(this, context);
    case AstNodeKind::SwitchCaseBlock:
        return clone<AstSwitchCaseBlock>(this, context);
    case AstNodeKind::TypeExpression:
        return clone<AstTypeExpression>(this, context);
    case AstNodeKind::TypeLambda:
    case AstNodeKind::TypeClosure:
        return clone<AstTypeLambda>(this, context);
    case AstNodeKind::ArrayPointerSlicing:
        return clone<AstArrayPointerSlicing>(this, context);
    case AstNodeKind::ArrayPointerIndex:
        return clone<AstArrayPointerIndex>(this, context);
    case AstNodeKind::StructDecl:
    case AstNodeKind::InterfaceDecl:
        return clone<AstStruct>(this, context);
    case AstNodeKind::Impl:
        return clone<AstImpl>(this, context);
    case AstNodeKind::EnumDecl:
        return clone<AstEnum>(this, context);
    case AstNodeKind::EnumValue:
        return clone<AstEnumValue>(this, context);
    case AstNodeKind::Init:
        return clone<AstInit>(this, context);
    case AstNodeKind::Drop:
    case AstNodeKind::PostCopy:
    case AstNodeKind::PostMove:
        return clone<AstDropCopyMove>(this, context);
    case AstNodeKind::Return:
        return clone<AstReturn>(this, context);
    case AstNodeKind::CompilerInline:
        return clone<AstCompilerInline>(this, context);
    case AstNodeKind::CompilerMacro:
        return clone<AstCompilerMacro>(this, context);
    case AstNodeKind::CompilerMixin:
        return clone<AstCompilerMixin>(this, context);
    case AstNodeKind::Inline:
        return clone<AstInline>(this, context);
    case AstNodeKind::CompilerIfBlock:
        return clone<AstCompilerIfBlock>(this, context);
    case AstNodeKind::CompilerRun:
    case AstNodeKind::CompilerRunExpression:
    case AstNodeKind::CompilerValidIf:
    case AstNodeKind::CompilerValidIfx:
    case AstNodeKind::CompilerAst:
        return clone<AstCompilerSpecFunc>(this, context);
    case AstNodeKind::Namespace:
        return clone<AstNameSpace>(this, context);
    case AstNodeKind::Try:
    case AstNodeKind::Catch:
    case AstNodeKind::TryCatch:
    case AstNodeKind::Assume:
    case AstNodeKind::Throw:
        return clone<AstTryCatchAssume>(this, context);
    case AstNodeKind::Alias:
        return clone<AstAlias>(this, context);
    case AstNodeKind::Cast:
    case AstNodeKind::AutoCast:
        return clone<AstCast>(this, context);
    case AstNodeKind::FuncCallParams:
        return clone<AstFuncCallParams>(this, context);
    case AstNodeKind::Range:
        return clone<AstRange>(this, context);
    case AstNodeKind::MakePointer:
    case AstNodeKind::MakePointerLambda:
        return clone<AstMakePointer>(this, context);
    case AstNodeKind::AffectOp:
    case AstNodeKind::FactorOp:
        return clone<AstOp>(this, context);
    case AstNodeKind::Defer:
        return clone<AstDefer>(this, context);
    case AstNodeKind::ExpressionList:
        return clone<AstExpressionList>(this, context);
    case AstNodeKind::With:
        return clone<AstWith>(this, context);
    case AstNodeKind::Literal:
        return clone<AstLiteral>(this, context);

    default:
    {
        auto newNode = Ast::newNode<AstNode>();
        if (flags & AST_NEED_SCOPE)
        {
            auto cloneContext        = context;
            cloneContext.parentScope = Ast::newScope(newNode, newNode->token.text, ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);

            // We need to register subdecls
            // All of this is a hack, not cool
            if (cloneContext.forceFlags & AST_IN_MIXIN)
                cloneContext.parentScope->symTable.mapNames.clone(&ownerScope->symTable.mapNames);

            newNode->copyFrom(cloneContext, this);
            context.propageResult(cloneContext);
        }
        else
            newNode->copyFrom(context, this);
        return newNode;
    }
    }
}

AstNode* AstVarDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstVarDecl>();
    newNode->copyFrom(context, this);
    newNode->publicName  = publicName;
    newNode->attributes  = attributes;
    newNode->assignToken = assignToken;
    if (newNode->attrUse)
        newNode->attrUse->content = newNode;

    newNode->type             = (AstTypeExpression*) findChildRef(type, newNode);
    newNode->assignment       = findChildRef(assignment, newNode);
    newNode->genTypeComesFrom = genTypeComesFrom;

    newNode->typeConstraint = findChildRef(typeConstraint, newNode);
    if (newNode->typeConstraint)
        newNode->typeConstraint->flags &= ~AST_NO_SEMANTIC;

    // Is there an alias ?
    auto it = context.replaceNames.find(newNode->token.text);
    if (it != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(it->second);
        newNode->token.text = it->second;
    }

    return newNode;
}

AstNode* AstIdentifierRef::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIdentifierRef>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstIdentifierRef* AstIdentifier::identifierRef()
{
    if (parent->kind == AstNodeKind::IdentifierRef)
        return CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);

    auto check = parent;
    while (check->kind != AstNodeKind::IdentifierRef)
    {
        check = check->parent;
        SWAG_ASSERT(check);
    }

    return CastAst<AstIdentifierRef>(check, AstNodeKind::IdentifierRef);
}

void AstIdentifier::allocateIdentifierExtension()
{
    if (identifierExtension)
        return;
    identifierExtension = Allocator::alloc<AstIdentifierExtension>();
}

AstIdentifier::~AstIdentifier()
{
    if (identifierExtension)
        Allocator::free<AstIdentifierExtension>(identifierExtension);
}

AstNode* AstIdentifier::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIdentifier>();
    newNode->copyFrom(context, this);

    // Is there an alias ?
    auto itn = context.replaceNames.find(newNode->token.text);
    if (itn != context.replaceNames.end())
    {
        context.usedReplaceNames.insert(itn->second);
        newNode->token.text = itn->second;
    }

    newNode->callParameters    = (AstFuncCallParams*) findChildRef(callParameters, newNode);
    newNode->genericParameters = findChildRef(genericParameters, newNode);

    if (identifierExtension)
    {
        newNode->allocateIdentifierExtension();
        newNode->identifierExtension->aliasNames   = identifierExtension->aliasNames;
        newNode->identifierExtension->scopeUpMode  = identifierExtension->scopeUpMode;
        newNode->identifierExtension->scopeUpValue = identifierExtension->scopeUpValue;
    }

    // Check if we need to replace the token.text with a type substitution
    // That way the new resolveIdentifier will just try to keep the typeinfo
    auto it = context.replaceTypes.find(newNode->token.text);
    if (it != context.replaceTypes.end())
    {
        // :ForLocationInValidIf
        auto it1 = context.replaceTypesFrom.find(newNode->token.text);
        if (it1 != context.replaceTypesFrom.end())
        {
            newNode->allocateIdentifierExtension();
            newNode->identifierExtension->fromAlternateVar = it1->second;
        }

        if (!it->second->isNative(NativeTypeKind::Undefined))
            newNode->token.text = it->second->name;

        newNode->typeInfo = it->second;
        if (newNode->typeInfo->declNode)
        {
            newNode->resolvedSymbolName     = newNode->typeInfo->declNode->resolvedSymbolName;
            newNode->resolvedSymbolOverload = newNode->typeInfo->declNode->resolvedSymbolOverload;
        }

        newNode->flags |= AST_FROM_GENERIC | AST_FROM_GENERIC_REPLACE;
    }

    return newNode;
}

bool AstFuncDecl::cloneSubDecls(ErrorContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode)
{
    // We need to duplicate sub declarations, and register the symbol in the new corresponding scope
    for (auto f : subDecls)
    {
        ScopedLock lk(f->mutex);

        // A sub declaration node has the root of the file as parent, but has the correct scope. We need to find
        // the duplicated parent node that corresponds to the original one, in order to get the corresponding new
        // scope (if a sub declaration is declared inside an if statement scope for example, we need the duplicated
        // sub declaration to be registered in the corresponding new scope).
        auto newScopeNode = oldOwnerNode->findChildRefRec(f->ownerScope->owner, refNode);
        SWAG_ASSERT(newScopeNode);
        auto subFuncScope = newScopeNode->ownerScope;

        cloneContext.parentScope = subFuncScope;
        cloneContext.parent      = nullptr; // Register in parent will be done at the end (race condition)
        if (f->parent->kind == AstNodeKind::AttrUse)
            f = f->parent;
        auto subF         = f->clone(cloneContext);
        auto subDecl      = subF->kind == AstNodeKind::AttrUse ? ((AstAttrUse*) subF)->content : subF;
        subDecl->typeInfo = subDecl->typeInfo->clone();

        SymbolKind symKind = SymbolKind::Invalid;
        switch (subDecl->kind)
        {
        case AstNodeKind::FuncDecl:
        {
            auto nodeFunc = CastAst<AstFuncDecl>(subDecl, AstNodeKind::FuncDecl);
            nodeFunc->content->flags &= ~AST_NO_SEMANTIC;
            if (cloneContext.alternativeScope)
                nodeFunc->addAlternativeScope(cloneContext.alternativeScope);
            symKind = SymbolKind::Function;

            // Function is supposed to start semantic when captured parameters have been evaluated
            // So no semantic on it now
            if (nodeFunc->captureParameters)
                nodeFunc->flags |= AST_NO_SEMANTIC | AST_SPEC_SEMANTIC1;

            break;
        }
        case AstNodeKind::StructDecl:
        {
            symKind           = SymbolKind::Struct;
            auto nodeStruct   = CastAst<AstStruct>(subDecl, AstNodeKind::StructDecl);
            auto typeStruct   = CastTypeInfo<TypeInfoStruct>(subDecl->typeInfo, TypeInfoKind::Struct);
            typeStruct->scope = nodeStruct->scope;
            break;
        }
        case AstNodeKind::InterfaceDecl:
            symKind = SymbolKind::Interface;
            break;

        default:
            SWAG_ASSERT(false);
            break;
        }

        subDecl->typeInfo->removeGenericFlag();
        subDecl->typeInfo->declNode = subDecl;

        subDecl->doneFlags |= AST_DONE_FILE_JOB_PASS;
        newFctNode->subDecls.push_back(subDecl);

        // Be sure symbol is not already there. This can happen when using mixins
        if (context)
        {
            auto sym = subFuncScope->symTable.find(subDecl->token.text);
            if (sym)
            {
                Diagnostic diag{subDecl, Fmt(Err(Err0346), subDecl->token.ctext())};
                return context->report(diag);
            }
        }

        // We need to add the parent scope of the inline function (the global one), in order for
        // the inline content to be resolved in the same context as the original function
        auto globalScope = f->ownerScope;
        while (!globalScope->isGlobalOrImpl())
            globalScope = globalScope->parentScope;
        subDecl->addAlternativeScope(globalScope);

        subDecl->resolvedSymbolName     = subFuncScope->symTable.registerSymbolName(nullptr, subDecl, symKind);
        subDecl->resolvedSymbolOverload = nullptr;

        // Do it last to avoid a race condition with the file job
        Ast::addChildBack(f->parent, subF);
    }

    return true;
}

AstFuncDecl::~AstFuncDecl()
{
    if (scope)
        scope->release();
}

AstNode* AstFuncDecl::clone(CloneContext& context)
{
    auto newNode      = Ast::newNode<AstFuncDecl>();
    auto cloneContext = context;
    cloneContext.forceSemFlags &= ~AST_SEM_SPEC_STACKSIZE;

    newNode->copyFrom(context, this, false);
    newNode->aliasMask   = aliasMask;
    newNode->stackSize   = stackSize;
    newNode->methodParam = methodParam;
    newNode->tokenName   = tokenName;

    newNode->specFlags &= ~(AST_SPEC_FUNCDECL_FULL_RESOLVE | AST_SPEC_FUNCDECL_PARTIAL_RESOLVE);

    cloneContext.ownerFct = newNode;
    cloneContext.parent   = newNode;
    auto functionScope    = Ast::newScope(newNode, newNode->token.text, ScopeKind::Function, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope        = functionScope;

    cloneContext.parentScope = functionScope;

    // Capture parameters are not part of the func hierarchy
    newNode->captureParameters = captureParameters ? captureParameters->clone(cloneContext) : nullptr;
    if (newNode->captureParameters)
        newNode->childs.pop_back();

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->parameters        = parameters ? parameters->clone(cloneContext) : nullptr;
    newNode->validif           = validif ? validif->clone(cloneContext) : nullptr;
    newNode->nodeCounts        = nodeCounts;
    newNode->makePointerLambda = makePointerLambda;

    if (makePointerLambda)
    {
        for (const auto& p : context.nodeRefsToUpdate)
        {
            if (p.node->kind == AstNodeKind::AffectOp)
            {
                auto affOp = CastAst<AstOp>(p.node, AstNodeKind::AffectOp);
                if (affOp->dependentLambda == newNode)
                {
                    newNode->makePointerLambda = CastAst<AstMakePointer>(affOp->childs.back(), AstNodeKind::MakePointerLambda);
                }
            }
        }
    }

    newNode->needRegisterGetContext = needRegisterGetContext;
    newNode->hasSpecMixin           = hasSpecMixin;

    newNode->returnType = returnType ? returnType->clone(cloneContext) : nullptr;
    if (newNode->returnType)
        newNode->returnType->ownerScope = context.parentScope ? context.parentScope : ownerScope;

    if (content)
    {
        auto bodyScope           = Ast::newScope(newNode, newNode->token.text, ScopeKind::FunctionBody, functionScope);
        cloneContext.parentScope = bodyScope;
        newNode->content         = content->clone(cloneContext);

        // Content can be an automatic try block in case of a throwable function
        // But we want the scope to have the function statement as an owner
        // :AutomaticTryContent
        if (newNode->content->kind == AstNodeKind::Try)
        {
            SWAG_ASSERT(newNode->content->childs.front()->kind == AstNodeKind::Statement || newNode->content->childs.front()->kind == AstNodeKind::Return);
            bodyScope->owner = newNode->content->childs.front();
        }
        else
            bodyScope->owner = newNode->content;

        cloneSubDecls(nullptr, cloneContext, this, newNode, newNode);
    }
    else
    {
        newNode->content = nullptr;
    }

    context.propageResult(cloneContext);
    return newNode;
}

AstNode* AstAttrDecl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAttrDecl>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstAttrUse::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAttrUse>();
    newNode->copyFrom(context, this);

    newNode->content = findChildRef(content, newNode);
    if (newNode->content)
        newNode->content->setOwnerAttrUse(newNode);

    return newNode;
}

AstNode* AstFuncCallParam::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFuncCallParam>();
    newNode->copyFrom(context, this);

    // :ReverseLiteralStruct
    // Order of childs is correct, so remove flag
    newNode->flags &= ~AST_REVERSE_SEMANTIC;

    newNode->resolvedParameter = resolvedParameter;
    newNode->indexParam        = indexParam;
    return newNode;
}

AstNode* AstBinaryOpNode::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstBinaryOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstConditionalOpNode::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstConditionalOpNode>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstIf::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstIf>();
    newNode->copyFrom(context, this);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->ifBlock        = (AstCompilerIfBlock*) findChildRef(ifBlock, newNode);
    newNode->elseBlock      = (AstCompilerIfBlock*) findChildRef(elseBlock, newNode);
    return newNode;
}

void AstBreakable::copyFrom(CloneContext& context, AstBreakable* from)
{
    AstNode::copyFrom(context, from, false);
    breakableFlags = from->breakableFlags;
    registerIndex  = from->registerIndex;
    registerIndex1 = from->registerIndex1;
}

AstNode* AstSubstBreakContinue::clone(CloneContext& context)
{
    if (altSubstBreakable == context.ownerBreakable)
        return altSubst->clone(context);
    return defaultSubst->clone(context);
}

AstNode* AstBreakContinue::clone(CloneContext& context)
{
    // Do the token replacement only if this is a break/continue in the original breakable,
    // not a 'new' breakable in the user code
    if (context.ownerBreakable == context.replaceTokensBreakable)
    {
        auto it = context.replaceTokens.find(token.id);
        if (it != context.replaceTokens.end())
        {
            auto newNode = Ast::newNode<AstSubstBreakContinue>(nullptr, AstNodeKind::SubstBreakContinue, sourceFile, context.parent);
            newNode->allocateExtension(ExtensionKind::Semantic);
            newNode->extension->semantic->semanticBeforeFct = SemanticJob::preResolveSubstBreakContinue;

            CloneContext cloneContext = context;
            cloneContext.replaceTokens.clear();
            cloneContext.parent = newNode;

            newNode->defaultSubst      = clone(cloneContext);
            newNode->altSubst          = it->second->clone(cloneContext);
            newNode->altSubstBreakable = context.ownerBreakable;

            context.propageResult(cloneContext);
            return newNode;
        }
    }

    auto newNode = Ast::newNode<AstBreakContinue>();
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

AstNode* AstScopeBreakable::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstScopeBreakable>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->block = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstWhile::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstWhile>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->block          = findChildRef(block, newNode);
    return newNode;
}

AstFor::~AstFor()
{
    ownerScope->release();
}

AstNode* AstFor::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFor>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->preExpression  = findChildRef(preExpression, newNode);
    newNode->boolExpression = findChildRef(boolExpression, newNode);
    newNode->postExpression = findChildRef(postExpression, newNode);
    newNode->block          = findChildRef(block, newNode);

    return newNode;
}

AstLoop::~AstLoop()
{
    ownerScope->release();
}

AstNode* AstLoop::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstLoop>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->AstBreakable::copyFrom(cloneContext, this);

    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->specificName = findChildRef(specificName, newNode);
    newNode->expression   = findChildRef(expression, newNode);
    newNode->block        = findChildRef(block, newNode);
    return newNode;
}

AstNode* AstVisit::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstVisit>();
    newNode->copyFrom(context, this);

    newNode->extraNameToken   = extraNameToken;
    newNode->wantPointerToken = wantPointerToken;
    newNode->expression       = findChildRef(expression, newNode);
    newNode->block            = findChildRef(block, newNode);
    newNode->aliasNames       = aliasNames;

    return newNode;
}

AstNode* AstSwitch::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitch>();
    newNode->AstBreakable::copyFrom(context, this);

    auto cloneContext           = context;
    cloneContext.ownerBreakable = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->expression = findChildRef(expression, newNode);
    for (auto expr : cases)
        newNode->cases.push_back((AstSwitchCase*) findChildRef(expr, newNode));
    return newNode;
}

AstNode* AstSwitchCase::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitchCase>();
    newNode->copyFrom(context, this);

    newNode->block       = findChildRef(block, newNode);
    newNode->ownerSwitch = CastAst<AstSwitch>(context.parent, AstNodeKind::Switch);
    newNode->caseIndex   = caseIndex;
    for (auto expr : expressions)
        newNode->expressions.push_back(findChildRef(expr, newNode));
    return newNode;
}

AstSwitchCaseBlock::~AstSwitchCaseBlock()
{
    ownerScope->release();
}

AstNode* AstSwitchCaseBlock::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstSwitchCaseBlock>();

    auto cloneContext        = context;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Statement, context.parentScope ? context.parentScope : ownerScope);
    newNode->copyFrom(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->ownerCase = CastAst<AstSwitchCase>(context.parent, AstNodeKind::SwitchCase);
    return newNode;
}

AstNode* AstTypeExpression::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTypeExpression>();
    newNode->copyFrom(context, this);

    newNode->identifier      = findChildRef(identifier, newNode);
    newNode->typeFromLiteral = typeFromLiteral;
    newNode->ptrCount        = ptrCount;
    newNode->literalType     = literalType;

    for (int i = 0; i < ptrCount; i++)
        newNode->ptrFlags[i] = ptrFlags[i];

    newNode->arrayDim  = arrayDim;
    newNode->typeFlags = typeFlags;

    return newNode;
}

AstNode* AstTypeLambda::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTypeLambda>();
    newNode->copyFrom(context, this);

    newNode->parameters = findChildRef(parameters, newNode);
    newNode->returnType = findChildRef(returnType, newNode);
    return newNode;
}

AstNode* AstArrayPointerSlicing::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstArrayPointerSlicing>();
    newNode->copyFrom(context, this);

    newNode->array      = findChildRef(array, newNode);
    newNode->lowerBound = findChildRef(lowerBound, newNode);
    newNode->upperBound = findChildRef(upperBound, newNode);
    for (auto f : structFlatParams)
        newNode->structFlatParams.push_back(findChildRef(f, newNode));

    return newNode;
}

AstNode* AstExpressionList::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstExpressionList>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstArrayPointerIndex::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstArrayPointerIndex>();
    newNode->copyFrom(context, this);

    newNode->array  = findChildRef(array, newNode);
    newNode->access = findChildRef(access, newNode);
    for (auto f : structFlatParams)
        newNode->structFlatParams.push_back(findChildRef(f, newNode));

    return newNode;
}

AstStruct::~AstStruct()
{
    if (scope)
        scope->release();
}

AstNode* AstStruct::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstStruct>();
    newNode->copyFrom(context, this, false);
    newNode->packing = packing;

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Struct, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;

    newNode->scope = cloneContext.parentScope;
    newNode->addAlternativeScope(scope);

    newNode->genericParameters = genericParameters ? genericParameters->clone(cloneContext) : nullptr;
    newNode->content           = content ? content->clone(cloneContext) : nullptr;
    newNode->validif           = validif ? validif->clone(cloneContext) : nullptr;
    newNode->flags |= AST_FROM_GENERIC;
    newNode->content->flags &= ~AST_NO_SEMANTIC;

    if (newNode->typeInfo)
    {
        newNode->typeInfo = newNode->typeInfo->clone();
        newNode->typeInfo->removeGenericFlag();
        newNode->typeInfo->declNode = newNode;
        newNode->typeInfo->forceComputeName();
    }

    context.propageResult(cloneContext);
    return newNode;
}

AstImpl::~AstImpl()
{
    if (scope)
        scope->release();
}

AstNode* AstImpl::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstImpl>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, newNode->token.text, ScopeKind::Impl, context.parentScope ? context.parentScope : ownerScope);
    SWAG_ASSERT(cloneContext.ownerStructScope); // Should be setup in generic instantiation
    newNode->scope = cloneContext.parentScope;

    // If this is an interface implementation block, we need to register a sub scope with the name
    // of the interface in the instantiated struct scope.
    // All following functions must be in that sub scope.
    // :SubScopeImplFor
    if (identifierFor)
    {
        auto baseScope           = cloneContext.ownerStructScope;
        auto itfName             = childs.front()->token.text;
        cloneContext.parentScope = Ast::newScope(newNode, itfName, ScopeKind::Impl, baseScope);
        newNode->scope           = cloneContext.parentScope;

        // :FakeImplForType
        auto implTypeInfo        = makeType<TypeInfoStruct>();
        implTypeInfo->name       = itfName;
        implTypeInfo->structName = implTypeInfo->name;
        implTypeInfo->scope      = newNode->scope;
        implTypeInfo->declNode   = newNode;

        baseScope->symTable.addSymbolTypeInfoNoLock(nullptr, newNode, implTypeInfo, SymbolKind::Struct, nullptr, OVERLOAD_IMPL_IN_STRUCT, nullptr, 0, nullptr, &itfName);
    }

    for (auto c : childs)
    {
        auto newChild = c->clone(cloneContext);

        if (newChild->kind == AstNodeKind::FuncDecl)
        {
            auto newFunc = CastAst<AstFuncDecl>(newChild, AstNodeKind::FuncDecl);

            // Resolution of 'impl' needs all functions to have their symbol registered in the 'impl' scope,
            // in order to be able to wait for the resolution of all functions before solving the 'impl'
            // implementation.
            newNode->scope->symTable.registerSymbolName(nullptr, newFunc, SymbolKind::Function);

            if (!newFunc->genericParameters)
            {
                // The function inside the implementation was generic, so be sure we can now solve
                // its content
                newFunc->content->flags &= ~AST_NO_SEMANTIC;

                // Be sure we have a specific no generic typeinfo
                SWAG_ASSERT(newFunc->typeInfo);
                newFunc->typeInfo = newFunc->typeInfo->clone();
                newFunc->typeInfo->removeGenericFlag();
                newFunc->typeInfo->declNode = newFunc;
                newFunc->typeInfo->forceComputeName();
            }
            else
            {
                // If function inside the impl block has specific generic parameters, then we
                // consider it to be generic
                newFunc->flags &= ~AST_FROM_GENERIC;
                newFunc->flags |= AST_IS_GENERIC;
                for (auto g : newFunc->genericParameters->childs)
                {
                    g->flags &= ~AST_FROM_GENERIC;
                    g->flags |= AST_IS_GENERIC;
                }

                // Be sure we keep a generic typeinfo
                SWAG_ASSERT(newFunc->typeInfo);
                newFunc->typeInfo = newFunc->typeInfo->clone();
                newFunc->typeInfo->flags &= ~TYPEINFO_FROM_GENERIC;
                newFunc->typeInfo->flags |= TYPEINFO_GENERIC;
                newFunc->typeInfo->declNode = newFunc;
                newFunc->typeInfo->forceComputeName();
            }
        }
    }

    newNode->identifier    = findChildRef(identifier, newNode);
    newNode->identifierFor = findChildRef(identifierFor, newNode);

    // Cloning an impl block should be called only for generic interface implementation
    SWAG_ASSERT(newNode->identifierFor);

    context.propageResult(cloneContext);
    return newNode;
}

AstEnum::~AstEnum()
{
    if (scope)
        scope->release();
}

AstNode* AstEnum::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstEnum>();
    newNode->copyFrom(context, this, false);

    auto cloneContext             = context;
    cloneContext.parent           = newNode;
    cloneContext.parentScope      = Ast::newScope(newNode, newNode->token.text, ScopeKind::Enum, context.parentScope ? context.parentScope : ownerScope);
    cloneContext.ownerStructScope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    newNode->scope = cloneContext.parentScope;

    SWAG_ASSERT(newNode->typeInfo);
    newNode->typeInfo = newNode->typeInfo->clone();
    newNode->typeInfo->removeGenericFlag();
    newNode->typeInfo->declNode = newNode;

    return newNode;
}

AstNode* AstEnumValue::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstEnumValue>();
    newNode->copyFrom(context, this);
    newNode->attributes = attributes;
    return newNode;
}

AstNode* AstInit::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstInit>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    newNode->parameters = (AstFuncCallParams*) findChildRef(parameters, newNode);
    return newNode;
}

AstNode* AstDropCopyMove::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstDropCopyMove>();
    newNode->copyFrom(context, this);

    newNode->expression = findChildRef(expression, newNode);
    newNode->count      = findChildRef(count, newNode);
    return newNode;
}

AstNode* AstReturn::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstReturn>();
    newNode->copyFrom(context, this);

    // If return in an inline block has already been solved, we need this flag !
    if (context.cloneFlags & CLONE_RAW)
        newNode->semFlags |= semFlags & AST_SEM_EMBEDDED_RETURN;

    return newNode;
}

AstCompilerInline::~AstCompilerInline()
{
    if (scope)
        scope->release();
}

AstNode* AstCompilerInline::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerInline>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = cloneContext.parentScope;
    childs.back()->clone(cloneContext);
    context.propageResult(cloneContext);

    return newNode;
}

AstCompilerMacro::~AstCompilerMacro()
{
    if (scope)
        scope->release();
}

AstNode* AstCompilerMacro::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerMacro>();
    newNode->copyFrom(context, this, false);

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Macro, context.parentScope ? context.parentScope : ownerScope);
    newNode->scope           = cloneContext.parentScope;
    childs.back()->clone(cloneContext);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerMixin::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerMixin>();
    newNode->copyFrom(context, this);
    newNode->replaceTokens = replaceTokens;
    return newNode;
}

AstInline::~AstInline()
{
    if (scope && scope->kind == ScopeKind::Inline)
        scope->release();
}

AstNode* AstInline::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstInline>();
    newNode->copyFrom(context, this, false);

    // If we clone an inline block after bytecode generation (this happens if we have a mixin inside an inline function
    // for example), then we do not want to copy the AST_NO_BYTECODE_CHILDS (because we want the inline block to be
    // generated).
    // :EmitInlineOnce
    newNode->flags &= ~AST_NO_BYTECODE_CHILDS;

    newNode->func = func;

    // Is this correct ? Seems a little wierd, but that way we do not have to copy the parametersScope
    // content, which is not really possible for now (12/07/2021) (i.e. no way to copy already registered symbols in the scope).
    // Because it can happen that an inline block already solved is copied.
    // For example because of convertStructParamsToTmpVar, with inline calls as parameters: titi(A{round(6)}) => round already inlined.
    // I guess one day this will hit me in the face...
    newNode->parametersScope = parametersScope;

    auto cloneContext        = context;
    cloneContext.parent      = newNode;
    cloneContext.ownerInline = newNode;

    if (scope && scope->kind == ScopeKind::Inline)
        cloneContext.parentScope = Ast::newScope(newNode, "", ScopeKind::Inline, context.parentScope ? context.parentScope : ownerScope);

    newNode->scope = cloneContext.parentScope;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstCompilerIfBlock::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerIfBlock>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                 = context;
    cloneContext.parent               = newNode;
    cloneContext.ownerCompilerIfBlock = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    if (newNode->ownerCompilerIfBlock)
        newNode->ownerCompilerIfBlock->blocks.push_back(newNode);
    return newNode;
}

AstNode* AstCompilerSpecFunc::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCompilerSpecFunc>();

    auto cloneContext = context;
    cloneContext.forceSemFlags &= ~AST_SEM_SPEC_STACKSIZE;

    newNode->copyFrom(cloneContext, this, false);

    // Clone childs by hand, because a compiler block can contain a sub function, and this sub
    // function shouldn't have the ownerInline set (if the #ast is in an inline block)
    cloneContext.parent = newNode;
    for (auto p : childs)
    {
        if (p->kind == AstNodeKind::FuncDecl)
        {
            cloneContext.ownerInline = nullptr;
            cloneContext.removeFlags |= AST_RUN_BLOCK;
        }
        else
        {
            cloneContext.ownerInline = context.ownerInline;
            cloneContext.removeFlags = context.removeFlags;
        }

        p->clone(cloneContext)->flags &= ~AST_NO_SEMANTIC;
    }

    // If the compiler block has an embedded function, we need to restore the semantic pass on that function
    // content now
    if (newNode->childs.size() > 1)
    {
        // We also want to replace the name of the function (and the reference to it) in case
        // the block is in a mixin block, because in that case the function can be registered
        // more than once in the same scope.
        int  id      = g_UniqueID.fetch_add(1);
        Utf8 newName = "__cmpfunc" + to_string(id);

        auto func        = CastAst<AstFuncDecl>(newNode->childs.front(), AstNodeKind::FuncDecl);
        func->token.text = newName;
        func->flags &= ~AST_NO_SEMANTIC;
        func->content->flags &= ~AST_NO_SEMANTIC;
        func->flags |= AST_FROM_GENERIC;
        newNode->ownerScope->symTable.registerSymbolName(nullptr, func, SymbolKind::Function);

        // Ref to the function
        auto idRef                       = CastAst<AstIdentifierRef>(newNode->childs.back(), AstNodeKind::IdentifierRef);
        idRef->childs.back()->token.text = newName;
    }

    context.propageResult(cloneContext);
    return newNode;
}

AstNode* AstNameSpace::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstNameSpace>();
    newNode->copyFrom(context, this);
    return newNode;
}

AstNode* AstTryCatchAssume::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstTryCatchAssume>();
    newNode->copyFrom(context, this, false);

    auto cloneContext                = context;
    cloneContext.ownerTryCatchAssume = newNode;
    newNode->cloneChilds(cloneContext, this);
    context.propageResult(cloneContext);

    return newNode;
}

AstNode* AstAlias::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstAlias>();
    newNode->copyFrom(context, this);
    if (resolvedSymbolName)
        newNode->resolvedSymbolName = newNode->ownerScope->symTable.registerSymbolName(nullptr, newNode, resolvedSymbolName->kind);
    return newNode;
}

AstNode* AstCast::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstCast>();
    newNode->copyFrom(context, this);
    newNode->toCastTypeInfo = toCastTypeInfo;
    return newNode;
}

AstNode* AstFuncCallParams::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstFuncCallParams>();
    newNode->copyFrom(context, this);
    newNode->aliasNames = aliasNames;

    // Propagate aliases
    int idx = 0;
    for (auto it : newNode->aliasNames)
    {
        auto itn = context.replaceNames.find(it.text);
        if (itn != context.replaceNames.end())
        {
            context.usedReplaceNames.insert(itn->second);
            newNode->aliasNames[idx].text = itn->second;
        }

        idx++;
    }

    return newNode;
}

AstNode* AstRange::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstRange>();
    newNode->copyFrom(context, this);
    newNode->expressionLow = findChildRef(expressionLow, newNode);
    newNode->expressionUp  = findChildRef(expressionUp, newNode);
    return newNode;
}

AstNode* AstMakePointer::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstMakePointer>();
    newNode->copyFrom(context, this);

    if (lambda)
    {
        // This is super hacky
        // The problem is that the relation between a lambda and the makepointer lambda is tight,
        // and with #mixin the lambda is not duplicated, but the makepointer lambda is...
        // So this is a mess
        if (context.forceFlags & AST_IN_MIXIN)
        {
            if (lambda->captureParameters && childs.front() == lambda->captureParameters)
            {
                SWAG_ASSERT(newNode->childs[0]->kind == AstNodeKind::FuncCallParams);
                lambda->captureParameters = newNode->childs[0];
            }

            lambda->makePointerLambda = newNode;
        }

        newNode->lambda = lambda;
        context.nodeRefsToUpdate.push_back({newNode, (AstNode**) &newNode->lambda});
    }

    return newNode;
}

AstNode* AstOp::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstOp>();
    newNode->copyFrom(context, this);

    if (dependentLambda)
    {
        newNode->dependentLambda = dependentLambda;
        context.nodeRefsToUpdate.push_back({newNode, &newNode->dependentLambda});
    }

    return newNode;
}

AstNode* AstDefer::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstDefer>();
    newNode->copyFrom(context, this);
    newNode->deferKind = deferKind;
    return newNode;
}

AstNode* AstWith::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstWith>();
    newNode->copyFrom(context, this);
    newNode->id = id;
    return newNode;
}

AstNode* AstLiteral::clone(CloneContext& context)
{
    auto newNode = Ast::newNode<AstLiteral>();
    newNode->copyFrom(context, this);
    newNode->literalType  = literalType;
    newNode->literalValue = literalValue;
    return newNode;
}