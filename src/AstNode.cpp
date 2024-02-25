#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Generic.h"
#include "Module.h"
#include "Parser.h"
#include "TypeInfo.h"

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneHie)
{
    kind = from->kind;

    flags = from->flags;
    flags.remove(AST_IS_GENERIC);
    flags.add(context.forceFlags);
    flags.remove(context.removeFlags);

    specFlags = from->specFlags;

    ownerStructScope = context.ownerStructScope ? context.ownerStructScope : from->ownerStructScope;
    ownerScope       = context.parentScope ? context.parentScope : from->ownerScope;
    ownerFct         = context.ownerFct || context.cloneFlags.has(CLONE_FORCE_OWNER_FCT) ? context.ownerFct : from->ownerFct;

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

    if (context.ownerCompilerIfBlock || from->hasOwnerCompilerIfBlock())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerCompilerIfBlock = context.ownerCompilerIfBlock ? context.ownerCompilerIfBlock : from->ownerCompilerIfBlock();
    }

    if (context.ownerTryCatchAssume || from->hasOwnerTryCatchAssume())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerTryCatchAssume = context.ownerTryCatchAssume ? context.ownerTryCatchAssume : from->ownerTryCatchAssume();
    }

    if (context.ownerInline || from->hasOwnerInline())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerInline = context.ownerInline ? context.ownerInline : from->ownerInline();
    }

    if (context.ownerBreakable || from->hasOwnerBreakable())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerBreakable = context.ownerBreakable ? context.ownerBreakable : from->ownerBreakable();
    }

    // Replace a type by another one during generic instantiation
    typeInfo = Generic::replaceGenericTypes(context.replaceTypes, from->typeInfo);
    if (typeInfo != from->typeInfo)
        addAstFlag(AST_FROM_GENERIC);

    // This should not be copied. It will be recomputed if necessary.
    // This can cause some problems with inline functions and auto cast, as inline functions are evaluated
    // as functions, and also each time they are inline.
    if (context.cloneFlags.has(CLONE_RAW))
    {
        addSemFlag(from->semFlags.mask(SEMFLAG_USER_CAST));
        addSemFlag(from->semFlags.mask(SEMFLAG_FROM_REF));
        addSemFlag(from->semFlags.mask(SEMFLAG_FROM_PTR_REF));
        castedTypeInfo = from->castedTypeInfo;
    }

    addSemFlag(from->semFlags.mask(SEMFLAG_HAS_SYMBOL_NAME));
    symbolName = from->symbolName;

    token = from->token;
    if (context.forceLocation)
    {
        token.startLocation = context.forceLocation->startLocation;
        token.endLocation   = context.forceLocation->endLocation;
    }

    semanticFct = from->semanticFct;
    byteCodeFct = from->byteCodeFct;

    if (from->hasExtMisc())
    {
        allocateExtension(ExtensionKind::Misc);
        extMisc()->extraPointers     = from->extMisc()->extraPointers;
        extMisc()->castOffset        = from->extMisc()->castOffset;
        extMisc()->stackOffset       = from->extMisc()->stackOffset;
        extMisc()->anyTypeOffset     = from->extMisc()->anyTypeOffset;
        extMisc()->alternativeScopes = from->extMisc()->alternativeScopes;
    }

    if (from->hasExtSemantic())
    {
        allocateExtension(ExtensionKind::Semantic);
        extSemantic()->semanticBeforeFct = from->extSemantic()->semanticBeforeFct;
        extSemantic()->semanticAfterFct  = from->extSemantic()->semanticAfterFct;

        if (from->extSemantic()->computedValue)
        {
            extSemantic()->computedValue  = Allocator::alloc<ComputedValue>();
            *extSemantic()->computedValue = *from->extSemantic()->computedValue;
        }
    }

    if (from->hasExtByteCode())
    {
        allocateExtension(ExtensionKind::ByteCode);
        extByteCode()->byteCodeBeforeFct = from->extByteCode()->byteCodeBeforeFct;
        extByteCode()->byteCodeAfterFct  = from->extByteCode()->byteCodeAfterFct;
    }

    attributeFlags = from->attributeFlags;
    safetyOn       = from->safetyOn;
    safetyOff      = from->safetyOff;

    parent = context.parent;
    if (parent)
        Ast::addChildBack(parent, this);

    if (cloneHie)
    {
        cloneChildren(context, from);

        // Force semantic on specific nodes on generic instantiation
        if (from->hasAstFlag(AST_IS_GENERIC) && from->hasSemFlag(SEMFLAG_ON_CLONE))
        {
            for (const auto one : children)
                one->removeAstFlag(AST_NO_SEMANTIC);
        }
    }
}

