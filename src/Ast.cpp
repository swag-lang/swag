
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Module.h"

atomic<int> g_UniqueID;

namespace Ast
{
    void initNewNode(AstNode* node, SyntaxJob* job, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds = 0)
    {
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

            ScopedLock lk(parent->mutex);
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
        }
    }

    Utf8 enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg)
    {
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Enum);

        Utf8 result;

        bool found    = false;
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);

        if (typeEnum->declNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS)
        {
            SWAG_ASSERT(typeEnum->rawType->kind == TypeInfoKind::Native);
            for (int i = 0; i < typeEnum->values.size(); i++)
            {
                auto value = typeEnum->values[i];
                bool ok    = false;
                switch (typeEnum->rawType->nativeType)
                {
                case NativeTypeKind::U8:
                    if (value->value.reg.u8 & reg.u8)
                        ok = true;
                    break;
                case NativeTypeKind::U16:
                    if (value->value.reg.u16 & reg.u16)
                        ok = true;
                    break;
                case NativeTypeKind::U32:
                    if (value->value.reg.u32 & reg.u32)
                        ok = true;
                    break;
                case NativeTypeKind::U64:
                case NativeTypeKind::UInt:
                    if (value->value.reg.u64 & reg.u64)
                        ok = true;
                    break;
                }

                if (ok)
                {
                    found = true;
                    if (!result.empty())
                        result += "|";
                    result += typeInfo->name;
                    result += ".";
                    result += value->namedParam;
                }
            }
        }
        else
        {
            result = typeInfo->name;
            result += ".";
            for (int i = 0; i < typeEnum->values.size(); i++)
            {
                auto value = typeEnum->values[i];
                bool ok    = false;
                if (typeEnum->rawType->kind == TypeInfoKind::Native)
                {
                    switch (typeEnum->rawType->nativeType)
                    {
                    case NativeTypeKind::S8:
                    case NativeTypeKind::U8:
                        if (value->value.reg.u8 == reg.u8)
                            ok = true;
                        break;
                    case NativeTypeKind::S16:
                    case NativeTypeKind::U16:
                        if (value->value.reg.u16 == reg.u16)
                            ok = true;
                        break;
                    case NativeTypeKind::S32:
                    case NativeTypeKind::U32:
                    case NativeTypeKind::Rune:
                    case NativeTypeKind::F32:
                        if (value->value.reg.u32 == reg.u32)
                            ok = true;
                        break;
                    case NativeTypeKind::S64:
                    case NativeTypeKind::U64:
                    case NativeTypeKind::Int:
                    case NativeTypeKind::UInt:
                    case NativeTypeKind::F64:
                        if (value->value.reg.u64 == reg.u64)
                            ok = true;
                        break;
                    case NativeTypeKind::String:
                        if (value->value.text == text)
                            ok = true;
                        break;
                    }

                    if (ok)
                    {
                        found = true;
                        result += value->namedParam;
                        break;
                    }
                }
            }
        }

        if (!found)
            result += "?";
        return result;
    }

    Utf8 literalToString(TypeInfo* typeInfo, const ComputedValue& value)
    {
        Utf8 result;
        result.reserve(value.text.capacity());

        if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            result = Utf8::format("%u", value.storageOffset);
            return result;
        }

        auto& reg = value.reg;
        typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            result = Utf8::format("%u", reg.u8);
            break;
        case NativeTypeKind::U16:
            result = Utf8::format("%u", reg.u16);
            break;
        case NativeTypeKind::U32:
            result = Utf8::format("%u", reg.u32);
            break;
        case NativeTypeKind::Rune:
            if (reg.ch < 32)
                result += Utf8::format("\\x%02x", reg.ch);
            else if (reg.ch > 127 && reg.ch <= 255)
                result += Utf8::format("\\x%02x", reg.ch);
            else
                result += reg.ch;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            result = Utf8::format("%llu", reg.u64);
            break;
        case NativeTypeKind::S8:
            result = Utf8::format("%d", reg.s8);
            break;
        case NativeTypeKind::S16:
            result = Utf8::format("%d", reg.s16);
            break;
        case NativeTypeKind::S32:
            result = Utf8::format("%d", reg.s32);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            result = Utf8::format("%lld", reg.s64);
            break;
        case NativeTypeKind::F32:
            result = Utf8::toStringF64(reg.f32);
            break;
        case NativeTypeKind::F64:
            result = Utf8::toStringF64(reg.f64);
            break;
        case NativeTypeKind::Bool:
            result = reg.b ? "true" : "false";
            return result;
        case NativeTypeKind::String:
        {
            for (auto c : value.text)
            {
                if (c < 32)
                    result += Utf8::format("\\x%02x", c);
                else if (c > 127)
                    result += Utf8::format("\\x%02x", c);
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

        ScopedLock lk(parent->mutex);
        auto        idx = child->childParentIdx;
        SWAG_ASSERT(parent->childs[idx] == child);
        parent->childs.erase(idx);
        for (int i = idx; i < parent->childs.size(); i++)
            parent->childs[i]->childParentIdx = i;
        child->parent = nullptr;
    }

    void insertChild(AstNode* parent, AstNode* child, uint32_t index)
    {
        if (!child)
            return;

        if (parent)
        {
            ScopedLock lk(parent->mutex);
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
            ScopedLock lk(parent->mutex);
            child->childParentIdx = 0;
            parent->childs.push_front(child);
            for (auto i = 1; i < parent->childs.size(); i++)
                parent->childs[i]->childParentIdx = i;
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
            ScopedLock lk(parent->mutex);
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

    Scope* newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName)
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

    AstNode* cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags)
    {
        if (!source)
            return nullptr;
        CloneContext cloneContext;
        cloneContext.parent     = parent;
        cloneContext.forceFlags = forceFlags;
        cloneContext.rawClone   = true;
        return source->clone(cloneContext);
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
        AstInline* node = Ast::newNode<AstInline>(syntaxJob, AstNodeKind::Inline, sourceFile, parent);
        node->allocateExtension();
        node->extension->semanticBeforeFct = SemanticJob::resolveInlineBefore;
        node->extension->semanticAfterFct  = SemanticJob::resolveInlineAfter;
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitInlineBefore;
        node->byteCodeFct                  = ByteCodeGenJob::emitInline;
        return node;
    }

    AstNode* newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, SyntaxJob* syntaxJob)
    {
        auto node         = Ast::newNode<AstOp>(syntaxJob, AstNodeKind::AffectOp, sourceFile, parent, 2);
        node->semanticFct = SemanticJob::resolveAffect;
        node->specFlags |= opFlags;
        node->attributeFlags |= attributeFlags;
        return node;
    }

    AstStruct* newStructDecl(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstStruct* node   = Ast::newNode<AstStruct>(syntaxJob, AstNodeKind::StructDecl, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveStruct;
        return node;
    }

    AstFuncCallParams* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        auto node         = Ast::newNode<AstFuncCallParams>(syntaxJob, AstNodeKind::FuncCallParams, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncCallParams;
        return node;
    }

    AstFuncCallParam* newFuncCallParam(SourceFile* sourceFile, AstNode* parent, SyntaxJob* syntaxJob)
    {
        AstFuncCallParam* node = Ast::newNode<AstFuncCallParam>(syntaxJob, AstNodeKind::FuncCallParam, sourceFile, parent);
        node->semanticFct      = SemanticJob::resolveFuncCallParam;
        return node;
    }

    AstVarDecl* newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob, AstNodeKind kind)
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

    AstIdentifier* newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, SyntaxJob* syntaxJob)
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

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, SyntaxJob* syntaxJob)
    {
        SWAG_ASSERT(!name.empty());

        AstIdentifierRef* node = Ast::newIdentifierRef(sourceFile, parent, syntaxJob);
        node->token.text       = name;
        if (syntaxJob && !syntaxJob->currentTokenLocation)
            node->inheritTokenLocation(syntaxJob->token);

        Utf8 str;
        auto pz = name.buffer;
        while (*pz)
        {
            auto pzStart = pz;
            while (*pz && *pz != '.')
                pz++;

            auto id         = Ast::newNode<AstIdentifier>(syntaxJob, AstNodeKind::Identifier, sourceFile, node);
            id->semanticFct = SemanticJob::resolveIdentifier;
            str.buffer      = pzStart;
            str.count       = (int) (pz - pzStart);
            id->token.text  = str;

            id->token.id = TokenId::Identifier;
            if (syntaxJob && !syntaxJob->currentTokenLocation)
                id->inheritTokenLocation(syntaxJob->token);
            id->identifierRef = node;
            id->inheritOwners(node);

            if (*pz)
                pz++;
        }

        str.buffer = nullptr; // to avoid free on destruction
        return node;
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

}; // namespace Ast
