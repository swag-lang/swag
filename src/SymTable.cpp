#include "pch.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SymTable.h"
#include "Ast.h"
#include "Module.h"
#include "Naming.h"

void SymTable::release()
{
    for (auto s : allSymbols)
    {
        for (auto over : s->overloads)
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
    auto res = addSymbolTypeInfoNoLock(context, toAdd);
    occupied = false;
    return res;
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(ErrorContext* context, AddSymbolTypeInfo& toAdd)
{
    if (!toAdd.aliasName)
        toAdd.aliasName = &toAdd.node->token.text;

    // Be sure we have a symbol
    auto symbol = findNoLock(*toAdd.aliasName);
    if (!symbol)
        symbol = registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, toAdd.aliasName);

    ScopedLock lock(symbol->mutex);

    // In case an #if block has passed before us
    if (symbol->cptOverloadsInit == 0)
        symbol = registerSymbolNameNoLock(context, toAdd.node, toAdd.kind, toAdd.aliasName);

    // If symbol was registered as a place holder, and is no more, then replace its kind
    if (symbol->kind == SymbolKind::PlaceHolder && toAdd.kind != SymbolKind::PlaceHolder)
    {
        symbol->kind = toAdd.kind;
    }

    // Only add an inline parameter/retval once in a given scope
    else if ((toAdd.flags & (OVERLOAD_VAR_INLINE | OVERLOAD_RETVAL)) && symbol->overloads.size())
    {
        toAdd.node->resolvedSymbolOverload = symbol->overloads[0];
        if (toAdd.resultName)
            *toAdd.resultName = symbol;
        return symbol->overloads[0];
    }

    if (toAdd.resultName)
        *toAdd.resultName = symbol;

    SymbolOverload* result           = nullptr;
    SymbolOverload* resultIncomplete = nullptr;
    if (toAdd.flags & OVERLOAD_STORE_SYMBOLS)
        toAdd.node->resolvedSymbolName = symbol;

    // Remove incomplete flag
    if (symbol->kind == SymbolKind::TypeAlias ||
        symbol->kind == SymbolKind::Variable ||
        symbol->kind == SymbolKind::Struct ||
        symbol->kind == SymbolKind::Interface ||
        symbol->kind == SymbolKind::Function)
    {
        for (auto resolved : symbol->overloads)
        {
            if (resolved->typeInfo == toAdd.typeInfo && (resolved->flags & OVERLOAD_INCOMPLETE))
            {
                resultIncomplete = resolved;
                result           = resolved;
                resultIncomplete->mutexIncomplete.lock();
                resultIncomplete->flags &= ~OVERLOAD_INCOMPLETE;
                break;
            }
        }
    }

    if (!result)
    {
        // No ghosting check for an inline parameter
        if (!(toAdd.flags & OVERLOAD_VAR_INLINE) && !(toAdd.flags & OVERLOAD_RETVAL))
        {
            if (!checkHiddenSymbolNoLock(context, toAdd.node, toAdd.typeInfo, toAdd.kind, symbol, toAdd.flags))
                return nullptr;
        }

        result = symbol->addOverloadNoLock(toAdd.node, toAdd.typeInfo, toAdd.computedValue);
        result->flags |= toAdd.flags;

        // Register for dropping in end of scope, if necessary
        if ((symbol->kind == SymbolKind::Variable) &&
            !(toAdd.flags & OVERLOAD_VAR_FUNC_PARAM) &&
            !(toAdd.flags & OVERLOAD_VAR_GLOBAL) &&
            !(toAdd.flags & OVERLOAD_TUPLE_UNPACK) &&
            !toAdd.computedValue)
        {
            addVarToDrop(result, result->typeInfo, toAdd.storageOffset);
        }
    }

    result->flags |= toAdd.flags;
    if (toAdd.computedValue &&
        toAdd.typeInfo->isPointerToTypeInfo() &&
        result->computedValue.storageOffset != UINT32_MAX &&
        result->computedValue.storageSegment)
    {
    }
    else
    {
        result->computedValue.storageOffset  = toAdd.storageOffset;
        result->computedValue.storageSegment = toAdd.storageSegment;
    }

    if (toAdd.flags & OVERLOAD_STORE_SYMBOLS)
        toAdd.node->resolvedSymbolOverload = result;

    // One less overload. When this reached zero, this means we know every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    if (!(toAdd.flags & OVERLOAD_INCOMPLETE))
        symbol->decreaseOverloadNoLock();

    if (symbol->overloads.size() == symbol->cptOverloadsInit)
    {
        // In case of an incomplete function, we can wakeup jobs too when every overloads have been covered,
        // because an incomplete function doesn't yet know its return type, but we don't need it in order
        // to make a match
        if (symbol->kind == SymbolKind::Function)
            symbol->dependentJobs.setRunning();
        else if (symbol->kind == SymbolKind::Struct)
            symbol->dependentJobs.setRunning();
    }

    if (resultIncomplete)
        resultIncomplete->mutexIncomplete.unlock();

    return result;
}

void SymTable::addVarToDrop(SymbolOverload* overload, TypeInfo* typeInfo, uint32_t storageOffset)
{
    StructToDrop st;
    st.typeStruct = nullptr;

    // A struct
    SWAG_ASSERT(typeInfo);
    if (typeInfo->isStruct())
        st.typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    // An array of structs
    else if (typeInfo->isArray())
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->isStruct())
            st.typeStruct = CastTypeInfo<TypeInfoStruct>(typeArr->finalType, TypeInfoKind::Struct);
    }

    if (!st.typeStruct)
        return;

    st.overload      = overload;
    st.typeInfo      = typeInfo;
    st.storageOffset = storageOffset;
    addVarToDrop(st);
}

