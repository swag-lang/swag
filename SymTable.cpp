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

SymbolName* SymTable::registerSymbolNameNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, SymbolKind type)
{
    auto symbol = findNoLock(name);
    if (!symbol)
    {
        symbol                                = sourceFile->poolFactory->symName.alloc();
        symbol->name                          = name;
        symbol->kind                          = type;
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

bool SymTable::addSymbolTypeInfo(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type)
{
    scoped_lock lk(mutex);
    return addSymbolTypeInfoNoLock(sourceFile, token, name, typeInfo, type);
}

bool SymTable::addSymbolTypeInfoNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type)
{
    auto symbol = findNoLock(name);
    if (!symbol)
        symbol = registerSymbolNameNoLock(sourceFile, token, name, type);

    if (!checkHiddenSymbolNoLock(sourceFile, token, name, typeInfo, type, symbol))
        return false;
    symbol->addOverloadNoLock(sourceFile, token, typeInfo);

    // One less overload. When this reached zero, this means we known every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    symbol->cptOverloads--;
    if (symbol->cptOverloads == 0)
    {
        for (auto job : symbol->dependentJobs)
            g_ThreadMgr.addJob(job);
    }

    return true;
}

bool SymTable::checkHiddenSymbol(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type)
{
    scoped_lock lk(mutex);
    return checkHiddenSymbolNoLock(sourceFile, token, name, typeInfo, type, nullptr);
}

bool SymTable::checkHiddenSymbolNoLock(SourceFile* sourceFile, const Token& token, const Utf8Crc& name, TypeInfo* typeInfo, SymbolKind type, SymbolName* symbol)
{
    if (!symbol)
        symbol = findNoLock(name);
    if (!symbol)
        return true;

    // A symbol with a different type already exists
    if (symbol->kind != type)
    {
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined with a different type in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    // Overloads are not allowed on certain types
    if (type != SymbolKind::Function && !symbol->overloads.empty())
    {
        auto       firstOverload = symbol->overloads[0];
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
        auto       firstOverload = &symbol->defaultOverload;
        Utf8       msg           = format("symbol '%s' already defined with the same type in an accessible scope", symbol->name.c_str());
        Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
        Utf8       note = "this is the other definition";
        Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
        sourceFile->report(diag, &diagNote);
        return false;
    }

    return true;
}

void SymbolName::addOverloadNoLock(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo)
{
    auto overload           = sourceFile->poolFactory->symOverload.alloc();
    overload->typeInfo      = typeInfo;
    overload->sourceFile    = sourceFile;
    overload->startLocation = token.startLocation;
    overload->endLocation   = token.endLocation;
    overloads.push_back(overload);
}