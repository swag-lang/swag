#include "pch.h"
#include "SymTable.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Module.h"
#include "Scope.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

void SymTable::release()
{
    for (auto s : allSymbols)
    {
        for (auto over :
             s->overloads)
            Allocator::free<SymbolOverload>(over);
        Allocator::free<SymbolName>(s);
    }

    allSymbols.clear();
}

SymbolName* SymTable::find(const Utf8& name, uint32_t crc)
{
    SharedLock lk(mutex);
    return findNoLock(name, crc);
}

SymbolName* SymTable::findNoLock(const Utf8& name, uint32_t crc)
{
    auto symbol = mapNames.find(name, crc);
    if (symbol && symbol->cptOverloadsInit == 0)
        return nullptr;
    return symbol;
}

SymbolName* SymTable::registerSymbolName(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName)
{
    ScopedLock lk(mutex);
    occupied    = true;
    auto result = registerSymbolNameNoLock(context, node, kind, aliasName);
    occupied    = false;
    return result;
}

SymbolName* SymTable::registerSymbolNameNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName)
{
    if (!aliasName)
        aliasName = &node->token.text;

    SWAG_ASSERT(!aliasName->empty());
    bool wasPlaceHolder = false;
    auto symbol         = findNoLock(*aliasName);
    if (!symbol)
    {
        symbol = Allocator::alloc<SymbolName>();
        allSymbols.push_back(symbol);
#ifdef SWAG_STATS
        g_Stats.memSymName += Allocator::alignSize(sizeof(SymbolName));
#endif
        symbol->name       = *aliasName;
        symbol->kind       = kind;
        symbol->ownerTable = this;
        mapNames.add(symbol);
    }
    else if (symbol->kind == SymbolKind::PlaceHolder)
    {
        wasPlaceHolder = true;
        symbol->kind   = kind;
    }

    symbol->nodes.push_back(node);

    // Register the symbol in the corresponding #if block, in case the block is disabled
    if (node->hasExtOwner() && node->extOwner()->ownerCompilerIfBlock)
    {
        symbol->cptIfBlock++;
        node->extOwner()->ownerCompilerIfBlock->addSymbol(node, symbol);
    }

    if (!wasPlaceHolder)
    {
        ScopedLock lock(symbol->mutex);
        if (kind == SymbolKind::Function || kind == SymbolKind::Attribute || symbol->cptOverloads == 0)
        {
            symbol->cptOverloads++;
            symbol->cptOverloadsInit++;
        }
    }

    return symbol;
}

