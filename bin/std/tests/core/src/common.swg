using Core, Memory

var g_dbg: DebugAllocator
var g_cxt: Swag.Context

private func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

private func filePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

#premain
{
    try Jobs.setNumWorkers(4)

    g_dbg.init()
    g_cxt = dref @getcontext()
    g_cxt.allocator = g_dbg
    @setcontext(g_cxt)
}

#drop
{
    @getcontext().tempAllocator.release()
    if !g_dbg.isEmpty()
    {
        @print("memory leaks !\n")
        g_dbg.printAllocated()
        @assert(false)
    }
}