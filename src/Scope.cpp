#include "pch.h"
#include "Scope.h"
#include "AstNode.h"
#include "Module.h"
#include "SourceFile.h"
#include "Ast.h"

const char* Scope::getNakedKindName(ScopeKind kind)
{
    switch (kind)
    {
    case ScopeKind::Namespace:
        return "namespace";
    case ScopeKind::Enum:
        return "enum";
    case ScopeKind::TypeList:
        return "tuple";
    case ScopeKind::Struct:
        return "struct";
    case ScopeKind::File:
        return "file";
    case ScopeKind::Module:
        return "global";
    case ScopeKind::Statement:
    case ScopeKind::EmptyStatement:
        return "statement";
    case ScopeKind::Inline:
        return "inline";
    case ScopeKind::Function:
    case ScopeKind::FunctionBody:
        return "function";
    default:
        return "scope";
    }
}

const char* Scope::getArticleKindName(ScopeKind kind)
{
    switch (kind)
    {
    case ScopeKind::Namespace:
        return "a namespace";
    case ScopeKind::Enum:
        return "an enum";
    case ScopeKind::TypeList:
        return "a tuple";
    case ScopeKind::Struct:
        return "a struct";
    case ScopeKind::File:
        return "a file";
    case ScopeKind::Module:
        return "a module";
    case ScopeKind::Statement:
    case ScopeKind::EmptyStatement:
        return "a statement";
    case ScopeKind::Inline:
        return "an inline";
    case ScopeKind::Function:
    case ScopeKind::FunctionBody:
        return "a function";
    default:
        return "a scope";
    }
}

const Utf8& Scope::getFullName()
{
    if (flags & SCOPE_PRIVATE)
        return name;

    ScopedLock lk(mutex);

    if (!fullname.empty())
        return fullname;
    if (parentScope)
        makeFullName(fullname, parentScope->getFullName(), name);
    else
        fullname = name;
    return fullname;
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

void Scope::collectScopeFromToExcluded(Scope* src, Scope* to, VectorNative<Scope*>& result)
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

void Scope::setHasExports()
{
    auto pscope = this;
    while (pscope && !(pscope->flags & SCOPE_FLAG_HAS_EXPORTS))
    {
        pscope->flags |= SCOPE_FLAG_HAS_EXPORTS;
        pscope = pscope->parentScope;
    }
}

void Scope::addPublicFunc(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicFunc.insert(node);
    setHasExports();
}

void Scope::addPublicAttribute(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicAttr.insert(node);
    setHasExports();
}

void Scope::addPublicInlinedFunc(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicInlinedFunc.insert(node);
    setHasExports();
}

void Scope::addPublicStruct(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicStruct.insert(node);
    setHasExports();
}

void Scope::addPublicInterface(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicInterface.insert(node);
    setHasExports();
}

void Scope::addPublicEnum(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicEnum.insert(node);
    setHasExports();
}

void Scope::addPublicConst(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicConst.insert(node);
    setHasExports();
}

void Scope::addPublicNode(AstNode* node)
{
    ScopedLock lk(mutex);
    allocPublicSet();
    publicSet->publicNodes.insert(node);
    setHasExports();
}

bool Scope::isParentOf(Scope* child)
{
    ScopedLock lk(mutex);
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
    SWAG_ASSERT(childScopes[child->indexInParent] == child);
    childScopes[child->indexInParent]                = childScopes.back();
    childScopes[child->indexInParent]->indexInParent = child->indexInParent;
    child->indexInParent                             = UINT32_MAX;
    childScopes.count--;

    if (child->flags & SCOPE_ROOT_PRIVATE)
    {
        auto it = privateScopes.find(child->owner->sourceFile);
        SWAG_ASSERT(it != privateScopes.end());
        privateScopes.erase(it);
    }
}

bool Scope::isSameOrParentOf(Scope* child)
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
    child->indexInParent = (uint32_t) childScopes.size();
    childScopes.push_back(child);
    child->parentScope = this;
    child->flags |= flags & SCOPE_PRIVATE;

    if (child->flags & SCOPE_ROOT_PRIVATE)
    {
        privateScopes[child->owner->sourceFile] = child;
    }
}

Scope* Scope::getOrAddChild(AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind, bool matchName, bool isPrivate)
{
    ScopedLock lk(mutex);

    // Do not create a scope if a scope with the same name already exists
    if (matchName)
    {
        for (auto child : childScopes)
        {
            if (child->name == scopeName)
            {
                return child;
            }
        }
    }

    auto newScope         = g_Allocator.alloc<Scope>();
    newScope->kind        = scopeKind;
    newScope->parentScope = this;
    SWAG_ASSERT(nodeOwner);
    newScope->owner = nodeOwner;
    newScope->name  = scopeName;
    if (isPrivate)
        newScope->flags |= SCOPE_ROOT_PRIVATE | SCOPE_PRIVATE;
    if (g_CommandLine.stats)
        g_Stats.memScopes += sizeof(Scope);

    addChildNoLock(newScope);
    return newScope;
}