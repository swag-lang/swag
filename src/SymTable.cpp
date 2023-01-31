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
    occupied = true;
    mapNames.remove(sym);
    occupied = false;
}

SymbolName* SymTable::registerSymbolName(JobContext* context, AstNode* node, SymbolKind kind, Utf8* aliasName)
{
    ScopedLock lk(mutex);
    occupied    = true;
    auto result = registerSymbolNameNoLock(context, node, kind, aliasName);
    occupied    = false;
    return result;
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
        if (g_CommandLine.stats)
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
    occupied = true;
    auto res = addSymbolTypeInfoNoLock(context, node, typeInfo, kind, computedValue, flags, resultName, storageOffset, storageSegment, aliasName);
    occupied = false;
    return res;
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

    // Be sure we have a symbol
    auto symbol = findNoLock(*aliasName);
    if (!symbol)
        symbol = registerSymbolNameNoLock(context, node, kind, aliasName);

    ScopedLock lock(symbol->mutex);

    // In case an #if block has passed before us
    if (symbol->cptOverloadsInit == 0)
        symbol = registerSymbolNameNoLock(context, node, kind, aliasName);

    // If symbol was registered as a place holder, and is no more, then replace its kind
    if (symbol->kind == SymbolKind::PlaceHolder && kind != SymbolKind::PlaceHolder)
    {
        symbol->kind = kind;
    }

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
            if (!checkHiddenSymbolNoLock(context, node, typeInfo, kind, symbol, flags))
                return nullptr;
        }

        result = symbol->addOverloadNoLock(node, typeInfo, computedValue);
        result->flags |= flags;

        // Register for dropping in end of scope, if necessary
        if ((symbol->kind == SymbolKind::Variable) &&
            !(flags & OVERLOAD_VAR_FUNC_PARAM) &&
            !(flags & OVERLOAD_VAR_GLOBAL) &&
            !(flags & OVERLOAD_TUPLE_UNPACK) &&
            !computedValue)
        {
            addVarToDrop(result, result->typeInfo, storageOffset);
        }
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

bool SymTable::checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, uint32_t overFlags)
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
        Diagnostic diag{node, *token, Fmt(Err(Err0394), symbol->name.c_str(), getArticleKindName(symbol->kind).c_str())};
        auto       front = symbol->nodes.front();
        Diagnostic diagNote{front, front->token, Nte(Nte0036), DiagnosticLevel::Note};
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
        auto       firstOverload = symbol->overloads[0];
        Diagnostic diag{node, *token, Fmt(Err(Err0305), symbol->name.c_str())};
        Diagnostic diagNote{firstOverload->node, firstOverload->node->token, Nte(Nte0036), DiagnosticLevel::Note};
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
            Diagnostic diag{node, *token, Fmt(Err(Err0305), symbol->name.c_str())};
            Diagnostic diagNote{firstOverload->node, firstOverload->node->token, Nte(Nte0036), DiagnosticLevel::Note};
            if (typeInfo->isFuncAttr())
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
    if (g_CommandLine.stats)
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
            Utf8       msg           = Fmt(Err(Err0394), symbol->name.c_str(), getArticleKindName(symbol->kind).c_str());
            Diagnostic diag{node, msg};
            Utf8       note = Nte(Nte0036);
            Diagnostic diagNote{firstOverload->node, note, DiagnosticLevel::Note};
            context->report(diag, &diagNote);
        }
        else
        {
            Utf8       msg = Fmt(Err(Err0890), symbol->name.c_str());
            Diagnostic diag{node, msg};
            context->report(diag);
        }

        return false;
    }

    symbol->overloads.push_back(overload);
    decreaseOverloadNoLock(symbol);
    return true;
}

