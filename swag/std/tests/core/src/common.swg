#import "core"
using core, memory

var g_dbg: DebugAllocator
var g_cxt: swag.Context

#init
{
    try jobs.setNumWorkers(4)

    try g_dbg.init()
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