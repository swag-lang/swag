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
        if result < value do
            result += toAlign
        return result
    }

    // Free a block, if possible.
    mtd free(addr: *void, size: u64)
    {
        @assert(.allocator != null)
        var req: AllocatorRequest
        req.mode    = .Free
        req.address = addr
        req.size    = size
        .allocator.req(&req)
    }

    // Allocate a block
    mtd alloc(size: u64, alignment: u32 = 0)->*void
    {
        @assert(.allocator != null)
        var req: AllocatorRequest
        req.mode      = .Alloc
        req.size      = size
        req.alignment = alignment
        .allocator.req(&req)
        return req.address
    }

    // Release the allocated block
    mtd release()
    {
        .freeAll()

        if .block
        {
            .free(.block, .capacity)
            .block = null
        }
    }

    // Release all memory
    mtd freeAll()
    {
        .used = 0
        while .firstLeak
        {
            let p = cast(*LeakHeader) .firstLeak
            let n = p.next
            .free(p, p.size)
            .firstLeak = n
        }
    }

    // This will call 'release' prior to changing the capacity, so this must
    // be called at an early stage
    mtd setCapacity(newCapacity: u64, allocator: IAllocator)
    {
        @assert(allocator != null)
        .release()
        .allocator = allocator
        .capacity  = newCapacity

        var req: AllocatorRequest
        req.mode = AllocatorMode.Alloc
        req.size = .capacity
        .allocator.req(&req)
        .block = req.address
    }
}

impl IAllocator for ScratchAllocator
{
    mtd free(request: *AllocatorRequest)
    {
        if !request.address do
            return

        // If this is the last allocated block in the buffer, we can free it
        // by changing the allocated used size
        let ptr = (.block + .used) - request.size
        if ptr == request.address
        {
            request.address = null
            .used -= request.size
            return
        }

        // If the pointer is in the buffer, then we are done. We cannot free.
        if request.address >= .block and request.address < .block + .capacity
        {
            request.address = null
            return
        }

        // Otherwise we assume it's a leak. We unlink it, and free it with
        // the allocator
        var leak = cast([*] LeakHeader) request.address
        leak -= 1
        if !leak.prev do
            .firstLeak = leak.next
        else do
            leak.prev.next = leak.next
        if leak.next do
            leak.next.prev = leak.prev
        .totalLeak -= leak.size - #sizeof(LeakHeader)
        .free(leak, leak.size)
        request.address = null
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc do
                .free(request)
            request.address = null
            return
        }

        // Allocate in the buffer if there is enough room
        let usedAlign = align(.used, request.alignment)
        if usedAlign + request.size <= .capacity
        {
            let oldAddress = request.address
            request.address = .block + usedAlign
            .used           = usedAlign + request.size
            .maxUsed        = @max(.maxUsed, .used)
            if oldAddress do
                @memcpy(request.address, oldAddress, request.oldSize)
            return
        }

        // Otherwise this is a leak
        // We prepend a LeakHeader to the user block, and linked it in
        // the chained list of leak allocations (at the first position)
        let totalSize = request.size + #sizeof(LeakHeader)
        let ptr       = cast([*] LeakHeader) .alloc(totalSize, request.alignment)
        let leak      = cast([*] LeakHeader) .firstLeak
        if leak do
            leak.prev = ptr
        ptr.prev        = null
        ptr.next        = .firstLeak
        .firstLeak      = ptr
        ptr.size        = totalSize
        request.address = ptr + 1 // user block is after the header
        .totalLeak += request.size
        .maxLeak = @max(.maxLeak, .totalLeak)
    }

    mtd impl req(request: *AllocatorRequest)
    {
        @assert(.block != null)
        switch request.mode
        {
        case Free:
            .free(request)

        case FreeAll:
            .freeAll()

        case Realloc, Alloc:
            .alloc(request)
        }
    }
}
