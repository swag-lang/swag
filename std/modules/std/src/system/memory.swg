namespace std.memory
{
    public 
    {
        func copy(dest, src: *void, size: u32)
        {
            os.memcpy(dest, src, size)
        }
        
        func compare(dest, src: *void, size: u32)->s32
        {
            return os.memcmp(dest, src, cast(u64) size)
        }        
        
        func clear(dest: *void, size: u32)
        {
            return os.memset(dest, 0, cast(u64) size)
        } 
    }
}