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
    T* newNode(SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent = nullptr)
    {
        auto node        = g_Allocator.alloc<T>();
        node->kind       = kind;
        node->parent     = parent;
        node->ownerScope = nullptr;
        node->ownerFct   = nullptr;
        node->sourceFile = sourceFile;

        if (job)
        {
            node->token.id = job->token.id;
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

            parent->lock();

            // If previous node is a doc comment, then move the text to this node
            if (g_CommandLine.generateDoc)
            {
                auto childBack = parent->childs.empty() ? nullptr : parent->childs.back();
                if (childBack && (childBack->kind == AstNodeKind::DocComment || childBack->kind == AstNodeKind::AttrUse))
                    node->docContent = move(childBack->docContent);
            }

            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
            parent->unlock();
        }

        lastGeneratedNode = node;
        return node;
    }

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
    extern AstNode* clone(AstNode* source, AstNode* parent);
    extern Utf8     computeTypeDisplay(const Utf8& name, TypeInfo* typeInfo);
    extern Utf8     computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params);

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

    extern bool outputLiteral(Concat& concat, AstNode* node, TypeInfo* typeInfo, const Utf8& text, Register& reg);
    extern bool output(Concat& concat, AstNode* node, int indent = 0);
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
