namespace std
{
    using swag
    public struct BitArray
    {
        readonly buffer:        *u8
        internal allocator:     IAllocator
        readonly maxBits:       u32
    }

    impl BitArray
    {
        public
        {
            func opCount(using self)->u32
            {
                return maxBits;
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
            func reserve(using self, numBits: u32)
            {
                if !numBits
                {
                    free(self)
                    return
                }

                numBytes := numBits / 8
                if numBytes * 8 < maxBits
                    numBytes += 1

                realloc(self, numBytes)
            }

            /// Reserve the given amount of bits
            func reserve(using self, numBits: u32, initialValue: bool)
            {
                reserve(self, numBits)
                //memory.set(buffer, initialValue ? 0xFF : 0x00)
            }
        }
    }
}