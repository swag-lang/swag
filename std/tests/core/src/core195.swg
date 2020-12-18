#import "core"
using core

var g_dbg: memory.DebugAllocator
var g_cxt: swag.Context

#init
{
    g_cxt = :@getcontext()
    g_dbg.allocator = @getcontext().allocator
    g_cxt.allocator = g_dbg
    @setcontext(g_cxt)
}

#drop
{
    if !g_dbg.isEmpty()
    {
        @print("memory leaks !\n")
        g_dbg.printAllocated()
        @assert(false)
    }
}

#test
{
    //#assert reflection.hasDrop(std.file.FileInfo)

    var dbg: memory.DebugAllocator
    dbg.allocator = @getcontext().allocator

    ptr := memory.alloc(16, dbg)
    ptr1 := memory.new'String(dbg)

    //dbg.printAllocated()

    memory.free(ptr, 16, dbg)
    memory.delete'String(ptr1, dbg)
    @assert(dbg.isEmpty())
}