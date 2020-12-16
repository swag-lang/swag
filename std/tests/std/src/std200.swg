#import "std"
using std.memory, swag

#test
{
    var mem: ScratchAllocator
    mem.createWithContext(16'uint)

    system.pushAllocator(cast(IAllocator) mem)
    {
        memory.alloc(100)
    }

    @assert(mem.usedLeaks == 100)
    mem.freeAll()
}