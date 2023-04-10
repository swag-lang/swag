#include "pch.h"
#include "Diagnostic.h"
#include "TypeManager.h"

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
