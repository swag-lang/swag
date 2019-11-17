#include "pch.h"
#include "Scope.h"
#include "SymTable.h"
#include "Diagnostic.h"

Pool<Scope> g_Pool_scope;

void Scope::allocateSymTable()
{
    scoped_lock lock(lockChilds);
    if (symTable)
        return;
    symTable = new SymTable(this);
}

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
        return "a statement";
    case ScopeKind::Inline:
        return "an inline";
    default:
        return "<scope>";
    }
}

string Scope::makeFullName(const string& parentName, const string& name)
{
    return parentName.empty() ? name : parentName + "." + name;
}

void Scope::collectScopeFrom(Scope* src, Scope* to, vector<Scope*>& result)
{
    result.clear();
    while (true)
    {
        result.insert(result.begin(), src);
        if (src == to)
            return;
        src = src->parentScope;
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
    scoped_lock lk(mutexPublic);
    publicFunc.insert(node);
    setHasExports();
}

void Scope::addPublicGenericFunc(AstNode* node)
{
    scoped_lock lk(mutexPublic);
    publicGenericFunc.insert(node);
    setHasExports();
}

void Scope::addPublicStruct(AstNode* node)
{
    scoped_lock lk(mutexPublic);
    publicStruct.insert(node);
    setHasExports();
}

void Scope::addPublicEnum(AstNode* node)
{
    scoped_lock lk(mutexPublic);
    publicEnum.insert(node);
    setHasExports();
}

void Scope::addPublicConst(AstNode* node)
{
    scoped_lock lk(mutexPublic);
    publicConst.insert(node);
    setHasExports();
}

void Scope::addPublicTypeAlias(AstNode* node)
{
    scoped_lock lk(mutexPublic);
    publicTypeAlias.insert(node);
    setHasExports();
}
