using Swag
namespace std
{
    const SmallSize = 28'u32
    const SmallSizeInternal = SmallSize - (#sizeof(IAllocator) + #sizeof(u32))
    #assert(#sizeof(String1372) == 40)

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
            if !buffer:
                return
            var req: AllocatorRequest
            req.mode = AllocatorMode.Free
            req.address = buffer
            allocator.req(&req)
        }

        func realloc(using self, newSize: u32)
        {
            if allocator == null:
                allocator = @getcontext().allocator

            var req: AllocatorRequest
            req.mode = AllocatorMode.Realloc
            req.size = cast(u64) newSize
            req.address = buffer
            allocator.req(&req)
            buffer = cast() req.address
        }
    }
}
