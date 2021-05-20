using core, Memory

var g_dbg: DebugAllocator
var g_cxt: Swag.Context

protected func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

protected func filePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

#init
{
    try Jobs.setNumWorkers(4)

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