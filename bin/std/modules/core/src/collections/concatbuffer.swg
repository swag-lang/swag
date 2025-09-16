#global public

// Represents a single memory block (a bucket) within a ConcatBuffer.
struct ConcatBufferBucket
{
    datas:           ^u8                     // Pointer to the raw memory of the bucket.
    next:            *ConcatBufferBucket     // Pointer to the next bucket in the chain.
    count:           u64                     // Number of bytes used in this bucket.
    size:            u64                     // Total allocated size of this bucket in bytes.
    countBefore:     u64                     // Total number of bytes in all preceding buckets.
}

// Represents a position (seek) within a ConcatBuffer.
// It holds both the bucket and the specific pointer within that bucket.
struct ConcatBufferSeek
{
    bucket:     *ConcatBufferBucket     // The bucket where the position is located.
    sp:         ^u8                     // The exact memory address of the position.
}

// Represents a growable buffer that is divided into buckets (chunks of memory).
// This structure avoids large reallocations and copies when the buffer needs to grow,
// making it efficient for building data piece by piece.
struct ConcatBuffer
{
    allocator:           Swag.IAllocator         // The allocator to use for all memory operations.

    firstBucket:         *ConcatBufferBucket     // The first bucket in the chain.
    curBucket:           *ConcatBufferBucket     // The bucket where the current seek pointer is located.
    lastBucket:          *ConcatBufferBucket     // The last bucket in the chain.
    currentSP:           ^u8                     // The current read/write pointer in the `curBucket`.
    granularity:         u64 = 1024              // The default size for newly allocated buckets.
    isAtEnd:             bool = true             // 'true' if the seek pointer is at the very end of the buffer.
    viewFirstBucket:     ConcatBufferBucket      // A pre-allocated bucket used for views, to avoid a separate allocation.
}

impl ConcatBuffer
{
    // Returns the total number of bytes in the buffer.
    mtd opCount()->u64
    {
        return me.count()
    }

    // Ensures all allocated memory is freed when the buffer goes out of scope.
    mtd opDrop()
    {
        me.release()
    }

