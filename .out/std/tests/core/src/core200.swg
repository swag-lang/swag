
using core.memory, swag

#test
{
    mem := catch ScratchAllocator.create(16'uint)
    @assert(!@err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        ptr := memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}