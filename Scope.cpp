#include "pch.h"
#include "Scope.h"
#include "AstNode.h"

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
    default:
        return "<scope>";
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
    default:
        return "<scope>";
    }
}

const Utf8& Scope::getFullName()
{
    unique_lock lk(mutex);
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
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicFunc.insert(node);
    setHasExports();
}

void Scope::addPublicGenericFunc(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicGenericFunc.insert(node);
    setHasExports();
}

void Scope::addPublicStruct(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicStruct.insert(node);
    setHasExports();
}

void Scope::addPublicInterface(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicInterface.insert(node);
    setHasExports();
}

void Scope::addPublicTypeSet(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicTypeSet.insert(node);
    setHasExports();
}

void Scope::addPublicEnum(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicEnum.insert(node);
    setHasExports();
}

void Scope::addPublicConst(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicConst.insert(node);
    setHasExports();
}

void Scope::addPublicNode(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicNodes.insert(node);
    setHasExports();
}

void Scope::addPublicNamespace(AstNode* node)
{
    unique_lock lk(mutex);
    allocPublicSet();
    publicSet->publicNamespace.insert(node);
}

bool Scope::isParentOf(Scope* child)
{
    unique_lock lk(mutex);
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
}

void Scope::removeChildNoLock(Scope* child)
{
    SWAG_ASSERT(childScopes[child->indexInParent] == child);
    childScopes[child->indexInParent]                = childScopes.back();
    childScopes[child->indexInParent]->indexInParent = child->indexInParent;
    child->indexInParent                             = UINT32_MAX;
}

Scope* Scope::getOrAddChild(AstNode* nodeOwner, const Utf8Crc& scopeName, ScopeKind scopeKind, bool matchName)
{
    unique_lock lk(mutex);

    // Do not create a scope if a scope with the same name already exists
    if (matchName)
    {
        for (auto child : childScopes)
        {
            if (child->name.compare(scopeName))
            {
                return child;
            }
        }
    }

    auto newScope         = g_Allocator.alloc<Scope>();
    newScope->kind        = scopeKind;
    newScope->parentScope = this;
    newScope->owner       = nodeOwner;
    newScope->name        = scopeName;

    addChildNoLock(newScope);

    return newScope;
}