void AstNode::cloneChildren(CloneContext& context, AstNode* from)
{
    const auto oldParent = context.parent;
    context.parent       = this;
    const auto num       = from->children.size();
    for (uint32_t i = 0; i < num; i++)
    {
        // Do not duplicate a struct if it's a child of something else (i.e. another struct), because
        // in case of generics, we do want the normal generic stuff to be done (cloning)
        if (from->children[i]->isNot(AstNodeKind::StructDecl))
            from->children[i]->clone(context);
    }

    context.parent = oldParent;
}

void AstNode::releaseChildren()
{
    for (const auto c : children)
        c->release();
    children.release();
}

void AstNode::release()
{
#ifdef SWAG_STATS
    ++g_Stats.releaseNodes;
#endif

    if (hasExtByteCode())
    {
        if (extByteCode()->bc)
            extByteCode()->bc->release();
        Allocator::free<NodeExtensionByteCode>(extByteCode());
    }

    if (hasExtSemantic())
    {
        Allocator::free<NodeExtensionSemantic>(extSemantic());
    }

    if (hasExtOwner())
    {
        for (const auto c : extOwner()->nodesToFree)
            c->release();
        Allocator::free<NodeExtensionOwner>(extOwner());
    }

    if (hasExtMisc())
    {
        Allocator::free<NodeExtensionMisc>(extMisc());
    }

    if (extension)
        Allocator::free<NodeExtension>(extension);

    // Pre-release, if we need to children to be alive
    if (kind == AstNodeKind::FuncDecl)
    {
        if (const auto funcDecl = castAst<AstFuncDecl>(this, AstNodeKind::FuncDecl); funcDecl->content)
            funcDecl->content->ownerScope->release();
    }

    // Release children first
    for (const auto c : children)
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
        case AstNodeKind::TypeAlias:
        case AstNodeKind::NameAlias:
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
        case AstNodeKind::RefSubDecl:
            Allocator::free<AstRefSubDecl>(this);
            break;
        case AstNodeKind::Statement:
        case AstNodeKind::StatementNoScope:
            Allocator::free<AstStatement>(this);
            break;
        case AstNodeKind::File:
            Allocator::free<AstFile>(this);
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
        case AstNodeKind::TypeAlias:
        case AstNodeKind::NameAlias:
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
        case AstNodeKind::RefSubDecl:
            return clone<AstRefSubDecl>(this, context);
        case AstNodeKind::Statement:
        case AstNodeKind::StatementNoScope:
            return clone<AstStatement>(this, context);
        case AstNodeKind::File:
            return clone<AstFile>(this, context);

        default:
        {
            const auto newNode = Ast::newNode<AstNode>();
            newNode->copyFrom(context, this);
            return newNode;
        }
    }
}

void AstNode::inheritAstFlagsOr(AstNodeFlags flag)
{
    for (const auto child : children)
        flags.add(child->flags.mask(flag));
}

void AstNode::inheritAstFlagsOr(const AstNode* op, AstNodeFlags flag)
{
    if (!op)
        return;
    flags.add(op->flags.mask(flag));
}

void AstNode::inheritAstFlagsAnd(AstNodeFlags flag)
{
    inheritAstFlagsAnd(this, flag);
}

void AstNode::inheritAstFlagsAnd(AstNodeFlags flag1, AstNodeFlags flag2)
{
    inheritAstFlagsAnd(this, flag1, flag2);
}

void AstNode::inheritAstFlagsAnd(AstNodeFlags flag1, AstNodeFlags flag2, AstNodeFlags flag3)
{
    inheritAstFlagsAnd(this, flag1, flag2, flag3);
}

void AstNode::inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag)
{
    for (const auto child : who->children)
    {
        if (!child->flags.has(flag))
            return;
    }

    flags.add(flag);
}

