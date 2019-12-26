namespace std
{
    using swag
    using diagnostics.debug

    /// Manages a compact array of bit values, which are represented as Booleans, where true indicates that
    /// the bit is on (1) and false indicates the bit is off (0)
    public struct BitArray
    {
        readonly buffer:        *u8
        internal allocator:     IAllocator

        // Maximum number of bits the array can hold
        readonly numBits:       u32
        /// The size, in bytes, of the allocated buffer
        readonly numBytes:      u32
    }

    impl BitArray
    {
        public
        {
            func opCount(using self)->u32
            {
                return numBits
            }
        }

        func free(using self)
        {
            if !buffer
                return
            var req: AllocatorRequest
            req.mode = AllocatorMode.Free
            req.address = buffer
            allocator.alloc(req)
            buffer = null
            numBits, numBytes = 0
        }

        func realloc(using self, newSize: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            var req: AllocatorRequest
            req.mode = AllocatorMode.Realloc
            req.size = newSize
            req.address = buffer
            allocator.alloc(req)
            buffer = acast req.address
        }        

        public
        {
            /// Reserve the given amount of bits
            func reserve(using self, maxBits: u32)
            {
                if !maxBits
                {
                    free(self)
                    return
                }

                numBits = maxBits
                numBytes = maxBits / 8
                if numBytes * 8 < maxBits
                    numBytes += 1

                realloc(self, numBytes)
            }

            /// Reserve the given amount of bits; and set an initial value to all bits
            func reserve(using self, maxBits: u32, initialValue: bool)
            {
                reserve(self, maxBits)
                memory.set(buffer, initialValue ? 0xFF : 0x00, numBytes)
            }   

            /// Sets all bits in the BitArray to the specified value
            func setAll(using self, value: bool)    
            {
                memory.set(buffer, value ? 0xFF : 0x00, numBytes)
            }

            /// Sets the bit at a specific position in the BitArray to the specified value
            func set(using self, index: u32, value: bool)
            {
                if !numBits return

                assert(index < numBits)
                val := index >> 5
                bit := index - (val << 5)
                if value
                    buffer[val] |= cast(u8) (1 << bit)
                else
                    buffer[val] ^= cast(u8) (1 << bit)
            }
        }
    }
}