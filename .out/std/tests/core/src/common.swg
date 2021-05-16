using core, memory

var g_dbg: DebugAllocator
var g_cxt: swag.Context

protected func dataPath()->String
{
    dataPath := path.getDirectoryName(#location.fileName)
    dataPath = path.getDirectoryName(dataPath)
    return path.combine(dataPath, "datas")
}

protected func filePath(name: string)->String
{
    return path.combine(dataPath(), name)
}

#init
{
    try jobs.setNumWorkers(4)

    g_dbg.init()
    g_cxt = dref @getcontext()
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