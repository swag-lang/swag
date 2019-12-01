namespace std.memory
{
    public 
    {
        func copy(dest, src: *void, size: u32)
        {
            @memcpy(dest, src, size)
        }
        
        func compare(dest, src: *void, size: u32)->s32
        {
            //@memcmp(dest, src, cast(u64) size)
            return 0
        }        
        
        func clear(dest: *void, size: u32)
        {
            //@memset(dest, 0, cast(u64) size)
        } 
    }
}