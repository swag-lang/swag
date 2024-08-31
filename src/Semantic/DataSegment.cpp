#include "pch.h"
#include "Semantic/DataSegment.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

void DataSegment::getNames(const char** name, const char** shortName, SegmentKind kind)
{
    switch (kind)
    {
        case SegmentKind::Compiler:
            *name      = "compiler segment";
            *shortName = "cmp";
            break;
        case SegmentKind::Tls:
            *name      = "tls segment";
            *shortName = "tls";
            break;
        case SegmentKind::Data:
            *name      = "data segment";
            *shortName = "dat";
            break;
        case SegmentKind::Bss:
            *name      = "bss segment";
            *shortName = "bss";
            break;
        case SegmentKind::Constant:
            *name      = "constant segment";
            *shortName = "cst";
            break;
        case SegmentKind::Global:
            *name      = "global segment";
            *shortName = "glb";
            break;
        case SegmentKind::String:
            *name      = "string segment";
            *shortName = "str";
            break;
        case SegmentKind::Me:
            break;
    }
}

void DataSegment::setup(SegmentKind myKind, Module* myModule)
{
    kind   = myKind;
    module = myModule;

    // :DefaultSizeBuckets
    if (myModule->is(ModuleKind::BootStrap))
        granularity = 4 * 1024;
    else if (myModule->is(ModuleKind::Runtime))
        granularity = 16 * 1024;
    else
        granularity = 4 * 1024;

    getNames(&name, &shortName, kind);
}

void DataSegment::initFrom(DataSegment* other)
{
    if (other->totalCount)
    {
        // :DefaultSizeBuckets
        // If this asserts, then we must increase the default granularity size in setup()
        SWAG_ASSERT(other->buckets.size() == 1);
        reserve(other->totalCount);
        std::copy_n(other->buckets[0].buffer, other->totalCount, buckets[0].buffer);
    }

    initPtr.clear();
    initFuncPtr.clear();

    // We need pointers to be in the right segment, not the original one, because sometimes
    // we have to find back the offset from the pointer.
    // :BackPtrOffset
    for (const auto& it : other->initPtr)
    {
        // If this asserts, then this means that the bootstrap/runtime makes cross-references between
        // segments. The update will have to be changed in that case, because here we will have to
        // get the address from another segment.
        SWAG_ASSERT(it.fromSegment == SegmentKind::Me || it.fromSegment == kind);

        const auto patchAddr                 = address(it.patchOffset);
        const auto fromAddr                  = address(it.fromOffset);
        *reinterpret_cast<void**>(patchAddr) = fromAddr;
        addInitPtr(it.patchOffset, it.fromOffset, it.fromSegment);
    }

    for (auto& it : other->initFuncPtr)
        addInitPtrFunc(it.first, it.second);

    // This maps contain direct pointer to the original buffers from bootstrap/runtime.
    // This is fine, as original buffers are persistent, and bytecode will use them.
    // For runtime, offset are conserved, so this is fine too...
    for (auto& it : other->storedStrings)
        storedStrings[it.first] = it.second;
    for (auto& it : other->storedValues8)
        storedValues8[it.first] = it.second;
    for (auto& it : other->storedValues16)
        storedValues16[it.first] = it.second;
    for (auto& it : other->storedValues32)
        storedValues32[it.first] = it.second;
    for (auto& it : other->storedValues64)
        storedValues64[it.first] = it.second;
}

void DataSegment::align(uint32_t alignOf)
{
    ScopedLock lk(mutex);
    alignNoLock(alignOf);
}

void DataSegment::alignNoLock(uint32_t alignOf)
{
    // Align
    if (!buckets.empty() && alignOf > 1)
    {
        const auto alignOffset = static_cast<uint32_t>(TypeManager::align(totalCount, alignOf));
        if (alignOffset != totalCount)
        {
            const auto diff = alignOffset - totalCount;
            reserveNoLock(diff);
        }
    }
}

uint32_t DataSegment::reserve(uint32_t size, uint8_t** resultPtr, uint32_t alignOf)
{
    ScopedLock lk(mutex);
    return reserveNoLock(size, alignOf, resultPtr);
}

uint32_t DataSegment::reserveNoLock(uint32_t size, uint32_t alignOf, uint8_t** resultPtr)
{
    alignNoLock(alignOf);
    return reserveNoLock(size, resultPtr);
}

