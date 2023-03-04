#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Parser.h"
#include "ThreadManager.h"

void AstNode::inheritOrFlag(uint64_t flag)
{
    for (auto child : childs)
        flags |= child->flags & flag;
}

void AstNode::inheritOrFlag(AstNode* op, uint64_t flag)
{
    if (!op)
        return;
    flags |= op->flags & flag;
}

void AstNode::inheritAndFlag1(uint64_t flag)
{
    inheritAndFlag1(this, flag);
}

void AstNode::inheritAndFlag2(uint64_t flag1, uint64_t flag2)
{
    inheritAndFlag2(this, flag1, flag2);
}

void AstNode::inheritAndFlag3(uint64_t flag1, uint64_t flag2, uint64_t flag3)
{
    inheritAndFlag3(this, flag1, flag2, flag3);
}

void AstNode::inheritAndFlag1(AstNode* who, uint64_t flag)
{
    for (auto child : who->childs)
    {
        if (!(child->flags & flag))
            return;
    }

    flags |= flag;
}

void AstNode::inheritAndFlag2(AstNode* who, uint64_t flag1, uint64_t flag2)
{
    flags |= flag1;
    flags |= flag2;

    for (auto child : who->childs)
    {
        if (!(child->flags & flag1))
            flags &= ~flag1;
        if (!(child->flags & flag2))
            flags &= ~flag2;
        if (!(flags & (flag1 | flag2)))
            return;
    }
}

void AstNode::inheritAndFlag3(AstNode* who, uint64_t flag1, uint64_t flag2, uint64_t flag3)
{
    flags |= flag1;
    flags |= flag2;
    flags |= flag3;

    for (auto child : who->childs)
    {
        if (!(child->flags & flag1))
            flags &= ~flag1;
        if (!(child->flags & flag2))
            flags &= ~flag2;
        if (!(child->flags & flag3))
            flags &= ~flag3;
        if (!(flags & (flag1 | flag2 | flag3)))
            return;
    }
}

void AstNode::inheritTokenName(Token& tkn)
{
    SWAG_ASSERT(!tkn.text.empty());
    token.text = move(tkn.text);
}

void AstNode::inheritTokenLocation(Token& tkn)
{
    token.startLocation = tkn.startLocation;
    token.endLocation   = tkn.endLocation;
}

void AstNode::inheritTokenLocation(AstNode* node)
{
    token.startLocation = node->token.startLocation;
    token.endLocation   = node->token.endLocation;
    sourceFile          = node->sourceFile;
}

void AstNode::inheritOwners(AstNode* op)
{
    if (!op)
        return;
    ownerStructScope = op->ownerStructScope;
    ownerScope       = op->ownerScope;
    ownerFct         = op->ownerFct;
    ownerBreakable   = op->ownerBreakable;
    ownerInline      = op->ownerInline;

    if (op->hasExtOwner() && op->extOwner()->ownerCompilerIfBlock)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerCompilerIfBlock = op->extOwner()->ownerCompilerIfBlock;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerCompilerIfBlock = nullptr;
    }
}

