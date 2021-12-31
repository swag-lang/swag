#include "pch.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SymTable.h"
#include "Ast.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Mutex.h"

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

void SymTable::removeSymbolName(SymbolName* sym)
{
    ScopedLock lk(mutex);
    mapNames.remove(sym);
}

SymbolName* SymTable::registerSymbolName(JobContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName)
{
    ScopedLock lk(mutex);
    return registerSymbolNameNoLock(context, node, kind, aliasName);
}

SymbolName* SymTable::registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName)
{
    if (!aliasName)
        aliasName = &node->token.text;

    SWAG_ASSERT(!aliasName->empty());
    bool wasPlaceHolder = false;
    auto symbol         = findNoLock(*aliasName);
    if (!symbol)
    {
        symbol = g_Allocator.alloc<SymbolName>();
        if (g_CommandLine->stats)
            g_Stats.memSymName += Allocator::alignSize(sizeof(SymbolName));
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
    if (node->ownerCompilerIfBlock)
    {
        symbol->cptIfBlock++;
        node->ownerCompilerIfBlock->addSymbol(node, symbol);
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

SymbolOverload* SymTable::addSymbolTypeInfo(JobContext*    context,
                                            AstNode*       node,
                                            TypeInfo*      typeInfo,
                                            SymbolKind     kind,
                                            ComputedValue* computedValue,
                                            uint32_t       flags,
                                            SymbolName**   resultName,
                                            uint32_t       storageOffset,
                                            DataSegment*   storageSegment,
                                            Utf8*          aliasName)
{
    ScopedLock lk(mutex);
    return addSymbolTypeInfoNoLock(context, node, typeInfo, kind, computedValue, flags, resultName, storageOffset, storageSegment, aliasName);
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(JobContext*    context,
                                                  AstNode*       node,
                                                  TypeInfo*      typeInfo,
                                                  SymbolKind     kind,
                                                  ComputedValue* computedValue,
                                                  uint32_t       flags,
                                                  SymbolName**   resultName,
                                                  uint32_t       storageOffset,
                                                  DataSegment*   storageSegment,
                                                  Utf8*          aliasName)
{
    if (!aliasName)
        aliasName = &node->token.text;

    auto symbol = findNoLock(*aliasName);

    // Be sure we have a symbol
    if (!symbol)
        symbol = registerSymbolNameNoLock(context, node, kind, aliasName);
    else if (symbol->kind == SymbolKind::PlaceHolder && kind != SymbolKind::PlaceHolder)
        symbol->kind = kind;
    // Only add an inline parameter/retval once in a given scope
    else if ((flags & (OVERLOAD_VAR_INLINE | OVERLOAD_RETVAL)) && symbol->overloads.size())
    {
        node->resolvedSymbolOverload = symbol->overloads[0];
        if (resultName)
            *resultName = symbol;
        return symbol->overloads[0];
    }

    if (resultName)
        *resultName = symbol;

    ScopedLock      lock(symbol->mutex);
    SymbolOverload* result           = nullptr;
    SymbolOverload* resultIncomplete = nullptr;
    if (flags & OVERLOAD_STORE_SYMBOLS)
        node->resolvedSymbolName = symbol;

    // Remove incomplete flag
    if (symbol->kind == SymbolKind::TypeAlias ||
        symbol->kind == SymbolKind::Variable ||
        symbol->kind == SymbolKind::Struct ||
        symbol->kind == SymbolKind::Interface ||
        symbol->kind == SymbolKind::Function)
    {
        for (auto resolved : symbol->overloads)
        {
            if (resolved->typeInfo == typeInfo && (resolved->flags & OVERLOAD_INCOMPLETE))
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
        if (!(flags & OVERLOAD_VAR_INLINE) && !(flags & OVERLOAD_RETVAL))
        {
            if (!checkHiddenSymbolNoLock(context, node, typeInfo, kind, symbol))
                return nullptr;
        }

        result = symbol->addOverloadNoLock(node, typeInfo, computedValue);
        result->flags |= flags;

        // Register for dropping in end of scope, if necessary
        if ((symbol->kind == SymbolKind::Variable) && !(flags & OVERLOAD_VAR_FUNC_PARAM) && !(flags & OVERLOAD_TUPLE_UNPACK) && !computedValue)
            addVarToDrop(result, result->typeInfo, storageOffset);
    }

    result->flags |= flags;
    if (computedValue &&
        typeInfo->isPointerToTypeInfo() &&
        result->computedValue.storageOffset != UINT32_MAX &&
        result->computedValue.storageSegment)
    {
    }
    else
    {
        result->computedValue.storageOffset  = storageOffset;
        result->computedValue.storageSegment = storageSegment;
    }

    if (flags & OVERLOAD_STORE_SYMBOLS)
        node->resolvedSymbolOverload = result;

    // One less overload. When this reached zero, this means we know every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    if (!(flags & OVERLOAD_INCOMPLETE))
        decreaseOverloadNoLock(symbol);

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
    if (typeInfo->kind == TypeInfoKind::Struct)
        st.typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

    // An array of structs
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->kind == TypeInfoKind::Struct)
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

void SymTable::decreaseOverloadNoLock(SymbolName* symbol)
{
    SWAG_ASSERT(symbol->cptOverloads);
    symbol->cptOverloads--;
    if (symbol->cptOverloads == 0)
        symbol->dependentJobs.setRunning();
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
}

bool SymTable::checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind type)
{
    SharedLock lk(mutex);
    return checkHiddenSymbolNoLock(context, node, typeInfo, type, nullptr, true);
}

bool SymTable::acceptGhostSymbolNoLock(JobContext* context, AstNode* node, SymbolKind kind, SymbolName* symbol)
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

bool SymTable::checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName)
{
    auto& token = node->token;
    auto& name  = node->token.text;

    if (!symbol)
        symbol = findNoLock(name);
    if (!symbol)
        return true;
    if (acceptGhostSymbolNoLock(context, node, kind, symbol))
        return true;

    // A symbol with a different kind already exists
    if (symbol->kind != kind)
    {
        Utf8       msg = Utf8::format(g_E[Err0885], symbol->name.c_str(), SymTable::getArticleKindName(symbol->kind));
        Diagnostic diag{node, token, msg};
        Utf8       note = g_E[Nte0036];
        Diagnostic diagNote{symbol->nodes.front(), note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
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
        auto firstOverload = symbol->overloads[0];

        // Special case for function parameters. We want to let the symbol resolution to deal with ambiguities if we can
        // because the user has some ways (#param) to solve this.
        // That way, a function parameter can have the same name as something else (like a field in a struct)
        if (symbol->kind == SymbolKind::Variable &&
            node->parent &&
            node->parent->kind == AstNodeKind::FuncDeclParams &&
            this != &node->ownerScope->symTable &&
            !(firstOverload->flags & OVERLOAD_VAR_FUNC_PARAM))
        {
            return true;
        }

        Utf8       msg = Utf8::format(g_E[Err0886], symbol->name.c_str());
        Diagnostic diag{node, token, msg};
        Utf8       note = g_E[Nte0036];
        Diagnostic diagNote{firstOverload->node, note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (!canOverload && checkSameName)
    {
        Utf8       msg = Utf8::format(g_E[Err0886], symbol->name.c_str());
        Diagnostic diag{node, token, msg};
        Utf8       note = g_E[Nte0036];
        Diagnostic diagNote{symbol->nodes.front(), note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    // A symbol with the same type already exists
    auto overload = symbol->findOverload(typeInfo);
    if (overload)
    {
        // This is fine to define an empty function multiple times, if the signatures are the same
        if (!(node->flags & AST_EMPTY_FCT) &&
            !(overload->node->flags & AST_EMPTY_FCT) &&
            !(node->flags & AST_HAS_SELECT_IF) &&
            !(overload->node->flags & AST_HAS_SELECT_IF))
        {
            auto       firstOverload = overload;
            Utf8       msg           = Utf8::format(g_E[Err0886], symbol->name.c_str());
            Diagnostic diag{node, token, msg};
            Utf8       note = g_E[Nte0036];
            Diagnostic diagNote{firstOverload->node, note, DiagnosticLevel::Note};
            if (typeInfo->kind == TypeInfoKind::FuncAttr)
                diagNote.remarks.push_back(Ast::computeGenericParametersReplacement(((TypeInfoFuncAttr*) typeInfo)->genericParameters));
            context->report(diag, &diagNote);
            return false;
        }
    }

    return true;
}

SymbolOverload* SymbolName::addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue)
{
    auto overload      = g_Allocator.alloc<SymbolOverload>();
    overload->typeInfo = typeInfo;
    overload->node     = node;
    if (g_CommandLine->stats)
        g_Stats.memSymOver += sizeof(SymbolOverload);

    if (computedValue)
    {
        overload->computedValue = *computedValue;
        overload->flags |= OVERLOAD_COMPUTED_VALUE;
    }

    overload->symbol = this;
    overloads.push_back(overload);
    return overload;
}

bool SymTable::registerUsingAliasOverload(JobContext* context, AstNode* node, SymbolName* symbol, SymbolOverload* overload)
{
    ScopedLock lkn(symbol->mutex);

    if (!symbol->overloads.empty())
    {
        if (symbol->kind != SymbolKind::Alias)
        {
            auto       firstOverload = symbol->overloads[0];
            Utf8       msg           = Utf8::format(g_E[Err0885], symbol->name.c_str(), SymTable::getArticleKindName(symbol->kind));
            Diagnostic diag{node, msg};
            Utf8       note = g_E[Nte0036];
            Diagnostic diagNote{firstOverload->node, note, DiagnosticLevel::Note};
            context->report(diag, &diagNote);
        }
        else
        {
            Utf8       msg = Utf8::format(g_E[Err0890], symbol->name.c_str());
            Diagnostic diag{node, msg};
            context->report(diag);
        }

        return false;
    }

    symbol->overloads.push_back(overload);
    decreaseOverloadNoLock(symbol);
    return true;
}

const char* SymTable::getArticleKindName(SymbolKind kind)
{
    switch (kind)
    {
    case SymbolKind::Attribute:
        return "an attribute";
    case SymbolKind::Enum:
        return "an enum";
    case SymbolKind::EnumValue:
        return "an enum value";
    case SymbolKind::Function:
        return "a function";
    case SymbolKind::Namespace:
        return "a namespace";
    case SymbolKind::TypeAlias:
        return "a type alias";
    case SymbolKind::Alias:
        return "a name alias";
    case SymbolKind::Variable:
        return "a variable";
    case SymbolKind::Struct:
        return "a struct";
    case SymbolKind::Interface:
        return "an interface";
    case SymbolKind::GenericType:
        return "a generic type";
    case SymbolKind::PlaceHolder:
        return "a placeholder";
    }

    return "<symbol>";
}

const char* SymTable::getNakedKindName(SymbolKind kind)
{
    switch (kind)
    {
    case SymbolKind::Attribute:
        return "attribute";
    case SymbolKind::Enum:
        return "enum";
    case SymbolKind::EnumValue:
        return "enum value";
    case SymbolKind::Function:
        return "function";
    case SymbolKind::Namespace:
        return "namespace";
    case SymbolKind::TypeAlias:
        return "type alias";
    case SymbolKind::Alias:
        return "name alias";
    case SymbolKind::Variable:
        return "variable";
    case SymbolKind::Struct:
        return "struct";
    case SymbolKind::Interface:
        return "interface";
    case SymbolKind::GenericType:
        return "generic type";
    case SymbolKind::PlaceHolder:
        return "placeholder";
    }

    return "<symbol>";
}

const char* SymTable::getNakedKindName(SymbolOverload* overload)
{
    if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        return "constant";
    return getNakedKindName(overload->symbol->kind);
}

SymbolOverload* SymbolName::findOverload(TypeInfo* typeInfo)
{
    for (auto it : overloads)
    {
        if (it->typeInfo == typeInfo)
            return it;
        if (it->typeInfo->isSame(typeInfo, ISSAME_EXACT))
            return it;
    }

    return nullptr;
}

void SymbolName::addDependentJob(Job* job)
{
    ScopedLock lk(mutex);
    dependentJobs.add(job);
}

void SymbolName::addDependentJobNoLock(Job* job)
{
    dependentJobs.add(job);
}

Utf8 SymbolName::getFullName()
{
    Utf8 fullName;
    Scope::makeFullName(fullName, ownerTable->scope->getFullName(), name);
    return fullName;
}

void SymbolName::unregisterNode(AstNode* node)
{
    // Unregister the node in the symbol
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] == node)
        {
            nodes.erase_unordered(i);
            break;
        }
    }
}

SymbolName* SymTableHash::find(const Utf8& str, uint32_t crc)
{
    if (!allocated)
        return nullptr;

    if (!crc)
        crc = str.hash();

    uint32_t idx = crc % allocated;
    while (buffer[idx].hash)
    {
        if (buffer[idx].hash == crc && buffer[idx].symbolName->name == str)
            return buffer[idx].symbolName;
        idx = (idx + 1) % allocated;
    }

    return nullptr;
}

void SymTableHash::remove(SymbolName* data)
{
    auto crc = data->name.hash();

    uint32_t idx = crc % allocated;
    while (buffer[idx].hash)
    {
        if (buffer[idx].hash == crc && buffer[idx].symbolName == data)
        {
            buffer[idx].hash       = 0;
            buffer[idx].symbolName = nullptr;
            return;
        }

        idx = (idx + 1) % allocated;
    }
}

void SymTableHash::addElem(SymbolName* data, uint32_t crc)
{
    if (!crc)
        crc = data->name.hash();

    // Find a free slot
    uint32_t idx = crc % allocated;
    while (buffer[idx].hash)
    {
        idx = (idx + 1) % allocated;
    }

    buffer[idx].hash       = crc;
    buffer[idx].symbolName = data;
    count += 1;
}

void SymTableHash::add(SymbolName* data)
{
    // First allocation
    if (!allocated)
    {
        count     = 0;
        allocated = 16;
        buffer    = (Entry*) g_Allocator.alloc(allocated * sizeof(Entry));
        memset(buffer, 0, allocated * sizeof(Entry));
        if (g_CommandLine->stats)
            g_Stats.memSymTable += allocated * sizeof(Entry);
    }

    // Need to grow the hash table, and add back the old values
    else if (count >= allocated / 2)
    {
        auto oldAllocated = allocated;
        auto oldBuffer    = buffer;
        auto oldCount     = count;

        allocated *= 3;
        buffer = (Entry*) g_Allocator.alloc(allocated * sizeof(Entry));
        memset(buffer, 0, allocated * sizeof(Entry));

        count = 0;
        for (uint32_t i = 0; i < oldAllocated; i++)
        {
            if (oldBuffer[i].hash)
            {
                oldCount--;
                addElem(oldBuffer[i].symbolName, oldBuffer[i].hash);
                if (!oldCount)
                    break;
            }
        }

        g_Allocator.free(oldBuffer, oldAllocated * sizeof(Entry));

        if (g_CommandLine->stats)
        {
            g_Stats.memSymTable -= oldAllocated * sizeof(Entry);
            g_Stats.memSymTable += allocated * sizeof(Entry);
        }
    }

    // Find a free slot
    addElem(data);
}