SymbolOverload* SymTable::addSymbolTypeInfo(ErrorContext* context, AddSymbolTypeInfo& toAdd)
{
    ScopedLock lk(mutex);
    occupied = true;

    // Be sure we have a symbol
    auto symName     = toAdd.aliasName ? toAdd.aliasName : &toAdd.node->token.text;
    toAdd.symbolName = findNoLock(*symName);
    if (!toAdd.symbolName)
        toAdd.symbolName = registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, symName);

    auto res = addSymbolTypeInfoNoLock(context, toAdd);
    occupied = false;
    return res;
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(ErrorContext* context, AddSymbolTypeInfo& toAdd)
{
    auto symbol = toAdd.symbolName;
    SWAG_ASSERT(symbol);

    ScopedLock lock(symbol->mutex);

    // In case an #if block has passed before us
    if (symbol->cptOverloadsInit == 0)
    {
        auto symName     = toAdd.aliasName ? toAdd.aliasName : &toAdd.node->token.text;
        symbol           = registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, symName);
        toAdd.symbolName = symbol;
    }

    // Only add an inline parameter/retval once in a given scope
    if ((toAdd.flags & (OVERLOAD_VAR_INLINE | OVERLOAD_RETVAL)) && symbol->overloads.size())
    {
        toAdd.node->resolvedSymbolOverload = symbol->overloads[0];
        return symbol->overloads[0];
    }

    // If symbol was registered as a place holder, and is no more, then replace its kind
    if (symbol->kind == SymbolKind::PlaceHolder && toAdd.kind != SymbolKind::PlaceHolder)
        symbol->kind = toAdd.kind;

    SymbolOverload* overload = nullptr;

    // Remove incomplete flag
    if (symbol->kind == SymbolKind::TypeAlias ||
        symbol->kind == SymbolKind::Variable ||
        symbol->kind == SymbolKind::Struct ||
        symbol->kind == SymbolKind::Interface ||
        symbol->kind == SymbolKind::Function)
    {
        for (auto resolved : symbol->overloads)
        {
            if (!(toAdd.flags & OVERLOAD_INCOMPLETE))
            {
                if ((resolved->flags & OVERLOAD_INCOMPLETE) && resolved->typeInfo == toAdd.typeInfo)
                {
                    overload = resolved;
                    overload->flags &= ~OVERLOAD_INCOMPLETE;
                    break;
                }
            }
            else
            {
                if ((resolved->flags & OVERLOAD_UNDEFINED) && resolved->typeInfo->isSame(toAdd.typeInfo, CASTFLAG_CAST))
                {
                    overload = resolved;
                    overload->flags &= ~OVERLOAD_UNDEFINED;
                    break;
                }
            }
        }
    }

    if (!overload)
    {
        // No ghosting check for an inline parameter
        if (!(toAdd.flags & (OVERLOAD_VAR_INLINE | OVERLOAD_RETVAL)))
        {
            if (!checkHiddenSymbolNoLock(context, toAdd.node, toAdd.typeInfo, toAdd.kind, symbol, toAdd.flags))
                return nullptr;
        }

        overload = symbol->addOverloadNoLock(toAdd.node, toAdd.typeInfo, toAdd.computedValue);
        overload->flags |= toAdd.flags;

        // Register for dropping in end of scope, if necessary
        if ((symbol->kind == SymbolKind::Variable) &&
            !(toAdd.flags & (OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_VAR_GLOBAL | OVERLOAD_TUPLE_UNPACK)) &&
            !toAdd.computedValue)
        {
            addVarToDrop(overload, overload->typeInfo, toAdd.storageOffset);
        }
    }
    else
    {
        overload->flags |= toAdd.flags;
    }

    if (!toAdd.computedValue ||
        !toAdd.typeInfo->isPointerToTypeInfo() ||
        overload->computedValue.storageOffset == UINT32_MAX ||
        !overload->computedValue.storageSegment)
    {
        overload->computedValue.storageOffset  = toAdd.storageOffset;
        overload->computedValue.storageSegment = toAdd.storageSegment;
    }

    // One less overload. When this reached zero, this means we know every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    if (!(toAdd.flags & OVERLOAD_INCOMPLETE) && !(toAdd.flags & OVERLOAD_UNDEFINED))
    {
        // For function, will be done later because we register the full
        // symbol with the function type, and we want to compute the access
        // after the complete semantic pass of the content (for exported inline)
        if (symbol->kind != SymbolKind::Function)
            Semantic::computeAccess(overload->node);
        symbol->decreaseOverloadNoLock();
    }

    // In case of an incomplete function, we can wakeup jobs too when every overloads have been covered,
    // because an incomplete function doesn't yet know its return type, but we don't need it in order
    // to make a match
    if ((symbol->overloads.size() == symbol->cptOverloadsInit) &&
        (symbol->kind == SymbolKind::Function || symbol->kind == SymbolKind::Struct))
    {
        symbol->dependentJobs.setRunning();
    }

    return overload;
}

void SymTable::addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset)
{
    if (scope->kind == ScopeKind::Struct)
        return;
    if (overload && overload->flags & OVERLOAD_RETVAL)
        return;

    StructToDrop st;
    SWAG_ASSERT(typeInfo);
    typeInfo = typeInfo->getConcreteAlias();

    // A struct
    if (typeInfo->isStruct())
    {
        st.typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    }

    // An array of structs
    else if (typeInfo->isArrayOfStruct())
    {
        auto typeArr  = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        st.typeStruct = CastTypeInfo<TypeInfoStruct>(typeArr->finalType->getConcreteAlias(), TypeInfoKind::Struct);
    }
    else
    {
        return;
    }

    // Be sure this is done only once
    for (auto& td : structVarsToDrop)
    {
        if (td.storageOffset == storageOffset)
        {
            SWAG_ASSERT(td.typeStruct == st.typeStruct);
            return;
        }
    }

    SWAG_ASSERT(storageOffset != UINT32_MAX);
    st.overload      = overload;
    st.typeInfo      = typeInfo;
    st.storageOffset = storageOffset;
    structVarsToDrop.push_back(st);
}

void SymTable::disabledIfBlockOverloadNoLock(AstNode* node, SymbolName* symbol)
{
    SWAG_ASSERT(symbol->cptIfBlock);
    symbol->cptIfBlock--;
    symbol->cptOverloadsInit = min(symbol->cptIfBlock, symbol->cptOverloadsInit);
    symbol->cptOverloads     = min(symbol->cptOverloads, symbol->cptOverloadsInit);
    symbol->unregisterNode(node);
    if (symbol->cptOverloads == 0)
        symbol->dependentJobs.setRunning();

    // Some nodes will share their created scope
    // We need to reset it in order to avoid a double free
    switch (node->kind)
    {
    case AstNodeKind::EnumDecl:
    {
        auto enumNode   = CastAst<AstEnum>(node, AstNodeKind::EnumDecl);
        enumNode->scope = nullptr;
        break;
    }
    case AstNodeKind::StructDecl:
    {
        auto structNode   = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
        structNode->scope = nullptr;
        break;
    }
    default:
        break;
    }
}

