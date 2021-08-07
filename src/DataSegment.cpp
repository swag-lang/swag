#include "pch.h"
#include "DataSegment.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "AstNode.h"
#include "ByteCode.h"

void DataSegment::setup(SegmentKind _kind, Module* _module)
{
    kind   = _kind;
    module = _module;

    // :DefaultSizeBuckets
    if (_module->kind == ModuleKind::BootStrap)
        granularity = 4 * 1024;
    else if (_module->kind == ModuleKind::Runtime)
        granularity = 16 * 1024;
    else
        granularity = 4 * 1024;

    switch (kind)
    {
    case SegmentKind::Compiler:
        name = "compiler segment";
        break;
    case SegmentKind::Tls:
        name = "tls segment";
        break;
    case SegmentKind::Data:
        name = "data segment";
        break;
    case SegmentKind::Bss:
        name = "bss segment";
        break;
    case SegmentKind::Constant:
        name = "constant segment";
        break;
    case SegmentKind::Global:
        name = "global segment";
        break;
    case SegmentKind::String:
        name = "string segment";
        break;
    }
}

void DataSegment::initFrom(DataSegment* other)
{
    if (other->totalCount)
    {
        // :DefaultSizeBuckets
        // If this assert, then we must increase the default granularity size in setup()
        SWAG_ASSERT(other->buckets.size() == 1);
        reserve(other->totalCount);
        memcpy(buckets[0].buffer, other->buckets[0].buffer, other->totalCount);
    }

    initPtr.clear();
    initFuncPtr.clear();

    // We need pointers to be in the right segment, not the original one, because sometimes
    // we have to find back the offset from the pointer.
    // :BackPtrOffset
    for (auto& it : other->initPtr)
    {
        // If this asserts, then this means that the bootstrap/runtime makes cross references between
        // segments. The update will have to be changed in that case, because here we will have to
        // get the address from another segment.
        SWAG_ASSERT(it.fromSegment == SegmentKind::Me || it.fromSegment == kind);

        auto patchAddr      = address(it.patchOffset);
        auto fromAddr       = address(it.fromOffset);
        *(void**) patchAddr = fromAddr;
        addInitPtr(it.patchOffset, it.fromOffset, it.fromSegment);
    }

    for (auto& it : other->initFuncPtr)
        addInitPtrFunc(it.first, it.second.first, it.second.second);

    // This maps contain direct pointer to the original buffers from bootstrap/runtime.
    // This is fine, as original buffers are persistent, and byte code will use them.
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
    scoped_lock lk(mutex);
    alignNoLock(alignOf);
}

void DataSegment::alignNoLock(uint32_t alignOf)
{
    // Align
    if (buckets.size() && alignOf > 1)
    {
        auto alignOffset = (uint32_t) TypeManager::align(totalCount, alignOf);
        if (alignOffset != totalCount)
        {
            auto diff = alignOffset - totalCount;
            reserveNoLock(diff);
        }
    }
}

uint32_t DataSegment::reserve(uint32_t size, uint8_t** resultPtr, uint32_t alignOf)
{
    scoped_lock lk(mutex);
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
    if (buckets.size())
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
        bucket.size   = (uint32_t) Allocator::alignSize(bucket.size);
        bucket.buffer = (uint8_t*) g_Allocator.alloc(bucket.size);
        if (g_CommandLine.stats)
            g_Stats.memSeg += bucket.size;
        memset(bucket.buffer, 0, bucket.size);
        bucket.count = size;
        buckets.push_back(bucket);
        last = &buckets.back();
        if (resultPtr)
            *resultPtr = last->buffer;
    }

    // Check that we do not overflow maximum size
    if ((uint64_t) totalCount + size > SWAG_LIMIT_SEGMENT)
    {
        if (!overflow)
        {
            overflow = true;
            module->error(Utf8::format(Msg0834, name));
        }
    }

    auto result = totalCount;
    totalCount += size;
    return result;
}

uint32_t DataSegment::offset(uint8_t* location)
{
    shared_lock lock(mutex);

    uint32_t offset = 0;
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = &buckets[i];
        if (location >= bucket->buffer && location < bucket->buffer + bucket->count)
        {
            offset += (uint32_t)(location - bucket->buffer);
            return offset;
        }

        offset += bucket->count;
    }

    SWAG_ASSERT(false);
    return 0;
}

uint8_t* DataSegment::address(DataSegment* lockedSegment, uint32_t location)
{
    if (lockedSegment != this)
        mutex.lock();
    auto result = addressNoLock(location);
    if (lockedSegment != this)
        mutex.unlock();
    return result;
}

uint8_t* DataSegment::address(uint32_t location)
{
    shared_lock lock(mutex);
    return addressNoLock(location);
}

