#pragma once
#include "Utf8Crc.h"
#include "Pool.h"
#include "RegisterList.h"
#include "DependentJobs.h"
#include "SymTable.h"
#include "Vector.h"
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
    Struct,
    Function,
    Attribute,
    Statement,
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

static const uint32_t SCOPE_FLAG_HAS_EXPORTS      = 0x00000001;
static const uint32_t SCOPE_FLAG_MODULE_FROM_TEST = 0x00000002;

struct Scope
{
    Scope()
    {
        symTable.scope = this;
    }

    void               setHasExports();
    void               addPublicFunc(AstNode* node);
    void               addPublicGenericFunc(AstNode* node);
    void               addPublicStruct(AstNode* node);
    void               addPublicEnum(AstNode* node);
    void               addPublicConst(AstNode* node);
    void               addPublicTypeAlias(AstNode* node);
    void               addPublicNamespace(AstNode* node);
    static Utf8        makeFullName(const Utf8& parentName, const Utf8& name);
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

    SymTable               symTable;
    Utf8Crc                name;
    Utf8                   fullname;
    vector<Scope*>         childScopes;
    shared_mutex           lockChilds;
    VectorNative<AstNode*> deferredNodes;
    RegisterList           registersToRelease;
    DependentJobs          dependentJobs;
    mutex                  mutexPublicFunc;
    mutex                  mutexPublicGenericFunc;
    mutex                  mutexPublicStruct;
    mutex                  mutexPublicEnum;
    mutex                  mutexPublicConst;
    mutex                  mutexPublicTypeAlias;
    mutex                  mutexPublicNamespace;
    set<AstNode*>          publicFunc;
    set<AstNode*>          publicGenericFunc;
    set<AstNode*>          publicStruct;
    set<AstNode*>          publicEnum;
    set<AstNode*>          publicConst;
    set<AstNode*>          publicTypeAlias;
    set<AstNode*>          publicNamespace;

    AstNode* owner       = nullptr;
    Scope*   parentScope = nullptr;

    ScopeKind kind           = ScopeKind::Invalid;
    uint32_t  indexInParent  = UINT32_MAX;
    uint32_t  flags          = 0;
    uint32_t  startStackSize = 0;
};