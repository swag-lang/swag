namespace std.collections
{
    using swag, std.diagnostics

    /// Represents an always growable buffer, which is divided in buckets to avoid a copy/realloc when
    /// the buffer becomes too small to hold a given value. This is the main difference with [Array]
    public struct ConcatBuffer
    {
        firstBucket:    *u8
        curBucket:      *u8
        allocator:      IAllocator 

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
                firstBucket, curBucket = newBucket
                ptr := cast(**void) newBucket
                ptr[0] = null
            }
            else
            {
                ptr := cast(**void) curBucket
                ptr[0] = newBucket
                curBucket = newBucket
            }

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