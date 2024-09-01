#global public

struct ConcatBufferBucket
{
    datas:           ^u8
    next:            *ConcatBufferBucket
    count:           u64
    size:            u64
    countBefore:     u64
}

struct ConcatBufferSeek
{
    bucket:     *ConcatBufferBucket
    sp:         ^u8
}

// Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
// the buffer needs to increase its size. This is the main difference with Array
struct ConcatBuffer
{
    allocator:           Swag.IAllocator

    firstBucket:         *ConcatBufferBucket
    curBucket:           *ConcatBufferBucket
    lastBucket:          *ConcatBufferBucket
    currentSP:           ^u8
    granularity:         u64 = 1024
    isAtEnd:             bool = true
    viewFirstBucket:     ConcatBufferBucket
}

impl ConcatBuffer
{
    mtd opCount()->u64
    {
        return count()
    }

    mtd opDrop()
    {
        release()
    }

    // Visit all valid buckets
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: code)
    {
        if !firstBucket:
            return
        var b = firstBucket
        while b != lastBucket.next
        {
            #macro
            {
                let #alias0 = #up b
                #mixin #up stmt
            }

            b = b.next
        }
    }

    // Returns the number of bytes
    mtd count()->u64
    {
        return lastBucket ? lastBucket.countBefore + lastBucket.count : 0
    }

    // Be sure that there is enough room to store at least 'numBytes' bytes
    mtd grow(numBytes: u64)
    {
        Debug.assert(isAtEnd)

        if lastBucket
        {
            // Enough room in the last bucket
            let seek = cast(u64) (currentSP - lastBucket.datas)
            if seek + numBytes <= lastBucket.size:
                return

            // Not enough room in the last bucket, but with have a bucket after that
            // can old the bytes (because of a previous clear())
            if lastBucket.next and numBytes <= lastBucket.next.size
            {
                lastBucket.next.countBefore = lastBucket.countBefore + lastBucket.count
                lastBucket.next.count       = 0

                curBucket, lastBucket = lastBucket.next
                currentSP = curBucket.datas
                return
            }
        }

        // Need to allocate a new bucket
        if allocator == null:
            allocator = @getcontext().allocator
        let newBucket = Memory.new'ConcatBufferBucket(allocator)

        if lastBucket
        {
            newBucket.next  = lastBucket.next
            lastBucket.next = newBucket
        }
        else:
            firstBucket = newBucket

        // Each bucket knows the total count before it
        if firstBucket and lastBucket == firstBucket:
            newBucket.countBefore = firstBucket.count
        elif lastBucket:
            newBucket.countBefore = lastBucket.countBefore + lastBucket.count

        curBucket, lastBucket = newBucket

        let bsize = Math.max(granularity, numBytes)
        lastBucket.datas = Memory.alloc(bsize, allocator)
        lastBucket.size  = bsize
        currentSP        = lastBucket.datas
    }

    // Associate an allocator with the buffer.
    // The allocator can only be changed if the buffer has no pending buckets.
    mtd setAllocator(alloc: Swag.IAllocator)
    {
        Debug.assert(firstBucket == null, "buffer is not empty")
        allocator = alloc
    }

    // Set the granularity of datas when allocated new buckets. Minimum size is 4.
    mtd setBucketSize(size: u64)
    {
        Debug.assert(size >= 4)
        granularity = size
    }

    // Release all allocated buffers
    mtd release()
    {
        if !firstBucket:
            return

        var ptr = firstBucket
        while ptr
        {
            let nextPtr = ptr.next
            Memory.free(ptr.datas, ptr.size, allocator)
            if ptr != &viewFirstBucket:
                Memory.free(ptr, #sizeof(ConcatBufferBucket), allocator)
            ptr = nextPtr
        }

        firstBucket = null
        clear()
    }

    // Clear the content without freing the buffers
    mtd clear()
    {
        if !firstBucket:
            return

        curBucket, lastBucket = firstBucket
        currentSP = curBucket.datas
        curBucket.countBefore, curBucket.count = 0
        isAtEnd = true
    }

    // Append one byte to the buffer
    mtd(T) addNative(val: T)
    {
        if !isAtEnd
        {
            // If not enough room in the current bucket, then we
            // have to split the content with the next bucket
            if #sizeof(T) > curBucket.count or
               currentSP > curBucket.datas + curBucket.count - #sizeof(T)
            {
                var v = val
                addBytes(@mkslice(cast(^u8) &v, #sizeof(T)), false)
                return
            }

            dref cast(*T) currentSP = val
            currentSP += #sizeof(T)
        }
        else
        {
            grow(#sizeof(T))
            dref cast(*T) currentSP = val
            currentSP += #sizeof(T)
            curBucket.count += #sizeof(T)
        }
    }

    // Append the content of a struct
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        addBytes(@mkslice(cast(const ^u8) &val, #sizeof(T)))
    }

    // Append a slice of bytes to the buffer
    // If 'contiguous' is false, the slice will be divided in chunks if necessary
    mtd addBytes(bytes: const [..] u8, contiguous = true)
    {
        var num = @countof(bytes)
        if !num:
            return

        var slicePtr = @dataof(bytes)

        // Be sure we have a buffer
        if !lastBucket:
            grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            // Overwrite
            if !isAtEnd
            {
                let curCount = cast(u64) (currentSP - curBucket.datas)
                var remain   = curBucket.count - curCount
                while num > remain
                {
                    Memory.copy(currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, currentSP += remain
                    if !num:
                        return

                    // We have reached the end
                    // So we must grow
                    if !curBucket.next
                    {
                        isAtEnd = true
                        grow(num)
                        break
                    }

                    curBucket = curBucket.next
                    currentSP = curBucket.datas
                    remain    = curBucket.count
                }

                Memory.copy(currentSP, slicePtr, num)
                currentSP += num
                if isAtEnd:
                    curBucket.count += num
                else:
                    setIsAtEnd()
                return
            }

            // Append
            else
            {
                let curCount = cast(u64) (currentSP - lastBucket.datas)
                var remain   = lastBucket.size - curCount
                while num > remain
                {
                    Memory.copy(currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, currentSP += remain
                    lastBucket.count += remain
                    if !num:
                        return
                    grow(granularity) // Will alloc a new bucket as the current one is full
                    remain = lastBucket.size
                }
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
        lastBucket.count += num
    }

    // Get the linearized seek offset
    func getOffset(seek: ConcatBufferSeek)->u64
    {
        return seek.bucket ? seek.bucket.countBefore + cast(u64) (seek.sp - seek.bucket.datas) : 0
    }

    // Returns the current 'seek' in the buffer
    mtd getSeek()->ConcatBufferSeek
    {
        var result: retval
        result.bucket = lastBucket
        result.sp     = currentSP
        return result
    }

    // Seek current write pointer
    mtd moveSeek(num: u64)
    {
        var total = num

        if !isAtEnd
        {
            var seek = cast(u64) (currentSP - curBucket.datas)
            while curBucket.next and seek + total > curBucket.count
            {
                total -= (curBucket.count - seek)
                if !curBucket.next:
                    break
                curBucket = curBucket.next
                currentSP = curBucket.datas
                seek      = 0
            }

            if seek + total <= curBucket.count
            {
                currentSP += total
                return
            }

            total -= (curBucket.count - seek)
            isAtEnd = true
        }

        grow(total)
        currentSP += total
        curBucket.count += total
    }

    #[Swag.Inline]
    internal mtd setIsAtEnd()
    {
        isAtEnd = currentSP == lastBucket.datas + lastBucket.count
    }

    // Set the current 'seek'
    mtd setSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            curBucket = firstBucket
            currentSP = curBucket.datas
        }
        else
        {
            curBucket = seek.bucket
            currentSP = seek.sp
        }

        setIsAtEnd()
    }

    // Set the end 'seek'
    mtd setEndSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            curBucket, lastBucket = firstBucket
            currentSP        = curBucket.datas
            lastBucket.count = 0
            isAtEnd          = true
        }
        else
        {
            curBucket, lastBucket = seek.bucket
            currentSP        = seek.sp
            lastBucket.count = cast(u64) (seek.sp - seek.bucket.datas)
            isAtEnd          = true
        }
    }

    // Convert to a slice *only* if the buffer is linear, or return null.
    // See [[ConcatBuffer.makeLinear]]
    mtd toSlice()->[..] u8
    {
        if lastBucket == firstBucket:
            return @mkslice(firstBucket.datas, firstBucket.count)
        return null
    }

    // Share 'data' with the firstBucket
    mtd setFirstBucket(data: [..] u8)
    {
        Debug.assert(firstBucket == null)
        @init(&viewFirstBucket, 1)
        viewFirstBucket.datas = @dataof(data)
        viewFirstBucket.size  = @countof(data)
        firstBucket, lastBucket, curBucket = &viewFirstBucket
        currentSP = firstBucket.datas
    }

    // Convert the buffer to a String.
    // A copy will be made, so the buffer is still valid after the call.
    // See [[ConcatBuffer.moveToString]]
    mtd toString(alloc: Swag.IAllocator = null)->String
    {
        var result: retval
        if !firstBucket:
            return result

        result.allocator = alloc
        result.reserve(count() + 1)

        var ptr = firstBucket
        while ptr != lastBucket.next
        {
            result.append(@mkstring(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }

    internal mtd eatBuffer()->{ data: ^u8, size: u64, capacity: u64 }
    {
        // Must be linear ?
        Debug.assert(lastBucket == firstBucket)

        let rdata     = firstBucket.datas
        let rcount    = firstBucket.count
        let rcapacity = firstBucket.size

        if firstBucket != &viewFirstBucket:
            Memory.free(firstBucket, #sizeof(ConcatBufferBucket), allocator)
        firstBucket = null

        return {rdata, rcount, rcapacity}
    }

    // Move the content of the buffer to the String if possible.
    // If a direct move is not possible because the buffer contains more than on bucket, then a copy will be made instead.
    mtd moveToString()->String
    {
        // Make a copy, as we have more than one bucket
        if lastBucket != firstBucket:
            return toString(allocator)

        // Otherwise we can just eat the buffer
        var eat = eatBuffer()
        with var result: retval
        {
            .allocator = allocator
            .buffer    = eat.data
            .capacity  = eat.capacity
            .length    = eat.size
        }

        return result
    }

    // linearize all buckets in one single big bucket
    mtd makeLinear()
    {
        // Already linear (and only one single bucket)
        if lastBucket == firstBucket:
            return

        let total    = count()
        let newDatas = cast(^u8) Memory.alloc(total, allocator)

        var curPtr = newDatas
        foreach b in dref self
        {
            Memory.copy(curPtr, b.datas, b.count)
            curPtr += b.count
        }

        Debug.assert(curPtr == newDatas + total)
        release()

        // Make one single bucket
        firstBucket       = Memory.new'ConcatBufferBucket(allocator)
        firstBucket.datas = newDatas
        firstBucket.size  = total
        firstBucket.count = total
        curBucket, lastBucket = firstBucket
        currentSP = firstBucket.datas + firstBucket.count
        isAtEnd   = true
    }
}
