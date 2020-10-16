#pragma once
#include "Utf8Crc.h"
#include "Pool.h"
#include "RegisterList.h"
#include "DependentJobs.h"
#include "SymTable.h"
#include "VectorNative.h"
struct SyntaxJob;
struct Scope;
struct SourceFile;
struct AstNode;

enum class ScopeKind
{
    Invalid,
    Module,
    File,
    Namespace,
    Enum,
    TypeSet,
    Struct,
    Impl,
    Function,
    FunctionBody,
    Attribute,
    Statement,
    EmptyStatement,
    Breakable,
    TypeList,
    Inline,
    Macro,
};

struct AlternativeScope
{
    AstNode* node;
    Scope*   scope;
};

static const uint32_t SCOPE_FLAG_HAS_EXPORTS = 0x00000001;
static const uint32_t SCOPE_PRIVATE          = 0x00000002;

struct ScopePublicSet
{
    set<AstNode*> publicFunc;
    set<AstNode*> publicAttr;
    set<AstNode*> publicGenericFunc;
    set<AstNode*> publicStruct;
    set<AstNode*> publicInterface;
    set<AstNode*> publicTypeSet;
    set<AstNode*> publicEnum;
    set<AstNode*> publicConst;
    set<AstNode*> publicNodes;
    set<AstNode*> publicNamespace;
};

struct Scope
{
    Scope()
    {
        symTable.scope = this;
    }

    void               setHasExports();
    void               addPublicFunc(AstNode* node);
    void               addPublicAttribute(AstNode* node);
    void               addPublicGenericFunc(AstNode* node);
    void               addPublicStruct(AstNode* node);
    void               addPublicInterface(AstNode* node);
    void               addPublicTypeSet(AstNode* node);
    void               addPublicEnum(AstNode* node);
    void               addPublicConst(AstNode* node);
    void               addPublicNode(AstNode* node);
    void               addPublicNamespace(AstNode* node);
    static void        makeFullName(Utf8& result, const Utf8& parentName, const Utf8& name);
    const Utf8&        getFullName();
    static const char* getNakedKindName(ScopeKind kind);
    static const char* getArticleKindName(ScopeKind kind);
    static void        collectScopeFromToExcluded(Scope* src, Scope* to, VectorNative<Scope*>& result);
    bool               isParentOf(Scope* child);

    bool isGlobal()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File || kind == ScopeKind::Namespace;
    }

    bool isTopLevel()
    {
        return kind == ScopeKind::Module || kind == ScopeKind::File;
    }

    bool isGlobalOrImpl()
    {
        return isGlobal() || kind == ScopeKind::Struct || kind == ScopeKind::Impl || kind == ScopeKind::Enum;
    }

    void allocPublicSet()
    {
        if (!publicSet)
            publicSet = g_Allocator.alloc<ScopePublicSet>();
    }

    SymTable               symTable;
    Utf8Crc                name;
    Utf8                   fullname;
    VectorNative<Scope*>   childScopes;
    VectorNative<AstNode*> deferredNodes;
    set<AstNode*>          doneLeaveScopeDefer;
    set<AstNode*>          doneLeaveScopeDrop;
    RegisterList           registersToRelease;
    DependentJobs          dependentJobs;
    shared_mutex           mutex;

    ScopePublicSet* publicSet   = nullptr;
    AstNode*        owner       = nullptr;
    Scope*          parentScope = nullptr;

    ScopeKind kind           = ScopeKind::Invalid;
    uint32_t  indexInParent  = UINT32_MAX;
    uint32_t  flags          = 0;
    uint32_t  startStackSize = 0;

    Scope* getOrAddChild(AstNode* nodeOwner, const Utf8Crc& scopeName, ScopeKind scopeKind, bool matchName);
    void   addChildNoLock(Scope* child);
    void   removeChildNoLock(Scope* child);
};