void AstNode::inheritOwnersAndFlags(Parser* parser)
{
    ownerStructScope = parser->currentStructScope;
    ownerScope       = parser->currentScope;
    ownerFct         = parser->currentFct;
    ownerBreakable   = parser->currentBreakable;
    ownerInline      = parser->currentInline;

    if (parser->currentCompilerIfBlock)
    {
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerCompilerIfBlock = parser->currentCompilerIfBlock;
    }
    else if (hasExtOwner())
    {
        extOwner()->ownerCompilerIfBlock = nullptr;
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

    flags |= parser->currentFlags;
}

void AstNode::allocateComputedValue()
{
    if (!computedValue)
    {
        computedValue = Allocator::alloc<ComputedValue>();
#ifdef SWAG_STATS
        g_Stats.memNodesLiteral += sizeof(ComputedValue);
#endif
    }
}

void AstNode::setFlagsValueIsComputed()
{
    allocateComputedValue();
    flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE;
}

void AstNode::inheritComputedValue(AstNode* from)
{
    if (!from)
        return;
    inheritOrFlag(from, AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO);
    if (flags & AST_VALUE_COMPUTED)
    {
        flags |= AST_CONST_EXPR | AST_R_VALUE;
        SWAG_ASSERT(from->computedValue);
        computedValue = from->computedValue;
    }
}

bool AstNode::isGeneratedSelf()
{
    return kind == AstNodeKind::FuncDeclParam && specFlags & AstVarDecl::SPECFLAG_GENERATED_SELF;
}

bool AstNode::isEmptyFct()
{
    return kind == AstNodeKind::FuncDecl && specFlags & AstFuncDecl::SPECFLAG_EMPTY_FCT;
}

bool AstNode::isFunctionCall()
{
    if (kind == AstNodeKind::FuncCall)
        return true;
    if (kind != AstNodeKind::Identifier)
        return false;

    auto id = CastAst<AstIdentifier>(this, AstNodeKind::Identifier);
    return id->callParameters != nullptr;
}

bool AstNode::isPublic()
{
    auto checkNode = this;
    while (checkNode)
    {
        if (checkNode->attributeFlags & ATTRIBUTE_PUBLIC)
            return true;
        checkNode = checkNode->parent;
    }

    return false;
}

bool AstNode::isConstantTrue()
{
    return (flags & AST_VALUE_COMPUTED) && computedValue->reg.b;
}

bool AstNode::isConstantFalse()
{
    return (flags & AST_VALUE_COMPUTED) && !computedValue->reg.b;
}

bool AstNode::forceTakeAddress()
{
    if ((flags & AST_TAKE_ADDRESS) && !(semFlags & SEMFLAG_FORCE_NO_TAKE_ADDRESS))
        return true;
    if (semFlags & SEMFLAG_FORCE_TAKE_ADDRESS)
        return true;
    return false;
}

void AstNode::swap2Childs()
{
    SWAG_ASSERT(childs.size() == 2);
    auto tmp  = childs[0];
    childs[0] = childs[1];
    childs[1] = tmp;
}

bool AstNode::hasSpecialFuncCall()
{
    return hasExtMisc() &&
           extMisc()->resolvedUserOpSymbolOverload &&
           extMisc()->resolvedUserOpSymbolOverload->symbol->kind == SymbolKind::Function;
}

bool AstNode::hasSpecialFuncCall(const Utf8& name)
{
    return hasExtMisc() &&
           extMisc()->resolvedUserOpSymbolOverload &&
           extMisc()->resolvedUserOpSymbolOverload->symbol->kind == SymbolKind::Function &&
           extMisc()->resolvedUserOpSymbolOverload->symbol->name == name;
}

AstNode* AstNode::inSimpleReturn()
{
    auto test = parent;
    if (!test)
        return nullptr;
    if (test->kind == AstNodeKind::Return)
        return test;
    if (test->kind == AstNodeKind::Try && test->parent->kind == AstNodeKind::Return)
        return test->parent;
    if (test->kind == AstNodeKind::Catch && test->parent->kind == AstNodeKind::Return)
        return test->parent;
    return nullptr;
}

bool AstNode::isSpecialFunctionGenerated()
{
    if (!hasExtByteCode() || !extByteCode()->bc || !extByteCode()->bc->isCompilerGenerated)
        return false;
    return true;
}

bool AstNode::isSpecialFunctionName()
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

void AstNode::allocateExtension(ExtensionKind extensionKind)
{
    ScopedLock lk(mutex);
    allocateExtensionNoLock(extensionKind);
}

void AstNode::allocateExtensionNoLock(ExtensionKind extensionKind)
{
    if (!extension)
    {
        extension = Allocator::alloc<Extension>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(Extension));
#endif
    }

    switch (extensionKind)
    {
    case ExtensionKind::ByteCode:
        if (hasExtByteCode())
            return;
        extension->bytecode = Allocator::alloc<ExtensionByteCode>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(ExtensionByteCode));
#endif
        break;

    case ExtensionKind::Semantic:
        if (hasExtSemantic())
            return;
        extension->semantic = Allocator::alloc<ExtensionSemantic>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(ExtensionSemantic));