void AstNode::inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag1, AstNodeFlags flag2)
{
    flags.add(flag1);
    flags.add(flag2);

    for (const auto child : who->children)
    {
        if (!child->flags.has(flag1))
            flags.remove(flag1);
        if (!child->flags.has(flag2))
            flags.remove(flag2);
        if (!flags.has(flag1 | flag2))
            return;
    }
}

void AstNode::inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag1, AstNodeFlags flag2, AstNodeFlags flag3)
{
    flags.add(flag1);
    flags.add(flag2);
    flags.add(flag3);

    for (const auto child : who->children)
    {
        if (!child->flags.has(flag1))
            flags.remove(flag1);
        if (!child->flags.has(flag2))
            flags.remove(flag2);
        if (!child->flags.has(flag3))
            flags.remove(flag3);
        if (!flags.has(flag1 | flag2 | flag3))
            return;
    }
}

void AstNode::inheritTokenName(Token& tkn)
{
    SWAG_ASSERT(!tkn.text.empty());
    token.text = std::move(tkn.text);
}

void AstNode::inheritTokenLocation(const Token& tkn)
{
    token.sourceFile    = tkn.sourceFile;
    token.startLocation = tkn.startLocation;
    token.endLocation   = tkn.endLocation;
}

void AstNode::inheritOwners(const AstNode* from)
{
    if (!from)
        return;

    ownerStructScope = from->ownerStructScope;
    ownerScope       = from->ownerScope;
    ownerFct         = from->ownerFct;

    if (from->hasOwnerCompilerIfBlock())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerCompilerIfBlock = from->ownerCompilerIfBlock();
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerCompilerIfBlock = nullptr;
    }

    if (from->hasOwnerInline())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerInline = from->ownerInline();
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerInline = nullptr;
    }

    if (from->hasOwnerBreakable())
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerBreakable = from->ownerBreakable();
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerBreakable = nullptr;
    }
}

void AstNode::inheritOwnersAndFlags(const Parser* parser)
{
    ownerStructScope = parser->currentStructScope;
    ownerScope       = parser->currentScope;
    ownerFct         = parser->currentFct;

    if (parser->currentCompilerIfBlock)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerCompilerIfBlock = parser->currentCompilerIfBlock;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerCompilerIfBlock = nullptr;
    }

    if (parser->currentInline)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerInline = parser->currentInline;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerInline = nullptr;
    }

    if (parser->currentBreakable)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerBreakable = parser->currentBreakable;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerBreakable = nullptr;
    }

    if (parser->currentTryCatchAssume)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerTryCatchAssume = parser->currentTryCatchAssume;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerTryCatchAssume = nullptr;
    }

    flags.add(parser->currentFlags);
}

void AstNode::allocateComputedValue()
{
    allocateExtension(ExtensionKind::Semantic);
    if (!extSemantic()->computedValue)
    {
        extSemantic()->computedValue = Allocator::alloc<ComputedValue>();
#ifdef SWAG_STATS
        g_Stats.memNodesLiteral += sizeof(ComputedValue);
#endif
    }
}

void AstNode::releaseComputedValue()
{
    if (!hasFlagComputedValue())
        return;
    removeAstFlag(AST_COMPUTED_VALUE);
    Allocator::free<ComputedValue>(computedValue());
    extSemantic()->computedValue = nullptr;
}

void AstNode::setFlagsValueIsComputed()
{
    allocateComputedValue();
    addAstFlag(AST_CONST_EXPR | AST_COMPUTED_VALUE | AST_R_VALUE);
}

void AstNode::inheritComputedValue(const AstNode* from)
{
    if (!from)
        return;
    inheritAstFlagsOr(from, AST_COMPUTED_VALUE | AST_VALUE_IS_GEN_TYPEINFO);
    if (hasAstFlag(AST_COMPUTED_VALUE))
    {
        SWAG_ASSERT(from->hasComputedValue());
        addAstFlag(AST_CONST_EXPR | AST_R_VALUE);
        allocateExtension(ExtensionKind::Semantic);
        extSemantic()->computedValue = from->computedValue();
    }
}

bool AstNode::hasFlagComputedValue() const
{
    if (hasAstFlag(AST_COMPUTED_VALUE))
    {
        SWAG_ASSERT(hasComputedValue());
        return true;
    }

    return false;
}

