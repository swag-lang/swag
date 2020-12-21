#import "core"
using core.memory, swag

#test
{
    var mem: ScratchAllocator
    mem.create(16*1024'uint)
    defer mem.release()

    system.pushAllocator(cast(IAllocator) mem)
    {
        memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}