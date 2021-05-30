#include "pch.h"
#include "Global.h"
#include "DataSegment.h"
#include "Log.h"
#include "SourceFile.h"
#include "Module.h"
#include "Runtime.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "AstNode.h"
#include "ByteCode.h"

static const uint32_t BUCKET_SIZE = 16 * 1024;

void DataSegment::setup(SegmentKind _kind, Module* _module)
{
    kind = _kind;
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
    case SegmentKind::Type:
        name = "type segment";
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
    module = _module;
}

uint32_t DataSegment::reserve(uint32_t size, uint32_t alignOf)
{
    scoped_lock lock(mutex);
    return reserveNoLock(size, alignOf);
}

void DataSegment::initFrom(DataSegment* other)
{
    if (other->totalCount)
    {
        reserve(other->totalCount);
        memcpy(buckets[0].buffer, other->buckets[0].buffer, other->totalCount);
    }

    initPtr.clear();
    initFuncPtr.clear();
    for (auto& it : other->initPtr)
        addInitPtr(it.patchOffset, it.srcOffset, it.fromSegment);
    for (auto& it : other->initFuncPtr)
        addInitPtrFunc(it.first, it.second.first, it.second.second);

    for (auto& it : other->mapString)
        mapString[it.first] = it.second;

    for (auto& it : other->storedValues8)
        storedValues8[it.first] = it.second;
    for (auto& it : other->storedValues16)
        storedValues16[it.first] = it.second;
    for (auto& it : other->storedValues32)
        storedValues32[it.first] = it.second;
    for (auto& it : other->storedValues64)
        storedValues64[it.first] = it.second;
}

uint32_t DataSegment::reserveNoLock(TypeInfo* typeInfo)
{
    return reserveNoLock(typeInfo->sizeOf, TypeManager::alignOf(typeInfo));
}

void DataSegment::align(uint32_t alignOf)
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

uint32_t DataSegment::reserveNoLock(uint32_t size, uint32_t alignOf)
{
    align(alignOf);
    return reserveNoLock(size);
}

uint32_t DataSegment::reserveNoLock(uint32_t size)
{
    SWAG_RACE_CONDITION_WRITE(raceC);

    SWAG_ASSERT(size);
    DataSegmentHeader* last = nullptr;
    if (buckets.size())
        last = &buckets.back();

    if (last && last->count + size <= last->size)
    {
        last->count += size;
    }
    else
    {
        DataSegmentHeader bucket;
        bucket.size   = max(size, BUCKET_SIZE);
        bucket.size   = (uint32_t) Allocator::alignSize(bucket.size);
        bucket.buffer = (uint8_t*) g_Allocator.alloc(bucket.size);
        if (g_CommandLine.stats)
            g_Stats.memSeg += bucket.size;
        memset(bucket.buffer, 0, bucket.size);
        bucket.count = size;
        buckets.push_back(bucket);
    }

    // Check that we do not overflow maximum size
    if ((uint64_t) totalCount + size > SWAG_LIMIT_SEGMENT)
    {
        if (!overflow)
        {
            overflow = true;
            module->error(format(Msg0834, name));
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

uint8_t* DataSegment::address(uint32_t location)
{
    shared_lock lock(mutex);
    return addressNoLock(location);
}

uint8_t* DataSegment::addressNoLock(uint32_t location)
{
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

uint32_t DataSegment::addComputedValueNoLock(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue)
{
    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
    SWAG_ASSERT(typeInfo->nativeType != NativeTypeKind::Any);

    if (typeInfo->nativeType == NativeTypeKind::String)
    {
        auto stringOffset     = addStringNoLock(computedValue.text);
        auto storageOffset    = reserveNoLock(2 * sizeof(uint64_t));
        auto addr             = addressNoLock(storageOffset);
        ((uint64_t*) addr)[0] = (uint64_t) computedValue.text.c_str();
        ((uint64_t*) addr)[1] = (uint32_t) computedValue.text.length();
        addInitPtr(storageOffset, stringOffset);
        return storageOffset;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
    {
        auto it = storedValues8.find(computedValue.reg.u8);
        if (it != storedValues8.end())
            return it->second;
        break;
    }

    case 2:
    {
        auto it = storedValues16.find(computedValue.reg.u16);
        if (it != storedValues16.end())
            return it->second;
        break;
    }

    case 4:
    {
        auto it = storedValues32.find(computedValue.reg.u32);
        if (it != storedValues32.end())
            return it->second;
        break;
    }
    case 8:
    {
        auto it = storedValues64.find(computedValue.reg.u64);
        if (it != storedValues64.end())
            return it->second;
        break;
    }
    }

    auto storageOffset = reserveNoLock(typeInfo->sizeOf);
    auto addr          = addressNoLock(storageOffset);
    switch (typeInfo->sizeOf)
    {
    case 1:
        *(uint8_t*) addr                    = computedValue.reg.u8;
        storedValues8[computedValue.reg.u8] = storageOffset;
        break;
    case 2:
        *(uint16_t*) addr                     = computedValue.reg.u16;
        storedValues16[computedValue.reg.u16] = storageOffset;
        break;
    case 4:
        *(uint32_t*) addr                     = computedValue.reg.u32;
        storedValues32[computedValue.reg.u32] = storageOffset;
        break;
    case 8:
        *(uint64_t*) addr                     = computedValue.reg.u64;
        storedValues64[computedValue.reg.u64] = storageOffset;
        break;
    }

    return storageOffset;
}

uint32_t DataSegment::addStringNoLock(const Utf8& str)
{
    // Same string already there ?
    auto it = mapString.find(str);
    if (it != mapString.end())
        return it->second;

    auto strLen = (uint32_t) str.length() + 1;
    auto offset = reserveNoLock(strLen);
    auto addr   = addressNoLock(offset);
    memcpy(addr, str.c_str(), strLen);
    mapString[str] = offset;

    return offset;
}

uint32_t DataSegment::addString(const Utf8& str)
{
    scoped_lock lk(mutex);
    return addStringNoLock(str);
}

void DataSegment::addPatchPtr(int64_t* addr, int64_t value)
{
    if (kind == SegmentKind::Compiler)
        return;

    SegmentPatchPtrRef st;
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
    unique_lock lk(mutexPatchMethod);
    patchMethods.push_back({funcDecl, storageOffset});
}

void DataSegment::doPatchMethods(JobContext* context)
{
    unique_lock lk(mutexPatchMethod);
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
            addInitPtrFunc(it.second, bc->callName(), DataSegment::RelocType::Local);
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

    SegmentInitPtrRef ref;
    ref.patchOffset = patchOffset;
    ref.srcOffset   = srcOffset;
    ref.fromSegment = seg;

    if (g_CommandLine.devMode)
    {
        // We must have at least one pointer difference with all other offsets, otherwise we will
        // have a weird memory overwrite
        for (int i = 0; i < initPtr.size(); i++)
            SWAG_ASSERT(abs((int) initPtr[i].patchOffset - (int) patchOffset) >= 8);
    }

    scoped_lock lk(mutexPtr);
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
    unique_lock lk(mutex);
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
    unique_lock lk(mutex);
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
    unique_lock lk(mutex);
    if (buckets.size() == 1)
        return;

    DataSegmentHeader h;

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
