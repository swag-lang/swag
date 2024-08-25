#pragma once
#include "Core/Allocator.h"
#include "Main/Statistics.h"
#include "Syntax/AstNode.h"

struct Concat;
struct JobContext;
struct Scope;
struct TypeInfoEnum;
struct TypeInfoList;
struct TypeInfoStruct;
struct Utf8;

enum class ScopeKind : uint8_t;

namespace Ast
{
    enum VisitResult
    {
        Continue,
        Stop,
        NoChildren,
    };

    void initNewNode(AstNodeKind kind, AstNode* node, Parser* parser, AstNode* parent);
    void removeFromParent(AstNode* child);
    void insertChild(AstNode* parent, AstNode* child, uint32_t index);
    void addChildBack(AstNode* parent, AstNode* child);
    void addChildFront(AstNode* parent, AstNode* child);

    void        setForceConstType(AstNode* node);
    void        normalizeIdentifierName(const Utf8& name);
    void        visit(AstNode* root, const std::function<void(AstNode*)>& fct);
    VisitResult visit(ErrorContext* context, AstNode* root, const std::function<VisitResult(ErrorContext*, AstNode*)>& fct);

    Utf8     enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped = true);
    Utf8     literalToString(const TypeInfo* typeInfo, const ComputedValue& value);
    AstNode* cloneRaw(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags = 0, const AstNodeFlags& removeFlags = 0);
    AstNode* clone(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags = 0, const AstNodeFlags& removeFlags = 0);

    Scope*             newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    AstStruct*         newStructDecl(Parser* parser, AstNode* parent);
    AstNode*           newFuncDeclParams(Parser* parser, AstNode* parent);
    AstFuncCallParams* newFuncCallGenParams(Parser* parser, AstNode* parent);
    AstFuncCallParams* newFuncCallParams(Parser* parser, AstNode* parent);
    AstFuncCallParam*  newFuncCallParam(Parser* parser, AstNode* parent);
    AstVarDecl*        newVarDecl(const Utf8& name, Parser* parser, AstNode* parent, AstNodeKind kind = AstNodeKind::VarDecl);
    AstIntrinsicProp*  newIntrinsicProp(TokenId id, Parser* parser, AstNode* parent);
    AstTypeExpression* newTypeExpression(Parser* parser, AstNode* parent);
    AstIdentifier*     newIdentifier(const AstIdentifierRef* identifierRef, const Utf8& name, Parser* parser, AstNode* parent);
    AstIdentifierRef*  newIdentifierRef(Parser* parser, AstNode* parent);
    AstIdentifierRef*  newIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent);
    AstIdentifierRef*  newMultiIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent);
    AstInline*         newInline(Parser* parser, AstNode* parent);
    AstNode*           newAffectOp(SpecFlags specFlags, const AttributeFlags& attributeFlags, Parser* parser, AstNode* parent);

    bool     convertLiteralTupleToStructVar(JobContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType = false);
    bool     convertLiteralTupleToStructType(JobContext* context, AstNode* paramNode, TypeInfoStruct* toType, AstNode* fromNode);
    AstNode* convertTypeToTypeExpression(JobContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType, bool raiseErrors = true);
    bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* assignment, AstStruct** result);
    bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* parent, AstNode* assignment, AstNode** result);
    bool     convertStructParamsToTmpVar(JobContext* context, AstIdentifier* identifier);
    void     convertTypeStructToStructDecl(JobContext* context, TypeInfoStruct* typeStruct);

    bool generateMissingInterfaceFct(SemanticContext* context, VectorNative<AstFuncDecl*>& mapItIdxToFunc, TypeInfoStruct* typeStruct, const TypeInfoStruct* typeBaseInterface, TypeInfoStruct* typeInterface);
    bool generateOpEquals(SemanticContext* context, TypeInfo* typeLeft, TypeInfo* typeRight);

    template<typename T>
    void constructNode(T* node)
    {
        memset(node, 0, sizeof(T));
        ::new (node) T;
    }

    template<typename T>
    T* newNode()
    {
        auto node = Allocator::allocRaw<T>();
        constructNode<T>(node);
#ifdef SWAG_STATS
        ++g_Stats.numNodes;
        g_Stats.memNodes += Allocator::alignSize(sizeof(T));
#ifdef SWAG_DEV_MODE
        node->rankId = g_Stats.numNodes;
#endif
#endif
        return node;
    }

    template<typename T>
    T* newNode(AstNodeKind kind, Parser* parser, AstNode* parent)
    {
        auto node = Allocator::allocRaw<T>();
        constructNode<T>(node);
        initNewNode(kind, node, parser, parent);
#ifdef SWAG_STATS
        ++g_Stats.numNodes;
        g_Stats.memNodes += Allocator::alignSize(sizeof(T));
#ifdef SWAG_DEV_MODE
        node->rankId = g_Stats.numNodes;
#endif
#endif
        return node;
    }
}

template<typename T>
T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind)
{
    T* cast = static_cast<T*>(ptr);
    SWAG_ASSERT(cast && cast->kind == kind);
    return cast;
}

template<typename T>
T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2)
{
    T* cast = static_cast<T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2));
    return cast;
}

template<typename T>
T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3)
{
    T* cast = static_cast<T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2 || cast->kind == kind3));
    return cast;
}

template<typename T>
T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3, [[maybe_unused]] AstNodeKind kind4)
{
    T* cast = static_cast<T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2 || cast->kind == kind3 || cast->kind == kind4));
    return cast;
}

template<typename T>
const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind)
{
    const T* cast = static_cast<const T*>(ptr);
    SWAG_ASSERT(cast && cast->kind == kind);
    return cast;
}

template<typename T>
const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2)
{
    const T* cast = static_cast<const T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2));
    return cast;
}

template<typename T>
const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3)
{
    const T* cast = static_cast<const T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2 || cast->kind == kind3));
    return cast;
}

template<typename T>
const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3, [[maybe_unused]] AstNodeKind kind4)
{
    const T* cast = static_cast<const T*>(ptr);
    SWAG_ASSERT(cast && (cast->kind == kind1 || cast->kind == kind2 || cast->kind == kind3 || cast->kind == kind4));
    return cast;
}

template<typename T>
T* castAst(AstNode* ptr)
{
    T* cast = static_cast<T*>(ptr);
    return cast;
}

template<typename T>
const T* castAst(const AstNode* ptr)
{
    const T* cast = static_cast<const T*>(ptr);
    return cast;
}

extern std::atomic<uint32_t> g_UniqueID;
