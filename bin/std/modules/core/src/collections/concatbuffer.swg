#global marked
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
        return self.count()
    }

    // Ensures all allocated memory is freed when the buffer goes out of scope.
    mtd opDrop()
    {
        self.release()
    }

    // Provides a way to iterate over each bucket in the buffer.
    // This is a macro that accepts a code block to execute for each bucket.
    // Alias: #alias0 for the bucket pointer.
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) const opVisit(stmt: #code void)
    {
        if !self.firstBucket:
            return
        var b = self.firstBucket
        while b != self.lastBucket.next
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
    mtd count()->u64
    {
        return self.lastBucket ? self.lastBucket.countBefore + self.lastBucket.count : 0
    }

    // Ensures that there is enough room to store at least 'numBytes' more bytes.
    // If the current bucket is full, a new one is allocated.
    mtd grow(numBytes: u64)
    {
        Debug.assert(self.isAtEnd)

        if self.lastBucket
        {
            // Enough room in the last bucket
            let seek = cast(u64) (self.currentSP - self.lastBucket.datas)
            if seek + numBytes <= self.lastBucket.size:
                return

            // Not enough room in the last bucket, but with have a bucket after that
            // can old the bytes (because of a previous clear())
            if self.lastBucket.next and numBytes <= self.lastBucket.next.size
            {
                self.lastBucket.next.countBefore = self.lastBucket.countBefore + self.lastBucket.count
                self.lastBucket.next.count       = 0

                self.curBucket, self.lastBucket = self.lastBucket.next
                self.currentSP = self.curBucket.datas
                return
            }
        }

        // Need to allocate a new bucket
        if self.allocator == null:
            self.allocator = @getcontext().allocator
        let newBucket = Memory.new'ConcatBufferBucket(self.allocator)

        if self.lastBucket
        {
            newBucket.next  = self.lastBucket.next
            self.lastBucket.next = newBucket
        }
        else:
            self.firstBucket = newBucket

        // Each bucket knows the total count before it
        if self.firstBucket and self.lastBucket == self.firstBucket:
            newBucket.countBefore = self.firstBucket.count
        elif self.lastBucket:
            newBucket.countBefore = self.lastBucket.countBefore + self.lastBucket.count

        self.curBucket, self.lastBucket = newBucket

        let bsize = Math.max(self.granularity, numBytes)
        self.lastBucket.datas = Memory.alloc(bsize, self.allocator)
        self.lastBucket.size  = bsize
        self.currentSP        = self.lastBucket.datas
    }

    // Associates a specific allocator with the buffer.
    // This can only be called when the buffer is empty.
    mtd setAllocator(alloc: Swag.IAllocator)
    {
        Debug.assert(self.firstBucket == null, "buffer is not empty")
        self.allocator = alloc
    }

    // Sets the minimum size for new buckets allocated by the buffer.
    mtd setBucketSize(size: u64)
    {
        Debug.assert(size >= 4)
        self.granularity = size
    }

    // Frees all memory associated with the buffer, including all buckets.
    mtd release()
    {
        if !self.firstBucket:
            return

        var ptr = self.firstBucket
        while ptr
        {
            let nextPtr = ptr.next
            Memory.free(ptr.datas, ptr.size, self.allocator)
            if ptr != &self.viewFirstBucket:
                Memory.free(ptr, #sizeof(ConcatBufferBucket), self.allocator)
            ptr = nextPtr
        }

        self.firstBucket = null
        self.clear()
    }

    // Clears the buffer's content without deallocating memory.
    // This resets the buffer to be empty, allowing its allocated buckets to be reused.
    mtd clear()
    {
        if !self.firstBucket:
            return

        self.curBucket, self.lastBucket = self.firstBucket
        self.currentSP = self.curBucket.datas
        self.curBucket.countBefore, self.curBucket.count = 0
        self.isAtEnd = true
    }

    // Appends a native type (like u8, s32, f64) to the buffer.
    mtd(T) addNative(val: T)
    {
        if !self.isAtEnd
        {
            // If not enough room in the current bucket, then we
            // have to split the content with the next bucket
            if #sizeof(T) > self.curBucket.count or
               self.currentSP > self.curBucket.datas + self.curBucket.count - #sizeof(T)
            {
                var v = val
                self.addBytes(@mkslice(cast(^u8) &v, #sizeof(T)), false)
                return
            }

            dref cast(*T) self.currentSP = val
            self.currentSP += #sizeof(T)
        }
        else
        {
            self.grow(#sizeof(T))
            dref cast(*T) self.currentSP = val
            self.currentSP += #sizeof(T)
            self.curBucket.count += #sizeof(T)
        }
    }

    // Appends the raw byte content of a struct to the buffer.
    #[Swag.Inline]
    mtd(T) addStruct(val: T)
    {
        self.addBytes(@mkslice(cast(const ^u8) &val, #sizeof(T)))
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
        if !self.lastBucket:
            self.grow(1)

        // Divide the slice in the given amount of buckets if necessary
        if !contiguous
        {
            // Overwrite
            if !self.isAtEnd
            {
                let curCount = cast(u64) (self.currentSP - self.curBucket.datas)
                var remain   = self.curBucket.count - curCount
                while num > remain
                {
                    Memory.copy(self.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, self.currentSP += remain
                    if !num:
                        return

                    // We have reached the end
                    // So we must grow
                    if !self.curBucket.next
                    {
                        self.isAtEnd = true
                        self.grow(num)
                        break
                    }

                    self.curBucket = self.curBucket.next
                    self.currentSP = self.curBucket.datas
                    remain    = self.curBucket.count
                }

                Memory.copy(self.currentSP, slicePtr, num)
                self.currentSP += num
                if self.isAtEnd:
                    self.curBucket.count += num
                else:
                    self.setIsAtEnd()
                return
            }

            // Append
            else
            {
                let curCount = cast(u64) (self.currentSP - self.lastBucket.datas)
                var remain   = self.lastBucket.size - curCount
                while num > remain
                {
                    Memory.copy(self.currentSP, slicePtr, remain)
                    num -= remain
                    slicePtr, self.currentSP += remain
                    self.lastBucket.count += remain
                    if !num:
                        return
                    self.grow(self.granularity) // Will alloc a new bucket as the current one is full
                    remain = self.lastBucket.size
                }
            }
        }
        else
        {
            self.grow(num)
        }

        // We should have enough size in the last bucket to store the
        // rest of the slice
        Memory.copy(self.currentSP, slicePtr, num)
        self.currentSP += num
        self.lastBucket.count += num
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
        result.bucket = self.lastBucket
        result.sp     = self.currentSP
        return result
    }

    // Moves the current seek pointer forward by 'num' bytes.
    mtd moveSeek(num: u64)
    {
        var total = num

        if !self.isAtEnd
        {
            var seek = cast(u64) (self.currentSP - self.curBucket.datas)
            while self.curBucket.next and seek + total > self.curBucket.count
            {
                total -= (self.curBucket.count - seek)
                if !self.curBucket.next:
                    break
                self.curBucket = self.curBucket.next
                self.currentSP = self.curBucket.datas
                seek      = 0
            }

            if seek + total <= self.curBucket.count
            {
                self.currentSP += total
                return
            }

            total -= (self.curBucket.count - seek)
            self.isAtEnd = true
        }

        self.grow(total)
        self.currentSP += total
        self.curBucket.count += total
    }

    #[Swag.Inline]
    internal mtd setIsAtEnd()
    {
        self.isAtEnd = self.currentSP == self.lastBucket.datas + self.lastBucket.count
    }

    // Sets the current read/write position to a given 'seek'.
    mtd setSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            self.curBucket = self.firstBucket
            self.currentSP = self.curBucket.datas
        }
        else
        {
            self.curBucket = seek.bucket
            self.currentSP = seek.sp
        }

        self.setIsAtEnd()
    }

    // Sets the end of the buffer to a given 'seek' position, effectively truncating it.
    mtd setEndSeek(seek: ConcatBufferSeek)
    {
        if !seek.bucket
        {
            self.curBucket, self.lastBucket = self.firstBucket
            self.currentSP        = self.curBucket.datas
            self.lastBucket.count = 0
            self.isAtEnd          = true
        }
        else
        {
            self.curBucket, self.lastBucket = seek.bucket
            self.currentSP        = seek.sp
            self.lastBucket.count = cast(u64) (seek.sp - seek.bucket.datas)
            self.isAtEnd          = true
        }
    }

    // Returns the buffer's content as a slice, but only if the buffer is 'linear'.
    // A buffer is linear if it consists of a single bucket. Returns 'null' otherwise.
    // See [[makeLinear]].
    mtd toSlice()->#null [..] u8
    {
        if self.lastBucket == self.firstBucket:
            return @mkslice(self.firstBucket.datas, self.firstBucket.count)
        return null
    }

    // Initializes the buffer to use an external, existing slice of data as its first bucket.
    // This is a "view" mode; the buffer does not own or free this data.
    mtd setFirstBucket(data: [..] u8)
    {
        Debug.assert(self.firstBucket == null)
        @init(&self.viewFirstBucket, 1)
        self.viewFirstBucket.datas = @dataof(data)
        self.viewFirstBucket.size  = @countof(data)
        self.firstBucket, self.lastBucket, self.curBucket = &self.viewFirstBucket
        self.currentSP = self.firstBucket.datas
    }

    // Converts the buffer to a 'String' by copying all its content.
    // The buffer remains valid after this call.
    // See [[moveToString]] for a more efficient version that avoids copying when possible.
    mtd toString(alloc: Swag.IAllocator = null)->String
    {
        var result: retval
        if !self.firstBucket:
            return result

        result.allocator = alloc
        result.reserve(self.count() + 1)

        var ptr = self.firstBucket
        while ptr != self.lastBucket.next
        {
            result.append(@mkstring(ptr.datas, ptr.count))
            ptr = ptr.next
        }

        return result
    }

    internal mtd eatBuffer()->{ data: ^u8, size: u64, capacity: u64 }
    {
        // Must be linear ?
        Debug.assert(self.lastBucket == self.firstBucket)

        let rdata     = self.firstBucket.datas
        let rcount    = self.firstBucket.count
        let rcapacity = self.firstBucket.size

        if self.firstBucket != &self.viewFirstBucket:
            Memory.free(self.firstBucket, #sizeof(ConcatBufferBucket), self.allocator)
        self.firstBucket = null

        return {rdata, rcount, rcapacity}
    }

    // Moves the buffer's content into a new 'String', invalidating the buffer.
    // If the buffer is linear (one bucket), this is a fast operation that avoids copying.
    // Otherwise, it falls back to copying the data.
    mtd moveToString()->String
    {
        // Make a copy, as we have more than one bucket
        if self.lastBucket != self.firstBucket:
            return self.toString(self.allocator)

        // Otherwise we can just eat the buffer
        var eat = self.eatBuffer()
        var result: retval
        result.allocator = self.allocator
        result.buffer    = eat.data
        result.capacity  = eat.capacity
        result.length    = eat.size
        return result
    }

    // Merges all buckets into a single, contiguous memory block.
    // After this call, the buffer is guaranteed to be linear.
    mtd makeLinear()
    {
        // Already linear (and only one single bucket)
        if self.lastBucket == self.firstBucket:
            return

        let total    = self.count()
        let newDatas = cast(^u8) Memory.alloc(total, self.allocator)

        var curPtr = newDatas
        foreach b in dref self
        {
            Memory.copy(curPtr, b.datas, b.count)
            curPtr += b.count
        }

        Debug.assert(curPtr == newDatas + total)
        self.release()

        // Make one single bucket
        self.firstBucket       = Memory.new'ConcatBufferBucket(self.allocator)
        self.firstBucket.datas = newDatas
        self.firstBucket.size  = total
        self.firstBucket.count = total
        self.curBucket, self.lastBucket = self.firstBucket
        self.currentSP = self.firstBucket.datas + self.firstBucket.count
        self.isAtEnd   = true
    }
}
