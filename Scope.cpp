#include "pch.h"
#include "Scope.h"
#include "SymTable.h"
#include "Diagnostic.h"

Pool<Scope> g_Pool_scope;

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
        result.push_back(src);
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
    unique_lock lk(mutexPublicFunc);
    publicFunc.insert(node);
    setHasExports();
}

void Scope::addPublicGenericFunc(AstNode* node)
{
    unique_lock lk(mutexPublicGenericFunc);
    publicGenericFunc.insert(node);
    setHasExports();
}

void Scope::addPublicStruct(AstNode* node)
{
    unique_lock lk(mutexPublicStruct);
    publicStruct.insert(node);
    setHasExports();
}

void Scope::addPublicEnum(AstNode* node)
{
    unique_lock lk(mutexPublicEnum);
    publicEnum.insert(node);
    setHasExports();
}

void Scope::addPublicConst(AstNode* node)
{
    unique_lock lk(mutexPublicConst);
    publicConst.insert(node);
    setHasExports();
}

void Scope::addPublicTypeAlias(AstNode* node)
{
    unique_lock lk(mutexPublicTypeAlias);
    publicTypeAlias.insert(node);
    setHasExports();
}
