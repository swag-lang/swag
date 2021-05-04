using swag
namespace std
{
    private const SmallSize = 28'u32
    private const SmallSizeInternal = SmallSize - (@sizeof(IAllocator) + @sizeof(u32))
    #assert @sizeof(String1372) == 40

    struct String1372
    {
        buffer:     *u8
        length:     u32
        size:       u32
        allocator:  IAllocator
        padding:    [SmallSizeInternal] u8
    }

    impl String1372
    {
        func free(using self)
        {
            if !buffer
                return
            var req: AllocatorRequest
            req.mode = AllocatorMode.Free
            req.address = buffer
            allocator.alloc(&req)
        }

        func realloc(using self, newSize: u32)
        {
            if allocator == null
                allocator = @getcontext().allocator

            var req: AllocatorRequest
            req.mode = AllocatorMode.Realloc
            req.size = cast(uint) newSize
            req.address = buffer
            allocator.alloc(&req)
            buffer = acast req.address
        }
    }
}
