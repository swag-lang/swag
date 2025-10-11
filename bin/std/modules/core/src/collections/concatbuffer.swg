#global public

// Represents a single memory block (a bucket) within a ConcatBuffer.
struct ConcatBufferBucket
{
    datas:           [*] u8                  // Pointer to the raw memory of the bucket.
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
    sp:         [*] u8                  // The exact memory address of the position.
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
    currentSP:           [*] u8                  // The current read/write pointer in the `curBucket`.
    granularity:         u64 = 1024              // The default size for newly allocated buckets.
    isAtEnd:             bool = true             // 'true' if the seek pointer is at the very end of the buffer.
    viewFirstBucket:     ConcatBufferBucket      // A pre-allocated bucket used for views, to avoid a separate allocation.
}

impl ConcatBuffer
{
    // Returns the total number of bytes in the buffer.
    mtd opCount()->u64
    {
        return .count()
    }

    // Ensures all allocated memory is freed when the buffer goes out of scope.
    mtd opDrop()
    {
        .release()
    }

    // Provides a way to iterate over each bucket in the buffer.
    // This is a macro that accepts a code block to execute for each bucket.
    // Alias: #alias0 for the bucket pointer.
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !.firstBucket do
            return
        var b = .firstBucket
        while b != .lastBucket.next
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
        return .lastBucket ? .lastBucket.countBefore + .lastBucket.count : 0
    }

    // Ensures that there is enough room to store at least 'numBytes' more bytes.
    // If the current bucket is full, a new one is allocated.
    mtd grow(numBytes: u64)
    {
        Debug.assert(.isAtEnd)

        if .lastBucket
        {
            // Enough room in the last bucket
            let seek = cast(u64) (.currentSP - .lastBucket.datas)
            if seek + numBytes <= .lastBucket.size do
                return

            // Not enough room in the last bucket, but with have a bucket after that
            // can old the bytes (because of a previous clear())
            if .lastBucket.next and numBytes <= .lastBucket.next.size
            {
                .lastBucket.next.countBefore = .lastBucket.countBefore + .lastBucket.count
                .lastBucket.next.count       = 0

                .curBucket, .lastBucket = .lastBucket.next
                .currentSP = .curBucket.datas
                return
            }
        }

        // Need to allocate a new bucket
        if .allocator == null do
            .allocator = @getcontext().allocator
        let newBucket = Memory.new'ConcatBufferBucket(.allocator)

        if .lastBucket
        {
            newBucket.next   = .lastBucket.next
            .lastBucket.next = newBucket
        }
        else do
            .firstBucket = newBucket

        // Each bucket knows the total count before it
        if .firstBucket and .lastBucket == .firstBucket do
            newBucket.countBefore = .firstBucket.count
        elif .lastBucket do
            newBucket.countBefore = .lastBucket.countBefore + .lastBucket.count

        .curBucket, .lastBucket = newBucket

        let bsize = Math.max(.granularity, numBytes)
        .lastBucket.datas = Memory.alloc(bsize, .allocator)
        .lastBucket.size  = bsize
        .currentSP        = .lastBucket.datas
    }

    // Associates a specific allocator with the buffer.
    // This can only be called when the buffer is empty.
    mtd setAllocator(alloc: Swag.IAllocator)
    {
        Debug.assert(.firstBucket == null, "buffer is not empty")
        .allocator = alloc
    }

    // Sets the minimum size for new buckets allocated by the buffer.
    mtd setBucketSize(size: u64)
    {
        Debug.assert(size >= 4)
        .granularity = size
    }

    // Frees all memory associated with the buffer, including all buckets.
    mtd release()
    {
        if !.firstBucket do
            return

        var ptr = .firstBucket
        while ptr
        {
            let nextPtr = ptr.next
            Memory.free(ptr.datas, ptr.size, .allocator)
            if ptr != &.viewFirstBucket do
                Memory.free(ptr, #sizeof(ConcatBufferBucket), .allocator)
            ptr = nextPtr
        }

        .firstBucket = null
        .clear()
    }

    // Clears the buffer's content without deallocating memory.
    // This resets the buffer to be empty, allowing its allocated buckets to be reused.
    mtd clear()
    {
        if !.firstBucket do
            return

        .curBucket, .lastBucket = .firstBucket
        .currentSP = .curBucket.datas
        .curBucket.countBefore, .curBucket.count = 0
        .isAtEnd = true
    }

    // Appends a native type (like u8, s32, f64) to the buffer.
    mtd(T) addNative(val: T)
    {
        if !.isAtEnd
        {
            // If not enough room in the current bucket, then we
            // have to split the content with the next bucket
            if #sizeof(T) > .curBucket.count or
               .currentSP > .curBucket.datas + .curBucket.count - #sizeof(T)
            {
                var v = val
                .addBytes(@mkslice(cast([*] u8) &v, #sizeof(T)), false)
                return
            }

            dref cast(*T) .currentSP = val
            .currentSP += #sizeof(T)
        }
        else
        {
            .grow(#sizeof(T))
            dref cast(*T) .currentSP = val
            .currentSP += #sizeof(T)
            .curBucket.count += #sizeof(T)
        }
    }

    // Appends the raw byte content of a struct to the buffer.
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        .addBytes(@mkslice(cast(const [*] u8) &val, #sizeof(T)))
    }

    // Appends a slice of bytes to the buffer.
    // If 'contiguous' is false, the slice can be split across multiple buckets if necessary.
    mtd addBytes(bytes: #null const [..] u8, contiguous = true)
    {
        var num = @countof(bytes)
        if !num do
            return

        var slicePtr = @dataof(bytes)

        // Be sure we have a buffer
        if !.lastBucket do
            .grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            // Overwrite
            if !.isAtEnd
            {
                let curCount = cast(u64) (.currentSP - .curBucket.datas)
                var remain   = .curBucket.count - curCount
                while num > remain
                {
                    Memory.copy(.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, .currentSP += remain
                    if !num do
                        return

                    // We have reached the end
                    // So we must grow
                    if !.curBucket.next
                    {
                        .isAtEnd = true
                        .grow(num)
                        break
                    }

                    .curBucket = .curBucket.next
                    .currentSP = .curBucket.datas
                    remain     = .curBucket.count
                }

                Memory.copy(.currentSP, slicePtr, num)
                .currentSP += num
                if .isAtEnd do
                    .curBucket.count += num
                else do
                    .setIsAtEnd()
                return
            }

            // Append
            else
            {
                let curCount = cast(u64) (.currentSP - .lastBucket.datas)
                var remain   = .lastBucket.size - curCount
                while num > remain
                {
                    Memory.copy(.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, .currentSP += remain
                    .lastBucket.count += remain
                    if !num do
                        return
                    .grow(.granularity) // Will alloc a new bucket as the current one is full
                    remain = .lastBucket.size
                }
            }
        }
        else
        {
            .grow(num)
        }

        // We should have enough size in the last bucket to store the
        // rest of the slice
        Memory.copy(.currentSP, slicePtr, num)
        .currentSP += num
        .lastBucket.count += num
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
        result.bucket = .lastBucket
        result.sp     = .currentSP
        return result
    }

    // Moves the current seek pointer forward by 'num' bytes.
    mtd moveSeek(num: u64)
    {
        var total = num

        if !.isAtEnd
        {
            var seek = cast(u64) (.currentSP - .curBucket.datas)
            while .curBucket.next and seek + total > .curBucket.count
            {
                total -= (.curBucket.count - seek)
                if !.curBucket.next do
                    break
                .curBucket = .curBucket.next
                .currentSP = .curBucket.datas
                seek       = 0
            }

            if seek + total <= .curBucket.count
            {
                .currentSP += total
                return
            }

            total -= (.curBucket.count - seek)
            .isAtEnd = true
        }

        .grow(total)
        .currentSP += total
        .curBucket.count += total
    }

    #[Swag.Inline]
    internal mtd setIsAtEnd()
    {
        .isAtEnd = .currentSP == .lastBucket.datas + .lastBucket.count
    }

    // Sets the current read/write position to a given 'seek'.
    mtd setSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            .curBucket = .firstBucket
            .currentSP = .curBucket.datas
        }
        else
        {
            .curBucket = seek.bucket
            .currentSP = seek.sp
        }

        .setIsAtEnd()
    }

    // Sets the end of the buffer to a given 'seek' position, effectively truncating it.
    mtd setEndSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            .curBucket, .lastBucket = .firstBucket
            .currentSP        = .curBucket.datas
            .lastBucket.count = 0
            .isAtEnd          = true
        }
        else
        {
            .curBucket, .lastBucket = seek.bucket
            .currentSP        = seek.sp
            .lastBucket.count = cast(u64) (seek.sp - seek.bucket.datas)
            .isAtEnd          = true
        }
    }

    // Returns the buffer's content as a slice, but only if the buffer is 'linear'.
    // A buffer is linear if it consists of a single bucket. Returns 'null' otherwise.
    // See [[makeLinear]].
    mtd toSlice()->#null [..] u8
    {
        if .lastBucket == .firstBucket do
            return @mkslice(.firstBucket.datas, .firstBucket.count)
        return null
    }

    // Initializes the buffer to use an external, existing slice of data as its first bucket.
    // This is a "view" mode; the buffer does not own or free this data.
    mtd setFirstBucket(data: [..] u8)
    {
        Debug.assert(.firstBucket == null)
        @init(&.viewFirstBucket, 1)
        .viewFirstBucket.datas = @dataof(data)
        .viewFirstBucket.size  = @countof(data)
        .firstBucket, .lastBucket, .curBucket = &.viewFirstBucket
        .currentSP = .firstBucket.datas
    }

    // Converts the buffer to a 'String' by copying all its content.
    // The buffer remains valid after this call.
    // See [[moveToString]] for a more efficient version that avoids copying when possible.
    mtd const toString(alloc: Swag.IAllocator = null)->String
    {
        var result: retval
        if !.firstBucket do
            return result

        result.allocator = alloc
        result.reserve(.count() + 1)

        var ptr = .firstBucket
        while ptr != .lastBucket.next
        {
            result.append(@mkstring(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }

    internal mtd eatBuffer()->{ data: [*] u8, size: u64, capacity: u64 }
    {
        // Must be linear ?
        Debug.assert(.lastBucket == .firstBucket)

        let rdata     = .firstBucket.datas
        let rcount    = .firstBucket.count
        let rcapacity = .firstBucket.size

        if .firstBucket != &.viewFirstBucket do
            Memory.free(.firstBucket, #sizeof(ConcatBufferBucket), .allocator)
        .firstBucket = null

        return {rdata, rcount, rcapacity}
    }

    // Moves the buffer's content into a new 'String', invalidating the buffer.
    // If the buffer is linear (one bucket), this is a fast operation that avoids copying.
    // Otherwise, it falls back to copying the data.
    mtd moveToString()->String
    {
        // Make a copy, as we have more than one bucket
        if .lastBucket != .firstBucket do
            return .toString(.allocator)

        // Otherwise we can just eat the buffer
        let eat = .eatBuffer()
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
        if .lastBucket == .firstBucket do
            return

        let total    = .count()
        let newDatas = cast([*] u8) Memory.alloc(total, .allocator)

        var curPtr = newDatas
        foreach b in dref me
        {
            Memory.copy(curPtr, b.datas, b.count)
            curPtr += b.count
        }

        Debug.assert(curPtr == newDatas + total)
        .release()

        // Make one single bucket
        .firstBucket       = Memory.new'ConcatBufferBucket(.allocator)
        .firstBucket.datas = newDatas
        .firstBucket.size  = total
        .firstBucket.count = total
        .curBucket, .lastBucket = .firstBucket
        .currentSP = .firstBucket.datas + .firstBucket.count
        .isAtEnd   = true
    }
}
