namespace std.collections
{
    using swag
    using diagnostics.debug

    /// Manages a compact array of bit values, which are represented as booleans, where `true` indicates that
    /// the bit is on (1) and `false` indicates the bit is off (0)
    public struct BitArray
    {
        buffer:     *u32
        allocator:  IAllocator

        /// Maximum number of bits the [BitArray] can hold
        count:      u32
        /// The size, in bytes, of the allocated buffer
        capacity:   u32
    }

    impl BitArray
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

            #[swag.nodoc]
            func opPostCopy(using self)
            {
                if(buffer)
                {
                    otherBuffer := buffer
                    buffer = null
                    reserve(self, count)
                    memory.copy(buffer, otherBuffer, capacity)
                }
            }

            /// Gets the value of the bit at a specific position in the [BitArray]
            func opIndex(using self, index: u32)->bool
            {
                return get(self, index)
            }

            /// Sets the value of the bit at a specific position in the [BitArray]
            func(op: string) opIndexAssign(using self, value: bool, index: u32)
            {
                #if op == "="
                    set(self, index, value)
                #else
                    #assert(false, "invalid assignment operator '" ~op~ "'")
            }            

            /// Initializes a [BitArray] that contains bit values copied from the specified array of booleans.
            func opAffect(using self, slice: [..] bool)
            {
                len := @countof(slice)
                reserve(self, len)
                buffer[len >> 5] = 0
                visit value, index: slice
                    set(self, index, value)
            }

            /// Compares two [BitArray], and returns `true` if they are equal
            func opEquals(using self, other: Self)->bool
            {
                if count != other.count
                    return false

                loop math.max(count >> 5, 1)
                {
                    if buffer[@index] != other.buffer[@index]
                        return false
                }

                return true
            }

            /// Visit all the booleans of the [BitArray]
            #[swag.macro]
            func(ptr: bool) opVisit(using self, stmt: code)
            {
                if !count return
                #assert(ptr == false, "pointer visit not supported for BitArray")

                remainingBits := count
                countBits := 0'u32
                loop idx: math.max(count >> 5, 1)
                {
                    var shift: u32 = 1
                    loop math.min(remainingBits, 32)
                    {
                        #macro
                        {
                            let @alias0 = cast(bool) (buffer[`idx] & `shift)
                            let @alias1 = `countBits
                            #mixin `stmt
                        }   

                        countBits += 1
                        shift <<= 1
                    }
                }
            }
        }

        func free(using self)
        {
            if !buffer
                return            
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
            /// Reserve the given amount of bits
            func reserve(using self, maxBits: u32)
            {
                if !maxBits
                {
                    free(self)
                    buffer = null
                    count, capacity = 0
                    return
                }

                count = maxBits
                capacity = maxBits >> 5
                if capacity << 5 < maxBits
                    capacity += 1

                capacity *= 4
                realloc(self, capacity)
            }

            /// Reserve the given amount of bits and set an initial value to all bits
            func reserve(using self, maxBits: u32, initialValue: bool)
            {
                reserve(self, maxBits)
                memory.set(buffer, initialValue ? 0xFF : 0x00, capacity)
            }   

            /// Sets all bits in the [BitArray] to the specified value
            func setAll(using self, value: bool)    
            {
                memory.set(buffer, value ? 0xFF : 0x00, capacity)
            }

            /// Sets the bit at a specific position in the [BitArray] to the specified value
            func set(using self, index: u32, value: bool)
            {
                assert(index < count)
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                if value
                    buffer[val] |= shift
                else
                    buffer[val] &= ~shift
            }

            /// Gets the value of the bit at a specific position in the [BitArray]
            func get(using self, index: u32)->bool
            {
                assert(index < count)
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                return cast(bool) (buffer[val] & shift)
            }

            /// Inverts the value of the bit at a specific position in the [BitArray]
            func invert(using self, index: u32)
            {
                assert(index < count)
                val := index >> 5
                bit := index - (val << 5)
                let shift = 1 << bit
                if buffer[val] & shift
                    buffer[val] ^= shift
                else
                    buffer[val] |= shift
            }

            /// Performs the bitwise AND operation between the elements of the current [BitArray] and the corresponding elements in the specified array. 
            /// The current [BitArray] will be modified to store the result of the bitwise AND operation.
            func and(using self, other: Self)
            {
                diagnostics.debug.assert(count == other.count)
                if !count return
                loop math.max(count >> 5, 1)
                    buffer[@index] &= other.buffer[@index]
            }

            /// Performs the bitwise OR operation between the elements of the current [BitArray] and the corresponding elements in the specified array. 
            /// The current [BitArray] will be modified to store the result of the bitwise OR operation.
            func or(using self, other: Self)
            {
                diagnostics.debug.assert(count == other.count)
                if !count return
                loop math.max(count >> 5, 1)
                    buffer[@index] |= other.buffer[@index]
            }            

            /// Performs the bitwise XOR operation between the elements of the current [BitArray] and the corresponding elements in the specified array. 
            /// The current [BitArray] will be modified to store the result of the bitwise XOR operation.
            func xor(using self, other: Self)
            {
                diagnostics.debug.assert(count == other.count)
                if !count return
                loop math.max(count >> 5, 1)
                    buffer[@index] ^= other.buffer[@index]
            }            

            /// Inverts all the bit values in the current [BitArray], so that elements set to `true` are changed to `false`, and elements set to `false` are changed to `true`.
            func not(using self)
            {
                if !count return
                loop math.max(count >> 5, 1)
                    buffer[@index] = ~buffer[@index]
            }              
        }
    }
}