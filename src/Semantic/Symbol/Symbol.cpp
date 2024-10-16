#include "pch.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeManager.h"

void SymbolOverload::from(const SymbolOverload* other)
{
    computedValue   = other->computedValue;
    symRegisters    = other->symRegisters;
    typeInfo        = other->typeInfo;
    node            = other->node;
    symbol          = other->symbol;
    fromInlineParam = other->fromInlineParam;
    storageIndex    = other->storageIndex;
    flags           = other->flags;
}

void SymbolOverload::setRegisters(const RegisterList& reg, OverloadFlags fl)
{
    ScopedLock lk(symbol->mutex);
    if (fl == OVERLOAD_REG_HINT)
    {
        if (hasFlag(OVERLOAD_REG_INLINE))
            return;
        if (hasFlag(OVERLOAD_REG_PERSISTENT))
            return;
    }
    else if (fl.has(OVERLOAD_REG_INLINE))
    {
        SWAG_ASSERT(!hasFlag(OVERLOAD_REG_PERSISTENT));
        flags.remove(OVERLOAD_REG_HINT);
    }
    else
    {
        SWAG_ASSERT(!hasFlag(OVERLOAD_REG_INLINE));
        flags.remove(OVERLOAD_REG_HINT);
    }

    flags.add(fl);
    symRegisters = reg;
}

void SymbolName::decreaseOverloadNoLock()
{
    SWAG_ASSERT(cptOverloads);
    cptOverloads--;
    if (cptOverloads == 0)
        dependentJobs.setRunning();
}

SymbolOverload* SymbolName::addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, const ComputedValue* computedValue)
{
    const auto overload = Allocator::alloc<SymbolOverload>();
    overload->typeInfo  = typeInfo;
    overload->node      = node;
#ifdef SWAG_STATS
    g_Stats.memSymOver += sizeof(SymbolOverload);
#endif

    if (computedValue)
    {
        overload->computedValue = *computedValue;
        overload->flags.add(OVERLOAD_CONST_VALUE);
    }

    overload->symbol = this;
    overloads.push_back(overload);
    return overload;
}

SymbolOverload* SymbolName::findOverload(const TypeInfo* typeInfo)
{
    for (const auto it : overloads)
    {
        if (it->typeInfo == typeInfo)
            return it;
        if (it->typeInfo->isSame(typeInfo, CAST_FLAG_EXACT))
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

Utf8 SymbolName::getFullName() const
{
    Utf8 fullName;
    Scope::makeFullName(fullName, ownerTable->scope->getFullName(), name);
    return fullName;
}

void SymbolName::unregisterNode(const AstNode* node)
{
    // Unregister the node in the symbol
    for (uint32_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] == node)
        {
            nodes.erase_unordered(i);
            break;
        }
    }
}