uint32_t DataSegment::reserveNoLock(uint32_t size, uint8_t** resultPtr)
{
    SWAG_RACE_CONDITION_WRITE(raceC);

    SWAG_ASSERT(size);
    Bucket* last = nullptr;
    if (!buckets.empty())
        last = &buckets.back();

    if (last && last->count + size <= last->size)
    {
        if (resultPtr)
            *resultPtr = last->buffer + last->count;
        last->count += size;
    }
    else
    {
        Bucket bucket;
        bucket.size = max(size, granularity);
        granularity *= 2;
        bucket.size   = Allocator::alignSize(bucket.size);
        bucket.buffer = Allocator::allocN<uint8_t>(bucket.size);
#ifdef SWAG_STATS
        g_Stats.memSeg += bucket.size;
#endif
        memset(bucket.buffer, 0, bucket.size);
        bucket.count = size;
        buckets.push_back(bucket);
        last = &buckets.back();
        if (resultPtr)
            *resultPtr = last->buffer;
    }

    // Check that we do not overflow maximum size
    if (static_cast<uint64_t>(totalCount) + size > SWAG_LIMIT_SEGMENT)
    {
        if (!overflow)
        {
            overflow = true;
            Report::error(module, formErr(Err0538, name, SWAG_LIMIT_SEGMENT));
        }
    }

    const auto result = totalCount;
    totalCount += size;
    return result;
}

uint32_t DataSegment::tryOffset(const uint8_t* location)
{
    SharedLock lock(mutex);

    uint32_t offset = 0;
    for (auto& i : buckets)
    {
        const auto bucket = &i;
        if (location >= bucket->buffer && location < bucket->buffer + bucket->count)
        {
            offset += static_cast<uint32_t>(location - bucket->buffer);
            return offset;
        }

        offset += bucket->count;
    }

    return UINT32_MAX;
}

uint32_t DataSegment::offset(const uint8_t* location)
{
    const auto offset = tryOffset(location);
    if (offset != UINT32_MAX)
        return offset;
    SWAG_ASSERT(false);
    return 0;
}

uint8_t* DataSegment::address(uint32_t location)
{
    SharedLock lock(mutex);
    return addressNoLock(location);
}

uint8_t* DataSegment::addressNoLock(uint32_t location)
{
    SWAG_RACE_CONDITION_READ(raceC);
    SWAG_ASSERT(location != UINT32_MAX);
    SWAG_ASSERT(!buckets.empty());
    for (auto& i : buckets)
    {
        const auto bucket = &i;
        if (location < static_cast<uint64_t>(bucket->count))
            return bucket->buffer + location;
        location -= bucket->count;
    }

    SWAG_ASSERT(false);
    return nullptr;
}

uint32_t DataSegment::addComputedValue(const TypeInfo* typeInfo, ComputedValue& computedValue, uint8_t** resultPtr)
{
    SWAG_ASSERT(typeInfo->isNative());
    SWAG_ASSERT(typeInfo->nativeType != NativeTypeKind::Any);
    SWAG_ASSERT(resultPtr);

    if (typeInfo->nativeType == NativeTypeKind::String)
    {
        const auto stringOffset = addString(computedValue.text);
        uint8_t*   addr;
        const auto storageOffset             = reserve(2 * sizeof(uint64_t), &addr);
        reinterpret_cast<uint64_t*>(addr)[0] = reinterpret_cast<uint64_t>(computedValue.text.buffer);
        reinterpret_cast<uint64_t*>(addr)[1] = computedValue.text.count;
        *resultPtr                           = addr;
        addInitPtr(storageOffset, stringOffset);
        return storageOffset;
    }

    ScopedLock lk(mutex);
    switch (typeInfo->sizeOf)
    {
        case 1:
            if (const auto it = storedValues8.find(computedValue.reg.u8); it != storedValues8.end())
            {
                *resultPtr = it->second.addr;
                return it->second.offset;
            }
            break;

        case 2:
            if (const auto it = storedValues16.find(computedValue.reg.u16); it != storedValues16.end())
            {
                *resultPtr = it->second.addr;
                return it->second.offset;
            }
            break;

        case 4:
            if (const auto it = storedValues32.find(computedValue.reg.u32); it != storedValues32.end())
            {
                *resultPtr = it->second.addr;
                return it->second.offset;
            }
            break;

        case 8:
            if (const auto it = storedValues64.find(computedValue.reg.u64); it != storedValues64.end())
            {
                *resultPtr = it->second.addr;
                return it->second.offset;
            }
            break;
    }

    uint8_t*   addr;
    const auto storageOffset = reserveNoLock(typeInfo->sizeOf, &addr);
    *resultPtr               = addr;

    switch (typeInfo->sizeOf)
    {
        case 1:
            *addr                               = computedValue.reg.u8;
            storedValues8[computedValue.reg.u8] = {storageOffset, addr};
            break;
        case 2:
            *reinterpret_cast<uint16_t*>(addr)    = computedValue.reg.u16;
            storedValues16[computedValue.reg.u16] = {storageOffset, addr};
            break;
        case 4:
            *reinterpret_cast<uint32_t*>(addr)    = computedValue.reg.u32;
            storedValues32[computedValue.reg.u32] = {storageOffset, addr};
            break;
        case 8:
            *reinterpret_cast<uint64_t*>(addr)    = computedValue.reg.u64;
            storedValues64[computedValue.reg.u64] = {storageOffset, addr};
            break;
    }

    return storageOffset;
}

