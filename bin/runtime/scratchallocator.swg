#global namespace Swag

impl ScratchAllocator
{
    struct LeakHeader
    {
        prev:     *LeakHeader
        next:     *LeakHeader
        size:     u64
    }

    // Adapt the value depending on the requested alignment
    func align(value: u64, alignment: u32)->u64
    {
        let toAlign = alignment orelse #sizeof(*void)
        var result  = value & ~(toAlign - 1)
        if result < value:
            result += toAlign
        return result
    }

    // Free a block, if possible.
    mtd free(addr: *void, size: u64)
    {
        @assert(me.allocator != null)
        var req: AllocatorRequest
        req.mode    = .Free
        req.address = addr
        req.size    = size
        me.allocator.req(&req)
    }

    // Allocate a block
    mtd alloc(size: u64, alignment: u32 = 0)->*void
    {
        @assert(me.allocator != null)
        var req: AllocatorRequest
        req.mode      = .Alloc
        req.size      = size
        req.alignment = alignment
        me.allocator.req(&req)
        return req.address
    }

    // Release the allocated block
    mtd release()
    {
        me.freeAll()

        if me.block
        {
            me.free(me.block, me.capacity)
            me.block = null
        }
    }

    // Release all memory
    mtd freeAll()
    {
        me.used = 0
        while me.firstLeak
        {
            let p = cast(*LeakHeader) me.firstLeak
            let n = p.next
            me.free(p, p.size)
            me.firstLeak = n
        }
    }

    // This will call 'release' prior to changing the capacity, so this must
    // be called at an early stage
    mtd setCapacity(newCapacity: u64, allocator: IAllocator)
    {
        @assert(#me.allocator != null)
        me.release()
        me.allocator = #me.allocator
        me.capacity  = newCapacity

        var req: AllocatorRequest
        req.mode = AllocatorMode.Alloc
        req.size = me.capacity
        me.allocator.req(&req)
        me.block = req.address
    }
}

impl IAllocator for ScratchAllocator
{
    mtd free(request: *AllocatorRequest)
    {
        if !request.address:
            return

        // If this is the last allocated block in the buffer, we can free it
        // by changing the allocated used size
        let ptr = (me.block + me.used) - request.size
        if ptr == request.address
        {
            request.address = null
            me.used -= request.size
            return
        }

        // If the pointer is in the buffer, then we are done. We cannot free.
        if request.address >= me.block and request.address < me.block + me.capacity
        {
            request.address = null
            return
        }

        // Otherwise we assume it's a leak. We unlink it, and free it with
        // the allocator
        var leak = cast(^LeakHeader) request.address
        leak -= 1
        if !leak.prev:
            me.firstLeak = leak.next
        else:
            leak.prev.next = leak.next
        if leak.next:
            leak.next.prev = leak.prev
        me.totalLeak -= leak.size - #sizeof(LeakHeader)
        me.free(leak, leak.size)
        request.address = null
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc:
                me.free(request)
            request.address = null
            return
        }

        // Allocate in the buffer if there is enough room
        let usedAlign = align(me.used, request.alignment)
        if usedAlign + request.size <= me.capacity
        {
            let oldAddress = request.address
            request.address = me.block + usedAlign
            me.used       = usedAlign + request.size
            me.maxUsed    = @max(me.maxUsed, me.used)
            if oldAddress:
                @memcpy(request.address, oldAddress, request.oldSize)
            return
        }

        // Otherwise this is a leak
        // We prepend a LeakHeader to the user block, and linked it in
        // the chained list of leak allocations (at the first position)
        let totalSize = request.size + #sizeof(LeakHeader)
        let ptr       = cast(^LeakHeader) me.alloc(totalSize, request.alignment)
        let leak      = cast(^LeakHeader) me.firstLeak
        if leak:
            leak.prev = ptr
        ptr.prev        = null
        ptr.next        = me.firstLeak
        me.firstLeak  = ptr
        ptr.size        = totalSize
        request.address = ptr + 1 // user block is after the header
        me.totalLeak += request.size
        me.maxLeak = @max(me.maxLeak, me.totalLeak)
    }

    mtd impl req(request: *AllocatorRequest)
    {
        @assert(me.block != null)
        switch request.mode
        {
        case Free:
            me.free(request)

        case FreeAll:
            me.freeAll()

        case Realloc, Alloc:
            me.alloc(request)
        }
    }
}
