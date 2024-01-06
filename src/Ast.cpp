
#include "pch.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Parser.h"

void Ast::initNewNode(AstNode* node, Parser* parser, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent)
{
    node->kind       = kind;
    node->parent     = parent;
    node->sourceFile = sourceFile;

    if (parser)
    {
        node->tokenId             = parser->token.id;
        node->token.text          = parser->token.text;
        node->token.startLocation = parser->token.startLocation;
        node->token.endLocation   = parser->token.endLocation;
        node->inheritOwnersAndFlags(parser);
    }
    else if (parent)
    {
        node->inheritTokenLocation(parent);
        node->inheritOwners(parent);
    }

    if (parent)
    {
        // Count nodes (not precise). Just to have an hint on the number of bytecode instructions
        if (parent->ownerFct)
            parent->ownerFct->nodeCounts++;

        // Some flags are inherited from the parent, whatever...
        node->flags |= parent->flags & (AST_NO_BACKEND | AST_IN_RUN_BLOCK | AST_IN_MIXIN);

        ScopedLock lk(parent->mutex);
        parent->childs.push_back(node);
    }
}

Utf8 Ast::enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped)
{
    SWAG_ASSERT(typeInfo->isEnum());

    Utf8 result;

    bool found    = false;
    auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);

    if (typeEnum->declNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS)
    {
        SWAG_ASSERT(typeEnum->rawType->isNative());
        for (size_t i = 0; i < typeEnum->values.size(); i++)
        {
            auto value = typeEnum->values[i];
            bool ok    = false;
            SWAG_ASSERT(value->value);
            switch (typeEnum->rawType->nativeType)
            {
            case NativeTypeKind::U8:
                if (value->value->reg.u8 == 0 && reg.u8 == 0)
                    ok = true;
                else if (value->value->reg.u8 == 0)
                    break;
                else if ((value->value->reg.u8 & reg.u8) == value->value->reg.u8)
                    ok = true;
                break;
            case NativeTypeKind::U16:
                if (value->value->reg.u16 == 0 && reg.u16 == 0)
                    ok = true;
                else if (value->value->reg.u16 == 0)
                    break;
                else if ((value->value->reg.u16 & reg.u16) == value->value->reg.u16)
                    ok = true;
                break;
            case NativeTypeKind::U32:
                if (value->value->reg.u32 == 0 && reg.u32 == 0)
                    ok = true;
                else if (value->value->reg.u32 == 0)
                    break;
                else if ((value->value->reg.u32 & reg.u32) == value->value->reg.u32)
                    ok = true;
                break;
            case NativeTypeKind::U64:
                if (value->value->reg.u64 == 0 && reg.u64 == 0)
                    ok = true;
                else if (value->value->reg.u64 == 0)
                    break;
                else if ((value->value->reg.u64 & reg.u64) == value->value->reg.u64)
                    ok = true;
                break;
            default:
                break;
            }

            if (ok)
            {
                found = true;
                if (!result.empty())
                    result += "|";
                if (scoped)
                    result += typeInfo->name;
                result += ".";
                result += value->name;
            }
        }
    }
    else
    {
        if (scoped)
            result = typeInfo->name;
        result += ".";
        for (size_t i = 0; i < typeEnum->values.size(); i++)
        {
            auto value = typeEnum->values[i];
            bool ok    = false;
            SWAG_ASSERT(value->value);
            if (typeEnum->rawType->isNative())
            {
                switch (typeEnum->rawType->nativeType)
                {
                case NativeTypeKind::S8:
                case NativeTypeKind::U8:
                    if (value->value->reg.u8 == reg.u8)
                        ok = true;
                    break;
                case NativeTypeKind::S16:
                case NativeTypeKind::U16:
                    if (value->value->reg.u16 == reg.u16)
                        ok = true;
                    break;
                case NativeTypeKind::S32:
                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                case NativeTypeKind::F32:
                    if (value->value->reg.u32 == reg.u32)
                        ok = true;
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::U64:
                case NativeTypeKind::F64:
                    if (value->value->reg.u64 == reg.u64)
                        ok = true;
                    break;
                case NativeTypeKind::String:
                    if (value->value->text == text)
                        ok = true;
                    break;
                default:
                    break;
                }

                if (ok)
                {
                    found = true;
                    result += value->name;
                    break;
                }
            }
        }
    }

    if (!found)
        result += "?";
    return result;
}

