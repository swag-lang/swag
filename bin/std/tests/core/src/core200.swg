/*using Core.Memory, Swag

#test
{
    mem := ScratchAllocator.create(16'u64)
    @assert(!@errmsg)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := Memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}*/