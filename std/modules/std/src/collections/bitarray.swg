namespace std
{
    using swag

    /// An array of bits
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

            /// Reserve the given amount of bits
            func reserve(using self, maxBits: u32, initialValue: bool)
            {
                reserve(self, maxBits)
                memory.set(buffer, initialValue ? 0xFF : 0x00, numBytes)
            }            
        }
    }
}