Utf8 Ast::literalToString(TypeInfo* typeInfo, const ComputedValue& value)
{
    Utf8 result;
    result.reserve(value.text.capacity());

    if (typeInfo->isStruct() || typeInfo->isListArray())
    {
        result = Fmt("%u", value.storageOffset);
        return result;
    }

    auto& reg = value.reg;
    typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
    SWAG_ASSERT(typeInfo->isNative());
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        result = Fmt("%u", reg.u8);
        break;
    case NativeTypeKind::U16:
        result = Fmt("%u", reg.u16);
        break;
    case NativeTypeKind::U32:
        result = Fmt("%u", reg.u32);
        break;
    case NativeTypeKind::Rune:
        if (reg.ch < 32)
            result += Fmt("\\x%02x", reg.ch);
        else if (reg.ch > 127 && reg.ch <= 255)
            result += Fmt("\\x%02x", reg.ch);
        else
            result += reg.ch;
        break;
    case NativeTypeKind::U64:
        result = Fmt("%llu", reg.u64);
        break;
    case NativeTypeKind::S8:
        result = Fmt("%d", reg.s8);
        break;
    case NativeTypeKind::S16:
        result = Fmt("%d", reg.s16);
        break;
    case NativeTypeKind::S32:
        result = Fmt("%d", reg.s32);
        break;
    case NativeTypeKind::S64:
        result = Fmt("%lld", reg.s64);
        break;
    case NativeTypeKind::F32:
        result = Utf8::toStringF64(reg.f32);
        break;
    case NativeTypeKind::F64:
        result = Utf8::toStringF64(reg.f64);
        break;
    case NativeTypeKind::Bool:
        result = reg.b ? "true" : "false";
        return result;
    case NativeTypeKind::String:
    {
        for (auto c : value.text)
        {
            if (c < 32)
                result += Fmt("\\x%02x", c);
            else if (c > 127)
                result += Fmt("\\x%02x", c);
            else
                result += c;
        }

        return result;
    }
    default:
        SWAG_ASSERT(false);
        return "";
    }

    return result;
}

void Ast::removeFromParent(AstNode* child)
{
    if (!child)
        return;
    auto parent = child->parent;
    if (!parent)
        return;

    ScopedLock lk(parent->mutex);
    SWAG_RACE_CONDITION_WRITE(parent->raceC);
    auto idx = child->childParentIdx();
    parent->childs.erase(idx);
    child->parent = nullptr;
}

void Ast::insertChild(AstNode* parent, AstNode* child, uint32_t index)
{
    if (!child)
        return;

    if (parent)
    {
        ScopedLock lk(parent->mutex);
        SWAG_RACE_CONDITION_WRITE(parent->raceC);
        parent->childs.insert_at_index(child, index);
    }
    else
    {
        SWAG_ASSERT(index == 0);
    }

    child->parent = parent;
}

void Ast::addChildFront(AstNode* parent, AstNode* child)
{
    if (!child)
        return;

    if (parent)
    {
        ScopedLock lk(parent->mutex);
        SWAG_RACE_CONDITION_WRITE(parent->raceC);
        parent->childs.push_front(child);
        if (!child->ownerScope)
            child->inheritOwners(parent);
    }

    child->parent = parent;
}

void Ast::addChildBack(AstNode* parent, AstNode* child)
{
    if (!child)
        return;

    if (parent)
    {
        ScopedLock lk(parent->mutex);
        SWAG_RACE_CONDITION_WRITE(parent->raceC);
        parent->childs.push_back(child);
        if (!child->ownerScope)
            child->inheritOwners(parent);
    }

    child->parent = parent;
}