bool AstNode::isConstantGenTypeInfo() const
{
    return hasAstFlag(AST_VALUE_IS_GEN_TYPEINFO);
}

ExportedTypeInfo* AstNode::getConstantGenTypeInfo() const
{
    SWAG_ASSERT(isConstantGenTypeInfo());
    return static_cast<ExportedTypeInfo*>(computedValue()->getStorageAddr());
}

bool AstNode::isConstantTrue() const
{
    return hasFlagComputedValue() && computedValue()->reg.b;
}

bool AstNode::isConstantFalse() const
{
    return hasFlagComputedValue() && !computedValue()->reg.b;
}

bool AstNode::isGeneratedSelf() const
{
    return kind == AstNodeKind::FuncDeclParam && hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERATED_SELF);
}

bool AstNode::isEmptyFct()
{
    if (kind != AstNodeKind::FuncDecl)
        return false;
    const auto funcDecl = castAst<AstFuncDecl>(this, AstNodeKind::FuncDecl);
    return funcDecl->content == nullptr;
}

bool AstNode::isForeign() const
{
    return hasAttribute(ATTRIBUTE_FOREIGN);
}

bool AstNode::isSilentCall() const
{
    return kind == AstNodeKind::Identifier && hasSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
}

bool AstNode::isPublic() const
{
    if (hasAttribute(ATTRIBUTE_PUBLIC))
        return true;
    if (hasSemFlag(SEMFLAG_ACCESS_PUBLIC))
        return true;
    return false;
}

bool AstNode::isFunctionCall()
{
    if (kind == AstNodeKind::FuncCall)
        return true;
    if (kind != AstNodeKind::Identifier)
        return false;

    const auto id = castAst<AstIdentifier>(this, AstNodeKind::Identifier);
    return id->callParameters != nullptr;
}

bool AstNode::isForceTakeAddress() const
{
    if (hasAstFlag(AST_TAKE_ADDRESS) && !hasSemFlag(SEMFLAG_FORCE_NO_TAKE_ADDRESS))
        return true;
    if (hasSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS))
        return true;
    return false;
}

void AstNode::swap2Children()
{
    SWAG_ASSERT(children.size() == 2);
    const auto tmp = children[0];
    children[0]    = children[1];
    children[1]    = tmp;
}

bool AstNode::hasSpecialFuncCall() const
{
    const auto userOp = extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
    return userOp && userOp->symbol->is(SymbolKind::Function);
}

bool AstNode::hasSpecialFuncCall(const Utf8& name) const
{
    const auto userOp = extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
    return userOp && userOp->symbol->is(SymbolKind::Function) && userOp->symbol->name == name;
}

AstNode* AstNode::inSimpleReturn() const
{
    const auto test = parent;
    if (!test)
        return nullptr;
    if (test->is(AstNodeKind::Return))
        return test;
    if (test->is(AstNodeKind::Try) && test->parent->is(AstNodeKind::Return))
        return test->parent;
    if (test->is(AstNodeKind::Catch) && test->parent->is(AstNodeKind::Return))
        return test->parent;
    return nullptr;
}

bool AstNode::isSpecialFunctionGenerated() const
{
    if (!hasExtByteCode() || !extByteCode()->bc || !extByteCode()->bc->isCompilerGenerated)
        return false;
    return true;
}

bool AstNode::isSpecialFunctionName() const
{
    // Check operators
    const auto& name = token.text;
    if (name.length() < 3)
        return false;

    // A special function starts with 'op', and then there's an upper case letter
    if (name[0] != 'o' || name[1] != 'p' || name[2] < 'A' || name[2] > 'Z')
        return false;

    return true;
}

void AstNode::setBcNotifyBefore(ByteCodeNotifyFct fct, ByteCodeNotifyFct checkIf)
{
    allocateExtension(ExtensionKind::ByteCode);

#ifdef SWAG_HAS_ASSERT
    if (extByteCode()->byteCodeBeforeFct != nullptr &&
        extByteCode()->byteCodeBeforeFct != fct &&
        extByteCode()->byteCodeBeforeFct != checkIf)
    {
        SWAG_ASSERT(false);
    }
#endif

    extByteCode()->byteCodeBeforeFct = fct;
}

