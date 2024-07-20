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

    [[nodiscard]] Utf8     enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped = true);
    [[nodiscard]] Utf8     literalToString(const TypeInfo* typeInfo, const ComputedValue& value);
    [[nodiscard]] AstNode* cloneRaw(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags = 0, const AstNodeFlags& removeFlags = 0);
    [[nodiscard]] AstNode* clone(AstNode* source, AstNode* parent, const AstNodeFlags& forceFlags = 0, const AstNodeFlags& removeFlags = 0);

    [[nodiscard]] Scope*             newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    [[nodiscard]] AstStruct*         newStructDecl(Parser* parser, AstNode* parent);
    [[nodiscard]] AstNode*           newFuncDeclParams(Parser* parser, AstNode* parent);
    [[nodiscard]] AstFuncCallParams* newFuncCallGenParams(Parser* parser, AstNode* parent);
    [[nodiscard]] AstFuncCallParams* newFuncCallParams(Parser* parser, AstNode* parent);
    [[nodiscard]] AstFuncCallParam*  newFuncCallParam(Parser* parser, AstNode* parent);
    [[nodiscard]] AstVarDecl*        newVarDecl(const Utf8& name, Parser* parser, AstNode* parent, AstNodeKind kind = AstNodeKind::VarDecl);
    [[nodiscard]] AstIntrinsicProp*  newIntrinsicProp(TokenId id, Parser* parser, AstNode* parent);
    [[nodiscard]] AstTypeExpression* newTypeExpression(Parser* parser, AstNode* parent);
    [[nodiscard]] AstIdentifier*     newIdentifier(const AstIdentifierRef* identifierRef, const Utf8& name, Parser* parser, AstNode* parent);
    [[nodiscard]] AstIdentifierRef*  newIdentifierRef(Parser* parser, AstNode* parent);
    [[nodiscard]] AstIdentifierRef*  newIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent);
    [[nodiscard]] AstIdentifierRef*  newMultiIdentifierRef(const Utf8& name, Parser* parser, AstNode* parent);
    [[nodiscard]] AstInline*         newInline(Parser* parser, AstNode* parent);
    [[nodiscard]] AstNode*           newAffectOp(SpecFlags specFlags, const AttributeFlags& attributeFlags, Parser* parser, AstNode* parent);

    [[nodiscard]] bool     convertLiteralTupleToStructVar(JobContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType = false);
    [[nodiscard]] bool     convertLiteralTupleToStructType(JobContext* context, AstNode* paramNode, TypeInfoStruct* toType, AstNode* fromNode);
    [[nodiscard]] AstNode* convertTypeToTypeExpression(JobContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType, bool raiseErrors = true);
    [[nodiscard]] bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* assignment, AstStruct** result);
    [[nodiscard]] bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* parent, AstNode* assignment, AstNode** result);
    [[nodiscard]] bool     convertStructParamsToTmpVar(JobContext* context, AstIdentifier* identifier);
    void                   convertTypeStructToStructDecl(JobContext* context, TypeInfoStruct* typeStruct);

    [[nodiscard]] bool generateMissingInterfaceFct(SemanticContext* context, VectorNative<AstFuncDecl*>& mapItIdxToFunc, TypeInfoStruct* typeStruct, const TypeInfoStruct* typeBaseInterface, TypeInfoStruct* typeInterface);
    [[nodiscard]] bool generateOpEquals(SemanticContext* context, TypeInfo* typeLeft, TypeInfo* typeRight);

    template<typename T>
    void constructNode(T* node)
    {
        memset(node, 0, sizeof(T));
        ::new (node) T;
    }

    template<typename T>
    [[nodiscard]] T* newNode()
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
    [[nodiscard]] T* newNode(AstNodeKind kind, Parser* parser, AstNode* parent)
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
[[nodiscard]] T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && casted->kind == kind);
    return casted;
}

template<typename T>
[[nodiscard]] T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2));
    return casted;
}

template<typename T>
[[nodiscard]] T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3));
    return casted;
}

template<typename T>
[[nodiscard]] T* castAst(AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3, [[maybe_unused]] AstNodeKind kind4)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3 || casted->kind == kind4));
    return casted;
}

template<typename T>
[[nodiscard]] const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind)
{
    const T* casted = static_cast<const T*>(ptr);
    SWAG_ASSERT(casted && casted->kind == kind);
    return casted;
}

template<typename T>
[[nodiscard]] const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2)
{
    const T* casted = static_cast<const T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2));
    return casted;
}

template<typename T>
[[nodiscard]] const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3)
{
    const T* casted = static_cast<const T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3));
    return casted;
}

template<typename T>
[[nodiscard]] const T* castAst(const AstNode* ptr, [[maybe_unused]] AstNodeKind kind1, [[maybe_unused]] AstNodeKind kind2, [[maybe_unused]] AstNodeKind kind3, [[maybe_unused]] AstNodeKind kind4)
{
    const T* casted = static_cast<const T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3 || casted->kind == kind4));
    return casted;
}

template<typename T>
[[nodiscard]] T* castAst(AstNode* ptr)
{
    T* casted = static_cast<T*>(ptr);
    return casted;
}

template<typename T>
[[nodiscard]] const T* castAst(const AstNode* ptr)
{
    const T* casted = static_cast<const T*>(ptr);
    return casted;
}

extern std::atomic<uint32_t> g_UniqueID;
