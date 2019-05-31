#include "pch.h"
#include "SymTable.h"
#include "PoolFactory.h"
#include "Diagnostic.h"
#include "Global.h"
#include "ThreadManager.h"

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

SymbolName* SymTable::registerSymbolNameNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, SymbolKind kind)
{
    auto symbol = findNoLock(name);
    if (!symbol)
    {
        symbol                                = sourceFile->poolFactory->symName.alloc();
        symbol->name                          = name;
        symbol->kind                          = kind;
        symbol->defaultOverload.sourceFile    = sourceFile;
        symbol->defaultOverload.startLocation = token.startLocation;
        symbol->defaultOverload.endLocation   = token.endLocation;
        int indexInTable                      = name.crc % HASH_SIZE;
        if (!mapNames[indexInTable])
            mapNames[indexInTable] = new map<Utf8Crc, SymbolName*>();
        (*mapNames[indexInTable])[name] = symbol;
    }

    symbol->cptOverloads++;
    return symbol;
}

SymbolOverload* SymTable::addSymbolTypeInfo(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue)
{
    scoped_lock lk(mutex);
    return addSymbolTypeInfoNoLock(sourceFile, token, name, typeInfo, kind, computedValue);
}

SymbolOverload* SymTable::addSymbolTypeInfoNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind kind, ComputedValue* computedValue)
{
    auto symbol = findNoLock(name);
    if (!symbol)
        symbol = registerSymbolNameNoLock(sourceFile, token, name, kind);

    if (!checkHiddenSymbolNoLock(sourceFile, token, name, typeInfo, kind, symbol))
        return nullptr;
    auto result = symbol->addOverloadNoLock(sourceFile, token, typeInfo, computedValue);

    // One less overload. When this reached zero, this means we known every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    symbol->mutex.lock();
    symbol->cptOverloads--;
    if (symbol->cptOverloads == 0)
    {
        for (auto job : symbol->dependentJobs)
            g_ThreadMgr.addJob(job);
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
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (kind != SymbolKind::Function && !symbol->overloads.empty())
    {
        auto       firstOverload = symbol->overloads[0];
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (checkSameName && kind != SymbolKind::Function)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
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
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    return true;
}

SymbolOverload* SymbolName::addOverloadNoLock(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo, ComputedValue* computedValue)
{
    auto overload           = sourceFile->poolFactory->symOverload.alloc();
    overload->typeInfo      = typeInfo;
    overload->sourceFile    = sourceFile;
    overload->startLocation = token.startLocation;
    overload->endLocation   = token.endLocation;
    if (computedValue)
        overload->computedValue = *computedValue;
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