void AstNode::setBcNotifyAfter(ByteCodeNotifyFct fct, ByteCodeNotifyFct checkIf)
{
    allocateExtension(ExtensionKind::ByteCode);

#ifdef SWAG_HAS_ASSERT
    if (extByteCode()->byteCodeAfterFct != nullptr &&
        extByteCode()->byteCodeAfterFct != fct &&
        extByteCode()->byteCodeAfterFct != checkIf)
    {
        SWAG_ASSERT(false);
    }
#endif

    extByteCode()->byteCodeAfterFct = fct;
}

void AstNode::allocateExtension(ExtensionKind extensionKind)
{
    ScopedLock lk(mutex);
    allocateExtensionNoLock(extensionKind);
}

void AstNode::allocateExtensionNoLock(ExtensionKind extensionKind)
{
    if (!extension)
    {
        extension = Allocator::alloc<NodeExtension>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtension));
#endif
    }

    switch (extensionKind)
    {
        case ExtensionKind::ByteCode:
            if (hasExtByteCode())
                return;
            extension->bytecode = Allocator::alloc<NodeExtensionByteCode>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionByteCode));
#endif
            break;

        case ExtensionKind::Semantic:
            if (hasExtSemantic())
                return;
            extension->semantic = Allocator::alloc<NodeExtensionSemantic>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionSemantic));
#endif
            break;

        case ExtensionKind::Owner:
            if (hasExtOwner())
                return;
            extension->owner = Allocator::alloc<NodeExtensionOwner>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionOwner));
#endif
            break;

        default:
            if (hasExtMisc())
                return;
            extension->misc = Allocator::alloc<NodeExtensionMisc>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionMisc));
#endif
            break;
    }
}

bool AstNode::isConstant0() const
{
    SWAG_ASSERT(typeInfo);
    if (!typeInfo->isNative())
        return false;
    if (!hasFlagComputedValue())
        return false;

    switch (typeInfo->sizeOf)
    {
        case 1:
            return computedValue()->reg.u8 == 0;
        case 2:
            return computedValue()->reg.u16 == 0;
        case 4:
            return computedValue()->reg.u32 == 0;
        case 8:
            return computedValue()->reg.u64 == 0;
        default:
            break;
    }

    return false;
}

bool AstNode::isConstant1() const
{
    SWAG_ASSERT(typeInfo);
    if (!typeInfo->isNative())
        return false;
    if (!hasFlagComputedValue())
        return false;

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::S8:
            return computedValue()->reg.u8 == 1;
        case NativeTypeKind::U16:
        case NativeTypeKind::S16:
            return computedValue()->reg.u16 == 1;
        case NativeTypeKind::U32:
        case NativeTypeKind::S32:
        case NativeTypeKind::Rune:
            return computedValue()->reg.u32 == 1;
        case NativeTypeKind::U64:
        case NativeTypeKind::S64:
            return computedValue()->reg.u64 == 1;
        case NativeTypeKind::F32:
            return computedValue()->reg.f32 == 1;
        case NativeTypeKind::F64:
            return computedValue()->reg.f64 == 1;
        default:
            break;
    }

    return false;
}

const Token& AstNode::getTokenName() const
{
    if (kind == AstNodeKind::FuncDecl)
    {
        const auto fctDecl = castAst<AstFuncDecl>(this, AstNodeKind::FuncDecl);
        return fctDecl->tokenName;
    }

    if (kind == AstNodeKind::AttrDecl)
    {
        const auto attrDecl = castAst<AstAttrDecl>(this, AstNodeKind::AttrDecl);
        return attrDecl->tokenName;
    }

    if (kind == AstNodeKind::StructDecl)
    {
        const auto structDecl = castAst<AstStruct>(this, AstNodeKind::StructDecl);
        return structDecl->tokenName;
    }

    if (kind == AstNodeKind::InterfaceDecl)
    {
        const auto itfDecl = castAst<AstStruct>(this, AstNodeKind::InterfaceDecl);
        return itfDecl->tokenName;
    }

    if (kind == AstNodeKind::EnumDecl)
    {
        const auto enumDecl = castAst<AstEnum>(this, AstNodeKind::EnumDecl);
        return enumDecl->tokenName;
    }

    if (kind == AstNodeKind::Impl)
    {
        const auto implDecl = castAst<AstImpl>(this, AstNodeKind::Impl);
        return implDecl->identifier->getTokenName();
    }

    return token;
}

