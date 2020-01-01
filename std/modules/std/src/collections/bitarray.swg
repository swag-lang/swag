namespace std
{
    using swag
    using diagnostics.debug

    /// Manages a compact array of bit values, which are represented as booleans, where true indicates that
    /// the bit is on (1) and false indicates the bit is off (0)
    public struct BitArray
    {
        readonly buffer:        *u32
        internal allocator:     IAllocator

        /// Maximum number of bits the array can hold
        readonly numBits:       u32
        /// The size, in bytes, of the allocated buffer
        readonly numBytes:      u32
    }

    impl BitArray
    {
        public
        {
            #[swag.nodoc]
            func opCount(using self)->u32
            {
                return numBits
            }

            #[swag.nodoc]
            func opDrop(using self)
            {
                free(self)
            }

            #[swag.nodoc]
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

            /// Gets the value of the bit at a specific position in the BitArray
            func opIndex(using self, index: u32)->bool
            {
                return get(self, index)
            }

            /// Sets the value of the bit at a specific position in the BitArray
            func(op: string) opIndexAssign(using self, value: bool, index: u32)
            {
                #if op == "="
                    set(self, index, value)
                #else
                    #assert(false, "invalid assignment operator '" ~op~ "'")
            }            

            /// Initializes a BitArray that contains bit values copied from the specified array of booleans.
            func opAffect(using self, slice: [..] bool)
            {
                reserve(self, @countof(slice))
                visit value, index: slice
                    set(self, index, value)
            }

            #[swag.macro]
            func(ptr: bool) opVisit(using self, stmt: code)
            {
                if !numBits return
                #assert(ptr == false, "pointer visitor not supported for BitArray")
                loop i: math.max(numBits >> 5, 1)
                {
                    var shift: u32 = 1
                    loop 32
                    {
                        #macro
                        {
                            let @alias0 = cast(bool) (buffer[i] & shift)
                            #mixin `stmt
                        }   
                    }
                }
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
                numBytes = maxBits >> 5
                if numBytes << 5 < maxBits
                    numBytes += 1

                numBytes *= 4
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
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                if value
                    buffer[val] |= shift
                else
                    buffer[val] &= ~shift
            }

            /// Gets the value of the bit at a specific position in the BitArray
            func get(using self, index: u32)->bool
            {
                assert(index < numBits)
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                return cast(bool) (buffer[val] & shift)
            }

            /// Inverts the value of the bit at a specific position in the BitArray
            func invert(using self, index: u32)
            {
                assert(index < numBits)
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                if buffer[val] & shift
                    buffer[val] ^= shift
                else
                    buffer[val] |= shift
            }

            /// Performs the bitwise AND operation between the elements of the current BitArray and the corresponding elements in the specified array. 
            /// The current BitArray will be modified to store the result of the bitwise AND operation.
            func and(using self, other: Self)
            {
                diagnostics.debug.assert(numBits == other.numBits)
                if !numBits return
                loop math.max(numBits >> 5, 1)
                    buffer[@index] &= other.buffer[@index]
            }

            /// Performs the bitwise OR operation between the elements of the current BitArray and the corresponding elements in the specified array. 
            /// The current BitArray will be modified to store the result of the bitwise OR operation.
            func or(using self, other: Self)
            {
                diagnostics.debug.assert(numBits == other.numBits)
                if !numBits return
                loop math.max(numBits >> 5, 1)
                    buffer[@index] |= other.buffer[@index]
            }            

            /// Performs the bitwise XOR operation between the elements of the current BitArray and the corresponding elements in the specified array. 
            /// The current BitArray will be modified to store the result of the bitwise XOR operation.
            func xor(using self, other: Self)
            {
                diagnostics.debug.assert(numBits == other.numBits)
                if !numBits return
                loop math.max(numBits >> 5, 1)
                    buffer[@index] ^= other.buffer[@index]
            }            

            /// Inverts all the bit values in the current BitArray, so that elements set to true are changed to false, and elements set to false are changed to true.
            func not(using self)
            {
                if !numBits return
                loop math.max(numBits >> 5, 1)
                    buffer[@index] = ~buffer[@index]
            }              
        }
    }
}