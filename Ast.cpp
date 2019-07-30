#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Utf8Crc.h"

namespace Ast
{
    void addChild(AstNode* parent, AstNode* child)
    {
        if (parent)
        {
            SWAG_ASSERT(parent && child);
            parent->lock();
            parent->childs.push_back(child);
            parent->unlock();
        }

        child->parent = parent;
    }

    Scope* newScope(const string& name, ScopeKind kind, Scope* parentScope, bool singleNamed)
    {
        if (parentScope)
        {
            parentScope->lockChilds.lock();

            // A scope with the same name already exists
            if (singleNamed)
            {
                for (auto child : parentScope->childScopes)
                {
					if (child->name == name)
					{
						parentScope->lockChilds.unlock();
						return child;
					}
                }
            }
        }

        Utf8 fullname         = parentScope ? Scope::makeFullName(parentScope->fullname, name) : name;
        auto newScope         = g_Pool_scope.alloc();
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->name        = name;
        newScope->fullname    = move(fullname);
        if (parentScope)
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
        auto idRef         = Ast::newNode(&g_Pool_astIdentifierRef, AstNodeKind::IdentifierRef, sourceFile->indexInModule, parent);
        idRef->semanticFct = &SemanticJob::resolveIdentifierRef;
        idRef->byteCodeFct = &ByteCodeGenJob::emitIdentifierRef;
        idRef->inheritOwnersAndFlags(job);
        idRef->name  = name;
        idRef->token = token;

        auto id         = Ast::newNode(&g_Pool_astIdentifier, AstNodeKind::Identifier, sourceFile->indexInModule, idRef);
        id->semanticFct = &SemanticJob::resolveIdentifier;
        id->byteCodeFct = &ByteCodeGenJob::emitIdentifier;
        id->inheritOwnersAndFlags(job);
        id->name  = name;
        id->token = token;

        return idRef;
    }
}; // namespace Ast