    // Provides a way to iterate over each bucket in the buffer.
    // This is a macro that accepts a code block to execute for each bucket.
    // Alias: #alias0 for the bucket pointer.
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !me.firstBucket:
            return
        var b = me.firstBucket
        while b != me.lastBucket.next
        {
            #macro
            {
                let #alias0 = #up b
                #inject(#up stmt)
            }

            b = b.next
        }
    }

    // Returns the total number of bytes currently stored in the buffer.
    mtd const count()->u64
    {
        return me.lastBucket ? me.lastBucket.countBefore + me.lastBucket.count : 0
    }

    // Ensures that there is enough room to store at least 'numBytes' more bytes.
    // If the current bucket is full, a new one is allocated.
    mtd grow(numBytes: u64)
    {
        Debug.assert(me.isAtEnd)

        if me.lastBucket
        {
            // Enough room in the last bucket
            let seek = cast(u64) (me.currentSP - me.lastBucket.datas)
            if seek + numBytes <= me.lastBucket.size:
                return

            // Not enough room in the last bucket, but with have a bucket after that
            // can old the bytes (because of a previous clear())
            if me.lastBucket.next and numBytes <= me.lastBucket.next.size
            {
                me.lastBucket.next.countBefore = me.lastBucket.countBefore + me.lastBucket.count
                me.lastBucket.next.count       = 0

                me.curBucket, me.lastBucket = me.lastBucket.next
                me.currentSP = me.curBucket.datas
                return
            }
        }

        // Need to allocate a new bucket
        if me.allocator == null:
            me.allocator = @getcontext().allocator
        let newBucket = Memory.new'ConcatBufferBucket(me.allocator)

        if me.lastBucket
        {
            newBucket.next     = me.lastBucket.next
            me.lastBucket.next = newBucket
        }
        else:
            me.firstBucket = newBucket

        // Each bucket knows the total count before it
        if me.firstBucket and me.lastBucket == me.firstBucket:
            newBucket.countBefore = me.firstBucket.count
        elif me.lastBucket:
            newBucket.countBefore = me.lastBucket.countBefore + me.lastBucket.count

        me.curBucket, me.lastBucket = newBucket

        let bsize = Math.max(me.granularity, numBytes)
        me.lastBucket.datas = Memory.alloc(bsize, me.allocator)
        me.lastBucket.size  = bsize
        me.currentSP        = me.lastBucket.datas
    }

    // Associates a specific allocator with the buffer.
    // This can only be called when the buffer is empty.
    mtd setAllocator(alloc: Swag.IAllocator)
    {
        Debug.assert(me.firstBucket == null, "buffer is not empty")
        me.allocator = alloc
    }

    // Sets the minimum size for new buckets allocated by the buffer.
    mtd setBucketSize(size: u64)
    {
        Debug.assert(size >= 4)
        me.granularity = size
    }

    // Frees all memory associated with the buffer, including all buckets.
    mtd release()
    {
        if !me.firstBucket:
            return

        var ptr = me.firstBucket
        while ptr
        {
            let nextPtr = ptr.next
            Memory.free(ptr.datas, ptr.size, me.allocator)
            if ptr != &me.viewFirstBucket:
                Memory.free(ptr, #sizeof(ConcatBufferBucket), me.allocator)
            ptr = nextPtr
        }

        me.firstBucket = null
        me.clear()
    }

    // Clears the buffer's content without deallocating memory.
    // This resets the buffer to be empty, allowing its allocated buckets to be reused.
    mtd clear()
    {
        if !me.firstBucket:
            return

        me.curBucket, me.lastBucket = me.firstBucket
        me.currentSP = me.curBucket.datas
        me.curBucket.countBefore, me.curBucket.count = 0
        me.isAtEnd = true
    }

    // Appends a native type (like u8, s32, f64) to the buffer.
    mtd(T) addNative(val: T)
    {
        if !me.isAtEnd
        {
            // If not enough room in the current bucket, then we
            // have to split the content with the next bucket
            if #sizeof(T) > me.curBucket.count or
               me.currentSP > me.curBucket.datas + me.curBucket.count - #sizeof(T)
            {
                var v = val
                me.addBytes(@mkslice(cast(^u8) &v, #sizeof(T)), false)
                return
            }

            dref cast(*T) me.currentSP = val
            me.currentSP += #sizeof(T)
        }
        else
        {
            me.grow(#sizeof(T))
            dref cast(*T) me.currentSP = val
            me.currentSP += #sizeof(T)
            me.curBucket.count += #sizeof(T)
        }
    }

    // Appends the raw byte content of a struct to the buffer.
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        me.addBytes(@mkslice(cast(const ^u8) &val, #sizeof(T)))
    }

    // Appends a slice of bytes to the buffer.
    // If 'contiguous' is false, the slice can be split across multiple buckets if necessary.
    mtd addBytes(bytes: #null const [..] u8, contiguous = true)
    {
        var num = @countof(bytes)
        if !num:
            return

        var slicePtr = @dataof(bytes)

        // Be sure we have a buffer
        if !me.lastBucket:
            me.grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            // Overwrite
            if !me.isAtEnd
            {
                let curCount = cast(u64) (me.currentSP - me.curBucket.datas)
                var remain   = me.curBucket.count - curCount
                while num > remain
                {
                    Memory.copy(me.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, me.currentSP += remain
                    if !num:
                        return

                    // We have reached the end
                    // So we must grow
                    if !me.curBucket.next
                    {
                        me.isAtEnd = true
                        me.grow(num)
                        break
                    }

                    me.curBucket = me.curBucket.next
                    me.currentSP = me.curBucket.datas
                    remain       = me.curBucket.count
                }

                Memory.copy(me.currentSP, slicePtr, num)
                me.currentSP += num
                if me.isAtEnd:
                    me.curBucket.count += num
                else:
                    me.setIsAtEnd()
                return
            }

            // Append
            else
            {
                let curCount = cast(u64) (me.currentSP - me.lastBucket.datas)
                var remain   = me.lastBucket.size - curCount
                while num > remain
                {
                    Memory.copy(me.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, me.currentSP += remain
                    me.lastBucket.count += remain
                    if !num:
                        return
                    me.grow(me.granularity) // Will alloc a new bucket as the current one is full
                    remain = me.lastBucket.size
                }
            }
        }
        else
        {
            me.grow(num)
        }

        // We should have enough size in the last bucket to store the
        // rest of the slice
        Memory.copy(me.currentSP, slicePtr, num)
        me.currentSP += num
        me.lastBucket.count += num
    }

    // Calculates the absolute byte offset of a given seek position.
    func getOffset(seek: ConcatBufferSeek)->u64
    {
        return seek.bucket ? seek.bucket.countBefore + cast(u64) (seek.sp - seek.bucket.datas) : 0
    }

    // Returns a 'ConcatBufferSeek' representing the current position in the buffer.
    mtd getSeek()->ConcatBufferSeek
    {
        var result: retval
        result.bucket = me.lastBucket
        result.sp     = me.currentSP
        return result
    }

    // Moves the current seek pointer forward by 'num' bytes.
    mtd moveSeek(num: u64)
    {
        var total = num

        if !me.isAtEnd
        {
            var seek = cast(u64) (me.currentSP - me.curBucket.datas)
            while me.curBucket.next and seek + total > me.curBucket.count
            {
                total -= (me.curBucket.count - seek)
                if !me.curBucket.next:
                    break
                me.curBucket = me.curBucket.next
                me.currentSP = me.curBucket.datas
                seek         = 0
            }

            if seek + total <= me.curBucket.count
            {
                me.currentSP += total
                return
            }

            total -= (me.curBucket.count - seek)
            me.isAtEnd = true
        }

        me.grow(total)
        me.currentSP += total
        me.curBucket.count += total
    }

    #[Swag.Inline]
    internal mtd setIsAtEnd()
    {
        me.isAtEnd = me.currentSP == me.lastBucket.datas + me.lastBucket.count
    }

    // Sets the current read/write position to a given 'seek'.
    mtd setSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            me.curBucket = me.firstBucket
            me.currentSP = me.curBucket.datas
        }
        else
        {
            me.curBucket = seek.bucket
            me.currentSP = seek.sp
        }

        me.setIsAtEnd()
    }

    // Sets the end of the buffer to a given 'seek' position, effectively truncating it.
    mtd setEndSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            me.curBucket, me.lastBucket = me.firstBucket
            me.currentSP        = me.curBucket.datas
            me.lastBucket.count = 0
            me.isAtEnd          = true
        }
        else
        {
            me.curBucket, me.lastBucket = seek.bucket
            me.currentSP        = seek.sp
            me.lastBucket.count = cast(u64) (seek.sp - seek.bucket.datas)
            me.isAtEnd          = true
        }
    }

    // Returns the buffer's content as a slice, but only if the buffer is 'linear'.
    // A buffer is linear if it consists of a single bucket. Returns 'null' otherwise.
    // See [[makeLinear]].
    mtd toSlice()->#null [..] u8
    {
        if me.lastBucket == me.firstBucket:
            return @mkslice(me.firstBucket.datas, me.firstBucket.count)
        return null
    }

    // Initializes the buffer to use an external, existing slice of data as its first bucket.
    // This is a "view" mode; the buffer does not own or free this data.
    mtd setFirstBucket(data: [..] u8)
    {
        Debug.assert(me.firstBucket == null)
        @init(&me.viewFirstBucket, 1)
        me.viewFirstBucket.datas = @dataof(data)
        me.viewFirstBucket.size  = @countof(data)
        me.firstBucket, me.lastBucket, me.curBucket = &me.viewFirstBucket
        me.currentSP = me.firstBucket.datas
    }

    // Converts the buffer to a 'String' by copying all its content.
    // The buffer remains valid after this call.
    // See [[moveToString]] for a more efficient version that avoids copying when possible.
    mtd const toString(alloc: Swag.IAllocator = null)->String
    {
        var result: retval
        if !me.firstBucket:
            return result

        result.allocator = alloc
        result.reserve(me.count() + 1)

        var ptr = me.firstBucket
        while ptr != me.lastBucket.next
        {
            result.append(@mkstring(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }

    internal mtd eatBuffer()->{ data: ^u8, size: u64, capacity: u64 }
    {
        // Must be linear ?
        Debug.assert(me.lastBucket == me.firstBucket)

        let rdata     = me.firstBucket.datas
        let rcount    = me.firstBucket.count
        let rcapacity = me.firstBucket.size

        if me.firstBucket != &me.viewFirstBucket:
            Memory.free(me.firstBucket, #sizeof(ConcatBufferBucket), me.allocator)
        me.firstBucket = null

        return {rdata, rcount, rcapacity}
    }

    // Moves the buffer's content into a new 'String', invalidating the buffer.
    // If the buffer is linear (one bucket), this is a fast operation that avoids copying.
    // Otherwise, it falls back to copying the data.
    mtd moveToString()->String
    {
        // Make a copy, as we have more than one bucket
        if me.lastBucket != me.firstBucket:
            return me.toString(me.allocator)

        // Otherwise we can just eat the buffer
        let eat = me.eatBuffer()
        with var result: retval
        {
            .allocator = me.allocator
            .buffer    = eat.data
            .capacity  = eat.capacity
            .length    = eat.size
        }

        return result
    }

    // Merges all buckets into a single, contiguous memory block.
    // After this call, the buffer is guaranteed to be linear.
    mtd makeLinear()
    {
        // Already linear (and only one single bucket)
        if me.lastBucket == me.firstBucket:
            return

        let total    = me.count()
        let newDatas = cast(^u8) Memory.alloc(total, me.allocator)

        var curPtr = newDatas
        foreach b in dref me
        {
            Memory.copy(curPtr, b.datas, b.count)
            curPtr += b.count
        }

        Debug.assert(curPtr == newDatas + total)
        me.release()

        // Make one single bucket
        me.firstBucket       = Memory.new'ConcatBufferBucket(me.allocator)
        me.firstBucket.datas = newDatas
        me.firstBucket.size  = total
        me.firstBucket.count = total
        me.curBucket, me.lastBucket = me.firstBucket
        me.currentSP = me.firstBucket.datas + me.firstBucket.count
        me.isAtEnd   = true
    }
}
