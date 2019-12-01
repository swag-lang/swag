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
            return @memcmp(dest, src, size)
        }        
        
        func clear(dest: *void, size: u32)
        {
            @memset(dest, 0, size)
        } 
    }
}