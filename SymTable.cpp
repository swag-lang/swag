#include "pch.h"
#include "SymTable.h"
#include "Diagnostic.h"
#include "Global.h"
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
    scoped_lock<SpinLock> sl(mutex);
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

SymbolName* SymTable::registerSymbolNameNoLock(SourceFile* sourceFile, AstNode* node, SymbolKind kind)
{
    auto symbol = findNoLock(node->name);
    if (!symbol)
    {
        symbol                             = g_Pool_symName.alloc();
        symbol->name                       = node->name;
        symbol->fullName                   = Scope::makeFullName(scope->fullname, node->name);
        symbol->kind                       = kind;
        symbol->defaultOverload.sourceFile = sourceFile;
        symbol->defaultOverload.node       = node;
        int indexInTable                   = node->name.crc % HASH_SIZE;
        if (!mapNames[indexInTable])
            mapNames[indexInTable] = new map<Utf8Crc, SymbolName*>();
        (*mapNames[indexInTable])[node->name] = symbol;
    }

    symbol->cptOverloads++;
    return symbol;
}

SymbolOverload* SymTable::addSymbolTypeInfo(SourceFile*       sourceFile,
                                            AstNode*          node,
                                            TypeInfo*         typeInfo,
                                            SymbolKind        kind,
                                            ComputedValue*    computedValue,
                                            uint32_t          flags,
                                            SymbolName**      resultName,
                                            uint32_t          storageOffset,
                                            SymbolAttributes* attributes)
{
    scoped_lock lk(mutex);
    return addSymbolTypeInfoNoLock(sourceFile, node, typeInfo, kind, computedValue, flags, resultName, storageOffset, attributes);
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(SourceFile*       sourceFile,
                                                  AstNode*          node,
                                                  TypeInfo*         typeInfo,
                                                  SymbolKind        kind,
                                                  ComputedValue*    computedValue,
                                                  uint32_t          flags,
                                                  SymbolName**      resultName,
                                                  uint32_t          storageOffset,
                                                  SymbolAttributes* attributes)
{
    auto symbol = findNoLock(node->name);
    if (!symbol)
        symbol = registerSymbolNameNoLock(sourceFile, node, kind);
    if (resultName)
        *resultName = symbol;

    if (!checkHiddenSymbolNoLock(sourceFile, node->token, node->name, typeInfo, kind, symbol))
        return nullptr;

    auto result = symbol->addOverloadNoLock(sourceFile, node, typeInfo, computedValue);
    result->flags |= flags;
    result->storageOffset = storageOffset;
    if (attributes)
        result->attributes = *attributes;

    // One less overload. When this reached zero, this means we known every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    symbol->mutex.lock();
    symbol->cptOverloads--;
    if (symbol->cptOverloads == 0)
    {
        for (auto job : symbol->dependentJobs)
            g_ThreadMgr.addJob(job);
		symbol->dependentJobs.clear();
    }

    symbol->mutex.unlock();
    return result;
}

bool SymTable::checkHiddenSymbol(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type)
{
    scoped_lock lk(mutex);
    return checkHiddenSymbolNoLock(sourceFile, token, name, typeInfo, type, nullptr, true);
}

bool SymTable::checkHiddenSymbolNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, SymbolName* symbol, bool checkSameName)
{
    if (!symbol)
        symbol = findNoLock(name);
    if (!symbol)
        return true;

    // A symbol with a different kind already exists
    if (symbol->kind != kind)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined as %s in an accessible scope", symbol->name.c_str(), SymTable::getArticleKindName(symbol->kind));
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    bool canOverload = kind == SymbolKind::Function || kind == SymbolKind::Attribute;
    if (!canOverload && !symbol->overloads.empty())
    {
        auto       firstOverload = symbol->overloads[0];
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (!canOverload && checkSameName)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // A symbol with the same type already exists
    auto overload = symbol->findOverload(typeInfo);
    if (overload)
    {
        auto       firstOverload = overload;
        Utf8       msg           = format("function '%s' already defined with the same signature in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->node->token, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    return true;
}

SymbolOverload* SymbolName::addOverloadNoLock(SourceFile* sourceFile, AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue)
{
    auto overload        = g_Pool_symOverload.alloc();
    overload->typeInfo   = typeInfo;
    overload->sourceFile = sourceFile;
    overload->node       = node;

    if (computedValue)
    {
        overload->computedValue = *computedValue;
        overload->flags |= OVERLOAD_COMPUTED_VALUE;
    }

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
    case SymbolKind::Type:
        return "a type";
    case SymbolKind::Variable:
        return "a variable";
    case SymbolKind::Struct:
        return "a struct";
    }

    return "something else";
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
    case SymbolKind::Type:
        return "type";
    case SymbolKind::Variable:
        return "variable";
    case SymbolKind::Struct:
        return "struct";
    }

    return "???";
}

SymbolOverload* SymbolName::findOverload(TypeInfo* typeInfo)
{
    for (auto it : overloads)
    {
        if (it->typeInfo == typeInfo)
            return it;
        if (it->typeInfo->isSame(typeInfo))
            return it;
    }

    return nullptr;
}

bool SymbolAttributes::getValue(const string& fullName, ComputedValue& value)
{
    auto it = values.find(fullName);
    if (it == values.end())
        return false;
    value = it->second;
    return true;
}