
#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Parser.h"

atomic<int> g_UniqueID;

namespace Ast
{
    void initNewNode(AstNode* node, Parser* parser, AstNodeKind kind, SourceFile* sourceFile, AstNode* parent, uint32_t allocChilds = 0)
    {
        node->kind       = kind;
        node->parent     = parent;
        node->sourceFile = sourceFile;

        if (allocChilds)
            node->childs.reserve(allocChilds);

        if (parser)
        {
            node->token.id   = parser->token.id;
            node->token.text = parser->token.text;

            if (parser->currentTokenLocation)
            {
                node->token.startLocation = parser->currentTokenLocation->startLocation;
                node->token.endLocation   = parser->currentTokenLocation->endLocation;
            }
            else
            {
                node->token.startLocation = parser->token.startLocation;
                node->token.endLocation   = parser->token.endLocation;
            }

            node->inheritOwnersAndFlags(parser);
        }
        else
        {
            if (parent)
                node->inheritTokenLocation(parent);
            node->inheritOwners(parent);
        }

        if (parent)
        {
            // Count nodes (not precise). Just to have an hint on the number of bytecode instructions
            if (parent->ownerFct)
                parent->ownerFct->nodeCounts++;

            // Some flags are inherited from the parent, whatever...
            node->flags |= parent->flags & (AST_NO_BACKEND | AST_RUN_BLOCK | AST_IN_MIXIN);

            ScopedLock lk(parent->mutex);
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
        }
    }

    Utf8 enumToString(TypeInfo* typeInfo, const Utf8& text, const Register& reg, bool scoped)
    {
        SWAG_ASSERT(typeInfo->isEnum());

        Utf8 result;

        bool found    = false;
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);

