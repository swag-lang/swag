#include "pch.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "Allocator.h"
#include "Ast.h"

SymbolName* SymTable::find(const Utf8Crc& name)
{
    shared_lock lk(mutex);
    return findNoLock(name);
}

SymbolName* SymTable::findNoLock(const Utf8Crc& name)
{
    return mapNames.find(name);
}

SymbolName* SymTable::registerSymbolName(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName)
{
    unique_lock lk(mutex);
    return registerSymbolNameNoLock(context, node, kind, aliasName);
}

SymbolName* SymTable::registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName)
{
    SWAG_RACE_CONDITION_WRITE(raceCondition);

    if (!aliasName)
        aliasName = &node->name;

    SWAG_ASSERT(!aliasName->empty());
    bool wasPlaceHolder = false;
    auto symbol         = findNoLock(*aliasName);
    if (!symbol)
    {
        symbol = g_Allocator.alloc<SymbolName>();
        if (g_CommandLine.stats)
            g_Stats.memSymName += sizeof(SymbolName);
        symbol->name                 = *aliasName;
        symbol->kind                 = kind;
        symbol->defaultOverload.node = node;
        symbol->ownerTable           = this;
        mapNames.add(symbol);
    }
    else if (symbol->kind == SymbolKind::PlaceHolder)
    {
        wasPlaceHolder = true;
        symbol->kind   = kind;
    }

    symbol->nodes.push_back(node);

    if (!wasPlaceHolder)
    {
        unique_lock lock(symbol->mutex);
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
                                            Utf8Crc*       aliasName)
{
    unique_lock lk(mutex);
    if (node->attributeFlags & ATTRIBUTE_PUBLIC || context->sourceFile->generated)
        flags |= OVERLOAD_PUBLIC;
    return addSymbolTypeInfoNoLock(context, node, typeInfo, kind, computedValue, flags, resultName, storageOffset, aliasName);
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(JobContext*    context,
                                                  AstNode*       node,
                                                  TypeInfo*      typeInfo,
                                                  SymbolKind     kind,
                                                  ComputedValue* computedValue,
                                                  uint32_t       flags,
                                                  SymbolName**   resultName,
                                                  uint32_t       storageOffset,
                                                  Utf8Crc*       aliasName)
{
    SWAG_RACE_CONDITION_WRITE(raceCondition);

    if (!aliasName)
        aliasName = &node->name;

    auto symbol = findNoLock(*aliasName);

    // Be sure we have a symbol
    if (!symbol)
        symbol = registerSymbolNameNoLock(context, node, kind, aliasName);
    else if (symbol->kind == SymbolKind::PlaceHolder && kind != SymbolKind::PlaceHolder)
        symbol->kind = kind;

    // Only add an inline parameter/retval once in a given scope
    else if ((flags & (OVERLOAD_VAR_INLINE | OVERLOAD_RETVAL)) && symbol->overloads.size())
    {
        if (resultName)
            *resultName = symbol;
        return symbol->overloads[0];
    }

    if (resultName)
        *resultName = symbol;

    {
        unique_lock lock(symbol->mutex);

        SymbolOverload* result = nullptr;

        // A structure is defined the first time as incomplete (so that it can reference itself)
        if (symbol->kind == SymbolKind::Struct ||
            symbol->kind == SymbolKind::Interface ||
            symbol->kind == SymbolKind::TypeSet ||
            symbol->kind == SymbolKind::Function)
        {
            for (auto resolved : symbol->overloads)
            {
                if (resolved->typeInfo == typeInfo && (resolved->flags & OVERLOAD_INCOMPLETE))
                {
                    result = resolved;
                    result->flags &= ~OVERLOAD_INCOMPLETE;
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
            if (!(flags & OVERLOAD_VAR_FUNC_PARAM) && symbol->kind == SymbolKind::Variable)
                addVarToDrop(result, result->typeInfo, storageOffset);
        }

        result->flags |= flags;
        result->storageOffset = storageOffset;

        // One less overload. When this reached zero, this means we know every types for the same symbol,
        // and so we can wakeup all jobs waiting for that symbol to be solved
        if (!(flags & OVERLOAD_INCOMPLETE))
        {
            decreaseOverloadNoLock(symbol);
        }

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

        return result;
    }
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

bool SymTable::checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind type)
{
    shared_lock lk(mutex);
    return checkHiddenSymbolNoLock(context, node, typeInfo, type, nullptr, true);
}

bool SymTable::checkHiddenSymbolNoLock(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName)
{
    auto& token = node->token;
    auto& name  = node->name;

    if (!symbol)
        symbol = findNoLock(name);
    if (!symbol)
        return true;

    // A symbol with a different kind already exists
    if (symbol->kind != kind)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined as %s in an accessible scope", symbol->name.c_str(), SymTable::getArticleKindName(symbol->kind));
        Diagnostic diag{node, token, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    if (symbol->kind == SymbolKind::Namespace)
        return true;

    // Overloads are not allowed on certain types
    bool canOverload = false;
    if (kind == SymbolKind::Function || kind == SymbolKind::Attribute || kind == SymbolKind::Struct || kind == SymbolKind::Interface || kind == SymbolKind::TypeSet)
        canOverload = true;
    if (!canOverload && !symbol->overloads.empty())
    {
        auto       firstOverload = symbol->overloads[0];
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{node, token, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (!canOverload && checkSameName)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{node, token, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    // A symbol with the same type already exists
    auto overload = symbol->findOverload(typeInfo);
    if (overload)
    {
        // This is fine to define a foreign thing multiple times, if they have the same signature
        if (!(node->attributeFlags & ATTRIBUTE_FOREIGN) || !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            auto       firstOverload = overload;
            Utf8       msg           = format("symbol '%s' already defined with the same signature in an accessible scope", symbol->name.c_str());
            Diagnostic diag{node, token, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
            if (typeInfo->kind == TypeInfoKind::FuncAttr)
                diagNote.codeComment = Ast::computeGenericParametersReplacement(((TypeInfoFuncAttr*) typeInfo)->genericParameters);
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
    scoped_lock lkn(symbol->mutex);

    if (!symbol->overloads.empty())
    {
        if (symbol->kind != SymbolKind::Alias)
        {
            auto       firstOverload = symbol->overloads[0];
            Utf8       msg           = format("symbol '%s' already defined as %s in an accessible scope", symbol->name.c_str(), SymTable::getArticleKindName(symbol->kind));
            Diagnostic diag{node, node->token, msg};
            Utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
            context->report(diag, &diagNote);
        }
        else
        {
            Utf8       msg = format("symbol '%s' already defined as a name alias in an accessible scope", symbol->name.c_str());
            Diagnostic diag{node, node->token, msg};
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
    case SymbolKind::TypeSet:
        return "a typeset";
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
    case SymbolKind::TypeSet:
        return "typeset";
    case SymbolKind::GenericType:
        return "generic type";
    case SymbolKind::PlaceHolder:
        return "placeholder";
    }

    return "<symbol>";
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
    unique_lock lk(mutex);
    dependentJobs.add(job);
}

void SymbolName::addDependentJobNoLock(Job* job)
{
    dependentJobs.add(job);
}

const Utf8& SymbolName::getFullName()
{
    unique_lock lk(mutex);
    if (!fullName.empty())
        return fullName;
    Scope::makeFullName(fullName, ownerTable->scope->getFullName(), name);
    return fullName;
}

SymbolName* SymTableHash::find(const Utf8Crc& str)
{
    if (!allocated)
        return nullptr;

    uint32_t idx = str.crc % allocated;
    while (buffer[idx].hash)
    {
        if (buffer[idx].hash == str.crc && !strcmp((const char*) buffer[idx].symbolName->name.buffer, (const char*) str.buffer))
            return buffer[idx].symbolName;
        idx = idx + 1;
        if (idx == allocated)
            idx = 0;
    }

    return nullptr;
}

void SymTableHash::addElem(SymbolName* data)
{
    // Find a free slot
    uint32_t idx = data->name.crc % allocated;
    while (buffer[idx].hash)
    {
        idx = idx + 1;
        if (idx == allocated)
            idx = 0;
    }

    buffer[idx].hash       = data->name.crc;
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

        allocated *= 2;
        buffer = (Entry*) g_Allocator.alloc(allocated * sizeof(Entry));
        memset(buffer, 0, allocated * sizeof(Entry));

        count = 0;
        for (uint32_t i = 0; i < oldAllocated; i++)
        {
            if (oldBuffer[i].hash)
            {
                oldCount--;
                addElem(oldBuffer[i].symbolName);
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