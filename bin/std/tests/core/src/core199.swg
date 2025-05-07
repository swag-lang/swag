/*using Core.Memory, Swag

#test
{
    var mem = ScratchAllocator.create(16*1024'u64)
    @assert(@err == null)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        var ptr = Memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}*/
