#import "core"
using core.memory, swag

#test
{
    (mem, err) := ScratchAllocator.create(16*1024'uint)
    @assert(!err)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}