uint8_t* DataSegment::addressNoLock(uint32_t location)
{
    SWAG_RACE_CONDITION_READ(raceC);
    SWAG_ASSERT(location != UINT32_MAX);
    SWAG_ASSERT(buckets.size());
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = &buckets[i];
        if (location < (uint64_t) bucket->count)
            return bucket->buffer + location;
        location -= bucket->count;
    }

    SWAG_ASSERT(false);
    return nullptr;
}

uint32_t DataSegment::addComputedValue(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue, uint8_t** resultPtr)
{
    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
    SWAG_ASSERT(typeInfo->nativeType != NativeTypeKind::Any);

    if (typeInfo->nativeType == NativeTypeKind::String)
    {
        auto     stringOffset = addString(computedValue.text);
        uint8_t* addr;
        auto     storageOffset = reserve(2 * sizeof(uint64_t), &addr);
        ((uint64_t*) addr)[0]  = (uint64_t) computedValue.text.c_str();
        ((uint64_t*) addr)[1]  = (uint32_t) computedValue.text.length();
        if (resultPtr)
            *resultPtr = addr;
        addInitPtr(storageOffset, stringOffset);
        return storageOffset;
    }

    scoped_lock lk(mutex);
    switch (typeInfo->sizeOf)
    {
    case 1:
    {
        auto it = storedValues8.find(computedValue.reg.u8);
        if (it != storedValues8.end())
        {
            if (resultPtr)
                *resultPtr = it->second.addr;
            return it->second.offset;
        }
        break;
    }

    case 2:
    {
        auto it = storedValues16.find(computedValue.reg.u16);
        if (it != storedValues16.end())
        {
            if (resultPtr)
                *resultPtr = it->second.addr;
            return it->second.offset;
        }
        break;
    }

    case 4:
    {
        auto it = storedValues32.find(computedValue.reg.u32);
        if (it != storedValues32.end())
        {
            if (resultPtr)
                *resultPtr = it->second.addr;
            return it->second.offset;
        }
        break;
    }
    case 8:
    {
        auto it = storedValues64.find(computedValue.reg.u64);
        if (it != storedValues64.end())
        {
            if (resultPtr)
                *resultPtr = it->second.addr;
            return it->second.offset;
        }
        break;
    }
    }

    uint8_t* addr;
    auto     storageOffset = reserveNoLock(typeInfo->sizeOf, &addr);
    if (resultPtr)
        *resultPtr = addr;

    switch (typeInfo->sizeOf)
    {
    case 1:
        *(uint8_t*) addr                    = computedValue.reg.u8;
        storedValues8[computedValue.reg.u8] = {storageOffset, addr};
        break;
    case 2:
        *(uint16_t*) addr                     = computedValue.reg.u16;
        storedValues16[computedValue.reg.u16] = {storageOffset, addr};
        break;
    case 4:
        *(uint32_t*) addr                     = computedValue.reg.u32;
        storedValues32[computedValue.reg.u32] = {storageOffset, addr};
        break;
    case 8:
        *(uint64_t*) addr                     = computedValue.reg.u64;
        storedValues64[computedValue.reg.u64] = {storageOffset, addr};
        break;
    }

    return storageOffset;
}

uint32_t DataSegment::addString(const Utf8& str, uint8_t** resultPtr)
{
    scoped_lock lk(mutex);
    return addStringNoLock(str, resultPtr);
}

uint32_t DataSegment::addStringNoLock(const Utf8& str, uint8_t** resultPtr)
{
    SWAG_RACE_CONDITION_WRITE(raceC);

    // Same string already there ?
    auto it = storedStrings.find(str);
    if (it != storedStrings.end())
    {
        if (resultPtr)
            *resultPtr = it->second.addr;
        return it->second.offset;
    }

    auto     strLen = (uint32_t) str.length() + 1;
    uint8_t* addr;
    auto     offset = reserveNoLock(strLen, &addr);
    if (resultPtr)
        *resultPtr = addr;
    memcpy(addr, str.c_str(), strLen);
    SWAG_ASSERT(addr[strLen - 1] == 0);
    storedStrings[str] = {offset, addr};

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

void DataSegment::applyPatchPtr()
{
    for (auto& it : patchPtr)
        *it.addr = it.value;
}

void DataSegment::addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset)
{
    scoped_lock lk(mutexPatchMethod);
    patchMethods.push_back({funcDecl, storageOffset});
}

void DataSegment::doPatchMethods(JobContext* context)
{
    scoped_lock lk(mutexPatchMethod);
    for (auto it : patchMethods)
    {
        auto      funcNode  = it.first;
        void*     lambdaPtr = nullptr;
        ByteCode* bc        = nullptr;
        if (funcNode->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            funcNode->computeFullNameForeign(false);
            lambdaPtr = ByteCodeRun::makeLambda(context, funcNode, nullptr);
            addInitPtrFunc(it.second, funcNode->fullnameForeign, DataSegment::RelocType::Foreign);
        }
        else if (funcNode->extension && funcNode->extension->bc)
        {
            bc        = funcNode->extension->bc;
            lambdaPtr = ByteCodeRun::makeLambda(context, funcNode, bc);
            addInitPtrFunc(it.second, bc->getCallName(), DataSegment::RelocType::Local);
        }

        if (lambdaPtr)
        {
            auto addr      = address(it.second);
            *(void**) addr = lambdaPtr;
        }
    }
}

