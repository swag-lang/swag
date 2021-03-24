#pragma once
#include "Utf8.h"
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
static const uint32_t SCOPE_ROOT_PRIVATE     = 0x00000004;
static const uint32_t SCOPE_AUTO_GENERATED   = 0x00000008;
static const uint32_t SCOPE_IMPORTED         = 0x00000010;

struct ScopePublicSet
{
    set<AstNode*> publicFunc;
    set<AstNode*> publicAttr;
    set<AstNode*> publicInlinedFunc;
    set<AstNode*> publicStruct;
    set<AstNode*> publicInterface;
    set<AstNode*> publicTypeSet;
    set<AstNode*> publicEnum;
    set<AstNode*> publicConst;
    set<AstNode*> publicNodes;
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
    void               addPublicInlinedFunc(AstNode* node);
    void               addPublicStruct(AstNode* node);
    void               addPublicInterface(AstNode* node);
    void               addPublicTypeSet(AstNode* node);
    void               addPublicEnum(AstNode* node);
    void               addPublicConst(AstNode* node);
    void               addPublicNode(AstNode* node);
    static void        makeFullName(Utf8& result, const Utf8& parentName, const Utf8& name);
    const Utf8&        getFullName();
    const Utf8&        getFullNameForeign();
    const Utf8&        getFullNameType(AstNode* declNode);
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
        return kind == ScopeKind::Module || kind == ScopeKind::File || (kind == ScopeKind::Namespace && (flags & SCOPE_AUTO_GENERATED));
    }

    bool isGlobalOrImpl()
    {
        if (isGlobal() || kind == ScopeKind::Impl)
            return true;
        if (kind == ScopeKind::Struct || kind == ScopeKind::Enum || kind == ScopeKind::TypeSet)
            return !parentScope || parentScope->isGlobal() || parentScope->kind == ScopeKind::Impl;
        return false;
    }

    void allocPublicSet()
    {
        if (!publicSet)
            publicSet = g_Allocator.alloc<ScopePublicSet>();
    }

    SymTable                 symTable;
    Utf8                     name;
    Utf8                     fullname;
    VectorNative<Scope*>     childScopes;
    VectorNative<AstNode*>   deferredNodes;
    map<SourceFile*, Scope*> privateScopes;
    VectorNative<uint32_t>   registersToReleaseInlineVar;
    VectorNative<uint32_t>   registersToReleaseTmp;
    DependentJobs            dependentJobs;
    shared_mutex             mutex;
    set<AstNode*>            doneDefer;
    set<AstNode*>            doneDrop;

    ScopePublicSet* publicSet   = nullptr;
    AstNode*        owner       = nullptr;
    Scope*          parentScope = nullptr;

    ScopeKind kind           = ScopeKind::Invalid;
    uint32_t  indexInParent  = UINT32_MAX;
    uint32_t  flags          = 0;
    uint32_t  startStackSize = 0;
    uint32_t  backendStart   = 0;
    uint32_t  backendEnd     = 0;

    Scope* getOrAddChild(AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind, bool matchName, bool isPrivate);
    void   addChildNoLock(Scope* child);
    void   removeChildNoLock(Scope* child);
    bool   isSameOrParentOf(Scope* child);
};