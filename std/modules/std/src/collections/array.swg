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
            func opCount(using self)->u32
            {
                return count
            }

            #[swag.nodoc]
            func opDrop(using self)
            {
                free(self)
            }
        }

        public
        {
            #[swag.nodoc]
            func free(using self)
            {
                if !buffer
                    return

                // Need to drop every structs
                #if reflection.hasMember(@typeof(T), "opDrop")
                {
                    loop count
                    {
                        buffer[count].opDrop()
                    }
                }

                memory.free(&allocator, buffer)
            }

            #[swag.nodoc]
            func realloc(using self, newCapacity: u32)
            {
                if allocator == null
                    allocator = @getcontext().allocator

                newBuffer := acast memory.realloc(&allocator, buffer, newCapacity * @sizeof(T))

                // New to call opPostMove on every structs if the buffer has not been reallocated
                // in place
                #if reflection.hasMember(@typeof(T), "opPostMove")
                {
                    if buffer && newBuffer != buffer
                    {
                        loop count
                            buffer[count].opPostMove()
                    }
                }

                buffer = newBuffer
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

            /// Add one element at the end of the [Array]
            func addBack(using self, value: T)
            {
                reserve(self, count + 1)
                buffer[count] = nodrop T
                count += 1
            }

            /// Add one element at the end of the [Array]
            func emplaceBack(using self, value: T)
            {
                reserve(self, count + 1)
                buffer[count] = nodrop move T
                count += 1
            }            
        }
    }
}