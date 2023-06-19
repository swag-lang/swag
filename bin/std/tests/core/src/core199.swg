/*using Core.Memory, Swag

#test
{
    mem := ScratchAllocator.create(16*1024'u64)
    @assert(!@errmsg)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := Memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}*/