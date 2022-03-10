#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "LanguageSpec.h"

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
    ownerStructScope     = op->ownerStructScope;
    ownerScope           = op->ownerScope;
    ownerFct             = op->ownerFct;
    ownerBreakable       = op->ownerBreakable;
    ownerInline          = op->ownerInline;
    ownerCompilerIfBlock = op->ownerCompilerIfBlock;
}

void AstNode::inheritOwnersAndFlags(SyntaxJob* job)
{
    ownerStructScope     = job->currentStructScope;
    ownerScope           = job->currentScope;
    ownerFct             = job->currentFct;
    ownerBreakable       = job->currentBreakable;
    ownerCompilerIfBlock = job->currentCompilerIfBlock;
    ownerInline          = job->currentInline;

    if (job->currentTryCatchAssume)
    {
        allocateExtension();
        extension->ownerTryCatchAssume = job->currentTryCatchAssume;
    }
    else if (extension)
    {
        extension->ownerTryCatchAssume = nullptr;
    }

    flags |= job->currentFlags;
}

void AstNode::allocateComputedValue()
{
    if (!computedValue)
        computedValue = g_Allocator.alloc<ComputedValue>();
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
    if ((flags & AST_TAKE_ADDRESS) && !(semFlags & AST_SEM_FORCE_NO_TAKE_ADDRESS))
        return true;
    if (semFlags & AST_SEM_FORCE_TAKE_ADDRESS)
        return true;
    return false;
}

void AstNode::swap2Childs()
{
    SWAG_ASSERT(childs.size() == 2);
    auto tmp                  = childs[0];
    childs[0]                 = childs[1];
    childs[1]                 = tmp;
    childs[0]->childParentIdx = 0;
    childs[1]->childParentIdx = 1;
}

