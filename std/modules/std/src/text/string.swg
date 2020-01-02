namespace std
{
    using swag
    private const smallSize = 24'u32
    private const smallSizeInternal = smallSize - @sizeof(IAllocator)
    #assert @sizeof(String) == 40

    public struct String
    {
        readonly buffer:     *u8
        readonly length:     u32
        readonly capacity:   u32
        internal allocator:  IAllocator
        internal padding:    [smallSizeInternal] u8
    }
    
    impl String
    {
        func free(using self)
        {
            memory.free(allocator, buffer, capacity)
        }

        func realloc(using self, newCapacity: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            buffer = acast memory.realloc(allocator, buffer, newCapacity)
        }
    }

    impl String
    {
        public
        {
            #[swag.inline]
            func opCount(using self)->u32
            {
                return length
            }

            func(T) opCast(using self)->T
            {
                #if @typeof(T) == @typeof(const [..] u8)
                    return [buffer, length]
                #else
                    #assert(false, "invalid cast from 'String' to '" ~ @typeof(T).name ~ "'")
            }

            func opAffect(using self, value: [..] u8)
            {
                if @dataof(value) == null
                {
                    reserve(self, 0)
                    return
                }

                newCapacity := @countof(value) + 1
                length = newCapacity - 1
                reserve(self, newCapacity)
                memory.copy(buffer, @dataof(value), length)
                buffer[length] = 0
            }

            func opEquals(using self, other: [..] u8)->bool
            {
                if buffer == @dataof(other)
                    return true
                if length != @countof(other)
                    return false
                return memory.compare(buffer, @dataof(other), length) == 0
            }

            func(op: string) opAssign(using self, value: [..] u8)
            {
                #if op == "+="
                    concat(self, value)
                #else
                    #assert(false, "invalid binary operator '" ~ op ~ "'")
            }

            func opPostMove(using self)
            {
                if capacity <= smallSize
                    buffer = acast &allocator
            }

            func opPostCopy(using self)
            {
                if !buffer
                    return
                if capacity <= smallSize
                    buffer = acast &allocator
                else
                {
                    oldBuffer := buffer
                    buffer = null
                    reserve(self, length + 1)
                    memory.copy(buffer, oldBuffer, length + 1)
                }
            }

            func opDrop(using self)
            {
                if buffer && buffer != acast &allocator
                    free(self)
            }
        }
    }

    impl String
    {
        public
        {
            /// Set the length of the string to 0
            func clear(using self)
            {
                length = 0
            }

            /// Returns true if the string has zero length
            func isEmpty(using self)->bool
            {
                return length == 0
            }

            /// Returns true if the string is null (undefined)
            func isNull(using self)->bool
            {
                return buffer == null
            }

            /// Returns true if the string is null or empty
            func isNullOrEmpty(using self)->bool
            {
                return buffer == null || length == 0
            }

            /// Concat string with a buffer of bytes
            func concat(using self, value: [..] u8)
            {
                if @countof(value) == 0
                    return
                newCapacity := length + @countof(value) + 1
                reserve(self, newCapacity)
                memory.copy(buffer + length, @dataof(value), @countof(value))
                length = newCapacity - 1
                buffer[length] = 0
            }

            /// Reserve room for newCapacity byte without changing the string length
            func reserve(using self, newCapacity: u32)
            {
                // Small size, just be sure with have a valid buffer address
                if newCapacity <= smallSize
                {
                    // Free everything
                    capacity = newCapacity
                    if !capacity
                    {
                        if !buffer
                            return
                        if buffer != acast &allocator
                            free(self)
                        buffer = null
                        length = 0
                        return
                    }

                    if !buffer
                        buffer = acast &allocator
                    return
                }

                // Actual size is fine
                if newCapacity <= capacity
                    return

                // We were in small size mode, and we are no more
                if buffer == acast &allocator
                {
                    buffer = null
                    allocator = null
                }

                realloc(self, newCapacity)
                capacity = newCapacity
            }
        }
    }
}