uint32_t DataSegment::addString(const Utf8& str, uint8_t** resultPtr)
{
    ScopedLock lk(mutex);
    return addStringNoLock(str, resultPtr);
}

uint32_t DataSegment::addStringNoLock(const Utf8& str, uint8_t** resultPtr)
{
    SWAG_RACE_CONDITION_WRITE(raceC);

    // Same string already there ?
    using P                                 = MapUtf8<CacheValue>;
    const std::pair<P::iterator, bool> iter = storedStrings.insert(P::value_type(str, {}));
    if (!iter.second)
    {
        if (resultPtr)
            *resultPtr = iter.first->second.addr;
        return iter.first->second.offset;
    }

    uint8_t*   addr;
    const auto offset = reserveNoLock(str.count + 1, &addr);
    if (resultPtr)
        *resultPtr = addr;
    std::copy_n(str.buffer, str.count, addr);
    addr[str.count] = 0;

    iter.first->second = {offset, addr};
    return offset;
}

void DataSegment::addPatchPtr(int64_t* addr, int64_t value)
{
    if (kind == SegmentKind::Compiler)
        return;

    PatchPtrRef st;
    st.addr  = addr;
    st.value = value;
    patchPtr.push_back(st);
}

void DataSegment::applyPatchPtr() const
{
    for (const auto& it : patchPtr)
        *it.addr = it.value;
}

void DataSegment::addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset)
{
    ScopedLock lk(mutexPatchMethod);
    patchMethods.push_back({funcDecl, storageOffset});
    funcDecl->addSpecFlag(AstFuncDecl::SPEC_FLAG_PATCH);
}

void DataSegment::doPatchMethods(JobContext* context)
{
    ScopedLock lk(mutexPatchMethod);
    for (const auto it : patchMethods)
    {
        const auto funcNode  = it.first;
        void*      lambdaPtr = nullptr;
        if (funcNode->isForeign())
        {
            lambdaPtr = ByteCodeRun::makeLambda(context, funcNode, nullptr);
            addInitPtrFunc(it.second, funcNode->getFullNameForeignImport());
        }
        else if (funcNode->hasExtByteCode() && funcNode->extByteCode()->bc)
        {
            const auto bc       = funcNode->extByteCode()->bc;
            bc->isInDataSegment = true;
            bc->isUsed          = true;
            lambdaPtr           = ByteCodeRun::makeLambda(context, funcNode, bc);
            addInitPtrFunc(it.second, funcNode->getCallName());
        }

        if (lambdaPtr)
        {
            const auto addr                 = address(it.second);
            *reinterpret_cast<void**>(addr) = lambdaPtr;
        }
    }
}

void DataSegment::addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg)
{
    if (kind == SegmentKind::Compiler)
        return;

#ifdef SWAG_STATS
    ++g_Stats.numInitPtr;
#endif

    InitPtrRef ref;
    ref.patchOffset = patchOffset;
    ref.fromOffset  = srcOffset;
    ref.fromSegment = seg;

    ScopedLock lk(mutexPtr);

#ifdef SWAG_DEV_MODE
    // We must have at least one pointer difference with all other offsets, otherwise we will
    // have a weird memory overwrite
    // Note: can happen, because of structs, that a pointer is patched twice, so offset of 0 is fine
    // (even if no optimal)
    for (const auto ptr : initPtr)
    {
        const auto diff = abs(static_cast<int>(ptr.patchOffset) - static_cast<int>(patchOffset));
        SWAG_ASSERT(diff == 0 || diff >= 8);
    }
#endif

    initPtr.push_back(ref);
}