bool AstNode::hasSpecialFuncCall()
{
    return extension &&
           extension->resolvedUserOpSymbolOverload &&
           extension->resolvedUserOpSymbolOverload->symbol->kind == SymbolKind::Function;
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
    if (!extension || !extension->bc || !extension->bc->isCompilerGenerated)
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

void AstNode::allocateExtension()
{
    if (extension)
        return;
    extension = g_Allocator.alloc<Extension>();
    if (g_CommandLine->stats)
        g_Stats.memNodesExt += Allocator::alignSize(sizeof(Extension));
}

bool AstNode::mustInline()
{
    if (attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
        return true;
    if (!(attributeFlags & ATTRIBUTE_INLINE))
        return false;
    if (sourceFile->module->buildCfg.byteCodeInline == false)
        return false;
    return true;
}

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
    if (typeInfo->kind != TypeInfoKind::Native)
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
    case NativeTypeKind::UInt:
    case NativeTypeKind::Int:
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
    if (extension)
    {
        extension->semanticAfterFct  = nullptr;
        extension->semanticBeforeFct = nullptr;
        extension->byteCodeAfterFct  = nullptr;
        extension->byteCodeBeforeFct = nullptr;
    }
}

bool AstNode::isParentOf(AstNode* child)
{
    while (child->parent)
    {
        if (child->parent == this)
            return true;
        child = child->parent;
    }

    return false;
}

bool AstNode::isSelectIfParam(SymbolOverload* overload)
{
    if (!(flags & AST_IN_SELECTIF))
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

void AstNode::addAlternativeScope(Scope* scope, uint32_t altFlags)
{
    AlternativeScope sv;
    sv.scope = scope;
    sv.flags = altFlags;

    allocateExtension();
    extension->alternativeScopes.push_back(sv);
}

void AstNode::addAlternativeScopeVar(Scope* scope, AstNode* varNode, uint32_t altFlags)
{
    AlternativeScopeVar sv;
    sv.scope    = scope;
    sv.node     = varNode;
    sv.leafNode = varNode;
    sv.flags    = altFlags;

    allocateExtension();
    extension->alternativeScopesVars.push_back(sv);
}

void AstNode::addAlternativeScopes(const VectorNative<AlternativeScope>& scopes)
{
    allocateExtension();
    extension->alternativeScopes.append(scopes);
}

void AstNode::computeEndLocation()
{
    for (auto p : childs)
    {
        if (p->kind == AstNodeKind::Statement)
            break;
        p->computeEndLocation();
    }

    switch (kind)
    {
    case AstNodeKind::MakePointer:
    case AstNodeKind::MakePointerLambda:
        token.endLocation = childs.back()->token.endLocation;
        break;

    case AstNodeKind::ArrayPointerIndex:
        if (childs.size() < 2)
            break;
        if (specFlags & AST_SPEC_ARRAYPTRIDX_ISDEREF)
        {
            token.endLocation = childs.front()->token.endLocation;
            break;
        }

        token.endLocation   = childs.back()->token.endLocation;
        token.startLocation = childs.front()->token.startLocation;
        break;

    case AstNodeKind::IdentifierRef:
    case AstNodeKind::ArrayPointerSlicing:
        if (childs.empty())
            break;
        token.startLocation = childs.front()->token.startLocation;
        token.endLocation   = childs.back()->token.endLocation;
        break;
    }
}

Utf8 AstNode::getScopedName()
{
    auto& fullName = ownerScope->getFullName();
    if (fullName.empty())
        return token.text;
    auto result = fullName;
    result += ".";
    result += token.text;
    return result;
}

Utf8 AstNode::getArticleKindName(AstNode* node)
{
    Utf8 result = getKindName(node);
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::FuncDeclType:
    case AstNodeKind::Namespace:
    case AstNodeKind::FuncDeclParam:
    case AstNodeKind::StructDecl:
        return "a " + result;
    case AstNodeKind::Alias:
    case AstNodeKind::EnumDecl:
    case AstNodeKind::EnumValue:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::Impl:
    case AstNodeKind::AttrDecl:
    case AstNodeKind::Identifier:
    case AstNodeKind::IntrinsicProp:
        return "an " + result;
    }

    return "node";
}

Utf8 AstNode::getKindName(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::VarDecl:
        if (node->ownerScope && node->ownerScope->isGlobal())
            return "global variable";
        if (node->flags & AST_STRUCT_MEMBER)
            return "struct member";
        if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM)
            return "function parameter";
        return "local variable";
    case AstNodeKind::ConstDecl:
        return "constant";
    case AstNodeKind::FuncDecl:
    case AstNodeKind::FuncDeclType:
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
        return "struct";
    case AstNodeKind::InterfaceDecl:
        return "interface";
    case AstNodeKind::Impl:
        return "implementation block";
    case AstNodeKind::Identifier:
        return "identifier";
    case AstNodeKind::IntrinsicProp:
        return "intrinsic";
    case AstNodeKind::FuncCall:
        return "function call";
    case AstNodeKind::TypeExpression:
        return "type";
    }

    return "node";
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
        return "`#ast` block";
    if (attributeFlags & (ATTRIBUTE_RUN_FUNC | ATTRIBUTE_RUN_GENERATED_FUNC))
        return "`#run` block";
    if (attributeFlags & ATTRIBUTE_SELECTIF_FUNC)
        return "`#selectif` block";

    if (attributeFlags & ATTRIBUTE_TEST_FUNC)
        return "`#test` block";
    if (attributeFlags & ATTRIBUTE_MAIN_FUNC)
        return "`#main` block";
    if (attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        return "`#message` block";
    if (attributeFlags & ATTRIBUTE_INIT_FUNC)
        return "`#init` block";
    if (attributeFlags & ATTRIBUTE_DROP_FUNC)
        return "`#drop` block";

    if (flags & AST_IS_LAMBDA_EXPRESSION)
        return "lambda";

    if (flags & AST_SPECIAL_COMPILER_FUNC)
        return Fmt("`%s` block", token.ctext());

    if (attributeFlags & ATTRIBUTE_MIXIN)
        return Fmt("mixin `%s`", token.ctext());
    if (attributeFlags & ATTRIBUTE_MACRO)
        return Fmt("macro `%s`", token.ctext());

    return Fmt("function `%s`", token.ctext());
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
            if (!(r->node->flags & AST_EMPTY_FCT))
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
        allocateExtension();
        extension->ownerAttrUse = attrUse;
        break;
    }
}
