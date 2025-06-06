#include "pch.h"
#include "Syntax/Ast.h"
#include "Format/FormatAst.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Parser/Parser.h"

void Ast::initNewNode(AstNodeKind kind, AstNode* node, Parser* parser, AstNode* parent)
{
    node->kind   = kind;
    node->parent = parent;

    if (parser)
    {
        node->token = parser->tokenParse.token;
        node->inheritOwnersAndFlags(parser);
        if (!parser->freezeFormat && !node->flags.has(AST_GENERATED))
        {
            FormatAst::inheritFormatBefore(parser, node, &parser->tokenParse);
            FormatAst::inheritFormatAfter(parser, node, &parser->tokenParse);
        }
    }
    else if (parent)
    {
        node->token = parent->token;
        node->inheritOwners(parent);
    }

    if (parent)
    {
        // Count nodes (not precise). Just to have a hint on the number of bytecode instructions
        if (parent->ownerFct)
            parent->ownerFct->nodeCounts++;

        // Some flags are inherited from the parent, whatever...
        node->addAstFlag(parent->flags.mask(AST_NO_BACKEND | AST_IN_RUN_BLOCK | AST_IN_MIXIN));

        ScopedLock lk(parent->mutex);
        parent->children.push_back(node);
    }
}

Utf8 Ast::enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped)
{
    SWAG_ASSERT(typeInfo->isEnum());

    Utf8 result;

    bool       found    = false;
    const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);

    if (typeEnum->declNode->hasAttribute(ATTRIBUTE_ENUM_FLAGS))
    {
        SWAG_ASSERT(typeEnum->rawType->isNative());
        for (uint32_t i = 0; i < typeEnum->values.size(); i++)
        {
            const auto value = typeEnum->values[i];
            bool       ok    = false;
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
        for (uint32_t i = 0; i < typeEnum->values.size(); i++)
        {
            const auto value = typeEnum->values[i];
            SWAG_ASSERT(value->value);
            if (typeEnum->rawType->isNative())
            {
                bool ok = false;
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

Utf8 Ast::literalToString(const TypeInfo* typeInfo, const ComputedValue& value)
{
    Utf8 result;
    result.reserve(value.text.capacity());

    if (typeInfo->isStruct() || typeInfo->isListArray())
    {
        result = form("%u", value.storageOffset);
        return result;
    }

    auto& reg = value.reg;
    typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
    SWAG_ASSERT(typeInfo->isNative());
    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
            result = form("%u", reg.u8);
            break;
        case NativeTypeKind::U16:
            result = form("%u", reg.u16);
            break;
        case NativeTypeKind::U32:
            result = form("%u", reg.u32);
            break;
        case NativeTypeKind::Rune:
            if (reg.ch < 32)
                result += form("\\x%02x", reg.ch);
            else if (reg.ch > 127 && reg.ch <= 255)
                result += form("\\x%02x", reg.ch);
            else
                result += reg.ch;
            break;
        case NativeTypeKind::U64:
            result = form("%llu", reg.u64);
            break;
        case NativeTypeKind::S8:
            result = form("%d", reg.s8);
            break;
        case NativeTypeKind::S16:
            result = form("%d", reg.s16);
            break;
        case NativeTypeKind::S32:
            result = form("%d", reg.s32);
            break;
        case NativeTypeKind::S64:
            result = form("%lld", reg.s64);
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
            for (const auto c : value.text)
            {
                if (c < 32)
                    result += form("\\x%02x", c);
                else if (static_cast<uint8_t>(c) > 127)
                    result += form("\\x%02x", c);
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
    const auto parent = child->parent;
    if (!parent)
        return;

    ScopedLock lk(parent->mutex);
    SWAG_RACE_CONDITION_WRITE(parent->raceC);
    const auto idx = child->childParentIdx();
    parent->children.erase(idx);
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
        parent->children.insert_at_index(child, index);
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
        parent->children.push_front(child);
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
        parent->children.push_back(child);
        if (!child->ownerScope)
            child->inheritOwners(parent);
    }

    child->parent = parent;
}

void Ast::setForceConstType(AstNode* node)
{
    if (node && node->is(AstNodeKind::TypeExpression))
        castAst<AstTypeExpression>(node)->typeFlags.add(TYPE_FLAG_FORCE_CONST);
}

Scope* Ast::newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName)
{
    if (parentScope)
        return parentScope->getOrAddChild(owner, name, kind, matchName);
    return Scope::allocScope(nullptr, owner, name, kind);
}

void Ast::visit(AstNode* root, const std::function<void(AstNode*)>& fct)
{
    fct(root);
    for (const auto child : root->children)
        visit(child, fct);
}

Ast::VisitResult Ast::visit(ErrorContext* context, AstNode* root, const std::function<VisitResult(ErrorContext*, AstNode*)>& fct)
{
    auto result = fct(context, root);
    if (result == Stop)
        return Stop;
    if (result == NoChildren)
        return Continue;

    for (const auto child : root->children)
    {
        result = visit(context, child, fct);
        if (result == Stop)
            return Stop;
    }

    return Continue;
}

AstNode* Ast::cloneRaw(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags, const AstNodeFlags& removeFlags)
{
    if (!source)
        return nullptr;
    CloneContext cloneContext;
    cloneContext.parent      = parent;
    cloneContext.forceFlags  = forceFlags;
    cloneContext.removeFlags = removeFlags;
    cloneContext.cloneFlags.add(CLONE_RAW);
    return source->clone(cloneContext);
}

AstNode* Ast::clone(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags, const AstNodeFlags& removeFlags)
{
    if (!source)
        return nullptr;
    CloneContext cloneContext;
    cloneContext.parent      = parent;
    cloneContext.forceFlags  = forceFlags;
    cloneContext.removeFlags = removeFlags;
    return source->clone(cloneContext);
}

void Ast::normalizeIdentifierName(const Utf8& name)
{
    const auto len = name.length();
    auto       pz  = name.buffer;

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
            default:
                break;
        }

        pz++;
    }
}

void Ast::setDiscard(AstNode* node)
{
    // Mark the identifier with AST_DISCARD
    while (node && node->isNot(AstNodeKind::IdentifierRef))
        node = node->firstChild();
    // This is where AST_DISCARD will be really used
    for (const auto c : node->children)
    {
        if (c->isNot(AstNodeKind::Identifier))
            break;
        c->addAstFlag(AST_DISCARD);
    }
}
