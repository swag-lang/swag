public namespace memory;
using swag;

struct ScratchAllocator
{
    fallback:   IAllocator
    block:      *u8
    capacity:   uint
    used:       uint    // Currently allocated size in 'blocks'
    leaks:      Array'{ptr: *void, size: uint}  // List of allocated blocks which do not feat in 'blocks'
    usedLeaks:  uint    // Currently total allocated size in 'leaks'
}

impl ScratchAllocator
{
    func opDrop(using self)
    {
        freeAll()
        memory.free(block, capacity, fallback)
    }

    // Free all allocated blocks
    func freeAll(using self)
    {
        used, usedLeaks = 0
        visit p: leaks
        {
            var newReq: AllocatorRequest
            newReq.mode = AllocatorMode.Free
            newReq.address  = p.ptr
            newReq.size = p.size
            fallback.alloc(&newReq)
        }

        leaks.clear()
    }

    // First create allocator by allocation a block of memory with
    // the current context allocator
    func createWithContext(using self, blockSize: uint, allocator: IAllocator = null)
    {
        debug.assert(blockSize > 0)

        // Allocate the big block to store all allocations
        fallback = allocator ?? @getcontext().allocator
        capacity = blockSize
        block = memory.alloc(blockSize, fallback)

        // Dynamic array allocator
        leaks.allocator = fallback
    }
}

impl IAllocator for ScratchAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        debug.assert(fallback != null, "missing fallback allocator")
        debug.assert(block != null, "ScratchAllocator should be initialized first")

        switch request.mode
        {
        case AllocatorMode.Free:
            break

        case AllocatorMode.FreeAll:
            freeAll();

        case AllocatorMode.Alloc:
            request.address = null
            fallthrough

        case AllocatorMode.Realloc:
            oldAddress := request.address

            usedAlign := memory.align(used, request.alignement)
            if usedAlign + request.size <= capacity
            {
                request.address = block + usedAlign
                used = usedAlign + request.size
            }

            // Not enough room. Use fallback allocator
            else
            {
                fallback.alloc(request)
                leaks.add(@{request.address, request.size})
                usedLeaks += request.size
            }

            // Copy previous block in case of a reallocation
            if oldAddress
                memory.copy(request.address, oldAddress, request.oldSize)
        }
    }
}