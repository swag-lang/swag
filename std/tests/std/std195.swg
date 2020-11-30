#import "std"
using std

#test
{
    var dbg: memory.DebugAllocator
    dbg.allocator = @getcontext().allocator

    ptr := memory.alloc(10, dbg)
    ptr1 := memory.new'String(dbg)

    dbg.printAllocated()

    memory.free(ptr, dbg)
    memory.free(ptr1, dbg)
    @assert(dbg.isEmpty())
}