Utf8 SymTable::getKindName(SymbolName* symbol, AstNode* node, TypeInfo* typeInfo, uint32_t overFlags, Utf8& article)
{
    if (typeInfo->isTuple())
    {
        article = "a";
        return "tuple";
    }

    if (typeInfo->isLambda())
    {
        article = "a";
        return "lambda";
    }

    if (typeInfo->isClosure())
    {
        article = "a";
        return "closure";
    }

    if (node->isGeneratedSelf())
    {
        article = "an";
        return "implicit parameter";
    }

    if (overFlags & OVERLOAD_VAR_CAPTURE)
    {
        article = "a";
        return "captured parameter";
    }

    if (overFlags & OVERLOAD_VAR_FUNC_PARAM && node->ownerFct && node->ownerFct->typeInfo->isLambda())
    {
        article = "a";
        return "lambda parameter";
    }

    if (overFlags & OVERLOAD_VAR_FUNC_PARAM && node->ownerFct && node->ownerFct->typeInfo->isClosure())
    {
        article = "a";
        return "closure parameter";
    }

    if (overFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        article = "a";
        return "function parameter";
    }

    if (overFlags & OVERLOAD_VAR_INLINE)
    {
        article = "a";
        return "function parameter";
    }

    if (overFlags & OVERLOAD_VAR_GLOBAL)
    {
        article = "a";
        return "global variable";
    }

    if (overFlags & OVERLOAD_VAR_LOCAL)
    {
        article = "a";
        return "local variable";
    }

    if (overFlags & OVERLOAD_VAR_STRUCT)
    {
        article = "a";
        return "field";
    }

    if (overFlags & OVERLOAD_CONSTANT)
    {
        article = "a";
        return "constant";
    }

    if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_MACRO)
    {
        article = "a";
        return "macro";
    }

    if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_MIXIN)
    {
        article = "a";
        return "mixin";
    }

    if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_INLINE)
    {
        article = "an";
        return "inlined function";
    }

    return getArticleKindName(symbol->kind);
}

Utf8 SymTable::getNakedKindName(SymbolOverload* overload)
{
    Utf8 article;
    return getKindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
}

Utf8 SymTable::getArticleKindName(SymbolOverload* overload)
{
    Utf8 article;
    auto result = getKindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
    article += " ";
    article += result;
    return article;
}

Utf8 SymTable::getArticleKindName(SymbolKind kind)
{
    Utf8 article;
    auto result = getKindName(kind, article);
    article += " ";
    article += result;
    return article;
}

Utf8 SymTable::getNakedKindName(SymbolKind kind)
{
    Utf8 article;
    return getKindName(kind, article);
}

Utf8 SymTable::getKindName(SymbolKind kind, Utf8& article)
{
    switch (kind)
    {
    case SymbolKind::Attribute:
        article = "an";
        return "attribute";
    case SymbolKind::Enum:
        article = "an";
        return "enum";
    case SymbolKind::EnumValue:
        article = "an";
        return "enum value";
    case SymbolKind::Function:
        article = "a";
        return "function";
    case SymbolKind::Namespace:
        article = "a";
        return "namespace";
    case SymbolKind::TypeAlias:
        article = "a";
        return "type alias";
    case SymbolKind::Alias:
        article = "a";
        return "name alias";
    case SymbolKind::Variable:
        article = "a";
        return "variable";
    case SymbolKind::Struct:
        article = "a";
        return "struct";
    case SymbolKind::Interface:
        article = "an";
        return "interface";
    case SymbolKind::GenericType:
        article = "a";
        return "generic type";
    case SymbolKind::PlaceHolder:
        article = "a";
        return "placeholder";
    }

    article = "a";
    return "symbol";
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

void SymTableHash::clone(SymTableHash* from)
{
    if (!from->allocated)
        return;
    fastReject = from->fastReject;
    allocated  = from->allocated;
    count      = from->count;
    buffer     = (Entry*) g_Allocator.alloc(from->allocated * sizeof(Entry));
    memcpy(buffer, from->buffer, from->allocated * sizeof(Entry));
}

SymbolName* SymTableHash::find(const Utf8& str, uint32_t crc)
{
    if (!allocated)
        return nullptr;

    uint32_t a = (str[0] | 0x20) - 'a';
    if (a < 32 && !(fastReject & (1 << a)))
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
    uint32_t a = (data->name[0] | 0x20) - 'a';
    if (a < 32)
        fastReject |= 1 << a;

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
        if (g_CommandLine.stats)
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

        if (g_CommandLine.stats)
        {
            g_Stats.memSymTable -= oldAllocated * sizeof(Entry);
            g_Stats.memSymTable += allocated * sizeof(Entry);
        }
    }

    // Find a free slot
    addElem(data);
}