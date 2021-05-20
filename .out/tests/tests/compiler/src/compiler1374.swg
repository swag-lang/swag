using Swag
private const SmallSize = 28'u32
private const SmallSizeInternal = SmallSize - (@sizeof(IAllocator) + @sizeof(u32))
#assert @sizeof(String) == 40

struct String
{
    buffer:     *u8
    length:     u32
    size:       u32
    allocator:  IAllocator
    padding:    [SmallSizeInternal] u8
}