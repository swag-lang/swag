#pragma once
#include "Flags.h"
#include "DependentJobs.h"
#include "Mutex.h"
#include "SymTable.h"

struct Scope;
struct AstNode;
struct AstDefer;

enum class ScopeKind : uint8_t
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

using ScopeFlags = Flags<uint8_t>;
constexpr ScopeFlags SCOPE_FLAG_HAS_EXPORTS = 0x00000001;
constexpr ScopeFlags SCOPE_FILE             = 0x00000002;
constexpr ScopeFlags SCOPE_AUTO_GENERATED   = 0x00000004;
constexpr ScopeFlags SCOPE_IMPORTED         = 0x00000008;
constexpr ScopeFlags SCOPE_FILE_PRIVATE     = 0x00000010;

struct ScopePublicSet
{
	Set<AstNode*> publicFunc;
	Set<AstNode*> publicAttr;
	Set<AstNode*> publicNodes;
};

struct Scope
{
	Scope();
	void release();
	void addPublicFunc(AstNode* node);
	void addPublicAttribute(AstNode* node);
	void addPublicNode(AstNode* node);
	void allocPublicSet();

	static void makeFullName(Utf8& result, const Utf8& parentName, const Utf8& name);
	const Utf8& getFullName();
	Utf8        getDisplayFullName();
	static void collectScopeFromToExcluded(Scope* src, const Scope* to, VectorNative<Scope*>& result);
	Scope*      getOrAddChild(AstNode* nodeOwner, const Utf8& scopeName, ScopeKind scopeKind, bool matchName);
	void        addChildNoLock(Scope* child);
	void        removeChildNoLock(Scope* child);
	bool        isParentOf(const Scope* child);
	bool        isSameOrParentOf(const Scope* child) const;
	bool        isGlobal() const;
	bool        isTopLevel() const;
	bool        isGlobalOrImpl() const;

	SharedMutex             mutex;
	SymTable                symTable;
	Utf8                    name;
	Utf8                    fullname;
	VectorNative<Scope*>    childScopes;
	DependentJobs           dependentJobs;
	VectorNative<AstDefer*> deferredNodes;
	VectorNative<AstNode*>  doneDefer;
	VectorNative<AstNode*>  doneDrop;

	ScopePublicSet* publicSet            = nullptr;
	AstNode*        owner                = nullptr;
	Scope*          parentScope          = nullptr;
	Scope*          hieScope             = nullptr;
	SymbolName*     symbolOpAffect       = nullptr;
	SymbolName*     symbolOpAffectSuffix = nullptr;
	SymbolName*     symbolOpCast         = nullptr;

	uint32_t indexInParent  = UINT32_MAX;
	uint32_t startStackSize = 0;
	uint32_t backendStart   = 0;
	uint32_t backendEnd     = 0;

	ScopeKind  kind  = ScopeKind::Invalid;
	ScopeFlags flags = 0;
	uint8_t    padding[6];
};
