#pragma once
#include "Pool.h"
#include "AstNode.h"
#include "Allocator.h"
#include "SourceFile.h"
struct Utf8Crc;
struct Scope;
struct Concat;
enum class ScopeKind;

namespace Ast
{
    extern thread_local AstNode* lastGeneratedNode;

    template<typename T>
    T* newNode(SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds = 0)
    {
        auto node        = g_Allocator.alloc0<T>();
        node->kind       = kind;
        node->parent     = parent;
        node->sourceFile = sourceFile;
        if (allocChilds)
            node->childs.reserve(allocChilds);

        if (job)
        {
            node->token.id            = job->token.id;
            node->token.startLocation = job->token.startLocation;
            node->token.endLocation   = job->token.endLocation;

            if (job->currentTokenLocation)
            {
                node->token.startLocation.line = job->currentTokenLocation->startLocation.line;
                node->token.endLocation.line   = job->currentTokenLocation->endLocation.line;
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

            parent->lock();
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
            parent->unlock();
        }

        lastGeneratedNode = node;
        return node;
    }

    extern Scope*   newPrivateScope(AstNode* owner, SourceFile* file, Scope* parentScope);
    extern Scope*   newScope(AstNode* owner, const Utf8Crc& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    extern void     removeFromParent(AstNode* child);
    extern void     insertChild(AstNode* parent, AstNode* child, uint32_t index);
    extern void     addChildBack(AstNode* parent, AstNode* child);
    extern void     addChildFront(AstNode* parent, AstNode* child);
    extern int      findChildIndex(AstNode* parent, AstNode* child);
    extern void     visit(AstNode* root, const function<void(AstNode*)>& fctor);
    extern void     setForceConstType(AstNode* node);
    extern Utf8     literalToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg);
    extern void     normalizeIdentifierName(Utf8& name);
    extern AstNode* clone(AstNode* source, AstNode* parent, uint64_t forceFlags = 0);
    extern Utf8     computeTypeDisplay(const Utf8& name, TypeInfo* typeInfo);
    extern Utf8     computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params);
    extern void     releaseNode(AstNode* node);

    extern AstNode*           newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstStruct*         newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstNode*           newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstFuncCallParam*  newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstVarDecl*        newVarDecl(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr, AstNodeKind kind = AstNodeKind::VarDecl);
    extern AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifier*     newIdentifier(SourceFile* sourceFile, const Utf8Crc& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstInline*         newInline(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstNode*           newAffectOp(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);

    struct OutputContext
    {
        int  indent    = 0;
        bool forExport = false;
    };

    extern bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const Utf8& text, Register& reg);
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
