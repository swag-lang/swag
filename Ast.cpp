#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Utf8Crc.h"

namespace Ast
{
    void addChild(AstNode* parent, AstNode* child)
    {
        if (parent)
        {
            assert(parent && child);
            parent->lock();
            parent->childs.push_back(child);
            parent->unlock();
        }

        child->parent = parent;
    }

    Scope* newScope(const string& name, ScopeKind kind, Scope* parentScope)
    {
        Utf8 fullname         = parentScope ? Scope::makeFullName(parentScope->fullname, name) : name;
        auto newScope         = g_Pool_scope.alloc();
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->name        = name;
        newScope->fullname    = move(fullname);
        if (parentScope)
        {
            scoped_lock lk(parentScope->lockChilds);
            parentScope->childScopes.push_back(newScope);
        }

        return newScope;
    }

    const char* getKindName(AstNode* node)
    {
        switch (node->kind)
        {
        case AstNodeKind::VarDecl:
            return "a variable";
        case AstNodeKind::FuncDecl:
            return "a function";
        case AstNodeKind::EnumDecl:
            return "an enum";
        case AstNodeKind::EnumValue:
            return "an enum value";
        case AstNodeKind::Namespace:
            return "a namespace";
        case AstNodeKind::TypeDecl:
            return "a type";
        case AstNodeKind::FuncDeclParam:
            return "a function parameter";
        }

        return "something else";
    }

    const char* getNakedName(AstNode* node)
    {
        switch (node->kind)
        {
        case AstNodeKind::VarDecl:
            return "variable";
        case AstNodeKind::FuncDecl:
            return "function";
        case AstNodeKind::EnumDecl:
            return "enum";
        case AstNodeKind::EnumValue:
            return "enum value";
        case AstNodeKind::Namespace:
            return "namespace";
        case AstNodeKind::TypeDecl:
            return "type";
        case AstNodeKind::FuncDeclParam:
            return "parameter";
        }

        return "something else";
    }

    AstNode* createIdentifierRef(SyntaxJob* job, const Utf8Crc& name, const Token& token, AstNode* parent)
    {
        auto sourceFile    = job->sourceFile;
        auto idRef         = Ast::newNode(&g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
        idRef->semanticFct = &SemanticJob::resolveIdentifierRef;
        idRef->inheritOwnersAndFlags(job);
        idRef->name  = name;
        idRef->token = token;

        auto id         = Ast::newNode(&g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, idRef);
        id->semanticFct = &SemanticJob::resolveIdentifier;
        id->inheritOwnersAndFlags(job);
        id->name  = name;
        id->token = token;

        return idRef;
    }
}; // namespace Ast
