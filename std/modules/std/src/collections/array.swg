namespace std.collections
{
    using swag

    public struct(T) Array
    {
        buffer:     *T
        allocator:  IAllocator

        /// Number of valid elements in the [Array]
        count:      u32
        /// Maximim size, in bytes, of the buffer
        capacity:   u32
    }

    impl Array
    {
        func free(using self)
        {
            memory.free(allocator, buffer)
        }

        func realloc(using self, newCapacity: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            buffer = acast memory.realloc(allocator, buffer, newCapacity)
        }
                
        public
        {
            func reserve(using self, newCapacity: u32)
            {
                if !newCapacity
                {
                    free(self)
                    buffer = null
                    count, capacity = 0
                    return
                }
            }
        }
    }
}