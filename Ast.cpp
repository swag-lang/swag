#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Utf8Crc.h"

namespace Ast
{
    int findChildIndex(AstNode* parent, AstNode* child)
    {
        for (int i = 0; i < parent->childs.size(); i++)
        {
            if (parent->childs[i] == child)
                return i;
        }

        return -1;
    }

    void removeFromParent(AstNode* child)
    {
        auto parent = child->parent;
        parent->lock();
        for (int i = 0; i < parent->childs.size(); i++)
        {
            if (parent->childs[i] == child)
            {
                parent->childs.erase(parent->childs.begin() + i);
                child->parent = nullptr;
                parent->unlock();
                return;
            }
        }

        parent->unlock();
    }

    void addChildFront(AstNode* parent, AstNode* child)
    {
        if (!child)
            return;

        if (parent)
        {
            parent->lock();
            parent->childs.insert(parent->childs.begin(), child);
            parent->unlock();
        }

        child->parent = parent;
    }

    void addChildBack(AstNode* parent, AstNode* child)
    {
        if (!child)
            return;

        if (parent)
        {
            parent->lock();
            parent->childs.push_back(child);
            parent->unlock();
        }

        child->parent = parent;
    }

    Scope* findOrCreateScopeByName(Scope* parentScope, const string& name)
    {
        SWAG_ASSERT(parentScope);
        scoped_lock lock(parentScope->lockChilds);
        for (auto child : parentScope->childScopes)
        {
            if (child->name == name)
                return child;
        }

        return g_Pool_scope.alloc();
    }

    Scope* newScope(AstNode* owner, const string& name, ScopeKind kind, Scope* parentScope, bool matchName)
    {
        if (parentScope)
            parentScope->lockChilds.lock();

        if (matchName)
        {
            SWAG_ASSERT(parentScope);
            for (auto child : parentScope->childScopes)
            {
                if (child->name == name)
                {
                    parentScope->lockChilds.unlock();
                    return child;
                }
            }
        }

        auto newScope = g_Pool_scope.alloc();

        Utf8 fullname         = parentScope ? Scope::makeFullName(parentScope->fullname, name) : name;
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->owner       = owner;
        newScope->name        = name;
        newScope->fullname    = move(fullname);

        if (parentScope && newScope->indexInParent >= (uint32_t) parentScope->childScopes.size())
        {
            newScope->indexInParent = (uint32_t) parentScope->childScopes.size();
            parentScope->childScopes.push_back(newScope);
        }

        if (parentScope)
            parentScope->lockChilds.unlock();

        return newScope;
    }

    AstNode* createIdentifierRef(SyntaxJob* job, const Utf8Crc& name, const Token& token, AstNode* parent)
    {
        auto sourceFile    = job->sourceFile;
        auto idRef         = Ast::newNode(job, &g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
        idRef->semanticFct = &SemanticJob::resolveIdentifierRef;
        idRef->byteCodeFct = &ByteCodeGenJob::emitIdentifierRef;
        idRef->name        = name;
        idRef->token       = token;

        vector<string> tokens;
        tokenize(name.c_str(), '.', tokens);
        for (int i = 0; i < tokens.size(); i++)
        {
            auto id           = Ast::newNode(job, &g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, idRef);
            id->semanticFct   = &SemanticJob::resolveIdentifier;
            id->byteCodeFct   = &ByteCodeGenJob::emitIdentifier;
            id->name          = tokens[i];
            id->token         = token;
            id->identifierRef = idRef;
        }

        return idRef;
    }

    void visit(AstNode* root, const function<void(AstNode*)>& fctor)
    {
        fctor(root);
        for (auto child : root->childs)
            visit(child, fctor);
    }

    AstNode* clone(AstNode* source, AstNode* parent)
    {
        CloneContext cloneContext;
        cloneContext.parent = parent;
        return source->clone(cloneContext);
    }

    AstVarDecl* newVarDecl(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent)
    {
        AstVarDecl* node  = Ast::newNode(nullptr, &g_Pool_astVarDecl, AstNodeKind::VarDecl, sourceFile->indexInModule, parent);
        node->name        = name;
        node->semanticFct = &SemanticJob::resolveVarDecl;
        node->inheritOwners(parent);
        return node;
    }

    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent)
    {
        AstTypeExpression* node = Ast::newNode(nullptr, &g_Pool_astTypeExpression, AstNodeKind::TypeExpression, sourceFile->indexInModule, parent);
        node->semanticFct       = &SemanticJob::resolveTypeExpression;
        node->inheritOwners(parent);
        return node;
    }

    AstIdentifier* newIdentifier(SourceFile* sourceFile, const Utf8Crc& name, AstIdentifierRef* identifierRef, AstNode* parent)
    {
        AstIdentifier* node = Ast::newNode(nullptr, &g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, parent);
        node->name          = name;
        node->identifierRef = identifierRef;
        node->semanticFct   = &SemanticJob::resolveIdentifier;
        node->inheritOwners(parent ? parent : identifierRef);
        return node;
    }

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent)
    {
        AstIdentifierRef* node = Ast::newNode(nullptr, &g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
        node->name             = name;
        node->semanticFct      = &SemanticJob::resolveIdentifierRef;
        node->inheritOwners(parent);

        vector<string> tokens;
        tokenize(name.c_str(), '.', tokens);
        for (int i = 0; i < tokens.size(); i++)
        {
            auto id           = Ast::newNode(nullptr, &g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, node);
            id->semanticFct   = &SemanticJob::resolveIdentifier;
            id->name          = tokens[i];
            id->identifierRef = node;
            id->inheritOwners(node);
        }

        return node;
    }
}; // namespace Ast
