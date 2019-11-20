#pragma once
#include "Utf8Crc.h"
#include "Pool.h"
#include "SpinLock.h"
#include "RegisterList.h"
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
    Attribute,
    Statement,
    Breakable,
    TypeList,
    Inline,
};

struct AlternativeScope
{
    AstNode* node;
    Scope*   scope;
};

static const uint32_t SCOPE_FLAG_HAS_EXPORTS = 0x00000001;

struct Scope : public PoolElement
{
    void reset() override
    {
        parentScope    = nullptr;
        symTable       = nullptr;
        startStackSize = 0;
        owner          = nullptr;
        flags          = 0;
        indexInParent  = UINT32_MAX;
    }

    void               setHasExports();
    void               allocateSymTable();
    void               addPublicFunc(AstNode* node);
    void               addPublicGenericFunc(AstNode* node);
    void               addPublicStruct(AstNode* node);
    void               addPublicEnum(AstNode* node);
    void               addPublicConst(AstNode* node);
    void               addPublicTypeAlias(AstNode* node);
    static string      makeFullName(const string& parentName, const string& name);
    static const char* getNakedKindName(ScopeKind kind);
    static const char* getArticleKindName(ScopeKind kind);
    static void        collectScopeFrom(Scope* src, Scope* to, vector<Scope*>& result);

    bool isGlobal()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File || kind == ScopeKind::Namespace;
    }

    bool isTopLevel()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File;
    }

    AstNode*         owner;
    ScopeKind        kind;
    Scope*           parentScope;
    SymTable*        symTable;
    uint32_t         indexInParent;
    Utf8Crc          name;
    Utf8             fullname;
    vector<Scope*>   childScopes;
    uint32_t         startStackSize;
    SpinLock         lockChilds;
    vector<AstNode*> deferredNodes;
    RegisterList     registersToRelease;
    uint32_t         flags;

    SpinLock      mutexPublic;
    set<AstNode*> publicFunc;
    set<AstNode*> publicGenericFunc;
    set<AstNode*> publicStruct;
    set<AstNode*> publicEnum;
    set<AstNode*> publicConst;
    set<AstNode*> publicTypeAlias;
};

extern Pool<Scope> g_Pool_scope;
