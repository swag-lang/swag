namespace std.memory
{
    using swag, diagnostics.debug
    public
    {
        /// Copy one block of memory to a given address
        func copy(dest, src: *void, size: u32)
        {
            if size @memcpy(dest, src, size)
        }

        /// Compare two blocks of memory
        func compare(dest, src: *void, size: u32)->s32
        {
            if !size return 0
            return @memcmp(dest, src, size)
        }

        /// Clear one block of memory
        func clear(dest: *void, size: u32)
        {
            if size @memset(dest, 0, size)
        }

        /// Set the content of one block of memory to a specific byte value
        func set(dest: *void, value: u8, size: u32)
        {
            if size @memset(dest, value, size)
        }  

        /// Reallocate/allocate a given amount of bytes
        func realloc(allocator: IAllocator, buffer: *void, newSize: u32)->*void
        {
            assert(allocator != null)
            var req: AllocatorRequest
            req.mode = AllocatorMode.Realloc
            req.size = newSize
            req.address = buffer
            allocator.alloc(req)
            return req.address
        }                       
    }
}