void Ast::setForceConstType(AstNode* node)
{
    if (node && node->kind == AstNodeKind::TypeExpression)
        ((AstTypeExpression*) node)->typeFlags |= TYPEFLAG_FORCE_CONST;
}

Scope* Ast::newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName)
{
    if (parentScope)
        return parentScope->getOrAddChild(owner, name, kind, matchName);

    auto newScope         = Allocator::alloc<Scope>();
    newScope->kind        = kind;
    newScope->parentScope = parentScope;
    newScope->owner       = owner;
    newScope->name        = name;
#ifdef SWAG_STATS
    g_Stats.memScopes += sizeof(Scope);
#endif

    return newScope;
}

void Ast::visit(AstNode* root, const function<void(AstNode*)>& fctor)
{
    fctor(root);
    for (auto child : root->childs)
        visit(child, fctor);
}

Ast::VisitResult Ast::visit(ErrorContext* context, AstNode* root, const function<VisitResult(ErrorContext*, AstNode*)>& fctor)
{
    auto result = fctor(context, root);
    if (result == Ast::VisitResult::Stop)
        return Ast::VisitResult::Stop;
    if (result == Ast::VisitResult::NoChilds)
        return Ast::VisitResult::Continue;

    for (auto child : root->childs)
    {
        result = visit(context, child, fctor);
        if (result == Ast::VisitResult::Stop)
            return Ast::VisitResult::Stop;
    }

    return Ast::VisitResult::Continue;
}

AstNode* Ast::cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags, uint64_t removeFlags)
{
    if (!source)
        return nullptr;
    CloneContext cloneContext;
    cloneContext.parent      = parent;
    cloneContext.forceFlags  = forceFlags;
    cloneContext.removeFlags = removeFlags;
    cloneContext.cloneFlags |= CLONE_RAW;
    return source->clone(cloneContext);
}

AstNode* Ast::clone(AstNode* source, AstNode* parent, uint64_t forceFlags, uint64_t removeFlags)
{
    if (!source)
        return nullptr;
    CloneContext cloneContext;
    cloneContext.parent      = parent;
    cloneContext.forceFlags  = forceFlags;
    cloneContext.removeFlags = removeFlags;
    return source->clone(cloneContext);
}

void Ast::normalizeIdentifierName(Utf8& name)
{
    auto len = name.length();
    auto pz  = name.buffer;

    for (uint32_t i = 0; i < len; i++)
    {
        switch (*pz)
        {
        case '\'':
        case '.':
        case '(':
        case ')':
        case '-':
        case ',':
        case ' ':
            *pz = '_';
            break;

        case '&':
            *pz = 'R';
            break;
        case '*':
        case '>':
            *pz = 'P';
            break;
        case '[':
        case ']':
            *pz = 'A';
            break;
        }

        pz++;
    }
}

Utf8 Ast::computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params)
{
    if (params.empty())
        return "";

    Utf8 result;
    for (int i = 0; i < (int) params.size(); i++)
    {
        auto param = params[i];
        if (param->name == param->typeInfo->name)
            continue;

        if (result.empty())
            result = "with ";
        else
            result += ", ";
        result += param->name;
        result += " = ";
        result += param->typeInfo->name;
    }

    return result;
}

Vector<Utf8> Ast::computeGenericParametersReplacement(VectorMap<Utf8, TypeInfo*>& replace)
{
    if (!replace.size())
        return {};

    Vector<Utf8> result;
    Utf8         remark;
    for (auto p : replace)
    {
        // Can occur in case of constants (like string for example)
        if (p.first == p.second->getDisplayName())
            continue;
        if (p.first.length() > 2 && p.first[0] == '_' && p.first[1] == '_') // Generated name
            continue;

        remark = "with ";
        remark += Fmt("%s = %s", p.first.c_str(), p.second->getDisplayNameC());
        result.push_back(remark);
    }

    return result;
}
