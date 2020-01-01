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

            func opDrop(using self)
            {
                free(self)
            }

            func opPostCopy(using self)
            {
                if(buffer)
                {
                    otherBuffer := buffer
                    buffer = null
                    reserve(self, numBits)
                    memory.copy(buffer, otherBuffer, numBytes)
                }
            }

            func opIndex(using self, index: u32)->bool
            {
                return get(self, index)
            }

            func(op: string) opIndexAssign(using self, value: bool, index: u32)
            {
                #if op == "="
                    set(self, index, value)
                #else
                    #assert(false, "invalid assignment operator '" ~op~ "'")
            }            

            func opAffect(using self, slice: [4] bool)
            {
                reserve(self, @countof(slice))
                visit b, i: slice
                    set(self, i, b)
            }
        }

        func free(using self)
        {
            memory.free(allocator, buffer)
        }

        func realloc(using self, newSize: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator
            buffer = acast memory.realloc(allocator, buffer, newSize)
        }        

        public
        {
            /// Reserve the given amount of bits
            func reserve(using self, maxBits: u32)
            {
                if !maxBits
                {
                    free(self)
                    buffer = null
                    numBits, numBytes = 0
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
                assert(index < numBits)
                val := index >> 3
                bit := index - (val << 3)
                let shift: u8 = cast(u8) (1 << bit)
                if value
                    buffer[val] |= shift
                else
                    buffer[val] &= ~shift
            }

            /// Gets the value of the bit at a specific position in the BitArray
            func get(using self, index: u32)->bool
            {
                assert(index < numBits)
                val := index >> 3
                bit := index - (val << 3)
                let shift: u8 = cast(u8) (1 << bit)
                return cast(bool) (buffer[val] & shift)
            }

            /// Inverts the value of the bit at a specific position in the BitArray
            func invert(using self, index: u32)
            {
                assert(index < numBits)
                val := index >> 3
                bit := index - (val << 3)
                let shift: u8 = cast(u8) (1 << bit)
                if buffer[val] & shift
                    buffer[val] ^= shift
                else
                    buffer[val] |= shift
            }       
        }
    }
}