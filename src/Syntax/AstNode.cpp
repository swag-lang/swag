#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"
#include "Wmf/Module.h"

void AstNode::inheritAstFlagsOr(const AstNodeFlags& flag)
{
    for (const auto child : children)
        flags.add(child->flags.mask(flag));
    SWAG_ASSERT(!flag.has(AST_COMPUTED_VALUE));
}

void AstNode::inheritAstFlagsOr(const AstNode* op, const AstNodeFlags& flag)
{
    if (!op)
        return;
    flags.add(op->flags.mask(flag));
}

void AstNode::inheritAstFlagsAnd(const AstNodeFlags& flag)
{
    inheritAstFlagsAnd(this, flag);
}

void AstNode::inheritAstFlagsAnd(const AstNodeFlags& flag1, const AstNodeFlags& flag2)
{
    inheritAstFlagsAnd(this, flag1, flag2);
}

void AstNode::inheritAstFlagsAnd(const AstNodeFlags& flag1, const AstNodeFlags& flag2, const AstNodeFlags& flag3)
{
    inheritAstFlagsAnd(this, flag1, flag2, flag3);
}

void AstNode::inheritAstFlagsAnd(AstNode* who, const AstNodeFlags& flag)
{
    for (const auto child : who->children)
    {
        if (!child->flags.has(flag))
            return;
    }

    SWAG_ASSERT(!flag.has(AST_COMPUTED_VALUE));
    flags.add(flag);
}

void AstNode::inheritAstFlagsAnd(AstNode* who, const AstNodeFlags& flag1, const AstNodeFlags& flag2)
{
    flags.add(flag1);
    flags.add(flag2);
    SWAG_ASSERT(!flag1.has(AST_COMPUTED_VALUE));
    SWAG_ASSERT(!flag2.has(AST_COMPUTED_VALUE));

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

void AstNode::inheritAstFlagsAnd(AstNode* who, const AstNodeFlags& flag1, const AstNodeFlags& flag2, const AstNodeFlags& flag3)
{
    flags.add(flag1);
    flags.add(flag2);
    flags.add(flag3);
    SWAG_ASSERT(!flag1.has(AST_COMPUTED_VALUE));
    SWAG_ASSERT(!flag2.has(AST_COMPUTED_VALUE));
    SWAG_ASSERT(!flag3.has(AST_COMPUTED_VALUE));

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

    flags.add(parser->currentAstNodeFlags);
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

    inheritAstFlagsOr(from, AST_COMPUTED_VALUE | AST_VALUE_GEN_TYPEINFO);
    if (hasAstFlag(AST_COMPUTED_VALUE))
    {
        addAstFlag(AST_CONST_EXPR | AST_R_VALUE);
        if (from->hasComputedValue())
        {
            allocateExtension(ExtensionKind::Semantic);
            extSemantic()->computedValue = from->computedValue();
        }
        else
        {
            SWAG_ASSERT(hasComputedValue());
        }
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
    return hasAstFlag(AST_VALUE_GEN_TYPEINFO);
}

ExportedTypeInfo* AstNode::getConstantGenTypeInfo() const
{
    SWAG_ASSERT(isConstantGenTypeInfo());
    return static_cast<ExportedTypeInfo*>(computedValue()->getStorageAddr());
}

bool AstNode::addAnyType(SemanticContext* context, TypeInfo* typeinfo)
{
    const auto module = context->sourceFile->module;

    const auto anyTypeSegment = Semantic::getConstantSegFromContext(this);
    addExtraPointer(ExtraPointerKind::AnyTypeSegment, anyTypeSegment);

    uint32_t          offset;
    ExportedTypeInfo* type = nullptr;
    SWAG_CHECK(module->typeGen.genExportedTypeInfo(context, typeinfo, anyTypeSegment, &offset, 0, nullptr, &type));
    YIELD();

    addExtraValue(ExtraPointerKind::AnyTypeOffset, offset);
    addExtraPointer(ExtraPointerKind::AnyTypeValue, type);
    return true;
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
    return is(AstNodeKind::FuncDeclParam) && hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERATED_SELF);
}

bool AstNode::isEmptyFct()
{
    if (isNot(AstNodeKind::FuncDecl))
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
    return is(AstNodeKind::Identifier) && hasSpecFlag(AstIdentifier::SPEC_FLAG_SILENT_CALL);
}

bool AstNode::isPublic() const
{
    if (hasAttribute(ATTRIBUTE_PUBLIC))
        return true;
    if (hasSemFlag(SEMFLAG_ACCESS_PUBLIC))
        return true;
    return false;
}

bool AstNode::isForceTakeAddress() const
{
    if (hasAstFlag(AST_TAKE_ADDRESS) && !hasSemFlag(SEMFLAG_FORCE_NO_TAKE_ADDRESS))
        return true;
    if (hasSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS))
        return true;
    return false;
}