void DataSegment::addInitPtrFunc(uint32_t offset, const Utf8& funcName)
{
    if (kind == SegmentKind::Compiler)
        return;

#ifdef SWAG_STATS
    ++g_Stats.numInitFuncPtr;
#endif

    ScopedLock lk(mutexPtr);
    initFuncPtr[offset] = funcName;
}

bool DataSegment::readU64(Seek& seek, uint64_t& result)
{
    if (seek.seekBucket == buckets.size())
        return false;

    const auto* curBucket = &buckets[seek.seekBucket];
    if (seek.seekRead + 8 <= curBucket->count)
    {
        const uint64_t* ptr = reinterpret_cast<uint64_t*>(curBucket->buffer + seek.seekRead);
        seek.seekRead += 8;
        result = *ptr;
        return true;
    }

    int cpt                    = 0;
    result                     = 0;
    const uint8_t* ptr         = curBucket->buffer + seek.seekRead;
    uint32_t       shift       = 0;
    bool           resultValid = false;
    while (cpt != 8 && seek.seekBucket != buckets.size())
    {
        while (cpt != 8 && seek.seekRead < curBucket->count)
        {
            result |= static_cast<uint64_t>(*ptr) << shift;
            shift += 8;
            cpt++;
            ptr++;
            seek.seekRead++;
            resultValid = true;
        }

        if (seek.seekRead == curBucket->count)
        {
            seek.seekBucket++;
            if (seek.seekBucket != buckets.size())
            {
                seek.seekRead = 0;
                curBucket     = &buckets[seek.seekBucket];
                ptr           = curBucket->buffer;
            }
            else if (!resultValid)
                return false;
        }
    }

    return true;
}

void DataSegment::saveValue(uint8_t* address, uint32_t size, bool zero)
{
    ScopedLock lk(mutex);
    if (const auto it = savedValues.find(address); it != savedValues.end())
        return;

    SaveValue sv;
    sv.size = size;

    if (zero)
    {
        sv.value.u64         = 0;
        savedValues[address] = sv;
        return;
    }

    switch (size)
    {
        case 1:
            sv.value.u8 = *address;
            break;
        case 2:
            sv.value.u16 = *reinterpret_cast<uint16_t*>(address);
            break;
        case 4:
            sv.value.u32 = *reinterpret_cast<uint32_t*>(address);
            break;
        case 8:
            sv.value.u64 = *reinterpret_cast<uint64_t*>(address);
            break;
        default:
            sv.value.pointer = Allocator::allocN<uint8_t>(size);
            std::copy_n(address, size, sv.value.pointer);
            break;
    }
    savedValues[address] = sv;
}

void DataSegment::restoreAllValues()
{
    ScopedLock lk(mutex);
    for (const auto& one : savedValues)
    {
        if (one.second.value.u64 == 0)
        {
            std::fill_n(one.first, one.second.size, 0);
            continue;
        }

        switch (one.second.size)
        {
            case 1:
                *one.first = one.second.value.u8;
                break;
            case 2:
                *reinterpret_cast<uint16_t*>(one.first) = one.second.value.u16;
                break;
            case 4:
                *reinterpret_cast<uint32_t*>(one.first) = one.second.value.u32;
                break;
            case 8:
                *reinterpret_cast<uint64_t*>(one.first) = one.second.value.u64;
                break;
            default:
                std::copy_n(one.second.value.pointer, one.second.size, one.first);
                Allocator::free(one.second.value.pointer, one.second.size);
                break;
        }
    }

    savedValues.clear();
}

void DataSegment::release()
{
    deleted = true;
    for (const auto& b : buckets)
        Allocator::free(b.buffer, b.size);
    buckets.clear();
}

void DataSegment::makeLinear()
{
    ScopedLock lk(mutex);
    if (buckets.size() == 1)
        return;

    Bucket h;
    h.count  = Allocator::alignSize(totalCount);
    h.size   = h.count;
    h.buffer = Allocator::allocN<uint8_t>(h.count);

    auto ptr = h.buffer;
    for (const auto& b : buckets)
    {
        std::copy_n(b.buffer, b.count, ptr);
        ptr += b.count;
        Allocator::free(b.buffer, b.size);
    }

    buckets.clear();
    buckets.push_back(h);
}
