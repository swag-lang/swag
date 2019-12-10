namespace std.memory
{
    public
    {
        /// Copy one block of memory to a given address
        func copy(dest, src: *void, size: u32)
        {
            @memcpy(dest, src, size)
        }

        /// Compare two blocks of memory
        func compare(dest, src: *void, size: u32)->s32
        {
            return @memcmp(dest, src, size)
        }

        /// Clear one block of memory
        func clear(dest: *void, size: u32)
        {
            @memset(dest, 0, size)
        }
    }
}