bool SymTable::acceptGhostSymbolNoLock(ErrorContext* context, AstNode* node, SymbolKind kind, SymbolName* symbol)
{
    // A symbol with a different kind already exists
    if (symbol->kind != kind)
    {
        // If they are not in the same scope, then this is fine for now.
        // The identifier resolution will deal with ambiguities later, if necessary
        if (node->ownerScope && &node->ownerScope->symTable != this)
        {
            // We do not accept, right away, a conflict with a namespace
            if (kind != SymbolKind::Namespace && symbol->kind != SymbolKind::Namespace)
                return true;
            // Except if the namespace name is the name of the current module
            if (symbol->kind != SymbolKind::Namespace && node->sourceFile->module->name == symbol->name)
                return true;
            if (kind != SymbolKind::Namespace && node->sourceFile->module->name == node->token.text)
                return true;
        }
    }

    return false;
}

bool SymTable::checkHiddenSymbolNoLock(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, uint32_t overFlags)
{
    auto token = &node->token;
    if (node->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        token         = &funcNode->tokenName;
    }

    if (!symbol)
        symbol = findNoLock(token->text);
    if (!symbol)
        return true;
    if (acceptGhostSymbolNoLock(context, node, kind, symbol))
        return true;

    // A symbol with a different kind already exists
    if (symbol->kind != kind)
    {
        auto front = symbol->nodes.front();
        return SemanticError::duplicatedSymbolError(context, node->sourceFile, *token, kind, symbol->name, symbol->kind, front);
    }

    if (symbol->kind == SymbolKind::Namespace)
        return true;

    // Overloads are not allowed on certain types
    bool canOverload = false;
    if (kind == SymbolKind::Function || kind == SymbolKind::Attribute || kind == SymbolKind::Struct || kind == SymbolKind::Interface)
        canOverload = true;
    if (!canOverload && !symbol->overloads.empty())
    {
        auto firstOverload = symbol->overloads[0];
        return SemanticError::duplicatedSymbolError(context, node->sourceFile, *token, symbol->kind, symbol->name, firstOverload->symbol->kind, firstOverload->node);
    }

    // A symbol with the same type already exists
    auto overload = symbol->findOverload(typeInfo);
    if (overload)
    {
        // This is fine to define an empty function multiple times, if the signatures are the same
        if (!node->isEmptyFct() &&
            !overload->node->isEmptyFct() &&
            !(overload->flags & OVERLOAD_UNDEFINED) &&
            !(node->flags & AST_HAS_SELECT_IF) &&
            !(overload->node->flags & AST_HAS_SELECT_IF))
        {
            auto firstOverload = overload;
            return SemanticError::duplicatedSymbolError(context, node->sourceFile, *token, symbol->kind, symbol->name, firstOverload->symbol->kind, firstOverload->node);
        }
    }

    return true;
}

bool SymTable::registerNameAlias(ErrorContext* context, AstNode* node, SymbolName* symbol, SymbolName* otherSymbol, SymbolOverload* otherOverload)
{
    ScopedLock lkn(symbol->mutex);
    SharedLock lkn1(otherSymbol->mutex);

    if (!symbol->overloads.empty())
    {
        auto firstOverload = symbol->overloads[0];
        for (auto over : symbol->overloads)
        {
            if (over->symbol->name == symbol->name)
            {
                firstOverload = over;
                break;
            }
        }

        return SemanticError::duplicatedSymbolError(context, node->sourceFile, node->token, SymbolKind::NameAlias, symbol->name, symbol->kind, firstOverload->node);
    }

    SWAG_ASSERT(!otherSymbol->cptOverloads);

    if (otherOverload)
    {
        symbol->cptOverloadsInit += 1;
        auto copy = Allocator::alloc<SymbolOverload>();
        copy->from(otherOverload);
        otherOverload->flags |= OVERLOAD_HAS_AFFECT;
        symbol->overloads.push_back(copy);
    }
    else
    {
        symbol->cptOverloadsInit += otherSymbol->cptOverloadsInit;
        for (auto o : otherSymbol->overloads)
        {
            auto copy = Allocator::alloc<SymbolOverload>();
            copy->from(o);
            o->flags |= OVERLOAD_HAS_AFFECT;
            symbol->overloads.push_back(copy);
        }
    }

    symbol->decreaseOverloadNoLock();
    return true;
}