void DataSegment::addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg)
{
    if (kind == SegmentKind::Compiler)
        return;

    InitPtrRef ref;
    ref.patchOffset = patchOffset;
    ref.fromOffset  = srcOffset;
    ref.fromSegment = seg;

    scoped_lock lk(mutexPtr);

#ifdef SWAG_DEV_MODE
    // We must have at least one pointer difference with all other offsets, otherwise we will
    // have a weird memory overwrite
    for (int i = 0; i < initPtr.size(); i++)
        SWAG_ASSERT(abs((int) initPtr[i].patchOffset - (int) patchOffset) >= 8);
#endif

    initPtr.push_back(ref);
    if (g_CommandLine.stats)
        g_Stats.numInitPtr++;
}

void DataSegment::addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType)
{
    if (kind == SegmentKind::Compiler)
        return;

    scoped_lock lk(mutexPtr);

    initFuncPtr[offset] = {funcName, relocType};
    if (g_CommandLine.stats)
        g_Stats.numInitFuncPtr++;
}

bool DataSegment::readU64(Seek& seek, uint64_t& result)
{
    if (seek.seekBucket == buckets.size())
        return false;

    auto* curBucket = &buckets[seek.seekBucket];
    if (seek.seekRead + 8 <= curBucket->count)
    {
        uint64_t* ptr = (uint64_t*) (curBucket->buffer + seek.seekRead);
        seek.seekRead += 8;
        result = *ptr;
        return true;
    }

    int cpt              = 0;
    result               = 0;
    uint8_t* ptr         = curBucket->buffer + seek.seekRead;
    uint32_t shift       = 0;
    bool     resultValid = false;
    while (cpt != 8 && seek.seekBucket != buckets.size())
    {
        while (cpt != 8 && (seek.seekRead < curBucket->count))
        {
            result |= ((uint64_t) *ptr) << shift;
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

void DataSegment::saveValue(void* address, uint32_t size, bool zero)
{
    scoped_lock lk(mutex);
    auto        it = savedValues.find(address);
    if (it != savedValues.end())
        return;

    if (zero)
    {
        savedValues[address] = {nullptr, size};
        return;
    }

    switch (size)
    {
    case 1:
        savedValues[address] = {(void*) (size_t) * (uint8_t*) address, size};
        break;
    case 2:
        savedValues[address] = {(void*) (size_t) * (uint16_t*) address, size};
        break;
    case 4:
        savedValues[address] = {(void*) (size_t) * (uint32_t*) address, size};
        break;
    case 8:
        savedValues[address] = {(void*) (size_t) * (uint64_t*) address, size};
        break;
    default:
        auto buf = g_Allocator.alloc(Allocator::alignSize(size));
        memcpy(buf, address, size);
        savedValues[address] = {buf, size};
        break;
    }
}

void DataSegment::restoreAllValues()
{
    scoped_lock lk(mutex);
    for (auto& one : savedValues)
    {
        if (one.second.ptr == nullptr)
        {
            memset(one.first, 0, one.second.size);
            continue;
        }

        switch (one.second.size)
        {
        case 1:
            *(uint8_t*) one.first = (uint8_t) * (uint8_t*) &one.second.ptr;
            break;
        case 2:
            *(uint16_t*) one.first = (uint16_t) * (uint16_t*) &one.second.ptr;
            break;
        case 4:
            *(uint32_t*) one.first = (uint32_t) * (uint32_t*) &one.second.ptr;
            break;
        case 8:
            *(uint64_t*) one.first = (uint64_t) * (uint64_t*) &one.second.ptr;
            break;
        default:
            memcpy(one.first, one.second.ptr, one.second.size);
            g_Allocator.free(one.second.ptr, Allocator::alignSize(one.second.size));
            break;
        }
    }

    savedValues.clear();
}

void DataSegment::release()
{
}

void DataSegment::makeLinear()
{
    scoped_lock lk(mutex);
    if (buckets.size() == 1)
        return;

    Bucket h;

    h.count  = (uint32_t) g_Allocator.alignSize(totalCount);
    h.size   = h.count;
    h.buffer = (uint8_t*) g_Allocator.alloc(h.count);

    auto ptr = h.buffer;
    for (auto& b : buckets)
    {
        memcpy(ptr, b.buffer, b.count);
        ptr += b.count;
        g_Allocator.free(b.buffer, b.count);
    }

    buckets.clear();
    buckets.push_back(h);
}
