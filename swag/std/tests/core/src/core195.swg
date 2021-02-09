#import "core"
using core.memory

var g_dbg: DebugAllocator
var g_cxt: swag.Context

#init
{
    g_dbg.init()
    g_cxt = :@getcontext()
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

    g_dbg.release()
}

#test
{
    //#assert reflection.hasDrop(std.file.FileInfo)

    dbg := DebugAllocator.create()
    defer dbg.release()

    ptr := alloc(16, dbg)
    ptr1 := new'String(dbg)

    //dbg.printAllocated()

    free(ptr, 16, dbg)
    delete'String(ptr1, dbg)
    @assert(dbg.isEmpty())
}