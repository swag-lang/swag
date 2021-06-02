#pragma once
#include "Pool.h"
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
    template<typename T>
    T* newNode()
    {
        auto node = g_Allocator.alloc0<T>();
        if (g_CommandLine.stats)
            g_Stats.memNodes += sizeof(T);
        return node;
    }

    template<typename T>
    T* newNode(SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds = 0)
    {
        auto node        = newNode<T>();
        node->kind       = kind;
        node->parent     = parent;
        node->sourceFile = sourceFile;
        if (allocChilds)
            node->childs.reserve(allocChilds);

        if (job)
        {
            node->token.id   = job->token.id;
            node->token.text = job->token.text;

            if (job->currentTokenLocation)
            {
                node->token.startLocation = job->currentTokenLocation->startLocation;
                node->token.endLocation   = job->currentTokenLocation->endLocation;
            }
            else
            {
                node->token.startLocation = job->token.startLocation;
                node->token.endLocation   = job->token.endLocation;
            }

            node->inheritOwnersAndFlags(job);
        }
        else
        {
            if (parent)
                node->inheritTokenLocation(parent->token);
            node->inheritOwners(parent);
        }

        if (parent)
        {
            // Some flags are inherited from the parent, whatever...
            node->flags |= parent->flags & AST_NO_BACKEND;
            node->flags |= parent->flags & AST_RUN_BLOCK;
            node->flags |= parent->flags & AST_IN_MIXIN;

            parent->lock();
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
            parent->unlock();
        }

        return node;
    }

    extern Scope*   newPrivateScope(AstNode* owner, SourceFile* file, Scope* parentScope);
    extern Scope*   newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    extern void     removeFromParent(AstNode* child);
    extern void     insertChild(AstNode* parent, AstNode* child, uint32_t index);
    extern void     addChildBack(AstNode* parent, AstNode* child);
    extern void     addChildFront(AstNode* parent, AstNode* child);
    extern int      findChildIndex(AstNode* parent, AstNode* child);
    extern void     visit(AstNode* root, const function<void(AstNode*)>& fctor);
    extern void     setForceConstType(AstNode* node);
    extern Utf8     enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg);
    extern Utf8     literalToString(TypeInfo* typeInfo, const ComputedValue& value);
    extern void     normalizeIdentifierName(Utf8& name);
    extern AstNode* cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags = 0);
    extern AstNode* clone(AstNode* source, AstNode* parent, uint64_t forceFlags = 0);
    extern Utf8     computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params);
    extern void     releaseNode(AstNode* node);

    extern AstNode*           newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstStruct*         newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstFuncCallParams* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstFuncCallParam*  newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstVarDecl*        newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr, AstNodeKind kind = AstNodeKind::VarDecl);
    extern AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifier*     newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstInline*         newInline(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstNode*           newAffectOp(SourceFile* sourceFile, AstNode* parent, uint32_t opFlags, uint64_t attributeFlags, SyntaxJob* syntaxJob = nullptr);

    struct OutputContext
    {
        int  indent    = 0;
        bool forExport = false;
    };

    extern bool outputAttributes(OutputContext& context, Concat& concat, SymbolAttributes& attributes);
    extern bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const ComputedValue& value);
    extern bool output(OutputContext& context, Concat& concat, AstNode* node);
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
