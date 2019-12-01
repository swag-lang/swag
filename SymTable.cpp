#include "pch.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "Scope.h"

Pool<SymbolOverload> g_Pool_symOverload;
Pool<SymbolName>     g_Pool_symName;

SymTable::SymTable(Scope* scope)
    : scope{scope}
{
    memset(mapNames, 0, sizeof(mapNames));
}

SymbolName* SymTable::find(const Utf8Crc& name)
{
    scoped_lock<shared_mutex> sl(mutex);
    return findNoLock(name);
}

SymbolName* SymTable::findNoLock(const Utf8Crc& name)
{
    int indexInTable = name.crc % HASH_SIZE;
    if (!mapNames[indexInTable])
        return nullptr;
    auto it     = mapNames[indexInTable]->find(name);
    auto result = it != mapNames[indexInTable]->end() ? it->second : nullptr;
    return result;
}

SymbolName* SymTable::registerSymbolNameNoLock(JobContext* context, AstNode* node, SymbolKind kind, Utf8Crc* aliasName)
{
    if (!aliasName)
        aliasName = &node->name;

    SWAG_ASSERT(!aliasName->empty());
    auto symbol = findNoLock(*aliasName);
    if (!symbol)
    {
        symbol                       = g_Pool_symName.alloc();
        symbol->name                 = *aliasName;
        symbol->fullName             = Scope::makeFullName(scope->fullname, *aliasName);
        symbol->kind                 = kind;
        symbol->defaultOverload.node = node;
        symbol->ownerTable           = this;
        int indexInTable             = aliasName->crc % HASH_SIZE;
        if (!mapNames[indexInTable])
            mapNames[indexInTable] = new map<Utf8Crc, SymbolName*>();
        (*mapNames[indexInTable])[*aliasName] = symbol;
    }

    // Error if overload is not possible
    {
        scoped_lock lock(symbol->mutex);
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
    scoped_lock lk(mutex);
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
    if (!aliasName)
        aliasName = &node->name;

    auto symbol = findNoLock(*aliasName);
    if (!symbol)
        symbol = registerSymbolNameNoLock(context, node, kind, aliasName);
    if (resultName)
        *resultName = symbol;

    {
        scoped_lock lock(symbol->mutex);

        SymbolOverload* result = nullptr;

        // A structure is defined the first time as incomplete (so that it can reference itself)
        if (symbol->kind == SymbolKind::Struct || symbol->kind == SymbolKind::Interface || symbol->kind == SymbolKind::Function)
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
            if (!(flags & OVERLOAD_VAR_INLINE))
            {
                if (!checkHiddenSymbolNoLock(context, node, typeInfo, kind, symbol))
                    return nullptr;
            }

            result = symbol->addOverloadNoLock(node, typeInfo, computedValue);

            // Remember all variables of type struct
            if (symbol->kind == SymbolKind::Variable && typeInfo->kind == TypeInfoKind::Struct)
                allStructs.push_back(result);
        }

        result->flags |= flags;
        result->storageOffset = storageOffset;

        // One less overload. When this reached zero, this means we known every types for the same symbol,
        // and so we can wakeup all jobs waiting for that symbol to be solved
        if (!(flags & OVERLOAD_INCOMPLETE))
        {
            decreaseOverloadNoLock(symbol);
        }

        // In case of an incomplete function, we can wakeup jobs too when every overloads have been covered,
        // because an incomplete function doesn't yet know its return type, but we don't need it in order
        // to make a match
        else if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
        {
            for (auto job : symbol->dependentJobs.list)
                g_ThreadMgr.addJob(job);
            symbol->dependentJobs.clear();
        }

        return result;
    }
}

void SymTable::decreaseOverloadNoLock(SymbolName* symbol)
{
    symbol->cptOverloads--;
    if (symbol->cptOverloads == 0)
    {
        for (auto job : symbol->dependentJobs.list)
            g_ThreadMgr.addJob(job);
        symbol->dependentJobs.clear();
    }
}

bool SymTable::checkHiddenSymbol(JobContext* context, AstNode* node, TypeInfo* typeInfo, SymbolKind type)
{
    scoped_lock lk(mutex);
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
    bool canOverload = kind == SymbolKind::Function || kind == SymbolKind::Attribute || kind == SymbolKind::Struct || kind == SymbolKind::Interface;
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
        auto       firstOverload = overload;
        Utf8       msg           = format("symbol '%s' already defined with the same signature in an accessible scope", symbol->name.c_str());
        Diagnostic diag{node, token, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->node, firstOverload->node->token, note, DiagnosticLevel::Note};
        context->report(diag, &diagNote);
        return false;
    }

    return true;
}

SymbolOverload* SymbolName::addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue)
{
    auto overload      = g_Pool_symOverload.alloc();
    overload->typeInfo = typeInfo;
    overload->node     = node;

    if (computedValue)
    {
        overload->computedValue = *computedValue;
        overload->flags |= OVERLOAD_COMPUTED_VALUE;
    }

    overload->overloadIndex = (uint32_t) overloads.size() - 1;
    overloads.push_back(overload);
    return overload;
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
    case SymbolKind::Variable:
        return "a variable";
    case SymbolKind::Struct:
        return "a struct";
    case SymbolKind::Interface:
        return "an interface";
    case SymbolKind::GenericType:
        return "a generic type";
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
    case SymbolKind::Variable:
        return "variable";
    case SymbolKind::Struct:
        return "struct";
    case SymbolKind::Interface:
        return "interface";
    case SymbolKind::GenericType:
        return "generic type";
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
	scoped_lock lk(mutex);
	dependentJobs.add(job);
}

void SymbolName::addDependentJobNoLock(Job* job)
{
    dependentJobs.add(job);
}