        if (typeEnum->declNode->attributeFlags & ATTRIBUTE_ENUM_FLAGS)
        {
            SWAG_ASSERT(typeEnum->rawType->isNative());
            for (int i = 0; i < typeEnum->values.size(); i++)
            {
                auto value = typeEnum->values[i];
                bool ok    = false;
                SWAG_ASSERT(value->value);
                switch (typeEnum->rawType->nativeType)
                {
                case NativeTypeKind::U8:
                    if (value->value->reg.u8 == 0 && reg.u8 == 0)
                        ok = true;
                    else if (value->value->reg.u8 == 0)
                        break;
                    else if ((value->value->reg.u8 & reg.u8) == value->value->reg.u8)
                        ok = true;
                    break;
                case NativeTypeKind::U16:
                    if (value->value->reg.u16 == 0 && reg.u16 == 0)
                        ok = true;
                    else if (value->value->reg.u16 == 0)
                        break;
                    else if ((value->value->reg.u16 & reg.u16) == value->value->reg.u16)
                        ok = true;
                    break;
                case NativeTypeKind::U32:
                    if (value->value->reg.u32 == 0 && reg.u32 == 0)
                        ok = true;
                    else if (value->value->reg.u32 == 0)
                        break;
                    else if ((value->value->reg.u32 & reg.u32) == value->value->reg.u32)
                        ok = true;
                    break;
                case NativeTypeKind::U64:
                    if (value->value->reg.u64 == 0 && reg.u64 == 0)
                        ok = true;
                    else if (value->value->reg.u64 == 0)
                        break;
                    else if ((value->value->reg.u64 & reg.u64) == value->value->reg.u64)
                        ok = true;
                    break;
                }

                if (ok)
                {
                    found = true;
                    if (!result.empty())
                        result += "|";
                    if (scoped)
                        result += typeInfo->name;
                    result += ".";
                    result += value->namedParam;
                }
            }
        }
        else
        {
            if (scoped)
                result = typeInfo->name;
            result += ".";
            for (int i = 0; i < typeEnum->values.size(); i++)
            {
                auto value = typeEnum->values[i];
                bool ok    = false;
                SWAG_ASSERT(value->value);
                if (typeEnum->rawType->isNative())
                {
                    switch (typeEnum->rawType->nativeType)
                    {
                    case NativeTypeKind::S8:
                    case NativeTypeKind::U8:
                        if (value->value->reg.u8 == reg.u8)
                            ok = true;
                        break;
                    case NativeTypeKind::S16:
                    case NativeTypeKind::U16:
                        if (value->value->reg.u16 == reg.u16)
                            ok = true;
                        break;
                    case NativeTypeKind::S32:
                    case NativeTypeKind::U32:
                    case NativeTypeKind::Rune:
                    case NativeTypeKind::F32:
                        if (value->value->reg.u32 == reg.u32)
                            ok = true;
                        break;
                    case NativeTypeKind::S64:
                    case NativeTypeKind::U64:
                    case NativeTypeKind::F64:
                        if (value->value->reg.u64 == reg.u64)
                            ok = true;
                        break;
                    case NativeTypeKind::String:
                        if (value->value->text == text)
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

        if (typeInfo->isStruct() || typeInfo->isListArray())
        {
            result = Fmt("%u", value.storageOffset);
            return result;
        }

        auto& reg = value.reg;
        typeInfo  = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
        SWAG_ASSERT(typeInfo->isNative());
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            result = Fmt("%u", reg.u8);
            break;
        case NativeTypeKind::U16:
            result = Fmt("%u", reg.u16);
            break;
        case NativeTypeKind::U32:
            result = Fmt("%u", reg.u32);
            break;
        case NativeTypeKind::Rune:
            if (reg.ch < 32)
                result += Fmt("\\x%02x", reg.ch);
            else if (reg.ch > 127 && reg.ch <= 255)
                result += Fmt("\\x%02x", reg.ch);
            else
                result += reg.ch;
            break;
        case NativeTypeKind::U64:
            result = Fmt("%llu", reg.u64);
            break;
        case NativeTypeKind::S8:
            result = Fmt("%d", reg.s8);
            break;
        case NativeTypeKind::S16:
            result = Fmt("%d", reg.s16);
            break;
        case NativeTypeKind::S32:
            result = Fmt("%d", reg.s32);
            break;
        case NativeTypeKind::S64:
            result = Fmt("%lld", reg.s64);
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
                    result += Fmt("\\x%02x", c);
                else if (c > 127)
                    result += Fmt("\\x%02x", c);
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
        auto       idx = child->childParentIdx;
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
            ((AstTypeExpression*) node)->typeFlags |= TYPEFLAG_FORCE_CONST;
    }

    Scope* newScope(AstNode* owner, const Utf8& name, ScopeKind kind, Scope* parentScope, bool matchName)
    {
        if (parentScope)
            return parentScope->getOrAddChild(owner, name, kind, matchName);

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

    bool visit(ErrorContext* context, AstNode* root, const function<bool(ErrorContext*, AstNode*)>& fctor)
    {
        if (!fctor(context, root))
            return false;
        for (auto child : root->childs)
        {
            if (!visit(context, child, fctor))
                return false;
        }

        return true;
    }

    AstNode* cloneRaw(AstNode* source, AstNode* parent, uint64_t forceFlags, uint64_t removeFlags)
    {
        if (!source)
            return nullptr;
        CloneContext cloneContext;
        cloneContext.parent      = parent;
        cloneContext.forceFlags  = forceFlags;
        cloneContext.removeFlags = removeFlags;
        cloneContext.cloneFlags |= CLONE_RAW;
        return source->clone(cloneContext);
    }

    AstNode* clone(AstNode* source, AstNode* parent, uint64_t forceFlags, uint64_t removeFlags)
    {
        if (!source)
            return nullptr;
        CloneContext cloneContext;
        cloneContext.parent      = parent;
        cloneContext.forceFlags  = forceFlags;
        cloneContext.removeFlags = removeFlags;
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

    AstNode* newNode(SourceFile* sourceFile, AstNodeKind kind, AstNode* parent, Parser* parser)
    {
        return Ast::newNode<AstNode>(parser, kind, sourceFile, parent);
    }

    AstInline* newInline(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node = Ast::newNode<AstInline>(parser, AstNodeKind::Inline, sourceFile, parent);
        node->allocateExtension(ExtensionKind::Semantic);
        node->extension->semantic->semanticAfterFct  = SemanticJob::resolveInlineAfter;
        node->extension->semantic->semanticBeforeFct = SemanticJob::resolveInlineBefore;
        node->allocateExtension(ExtensionKind::ByteCode);
        node->extension->bytecode->byteCodeBeforeFct = ByteCodeGenJob::emitInlineBefore;
        node->byteCodeFct                            = ByteCodeGenJob::emitInline;
        return node;
    }

    AstNode* newAffectOp(SourceFile* sourceFile, AstNode* parent, uint8_t opFlags, uint64_t attributeFlags, Parser* parser)
    {
        auto node         = Ast::newNode<AstOp>(parser, AstNodeKind::AffectOp, sourceFile, parent, 2);
        node->semanticFct = SemanticJob::resolveAffect;
        node->specFlags |= opFlags;
        node->attributeFlags |= attributeFlags;
        return node;
    }

    AstStruct* newStructDecl(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstStruct>(parser, AstNodeKind::StructDecl, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveStruct;
        return node;
    }

    AstNode* newFuncDeclParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstNode>(parser, AstNodeKind::FuncDeclParams, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncDeclParams;
        node->flags |= AST_NO_BYTECODE_CHILDS; // We do not want default assignations to generate bytecode
        return node;
    }

    AstFuncCallParams* newFuncCallGenParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncCallGenParams;
        return node;
    }

    AstFuncCallParams* newFuncCallParams(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstFuncCallParams>(parser, AstNodeKind::FuncCallParams, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncCallParams;
        return node;
    }

    AstFuncCallParam* newFuncCallParam(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstFuncCallParam>(parser, AstNodeKind::FuncCallParam, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveFuncCallParam;
        return node;
    }

    AstVarDecl* newVarDecl(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser, AstNodeKind kind)
    {
        auto node         = Ast::newNode<AstVarDecl>(parser, kind, sourceFile, parent, 2);
        node->token.text  = name;
        node->semanticFct = SemanticJob::resolveVarDecl;
        return node;
    }

    AstIntrinsicProp* newIntrinsicProp(SourceFile* sourceFile, TokenId id, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstIntrinsicProp>(parser, AstNodeKind::IntrinsicProp, sourceFile, parent);
        node->token.id    = id;
        node->semanticFct = SemanticJob::resolveIntrinsicProperty;
        return node;
    }

    AstTypeExpression* newTypeExpression(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node         = Ast::newNode<AstTypeExpression>(parser, AstNodeKind::TypeExpression, sourceFile, parent);
        node->semanticFct = SemanticJob::resolveType;
        return node;
    }

    AstIdentifier* newIdentifier(SourceFile* sourceFile, const Utf8& name, AstIdentifierRef* identifierRef, AstNode* parent, Parser* parser)
    {
        auto node           = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, parent);
        node->token.text    = name;
        node->identifierRef = identifierRef;
        node->semanticFct   = SemanticJob::resolveIdentifier;
        if (identifierRef)
            node->inheritOwners(identifierRef);
        return node;
    }

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, AstNode* parent, Parser* parser)
    {
        auto node = Ast::newNode<AstIdentifierRef>(parser, AstNodeKind::IdentifierRef, sourceFile, parent);
        node->allocateExtension(ExtensionKind::Semantic);
        node->extension->semantic->semanticBeforeFct = SemanticJob::preResolveIdentifierRef;
        node->semanticFct                            = SemanticJob::resolveIdentifierRef;
        node->byteCodeFct                            = ByteCodeGenJob::emitIdentifierRef;
        return node;
    }

    AstIdentifierRef* newIdentifierRef(SourceFile* sourceFile, const Utf8& name, AstNode* parent, Parser* parser)
    {
        SWAG_ASSERT(!name.empty());

        auto node        = Ast::newIdentifierRef(sourceFile, parent, parser);
        node->token.text = name;
        if (parser && !parser->currentTokenLocation)
            node->inheritTokenLocation(parser->token);

        Utf8 str;
        int  cpt = 0;
        auto pz  = name.buffer;
        while (*pz && cpt != name.count)
        {
            auto pzStart = pz;
            while (*pz && *pz != '.' && cpt != name.count)
            {
                pz++;
                cpt++;
            }

            auto id         = Ast::newNode<AstIdentifier>(parser, AstNodeKind::Identifier, sourceFile, node);
            id->semanticFct = SemanticJob::resolveIdentifier;
            str.buffer      = pzStart;
            str.count       = (int) (pz - pzStart);
            str.allocated   = name.allocated;
            id->token.text  = str;

            id->token.id = TokenId::Identifier;
            if (parser && !parser->currentTokenLocation)
                id->inheritTokenLocation(parser->token);
            id->identifierRef = node;
            id->inheritOwners(node);

            if (*pz && cpt != name.count)
            {
                cpt++;
                pz++;
            }
        }

        str.buffer = nullptr; // to avoid free on destruction
        return node;
    }

    Utf8 computeGenericParametersReplacement(VectorNative<TypeInfoParam*>& params)
    {
        if (params.empty())
            return "";

        Utf8 result;
        for (int i = 0; i < (int) params.size(); i++)
        {
            auto param = params[i];
            if (param->namedParam == param->typeInfo->name)
                continue;

            if (result.empty())
                result = "with ";
            else
                result += ", ";
            result += param->namedParam;
            result += " = ";
            result += param->typeInfo->name;
        }

        return result;
    }

    Vector<Utf8> computeGenericParametersReplacement(map<Utf8, TypeInfo*>& replace)
    {
        if (!replace.size())
            return {};

        Vector<Utf8> result;
        Utf8         remark;
        for (auto p : replace)
        {
            // Can occur in case of constants (like string for example)
            if (p.first == p.second->getDisplayName())
                continue;

            remark = "with ";
            remark += Fmt("%s = %s", p.first.c_str(), p.second->getDisplayNameC());
            result.push_back(remark);
        }

        return result;
    }

}; // namespace Ast
