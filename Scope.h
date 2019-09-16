#pragma once
#include "Utf8Crc.h"
#include "Pool.h"
#include "SpinLock.h"
struct SyntaxJob;
struct Scope;
struct SymTable;
struct SourceFile;
struct AstNode;

enum class ScopeKind
{
    Module,
    File,
    Namespace,
    Enum,
    Struct,
    Function,
    Statement,
    Breakable,
    TypeList,
};

struct AlternativeScope
{
    AstNode* node;
    Scope*   scope;
};

struct Scope : public PoolElement
{
    void reset() override
    {
        parentScope    = nullptr;
        symTable       = nullptr;
        startStackSize = 0;
        owner          = nullptr;
        indexInParent  = UINT32_MAX;
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

    AstNode*                 owner;
    ScopeKind                kind;
    Scope*                   parentScope;
    SymTable*                symTable;
    uint32_t                 indexInParent;
    Utf8Crc                  name;
    Utf8                     fullname;
    vector<Scope*>           alternativeScopes;
    vector<AlternativeScope> alternativeScopesVars;
    vector<Scope*>           childScopes;
    int                      startStackSize;
    SpinLock                 lockChilds;
    vector<AstNode*>         deferedNodes;
};

extern Pool<Scope> g_Pool_scope;
