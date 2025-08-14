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
    mtd freePtr(addr: *void, size: u64)
    {
        @assert(self.allocator != null)
        var req: AllocatorRequest
        req.mode    = .Free
        req.address = addr
        req.size    = size
        self.allocator.req(&req)
    }

    // Allocate a block
    mtd allocPtr(size: u64, alignment: u32 = 0)->*void
    {
        @assert(self.allocator != null)
        var req: AllocatorRequest
        req.mode      = .Alloc
        req.size      = size
        req.alignment = alignment
        self.allocator.req(&req)
        return req.address
    }

    // Release the allocated block
    mtd release()
    {
        self.freeAll()

        if self.block
        {
            self.freePtr(self.block, self.capacity)
            self.block = null
        }
    }

    // Release all memory
    mtd freeAll()
    {
        self.used = 0
        while self.firstLeak
        {
            let p = cast(*LeakHeader) self.firstLeak
            let n = p.next
            self.freePtr(p, p.size)
            self.firstLeak = n
        }
    }

    // This will call 'release' prior to changing the capacity, so this must
    // be called at an early stage
    mtd setCapacity(newCapacity: u64, allocator: IAllocator)
    {
        @assert(#self.allocator != null)
        self.release()
        self.allocator = #self.allocator
        self.capacity       = newCapacity

        var req: AllocatorRequest
        req.mode = AllocatorMode.Alloc
        req.size = self.capacity
        self.allocator.req(&req)
        self.block = req.address
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
        let ptr = (self.block + self.used) - request.size
        if ptr == request.address
        {
            request.address = null
            self.used -= request.size
            return
        }

        // If the pointer is in the buffer, then we are done. We cannot free.
        if request.address >= self.block and request.address < self.block + self.capacity
        {
            request.address = null
            return
        }

        // Otherwise we assume it's a leak. We unlink it, and free it with
        // the allocator
        var leak = cast(^LeakHeader) request.address
        leak -= 1
        if !leak.prev:
            self.firstLeak = leak.next
        else:
            leak.prev.next = leak.next
        if leak.next:
            leak.next.prev = leak.prev
        self.totalLeak -= leak.size - #sizeof(LeakHeader)
        self.freePtr(leak, leak.size)
        request.address = null
    }

    mtd alloc(request: *AllocatorRequest)
    {
        // Special case
        if request.size == 0
        {
            if request.mode == .Realloc:
                self.free(request)
            request.address = null
            return
        }

        // Allocate in the buffer if there is enough room
        let usedAlign = align(self.used, request.alignment)
        if usedAlign + request.size <= self.capacity
        {
            let oldAddress = request.address
            request.address = self.block + usedAlign
            self.used       = usedAlign + request.size
            self.maxUsed    = @max(self.maxUsed, self.used)
            if oldAddress:
                @memcpy(request.address, oldAddress, request.oldSize)
            return
        }

        // Otherwise this is a leak
        // We prepend a LeakHeader to the user block, and linked it in
        // the chained list of leak allocations (at the first position)
        let totalSize = request.size + #sizeof(LeakHeader)
        let ptr       = cast(^LeakHeader) self.allocPtr(totalSize, request.alignment)
        let leak      = cast(^LeakHeader) self.firstLeak
        if leak:
            leak.prev = ptr
        ptr.prev        = null
        ptr.next        = self.firstLeak
        self.firstLeak       = ptr
        ptr.size        = totalSize
        request.address = ptr + 1 // user block is after the header
        self.totalLeak += request.size
        self.maxLeak = @max(self.maxLeak, self.totalLeak)
    }

    mtd impl req(request: *AllocatorRequest)
    {
        @assert(self.block != null)
        switch request.mode
        {
        case Free:
            self.free(request)

        case FreeAll:
            self.freeAll()

        case Realloc, Alloc:
            self.alloc(request)
        }
    }
}