#endif
        break;

    case ExtensionKind::Owner:
        if (hasExtOwner())
            return;
        extension->owner = Allocator::alloc<ExtensionOwner>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(ExtensionOwner));
#endif
        break;

    default:
        if (hasExtMisc())
            return;
        extension->misc = Allocator::alloc<ExtensionMisc>();
#ifdef SWAG_STATS
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(ExtensionMisc));
#endif
        break;
    }
}

bool AstNode::isConstant0()
{
    SWAG_ASSERT(typeInfo);
    if (!typeInfo->isNative())
        return false;
    if (!(flags & AST_VALUE_COMPUTED))
        return false;

    switch (typeInfo->sizeOf)
    {
    case 1:
        return computedValue->reg.u8 == 0;
    case 2:
        return computedValue->reg.u16 == 0;
    case 4:
        return computedValue->reg.u32 == 0;
    case 8:
        return computedValue->reg.u64 == 0;
    }

    return false;
}

bool AstNode::isConstant1()
{
    SWAG_ASSERT(typeInfo);
    if (!typeInfo->isNative())
        return false;
    if (!(flags & AST_VALUE_COMPUTED))
        return false;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        return computedValue->reg.u8 == 1;
    case NativeTypeKind::U16:
    case NativeTypeKind::S16:
        return computedValue->reg.u16 == 1;
    case NativeTypeKind::U32:
    case NativeTypeKind::S32:
    case NativeTypeKind::Rune:
        return computedValue->reg.u32 == 1;
    case NativeTypeKind::U64:
    case NativeTypeKind::S64:
        return computedValue->reg.u64 == 1;
    case NativeTypeKind::F32:
        return computedValue->reg.f32 == 1;
    case NativeTypeKind::F64:
        return computedValue->reg.f64 == 1;
    }

    return false;
}

void AstNode::setPassThrough()
{
    semanticFct = nullptr;
    byteCodeFct = ByteCodeGenJob::emitPassThrough;
    if (hasExtSemantic())
    {
        extSemantic()->semanticAfterFct  = nullptr;
        extSemantic()->semanticBeforeFct = nullptr;
    }

    if (hasExtByteCode())
    {
        extByteCode()->byteCodeAfterFct  = nullptr;
        extByteCode()->byteCodeBeforeFct = nullptr;
    }
}

bool AstNode::isParentOf(AstNode* child)
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

bool AstNode::isValidIfParam(SymbolOverload* overload)
{
    if (!(flags & AST_IN_VALIDIF))
        return false;
    if (!overload)
        return false;
    if (!(overload->flags & OVERLOAD_VAR_FUNC_PARAM))
        return false;
    if (isSameStackFrame(overload))
        return false;
    return true;
}

bool AstNode::isSameStackFrame(SymbolOverload* overload)
{
    if (overload->symbol->kind != SymbolKind::Variable)
        return true;
    if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        return true;
    if (overload->flags & OVERLOAD_VAR_INLINE && !ownerInline)
        return false;
    if (overload->flags & OVERLOAD_VAR_INLINE && ownerInline->ownerFct != ownerFct)
        return false;
    if (!(overload->flags & OVERLOAD_VAR_FUNC_PARAM) && !(overload->flags & OVERLOAD_VAR_LOCAL))
        return true;

    auto nodeVar = overload->node;
    if ((flags & AST_RUN_BLOCK) != (nodeVar->flags & AST_RUN_BLOCK))
        return false;
    if (ownerFct != nodeVar->ownerFct)
        return false;

    return true;
}

