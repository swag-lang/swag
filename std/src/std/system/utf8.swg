using swag
namespace std
{    
    private const SmallSize = 40 + 4

    struct Utf8
    {
        buffer:     *u8
        length:     u32
        size:       u32
        allocator:  Allocator
        padding:    [SmallSize] u8
    }

    impl Utf8
    {
        func free(using self)
        {
            if !allocator
                return
            var req: AllocatorRequest
            req.mode = AllocatorMode.Free
            req.address = buffer
            allocator(req)
            buffer = null
        }
        
        func alloc(using self, newSize: u32)
        {
            free(self)
            
            if !allocator
                allocator = @context().allocator
            var req: AllocatorRequest
            req.size = newSize
            allocator(req)
            buffer = acast req.address
        }
        


        func reserve(using self, newSize: u32)
        {
            if size >= newSize
                return
        }
    }
}
