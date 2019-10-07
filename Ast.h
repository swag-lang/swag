#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "AstNode.h"
struct Utf8Crc;
struct Scope;
enum class ScopeKind;

namespace Ast
{
    template<typename T>
    T* newNode(SyntaxJob* job, Pool<T>* pool, AstNodeKind kind, uint32_t sourceFileIdx, AstNode* parent = nullptr)
    {
        auto node           = pool->alloc();
        node->kind          = kind;
        node->parent        = parent;
        node->ownerScope    = nullptr;
        node->ownerFct      = nullptr;
        node->sourceFileIdx = sourceFileIdx;

        if (job)
        {
            node->token.id            = job->token.id;
            node->token.startLocation = job->token.startLocation;
            node->token.endLocation   = job->token.endLocation;
            node->inheritOwnersAndFlags(job);
        }
        else
        {
            node->inheritOwners(parent);
        }

        if (parent)
        {
            parent->lock();
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
            parent->unlock();
        }

        return node;
    }

    extern Scope* newScope(AstNode* owner, const string& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    extern void   removeFromParent(AstNode* child);
    extern void   addChildBack(AstNode* parent, AstNode* child);
    extern void   addChildFront(AstNode* parent, AstNode* child);
    extern int    findChildIndex(AstNode* parent, AstNode* child);
    extern void   visit(AstNode* root, const function<void(AstNode*)>& fctor);
    extern void   setForceConstType(AstNode* node);

    extern AstNode* createIdentifierRef(SyntaxJob* job, const Utf8Crc& name, const Token& token, AstNode* parent);

    extern AstNode* clone(AstNode* source, AstNode* parent);

    extern AstNode*           newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstStruct*         newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstNode*           newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstFuncCallParam*  newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstVarDecl*        newVarDecl(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifier*     newIdentifier(SourceFile* sourceFile, const Utf8Crc& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
    extern AstIdentifierRef*  newIdentifierRef(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob = nullptr);
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