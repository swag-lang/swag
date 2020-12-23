#import "core"
using core.memory, swag

#test
{
    (mem, err) := ScratchAllocator.create(16'uint)
    @assert(!err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}