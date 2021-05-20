
using core.Memory, Swag

#test
{
    mem := ScratchAllocator.create(16'uint)
    @assert(!@err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := Memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}