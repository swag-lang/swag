namespace std
{
    using swag
    private const SmallSize = 28'u32
    private const SmallSizeInternal = SmallSize - (@sizeof(Allocator) + @sizeof(u32))

    public struct Utf8
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
            if !buffer
                return
            var req: AllocatorRequest
            req.mode = AllocatorMode.Free
            req.address = buffer
            allocator(req)
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

        public func opAffect(using self, value: string)
        {
            if value == null
            {
                reserve(self, 0)
                return
            }
            
            newSize := @countof(value) + 1
            length = newSize - 1
            reserve(self, newSize)
            @memcpy(buffer, @dataof(value), newSize)
        }
        
        public func reserve(using self, newSize: u32)
        {
            // Small size, just be sure with have a valid buffer address
            if newSize <= SmallSize
            {
                // Free everything
                if !newSize
                {
                    if !buffer
                        return
                    if buffer != acast &size
                        free(self)
                    buffer = null
                    length = 0
                    size = 0
                    allocator = null
                    return
                }

                if !buffer
                    buffer = acast &size
                return
            }

            // Actual size is fine
            if newSize <= size
                return

            // We were in small size mode, and we are no more
            if buffer == acast &size
            {
                buffer = null
                allocator = null
            }

            realloc(self, newSize)
            size = newSize
        }
    }
}

//#[swag.printbc]
#test
{
    using std
    var x: Utf8 = "coucou"
    @assert(x.length == 6)
    x = null
    @assert(x.length == 0)
}