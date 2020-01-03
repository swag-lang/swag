namespace std.collections
{
    using swag

    public struct(T) Array
    {
        buffer:     *T
        allocator:  IAllocator

        /// Number of valid elements in the [Array]
        count:      u32
        /// Maximim elements the [Array] can store
        capacity:   u32
    }

    impl Array
    {               
        public
        {
            #[swag.nodoc]
            func free(using self)
            {
                memory.free(allocator, buffer)
            }

            #[swag.nodoc]
            func realloc(using self, newCapacity: u32)
            {
                if allocator == null
                    allocator = @getcontext().allocator
                buffer = acast memory.realloc(allocator, buffer, newCapacity * @sizeof(T))
                capacity = newCapacity
            }

            /// Ensure the [Array] is big enough to store at least `newCount` elements
            func ensureCapacity(using self, newCount : u32)
            {
                if newCount <= capacity
                    return
                newCapacity := math.max(capacity * 2, newCount)
                reserve(self, newCapacity)
            }

            /// Reserve room for `newCapacity` elements without changing the [Array] count
            func reserve(using self, newCapacity: u32)
            {
                if !newCapacity
                {
                    free(self)
                    buffer = null
                    count, capacity = 0
                    return
                }

                if newCapacity <= capacity
                    return

                realloc(self, newCapacity)
            }
        }
    }
}