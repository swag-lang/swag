#global public

struct ConcatBufferBucket
{
    datas:  *u8
    next:   *ConcatBufferBucket
    count:  uint
    size:   uint
}

// Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
// the buffer needs to increase its size. This is the main difference with Array
struct ConcatBuffer
{
    allocator:      Swag.IAllocator

    firstBucket:    *ConcatBufferBucket
    lastBucket:     *ConcatBufferBucket
    currentSP:      *u8

    granularity:    uint = 1024
    totalCount:     uint
}

impl ConcatBuffer
{
    private mtd release()
    {
        if !firstBucket
            return
        ptr := firstBucket
        while ptr
        {
            nextPtr := ptr.next
            Memory.free(ptr.datas, ptr.size, allocator)
            Memory.free(ptr, @sizeof(ConcatBufferBucket), allocator)
            ptr = nextPtr
        }
    }

    mtd opCount()->uint
    {
        return count()
    }

    mtd opDrop()
    {
        release()
    }

    // Returns the number of bytes
    mtd count()->uint
    {
        if !lastBucket
            return 0;
        lastBucket.count = cast(uint) (currentSP - lastBucket.datas)
        return totalCount + lastBucket.count
    }

    // Returns the number of elements in the given bucket
    func bucketCount(using const self, b: *ConcatBufferBucket)->uint
    {
        if b != lastBucket
            return b.count
        return cast(uint) (currentSP - lastBucket.datas)
    }

    // Be sure that there is enough room to store at least 'numBytes' bytes
    mtd grow(numBytes: uint)
    {
        if lastBucket
        {
            bcount := cast(uint) (currentSP - lastBucket.datas);
            if bcount + numBytes <= lastBucket.size
                return

            totalCount += bcount
            lastBucket.count = bcount
        }

        // Need to allocate a new bucket
        if allocator == null
            allocator = @getcontext().allocator
        newBucket := cast(*ConcatBufferBucket) Memory.alloc(cast(uint) @sizeof(ConcatBufferBucket), allocator)
        @init(newBucket)
        if lastBucket
            lastBucket.next = newBucket
        else
            firstBucket = newBucket
        lastBucket = newBucket

        bsize := Math.max(granularity, numBytes)
        lastBucket.datas = Memory.alloc(bsize, allocator)
        lastBucket.size  = bsize
        currentSP = lastBucket.datas
    }

    // Associate an allocator with the buffer.
    // The allocator can only be changed if the buffer has no pending buckets.
    mtd setAllocator(alloc: Swag.IAllocator)
    {
        Debug.assert(firstBucket == null, "buffer is not empty")
        allocator = alloc;
    }

    // Set the granularity of datas of a given bucket. Minimum size is 4.
    // The buffer will be erased before the change, even if the new size is the same as the current one.
    mtd setBucketSize(size: uint)
    {
        Debug.assert(size >= 4)
        clear()
        granularity = size
    }

    // Release the content of the buffer
    mtd clear()
    {
        @drop(self)
        firstBucket, lastBucket = null
        currentSP = null
        totalCount = 0
    }

    // Append one byte to the buffer
    mtd addByte(byte: u8)
    {
        grow(1)
        currentSP[0] = byte
        currentSP += 1
    }

    // Append a slice of bytes to the buffer
    // If 'contiguous' is false, the slice will be divided in chunks if necessary
    mtd addBytes(bytes: const [..] u8, contiguous = true)
    {
        num := @countof(bytes)
        if !num
            return

        slicePtr := @dataof(bytes)

        // Be sure we have a buffer
        if !lastBucket
            grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            curCount := cast(uint) (currentSP - lastBucket.datas)
            remain := lastBucket.size - curCount
            while num > remain
            {
                Memory.copy(currentSP, slicePtr, remain)
                num -= remain
                slicePtr += remain
                currentSP += remain
                if !num
                    return
                grow(granularity) // Will alloc a new bucket as the current one is full
                remain = lastBucket.size
            }
        }
        else
        {
            grow(num)
        }

        // We should have enough size in the last bucket to store the
        // rest of the slice
        Memory.copy(currentSP, slicePtr, num)
        currentSP += num
    }

    // Append the content of a struct
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        addBytes(@mkslice(cast(const *u8) &val, @sizeof(T)))
    }

    // Convert buffer to a String
    mtd toString()->String
    {
        var result: retval
        if !firstBucket
            return result

        result.reserve(count() + 1)

        ptr := firstBucket
        while ptr != lastBucket.next
        {
            result.append(@mkstring(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }

    // Convert buffer to a String by moving the content if possible
    // After the call, the buffer will be empty if the,move can be done
    // (otherwise, the result will contain a copy)
    mtd moveToString()->String
    {
        if !firstBucket
        {
            var result: retval
            return result
        }

        if firstBucket == lastBucket
        {
            var result: retval

            // Move buffer
            result.allocator = allocator
            result.buffer    = firstBucket.datas
            result.capacity  = firstBucket.size
            result.length    = cast(uint) (currentSP - lastBucket.datas)

            // And clear me
            Memory.free(firstBucket, @sizeof(ConcatBufferBucket), allocator)
            firstBucket, lastBucket = null
            currentSP = null
            totalCount = 0

            return result
        }

        return toString()
    }
}
