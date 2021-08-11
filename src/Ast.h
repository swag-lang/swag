#pragma once
#include "AstNode.h"
#include "Allocator.h"
#include "SourceFile.h"
#include "CommandLine.h"
struct Utf8;
struct Scope;
struct Concat;
enum class ScopeKind;

namespace Ast
{
    struct OutputContext
    {
        int  indent    = 0;
        bool forExport = false;
    };

    void initNewNode(AstNode* node, SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds);
    void removeFromParent(AstNode* child);
    void insertChild(AstNode* parent, AstNode* child, uint32_t index);
    void addChildBack(AstNode* parent, AstNode* child);
    void addChildFront(AstNode* parent, AstNode* child);
    int  findChildIndex(AstNode* parent, AstNode* child);

    void     visit(AstNode* root, const function<void(AstNode*)>& fctor);
    void     setForceConstType(AstNode* node);
    Utf8     enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg);
    Utf8     literalToString(TypeInfo* typeInfo, const ComputedValue& value);
    void     normalizeIdentifierName(Utf8& name);
    AstNode* cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags = 0);
    AstNode* clone(AstNode* source, AstNode* parent, uint64_t forceFlags = 0);
    Utf8     computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params);

    Scope*             newPrivateScope(AstNode* owner, SourceFile* file, Scope* parentScope);
    Scope*             newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    AstNode*           newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstStruct*         newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstFuncCallParams* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstFuncCallParam*  newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstVarDecl*        newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr, AstNodeKind kind = AstNodeKind::VarDecl);
    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstIdentifier*     newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstInline*         newInline(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    AstNode*           newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, SyntaxJob* syntaxJob = nullptr);

    bool outputAttributes(OutputContext& context, Concat& concat, AttributeList& attributes);
    bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    bool output(OutputContext& context, Concat& concat, AstNode* node);

    template<typename T>
    T* newNode()
    {
        auto node = g_Allocator.alloc0<T>();
        if (g_CommandLine.stats)
        {
            g_Stats.numNodes++;
            g_Stats.memNodes += Allocator::alignSize(sizeof(T));
        }

        return node;
    }

    template<typename T>
    T* newNode(SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds = 0)
    {
        auto node = newNode<T>();
        initNewNode(node, job, kind, sourceFile, parent, allocChilds);
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

extern atomic<int> g_UniqueID;
