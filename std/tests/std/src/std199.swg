#import "std"
using std.memory, swag

#test
{
    var mem: ScratchAllocator
    mem.createWithContext(16*1024'uint)

    system.pushAllocator(cast(IAllocator) mem)
    {
        memory.alloc(100)
    }

    @assert(mem.used == 100)
    mem.freeAll()
    @assert(mem.used == 0)
}