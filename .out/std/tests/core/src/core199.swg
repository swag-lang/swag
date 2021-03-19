
using core.memory, swag

#test
{
    mem := ScratchAllocator.create(16*1024'uint)
    @assert(!@err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}