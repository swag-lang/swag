namespace std.collections
{
    using swag, std.diagnostics

    /// Represents a growable buffer, which is divided in buckets to avoid a copy/realloc when
    /// the buffer needs to increase its size. This is the main difference with [Array]
    public readonly struct ConcatBuffer
    {
        allocator:      const IAllocator 
        
        firstBucket:    *u8
        curBucket:      *u8

        totalBuckets:   u32
        usedBuckets:    u32
        curCount:       u32

        sizeBucket:     u32 = 1024
    }

    impl ConcatBuffer
    {    
        func growBuckets(using self, size: u32)
        {
            if curBucket != null && curCount + size + @sizeof(*void) < sizeBucket
                return

            debug.assert(size + @sizeof(*void) <= sizeBucket)
            curCount = 0

            // No need to allocate a new bucket if we are not already using them all
            if usedBuckets < totalBuckets
            {
                ptr := cast(**void) curBucket
                curBucket = acast ptr[0]
                debug.assert(curBucket != null)
                usedBuckets += 1
                return;
            }

            if allocator == null
                allocator = @getcontext().allocator
            newBucket := acast memory.alloc(&allocator, sizeBucket)
            if !firstBucket
            {
                firstBucket = newBucket
            }
            else
            {
                ptr := cast(**void) curBucket
                ptr[0] = newBucket
            }

            curBucket = newBucket
            newPtr := cast(**void) newBucket
            newPtr[0] = null

            usedBuckets += 1
            totalBuckets += 1
        }

        func release(using self)
        {  
            if firstBucket == null
                return
            ptr := cast(*void) firstBucket
            while ptr != null
            {                    
                nextPtr := cast(**void) ptr
                nextPtrValue := nextPtr[0]        
                memory.free(&allocator, ptr)
                ptr = nextPtrValue
            }
        }

        public
        {
            #[nodoc]
            func opCount(using self)->u32
            {
                if curBucket == null
                    return 0
                total := ((usedBuckets - 1) * (sizeBucket - @sizeof(*void)))
                total += curCount
                return total
            }

            #[nodoc]
            func opDrop(using self)
            {
                release(self)
            }
        }

        public
        {
            /// Associate an allocator with the buffer.
            ///
            /// The allocator can only be changed if the buffer has no pending buckets.
            func setAllocator(using self, alloc: IAllocator)
            {
                diagnostics.debug.assert(firstBucket == null, "bucket is not empty")
                allocator = alloc;
            }

            /// Set the size of one given bucket. Minimal size is 16.
            ///
            /// The buffer will be erased before the change, even if the new size is the same as the current one.
            func setBucketSize(using self, size: u32)
            {
                diagnostics.debug.assert(size >= 16)
                free(self)
                sizeBucket = size
            }

            /// Release the content of the buffer
            func free(using self)
            {
                drop(self)
                curCount, usedBuckets, totalBuckets = 0
                firstBucket, curBucket = null
            }

            /// Clear the content of the buffer
            func clear(using self)
            {
                curCount = 0
                curBucket = firstBucket
                usedBuckets = curBucket ? 1 : 0
            }

            /// Add one byte to the buffer
            func addByte(using self, value: u8)
            {
                growBuckets(self, 1)
                ptr := curBucket + curCount + @sizeof(*void)
                ptr[0] = value
                curCount += 1
            }
        }
    }
}