uint32_t AstNode::childParentIdx()
{
    SWAG_ASSERT(parent);
    for (uint32_t it = 0; it < parent->childs.size(); it++)
    {
        if (parent->childs[it] == this)
            return it;
    }

    SWAG_ASSERT(false);
    return UINT32_MAX;
}

void AstNode::addAlternativeScope(Scope* scope, uint32_t altFlags)
{
    AlternativeScope sv;
    sv.scope = scope;
    sv.flags = altFlags;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopes.push_back(sv);
}

void AstNode::addAlternativeScopeVar(Scope* scope, AstNode* varNode, uint32_t altFlags)
{
    AlternativeScopeVar sv;
    sv.scope    = scope;
    sv.node     = varNode;
    sv.leafNode = varNode;
    sv.flags    = altFlags;

    allocateExtension(ExtensionKind::Misc);
    ScopedLock(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopesVars.push_back(sv);
}

void AstNode::addAlternativeScopes(const VectorNative<AlternativeScope>& scopes)
{
    allocateExtension(ExtensionKind::Misc);
    ScopedLock(extMisc()->mutexAltScopes);
    extMisc()->alternativeScopes.append(scopes);
}

void AstNode::computeLocation(SourceLocation& start, SourceLocation& end)
{
    start = token.startLocation;
    end   = token.endLocation;
    for (auto p : childs)
    {
        if (p->kind == AstNodeKind::Statement)
            break;
        if (p->kind == AstNodeKind::Inline)
            break;
        if (p->flags & AST_GENERATED)
            continue;
        if (p->kind == AstNodeKind::FuncDeclType && p->childs.empty())
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

Utf8 AstNode::getScopedName()
{
    auto& fullName = ownerScope->getFullName();
    if (fullName.empty())
        return token.text;
    Utf8 result{fullName, fullName.length() + token.text.length() + 1};
    result += ".";
    result += token.text;
    return result;
}

AstNode* AstNode::findParent(AstNodeKind parentKind)
{
    auto find = parent;
    while (find && find->kind != parentKind)
        find = find->parent;
    return find;
}

AstNode* AstNode::findParent(AstNodeKind parentKind1, AstNodeKind parentKind2)
{
    auto find = parent;
    while (find && find->kind != parentKind1 && find->kind != parentKind2)
        find = find->parent;
    return find;
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
    if (this == ref)
        return fromChild;
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

void AstIdentifierRef::computeName()
{
    token.text.clear();
    for (auto child : childs)
    {
        if (!token.text.empty())
            token.text += ".";
        token.text += child->token.text;
    }
}

bool AstFuncDecl::mustInline()
{
    if (attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
        return true;
    if (sourceFile->module->buildCfg.byteCodeInline == false)
        return false;
    if (attributeFlags & ATTRIBUTE_INLINE)
        return true;
    if (!content)
        return false;

    if (sourceFile->module->buildCfg.byteCodeAutoInline == false)
        return false;
    if (attributeFlags & ATTRIBUTE_NO_INLINE)
        return false;

    // All short functions
    if (specFlags & AstFuncDecl::SPECFLAG_SHORT_FORM)
        return true;

    return false;
}

Utf8 AstFuncDecl::getCallName()
{
    if (attributeFlags & (ATTRIBUTE_FOREIGN | ATTRIBUTE_PUBLIC))
    {
        if (!(attributeFlags & ATTRIBUTE_SHARP_FUNC))
        {
            computeFullNameForeign(true);
            return fullnameForeign;
        }
    }

    SWAG_ASSERT(hasExtByteCode() && extByteCode()->bc);
    return extByteCode()->bc->getCallName();
}

Utf8 AstFuncDecl::getNameForUserCompiler()
{
    if (attributeFlags & ATTRIBUTE_SHARP_FUNC)
    {
        auto fct = parent;
        while (fct && (fct->kind != AstNodeKind::FuncDecl || fct->attributeFlags & ATTRIBUTE_SHARP_FUNC))
            fct = fct->parent;
        if (fct)
            return fct->getScopedName();
    }

    return getScopedName();
}

const char* AstFuncDecl::getDisplayNameC()
{
    auto res = getDisplayName();
    return _strdup(res.c_str()); // Leak and slow, but only for messages
}

Utf8 AstFuncDecl::getDisplayName()
{
    if (attributeFlags & ATTRIBUTE_AST_FUNC)
        return "'#ast' block";
    if (attributeFlags & (ATTRIBUTE_RUN_FUNC | ATTRIBUTE_RUN_GENERATED_FUNC))
        return "'#run' block";
    if (attributeFlags & ATTRIBUTE_MATCH_VALIDIF_FUNC)
        return "'#validif' block";
    if (attributeFlags & ATTRIBUTE_MATCH_VALIDIFX_FUNC)
        return "'#validifx' block";

    if (attributeFlags & ATTRIBUTE_TEST_FUNC && attributeFlags & ATTRIBUTE_SHARP_FUNC)
        return "'#test' block";
    if (attributeFlags & ATTRIBUTE_MAIN_FUNC)
        return "'#main' block";
    if (attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        return "'#message' block";
    if (attributeFlags & ATTRIBUTE_INIT_FUNC)
        return "'#init' block";
    if (attributeFlags & ATTRIBUTE_DROP_FUNC)
        return "'#drop' block";
    if (attributeFlags & ATTRIBUTE_PREMAIN_FUNC)
        return "'#premain' block";

    if (specFlags & AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION)
        return "lambda";

    if (attributeFlags & ATTRIBUTE_SHARP_FUNC)
        return Fmt("'%s' block", token.ctext());

    if (attributeFlags & ATTRIBUTE_MIXIN)
        return Fmt("mixin '%s'", token.ctext());
    if (attributeFlags & ATTRIBUTE_MACRO)
        return Fmt("macro '%s'", token.ctext());

    return Fmt("function '%s'", token.ctext());
}

void AstFuncDecl::computeFullNameForeign(bool forExport)
{
    ScopedLock lk(mutex);
    if (!fullnameForeign.empty())
        return;

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
    if (!forExport)
    {
        auto value = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
        if (value && !value->text.empty())
            fullnameForeign = value->text;
        else
            fullnameForeign = token.text;
        return;
    }

    SWAG_ASSERT(ownerScope);

    auto nameForeign = getScopedName();

    // If the symbol has overloads, i.e. more than one definition, then we
    // append the type
    if (resolvedSymbolName && resolvedSymbolName->overloads.size() > 1)
    {
        // Empty (forward) decl are counted as overloads, so be sure it's not a bunch of
        // empty functions.
        uint32_t countNoEmpty = 0;
        for (auto r : resolvedSymbolName->overloads)
        {
            if (!r->node->isEmptyFct())
            {
                countNoEmpty++;
                if (countNoEmpty > 1)
                    break;
            }
        }

        if (countNoEmpty > 1)
        {
            nameForeign += "@@";
            typeFunc->computeScopedName();
            auto pz = strstr(typeFunc->scopedName.c_str(), "(");
            SWAG_ASSERT(pz);
            nameForeign += pz;
        }
    }

    fullnameForeign = nameForeign;

    // Normalize token.text
    auto len = nameForeign.length();
    auto pz  = nameForeign.buffer;
    auto pzd = fullnameForeign.buffer;
    for (int i = 0; i < len; i++)
    {
        if (*pz == ' ')
        {
            pz++;
        }
        else if (*pz == ',' || *pz == '\'' || *pz == '-' || *pz == '>')
        {
            *pzd++ = '@';
            pz++;
        }
        else
            *pzd++ = *pz++;
    }

    *pzd++ = 0;

    fullnameForeign.count = (uint32_t) (pzd - fullnameForeign.buffer) - 1;
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
        for (auto s : childs)
            s->setOwnerAttrUse(attrUse);
        break;

    default:
        allocateExtension(ExtensionKind::Owner);
        extOwner()->ownerAttrUse = attrUse;
        break;
    }
}