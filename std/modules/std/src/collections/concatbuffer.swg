namespace std;

public struct ConcatBufferBucket
{
    datas:  *u8
    next:   *ConcatBufferBucket
    count:  u32
}

// Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
// the buffer needs to increase its size. This is the main difference with Array
public readonly struct ConcatBuffer
{
    allocator:      const swag.IAllocator

    firstBucket:    *ConcatBufferBucket
    lastBucket:     *ConcatBufferBucket
    currentSP:      *u8

    bucketSize:     u32 = 1024
    totalCount:     u32
}

impl ConcatBuffer
{
    func release(using self)
    {
        if !firstBucket
            return
        ptr := firstBucket
        while ptr
        {
            nextPtr := ptr.next
            memory.free(&allocator, ptr.datas)
            memory.free(&allocator, ptr)
            ptr = nextPtr
        }
    }

    func count(using self)->u32
    {
        if !lastBucket
            return 0;
        lastBucket.count = cast(u32) (currentSP - lastBucket.datas)
        return totalCount + lastBucket.count
    }
}

public impl ConcatBuffer
{
    func opCount(using self)->u32
    {
        return self.count()
    }

    func opDrop(using self)
    {
        self.release()
    }

    // Returns the number of elements in the given bucket
    func bucketCount(using self, b: *ConcatBufferBucket)->u32
    {
        if b != lastBucket
            return b.count
        bcount := cast(u32) (currentSP - lastBucket.datas)
        return bcount
    }

    // Be sure that their is enough room to store at least 'numBytes' bytes
    func grow(using self, numBytes: u32)
    {
        debug.assert(numBytes <= bucketSize)

        if firstBucket
        {
            bcount := cast(u32) (currentSP - lastBucket.datas);
            if bcount + numBytes <= bucketSize
                return

            totalCount += bcount;
            lastBucket.count = bcount;

            // Next is already allocated
            if lastBucket.next
            {
                lastBucket = lastBucket.next
                currentSP = lastBucket.datas
                lastBucket.count = 0
                return;
            }
        }

        // Need to allocate a new bucket
        if allocator == null
            allocator = @getcontext().allocator
        newBucket := cast(*ConcatBufferBucket) memory.alloc(&allocator, @sizeof(ConcatBufferBucket))
        @init(newBucket)
        if lastBucket
            lastBucket.next = newBucket
        else
            firstBucket = newBucket
        lastBucket = newBucket

        lastBucket.datas = acast memory.alloc(&allocator, bucketSize)
        currentSP = lastBucket.datas
    }

    // Associate an allocator with the buffer.
    // The allocator can only be changed if the buffer has no pending buckets.
    func setAllocator(using self, alloc: swag.IAllocator)
    {
        debug.assert(firstBucket == null, "bucket is not empty")
        allocator = alloc;
    }

    // Set the size of one given bucket. Minimum size is 4.
    // The buffer will be erased before the change, even if the new size is the same as the current one.
    func setBucketSize(using self, size: u32)
    {
        debug.assert(size >= 4)
        self.free()
        bucketSize = size
    }

    // Release the content of the buffer
    func free(using self)
    {
        @drop(self)
        firstBucket, lastBucket = null
        currentSP = null
        totalCount = 0
    }

    // Clear the content of the buffer, but do not free the memory
    func clear(using self)
    {
        if !firstBucket
            return
        lastBucket = firstBucket
        lastBucket.count = 0
        currentSP = lastBucket.datas
        totalCount = 0
    }

    // Add one byte to the buffer
    func addByte(using self, byte: u8)
    {
        self.grow(1)
        currentSP[0] = byte
        currentSP += 1
    }

    // Add a slice of bytes to the buffer
    // If 'contiguous' is false, the slice will be divided in chunks if necessary
    func addBytes(using self, bytes: const [..] u8, contiguous: bool = true)
    {
        num := @countof(bytes)
        if !num
            return

        slicePtr := @dataof(bytes)

        // Be sure we have a buffer
        if !lastBucket
            self.grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            curCount := cast(u32) (currentSP - lastBucket.datas)
            remain := bucketSize - curCount
            while num > remain
            {
                memory.copy(currentSP, slicePtr, remain)
                num -= remain
                slicePtr += remain
                currentSP += remain
                if !num
                    return
                self.grow(1) // Will alloc a new bucket as the current one is full
                remain = bucketSize
            }
        }
        else
        {
            self.grow(num)
        }

        // We should have enough size in the last bucket to store the
        // rest of the slice
        memory.copy(currentSP, slicePtr, num)
        currentSP += num
    }

    // Convert buffer to a String
    func toString(using self)->String
    {
        var result: String
        if !firstBucket
            return result

        result.reserve(self.count() + 1)

        ptr := firstBucket
        while ptr != lastBucket.next
        {
            result.append(cast(string) @mkslice(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }
}