void AstNode::setPassThrough()
{
    semanticFct = nullptr;

    byteCodeFct = ByteCodeGen::emitPassThrough;
    if (hasExtByteCode())
    {
        extByteCode()->byteCodeAfterFct  = nullptr;
        extByteCode()->byteCodeBeforeFct = nullptr;
    }
}

bool AstNode::isParentOf(const AstNode* child) const
{
    if (!child)
        return false;
    while (child->parent)
    {
        if (child->parent == this)
            return true;
        child = child->parent;
    }

    return false;
}

bool AstNode::isValidIfParam(const SymbolOverload* overload) const
{
    if (!hasAstFlag(AST_IN_VALIDIF))
        return false;
    if (!overload)
        return false;
    if (!overload->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
        return false;
    if (isSameStackFrame(overload))
        return false;
    return true;
}

bool AstNode::isSameStackFrame(const SymbolOverload* overload) const
{
    if (overload->symbol->isNot(SymbolKind::Variable))
        return true;
    if (overload->hasFlag(OVERLOAD_COMPUTED_VALUE))
        return true;
    if (overload->hasFlag(OVERLOAD_VAR_INLINE) && !hasOwnerInline())
        return false;
    if (overload->hasFlag(OVERLOAD_VAR_INLINE) && ownerInline()->ownerFct != ownerFct)
        return false;
    if (!overload->hasFlag(OVERLOAD_VAR_FUNC_PARAM) && !overload->hasFlag(OVERLOAD_VAR_LOCAL))
        return true;

    const auto nodeVar = overload->node;
    if (hasAstFlag(AST_IN_RUN_BLOCK) != nodeVar->hasAstFlag(AST_IN_RUN_BLOCK))
        return false;
    if (ownerFct != nodeVar->ownerFct)
        return false;

    return true;
}

void AstNode::printLoc() const
{
    if (!token.sourceFile)
        return;
    printf("%s:%d:%d\n", token.sourceFile->path.c_str(), token.startLocation.line + 1, token.startLocation.column + 1);
}

uint32_t AstNode::childParentIdx() const
{
    SWAG_ASSERT(parent);
    for (uint32_t it = 0; it < parent->children.size(); it++)
    {
        if (parent->children[it] == this)
            return it;
    }

    SWAG_ASSERT(false);
    return UINT32_MAX;
}

void AstNode::addAlternativeScope(Scope* scope, AltScopeFlags altFlags)
{
    AlternativeScope sv;
    sv.scope = scope;
    sv.flags = altFlags;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock lk(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopes.push_back(sv);
}

void AstNode::addAlternativeScopeVar(Scope* scope, AstNode* varNode, AltScopeFlags altFlags)
{
    AlternativeScopeVar sv;
    sv.scope    = scope;
    sv.node     = varNode;
    sv.leafNode = varNode;
    sv.flags    = altFlags;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock lk(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopesVars.push_back(sv);
}

void AstNode::addAlternativeScopes(NodeExtensionMisc* ext)
{
    SharedLock lk(ext->mutexAltScopes);
    if (ext->alternativeScopes.empty())
        return;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock lk1(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopes.append(ext->alternativeScopes);
}

void AstNode::computeLocation(SourceLocation& start, SourceLocation& end)
{
    start = token.startLocation;
    end   = token.endLocation;
    for (const auto p : children)
    {
        if (p->is(AstNodeKind::Statement))
            break;
        if (p->is(AstNodeKind::Inline))
            break;
        if (p->hasAstFlag(AST_GENERATED))
            continue;
        if (p->is(AstNodeKind::FuncDeclType) && p->children.empty())
            continue;

        SourceLocation childStart, childEnd;
        p->computeLocation(childStart, childEnd);

        if (childStart.line < start.line ||
            (childStart.line == start.line && childStart.column < start.column))
            start = childStart;
        if (childEnd.line > end.line ||
            (childEnd.line == end.line && childEnd.column > end.column))
            end = childEnd;
    }
}

Utf8 AstNode::getScopedName() const
{
    auto& fullName = ownerScope->getFullName();
    if (fullName.empty())
        return token.text;
    Utf8 result{fullName, fullName.length() + token.text.length() + 1};
    result += ".";
    result += token.text;
    return result;
}

AstNode* AstNode::findParentAttrUse(const Utf8& name) const
{
    auto search = parent;
    while (search)
    {
        if (search->is(AstNodeKind::AttrUse))
        {
            const auto attrDecl = castAst<AstAttrUse>(search, AstNodeKind::AttrUse);
            if (const auto it = attrDecl->attributes.getAttribute(name))
                return it->child ? it->child : it->node;
        }

        search = search->parent;
    }

    return nullptr;
}

AstNode* AstNode::findParent(TokenId tkn) const
{
    auto find = parent;
    while (find && find->token.id != tkn)
        find = find->parent;
    return find;
}

AstNode* AstNode::findParent(AstNodeKind parentKind) const
{
    auto find = parent;
    while (find && find->kind != parentKind)
        find = find->parent;
    return find;
}

AstNode* AstNode::findParentOrMe(AstNodeKind parentKind)
{
    auto find = this;
    while (find && find->kind != parentKind)
        find = find->parent;
    return find;
}

AstNode* AstNode::findParent(AstNodeKind parentKind1, AstNodeKind parentKind2) const
{
    auto find = parent;
    while (find && find->kind != parentKind1 && find->kind != parentKind2)
        find = find->parent;
    return find;
}

AstNode* AstNode::findChild(AstNodeKind childKind) const
{
    for (const auto c : children)
    {
        if (c->kind == childKind)
            return c;
    }

    return nullptr;
}

AstNode* AstNode::findChildRef(const AstNode* ref, AstNode* fromChild) const
{
    if (!ref)
        return nullptr;
    for (uint32_t i = 0; i < children.size(); i++)
    {
        if (children[i] == ref)
            return fromChild->children[i];
    }

    return nullptr;
}

AstNode* AstNode::findChildRefRec(AstNode* ref, AstNode* fromChild) const
{
    if (!ref)
        return nullptr;
    if (this == ref)
        return fromChild;
    for (uint32_t i = 0; i < children.size(); i++)
    {
        if (children[i] == ref)
            return fromChild->children[i];

        if (children[i]->children.count == fromChild->children[i]->children.count)
        {
            if (const auto result = children[i]->findChildRefRec(ref, fromChild->children[i]))
                return result;
        }
    }

    return nullptr;
}

void AstNode::setOwnerAttrUse(AstAttrUse* attrUse)
{
    switch (kind)
    {
        case AstNodeKind::CompilerAst:
        case AstNodeKind::Statement:
        case AstNodeKind::Namespace:
        case AstNodeKind::Impl:
        case AstNodeKind::CompilerIf:
        case AstNodeKind::CompilerIfBlock:
        case AstNodeKind::SwitchCaseBlock:
            for (const auto s : children)
                s->setOwnerAttrUse(attrUse);
            break;

        default:
            allocateExtension(ExtensionKind::Owner);
            extOwner()->ownerAttrUse = attrUse;
            break;
    }
}

void AstNode::setOwnerBreakable(AstBreakable* bkp)
{
    if (!bkp)
        return;
    allocateExtension(ExtensionKind::Owner);
    extOwner()->ownerBreakable = bkp;
}

void AstNode::setResolvedSymbolName(SymbolName* sym)
{
    addSemFlag(SEMFLAG_HAS_SYMBOL_NAME);
    symbolName = sym;
}

void AstNode::setResolvedSymbolOverload(SymbolOverload* over)
{
    if (over)
    {
        symbolOverload = over;
        removeSemFlag(SEMFLAG_HAS_SYMBOL_NAME);
    }
    else
    {
        addSemFlag(SEMFLAG_HAS_SYMBOL_NAME);
    }
}

void AstNode::setResolvedSymbol(SymbolName* sym, SymbolOverload* over)
{
    if (!over)
    {
        addSemFlag(SEMFLAG_HAS_SYMBOL_NAME);
        symbolName = sym;
    }
    else
    {
        SWAG_ASSERT(sym == over->symbol);
        removeSemFlag(SEMFLAG_HAS_SYMBOL_NAME);
        symbolOverload = over;
    }
}
