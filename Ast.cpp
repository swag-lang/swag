
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Module.h"

namespace Ast
{
    thread_local AstNode* lastGeneratedNode = nullptr;
    ;

    Utf8 literalToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg)
    {
        Utf8 result;
        result.reserve(text.capacity());

        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            result = format("%u", reg.u8);
            break;
        case NativeTypeKind::U16:
            result = format("%u", reg.u16);
            break;
        case NativeTypeKind::U32:
            result = format("%u", reg.u32);
            break;
        case NativeTypeKind::Char:
            if (reg.ch < 32)
                result += format("\\x%02x", reg.ch);
            else if (reg.ch > 127 && reg.ch <= 255)
                result += format("\\x%02x", reg.ch);
            else
                result += reg.ch;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            result = format("%llu", reg.u64);
            break;
        case NativeTypeKind::S8:
            result = format("%d", reg.s8);
            break;
        case NativeTypeKind::S16:
            result = format("%d", reg.s16);
            break;
        case NativeTypeKind::S32:
            result = format("%d", reg.s32);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            result = format("%lld", reg.s64);
            break;
        case NativeTypeKind::F32:
            result = toStringF64(reg.f32);
            break;
        case NativeTypeKind::F64:
            result = toStringF64(reg.f64);
            break;
        case NativeTypeKind::Bool:
            result = reg.b ? "true" : "false";
            return result;
        case NativeTypeKind::String:
        {
            for (auto c : text)
            {
                if (c < 32)
                    result += format("\\x%02x", c);
                else if (c > 127)
                    result += format("\\x%02x", c);
                else
                    result += c;
            }

            return result;
        }
        default:
            SWAG_ASSERT(false);
            return "";
        }

        return result;
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
        if (!child)
            return;
        auto parent = child->parent;
        if (!parent)
            return;
        scoped_lock lk(parent->mutex);
        for (int i = 0; i < parent->childs.size(); i++)
        {
            if (parent->childs[i] == child)
            {
                parent->childs.erase(i);
                child->parent = nullptr;

                // Update of each child index
                while (i < parent->childs.size())
                {
                    parent->childs[i]->childParentIdx = i;
                    i++;
                }

                return;
            }
        }
    }

    void insertChild(AstNode* parent, AstNode* child, uint32_t index)
    {
        if (!child)
            return;

        if (parent)
        {
            scoped_lock lk(parent->mutex);
            child->childParentIdx = index;
            parent->childs.insertAtIndex(child, index);
            for (auto i = index; i < parent->childs.size(); i++)
                parent->childs[i]->childParentIdx = i;
        }
        else
        {
            SWAG_ASSERT(index == 0);
        }

        child->parent = parent;
    }

    void addChildFront(AstNode* parent, AstNode* child)
    {
        if (!child)
            return;

        if (parent)
        {
            scoped_lock lk(parent->mutex);
            parent->childs.push_front(child);
            if (!child->ownerScope)
                child->inheritOwners(parent);
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
            if (!child->ownerScope)
                child->inheritOwners(parent);
        }

        child->parent = parent;
    }

    void setForceConstType(AstNode* node)
    {
        if (node && node->kind == AstNodeKind::TypeExpression)
            ((AstTypeExpression*) node)->typeFlags |= TYPEFLAG_FORCECONST;
    }

    Scope* newPrivateScope(AstNode* owner, SourceFile* sourceFile, Scope* parentScope)
    {
        SWAG_ASSERT(parentScope);
        if (parentScope == sourceFile->module->scopeRoot)
            return sourceFile->scopePrivate;
        sourceFile->computePrivateScopeName();
        return parentScope->getOrAddChild(owner, sourceFile->scopeName, ScopeKind::File, true, true);
    }

    Scope* newScope(AstNode* owner, const Utf8Crc& name, ScopeKind kind, Scope* parentScope, bool matchName)
    {
        if (parentScope)
            return parentScope->getOrAddChild(owner, name, kind, matchName, false);

        auto newScope         = g_Allocator.alloc<Scope>();
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->owner       = owner;
        newScope->name        = name;
        if (g_CommandLine.stats)
            g_Stats.memScopes += sizeof(Scope);

        return newScope;
    }

    void visit(AstNode* root, const function<void(AstNode*)>& fctor)
    {
        fctor(root);
        for (auto child : root->childs)
            visit(child, fctor);
    }

    AstNode* clone(AstNode* source, AstNode* parent, uint64_t forceFlags)
    {
        if (!source)
            return nullptr;
        CloneContext cloneContext;
        cloneContext.parent     = parent;
        cloneContext.forceFlags = forceFlags;
        return source->clone(cloneContext);
    }

    void normalizeIdentifierName(Utf8& name)
    {
        auto len = name.length();
        auto pz  = name.buffer;

        for (int i = 0; i < len; i++)
        {
            switch (*pz)
            {
            case '\'':
            case '.':
            case '(':
            case ')':
            case '-':
            case ',':
            case ' ':
                *pz = '_';
                break;

            case '&':
                *pz = 'R';
                break;
            case '*':
            case '>':
                *pz = 'P';
                break;
            case '[':
            case ']':
                *pz = 'A';
                break;
            }

            pz++;
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
        auto node         = Ast::newNode<AstNode>(syntaxJob, AstNodeKind::AffectOp, sourceFile, parent, 2);
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
        AstVarDecl* node  = Ast::newNode<AstVarDecl>(syntaxJob, kind, sourceFile, parent, 2);
        node->token.text  = name;
        node->semanticFct = SemanticJob::resolveVarDecl;
        return node;
    }

    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstTypeExpression* node = Ast::newNode<AstTypeExpression>(syntaxJob, AstNodeKind::TypeExpression, sourceFile, parent);
        node->semanticFct       = SemanticJob::resolveType;
        return node;
    }

    AstIdentifier* newIdentifier(SourceFile* sourceFile, const Utf8Crc& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstIdentifier* node = Ast::newNode<AstIdentifier>(syntaxJob, AstNodeKind::Identifier, sourceFile, parent);
        node->token.text    = name;
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
        node->token.text       = name;
        if (syntaxJob && !syntaxJob->currentTokenLocation)
            node->inheritTokenLocation(syntaxJob->token);

        vector<Utf8> subNames;
        tokenize(name.c_str(), '.', subNames);
        node->childs.reserve((int) subNames.size());

        for (int i = 0; i < subNames.size(); i++)
        {
            auto id         = Ast::newNode<AstIdentifier>(syntaxJob, AstNodeKind::Identifier, sourceFile, node);
            id->semanticFct = SemanticJob::resolveIdentifier;
            id->token.text  = subNames[i];
            id->token.id    = TokenId::Identifier;
            if (syntaxJob && !syntaxJob->currentTokenLocation)
                id->inheritTokenLocation(syntaxJob->token);
            id->identifierRef = node;
            id->inheritOwners(node);
        }

        return node;
    }

    Utf8 computeTypeDisplay(const Utf8& name, TypeInfo* typeInfo)
    {
        Utf8 result;
        if (typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
            result        = "func";
            if (!typeFunc->genericParameters.empty())
            {
                result += "(";
                for (int i = 0; i < (int) typeFunc->genericParameters.size(); i++)
                {
                    if (i)
                        result += ", ";
                    auto param = typeFunc->parameters[i];
                    result += param->typeInfo->name;
                }
                result += ")";
            }
            result += " ";

            result += name;
            result += "(";
            for (int i = 0; i < (int) typeFunc->parameters.size(); i++)
            {
                if (i)
                    result += ", ";
                auto param = typeFunc->parameters[i];
                result += param->typeInfo->name;
            }

            result += ")";
            if (typeFunc->returnType != g_TypeMgr.typeInfoVoid)
            {
                result += "->";
                result += typeFunc->returnType->name;
            }
        }

        return result;
    }

    Utf8 computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params)
    {
        if (params.empty())
            return "";

        Utf8 result = "with ";
        for (int i = 0; i < (int) params.size(); i++)
        {
            if (i)
                result += ", ";
            auto param = params[i];
            result += param->namedParam;
            result += " = ";
            result += param->typeInfo->name;
        }

        return result;
    }

    void releaseNode(AstNode* node)
    {
    }

}; // namespace Ast
