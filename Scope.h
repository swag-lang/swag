#pragma once
#include "Utf8Crc.h"
#include "Pool.h"
#include "SpinLock.h"
struct SyntaxJob;
struct Scope;
struct SymTable;
struct SourceFile;

enum class ScopeKind
{
    Module,
    File,
    Namespace,
    Enum,
    Function,
    Statement,
    Breakable,
	TypeList,
};

struct Scope : public PoolElement
{
    void reset() override
    {
        parentScope    = nullptr;
        symTable       = nullptr;
        startStackSize = 0;
    }

    void               allocateSymTable();
    static string      makeFullName(const string& parentName, const string& name);
    static const char* getNakedName(ScopeKind kind);

    bool isGlobal()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File || kind == ScopeKind::Namespace;
    }

    bool isTopLevel()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File;
    }

    ScopeKind      kind;
    Scope*         parentScope;
    SymTable*      symTable;
    uint32_t       indexInParent = UINT32_MAX;
    Utf8Crc        name;
    Utf8           fullname;
    vector<Scope*> alternativeScopes;
    vector<Scope*> childScopes;
    int            startStackSize;
    SpinLock       lockChilds;
};

extern Pool<Scope> g_Pool_scope;
