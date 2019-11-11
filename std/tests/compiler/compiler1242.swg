/*
using swag
namespace std
{    
    const SmallSize = 40'u32
    const SmallSizeInternal = SmallSize - @sizeof(Utf8.size) - @sizeof(Utf8.allocator)

    struct Utf8
    {
        buffer:     *u8
        length:     u32
        size:       u32
        allocator:  Allocator
        padding:    [SmallSizeInternal] u8
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
        
        func realloc(using self, newSize: u32)
        {           
            if !allocator
                allocator = @context().allocator
                
            var req: AllocatorRequest
            req.mode = AllocatorMode.Realloc
            req.size = newSize
            req.address = buffer
            allocator(req)
            buffer = acast req.address
        }

        func reserve(using self, newSize: u32)
        {
            // Small size, just be sure with have a valid buffer address
            if newSize <= SmallSize
            {
                if !buffer
                    buffer = acast &size
                return
            }

            // Actual size is fine, don't change anything
            if newSize <= size 
                return
                
            realloc(self, newSize)
            size = newSize
        }
    }
}
*/