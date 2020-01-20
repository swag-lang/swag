namespace std.text
{
    using swag
    private const SmallSize = 24'u32
    private const SmallSizeInternal = SmallSize - @sizeof(IAllocator)
    #assert @sizeof(String) == 40

    public struct String
    {
        readonly buffer:     *u8
        readonly length:     u32
        readonly capacity:   u32
        internal allocator:  IAllocator
        internal padding:    [SmallSizeInternal] u8
    }
    
    impl String
    {
        func free(using self)
        {
            memory.free(&allocator, buffer)
        }

        func realloc(using self, newCapacity: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            buffer = acast memory.realloc(&allocator, buffer, newCapacity)
        }      
    }

    impl String
    {
        public
        {
            #[swag.inline]
            #[swag.nodoc]
            func opCount(using self)->u32
            {
                return length
            }

            #[swag.nodoc]
            func(T) opCast(using self)->T
            {
                #if @typeof(T) == @typeof(string)
                    return cast(string) {buffer, length}
                #else
                    #assert(false, "invalid cast from 'String' to '" ~ @typeof(T).name ~ "'")
            }

            #[swag.nodoc]
            func opAffect(using self, value: string)
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

            #[swag.nodoc]
            func opEquals(using self, other: string)->bool
            {
                if buffer == @dataof(other)
                    return true
                if length != @countof(other)
                    return false
                return memory.compare(buffer, @dataof(other), length) == 0
            }

            #[swag.nodoc]
            func(op: string) opAssign(using self, value: string)
            {
                #if op == "+="
                    concat(self, value)
                #else
                    #assert(false, "invalid binary operator '" ~ op ~ "'")
            }

            #[swag.nodoc]
            func opPostMove(using self)
            {
                if capacity <= SmallSize
                    buffer = acast &allocator
            }

            #[swag.nodoc]
            func opPostCopy(using self)
            {
                if !buffer
                    return
                if capacity <= SmallSize
                    buffer = acast &allocator
                else
                {
                    oldBuffer := buffer
                    buffer = null
                    reserve(self, length + 1)
                    memory.copy(buffer, oldBuffer, length + 1)
                }
            }

            #[swag.nodoc]
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
            /// Set the length of the [String] to 0
            func clear(using self)
            {
                length = 0
            }

            /// Returns true if the [String] has zero length
            func isEmpty(using self)->bool
            {
                return length == 0
            }

            /// Returns true if the [String] is null (undefined)
            func isNull(using self)->bool
            {
                return buffer == null
            }

            /// Returns true if the [String] is null or empty
            func isNullOrEmpty(using self)->bool
            {
                return buffer == null || length == 0
            }

            /// Concat the [String] with a buffer of bytes
            func concat(using self, value: string)
            {
                concatLength := @countof(value)
                if concatLength == 0
                    return

                newLength := length + concatLength + 1
                ensureCapacity(self, newLength)
                memory.copy(buffer + length, @dataof(value), concatLength)
                length = newLength - 1
                buffer[length] = 0
            }

            /// Ensure the [String] is big enough to store a given amount of bytes
            func ensureCapacity(using self, newLength: u32)
            {
                if newLength <= capacity
                    return
                newCapacity := math.max(capacity * 2, newLength)
                reserve(self, newCapacity)
            }

            /// Reserve room for newCapacity bytes without changing the string length
            func reserve(using self, newCapacity: u32)
            {
                // Small size, just be sure with have a valid buffer address
                if newCapacity <= SmallSize
                {
                    // Free everything
                    if !newCapacity
                    {
                        if !buffer
                            return
                        if buffer != acast &allocator
                            free(self)
                        buffer = null
                        length, capacity = 0
                        return
                    }

                    capacity = SmallSize
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
                    oldBuffer := buffer
                    newAllocator := @getcontext().allocator
                    buffer = acast memory.alloc(&newAllocator, newCapacity)
                    memory.copy(buffer, oldBuffer, length + 1)
                    allocator = newAllocator
                }
                else
                {                    
                    realloc(self, newCapacity)
                }
                
                capacity = newCapacity
            }

            /// Remove whitespaces at the start of the [String]
            func trimLeft(using self)
            {
                if !length
                    return                

                ptr := cast(const *u8) buffer
                while true
                {
                    var (nextPtr, c) = utf8.nextChar(ptr)
                    if !unicode.isWhiteSpace(c)
                        break
                    length -= cast(u32) (nextPtr - ptr)
                    ptr = nextPtr
                }

                memory.copy(buffer, ptr, length)
            }
        }
    }
}