void SymTable::addVarToDrop(StructToDrop& st)
{
    if (scope->kind == ScopeKind::Struct)
        return;
    if (st.overload && st.overload->flags & OVERLOAD_RETVAL)
        return;
    SWAG_ASSERT(st.storageOffset != UINT32_MAX);

    // Be sure this is done only once
    for (auto& td : structVarsToDrop)
    {
        if (td.storageOffset == st.storageOffset)
        {
            SWAG_ASSERT(td.typeStruct == st.typeStruct);
            return;
        }
    }

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
        Diagnostic diag{node, *token, Fmt(Err(Err0394), symbol->name.c_str(), Naming::aKindName(symbol->kind).c_str())};
        auto       front = symbol->nodes.front();
        auto       note  = Diagnostic::note(front, front->token, Nte(Nte0036));
        context->report(diag, note);
        return false;
    }

    if (symbol->kind == SymbolKind::Namespace)
        return true;

    // Overloads are not allowed on certain types
    bool canOverload = false;
    if (kind == SymbolKind::Function || kind == SymbolKind::Attribute || kind == SymbolKind::Struct || kind == SymbolKind::Interface)
        canOverload = true;
    if (!canOverload && !symbol->overloads.empty())
    {
        auto       firstOverload = symbol->overloads[0];
        Diagnostic diag{node, *token, Fmt(Err(Err0305), symbol->name.c_str())};
        auto       note = Diagnostic::note(firstOverload->node, firstOverload->node->token, Nte(Nte0036));
        context->report(diag, note);
        return false;
    }

    // A symbol with the same type already exists
    auto overload = symbol->findOverload(typeInfo);
    if (overload)
    {
        // This is fine to define an empty function multiple times, if the signatures are the same
        if (!node->isEmptyFct() &&
            !overload->node->isEmptyFct() &&
            !(node->flags & AST_HAS_SELECT_IF) &&
            !(overload->node->flags & AST_HAS_SELECT_IF))
        {
            auto       firstOverload = overload;
            Diagnostic diag{node, *token, Fmt(Err(Err0305), symbol->name.c_str())};
            auto       note = Diagnostic::note(firstOverload->node, firstOverload->node->token, Nte(Nte0036));
            context->report(diag, note);
            return false;
        }
    }

    return true;
}

bool SymTable::registerUsingAliasOverload(ErrorContext* context, AstNode* node, SymbolName* symbol, SymbolOverload* overload)
{
    ScopedLock lkn(symbol->mutex);

    if (!symbol->overloads.empty())
    {
        if (symbol->kind != SymbolKind::Alias)
        {
            auto       firstOverload = symbol->overloads[0];
            Diagnostic diag{node, Fmt(Err(Err0394), symbol->name.c_str(), Naming::aKindName(symbol->kind).c_str())};
            auto       note = Diagnostic::note(firstOverload->node, Nte(Nte0036));
            context->report(diag, note);
        }
        else
        {
            Diagnostic diag{node, Fmt(Err(Err0890), symbol->name.c_str())};
            context->report(diag);
        }

        return false;
    }

    symbol->overloads.push_back(overload);
    symbol->decreaseOverloadNoLock();
    return true;
}
