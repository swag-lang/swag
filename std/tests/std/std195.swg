#import "std"
using std

#test
{
    var dbg: memory.DebugAllocator
    dbg.allocator = @getcontext().allocator

    ptr := memory.alloc(10, dbg)
    dbg.printAllocated()
    memory.free(ptr, dbg)
    //memory.free(ptr, dbg)
}