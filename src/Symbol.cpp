#include "pch.h"
#include "Diagnostic.h"
#include "TypeManager.h"

void SymbolOverload::from(SymbolOverload* other)
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

void SymbolOverload::setRegisters(const RegisterList& reg, uint32_t fl)
{
    if (fl == OVERLOAD_HINT_REG)
    {
        if (flags & OVERLOAD_INLINE_REG)
            return;
        if (flags & OVERLOAD_PERSISTENT_REG)
            return;
    }
    else if (fl & OVERLOAD_INLINE_REG)
    {
        SWAG_ASSERT(!(flags & OVERLOAD_INLINE_REG) || symRegisters == reg);
        SWAG_ASSERT(!(flags & OVERLOAD_PERSISTENT_REG));
        flags &= ~OVERLOAD_HINT_REG;
    }
    else
    {
        SWAG_ASSERT(!(flags & OVERLOAD_PERSISTENT_REG) || symRegisters == reg);
        SWAG_ASSERT(!(flags & OVERLOAD_INLINE_REG));
        flags &= ~OVERLOAD_HINT_REG;
    }

    flags |= fl;
    symRegisters = reg;
}

void SymbolName::decreaseOverloadNoLock()
{
    SWAG_ASSERT(cptOverloads);
    cptOverloads--;
    if (cptOverloads == 0)
        dependentJobs.setRunning();
}

SymbolOverload* SymbolName::addOverloadNoLock(AstNode* node, TypeInfo* typeInfo, ComputedValue* computedValue)
{
    auto overload      = Allocator::alloc<SymbolOverload>();
    overload->typeInfo = typeInfo;
    overload->node     = node;
#ifdef SWAG_STATS
    g_Stats.memSymOver += sizeof(SymbolOverload);
#endif

    if (computedValue)
    {
        overload->computedValue = *computedValue;
        overload->flags |= OVERLOAD_COMPUTED_VALUE;
    }

    overload->symbol = this;
    overloads.push_back(overload);
    return overload;
}

SymbolOverload* SymbolName::findOverload(TypeInfo* typeInfo)
{
    for (auto it : overloads)
    {
        if (it->typeInfo == typeInfo)
            return it;
        if (it->typeInfo->isSame(typeInfo, CASTFLAG_EXACT))
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
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] == node)
        {
            nodes.erase_unordered(i);
            break;
        }
    }
}
