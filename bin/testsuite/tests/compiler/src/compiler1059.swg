using Swag

struct(T) Array
{
    buffer:     ^T
    count:      u32
    size:       u32
}

impl Array
{
    mtd opDrop()
    {
        var request: AllocatorRequest
        request.mode    = AllocatorMode.Free
        request.address = me.buffer
        request.size    = cast(u64) me.size * #sizeof(T)
        @getcontext().allocator.req(&request)
    }

    func validateCount(using me, newCount: u32)
    {
        if newCount < size:
            return

        size = size * 2
        if size < 8:
            size = 8

        let context = @getcontext()

        var request: AllocatorRequest
        request.size = cast(u64) size * #sizeof(T)
        context.allocator.req(&request)
        let newBuffer = request.address

        if buffer:
            @memcpy(newBuffer, buffer, cast(u64) count * #sizeof(T))

        request.mode    = AllocatorMode.Free
        request.address = buffer
        context.allocator.req(&request)

        buffer = cast() newBuffer
    }

    func add(using me, value: T)
    {
        validateCount(me, count + 1)
        buffer[count] = value
        count += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var arr: Array's32
    arr.add(666)
    @assert(arr.buffer[0] == 666)
}
