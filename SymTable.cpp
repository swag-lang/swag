#include "pch.h"
#include "SymTable.h"
#include "PoolFactory.h"
#include "Diagnostic.h"
#include "Global.h"

SymbolName* SymTable::find(const utf8crc& name)
{
    int                   indexInTable = name.crc % HASH_SIZE;
    scoped_lock<SpinLock> sl(mutex);
    if (!mapNames[indexInTable])
        return nullptr;
    auto it     = mapNames[indexInTable]->find(name);
    auto result = it != mapNames[indexInTable]->end() ? it->second : nullptr;
    return result;
}

SymbolName* SymTable::registerSymbolName(PoolFactory* factory, const utf8crc& name, SymbolType type)
{
    auto symbol = find(name);
    if (!symbol)
    {
        symbol           = factory->symName.alloc();
        symbol->name     = name;
        symbol->type     = type;
        int indexInTable = name.crc % HASH_SIZE;
        mutex.lock();
        if (!mapNames[indexInTable])
            mapNames[indexInTable] = new map<utf8crc, SymbolName*>();
        (*mapNames[indexInTable])[name] = symbol;
        mutex.unlock();
    }

    symbol->cptOverloads++;
    return symbol;
}

bool SymTable::addSymbol(SourceFile* sourceFile, const Token& token, const utf8crc& name, TypeInfo* typeInfo, SymbolType type)
{
    PoolFactory* factory = sourceFile->poolFactory;
    auto         symbol  = find(name);
    if (!symbol)
    {
        symbol = registerSymbolName(factory, name, type);
    }

    {
        scoped_lock lk(mutex);

        // A symbol with a different type already exists
        if (symbol->type != type)
        {
            auto       firstOverload = symbol->overloads[0];
            utf8       msg           = format("symbol '%s' already defined with a different type in the same scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
            sourceFile->report(diag, &diagNote);
            return false;
        }

        // Overloads are not allowed on certain types
        if (type == SymbolType::Variable && !symbol->overloads.empty())
        {
            auto       firstOverload = symbol->overloads[0];
            utf8       msg           = format("symbol '%s' already defined in the same scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
            sourceFile->report(diag, &diagNote);
            return false;
        }

        // A symbol with the same type already exists
        auto overload = symbol->findOverload(typeInfo);
        if (overload)
        {
            auto       firstOverload = symbol->overloads[0];
            utf8       msg           = format("symbol '%s' already defined with the same type in the same scope", symbol->name.c_str());
            Diagnostic diag{sourceFile, token.startLocation, token.endLocation, msg};
            utf8       note = "this is the other definition";
            Diagnostic diagNote{firstOverload->sourceFile, firstOverload->startLocation, firstOverload->endLocation, note, DiagnosticLevel::Note};
            sourceFile->report(diag, &diagNote);
            return false;
        }

        symbol->addOverload(sourceFile, token, typeInfo);
    }

    // One less overload. When this reached zero, this means we known every types for the same symbol,
    // and so we can wakeup all jobs waiting for that symbol to be solved
    symbol->cptOverloads--;
    return true;
}

void SymbolName::addOverload(SourceFile* sourceFile, const Token& token, TypeInfo* typeInfo)
{
    auto overload           = sourceFile->poolFactory->symOverload.alloc();
    overload->typeInfo      = typeInfo;
    overload->sourceFile    = sourceFile;
    overload->startLocation = token.startLocation;
    overload->endLocation   = token.endLocation;
    overloads.push_back(overload);
}