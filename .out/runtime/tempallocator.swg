#global namespace swag

impl TempAllocator
{
    // Release the allocated block
    func release(using self)
    {
        freeAll()
        @free(block)
        block = null
    }

    // Restore all memory
    func freeAll(using self)
    {
        used = 0
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
        switch request.mode
        {
        case Free:
            ptr := block + used - request.size
            if ptr == request.address
            {
                used = lastUsed
                lastUsed = 0
            }
            break

        case FreeAll:
            freeAll();

        case Alloc:
            request.address = null
            fallthrough

        case Realloc:

            // First block allocation
            if !block
            {
                if !capacity capacity = 16 * 1024
                block = @alloc(capacity)
            }

            // Deal with alignement
            toAlign := request.alignement ?? @sizeof(*void)
            usedAlign := used & ~(toAlign - 1)
            if usedAlign < used usedAlign += toAlign

            if usedAlign + request.size <= capacity
            {
                oldAddress := request.address
                request.address = block + usedAlign
                lastUsed = used
                used = usedAlign + request.size
                if used > maxUsed
                    maxUsed = used
                if oldAddress
                    @memcpy(request.address, oldAddress, request.oldSize)
            }
            else
            {
                @errormsg("not enough memory in the temporary allocator", #location)
                @assert(false)
            }
        }
    }
}