bool AstNode::hasIntrinsicName() const
{
    return token.text.length() > 1 && token.text[0] == '@';
}

void AstNode::swap2Children()
{
    SWAG_ASSERT(childCount() >= 2);
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
    if (fct != nullptr &&
        extByteCode()->byteCodeAfterFct != nullptr &&
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
    ScopedLock lk(mutexExt);

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
        {
            if (extension->bytecode)
                return;
            extension->bytecode = Allocator::alloc<NodeExtensionByteCode>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionByteCode));
#endif
            break;
        }

        case ExtensionKind::Semantic:
        {
            if (extension->semantic)
                return;
            extension->semantic = Allocator::alloc<NodeExtensionSemantic>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionSemantic));
#endif
            break;
        }

        case ExtensionKind::Owner:
        {
            if (extension->owner)
                return;
            extension->owner = Allocator::alloc<NodeExtensionOwner>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionOwner));
#endif
            break;
        }

        default:
        {
            if (extension->misc)
                return;
            extension->misc = Allocator::alloc<NodeExtensionMisc>();
#ifdef SWAG_STATS
            g_Stats.memNodesExt += Allocator::alignSize(sizeof(NodeExtensionMisc));
#endif
            break;
        }
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
    }

    return false;
}

// Never returns null, but only 'kind' is always valid.
// If 'kind' is AstNodeKind::Invalid, then the rest of the node is garbage.
AstNode* AstNode::getParent(uint32_t level) const
{
    SWAG_ASSERT(level);
    auto p = parent;
    for (uint32_t i = 1; i < level; i++)
    {
        if (!p->parent)
        {
            static AstNode n;
            n.kind = AstNodeKind::Invalid;
            return &n;
        }

        p = p->parent;
    }

    return p;
}

const Token& AstNode::getTokenName() const
{
    if (is(AstNodeKind::FuncDecl))
    {
        const auto fctDecl = castAst<AstFuncDecl>(this, AstNodeKind::FuncDecl);
        return fctDecl->tokenName;
    }

    if (is(AstNodeKind::AttrDecl))
    {
        const auto attrDecl = castAst<AstAttrDecl>(this, AstNodeKind::AttrDecl);
        return attrDecl->tokenName;
    }

    if (is(AstNodeKind::StructDecl))
    {
        const auto structDecl = castAst<AstStruct>(this, AstNodeKind::StructDecl);
        return structDecl->tokenName;
    }

    if (is(AstNodeKind::InterfaceDecl))
    {
        const auto itfDecl = castAst<AstStruct>(this, AstNodeKind::InterfaceDecl);
        return itfDecl->tokenName;
    }

    if (is(AstNodeKind::EnumDecl))
    {
        const auto enumDecl = castAst<AstEnum>(this, AstNodeKind::EnumDecl);
        return enumDecl->tokenName;
    }

    if (is(AstNodeKind::Impl))
    {
        const auto implDecl = castAst<AstImpl>(this, AstNodeKind::Impl);
        if (implDecl->identifier)
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

bool AstNode::isWhereParam(const SymbolOverload* overload) const
{
    if (!hasAstFlag(AST_IN_CONSTRAINT))
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
    if (overload->hasFlag(OVERLOAD_CONST_VALUE))
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
    printf("%s:%u:%u\n", token.sourceFile->path.cstr(), token.startLocation.line + 1, token.startLocation.column + 1);
}

uint32_t AstNode::childParentIdx() const
{
    SWAG_ASSERT(parent);
    for (uint32_t it = 0; it < parent->childCount(); it++)
    {
        if (parent->children[it] == this)
            return it;
    }

    SWAG_ASSERT(false);
    return UINT32_MAX;
}

void AstNode::addAlternativeScope(Scope* scope, CollectedScopeFlags altFlags)
{
    CollectedScope sv;
    sv.scope = scope;
    sv.flags = altFlags;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock lk(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopes.push_back(sv);
}

void AstNode::addAlternativeScopeVar(Scope* scope, AstNode* varNode, CollectedScopeFlags altFlags)
{
    CollectedScopeVar sv;
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

void AstNode::computeLocation(SourceLocation& start, SourceLocation& end) const
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
    while (find && find->token.isNot(tkn))
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
    for (uint32_t i = 0; i < childCount(); i++)
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
    for (uint32_t i = 0; i < childCount(); i++)
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
