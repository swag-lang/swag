#global public

struct ConcatBufferBucket
{
    datas:          *u8
    next:           *ConcatBufferBucket
    count:          uint
    size:           uint
    countBefore:    uint
}

struct ConcatBufferSeek
{
    bucket: *ConcatBufferBucket
    sp:     *u8
}

// Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
// the buffer needs to increase its size. This is the main difference with Array
struct ConcatBuffer
{
    allocator:      Swag.IAllocator

    firstBucket:    *ConcatBufferBucket
    curBucket:      *ConcatBufferBucket
    lastBucket:     *ConcatBufferBucket
    currentSP:      *u8
    granularity:    uint = 1024
    isAtEnd:        bool = true
}

impl ConcatBuffer
{
    mtd opCount()->uint
    {
        return count()
    }

    mtd opDrop()
    {
        release()
    }

    // Visit all valid buckets
    #[Swag.Macro]
    mtdc(ptr: bool) opVisit(stmt: code)
    {
        if !firstBucket
            return
        b := firstBucket
        while b != lastBucket.next
        {
            #macro
            {
                var @alias0 = `b
                #mixin stmt
            }

            b = b.next
        }
    }

    // Returns the number of bytes
    mtd count()->uint
    {
        return lastBucket ? lastBucket.countBefore + lastBucket.count : 0
    }

    // Be sure that there is enough room to store at least 'numBytes' bytes
    mtd grow(numBytes: uint)
    {
        // We have seek inside the buffer, so we must not change
        // the size of the current bucket
        total := numBytes
        if !isAtEnd
        {
            seek := cast(uint) (currentSP - curBucket.datas)
            b := curBucket
            while seek + total > b.count
            {
                if !b.next
                    break
                total -= (b.count - seek)
                b = b.next
                seek = 0
            }

            Debug.assert(seek + total <= b.count)
            return
        }

        if lastBucket
        {
            seek := cast(uint) (currentSP - lastBucket.datas)
            if seek + total <= lastBucket.size
                return
        }

        // Need to allocate a new bucket
        if allocator == null
            allocator = @getcontext().allocator
        newBucket := Memory.new'ConcatBufferBucket(allocator)
        if lastBucket
            lastBucket.next = newBucket
        else
            firstBucket = newBucket

        // Each bucket knows the total count before it
        if firstBucket and lastBucket == firstBucket
            newBucket.countBefore = firstBucket.count
        elif lastBucket
            newBucket.countBefore = lastBucket.countBefore + lastBucket.count

        curBucket, lastBucket = newBucket

        bsize := Math.max(granularity, total)
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

    // Release all allocated buffers
    mtd release()
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

        firstBucket = null
        clear()
    }

    // Clear the content without freing the buffers
    mtd clear()
    {
        if !firstBucket
            return

        curBucket, lastBucket = firstBucket
        currentSP = curBucket.datas
        curBucket.count = 0
        isAtEnd = true
    }

    // Append one byte to the buffer
    mtd(T) addNative(val: T)
    {
        if !isAtEnd
        {
            if currentSP == curBucket.datas + curBucket.count
            {
                curBucket = curBucket.next
                currentSP = curBucket.datas
            }

            dref cast(*T) currentSP = val
            currentSP += @sizeof(val)
        }
        else
        {
            grow(@sizeof(val))
            dref cast(*T) currentSP = val
            currentSP += @sizeof(val)
            curBucket.count += @sizeof(val)
        }
    }

    // Append the content of a struct
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        addBytes(@mkslice(cast(const *u8) &val, @sizeof(T)))
    }

    // Append a slice of bytes to the buffer
    // If 'contiguous' is false, the slice will be divided in chunks if necessary
    mtd addBytes(bytes: const [..] u8, contiguous = true)
    {
        Debug.assert(isAtEnd)

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
                lastBucket.count += remain
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
        lastBucket.count += num
    }

    // Get the linearized seek offset
    mtd getOffset(seek: ConcatBufferSeek)->uint
    {
        return seek.bucket ? seek.bucket.countBefore + cast(uint) (seek.sp - seek.bucket.datas) : 0
    }

    // Returns the current 'seek' in the buffer
    mtd getSeek()->ConcatBufferSeek
    {
        var result: retval
        result.bucket = lastBucket
        result.sp = currentSP
        return result
    }

    // Seek current write pointer
    mtd moveSeek(num: uint)
    {
        total := num

        if !isAtEnd
        {
            seek := cast(uint) (currentSP - curBucket.datas)
            while curBucket.next and seek + total > curBucket.count
            {
                total -= (curBucket.count - seek)
                if !curBucket.next
                    break
                curBucket = curBucket.next
                currentSP = curBucket.datas
                seek = 0
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

        isAtEnd = currentSP == lastBucket.datas + lastBucket.count
    }

    // Set the end 'seek'
    mtd setEndSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            curBucket, lastBucket = firstBucket
            currentSP = curBucket.datas
            lastBucket.count = 0
            isAtEnd = true

        }
        else
        {
            curBucket, lastBucket = seek.bucket
            currentSP  = seek.sp
            lastBucket.count = cast(uint) (seek.sp - seek.bucket.datas)
            isAtEnd = true
        }
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

    // Convert to a slice *only* if the buffer is linear (see 'makeLinear')
    mtd toSlice()->[..] u8
    {
        if lastBucket == firstBucket
            return @mkslice(firstBucket.datas, firstBucket.count)
        return null
    }

    // linearize all buckets in one single big bucket
    mtd makeLinear()
    {
        // Already linear (and only one single bucket)
        if lastBucket == firstBucket
            return

        total := count()
        newDatas := Memory.alloc(total, allocator)

        curPtr := newDatas
        visit b: dref self
        {
            Memory.copy(curPtr, b.datas, b.count)
            curPtr += b.count
        }

        Debug.assert(curPtr == newDatas + total)
        release()

        // Make one single bucket
        firstBucket = Memory.new'ConcatBufferBucket(allocator)
        firstBucket.datas = newDatas
        firstBucket.size  = total
        firstBucket.count = total
        curBucket, lastBucket = firstBucket
        currentSP = firstBucket.datas + firstBucket.count
        isAtEnd = true
    }
}
