
using Core.Memory, Swag

#test
{
    mem := ScratchAllocator.create(16*1024'uint)
    @assert(!@err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := Memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}