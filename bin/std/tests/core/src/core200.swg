/*using Core.Memory, Swag

#test
{
    var mem = ScratchAllocator.create(16'u64)
    @assert(@err() == null)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        var ptr = Memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}*/
