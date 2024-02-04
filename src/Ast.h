#pragma once
#include "Allocator.h"
#include "AstNode.h"
#include "Statistics.h"

struct Concat;
struct JobContext;
struct Scope;
struct SourceFile;
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
        NoChilds,
    };

    void initNewNode(AstNode* node, Parser* parser, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent);
    void removeFromParent(AstNode* child);
    void insertChild(AstNode* parent, AstNode* child, uint32_t index);
    void addChildBack(AstNode* parent, AstNode* child);
    void addChildFront(AstNode* parent, AstNode* child);

    void             visit(AstNode* root, const function<void(AstNode*)>& fctor);
    Ast::VisitResult visit(ErrorContext* context, AstNode* root, const function<VisitResult(ErrorContext*, AstNode*)>& fctor);
    void             setForceConstType(AstNode* node);
    Utf8             enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped = true);
    Utf8             literalToString(TypeInfo* typeInfo, const ComputedValue& value);
    void             normalizeIdentifierName(Utf8& name);
    AstNode*         cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags = 0, uint64_t removeFlags = 0);
    AstNode*         clone(AstNode* source, AstNode* parent, uint64_t forceFlags = 0, uint64_t removeFlags = 0);
    Utf8             computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params);
    Vector<Utf8>     computeGenericParametersReplacement(VectorMap<Utf8, GenericReplaceType>& replace);

    Scope*             newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    AstStruct*         newStructDecl(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstNode*           newFuncDeclParams(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstFuncCallParams* newFuncCallGenParams(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstFuncCallParams* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstFuncCallParam*  newFuncCallParam(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstVarDecl*        newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser = nullptr, AstNodeKind kind = AstNodeKind::VarDecl);
    AstIntrinsicProp*  newIntrinsicProp(SourceFile* sourceFile, TokenId id, AstNode* parent, Parser* parser = nullptr);
    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstIdentifier*     newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, Parser* parser = nullptr);
    AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser = nullptr);
    AstIdentifierRef*  newMultiIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser = nullptr);
    AstInline*         newInline(SourceFile* sourceFile, AstNode* parent, Parser* parser = nullptr);
    AstNode*           newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, Parser* parser = nullptr);

    bool     convertLiteralTupleToStructVar(JobContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType = false);
    bool     convertLiteralTupleToStructType(JobContext* context, AstNode* paramNode, TypeInfoStruct* toType, AstNode* fromNode);
    AstNode* convertTypeToTypeExpression(JobContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType, bool raiseErrors = true);
    bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* assignment, AstStruct** result);
    bool     convertLiteralTupleToStructDecl(JobContext* context, AstNode* parent, AstNode* assignment, AstNode** result);
    void     convertTypeStructToStructDecl(JobContext* context, TypeInfoStruct* typeStruct);
    bool     convertStructParamsToTmpVar(JobContext* context, AstIdentifier* identifier);

    bool generateMissingInterfaceFct(SemanticContext*            context,
                                     VectorNative<AstFuncDecl*>& mapItIdxToFunc,
                                     TypeInfoStruct*             typeStruct,
                                     TypeInfoStruct*             typeBaseInterface,
                                     TypeInfoStruct*             typeInterface);
    bool generateOpEquals(SemanticContext* context, TypeInfo* typeLeft, TypeInfo* typeRight);

    template<typename T>
    void constructNode(T* node)
    {
        memset(node, 0, sizeof(T));
        ::new(node) T;
    }

    template<typename T>
    T* newNode()
    {
        auto node = Allocator::allocRaw<T>();
        constructNode<T>(node);
#ifdef SWAG_STATS
        g_Stats.numNodes++;
        g_Stats.memNodes += Allocator::alignSize(sizeof(T));
#ifdef SWAG_DEV_MODE
        node->rankId = g_Stats.numNodes;
#endif
#endif
        return node;
    }

    template<typename T>
    T* newNode(Parser* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent)
    {
        auto node = Allocator::allocRaw<T>();
        constructNode<T>(node);
        initNewNode(node, job, kind, sourceFile, parent);
#ifdef SWAG_STATS
        g_Stats.numNodes++;
        g_Stats.memNodes += Allocator::alignSize(sizeof(T));
#ifdef SWAG_DEV_MODE
        node->rankId = g_Stats.numNodes;
#endif
#endif
        return node;
    }

}; // namespace Ast

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind1, AstNodeKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2));
    return casted;
}

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind1, AstNodeKind kind2, AstNodeKind kind3)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3));
    return casted;
}

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind1, AstNodeKind kind2, AstNodeKind kind3, AstNodeKind kind4)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted && (casted->kind == kind1 || casted->kind == kind2 || casted->kind == kind3 || casted->kind == kind4));
    return casted;
}

extern atomic<int> g_UniqueID;
