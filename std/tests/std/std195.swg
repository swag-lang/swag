#import "std"
using std

var g_dbg: memory.DebugAllocator
var g_cxt: swag.Context

#init
{
    /*g_cxt = :@getcontext()
    g_dbg.allocator = @getcontext().allocator
    g_cxt.allocator = g_dbg
    @setcontext(g_cxt)*/
}

#drop
{
    g_dbg.printAllocated()
}

#test
{
    var dbg: memory.DebugAllocator
    dbg.allocator = @getcontext().allocator

    ptr := memory.alloc(16, dbg)
    ptr1 := memory.new'String(dbg)

    //dbg.printAllocated()

    memory.free(ptr, dbg)
    memory.free(ptr1, dbg)
    @assert(dbg.isEmpty())
}