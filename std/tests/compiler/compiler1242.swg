const SmallSize = 28'u32
const SmallSizeInternal = SmallSize - @sizeof(*void) - @sizeof(u32)

struct Utf8
{
    buffer:     *u8
    length:     u32
    size:       u32
    allocator:  *u8
    padding:    [SmallSizeInternal] u8
}

#test
{
    @assert(@sizeof(*void) == 8)
    @assert(@sizeof(u32) == 4)
    @assert(SmallSize - @sizeof(*void) == 20)
    @assert(2 - 1 - 1 == 0)
    
    
    @assert(SmallSizeInternal == 16)
}