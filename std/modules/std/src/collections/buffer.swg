namespace std.collections
{
    using swag

    public struct Buffer
    {
        firstBucket:    *u8
        lastBucket:     *u8
        allocator:      IAllocator 

        numBuckets:     u32
        sizeBucket:     u32 = 1024
        count:          u32
    }

    impl Buffer
    {    
        func newBucket(using self)
        {
            if allocator == null
                allocator = @getcontext().allocator
            newBucket := acast memory.alloc(&allocator, sizeBucket + @sizeof(*void))
            if !firstBucket
            {
                firstBucket, lastBucket = newBucket
                ptr := cast(**void) newBucket
                ptr[0] = null
            }
            else
            {
                ptr := cast(**void) lastBucket
                ptr[0] = newBucket
                lastBucket = newBucket
            }

            numBuckets += 1
        }

        public
        {
            #[nodoc]
            func opCount(using self)->u32
            {
                return count
            }

            #[nodoc]
            func opDrop(using self)
            {
                free(self)
            }
        }

        public
        {
            #[nodoc]
            func free(using self)
            {
            }

            /// Clear the content of the buffer
            func clear(using self)
            {
                count = 0
            }
        }
    }
}