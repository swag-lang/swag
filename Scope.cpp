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

const char* Scope::getNakedName(ScopeKind kind)
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
        return "???";
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
    while (pscope && !(pscope->hasExports))
    {
        pscope->hasExports = true;
        pscope             = pscope->parentScope;
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
