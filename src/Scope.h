#pragma once
#include "Utf8.h"
#include "Register.h"
#include "DependentJobs.h"
#include "SymTable.h"
#include "VectorNative.h"
#include "Mutex.h"
struct Parser;
struct Scope;
struct SourceFile;
struct AstNode;
struct AstDefer;

enum class ScopeKind
{
    Invalid,
    Module,
    File,
    Namespace,
    Enum,
    Struct,
    Impl,
    Function,
    FunctionBody,
    TypeLambda,
    Attribute,
    Statement,
    EmptyStatement,
    Breakable,
    TypeList,
    Inline,
    Macro,
};

const uint32_t SCOPE_FLAG_HAS_EXPORTS = 0x00000001;
const uint32_t SCOPE_FILE             = 0x00000002;
const uint32_t SCOPE_AUTO_GENERATED   = 0x00000004;
const uint32_t SCOPE_IMPORTED         = 0x00000008;

struct ScopePublicSet
{
    set<AstNode*> publicFunc;
    set<AstNode*> publicAttr;
    set<AstNode*> publicNodes;
};

struct Scope
{
    Scope();
    void addPublicFunc(AstNode* node);
    void addPublicAttribute(AstNode* node);
    void addPublicNode(AstNode* node);
    void allocPublicSet();

    static void makeFullName(Utf8& result, const Utf8& parentName, const Utf8& name);
    const Utf8& getFullName();
    Utf8        getDisplayFullName();
    static void collectScopeFromToExcluded(Scope* src, Scope* to, VectorNative<Scope*>& result);
    Scope*      getOrAddChild(AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind, bool matchName);
    void        addChildNoLock(Scope* child);
    void        removeChildNoLock(Scope* child);
    bool        isParentOf(Scope* child);
    bool        isSameOrParentOf(Scope* child);
    bool        isGlobal();
    bool        isTopLevel();
    bool        isGlobalOrImpl();

    SymTable                symTable;
    Utf8                    name;
    Utf8                    fullname;
    VectorNative<Scope*>    childScopes;
    VectorNative<AstDefer*> deferredNodes;
    DependentJobs           dependentJobs;
    SharedMutex             mutex;
    VectorNative<AstNode*>  doneDefer;
    VectorNative<AstNode*>  doneDrop;

    ScopePublicSet* publicSet   = nullptr;
    AstNode*        owner       = nullptr;
    Scope*          parentScope = nullptr;

    ScopeKind kind           = ScopeKind::Invalid;
    uint32_t  indexInParent  = UINT32_MAX;
    uint32_t  flags          = 0;
    uint32_t  startStackSize = 0;
    uint32_t  backendStart   = 0;
    uint32_t  backendEnd     = 0;
};