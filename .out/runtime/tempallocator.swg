#global namespace Swag

private struct LeakHeader
{
    prev:   *LeakHeader
    next:   *LeakHeader
    size:   uint
}

impl TempAllocator
{
    private func align(value: uint, alignement: u32)->uint
    {
        toAlign := alignement orelse @sizeof(*void)
        result := value & ~(toAlign - 1)
        if result < value result += toAlign
        return result
    }

    private func free(using self, addr: *void, size: uint)
    {
        var req: AllocatorRequest
        req.mode = .Free
        req.address = addr
        req.size = size
        allocator.alloc(&req)
    }

    private func alloc(using self, size: uint, alignement: u32 = 0)->*void
    {
        var req: AllocatorRequest
        req.mode = .Alloc
        req.size = size
        req.alignement = alignement
        allocator.alloc(&req)
        return req.address
    }

    // Release the allocated block
    func release(using self)
    {
        freeAll()
        free(block, capacity)
        block = null
    }

    // Restore all memory
    func freeAll(using self)
    {
        used = 0

        while firstLeak
        {
            p := cast(*LeakHeader) firstLeak
            n := p.next
            free(p, p.size)
            firstLeak = n
        }
    }

    // This will call 'release' prior to changing the capacity, so this must
    // be called at an early stage
    func setCapacity(using self, newCapacity: uint)
    {
        if capacity == newCapacity
            return
        release()
        capacity = newCapacity
    }
}

impl IAllocator for TempAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        // First block allocation
        if !block
        {
            if !capacity            capacity = 4 * 1024 * 1024
            if allocator == null    allocator = @getcontext().allocator
            block = alloc(capacity)
        }

        switch request.mode
        {
        case Free:
            // If this is the last allocated block in the buffer, we can free it
            // by changing the allocated used size
            ptr := (block + used) - request.size
            if ptr == request.address
            {
                used = lastUsed
                lastUsed = 0
                break
            }

            // If the pointer is in the buffer, then we are done. We cannot free.
            if request.address >= block and request.address < block + capacity
                break

            // Otherwise we assume it's a leak. We unlink it, and free it with
            // the allocator
            leak := cast(*LeakHeader) request.address
            leak -= 1
            if !leak.prev
                firstLeak = leak.next
            else
                leak.prev.next = leak.next
            if leak.next
                leak.next.prev = leak.prev
            free(leak, leak.size)
            totalLeak -= leak.size - @sizeof(LeakHeader)
            break

        case FreeAll:
            freeAll();

        case Alloc:
            request.address = null
            fallthrough

        case Realloc:
            // Allocate in the buffer if there is enough room
            usedAlign := align(used, request.alignement)
            if usedAlign + request.size <= capacity
            {
                oldAddress := request.address
                request.address = block + usedAlign
                lastUsed = used
                used = usedAlign + request.size
                maxUsed = @max(maxUsed, used)
                if oldAddress
                    @memcpy(request.address, oldAddress, request.oldSize)
                break
            }

            // Otherwise this is a leak
            // We prepend a LeakHeader to the user block, and linked it in
            // the chained list of leak allocations (at the first position)
            totalSize := request.size + @sizeof(LeakHeader)
            ptr  := cast(*LeakHeader) alloc(totalSize, request.alignement)
            leak := cast(*LeakHeader) firstLeak
            if leak leak.prev = ptr
            ptr.prev = null
            ptr.next = firstLeak
            firstLeak = ptr
            ptr.size = totalSize
            request.address = ptr + 1 // user block is after the header
            totalLeak += request.size
            maxLeak = @max(maxLeak, totalLeak)
        }
    }
}