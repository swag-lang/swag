#include "pch.h"
#include "Semantic/Generic/Generic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"
#include "Wmf/Module.h"

void AstNode::copyFrom(CloneContext& context, AstNode* from, bool cloneHie)
{
    kind = from->kind;

    flags = from->flags;
    flags.remove(AST_GENERIC);
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

    symbolName     = from->symbolName.load();
    symbolOverload = from->symbolOverload.load();

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
        if (from->hasAstFlag(AST_GENERIC) && from->hasSemFlag(SEMFLAG_ON_CLONE))
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
    const auto num       = from->childCount();
    for (uint32_t i = 0; i < num; i++)
    {
        // Do not duplicate a struct if it's a child of something else (i.e. another struct), because
        // in case of generics, we do want the normal generic stuff to be done (cloning)
        if (from->children[i]->isNot(AstNodeKind::StructDecl))
            from->children[i]->clone(context);
    }

    context.parent = oldParent;
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
        case AstNodeKind::CompilerWhere:
        case AstNodeKind::CompilerWhereCall:
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
        case AstNodeKind::CompilerImport:
            return clone<AstCompilerImport>(this, context);
        case AstNodeKind::Using:
            return clone<AstUsing>(this, context);
        case AstNodeKind::CompilerGlobal:
            return clone<AstCompilerGlobal>(this, context);
        case AstNodeKind::CompilerCode:
            return clone<AstCompilerCode>(this, context);

        default:
        {
            const auto newNode = Ast::newNode<AstNode>();
            newNode->copyFrom(context, this);
            return newNode;
        }
    }
}
