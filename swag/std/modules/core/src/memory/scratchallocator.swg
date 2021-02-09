#global public
#global namespace memory
using swag, sync

struct ScratchAllocator
{
    fallback:   IAllocator
    block:      *u8
    capacity:   uint
    used:       uint    // Currently allocated size in 'blocks'
    leaks:      Array'{ptr: *void, size: uint}  // List of allocated blocks which do not feat in 'blocks'
    usedLeaks:  uint    // Currently total allocated size in 'leaks'
    mutex:      Mutex

}

impl ScratchAllocator
{
    func opDrop(using self)
    {
        debug.assert(block == null, "missing call to release()")
    }

    // Creates a new allocator
    func create(blockSize: uint, allocator: IAllocator = null)->ScratchAllocator
    {
        var result: retval
        result.init(blockSize, allocator)
        return result
    }

    // First initialize allocator by allocating a block of memory with
    // the specified allocator.
    // If 'allocator' is null, the current context allocator will be used
    func init(using self, blockSize: uint, allocator: IAllocator = null)
    {
        // Mutex
        mutex.init()
        if @err return

        // Allocate the big block to store all allocations
        fallback = allocator ?? @getcontext().allocator
        debug.assert(blockSize > 0)
        capacity = blockSize
        block = memory.alloc(blockSize, fallback)

        // Dynamic array allocator
        leaks.allocator = fallback
    }

    // Release all allocated memory
    func release(using self)
    {
        freeAll()
        memory.free(block, capacity, fallback)
        block = null
        mutex.release()
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
}

impl IAllocator for ScratchAllocator
{
    func alloc(using self, request: *AllocatorRequest)
    {
        debug.assert(fallback != null, "not created")
        debug.assert(block != null, "not created")

        mutex.lock()
        defer mutex.unlock()

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