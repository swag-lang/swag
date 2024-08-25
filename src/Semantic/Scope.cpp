#include "pch.h"
#include "Semantic/Scope.h"
#include "Syntax/Ast.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

Scope::Scope()
{
    symTable.scope = this;
}

void Scope::release()
{
    symTable.release();
    // Allocator::free<Scope>(this);
}

bool Scope::isGlobal() const
{
    return kind == ScopeKind::Module || kind == ScopeKind::File || kind == ScopeKind::Namespace;
}

bool Scope::isTopLevel() const
{
    return kind == ScopeKind::Module || kind == ScopeKind::File || (kind == ScopeKind::Namespace && flags.has(SCOPE_AUTO_GENERATED));
}

bool Scope::isGlobalOrImpl() const
{
    if (isGlobal() || kind == ScopeKind::Impl)
        return true;
    if (kind == ScopeKind::Struct || kind == ScopeKind::Enum)
        return !parentScope || parentScope->isGlobal() || parentScope->is(ScopeKind::Impl);
    return false;
}

const Utf8& Scope::getFullName()
{
    if (flags.has(SCOPE_FILE))
        return name;

    {
        SharedLock lk(mutex);
        if (!fullname.empty())
            return fullname;
    }

    {
        ScopedLock lk(mutex);
        if (!fullname.empty())
            return fullname;
        if (parentScope)
            makeFullName(fullname, parentScope->getFullName(), name);
        else
            fullname = name;
        return fullname;
    }
}

Utf8 Scope::getDisplayFullName()
{
    if (flags.has(SCOPE_FILE))
        return name;
    if (!parentScope)
        return name;
    if (!parentScope->isGlobalOrImpl())
        return name;

    Utf8 result;
    makeFullName(result, parentScope->getFullName(), name);
    return result;
}

void Scope::makeFullName(Utf8& result, const Utf8& parentName, const Utf8& name)
{
    if (parentName.empty())
        result = name;
    else
    {
        result.reserve(parentName.length() + name.length() + 2);
        result = parentName + "." + name;
    }
}

void Scope::collectScopeFromToExcluded(Scope* src, const Scope* to, VectorNative<Scope*>& result)
{
    result.clear();
    while (true)
    {
        result.push_back(src);
        src = src->parentScope;
        if (src == to)
            return;
        SWAG_ASSERT(src);
    }
}

void Scope::allocPublicSet()
{
    if (!publicSet)
        publicSet = Allocator::alloc<ScopePublicSet>();

    auto ptrScope = this;
    while (ptrScope && !ptrScope->flags.has(SCOPE_FLAG_HAS_EXPORTS))
    {
        ptrScope->flags.add(SCOPE_FLAG_HAS_EXPORTS);
        ptrScope = ptrScope->parentScope;
    }
}

void Scope::addPublicFunc(AstNode* node)
{
    ScopedLock lk(mutex);

    if (node->hasSemFlag(SEMFLAG_NO_PUBLIC))
        return;

    if (kind == ScopeKind::Impl && parentScope->owner && parentScope->owner->token.sourceFile->imported)
    {
        const auto scope = node->token.sourceFile->scopeFile->parentScope;
        scope->addPublicImpl(this);
        allocPublicSet();
        publicSet->publicFunc.insert(node);
        return;
    }

    allocPublicSet();
    publicSet->publicFunc.insert(node);
}

void Scope::addPublicImpl(Scope* scope)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicImpl.insert(scope);
}

void Scope::addPublicAttribute(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicAttr.insert(node);
}

void Scope::addPublicNode(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicNodes.insert(node);
}

bool Scope::isParentOf(const Scope* child)
{
    SharedLock lk(mutex);
    while (child)
    {
        if (child == this)
            return true;
        child = child->parentScope;
    }

    return false;
}

void Scope::removeChildNoLock(Scope* child)
{
    SWAG_ASSERT(childrenScopes[child->indexInParent] == child);
    childrenScopes[child->indexInParent]                = childrenScopes.back();
    childrenScopes[child->indexInParent]->indexInParent = child->indexInParent;
    child->indexInParent                                = UINT32_MAX;
    childrenScopes.count--;
}

bool Scope::isSameOrParentOf(const Scope* child) const
{
    while (child)
    {
        if (child == this)
            return true;
        child = child->parentScope;
    }

    return false;
}

void Scope::addChildNoLock(Scope* child)
{
    if (!child)
        return;
    child->indexInParent = childrenScopes.size();
    childrenScopes.push_back(child);
    child->parentScope = this;
}

Scope* Scope::allocScope(Scope* parentScope, AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind)
{
    const auto newScope   = Allocator::alloc<Scope>();
    newScope->kind        = scopeKind;
    newScope->parentScope = parentScope;
    newScope->owner       = nodeOwner;
    newScope->name        = scopeName;
#ifdef SWAG_STATS
    g_Stats.memScopes += sizeof(Scope);
#endif
    return newScope;
}

Scope* Scope::getOrAddChild(AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind, bool matchName)
{
    ScopedLock lk(mutex);

    // Do not create a scope if a scope with the same name already exists
    if (matchName)
    {
        for (const auto child : childrenScopes)
        {
            if (child->name == scopeName)
            {
                return child;
            }
        }
    }

    SWAG_ASSERT(nodeOwner);
    const auto newScope = allocScope(this, nodeOwner, scopeName, scopeKind);
    addChildNoLock(newScope);
    return newScope;
}
