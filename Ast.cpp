
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"

namespace Ast
{
    Utf8 literalToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg)
    {
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            return format("%u", reg.u8);
        case NativeTypeKind::U16:
            return format("%u", reg.u16);
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            return format("%u", reg.u32);
        case NativeTypeKind::U64:
            return format("%llu", reg.u64);
        case NativeTypeKind::S8:
            return format("%d", reg.s8);
        case NativeTypeKind::S16:
            return format("%d", reg.s16);
            break;
        case NativeTypeKind::S32:
            return format("%d", reg.s32);
            break;
        case NativeTypeKind::S64:
            return format("%lld", reg.s64);
            break;
        case NativeTypeKind::F32:
            return toStringF64(reg.f32);
            break;
        case NativeTypeKind::F64:
            return toStringF64(reg.f64);
            break;
        case NativeTypeKind::Bool:
            return reg.b ? "true" : "false";
            break;
        case NativeTypeKind::String:
        {
            Utf8 result;
            result.reserve(text.capacity());
            for (auto c : text)
            {
                switch (c)
                {
                case '\r':
                    result += "\\r";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                default:
                    result += c;
                }
            }

            return result;
        }
        default:
            SWAG_ASSERT(false);
            return "";
        }
    }

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
        auto        parent = child->parent;
        scoped_lock lk(parent->mutex);
        for (int i = 0; i < parent->childs.size(); i++)
        {
            if (parent->childs[i] == child)
            {
                parent->childs.erase(i);
                child->parent = nullptr;
                return;
            }
        }
    }

    void addChildFront(AstNode* parent, AstNode* child)
    {
        if (!child)
            return;

        if (parent)
        {
            scoped_lock lk(parent->mutex);
            parent->childs.push_front(child);
        }

        child->parent = parent;
    }

    void addChildBack(AstNode* parent, AstNode* child)
    {
        if (!child)
            return;

        if (parent)
        {
            scoped_lock lk(parent->mutex);
            child->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(child);
        }

        child->parent = parent;
    }

    void setForceConstType(AstNode* node)
    {
        if (node)
        {
            if (node->kind == AstNodeKind::TypeExpression)
                ((AstTypeExpression*) node)->forceConstType = true;
            if (node->kind == AstNodeKind::ExpressionList)
                ((AstExpressionList*) node)->forceConstType = true;
        }
    }

    Scope* newScope(AstNode* owner, const Utf8Crc& name, ScopeKind kind, Scope* parentScope, bool matchName)
    {
        // Do not create a scope if a scope with the same name already exists
        if (matchName)
        {
            SWAG_ASSERT(parentScope);
            shared_lock lk(parentScope->lockChilds);
            for (auto child : parentScope->childScopes)
            {
                if (child->name.compare(name))
                    return child;
            }
        }

        auto newScope = g_Allocator.alloc<Scope>();

        Utf8 fullname         = parentScope ? Scope::makeFullName(parentScope->fullname, (const string&) name) : (const string&) name;
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->owner       = owner;
        newScope->name        = name;
        newScope->fullname    = move(fullname);

        if (parentScope)
        {
            unique_lock lk(parentScope->lockChilds);
            newScope->indexInParent = (uint32_t) parentScope->childScopes.size();
            parentScope->childScopes.push_back(newScope);
        }

        return newScope;
    }

    void visit(AstNode* root, const function<void(AstNode*)>& fctor)
    {
        fctor(root);
        for (auto child : root->childs)
            visit(child, fctor);
    }

    AstNode* clone(AstNode* source, AstNode* parent)
    {
        if (!source)
            return nullptr;
        CloneContext cloneContext;
        cloneContext.parent = parent;
        return source->clone(cloneContext);
    }

    void normalizeIdentifierName(Utf8& name)
    {
        auto len = name.length();
        for (int i = 0; i < len; i++)
        {
            auto& c = name[i];
            if (c == '*')
                c = 'P';
            else if (c == '.')
                c = '_';
        }
    }

    AstNode* newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstNode* node = Ast::newNode<AstNode>(syntaxJob, kind, sourceFile, parent);
        return node;
    }

    AstInline* newInline(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstInline* node         = Ast::newNode<AstInline>(syntaxJob, AstNodeKind::Inline, sourceFile, parent);
        node->semanticBeforeFct = SemanticJob::resolveInlineBefore;
        node->semanticAfterFct  = SemanticJob::resolveInlineAfter;
        node->byteCodeBeforeFct = ByteCodeGenJob::emitInlineBefore;
        node->byteCodeFct       = ByteCodeGenJob::emitInline;
        return node;
    }

    AstNode* newAffectOp(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        auto node         = Ast::newNode<AstNode>(syntaxJob, AstNodeKind::AffectOp, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveAffect;
        node->flags |= AST_REVERSE_SEMANTIC;
        return node;
    }

    AstStruct* newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstStruct* node   = Ast::newNode<AstStruct>(syntaxJob, AstNodeKind::StructDecl, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveStruct;
        return node;
    }

    AstNode* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstNode* node     = Ast::newNode<AstNode>(syntaxJob, AstNodeKind::FuncCallParams, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncCallParams;
        return node;
    }

    AstFuncCallParam* newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstFuncCallParam* node = Ast::newNode<AstFuncCallParam>(syntaxJob, AstNodeKind::FuncCallParam, sourceFile, parent);
        node->semanticFct      = SemanticJob::resolveFuncCallParam;
        return node;
    }

    AstVarDecl* newVarDecl(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob, AstNodeKind kind)
    {
        AstVarDecl* node = Ast::newNode<AstVarDecl>(syntaxJob, kind, sourceFile, parent);

        // We need to evaluate assignment first, then type, in order to be able to generate the type depending on the
        // assignment if necessary
        node->flags |= AST_REVERSE_SEMANTIC;

        node->name        = name;
        node->semanticFct = SemanticJob::resolveVarDecl;
        return node;
    }

    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstTypeExpression* node = Ast::newNode<AstTypeExpression>(syntaxJob, AstNodeKind::TypeExpression, sourceFile, parent);
        node->semanticFct       = SemanticJob::resolveTypeExpression;
        return node;
    }

    AstIdentifier* newIdentifier(SourceFile* sourceFile, const Utf8Crc& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstIdentifier* node = Ast::newNode<AstIdentifier>(syntaxJob, AstNodeKind::Identifier, sourceFile, parent);
        node->name          = name;
        node->identifierRef = identifierRef;
        node->semanticFct   = SemanticJob::resolveIdentifier;
        if (identifierRef)
            node->inheritOwners(identifierRef);
        return node;
    }

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstIdentifierRef* node = Ast::newNode<AstIdentifierRef>(syntaxJob, AstNodeKind::IdentifierRef, sourceFile, parent);
        node->semanticFct      = SemanticJob::resolveIdentifierRef;
        node->byteCodeFct      = ByteCodeGenJob::emitIdentifierRef;
        return node;
    }

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, const Utf8Crc& name, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstIdentifierRef* node = Ast::newIdentifierRef(sourceFile, parent, syntaxJob);
        node->name             = name;
        if (syntaxJob)
            node->inheritTokenLocation(syntaxJob->token);

        vector<string> subNames;
        tokenize(name.c_str(), '.', subNames);
        for (int i = 0; i < subNames.size(); i++)
        {
            auto id         = Ast::newNode<AstIdentifier>(syntaxJob, AstNodeKind::Identifier, sourceFile, node);
            id->semanticFct = SemanticJob::resolveIdentifier;
            id->name        = move(subNames[i]);
            if (syntaxJob)
                id->inheritTokenLocation(syntaxJob->token);
            id->identifierRef = node;
            id->inheritOwners(node);
        }

        return node;
    }

    Utf8 computeFullNameForeign(AstNode* node, bool forExport)
    {
        if (!forExport)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
            auto it       = typeFunc->attributes.values.find("swag.foreign.function");
            if (it != typeFunc->attributes.values.end())
                return it->second.second.text;
            return node->name;
        }

        Utf8 fullnameForeign;

        SWAG_ASSERT(node->ownerScope);
        if (!node->ownerScope->fullname.empty())
            concatForC(fullnameForeign, node->ownerScope->fullname);
        fullnameForeign += format("_%lX", (uint64_t) node);

        return fullnameForeign;
    }

}; // namespace Ast
