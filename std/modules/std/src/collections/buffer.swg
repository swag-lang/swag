namespace std.collections
{
    using swag, std.collections

    public struct Buffer
    {
        buckets:    Array'(*bool)
        allocator:  IAllocator

        count:      u32
        capacity:   u32
        bucketSize: u32 = 1024
    }

    impl Buffer
    {    
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
                loop i: buckets
                {
                    memory.free(&allocator, cast(*void) buckets[i])
                    //buckets[i] = null
                }
            }

            /// Clear the content of the buffer
            func clear(using self)
            {
                buckets.clear()
                count = 0
            }
        }
    }
}